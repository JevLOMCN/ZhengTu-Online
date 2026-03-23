/**
 * \file
 * \version	$Id: LiveSkill.cpp  $
 * \author	
 * \date	
 * \brief	实现生活技能
 * 
 */
 
#include "LiveSkill.h"
#include "SceneUser.h"
#include "ScenePk.h"
#include "zTime.h"
#include "Zebra.h"
#include "Scene.h"
#include "Chat.h"

const int LiveSkill::odds[] = {9000, 500, 10, 5, 1, 1, 0};

/**
 * \brief 消耗体力值
 *
 *
 * \return 需要消耗的体力值
 */
int LiveSkill::consume_sp()
{
	return zMisc::randBetween(MIN_NEED_SP, MAX_NEED_SP);
}

LiveSkills::LiveSkills() : working_time(0), skill_id(0)
{
	
}

LiveSkills::~LiveSkills()
{
	
}


/**
 * \brief 劳动完成后得到的奖励
 *
 *
 * \param user: 完成劳动的用户
 * \param points: 应该得到的经验点,如果是种植类技能使用默认值0 ,道具打造类技能该点来自道具
 * \return 如果存在该技能返回0,否则返回-1
 */
int LiveSkill::bonus(SceneUser* user, int points)
{
	//Channel::sendSys(user, Cmd::INFO_TYPE_GAME, "TO REMOVE:劳动完成!");	
	zLiveSkillB* skill_base = liveskillbm.get( (level << 16) | id );
	//must be exist
	if (!skill_base) return -1;
/*目前不处理体力所以注释掉
	user->charbase.sp -= LiveSkill::consume_sp();
	if ((int)user->charbase.sp < 0) user->charbase.sp = 0;
	//notify me
	Cmd::stSetHPAndMPDataUserCmd ret;
	ret.wdHP = user->charbase.hp;
	ret.wdMP = user->charbase.mp;
	ret.wdSP = user->charbase.sp;
	user->sendCmdToMe(&ret , sizeof(ret));
	*/
	//add skill bonus
	if (skill_base->upgrade) {
		if (level < skill_base->max_level) {
			if (!points) {
				//基本劳动
				int add = zMisc::randBetween(skill_base->min_point_bonus, skill_base->max_point_bonus);
				point += add;
			}else {
				//打造类
				point += points;
			}

			if (point > skill_base->point) {
				point -= skill_base->point;	
				++level;
				
				//maybe can do this check by client
				//基本技能满足要求并且没有获得进阶技能
				if (skill_base->kind == LiveSkill::BASIC_WORK && level > ADVANCE_LEVEL && !user->live_skills.skill(skill_base->up_skill_id)) {	
					Channel::sendSys(user, Cmd::INFO_TYPE_GAME, "你可以通过任务来获得该技能的进阶技能了!");
				}
			}
		}

		//notify client
		Cmd::stAddUserSkillPropertyUserCmd ret;
		ret.dwSkillID = id;
		ret.wdLevel = level;
		ret.dwExperience = point;
		user->sendCmdToMe(&ret ,sizeof(ret));
		//Channel::sendSys(user, Cmd::INFO_TYPE_GAME, "TO REMOVE:技能(%s, %ld, %ld)", skill_base->name, level, point);			
	}


	if (skill_base->kind == LiveSkill::BASIC_WORK && user->live_skills.tool && user->live_skills.tool->data.dur > 0 ) {
		this->bonus_exp(user, skill_base->exp_bonus);
		this->bonus_items(user, skill_base);

		--user->live_skills.tool->data.dur;		
		Cmd::stDurabilityUserCmd ret;
		ret.dwThisID = user->live_skills.tool->data.qwThisID;
		ret.dwDur = user->live_skills.tool->data.dur;
		ret.dwMaxDur = user->live_skills.tool->data.maxdur;
		user->sendCmdToMe(&ret , sizeof(ret));
	}
		
	return 0;
}

/**
 * \brief 劳动得到经验值
 *
 *
 * \param user: 劳动的用户
 * \param exp: 应该得到的经验
 * \return 0
 */
