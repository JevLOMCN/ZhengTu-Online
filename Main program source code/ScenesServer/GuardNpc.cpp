/**
 * \file
 * \version	$Id: GuardNpc.cpp 6808 2006-06-08 06:14:12Z xwl $
 * \author	liqingyu,liqingyu@zhengtu.com
 * \date		2005-06-15
 * \brief		护镖
 * 
 */

#include "GuardNpc.h"
#include "Chat.h"
#include "TimeTick.h"
#include "Scene.h"
#include "QuestEvent.h"
#include "QuestTable.h"
#include "SessionClient.h"

/**
 * \brief 构造函数
 *
 * 初始化相关变量
 * \param scene npc所在的场景
 * \param npc 基本数据
 * \param define 定义数据
 * \param type 类型
 * \param entrytype 场景类型 
 * \param abase 增强npc字典数据
 */
GuardNpc::GuardNpc(Scene *scene, zNpcB *npc, const t_NpcDefine *define, const SceneNpcType type, const SceneEntryType entrytype, zNpcB *abase) :
	ScenePet(scene, npc, define, type, entrytype, abase), _exp(0), _gold(0), _status(0),  _time(SceneTimeTick::currentTime)
{
	masterIsAlive = true;
	isSeptGuard = false;
}

/**
 * \brief 析构函数
 *
 * 重置相关变量
 *
 */
GuardNpc::~GuardNpc()
{
	reset();	
}

/**     
 * \brief  设置镖车所有者
 *
 * \param user: 接镖人
 * \return 无
 */   
void GuardNpc::owner(SceneUser* user)
{
	//_owner = user;	
	setMaster(user);
	_name = user->name;
	user->guard = this;
	/*
	Zebra::logger->debug("[镖车] %s, %u, %s(%u, %u), %s(%u, %u) 接镖 _gold=%u",
					_name.c_str(),
					tempid, name, getPos().x, getPos().y,
					user ? user->name : "NULL", user ? user->getPos().x : 0, user ? user->getPos().y : 0,
					_gold);
					*/
}	

/**     
 * \brief  取得镖车所有者
 *
 * \return  接镖人
 */   
SceneUser* GuardNpc::owner()
{
	SceneUser * _owner = (SceneUser *)getMaster();
	return _owner;
}

/**     
 * \brief  设置镖车金额
 *
 * \param money: 金额
 * \return 无
 */   	
void GuardNpc::gold(int money)
{
	/*
	if (_gold)
		Zebra::logger->trace("[镖车]%s 的 %s 金钱改变 %u->%u", _name.c_str(), name, _gold, money);
		*/

	_gold = money/2; //soke 镖车被砍得到一半金钱
//	_gold = money;
}

/**     
 * \brief  取得镖车金额
 *
 * \return  金额
 */
int GuardNpc::gold() const
{
	return _gold;
}

/**     
 * \brief  设置镖车经验
 *
 * \param experience: 经验
 * \return 无
 */   
void GuardNpc::exp(int experience)
{
	_exp = experience;
}

/**     
 * \brief  取得镖车经验
 *
 * \return 经验
 */
int GuardNpc::exp() const
{
	return _exp;
}

/**     
 * \brief 运镖失败
 *
 * 设置该次运镖失败
 *
 * \return 无
 */
void GuardNpc::reset()
{
	SceneUser * _owner = (SceneUser *)getMaster();
	if (_owner)
	{
		Cmd::stDelPetPetCmd del;
		del.id= tempid;
		del.type = Cmd::PET_TYPE_GUARDNPC;
		_owner->sendCmdToMe(&del, sizeof(del));

		_owner->guard = NULL;
	}
	_owner = NULL;
	clearMaster();
	
//	_gold = 0;
	
	moveAction = false;
	_status = 2;
	_time = SceneTimeTick::currentTime;
	_time.addDelay(3*60*1000); //three mins 
#ifdef _XWL_DEBUG
	//Zebra::logger->debug("[镖车]%s 镖车 reset time:%d", _name.c_str(), _time.sec()-SceneTimeTick::currentTime.sec());
#endif
}

