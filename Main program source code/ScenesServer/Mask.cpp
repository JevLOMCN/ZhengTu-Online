/**
 * \file
 * \version	$Id: Mask.cpp  $
 * \author	
 * \date	
 * \brief	蒙面系统
 * 
 */

#include "Mask.h"

#include "zObject.h"
#include "SceneUser.h"
#include "Command.h"
#include "zMisc.h"
#include "Chat.h"
#include "TimeTick.h"

Mask::Mask() : _drop(false), _mask(NULL), _user(NULL)
{

}

Mask::~Mask()
{

}

bool Mask::is_use(zObject* ob) const
{
	return _mask == ob;
}
/**
 * \brief 使用蒙面巾
 *
 *
 * \param user: 使用蒙面巾的用户
 * \param ob: 蒙面巾物品
 * \return 物品类型不正确返回1,蒙面巾处于冷却期返回-1,成功返回0
 */
int Mask::on_use(SceneUser* user, zObject* ob)
{
	if (!ob || ob->base->kind!=ItemType_MASK) {
		return 1;
	}

	if (_drop && _last > SceneTimeTick::currentTime) {
		Channel::sendSys(user, Cmd::INFO_TYPE_FAIL, "暂时不能使用!");
		return -1;
	}

	_drop = false;
	bool notify=true;

	if (_mask && _mask->data.qwThisID == ob->data.qwThisID) {
		_mask = NULL;
		_user = NULL;
		user->sendMeToNine();
		user->sendPetDataToNine();
		Channel::sendSys(user, Cmd::INFO_TYPE_GAME, "解除%s!", ob->data.strName);
	}else {
		if(_mask)
		{
			notify=false;
		}
		_mask = ob;
		_mask->data.dur -= TIME_CONSUME_PERMANENCE;
		if((short)_mask->data.dur < 0)
		{
			_mask->data.dur = 0;
		}
		Channel::sendSys(user, Cmd::INFO_TYPE_GAME, "使用%s!", ob->data.strName);	
		if ( user->tradeorder.hasBegin() ) {
			user->tradeorder.cancel();
		}
		if((short)_mask->data.dur > 0 && notify)
		{
			user->sendMeToNine();
			user->sendPetDataToNine();
		}

		_user = user;
		reduce_permanence();
	}
	
	return 0;
}

/**
 * \brief 判断是否处于蒙面状态
 *
 *
 * \return 蒙面返回true,否则返回false
 */
bool Mask::is_masking() const
{
	return (_mask)?true:false;	
}

/**
 * \brief 蒙面时进行交易则取消蒙面状态
 *
 *
 * \return 0
 */
int Mask::on_trade()
{
	//FunctionTimes times(24,__FUNCTION__);
	_mask = NULL;
	if (_user) _user->sendMeToNine();
	_user = NULL;

	return 0;
}

/**
 * \brief 蒙面时获得护宝状态则取消蒙面状态
 *
 *
 * \return 0
 */
int Mask::on_gem()
{
	//FunctionTimes times(25,__FUNCTION__);
	_mask = NULL;
	if (_user) _user->sendMeToNine();
	_user = NULL;

	return 0;
}

/**
 * \brief 定时器消耗耐久
 *
 *
 * \return 处于蒙面状态返回0,否则返回-1
 */
int Mask::on_timer()
{
	if (_mask && _user) {
		_mask->data.dur -= TIME_CONSUME_PERMANENCE;
		if((short)_mask->data.dur < 0)
		{
			_mask->data.dur = 0;
		}
			
		
		return reduce_permanence();		
	}
	
	return -1;
}

/**
 * \brief 蒙面状态时被攻击,可能造成蒙面巾掉落
 *
 *
 * \return 处于蒙面状态返回0,否则返回-1
 */
int Mask::on_defence()
{
	if (_mask && _user) {
		_mask->data.dur -= ATTACKED_CONSUME_PERMANENCE;
		
		reduce_permanence();
	
		if (!_mask) return 0;	

		return 0; //目前不处理被打掉，所以在这里直接返回了。

		int drop = drop_odds();
		Zebra::logger->debug("蒙面巾被打掉概率:%d", drop);
		if (zMisc::selectByTenTh(drop)) {
			//FunctionTimes times(26,__FUNCTION__);
			Channel::sendSys(_user, Cmd::INFO_TYPE_GAME, "你的%s被打掉了!", _mask->data.strName);
			_drop = true;
			_last = SceneTimeTick::currentTime;
			_last.addDelay(ATTACK_USE_TIME);
			_mask = NULL;
			//refresh right now
			_user->sendMeToNine();
			_user = NULL;
		}
		
		return 0;
		
	}
	
	return -1;	
}


/**
 * \brief 处于蒙面状态时攻击别人发出系统提示
 *
 *
 * \param victim: 被攻击用户
 * \return 0
 */
int Mask::on_attack(SceneUser* victim)
{
	if (_mask && _user) {
		Channel::sendSys(victim, Cmd::INFO_TYPE_GAME, "你正受到蒙面人的恶意攻击，你可以对其正当防卫!");
	}
	
	return 0;
}

/**
 * \brief 消耗蒙面巾持久
 *
 *
 * \return 0
 */
int Mask::reduce_permanence()
{
	if ((SWORD)_mask->data.dur > 0) {
		Cmd::stDurabilityUserCmd ret;
		ret.dwThisID = _mask->data.qwThisID;
		ret.dwDur = _mask->data.dur;
		ret.dwMaxDur = _mask->data.maxdur;
		_user->sendCmdToMe(&ret , sizeof(ret));	
	}else {
		//FunctionTimes times(27,__FUNCTION__);
		_user->packs.removeObject(_mask); //notify and delete
		_mask = NULL;		
//refresh right now
		_user->sendMeToNine();		
		//_user->sendPetDataToNine();
		_user = NULL;
	}
	
	return 0;
}

/**
 * \brief 蒙面时被别人攻击消耗耐久
 *
 *
 * \return 消耗的耐久值
 */
int Mask::drop_odds() const
{
	return 100*DROP_ODDS/_mask->data.maxdur;
}