int LiveSkill::bonus_exp(SceneUser* user, DWORD exp)
{
	//add exp bonus	
	user->addExp(exp);
	/*
	user->charbase.exp += exp;
	//Channel::sendSys(user, Cmd::INFO_TYPE_GAME, "TO REMOVE:奖励经验(%ld)!", skill_base->exp_bonus);		
	ScenePk::attackRTExp(user , exp);
	if (user->charbase.exp >= user->charstate.nextexp) { 
		//Channel::sendSys(user, Cmd::INFO_TYPE_GAME, "TO REMOVE:升级!");	
		user->upgrade();
	}
	*/

	return 0;
}

/**
 * \brief 劳动得到物品
 *
 *
 * \param user: 劳动的用户
 * \param base_skill: 使用的劳动
 * \return 得到物品失败返回-1,否则返回得到得到物品数
 */
int LiveSkill::bonus_items(SceneUser* user, zLiveSkillB* base_skill)
{
	int count = 0;
	
	zLiveSkillB::ITEMS::iterator it = base_skill->items.begin();
	while (it != base_skill->items.end()) {
		int odds = it->odds;
		if (user->live_skills.skill(base_skill->up_skill_id)) {
			//进阶技能加成
			odds += LiveSkill::ADVANCE_WORK_BONUS;
		}
		
		//judge if the user can get a item
		if ( zMisc::selectByTenTh(odds) ) {
			int number = zMisc::randBetween(it->min_number, it->max_number);
			//ok, store it
			
			if (user->live_skills.is_upgrade()) {
				int upgrade_count[sizeof(LiveSkill::odds) - 1]; //tail unuseable
				memset(upgrade_count, 0, sizeof(upgrade_count));
				
				while (number -- > 0) {
					int level = -1;
					int upgrade_odds = 0;
					while ( ( upgrade_odds = LiveSkill::odds[++level]) != 0) {
						if 	(zMisc::selectByTenTh(upgrade_odds)) {
							++upgrade_count[level] ;
							break;
						}
					}				
				}
				
				int level = -1;
				while ( LiveSkill::odds[++level] != 0) {
					if (upgrade_count[level]) {
						//1 means package is full
						if (user->addObjectNum(it->item, upgrade_count[level], level) == 1) {
							Channel::sendSys(user, Cmd::INFO_TYPE_FAIL, "包裹已满!");
							return -1;
						}
						
						count += upgrade_count[level];
					}
				}
			}else {
				//1 means package is full
				if (user->addObjectNum(it->item, number) == 1) {
					Channel::sendSys(user, Cmd::INFO_TYPE_FAIL, "包裹已满!");
					return -1;
				}
				
				count += number;
			}
		}
		
		++it;
	}
	
	return count;
}

/**
 * \brief 根据id得到生活技能
 *
 *
 * \param id: 生活技能的id
 * \return 存在该生活技能返回,否则返回NULL
 */
LiveSkill* LiveSkills::skill(WORD id)
{
	iterator it = skills.find(id);
	if ( it != skills.end() ) {
		return &(it->second);
	}
	
	return NULL;
}

/**
 * \brief 在劳动过程中过虑禁止的操作
 *
 *
 * \param user: 劳动中的用户
 * \param ptNullCmd: 劳动过程中收到的指令
 * \param cmdLen: 指令长度
 * \return 如果该指令可以在劳动过程中处理返回true,否则返回false
 */
bool LiveSkillsManager::command_filter(SceneUser* user, const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen)
{
	bool result = false;
	
	if (user->live_skills.skill_id) {
		switch (ptNullCmd->cmd	)
		{
			//
			case Cmd::MOVE_USERCMD:
			case Cmd::MAGIC_USERCMD:
			case Cmd::TRADE_USERCMD:
				{
					result  = true;
				}
				break;
			case Cmd::PROPERTY_USERCMD:
				using namespace Cmd; //for SWAPUSEROBJECT_PROPERTY_USERCMD_PARAMETER
				if ( ptNullCmd->para == SWAPUSEROBJECT_PROPERTY_USERCMD_PARAMETER) {
					Cmd::stSwapObjectPropertyUserCmd *swap = (Cmd::stSwapObjectPropertyUserCmd *)ptNullCmd;
					zObject *ob = user->packs.uom.getObjectByThisID(swap->qwThisID);
					if (ob && (ob == user->live_skills.tool) && (swap->dst.loc() == Cmd::OBJECTCELLTYPE_NONE) ) {
						result = true;					
					}
				}
				break;
			default:
				break;
		}
	}

	if (result) {
		user->clearUState(user->live_skills.state());
		user->live_skills.skill_id = 0;
		//notify client
		user->clearStateToNine(user->live_skills.state());
		//Channel::sendSys(user, Cmd::INFO_TYPE_GAME, "TO REMOVE:劳动被取消!");	
	}

	return result;
}