/**     
 * \brief 状态检查
 *
 * 回调函数,定期检查运镖状态,并做相应处理
 *
 * \return 无
 */
void GuardNpc::check()
{
	SceneUser * _owner = (SceneUser *)getMaster();

	if (_owner && 0==_owner->guard) _owner->guard = this;

	if (_status == 0)
	{
		if (!_owner || abs(pos.x-_owner->getPos().x)>SCREEN_WIDTH || abs(pos.y-_owner->getPos().y)>SCREEN_HEIGHT)
		{
			if (_owner)
			{
				Channel::sendSys(_owner, Cmd::INFO_TYPE_GAME, "你离护送目标太远了，会有危险的!");
				Channel::sendSys(_owner, Cmd::INFO_TYPE_EXP, "你离护送目标太远了");
				if (_owner->getState() == zSceneEntry::SceneEntry_Normal)
					masterIsAlive = true;
			}

			moveAction = false;
			/*
			Zebra::logger->debug("[镖车] %s, %u, %s(%u, %u), %s(%u, %u) _gold=%u, 0 -> 1",
							_name.c_str(),
					tempid, name, getPos().x, getPos().y,
					_owner ? _owner->name : "NULL", _owner ? _owner->getPos().x : 0, _owner ? _owner->getPos().y : 0,
					_gold);
					*/
			_status = 1;
			_time = SceneTimeTick::currentTime;
			_time.addDelay(3*60*1000); //three mins 
		}
	}
	
	if (_status == 1)
	{
		if (_owner && _owner->scene==scene && (abs(pos.x-_owner->getPos().x) < SCREEN_WIDTH && abs(pos.y-_owner->getPos().y) < SCREEN_HEIGHT))
		{
			if (_owner->getState() == zSceneEntry::SceneEntry_Normal)
				masterIsAlive = true;
			moveAction = true;
			_status = 0;

			/*
			Zebra::logger->debug("[镖车] %s, %u, %s(%u, %u), %s(%u, %u) _gold=%u, 1 -> 0",
							_name.c_str(),
					tempid, name, getPos().x, getPos().y,
					_owner ? _owner->name : "NULL", _owner ? _owner->getPos().x : 0, _owner ? _owner->getPos().y : 0,
					_gold);
					*/
		}
		else if (!_owner && (SceneTimeTick::currentTime>_time))
		{
			/*
			Zebra::logger->debug("[镖车] %s, %u, %s(%u, %u), %s(%u, %u) _gold=%u, 1 -> 2, 3分钟没找到主人，运镖失败",
							_name.c_str(),
					tempid, name, getPos().x, getPos().y,
					_owner ? _owner->name : "NULL", _owner ? _owner->getPos().x : 0, _owner ? _owner->getPos().y : 0,
					_gold);
					*/

			reset();
			setClearState();
		}
	}
	/*
	if (_status == 1 && _owner && _owner->scene==scene && (abs(pos.x-_owner->getPos().x) < SCREEN_WIDTH && abs(pos.y-_owner->getPos().y) < SCREEN_HEIGHT)) {
		moveAction = true;
		_status = 0;
	}


	if (_status != 1  && _owner && (abs(pos.x-_owner->getPos().x) > SCREEN_WIDTH || abs(pos.y-_owner->getPos().y) > SCREEN_HEIGHT)) {
		Channel::sendSys(_owner, Cmd::INFO_TYPE_GAME, "你离护送目标太远了，会有危险的!");
		Channel::sendSys(_owner, Cmd::INFO_TYPE_EXP, "你离护送目标太远了");
		moveAction = false;
		_status = 1;
	}
	*/

	/*
	if (_status !=2 && _owner && (abs(pos.x-_owner->getPos().x) > 2*SCREEN_WIDTH || abs(pos.y-_owner->getPos().y) > 2*SCREEN_HEIGHT)) {
		Channel::sendSys(_owner, Cmd::INFO_TYPE_GAME, "由于你离镖车太远了，这次护镖失败了");
		OnOther event(2);
		EventTable::instance().execute(*_owner, event);
		reset();
	}
	*/
	
	//Zebra::logger->debug("check: current time:%d, check time:%d", SceneTimeTick::currentTime.msecs(), _time.msecs());
	if (_status == 2  && SceneTimeTick::currentTime > _time) {
		//Zebra::logger->debug("clear: current time:%d, check time:%d", SceneTimeTick::currentTime.msecs(), _time.msecs());
		setClearState();
		return;
	}

	bool ok = false;
	if (scene) {
		std::string n = scene->name;
		if (n.find(_map) != std::string::npos) {
			ok = true;
		}
	}
	if (_status == 0 && ok && abs(pos.x-_dest.x) < (SCREEN_WIDTH>>1) && abs(pos.y-_dest.y) < (SCREEN_HEIGHT >> 1) ) {
		on_reached();
	}
	//_time = SceneTimeTick::currentTime;
}

