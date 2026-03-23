/**
 * \file
 * \version  $Id: SceneSkill.cpp $
 * \author  
 * \date 
 * \brief 实现技能命令的处理
 *
 */
#include "SceneUser.h"
#include "Scene.h"
#include "SceneSkill.h"
#include "ScenePk.h"
#include "zObject.h"
#include "Chat.h"

using namespace Cmd;


struct TeamSkillExec : public TeamMemExec
{
	SceneUser *leader;
	zSkill *skill;
	TeamSkillExec(SceneUser *user , zSkill *s)
	{
		leader = user;
		skill = s;
	}
	bool exec(TeamMember &member)
	{
		SceneUser *u = leader->scene->getUserByTempID(member.tempid);
		if(u)
		{
			/*
			std::vector<DWORD>::const_iterator iter = skill->base->benignID.gid.begin();
			std::vector<zSkillB::Odds>::const_iterator iter_1 = skill->base->benignrating.gr.begin();
			for(;iter != skill->base->benignID.gid.end(),iter_1 != skill->base->benignrating.gr.end();iter++ , iter_1++)
			{
				if(zMisc::selectByPercent(iter_1->percent))
				{
					u->skillState.plus.setCharState(u , *iter , false);
					u->skillState.plus.good[*iter].value = iter_1->value;
					u->skillState.plus.good[*iter].leavetime = skill->base->ptime + 1;
					u->skillState.plus.setCharState(u , *iter , true);
				}
			}
			Cmd::stRefreshStateMapScreenUserCmd ret;
			ret.dwUserTempID = u->tempid;
			bcopy(u->getByState(),ret.state,sizeof(ret.state));
			u->scene->sendCmdToNine(u->getPosI(),&ret,sizeof(ret) , false);
			// */
			return true;
		}
		return false;
	}
};
/**
  * \brief 添加玩家技能
  *
  * \param rev 添加技能命令
  *
  * \return 添加成功返回TRUE,否则返回FALSE
  *
  */
bool SceneUser::addSkillData(const Cmd::stAddUserSkillPropertyUserCmd *rev)
{
	zSkill::create(this , rev->dwSkillID , rev->wdLevel) ;
	charbase.skillpoint --;

	//刷新用户数据
	Cmd::stMainUserDataUserCmd ret;
	full_t_MainUserData(ret.data);
	sendCmdToMe(&ret , sizeof(ret));
	return true;
}
/**
  * \brief 删除玩家技能
  *
  * \param rev 删除技能命令
  *
  * \return 删除成功返回TRUE,否则返回FALSE
  *
  */
bool SceneUser::removeSkill(const Cmd::stRemoveUserSkillPropertyUserCmd *rev)
{
	zSkill *skill = usm.findSkill(rev->dwSkillID);
	if(!skill)
	{
		return false;
	}
	usm.removeSkill(skill);

	//刷新用户数据
	Cmd::stMainUserDataUserCmd  userinfo;
	full_t_MainUserData(userinfo.data);
	sendCmdToMe(&userinfo,sizeof(userinfo));
	return true;
}
/**
  * \brief 更新玩家技能
  *
  * \param dwSkillID 技能ID
  * \param needSkillPoint 所需技能点
  * \param skilllevelup 升级所需技能点
  *
  * \return 更新成功返回TRUE,否则返回FALSE
  *
  */