/**
 * \brief 开始劳动
 *
 *
 * \param user: 请求劳动的用户
 * \param ob: 劳动必须的物品
 * \return 如果条件满足可以劳动返回0,否则返回-1
 */
int LiveSkillsManager::execute(SceneUser* user, zObject* ob)
{
	{
		Factory::iterator it = factory.identifies.find(ob->data.dwObjectID);
		if (it == factory.identifies.end() ) {
			//not work tool
			//Zebra::logger->debug("%s(%d)不是劳动工具", ob->base->name, ob->data.dwObjectID);
			return 1;
		}
	}
	
	{
		if ( (static_cast<SWORD>(ob->data.dur)) <= 0) {
			ob->data.dur = 0;
			Channel::sendSys(user, Cmd::INFO_TYPE_FAIL, "请先去修理你的%s!", ob->data.strName);
			return -1;	
		}
		user->live_skills.tool = ob;
	}

	const Scene::Tile* tile = user->scene->getTile(user->getPos());
	if (!tile) {
		//wrong position
		return -1;
	}

	std::map<DWORD, DWORD>::iterator it = factory.skills.find( (tile->type << 16) | ob->data.dwObjectID );
	if (it == factory.skills.end()) {
		//wrong map
		Channel::sendSys(user, Cmd::INFO_TYPE_FAIL, "劳动地点不对，不能使用该物品!");
		return -1;
	}

	if ((int)user->charbase.sp < LiveSkill::MAX_NEED_SP) {
		Channel::sendSys(user, Cmd::INFO_TYPE_FAIL, "体力值不足，不能劳动!");
		return -1;	
	}
	
	//check if package is full
	if ( !user->packs.main.space() < 1) {
		Channel::sendSys(user, Cmd::INFO_TYPE_FAIL, "包裹空间不足，无法劳动!");
		return -1;
	}
	
	LiveSkill* live_skill = user->live_skills.skill(it->second);
	BYTE level = (live_skill)?live_skill->level:LiveSkill::DEFAULT_START_LEVEL;
		
	zLiveSkillB* skill_base = liveskillbm.get( (level << 16) | it->second );
	if (!skill_base) {
		Zebra::logger->alarm("用户(%ld)请求了不存在的生活技能", user->accid);
		return -1;
	}

	//user havn't this skill before, add it to this user
	if (!live_skill) {
		LiveSkill tmp;
		tmp.id = it->second;
		tmp.level = level;
		tmp.point = 0;
		user->live_skills.skills[tmp.id] = tmp;
	}

	//ok, set flag
	user->live_skills.skill_id = it->second;
	user->live_skills.working_time = LiveSkill::WORKING_TIME;

	user->setUState(user->live_skills.state());

	//Channel::sendSys(user, Cmd::INFO_TYPE_GAME, "TO REMOVE:开始劳动!");	
	//notify client
	user->setStateToNine(user->live_skills.state());

	return 0;
}

/**
 * \brief 刷新劳动状态
 *
 *
 * \param user: 劳动中的用户
 * \return 刷新成功返回true,否则返回false
 */