/**     
 * \brief 镖车死亡
 *
 * 回调函数,在镖车被击毁时调用,奖励劫镖者
 * ]param attacker 凶手
 * \return 无
 */
void GuardNpc::on_death(SceneEntryPk* attacker)
{
	if (0==attacker) return;
	/*
	   if ( !strcmp(_name.c_str().c_str(), attacker->name) ) {
	   return;
	   }
	   */

	SceneEntryPk *m = attacker->getTopMaster();
	if (!m) return;

	if (m->getType()==zSceneEntry::SceneEntry_Player)
	{

		/////////////////////////////////////////////////////////////////////////////////////////////
		//soke 显示个劫镖公告
		Cmd::stChannelChatUserCmd send;
		zRTime ctv;
		send.dwType = Cmd::CHAT_TYPE_NINE;
		send.dwChatTime = ctv.sec();
		if (((SceneUser *)m)->mask.is_masking())
	//	snprintf((char *)send.pstrChat, MAX_CHATINFO, "蒙面人 劫镖成功，获得金子%d", _gold);
		Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEFROLL,"哇～ %s镖车在(%u, %u)被某神秘人截获，获得金子%d文",_name.c_str(),getPos().x, getPos().y, _gold);
		else
	//	snprintf((char *)send.pstrChat, MAX_CHATINFO, "%s劫镖成功，获得金子%d", m->name, _gold);

		Channel::sendAllInfo(Cmd::INFO_TYPE_ZISETROLL,"哇～ %s镖车在(%u, %u)被%s 截获，获得金子%d文",_name.c_str(),getPos().x, getPos().y, m->name,_gold);
		//strncpy((char *)send.pstrName, "护镖", MAX_NAMESIZE);
		//scene->sendCmdToNine(getPosI(), &send, sizeof(send));	
		Zebra::logger->debug("%s劫镖成功，获得金子%d", m->name, _gold);
		/////////////////////////////////////////////////////////////////////////////////////////////

		char info[MAX_CHATINFO];
		memset(info, 0, MAX_CHATINFO);
		snprintf(info,  MAX_CHATINFO, "你抢劫成功，获得金子"); 

		if (((SceneUser*)m)->charbase.country!=scene->getCountryID())//在外国截镖才有钱
			((SceneUser*)m)->packs.addGold(getRobGold(), "抢劫获得",info);
	}

	SceneUser * _owner = (SceneUser *)getMaster();
	/*
	Zebra::logger->debug("[镖车] %s, %u, %s(%u, %u), %s(%u, %u), 死亡，杀手 %s _gold=%u",
							_name.c_str(),
			tempid, name, getPos().x, getPos().y,
			_owner ? _owner->name : "NULL", _owner ? _owner->getPos().x : 0, _owner ? _owner->getPos().y : 0,
			m->name, _gold);
			*/

	if (_owner) {
		OnOther event(2);
		EventTable::instance().execute(*_owner, event);
		Channel::sendSys(_owner, Cmd::INFO_TYPE_EXP, "你的护送目标死亡");
	}
	else
	{
		if (getMasterID())
		{
			Cmd::Session::t_guardFail_SceneSession send;
			send.userID = getMasterID();
			send.type = 0;
			sessionClient->sendCmd(&send, sizeof(send));
		}
	}
}