bool SceneUser::upgradeSkill(DWORD dwSkillID ,bool needSkillPoint,bool skilllevelup)
{

	if( this->scene->getRealMapID()>=10001 && this->scene->getRealMapID()<=10601)//天下第一比武观战地图
	{	
		Channel::sendSys(this, Cmd::INFO_TYPE_SYS, "此地图禁止学习技能！"); 
		return false;
	}	

//TZ 增加学习天赋技能不扣技能点	
switch(dwSkillID)
	{
		case 20001:
		case 20002:
		case 20003:
		case 20004:
		case 20005:
		case 20006:
		case 20007:
		case 20008:
		case 20009:
		case 10001:
		case 10002:
		case 10003:
		case 10004:
		case 10005:
		case 10006:
		case 10007:
		case 10008:
		case 10009:
		case 10010:
		case 10011:
		case 10012:
		case 10013:
		case 10014:
		case 10015:
		case 10016:
		case 10017:
		case 3001:
		case 3002:
		case 3003:
		case 3004:
		case 3005:
		case 3006:
		case 3007:
		case 3008:
		case 3009:
			{
				needSkillPoint = false;
				skilllevelup = false;
			}
			break;
		default:
			break;
	}
	if(charbase.skillpoint < 1 && needSkillPoint)
	{
		return false;
	}

	zSkill *skill = usm.findSkill(dwSkillID);
	//恋恋 三大职业三选一
		zSkill *s1 = usm.findSkill(10773);
		zSkill *s2 = usm.findSkill(10050);
		zSkill *s3 = usm.findSkill(920);
		if(dwSkillID == 10773)
		{
			if(s2 || s3)
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_SYS, "卫士、刺客、枪职业只能学习一种"); 
				return false;
			}
		}
		if(dwSkillID == 10050)
		{
			if(s1 || s3)
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_SYS, "卫士、刺客、枪职业只能学习一种"); 
				return false;
			}
		}
		if(dwSkillID == 920)
		{
			if(s2 || s1)
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_SYS, "卫士、刺客、枪职业只能学习一种"); 
				return false;
			}
		}
	if(!skill)
	{
		skill = zSkill::create(this , dwSkillID , 1);
		if(!skill)
		{
			return false;
		}

		
				
	//soke 判断下技能的职业要求是否和用户的职业一致(阵法除外)
	if(skill->base->kind != charbase.changejob && charbase.changejob !=9 && skill->base->kind!=9)
	{
		Zebra::logger->error("系统:玩家[%s]企图学习该职业系不可以学习的技能:[%s]", charbase.name, skill->base->name);
		if(charbase.changejob==0)
	    {
		   Channel::sendSys(this, Cmd::INFO_TYPE_SYS, "你没有选择职业,不能学习职业技能！"); 
		}
		else if(charbase.changejob==1)
	    {
		   Channel::sendSys(this, Cmd::INFO_TYPE_SYS, "你的技能职业是：武术，不能学习其它系职业技能！"); 
		}
		else if(charbase.changejob==2)
	    {
		   Channel::sendSys(this, Cmd::INFO_TYPE_SYS, "你的技能职业是：射术，不能学习其它系职业技能！"); 
		}
		else if(charbase.changejob==3)
	    {
		   Channel::sendSys(this, Cmd::INFO_TYPE_SYS, "你的技能职业是：法术，不能学习其它系职业技能！"); 
		}
		else if(charbase.changejob==4)
	    {
		   Channel::sendSys(this, Cmd::INFO_TYPE_SYS, "你的技能职业是：召唤，不能学习其它系职业技能！"); 
		}
		else if(charbase.changejob==5)
	    {
		   Channel::sendSys(this, Cmd::INFO_TYPE_SYS, "你的技能职业是：仙术，不能学习其它系职业技能！"); 
		}
		else if(charbase.changejob==6)
		{
			Channel::sendSys(this, Cmd::INFO_TYPE_SYS, "你的技能职业是：刺客，不能学习其它系职业技能！"); 
		}
		else if(charbase.changejob==7)
		{
			Channel::sendSys(this, Cmd::INFO_TYPE_SYS, "你的技能职业是：卫士，不能学习其它系职业技能！"); 
		}
		else if(charbase.changejob==8)
		{
			Channel::sendSys(this, Cmd::INFO_TYPE_SYS, "你的技能职业是：枪系，不能学习其它系职业技能！"); 
		}
		else if(charbase.changejob>9)
	    {
		   Channel::sendSys(this, Cmd::INFO_TYPE_SYS, "你没有选择职业,不能学习职业技能！"); 
		}
		return false;
	}
		if(needSkillPoint)
			charbase.skillpoint --;

		Cmd::stAddUserSkillPropertyUserCmd ret;
		ret.dwSkillID = skill->data.skillid;
		ret.wdLevel = skill->data.level;
		ret.wdUpNum = this->skillUpLevel(skill->base->skillid, skill->base->kind);
		if (ret.wdUpNum+ret.wdLevel>10) ret.wdUpNum = 10 - ret.wdLevel;
		ret.dwExperience = 0;
		ret.dwMaxExperience = 0;
		sendCmdToMe(&ret , sizeof(ret));

		this->setupCharBase();
		//刷新用户数据
		Cmd::stMainUserDataUserCmd ret_1;
		full_t_MainUserData(ret_1.data);
		sendCmdToMe(&ret_1 , sizeof(ret_1));
		return true;
	}
/*
	if(!skill->canUpgrade(this))
	{
		return false;
	}
*/

	if(skill->data.level >= MAX_SKILLLEVEL)
	{
		return false;
	}

	if (!skill->checkSkillStudy(true))
	{
		return false;
	}

	if (!skill->checkSkillBook(true))
	{
		return false;
	}

	zSkillB *tempbm = skillbm.get(skill_hash(skill->base->skillid , skill->base->level+1));
	if (tempbm)
	{
		if (this->usm.getPointInTree(skill->base->kind, skill->base->subkind) < tempbm->needpoint)
		{
			return false;
		}
	}
	else
	{
		return false;
	}
#ifdef _DEBUGLOG
	Zebra::logger->error("技能升级检查前提技能点数 当前点[%u] 需要点[%u] 当前BASE技能等级[%u] dwSkillID=[%u]", this->usm.getPointInTree(skill->base->kind, skill->base->subkind), skill->base->needpoint, skill->base->level, dwSkillID);
#endif
    
    if(skilllevelup)
    {
      	if ((int)charbase.skillpoint - 1 >=0   )
	    {
		    charbase.skillpoint -=1;
	    }
	    else
	    {
	     	return false;
	    }
	}

	skill->data.level ++;
	skill->setupSkillBase(this);
	zRTime ctv;
	skill->lastUseTime = ctv.msecs() - skill->base->dtime;
	this->setupCharBase();

	Cmd::stAddUserSkillPropertyUserCmd ret;
	ret.dwSkillID = skill->data.skillid;
	ret.wdLevel = skill->data.level;
	ret.wdUpNum = this->skillUpLevel(skill->base->skillid, skill->base->kind);
	if (ret.wdUpNum+ret.wdLevel>10) ret.wdUpNum = 10 - ret.wdLevel;
	ret.dwExperience = 0;
	ret.dwMaxExperience = 0;
	sendCmdToMe(&ret , sizeof(ret));
	
	//刷新用户数据
	Cmd::stMainUserDataUserCmd ret_1;
	full_t_MainUserData(ret_1.data);
	sendCmdToMe(&ret_1 , sizeof(ret_1));
	return true;
}

int getCharType(DWORD type)
{
	int ret = 0;
	if(type != 0)
	{
		for( ; type > 0; type = type >> 1 ,ret ++);
		ret --;
	}
	else
	{
		return -1;
	}
	return ret;
}