bool LiveSkillsManager::update(SceneUser* user)
{
	if (!user->live_skills.skill_id) {
		//user cancel
		return false;
	}
	
	if ((int)--user->live_skills.working_time <= 0) {
		//time to finish
		LiveSkill* live_skill = user->live_skills.skill(user->live_skills.skill_id);
		if (!live_skill) return false; //can not be occured when normal case
		
		//add bonus	
		live_skill->bonus(user);
		
		if ( (static_cast<SWORD>(user->live_skills.tool->data.dur)) <= 0) {
			user->live_skills.tool->data.dur = 0;
			Channel::sendSys(user, Cmd::INFO_TYPE_FAIL, "你的%s已经损坏了!", user->live_skills.tool->data.strName);
			user->clearUState(user->live_skills.state());
			user->live_skills.skill_id = 0;
			return true;	
		}
		
		if ((int)user->charbase.sp < LiveSkill::MAX_NEED_SP) {
			Channel::sendSys(user, Cmd::INFO_TYPE_FAIL, "体力值不足，不能继续劳动!");
			user->clearUState(user->live_skills.state());
			user->live_skills.skill_id = 0;
			return true;	
		}

		if ( !user->packs.main.space() < 1 ) {
			Channel::sendSys(user, Cmd::INFO_TYPE_FAIL, "包裹空间不足，无法继续劳动!");
			user->clearUState(user->live_skills.state());
			user->live_skills.skill_id = 0;
			return true;
		}
		
		//reset time		
		user->live_skills.working_time = LiveSkill::WORKING_TIME;
		return false;
	}

	//Channel::sendSys(user, Cmd::INFO_TYPE_GAME, "TO REMOVE:劳动中!");
	
	return false;
}

/**
 * \brief 加载生活技能
 *
 *
 * \param user: 拥有该技能的用户
 * \param live_skill: 生活技能
 * \return 加载成功返回0,否则返回-1
 */
int LiveSkillsManager::load(SceneUser* user, const LiveSkill& live_skill)
{
	zLiveSkillB* skill_base = liveskillbm.get( (live_skill.level << 16) | live_skill.id );
	if (!skill_base)  {
		Zebra::logger->debug("生活技能不存在(%d, %d, %d)", live_skill.id, live_skill.level, live_skill.point);
		return -1;
	}
		
	user->live_skills.add(live_skill);
	
	return 0;
}

/**
 * \brief 保存生活技能
 *
 *
 * \param user: 用户
 * \param dest: 生活技能输出buf(输出)
 * \return 填充buf的长度
 */
int LiveSkillsManager::save(SceneUser* user, unsigned char* dest)
{
	//store number
	int len = sizeof(DWORD);
	int count = user->live_skills.count();
	memcpy(dest, (unsigned char*)&count, len);
	//Zebra::logger->debug("压缩生活技能个数(%d)", count);
	
	//store real data
	LiveSkills::iterator it = user->live_skills.skills.begin();

	while (it != user->live_skills.skills.end()) {
		memcpy(dest+len, (unsigned char*)&(it->second), sizeof(LiveSkill));
		len += sizeof(LiveSkill);
		++it;	
	}
	
	return len;
}

/**
 * \brief 发送自己的生活技能给客户端
 *
 *
 * \param user: 用户
 */
void LiveSkillsManager::notify(SceneUser* user)
{
	Cmd::stAddUserSkillPropertyUserCmd ret;

	LiveSkills::iterator it = user->live_skills.skills.begin();

	while ( (it) != user->live_skills.skills.end()) {

		ret.dwSkillID = it->second.id;
		ret.wdLevel = it->second.level;
		ret.dwExperience = it->second.point;
		ret.dwMaxExperience = 0;
		user->sendCmdToMe(&ret , sizeof(ret));
		
		++it;
	}

}

LiveSkillsManager* LiveSkillsManager::instance_ = NULL;

/**
 * \brief 得到唯一实例
 *
 *
 * \return 返回生活技能管理器
 */
LiveSkillsManager& LiveSkillsManager::instance()
{
	if (!instance_) {
		static LiveSkillsManager factory;
		instance_ = &factory;
	}
	
	return *instance_;
}


/**
 * \brief 构造函数
 *
 */
LiveSkillsManager::LiveSkillsManager()
{
	liveskillbm.execAll(factory);
}


LiveSkillsManager::~LiveSkillsManager()
{
	
	
	
}