/**     
 * \brief 镖车到达
 *
 * 回调函数,在镖车到达目的地时被调用,奖励护镖者
 *
 * \return 无
 */
void GuardNpc::on_reached()
{
	SceneUser * _owner = (SceneUser *)getMaster();
	if (_owner)
	{
		if (id>54201 && id<54350)
		{
			OnOther event(11);
			EventTable::instance().execute(*_owner, event);
		}
		else
		{
			OnOther event(1);
			EventTable::instance().execute(*_owner, event);
		}
	}
	/*
	Zebra::logger->debug("[镖车] %s, %u, %s(%u, %u), %s(%u, %u), 成功运到 _gold=%u",
							_name.c_str(),
			tempid, name, getPos().x, getPos().y,
			_owner ? _owner->name : "NULL", _owner ? _owner->getPos().x : 0, _owner ? _owner->getPos().y : 0,
			_gold);
			*/

	////////////////////////////////////////////////////////
	if (_owner&&_exp>0) {
		Channel::sendSys(_owner, Cmd::INFO_TYPE_GAME, "护镖成功，获得经验%d", _exp);
		Channel::sendSys(_owner, Cmd::INFO_TYPE_EXP, "护镖成功，获得经验%d", _exp);
		//add exp bonus
		//_owner->charbase.exp += _exp;
		////ScenePk::attackRTExp(_owner, _exp);
		//if (_owner->charbase.exp >= _owner->charstate.nextexp) { 
		//	_owner->upgrade();
		//}
		_owner->addExp(_exp);
	}
    /////////////////////////////////////////////////////
	reset();
	setClearState();
	return;
}

void GuardNpc::dest(const zPos& pos)
{
	_dest = pos;
}

void GuardNpc::map(const std::string& name)
{
	_map = name;
}

/**
 * \brief 检查是否可以移动
 *
 * \return 是否可以移动
 */
bool GuardNpc::canMove()
{
	return (masterIsAlive && ScenePet::canMove()); 
}

/**
 * \brief 向主人移动，并检查距离
 *
 * \return 是否移动成功
 */
bool GuardNpc::moveToMaster()
{
#ifdef _XWL_DEBUG
	//Channel::sendNine(this, "GuardNpc::moveToMaster()");
#endif
	SceneEntryPk * master = getMaster();
	if (!master)
	{
		check();
		return true;
	}

	//AIC->setActRegion(master->getPos(), 2, 2);

	if (!canMove())
	{
		check();
		if (getMaster())
			return false;
		else
			return true;
	}

	//check();
	if (checkMoveTime(SceneTimeTick::currentTime))
	{
		if (master->scene == scene)
		{
			check();
			if (!getMaster()) return true;//运镖成功删除了主人

			if ((scene->zPosShortRange(getPos(), master->getPos(), npc_pet_chase_region)))
				return false;
			else
			{
				if (!gotoFindPath(getPos(), master->getPos()))
					return goTo(master->getPos());
				return true;
			}
		}
		/*
		else
		{
			if (canMove() && master->scene && masterIsAlive)
			{
				if (changeMap(master->scene, master->getPos()))
				{
					check();
					return true;
				}
				else
					return false;
				
			}
		}
		*/
	}
	return false;
}

/*
 * \brief 保存护卫NPC的数据到指定位置（跨服时用）
 *
 * \param dest 目的地址
 *
 * \return 数据大小
 */
DWORD GuardNpc::save(BYTE * dest)
{
	bcopy(&petData, dest, sizeof(Cmd::t_PetData));
	Cmd::t_PetData * t = (Cmd::t_PetData *)dest;
	strncpy(t->name, _map.c_str(), MAX_NAMESIZE);
	t->str = _dest.x;
	t->intel = _dest.y;
	t->agi = _gold;
	t->men = _exp;
	return sizeof(Cmd::t_PetData);
}

DWORD GuardNpc::getRobGold()
{
	return _gold/(isSeptGuard?2:1);
}
