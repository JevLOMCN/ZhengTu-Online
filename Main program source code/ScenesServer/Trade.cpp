#include "SceneUser.h"
#include "Scene.h"
#include "Chat.h"
#include "SceneNpc.h"
#include "SceneNpcManager.h"
#include "NpcTrade.h"
#include "QuestEvent.h"
#include "QuestTable.h"
#include "RebuildObject.h"
#include "SessionClient.h"
#include <math.h>

PrivateStore::PrivateStore() : _step(NONE)
{


}

PrivateStore::~PrivateStore() 
{
	clear();
	_step = NONE;
}

void PrivateStore::step(STEP step_, SceneUser *pUser)
{
	_step = step_;

	if (_step == BEGIN) {
		pUser->setUState(Cmd::USTATE_SITDOWN);
		pUser->setUState(Cmd::USTATE_PRIVATE_STORE);
	}

	if (_step == NONE) {
		pUser->clearUState(Cmd::USTATE_PRIVATE_STORE);
		pUser->clearUState(Cmd::USTATE_SITDOWN);
		clear();
	}
}

PrivateStore::STEP PrivateStore::step()
{
	return _step;
}

void PrivateStore::clear()
{
	_items.clear();
}

void PrivateStore::add(zObject* ob, DWORD money, BYTE x, BYTE y)
{
	_items[ob->data.qwThisID] = SellInfo(ob, money, x, y);
}

void PrivateStore::remove(DWORD id) 
{
	_items.erase(id);
}

void PrivateStore::show(SceneUser* target)
{
#if 0
	std::map<DWORD, SellInfo>::iterator it = _items.begin();
	for ( ; it!=_items.end(); ++it) {
		Cmd::stAddObjectSellUserCmd cmd;
		cmd.object = (*it).second.object()->data;
		cmd.price = (*it).second.money();
		cmd.x = (*it).second.x();
		cmd.y = (*it).second.y();
		target->sendCmdToMe(&cmd, sizeof(cmd));
	}

#else
	std::map<DWORD, SellInfo>::iterator it = _items.begin();
	char buffer[zSocket::MAX_USERDATASIZE];
	Cmd::stAddObjectSellListUserCmd *cmd = (Cmd::stAddObjectSellListUserCmd *)buffer;
	constructInPlace(cmd);
	for ( ; it!=_items.end(); ++it) {
		if(sizeof(Cmd::stAddObjectSellListUserCmd) + (cmd->num + 1) * sizeof(cmd->list[0])>= zSocket::MAX_USERDATASIZE)
		{
			target->sendCmdToMe(cmd, sizeof(Cmd::stAddObjectSellListUserCmd) + cmd->num * sizeof(cmd->list[0]));
			cmd->num = 0;
		}
		cmd->list[cmd->num].object = (*it).second.object()->data;
		cmd->list[cmd->num].price = (*it).second.money();
		cmd->list[cmd->num].x = (*it).second.x();
		cmd->list[cmd->num].y = (*it).second.y();
		cmd->num ++;
	}
	if(cmd->num)
	{
			target->sendCmdToMe(cmd, sizeof(Cmd::stAddObjectSellListUserCmd) + cmd->num * sizeof(cmd->list[0]));
			cmd->num = 0;
	}
#endif
	Cmd::stSellTradeUserCmd ret;
	target->sendCmdToMe(&ret, sizeof(ret));
}

PrivateStore::SellInfo* PrivateStore::sell_ob(DWORD id)
{

	std::map<DWORD, SellInfo>::iterator it = _items.find(id);
	if (it != _items.end()) {
		return &(it->second);
	}

	return NULL;
}

TradeOrder::TradeOrder(SceneUser* owner) : _me(owner), _target(NULL),_targetid(0), _money(0)
{
	finish();
}

TradeOrder::~TradeOrder()
{
	reset();
}

void TradeOrder::reset()
{
	SceneUser *pUser=_me->scene->getUserByID(_targetid);
	if(pUser && (pUser == _target) && _target->tradeorder.target() == _me)
	{

		_target->tradeorder.finish();


		Cmd::stCancelTradeUserCmd cancel;
		cancel.dwUserTempID = _target->tempid;
		_target->sendCmdToMe(&cancel, sizeof(cancel));
		Channel::sendSys(_target, Cmd::INFO_TYPE_FAIL, "交易被取消");
		
		Zebra::logger->trace("[交易:玩家<------>玩家]%s取消与%s的交易", _me->name, _target->name);
	}
	else
	{
		if(_targetid)
		{
			Zebra::logger->debug("[交易:玩家<------>玩家]%s取消与%d的交易,但这个人已经不在了", _me->name, _targetid);
		}
	}

}

void TradeOrder::cancel()
{	
//	Channel::sendSys(_me, Cmd::INFO_TYPE_FAIL, "交易被取消");
	
	Cmd::stCancelTradeUserCmd cancel;
	cancel.dwUserTempID = _me->tempid;
	_me->sendCmdToMe(&cancel, sizeof(cancel));

	reset();

	finish();	
}

SceneUser* TradeOrder::target() const
{
	return _target;
}

bool TradeOrder::can_trade()
{
	if (_target->packs.uom.space(_target) < (int)_items.size() ||!_me->packs.checkMoney( _money) ) {
		return false;
	}

	return true;
}

void TradeOrder::trade() 
{
	bool changed = false;
	for (std::map<DWORD, zObject*>::iterator it=_items.begin(); it!=_items.end(); ++it) {
		_me->packs.removeObject(it->second, false, false); //not delete and not notify
		if (it->second->data.pos.loc() == Cmd::OBJECTCELLTYPE_EQUIP) {
			changed = true;
		}
		it->second->data.exp = 0;
/*		
		it->second->data.pos.dwLocation = Cmd::OBJECTCELLTYPE_COMMON;
		if (_target->packsaddObject(it->second, true)) {
*/
		if (_target->packs.addObject(it->second, true, AUTO_PACK)) {
			zObject::logger(it->second->createid,it->second->data.qwThisID,it->second->data.strName,it->second->data.dwNum,it->second->data.dwNum,0,_me->id,_me->name,_target->id,_target->name,"trade_ok",NULL,0,0);
			Zebra::logger->trace("[交易:玩家<------>玩家]用户%s交易%s给%s成功", _me->name, it->second->data.strName, _target->name);

			Cmd::stAddObjectPropertyUserCmd ret;
			ret.byActionType = Cmd::EQUIPACTION_OBTAIN;
			bcopy(&it->second->data, &ret.object, sizeof(ret.object));
			_target->sendCmdToMe(&ret, sizeof(ret));
		}else {
			zObject::logger(it->second->createid,it->second->data.qwThisID,it->second->data.strName,it->second->data.dwNum,0,0,_me->id,_me->name,_target->id,_target->name,"trade_err",it->second->base,it->second->data.kind,it->second->data.upgrade);
			Zebra::logger->trace("[交易:玩家<------>玩家]用户%s交易%s给%s失败", _me->name, it->second->data.strName, _target->name);
		}
	}

	if (changed) {
		_me->packs.equip.calcAll();
		_me->setupCharBase();
		Cmd::stMainUserDataUserCmd  userinfo;
		_me->full_t_MainUserData(userinfo.data);
		_me->sendCmdToMe(&userinfo,sizeof(userinfo));

		_me->sendMeToNine();
	}
/*	
	zObject* m_gold = _me->packs.getGold();
	m_gold->data.dwNum -= _money;

	if (_money) {
		zObject* t_gold = _target->packs.getGold();
		if (t_gold->base->maxnum - t_gold->data.dwNum > (DWORD)_money) {
			t_gold->data.dwNum += _money;
		}else {
			t_gold->data.dwNum = t_gold->base->maxnum;
		}
	}
*/
	if (_money) {
		_me->packs.removeMoney(_money,"交易");
		_target->packs.addMoney(_money,"交易");
		Zebra::logger->trace("[交易:玩家<------>玩家]用户%s交易银子%d给%s", _me->name, _money, _target->name);
	}
}

bool TradeOrder::canRequest()
{
	return (_target == NULL);
}

void TradeOrder::ready(SceneUser* target)
{
	_target = target;
	_targetid = target->id;
}

bool TradeOrder::canAnswer()
{
	return (_target!=NULL && !begined);
}

void TradeOrder::begin()
{
	begined = true;
}

bool TradeOrder::hasBegin()
{
	return begined;
}

bool TradeOrder::commit()
{
	if(begined)
	{
		commited = true;
		return true;
	}
	return false;
}

void TradeOrder::rollback()
{
	commited = false;
}

bool TradeOrder::hasCommit()
{
	return commited;
}

void TradeOrder::finish()
{
	if (_me->name[0] && _targetid) Zebra::logger->trace("[交易:玩家<------>玩家]%s的交易状态完成", _me->name);
	_targetid=0; 
	_target = NULL;
	_money = 0;
	commited = false;
	lastmove= 0;
	begined = false;

	clear();	
}

void TradeOrder::clear()
{
	_items.clear();
}

void TradeOrder::add_money(DWORD money)
{
	_money = money;
	Zebra::logger->trace("[交易:玩家<------>玩家]用户%s对%s更改交易银子数量(%d)", _me->name, _target->name, money);
}

void TradeOrder::add(zObject* ob)
{
	_items[ob->data.qwThisID] = ob;
	Zebra::logger->trace("[交易:玩家<------>玩家]用户%s对%s添加交易物品%s(%d)" , _me->name, _target->name, ob->data.strName, ob->data.qwThisID);	
}


void TradeOrder::remove(DWORD id)
{
	std::map<DWORD, zObject*>::iterator it = _items.find(id);
	if (it != _items.end()) {
		Zebra::logger->trace("[交易:玩家<------>玩家]用户%s对%s删除交易物品%s(%d)" , _me->name, _target->name, it->second->data.strName, it->second->data.qwThisID);	
		_items.erase(it);
	}
}


bool TradeOrder::in_trade(zObject* ob) const
{
	std::map<DWORD, zObject*>::const_iterator it = _items.find(ob->data.qwThisID);
	
	return it != _items.end();
}

#define ISPROPERTY(value)  if (ob->data.value!=0) ++num

WORD get_prop_num(zObject* ob)
{
	WORD num = 0;

	ISPROPERTY(fivepoint);
	ISPROPERTY(maxmp);
	ISPROPERTY(mvspeed);
	ISPROPERTY(bang);
	ISPROPERTY(hpr);
	ISPROPERTY(mpr);
	ISPROPERTY(spr);
	ISPROPERTY(akspeed);
	ISPROPERTY(pdam);
	ISPROPERTY(pdef);

	ISPROPERTY(mdam);
	ISPROPERTY(mdef);
	ISPROPERTY(atrating);
	ISPROPERTY(akdodge);
	ISPROPERTY(poisondef);
	ISPROPERTY(lulldef);
	ISPROPERTY(reeldef);
	ISPROPERTY(evildef);
	ISPROPERTY(bitedef);
	ISPROPERTY(chaosdef);

	ISPROPERTY(colddef);
	ISPROPERTY(petrifydef);
	ISPROPERTY(blinddef);
	ISPROPERTY(stabledef);
	ISPROPERTY(slowdef);
	ISPROPERTY(luredef);
	ISPROPERTY(skill[0].point);
	ISPROPERTY(skills.point);
	ISPROPERTY(holy);
	ISPROPERTY(hpleech.odds);

	ISPROPERTY(mpleech.odds);
	ISPROPERTY(hptomp);
	ISPROPERTY(dhpp);
	ISPROPERTY(dmpp);
	ISPROPERTY(incgold);
	ISPROPERTY(doublexp);
	ISPROPERTY(mf);
	ISPROPERTY(dpdam);     //物理伤害减少 %x
	ISPROPERTY(dmdam);     //法术伤害减少 %x
	ISPROPERTY(bdam);      //增加伤害 x%
	ISPROPERTY(rdam);      //伤害反射 %x
	ISPROPERTY(ignoredef); //%x 忽视目标防御
	//soke 灵魂锁链
	ISPROPERTY(addpdef);   //物理防御力增强 x
	ISPROPERTY(addmdef);   //魔法防御力增强 x
    ISPROPERTY(addpdam);   //增加物理攻击力 x
    ISPROPERTY(addmdam);   //增加魔法攻击力 x
    ISPROPERTY(addhp);     //增加生命值 x
	ISPROPERTY(poison);
	ISPROPERTY(lull);
	ISPROPERTY(reel);
	ISPROPERTY(evil);
	ISPROPERTY(bite);
	ISPROPERTY(chaos);
	ISPROPERTY(cold);
	ISPROPERTY(petrify);

	ISPROPERTY(blind);
	ISPROPERTY(stable);
	ISPROPERTY(slow);
	ISPROPERTY(lure);

	return num;
}

float get_kind_bonus(BYTE kind)
{
	float bonus = 0.0f;

	if (kind & 0x4)
	{
		bonus = 2.0f;
	}
	else if (kind & 0x2)
	{
		bonus = 1.5f;
	}
	else if (kind & 0x1)
	{
		bonus = 1.2f;
	}
	else
		bonus = 1.0f;

	return bonus;
}

float get_sell_dur_rate(zObject* ob)
{
	if (!ob->data.maxdur) return 1;
	return (float)((ob->data.dur+49)/50) / ((ob->base->durability+49)/50) ;
}


float get_repair_dur_rate(zObject* ob)
{
	if (!ob->data.maxdur) return 1;
	float temp=(float)(((ob->data.maxdur - ob->data.dur)+49)/50) / ((ob->base->durability+49)/50) ;
	//Zebra::logger->debug("get_repair_dur_rate=%f",temp);
	return temp;
	return (float)(((ob->data.maxdur - ob->data.dur)+49)/50) / ((ob->base->durability+49)/50) ;
	//return 1 - get_sell_dur_rate(ob);
}

float get_sell_price(zObject* ob)
{
	float money = 0;
	
	if (!ob) return money;

	money = (0.25*ob->base->price*(1+get_prop_num(ob)*0.1f)*get_kind_bonus(ob->data.kind));

	if (money > 0 ) money += 1;

	//Zebra::logger->debug("get_sell_price=%f",money);
	return money;
}

DWORD get_sell_price_by_dur(zObject* ob)
{
	return (DWORD)(get_sell_price(ob)*get_sell_dur_rate(ob));
}

DWORD get_repair_price(zObject* ob)
{
	DWORD money = 0;
	if (!ob) return money;

	money = (DWORD)(2*get_sell_price(ob)*get_repair_dur_rate(ob));

	return money;
}

#include "luabind/luabind.hpp"
#include "script.h"

bool SceneUser::do_trade_rs_cmd(const Cmd::stTradeUserCmd *rev,unsigned int cmdLen)
{
	Cmd::stRequestSellBuyUserCmd* cmd = (Cmd::stRequestSellBuyUserCmd *)rev;
	
	if(cmd->temp_id == tempid) {
		return true;
	}

	SceneUser *target = scene->getUserByTempID(cmd->temp_id);
	if (!target || target->privatestore.step() != PrivateStore::BEGIN) {
		Zebra::logger->alarm("%s(%ld)请求购买摆摊物品的用户不存在或者没有摆摊", name, id);
		return true;
	}
	if ( abs(pos.x- target->getPos().x) > (SCREEN_WIDTH ) || abs(pos.y-target->getPos().y) > (SCREEN_HEIGHT))  {
		return true;
	}

	PrivateStore::SellInfo* sf = target->privatestore.sell_ob(cmd->object_id);

	if (!sf  || !sf->object() ) {
		Zebra::logger->debug("%s(%ld)请求购买摆摊物品不存在", name, id);
		return true;
	}

	if (packs.uom.space(this) < 1) {
		return Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "包裹空间不足");
	}

	if (packs.checkMoney(sf->money()) && target->packs.removeObject(sf->object(), true, false) ) //notify but not delete) 
	{
		packs.addObject(sf->object(), true, AUTO_PACK);
		//notify client about add
		zObject::logger(sf->object()->createid,sf->object()->data.qwThisID,sf->object()->data.strName,sf->object()->data.dwNum,sf->object()->data.dwNum,0,target->id,target->name,this->id,this->name,"摆摊",NULL,0,0);
		Cmd::stAddObjectPropertyUserCmd ret1;
		ret1.byActionType = Cmd::EQUIPACTION_OBTAIN;
		bcopy(&(sf->object()->data), &ret1.object, sizeof(t_Object));
		sendCmdToMe(&ret1, sizeof(ret1));

		//compute money
		target->packs.addMoney(sf->money(),"摆摊");				
		packs.removeMoney(sf->money(),"摆摊");

		//clear from list
		target->privatestore.remove(cmd->object_id);
	}

	return true;
}

bool SceneUser::doTradeCmd(const Cmd::stTradeUserCmd *rev,unsigned int cmdLen)
{
	using namespace Cmd;
	switch(rev->byParam)
	{
		/// 领赠品品精致升级宝石
		case GOLD_GIVE_USERCMD_PARAMETER:
			{
				Zebra::logger->debug("收到指令");
				stGoldGiveTradeUserCmd *rett = ( Cmd::stGoldGiveTradeUserCmd * )rev;
				Zebra::logger->debug("%d是收到的类型",rett->type);
				if(rett->type == STORN)
				{
					if(this->charbase.goldgive == 0 && this->Card_num>0)
					{
						this->Card_num --;
						if(this->Card_num > 60000)
							this->Card_num = 0;
						this->charbase.goldgive += 70;
						if(this->charbase.goldgive>70)
							this->charbase.goldgive = 70;
					}

					if(this->charbase.goldgive)
					{
						if(this->packs.uom.space(this))
						{
							zObjectB *base = objectbm.get(795);
							if(base)
							{
								//soke 修改支持一键领取全部精致宝石
								zObject *o = zObject::create(base, charbase.goldgive);
							//	zObject *o = zObject::create(base, 1);
								if(o)
								{
									zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,1,1,0,NULL,this->id,this->name,"赠品",o->base,o->data.kind,o->data.upgrade);
									packs.addObject(o, true, AUTO_PACK);
							  //   this->charbase.goldgive--;
									
								    //soke 如果数量大于0，直接清空 兑换相应数量精致升级宝石
									if (this->charbase.goldgive > 0)
								    this->charbase.goldgive = 0;
									o->data.bind=1; //soke 充值领取的精致宝石绑定
									Cmd::stAddObjectPropertyUserCmd ret1;
									ret1.byActionType = Cmd::EQUIPACTION_OBTAIN;
									bcopy(&o->data, &ret1.object, sizeof(t_Object));
									sendCmdToMe(&ret1, sizeof(ret1));

									stReturnGoldGiveTradeUserCmd ret;
                                	ret.Storn_num=this->charbase.goldgive;
                                	ret.Matarial_num=this->Give_MatarialNum;
                                	ret.Card_num=this->Card_num;
                                	sendCmdToMe(&ret, sizeof(ret));
                                					
								}
							}
						}
						else
						{
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你的包裹已满，领取精致升级宝石失败");
						}
					}
				}
				else
				{
					if(this->Give_MatarialNum == 0 && this->Card_num>0)
					{
						this->Card_num --;
						if(this->Card_num >60000)
							this->Card_num = 0;
						this->Give_MatarialNum +=6;
						if(this->Give_MatarialNum > 6)
							this->Give_MatarialNum = 6;
					}

					if(this->Give_MatarialNum)
                                	{
						DWORD Matarial_id = 0;
						switch(rett->type){
						case SIVER :
							{	Matarial_id = 517;}
							break;
						case SILK :
							{	Matarial_id = 507;}
							break;
						case CRYSTAL:
							{	Matarial_id = 527;}
							break;	
						case EBONY :
							{	Matarial_id = 537;}
							break;
						case YINGPI :
							{	Matarial_id = 547;}
							break;
						default :
							break;}
                                       		if(this->packs.uom.space(this))
                                       		{
                                      	         	zObjectB *base = objectbm.get(Matarial_id);
                                        	        if(base)
                                               		{
                                                        	zObject *o = zObject::create(base,50,0);
                                                        	if(o)
                                                        	{
                                                                	zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,1,1,0,NULL,this->id,this->name,"赠品",o->base,o->data.kind,o->data.upgrade);
                                                                	packs.addObject(o, true, AUTO_PACK);
                                                                	this->Give_MatarialNum--;
									if(this->Give_MatarialNum > 6)
										Give_MatarialNum = 0;
                                                                	Cmd::stAddObjectPropertyUserCmd ret1;
                                                                	ret1.byActionType = Cmd::EQUIPACTION_OBTAIN;
                                                                	bcopy(&o->data, &ret1.object, sizeof(t_Object));
                                                                	sendCmdToMe(&ret1, sizeof(ret1));
									
									stReturnGoldGiveTradeUserCmd ret;
                               						ret.Storn_num=this->charbase.goldgive;
                                					ret.Matarial_num=this->Give_MatarialNum;
                                					ret.Card_num=this->Card_num;
                                					sendCmdToMe(&ret, sizeof(ret));
                                					
                                                        	}
                                                	}
                                        	}
						else
						{	
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你的包裹已满，领取材料赠品失败");
						}
					}
				}
				return true;
			}
			break;
		case REQUEST_GOLD_GIVE_USERCMD_PARAMETER:
			{
				stReturnGoldGiveTradeUserCmd ret;
				ret.Storn_num=this->charbase.goldgive;
				ret.Matarial_num=this->Give_MatarialNum;
				ret.Card_num=this->Card_num;
				sendCmdToMe(&ret, sizeof(ret));
				return true;
			}
			break;

		case REQUEST_TRADE_USERCMD_PARAMETER:
			{
				stRequestTradeUserCmd *request=(stRequestTradeUserCmd *)rev;
				if(tradeorder.canRequest())
				{
					if(request->dwAnswerTempID==tempid)
					{
						Zebra::logger->alarm("[交易:玩家<------>玩家]%s(%ld)向自己请求交易",name,id);
						return true;
					}
					SceneUser *pAnswer=scene->getUserByTempID(request->dwAnswerTempID);
					if(pAnswer)
					{
						if (!isset_state(pAnswer->sysSetting , USER_SETTING_TRADE))
						{
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对方交易未开启");
							return true;
						}

						if ( abs(pos.x- pAnswer->getPos().x) > (SCREEN_WIDTH >> 1) || abs(pos.y-pAnswer->getPos().y) > (SCREEN_HEIGHT >> 1))  {
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,  "距离太远，不能交易!");	
							return true;
						}
 	
						if (mask.is_masking() || pAnswer->mask.is_masking()) {
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "蒙面人不可交易 !");	
							return true;
						}

						//mask.on_trade();
						Zebra::logger->trace("[交易:玩家<------>玩家]%s(%ld)请求%s(%ld)交易",name,id,pAnswer->name,pAnswer->id);
						if( pAnswer->tradeorder.canRequest())
						{
							tradeorder.ready(pAnswer);
							pAnswer->tradeorder.ready(this);

							stRequestTradeUserCmd req;
							req.dwAskerTempID = tempid;
							req.dwAnswerTempID = pAnswer->tempid;
							pAnswer->sendCmdToMe(&req,sizeof(req));
							return true;
						}
						else
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对方不能交易接受你的请求",name);
					}
					else
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对方不在!");
				}
				else
					Zebra::logger->alarm("[交易:玩家<------>玩家]%s(%ld)不能请求交易时请求交易",name,id);
				return true;
			}
			break;
		case ANSWER_TRADE_USERCMD_PARAMETER:
			{
				stAnswerTradeUserCmd *answer=(stAnswerTradeUserCmd *)rev;
				if(tradeorder.canAnswer())
				{
					SceneUser *pAsker=tradeorder.target();
					if(!pAsker)
					{
						tradeorder.finish();
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对方不在了");
						return true;
					}

					if ( abs(pos.x- pAsker->getPos().x) > (SCREEN_WIDTH >> 1) || abs(pos.y-pAsker->getPos().y) > (SCREEN_HEIGHT >> 1))  {
						tradeorder.cancel();
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,  "距离太远，不能交易!");	
						return true;
					}

					Zebra::logger->trace("[交易:玩家<------>玩家]%s(%ld)应答%s(%ld)的交易",name,id,pAsker->name,pAsker->id);
					if(pAsker->tradeorder.canAnswer() && pAsker->tradeorder.target()==this)
					{
						if(answer->byAgree)
						{
							tradeorder.begin();
							pAsker->tradeorder.begin();

							stBeginTradeUserCmd begin;
							begin.dwAskerTempID=pAsker->tempid;
							begin.dwAnswerTempID=tempid;

							pAsker->sendCmdToMe(&begin,sizeof(begin));
							sendCmdToMe(&begin,sizeof(begin));

							// 发送现有物品到对方
							//packs.trademyself->sendAllToAnother();
							//pAsker->packs.trademyself->sendAllToAnother();
						}
						else
						{
							tradeorder.finish();
							pAsker->tradeorder.finish();
							Channel::sendSys(pAsker, Cmd::INFO_TYPE_FAIL, "对方不同意和你交易");
						}
					}
					else
						Zebra::logger->alarm("[交易:玩家<------>玩家]%s(%ld)交易应答另一个用户",name,id);
				}
				else
					Zebra::logger->alarm("[交易:玩家<------>玩家]%s(%ld)不能应答交易时应答交易",name,id);
				return true;
			}
			break;
		case COMMIT_TRADE_USERCMD_PARAMETER:
			{
				if(tradeorder.commit())
				{
					SceneUser *pAnother = tradeorder.target();
					if(pAnother)
					{
						if(pAnother->tradeorder.hasCommit())
						{
							//packs.trade();
							//pAnother->packs.trade();
							if (tradeorder.can_trade() && pAnother->tradeorder.can_trade() ) {
								stFinishTradeUserCmd finish;
								pAnother->sendCmdToMe(&finish,sizeof(finish));
								sendCmdToMe(&finish,sizeof(finish));

								tradeorder.trade();
								pAnother->tradeorder.trade();
							}else {
								stCancelTradeUserCmd cancel;
								cancel.dwUserTempID=tempid;
								sendCmdToMe(&cancel,sizeof(cancel));
								pAnother->sendCmdToMe(&cancel,sizeof(cancel));
								Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "%s包裹已满或银子不足，交易失败", 
									tradeorder.can_trade()?name:pAnother->name);
								Channel::sendSys(pAnother, Cmd::INFO_TYPE_FAIL, "%s包裹已满或银子不足，交易失败", 
									tradeorder.can_trade()?name:pAnother->name);
							}
							pAnother->tradeorder.finish();
							tradeorder.finish();
						}
						else
						{
							stCommitTradeUserCmd ci;
							ci.dwUserTempID=tempid;
							pAnother->sendCmdToMe(&ci,sizeof(ci));
							sendCmdToMe(&ci,sizeof(ci));
						}
					}
					else
					{
						tradeorder.finish();
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对方不在了");
						stCancelTradeUserCmd cancel;
						cancel.dwUserTempID=tempid;
						sendCmdToMe(&cancel,sizeof(cancel));
					}
				}
				else
					Zebra::logger->alarm("[交易:玩家<------>玩家]%s(%ld)不能确定交易时确定交易",name,id);
				return true;
			}
			break;
		case CANCEL_TRADE_USERCMD_PARAMETER:
			{
				SceneUser *pAnother=tradeorder.target();
				tradeorder.finish();
				stCancelTradeUserCmd cancel;
				cancel.dwUserTempID=tempid;
				sendCmdToMe(&cancel,sizeof(cancel));

				if(pAnother)
				{
					pAnother->tradeorder.finish();

					cancel.dwUserTempID=pAnother->tempid;
					pAnother->sendCmdToMe(&cancel,sizeof(cancel));
				}
				return true;
			}
			break;
		case ADD_OBJECT_TRADE_USERCMD_PARAMETER:
			{
				stAddObjectTradeUserCmd *cmd = (stAddObjectTradeUserCmd *)rev;
				if (!tradeorder.hasBegin() || tradeorder.hasCommit() ) {
					return true;
				}
				if (cmd->x > TradeOrder::WIDTH || cmd->y > TradeOrder::HEIGHT) {
					Zebra::logger->alarm("[交易:玩家<------>玩家]%s(%ld)请求交易的物品坐标非法",name,id);	
					return true;	
				}

				zObject* ob = packs.uom.getObjectByThisID(cmd->object.qwThisID);
				if (!ob) {
					Zebra::logger->alarm("[交易:玩家<------>玩家]%s(%ld)请求交易的物品不存在",name,id);	
					return true;
				}

				if (ob->base->kind == ItemType_Money && cmd->object.dwNum > ob->data.dwNum  ){
					Zebra::logger->alarm("[交易:玩家<------>玩家]%s(%ld)请求交易的物品数量非法",name,id);	
					return true;
				}

				if (ob->data.bind || ob->data.dwObjectID == 800 || ob->base->kind == ItemType_Quest) {
					Zebra::logger->alarm("[交易:玩家<------>玩家]%s(%ld)用户试图交易不能交易的物品",name,id);						
					return true;
				}

				if (ob->data.pos.loc() != Cmd::OBJECTCELLTYPE_COMMON) {
					Zebra::logger->alarm("[交易:玩家<------>玩家]%s(%ld)用户试图交易不在主包裹中的物品",name,id);
					return true;
				}
					
				SceneUser *pAnother=tradeorder.target();
				if(pAnother && pAnother->tradeorder.hasBegin() && !pAnother->tradeorder.hasCommit())
				{
					if (ob->base->kind ==ItemType_Money) {
						tradeorder.add_money(cmd->object.dwNum);
					}else {
						tradeorder.add(ob);
					}
					
					stAddObjectTradeUserCmd ret;
					ret.user_id = tempid;
					memcpy(&ret.object, &(ob->data), sizeof(ret.object));
					ret.x = cmd->x;
					ret.y = cmd->y;
					if (ob->base->kind == ItemType_Money) {
						ret.object.dwNum = cmd->object.dwNum;
					}
					
					//this->sendCmdToMe(&ret, sizeof(ret));
					pAnother->sendCmdToMe(&ret, sizeof(ret));
				}
				return true;
			}
			break;
		case REMOVE_OBJECT_TRADE_USERCMD_PARAMETER:
			{
				stRemoveObjectTradeUserCmd *cmd = (stRemoveObjectTradeUserCmd *)rev;
				if (!tradeorder.hasBegin() || tradeorder.hasCommit() ) {
					return true;
				}
				
				SceneUser *pAnother=tradeorder.target();
				if(pAnother && pAnother->tradeorder.hasBegin() && !pAnother->tradeorder.hasCommit())
				{
					tradeorder.remove(cmd->object_id);
					cmd->user_id = tempid;

					//this->sendCmdToMe(cmd, sizeof(stRemoveObjectTradeUserCmd));
					pAnother->sendCmdToMe(cmd, sizeof(stRemoveObjectTradeUserCmd));
				}
				return true;
			}
			break;

		case VISITNPC_TRADE_USERCMD_PARAMETER:
			{
                //soke 飞行可以访问NPC
				//if(isFly())
				//{
				//	Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "飞行时无法访问NPC.");
				//	return false;
				//}
				stVisitNpcTradeUserCmd *ptCmd=(stVisitNpcTradeUserCmd *)rev;
				BYTE buf[zSocket::MAX_DATASIZE];
				stVisitNpcTradeUserCmd *cmd=(stVisitNpcTradeUserCmd *)buf;
				bzero(buf, sizeof(buf));
				constructInPlace(cmd);

				//soke 新增随身商店
		        if( ptCmd->dwNpcTempID == 100000000 )
		        {
				//soke 解决是魔头不能购买掉场景问题
				if(this->getGoodnessState() == Cmd::GOODNESS_6)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "随身商店不与大魔头交易.");
					return false;
				}
				//TODO 检查Npc是否在同一个场景，并且检查距离
			    OnVisit event( 5281);
			    EventTable::instance().execute(*this,event);
			    int status;
			    int len = quest_list.get_menu(cmd->menuTxt,status);

			    if (NpcTrade::getInstance().getNpcMenu(5281,cmd->menuTxt+len))
			    {
				//Zebra::logger->debug("%ld\n%s",strlen(cmd->menuTxt),cmd->menuTxt);
				visitNpc( 5281,100000000 );
				cmd->byReturn = 1;
			      }
		        }

				if( ptCmd->dwNpcTempID == 100000004 )
		        {
				
				//TODO 检查Npc是否在同一个场景，并且检查距离
			    OnVisit event( 5141);
			    EventTable::instance().execute(*this,event);
			    int status;
			    int len = quest_list.get_menu(cmd->menuTxt,status);

			    if (NpcTrade::getInstance().getNpcMenu(5141,cmd->menuTxt+len))
			    {
				//Zebra::logger->debug("%ld\n%s",strlen(cmd->menuTxt),cmd->menuTxt);
					visitNpc( 5141,100000004 );
					cmd->byReturn = 1;
					sendCmdToMe(cmd, sizeof(stVisitNpcTradeUserCmd) + strlen(cmd->menuTxt));
					return true;
			      }
		        }

				if( ptCmd->dwNpcTempID == 100000003 )
		        {
				
				//TODO 检查Npc是否在同一个场景，并且检查距离
			    OnVisit event( 5140);
			    EventTable::instance().execute(*this,event);
			    int status;
			    int len = quest_list.get_menu(cmd->menuTxt,status);

			    if (NpcTrade::getInstance().getNpcMenu(5140,cmd->menuTxt+len))
			    {
				//Zebra::logger->debug("%ld\n%s",strlen(cmd->menuTxt),cmd->menuTxt);
					visitNpc( 5140,100000003 );
					cmd->byReturn = 1;
					sendCmdToMe(cmd, sizeof(stVisitNpcTradeUserCmd) + strlen(cmd->menuTxt));
					return true;
			      }
		        }

				//soke 新增随身商店
		        if( ptCmd->dwNpcTempID == 100000002 )
		        {
				
				//TODO 检查Npc是否在同一个场景，并且检查距离
			    OnVisit event( 5164);
			    EventTable::instance().execute(*this,event);
			    int status;
			    int len = quest_list.get_menu(cmd->menuTxt,status);

			    if (NpcTrade::getInstance().getNpcMenu(5164,cmd->menuTxt+len))
			    {
				//Zebra::logger->debug("%ld\n%s",strlen(cmd->menuTxt),cmd->menuTxt);
					visitNpc( 5164,100000002 );
					cmd->byReturn = 1;
					sendCmdToMe(cmd, sizeof(stVisitNpcTradeUserCmd) + strlen(cmd->menuTxt));
					return true;
			      }
		        }


				SceneNpc *sceneNpc = SceneNpcManager::getMe().getNpcByTempID( ptCmd->dwNpcTempID);
                if ( (sceneNpc && this->canVisitNpc(sceneNpc)) /*|| ( sceneNpc->id == 5281 )*/ )/*( (sceneNpc->id>=5000&&sceneNpc->id<=6000)  || (sceneNpc->scene && (sceneNpc->scene->getCountryID() == charbase.country || changeface)) ) */
				{
					//TODO 检查Npc是否在同一个场景，并且检查距离
					OnVisit event(sceneNpc->id);
					EventTable::instance().execute(*this, event);
					int status;
					int len = quest_list.get_menu(cmd->menuTxt, status);
					
					//Zebra::logger->debug("TODO 检查Npc(%lu)是否在同一个场景，并且检查距离",sceneNpc->id);
		        if ( (sceneNpc->scene && sceneNpc->scene == this->scene && 
			  this->scene->zPosShortRange(this->getPos(),sceneNpc->getPos(),SCREEN_WIDTH,SCREEN_HEIGHT)) /*|| ( sceneNpc->id == 5281 )*/ )
					{		
						if (NpcTrade::getInstance().getNpcMenu(sceneNpc->id, cmd->menuTxt+len))
						{
							//Zebra::logger->debug("%ld\n%s",strlen(cmd->menuTxt),cmd->menuTxt);
							visitNpc(sceneNpc->id, sceneNpc->tempid);
							cmd->byReturn = 1;
						}

						if (ScriptQuest::get_instance().has(ScriptQuest::NPC_VISIT, sceneNpc->id)) { 
							char func_name[32];
							sprintf(func_name, "%s_%d", "visit", sceneNpc->id);
							if (execute_script_event(this,func_name, sceneNpc)) return true;
						}
						
					}
					else
					{
						Zebra::logger->iffy("用户%s(%d)检查Npc(%lu)距离不合法(%u,%d,%d),(%u,%d,%d)",this->name,this->id,sceneNpc->id,this->scene->id,this->getPos().x,this->getPos().y,sceneNpc->scene->id,sceneNpc->getPos().x,sceneNpc->getPos().y);
					}
//					Zebra::logger->debug("动态菜单(%s)", cmd->menuTxt);
				}
				else
					Zebra::logger->error("%s(%d)访问不能访问的Npc",this->name,this->id);

				sendCmdToMe(cmd, sizeof(stVisitNpcTradeUserCmd) + strlen(cmd->menuTxt));

				return true;
			}
			break;
			
		//soke 2016 - 3 - 10 工资购买
		case BUYOBJECT_SALARYTRADE_USERCMD_PARAMETER:
			{
			//	this->charbase.honor -= price*count;
				if(this->charbase.level <60)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您的等级低于60级,还不能使用工资");
					return false;
				}
				else
				{
				    stBuyObjectSalaryTradeUserCmd *ptCmd=(stBuyObjectSalaryTradeUserCmd *)rev;
				    zObjectB *base = objectbm.get(ptCmd->dwObjectID);
				    //ptCmd->itemLevel = 0; //soke 注释掉购买材料等级才正常
				    ///如果需要用工资买
				    if(base)
				    {
					    if(base->cointype & eBuySalary)
						this->npcTradeSalary(ptCmd, base, ptCmd->itemLevel);
				    }
					return true;
				}
			}
			break;
        //soke 2023 - 8 - 1 积分点数购买
		case BUYOBJECT_TICKETTRADE_USERCMD_PARAMETER:
			{
			//	this->charbase.honor -= price*count;
				if(this->charbase.level <60)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您的等级低于60级,还不能使用积分");
					return false;
				}
				else
				{
				    stBuyObjectTicketTradeUserCmd *ptCmd=(stBuyObjectTicketTradeUserCmd *)rev;
				    zObjectB *base = objectbm.get(ptCmd->dwObjectID);
				    //ptCmd->itemLevel = 0; //soke 注释掉购买材料等级才正常
				    ///如果需要用积分点数买
				    if(base)
				    {
					    if(base->cointype & eBuyTicket)
						this->npcTradeTicket(ptCmd, base, ptCmd->itemLevel);
				    }
					return true;
				}
			}
			break;
        //soke 2023 - 10 - 10 充值点数购买
		case BUYOBJECT_CHONGZHITRADE_USERCMD_PARAMETER:
			{
			//	this->charbase.honor -= price*count;
				if(this->charbase.level <60)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您的等级低于60级,还不能使用充值点");
					return false;
				}
				else
				{
				    stBuyObjectChongzhiTradeUserCmd *ptCmd=(stBuyObjectChongzhiTradeUserCmd *)rev;
				    zObjectB *base = objectbm.get(ptCmd->dwObjectID);
				    //ptCmd->itemLevel = 0; //soke 注释掉购买材料等级才正常
				    ///如果需要用充值点数买
				    if(base)
				    {
					    if(base->cointype & eBuyChongzhi) //点数类型
						this->npcTradeChongzhi(ptCmd, base, ptCmd->itemLevel);
				    }
					return true;
				}
			}
			break;			
		 //soke 2023 - 10 - 15新商城点数购买	
		case BUYOBJECT_MALL_USERCMD_PARAMETER:
			{
				stBuyObjectMallUserCmd *ptCmd=(stBuyObjectMallUserCmd *)rev; //点数类型
				zObjectB *base= objectbm.get(ptCmd->dwObjectID);
				//ptCmd->itemLevel = 0; //soke 注释掉购买材料等级才正常
				///如果需要用新点数买
				if(base)
				{
					if(base->cointype & eBuyChongzhi) //点数类型
						this->MallTradeObject(ptCmd,base);
				}
				
				return true;
			}
			break;
		///////////////////////////////////////////////				
		case BUYOBJECT_NPCTRADE_USERCMD_PARAMETER:
			{
				stBuyObjectNpcTradeUserCmd *ptCmd=(stBuyObjectNpcTradeUserCmd *)rev;
				zObjectB *base = objectbm.get(ptCmd->dwObjectID);
				//ptCmd->itemLevel = 0; //soke 注释掉购买材料等级才正常
				///如果需要用金子或者银两买
				if(base)
				{
					if(base->cointype & eBuyGold)
					{
						this->npcTradeGold(ptCmd, base, ptCmd->itemLevel);
						return true;
					}
				}
				SceneNpc * n = SceneNpcManager::getMe().getNpcByTempID(npc_dwNpcTempID);
				if (!n && npc_dwNpcTempID != 100000000 )
				{
					Zebra::logger->debug("[交易:玩家<------商店]%s 交易时，找不到该npc tempID=%u", name, npc_dwNpcTempID);
					return true;
				}

				if((getGoodnessState() != Cmd::GOODNESS_6) || ((getGoodnessState()==Cmd::GOODNESS_6) && (NPC_TYPE_MOBILETRADE==n->npc->kind)) )
				{
					if (base)
					{
						//DWORD price = (DWORD )(this->getGoodnessPrice(base->price , true)+0.99f);
						//soke 商店银子价格
						float levelopt = pow(5,ptCmd->itemLevel);
						DWORD price = (DWORD )(this->getGoodnessPrice(base->price*(DWORD)((float)levelopt) , true)+0.99f);
						if (this->scene->getCountryID() == PUBLIC_COUNTRY)
						{
							//在公共国买东西，价格上涨10%
							price = price + (DWORD)(price*0.1);
						}

						if (ptCmd->dwNum == 0) 
						{
							ptCmd->dwNum = 1;
						}
						if (ptCmd->dwNum > base->maxnum) 
						{
							ptCmd->dwNum = base->maxnum;
						}

						DWORD need = price*ptCmd->dwNum;
						int dayssize=packs.store.days.size();
						if(this->charbase.bitmask & CHARBASE_VIP)
						{
							dayssize=dayssize?(dayssize - 1):dayssize;
						}
						if (base->kind == ItemType_Store) 
						{
							need = price*base->durability;
							if (dayssize==1)  //the second page
								need = 2000;
							if (dayssize==2)  //the third page
								need = 100000;
						}

						if (base->kind == ItemType_HORSE && charbase.level<30)
						{
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你还没满30级，不能购买马匹");
							return true;
						}
                        
			            DWORD taxMoney;
			            if( npc_dwNpcTempID == 100000000 )
			            {
							taxMoney = (DWORD)((price*ptCmd->dwNum/100.0f)+0.5f);
							
							//soke 随身商店 银子购买
						    if( this->charbase.money >= need+taxMoney )
			                {				             
						        if((base->kind == ItemType_DoubleExp && this->charbase.honor >= need) ||							      
							        (base->kind != ItemType_DoubleExp && packs.checkMoney(need+taxMoney)))
						        {
							        NpcTrade::NpcItem item;
							        item.id = base->id;
							        item.kind = base->kind;
							        item.lowLevel = 0;
							        item.level = base->needlevel;
							        item.itemlevel = ptCmd->itemLevel;
							        item.action = NpcTrade::NPC_BUY_OBJECT;
							        if (NpcTrade::getInstance().verifyNpcAction(npc_dwNpcDataID, item))
							        {								        
								        zObject* o = NULL;
								        if (base->recast)
								        {
									        o = zObject::create(base, ptCmd->dwNum, 0);
									        if (o && ptCmd->itemLevel)
									        {
										        do
										        {
											        //Upgrade::upgrade(*this, o, 100); //must success
										        }
										        while (--ptCmd->itemLevel >0);
									        }
								        }
								        else
								        {
									        o = zObject::create(base, ptCmd->dwNum, ptCmd->itemLevel);
								        }
								        if(o)
								        {
									        Combination callback(this, o);
									        packs.main.execEvery(callback);
									        if (packs.equip.pack(EquipPack::L_PACK)) packs.equip.pack(EquipPack::L_PACK)->execEvery(callback);
									        if (packs.equip.pack(EquipPack::R_PACK)) packs.equip.pack(EquipPack::R_PACK)->execEvery(callback);

									        int free = 0;
									        if (o->data.dwNum) 
									        {
										        if (packs.addObject(o, true, AUTO_PACK)) 
										        {
											        free = o->data.dwNum;
											        //soke 银两买物品绑定
											        o->data.bind=1;
											        Cmd::stAddObjectPropertyUserCmd status;
											        status.byActionType = Cmd::EQUIPACTION_OBTAIN;
											        bcopy(&o->data, &status.object, sizeof(t_Object));
											        sendCmdToMe(&status, sizeof(status));
										        }
									        }

									        if (callback.num() || free) 
									        {
										        //get object
										        int count = callback.num() + free;
										        if(base->kind == ItemType_DoubleExp)
										        {
											       this->charbase.honor -= price*count;
											       if((int)this->charbase.honor <0)
											       {
												       Zebra::logger->fatal("[交易:玩家<------随身商店]用户(%s)买%s时荣誉点数计算错误!",name, o->base->name);
												       this->charbase.honor=0;
											       }
											       Cmd::stMainUserDataUserCmd  userinfo;
											       full_t_MainUserData(userinfo.data);
											       sendCmdToMe(&userinfo,sizeof(userinfo));
											       zObject::logger(0,0,"荣誉值",this->charbase.honor,price*count,0,this->id,this->name,0,NULL,"买东西扣除荣誉值",NULL,0,0);
											       zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,count,1,n->id,n->name,this->id,this->name,"buy_npc",o->base,o->data.kind,o->data.upgrade);
											       Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"得到物品 %s(%d)个 , 花费荣誉点数%u",o->name , count, price*count);
											       Zebra::logger->trace("[交易:玩家<------随身商店]用户(%s,%u)买%s(%d)个花费荣誉点%d",name , id , o->name , count ,    price*count);
										        }
										        else 
										        {
							                       DWORD taxMoney = (DWORD)((price*count/100.0f)+0.5f);

										           if (!packs.removeMoney(price*count+taxMoney,"买东西"))
										           {
										               Zebra::logger->fatal("[交易:玩家<------随身商店]用户(%s)买%s时银子计算错误!", name, o->base->name);
										           }
											       Cmd::Session::t_taxAddCountry_SceneSession send;

						                           send.dwCountryID = 2;
											       send.qwTaxMoney = taxMoney;
											       sessionClient->sendCmd(&send, sizeof(send));
						                           Zebra::logger->trace("[交易:玩家<------随身商店]用户(%s,%u)买%s(%d)个花费银子%d",name,id,o->name,count,price*count+taxMoney);
											       Channel::sendMoney(this, Cmd::INFO_TYPE_GAME, price*count+taxMoney,"得到物品 %s(%d)个 , 花费银子",o->name ,count);
										        }
									        } 

									        if (!free) 
									        {   //package is full
										        //Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "你的包裹已满");
										        zObject::destroy(o);
									        } 									
								        }
							        }
							        else
								        Zebra::logger->error("[交易:玩家<------随身商店]不能在这里买这样物品 %u, %u, %s", npc_dwNpcDataID, base->id, base->name);
						        }
								else
						        {
							       if(base->kind == ItemType_DoubleExp)
							       {
								      Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你的荣誉点数不够买这件物品");
							       }
						        }
						    }
						    else //soke 随身商店 银子购买
			                { 
						        if((base->kind == ItemType_DoubleExp && this->charbase.honor >= need) ||
							        (base->kind != ItemType_DoubleExp && packs.checkMoney(need+taxMoney)))
						        {
							        NpcTrade::NpcItem item;
							        item.id = base->id;
							        item.kind = base->kind;
							        item.lowLevel = 0;
							        item.level = base->needlevel;
							        item.itemlevel = ptCmd->itemLevel;
							        item.action = NpcTrade::NPC_BUY_OBJECT;
							        if (NpcTrade::getInstance().verifyNpcAction(npc_dwNpcDataID, item))
							        {
								        zObject* o = NULL;
								        if (base->recast)
								        {
									        o = zObject::create(base, ptCmd->dwNum, 0);
									        if (o && ptCmd->itemLevel)
									        {
										        do
										        {
											        //Upgrade::upgrade(*this, o, 100); //must success
										        }
										        while (--ptCmd->itemLevel >0);
									        }
								        }
								        else
								        {
									        o = zObject::create(base, ptCmd->dwNum, ptCmd->itemLevel);
								        }
								        if(o)
								        {
									        Combination callback(this, o);
									        packs.main.execEvery(callback);
									        if (packs.equip.pack(EquipPack::L_PACK)) packs.equip.pack(EquipPack::L_PACK)->execEvery(callback);
									        if (packs.equip.pack(EquipPack::R_PACK)) packs.equip.pack(EquipPack::R_PACK)->execEvery(callback);

									        int free = 0;
									        if (o->data.dwNum) 
									        {

										        if (packs.addObject(o, true, AUTO_PACK)) 
										        {
											        free = o->data.dwNum;
											        //如果是双倍经验道具和荣誉道具需要绑定
											        o->checkBind();
											        Cmd::stAddObjectPropertyUserCmd status;
											        status.byActionType = Cmd::EQUIPACTION_OBTAIN;
											        bcopy(&o->data, &status.object, sizeof(t_Object));
											        sendCmdToMe(&status, sizeof(status));
										        }
									        }

									        if (callback.num() || free) 
									        {
										        //get object
										        int count = callback.num() + free;
										        if(base->kind == ItemType_DoubleExp)
										        {
											       this->charbase.honor -= price*count;
											       if((int)this->charbase.honor <0)
											       {
												       Zebra::logger->fatal("[交易:玩家<------随身商店]用户(%s)买%s时荣誉点数计算错误!",name, o->base->name);
												       this->charbase.honor=0;
											       }
											       Cmd::stMainUserDataUserCmd  userinfo;
											       full_t_MainUserData(userinfo.data);
											       sendCmdToMe(&userinfo,sizeof(userinfo));
											       zObject::logger(0,0,"荣誉值",this->charbase.honor,price*count,0,this->id,this->name,0,NULL,"买东西扣除荣誉值",NULL,0,0);
											       zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,count,1,n->id,n->name,this->id,this->name,"buy_npc",o->base,o->data.kind,o->data.upgrade);
											       Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"得到物品 %s(%d)个 , 花费荣誉点数%u",o->name , count, price*count);
											       Zebra::logger->trace("[交易:玩家<------随身商店]用户(%s,%u)买%s(%d)个花费荣誉点%d",name , id , o->name , count , price*count);
										        }
										        else 
										        {
							                       DWORD taxMoney = (DWORD)((price*count/100.0f)+0.5f);

										           if (!packs.removeMoney(price*count+taxMoney,"买东西"))
										           {
										               Zebra::logger->fatal("[交易:玩家<------随身商店]用户(%s)买%s时银子计算错误!", name, o->base->name);
										           }
											       Cmd::Session::t_taxAddCountry_SceneSession send;

						                           send.dwCountryID = 2;
											       send.qwTaxMoney = taxMoney;
											       sessionClient->sendCmd(&send, sizeof(send));
						                           Zebra::logger->trace("[交易:玩家<------随身商店]用户(%s,%u)买%s(%d)个花费银子%d",name,id,o->name,count,price*count+taxMoney);
											       Channel::sendMoney(this, Cmd::INFO_TYPE_GAME, price*count+taxMoney,"得到物品 %s(%d)个 , 花费银子",o->name ,count);
										        }
									        } 

									        if (!free) 
									        {   //package is full
										        //Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "你的包裹已满");
										        zObject::destroy(o);
									        } 									
								        }
							        }
							        else
								        Zebra::logger->error("[交易:玩家<------随身商店]不能在这里买这样物品 %u, %u, %s", npc_dwNpcDataID, base->id, base->name);
						        }
							    else
						        {
							       if(base->kind == ItemType_DoubleExp)
							       {
								      Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你的荣誉点数不够买这件物品");
							       }
							       else
							       {
								      Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你的银子不够买这件物品");
							       }
						        }
						    }
			            }
			            else
						{   
					        //soke 银子商店
							taxMoney = (DWORD)((price*ptCmd->dwNum*n->scene->getTax()/100.0f)+0.5f); // 买东西收税
						
						    if((base->kind == ItemType_DoubleExp && this->charbase.honor >= need) ||
							    (base->kind == ItemType_HORSE && packs.checkMoney(need))||
							    (base->kind == ItemType_Store && packs.checkMoney(need))||
							    (base->kind != ItemType_DoubleExp && packs.checkMoney(need+taxMoney)))
						    {
							    NpcTrade::NpcItem item;
							    item.id = base->id;
							    item.kind = base->kind;
							    item.lowLevel = 0;
							    item.level = base->needlevel;
							    item.itemlevel = ptCmd->itemLevel;
							    item.action = NpcTrade::NPC_BUY_OBJECT;
							    if (NpcTrade::getInstance().verifyNpcAction(npc_dwNpcDataID, item))
							    {
								    if (base->kind == ItemType_HORSE) 
								    {
									    if (horse.horse()) 
									    {
										    return Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "你已经有马了!");
									    }

									    if (packs.removeMoney(need,"买东西")) 
									    {
										    horse.horse(base->id);

										    Channel::sendSys(this , Cmd::INFO_TYPE_GAME, "你得到一匹枣红马");
										    Zebra::logger->trace("[交易:玩家<------银子商店]用户(%s,%u)买马(%d)花费银子%d",name , id ,base->id, need);
									    }
									    else 
									    {
										    Zebra::logger->fatal("[交易:玩家<------银子商店]用户(%s)买马时银子计算错误!", name);
									    }
									    return true;
								    }

								    if (base->kind == ItemType_Store) 
								    {
									    if ((dayssize==0 && charbase.level<20)
										    ||(dayssize==1 && charbase.level<40))
										  //||(dayssize==2 && charbase.level<80))
										    return Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "你的等级不够买下一个储物箱!");
									    if (dayssize>1)
										    return Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "已经买了两个储物箱了，无法再购买!");

									    if (packs.removeMoney(need,"买东西") ) 
									    {
										    Zebra::logger->trace("[交易:玩家<------银子商店]用户(%s,%u)买储物箱花费银子%d",name , id , need);
										    Channel::sendMoney(this, Cmd::INFO_TYPE_GAME, need,"买储物箱花费银子");

										    packs.store.days.push_back(base->durability);
										    packs.store.notify(this);
										    //检查是否需要赠送第三个包裹
										    //packs.store.goldstore(this);
									    }
									    else 
									    {
										    Zebra::logger->fatal("[交易:玩家<------银子商店]用户(%s)买储物箱时银子计算错误!", name);
									    }
									    return true;
								    }
								    zObject* o = NULL;
								    if (base->recast)
								    {
									    o = zObject::create(base, ptCmd->dwNum, 0);
									    if (o && ptCmd->itemLevel)
									    {
										    do
										    {
											    //Upgrade::upgrade(*this, o, 100); //must success
										    }
										    while (--ptCmd->itemLevel >0);
									    }
								    }
								    else
								    {
									    o = zObject::create(base, ptCmd->dwNum, ptCmd->itemLevel);
								    }
								    if(o)
								    {
									    Combination callback(this, o);
									    packs.main.execEvery(callback);
									    if (packs.equip.pack(EquipPack::L_PACK)) packs.equip.pack(EquipPack::L_PACK)->execEvery(callback);
									    if (packs.equip.pack(EquipPack::R_PACK)) packs.equip.pack(EquipPack::R_PACK)->execEvery(callback);

									    int free = 0;
									    if (o->data.dwNum) 
									    {
										    if (packs.addObject(o, true, AUTO_PACK)) 
										    {
											    free = o->data.dwNum;
											    //如果是双倍经验道具和荣誉道具需要绑定
											    o->checkBind();
											    Cmd::stAddObjectPropertyUserCmd status;
											    status.byActionType = Cmd::EQUIPACTION_OBTAIN;
											    bcopy(&o->data, &status.object, sizeof(t_Object));
											    sendCmdToMe(&status, sizeof(status));
										    }
									    }

									    if (callback.num() || free) 
									    {
										    //get object
										    int count = callback.num() + free;
										    if(base->kind == ItemType_DoubleExp)
										    {
											    this->charbase.honor -= price*count;
											    if((int)this->charbase.honor <0)
											    {
												    Zebra::logger->fatal("[交易:玩家<------荣誉商店]用户(%s)买%s时荣誉点数计算错误!",name, o->base->name);
												    this->charbase.honor=0;
											    }
											    Cmd::stMainUserDataUserCmd  userinfo;
											    full_t_MainUserData(userinfo.data);
											    sendCmdToMe(&userinfo,sizeof(userinfo));
											    zObject::logger(0,0,"荣誉值",this->charbase.honor,price*count,0,this->id,this->name,0,NULL,"买东西扣除荣誉值",NULL,0,0);
											    zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,count,1,n->id,n->name,this->id,this->name,"buy_npc",o->base,o->data.kind,o->data.upgrade);
											    Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"得到物品 %s(%d)个 , 花费荣誉点数%u",o->name , count, price*count);
											    Zebra::logger->trace("[交易:玩家<------荣誉商店]用户(%s,%u)买%s(%d)个花费荣誉点%d",name , id , o->name , count , price*count);
										    }
										    else 
										    {						                   
										        DWORD taxMoney = (DWORD)((price*count*n->scene->getTax()/100.0f)+0.5f); // 买东西收税
										  
										        if (!packs.removeMoney(price*count+taxMoney,"买东西"))
										        {
										            Zebra::logger->fatal("[交易:玩家<------银子商店]用户(%s)买%s时银子计算错误!", name, o->base->name);
										        }
											    Cmd::Session::t_taxAddCountry_SceneSession send;                                           
											    send.dwCountryID = n->scene->getCountryID();
											    send.qwTaxMoney = taxMoney;
											    sessionClient->sendCmd(&send, sizeof(send));
											    zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,count,1,n->id,n->name,this->id,this->name,"buy_npc",o->base,o->data.kind,o->data.upgrade);
											    Channel::sendMoney(this, Cmd::INFO_TYPE_GAME, price*count+taxMoney,"得到物品 %s(%d)个 , 花费银子",o->name , count);
											    Zebra::logger->trace("[交易:玩家<------银子商店]用户(%s,%u)买%s(%d)个花费银子%d",name , id , o->name , count , price*count+taxMoney);
										    }
									    } 
									    if (!free) 
									    {   //package is full
										    //Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "你的包裹已满");
										    zObject::destroy(o);
									    }									
								    }
							    }
							    else
								    Zebra::logger->error("[交易:玩家<------银子商店]不能在这里买这样物品 %u, %u, %s", npc_dwNpcDataID, base->id, base->name);
						    }
						    else
						    {
							    if(base->kind == ItemType_DoubleExp)
							    {
								    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你的荣誉点数不够买这件物品");
							    }
							    else
							    {
								    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你的银子不够买这件物品");
							    }
						    }
					    }
					}
				}
				else
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "本人不与大魔头交易");
				return true;
			}
			break;
		case SELLOBJECT_NPCTRADE_USERCMD_PARAMETER:
			{
				stSellObjectNpcTradeUserCmd *ptCmd=(stSellObjectNpcTradeUserCmd *)rev;
				zObject *srcobj=packs.uom.getObjectByThisID(ptCmd->qwThisID);
				if(srcobj)
				{
					if(this->getGoodnessState() != Cmd::GOODNESS_6)
					{
						//交易处理
						if(tradeorder.hasBegin() && tradeorder.in_trade(srcobj))
						{
							return true;
						}


						if (mask.is_use(srcobj)) 
						{
							return Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "请先解除该蒙面巾!");
						}

						if (srcobj->base->kind == ItemType_Quest) 
						{
							return Channel::sendSys(this,  Cmd::INFO_TYPE_FAIL, "任务物品不可买卖");
						};
						NpcTrade::NpcItem item;
						item.id = srcobj->data.dwObjectID;
						item.kind = srcobj->base->kind;
						item.lowLevel = 0;
						item.level = srcobj->data.needlevel;
						item.action = NpcTrade::NPC_SELL_OBJECT;
						if (NpcTrade::getInstance().verifyNpcAction(npc_dwNpcDataID, item))
						{
							//soke 卖东西要收钱（积分）
							if(srcobj->data.price>0  && srcobj->base->kind != ItemType_SOULSTONE)
							{
								DWORD price = 0;
								DWORD real_price = 0;
								if(srcobj->base->id == 655)
								{
									if(srcobj->base->durability)
									{
										real_price = (DWORD)(((float)((srcobj->data.dur+49)/50.0f)/((srcobj->base->durability+49)/50.0f)) * 400.0f);
									}
								}
								else if(srcobj->base->id == 685) //魔力之源
								{
									if(srcobj->base->durability)
									{
										real_price = (DWORD)(((float)((srcobj->data.dur+49)/50.0f)/((srcobj->base->durability+49)/50.0f)) * 400.0f);
									}
								}
								else if(srcobj->base->id == 760) //洗髓宝珠
								{
									//if(srcobj->base->durability)
									//{
										real_price = (DWORD)(((float)((srcobj->data.dur+49)/50.0f)/((srcobj->base->durability+49)/50.0f)) * 2000.0f); //20两积分
									//}
								}
								else if(srcobj->base->id == 761) // 易筋宝珠
								{
									//if(srcobj->base->durability)
									//{
										real_price = (DWORD)(((float)((srcobj->data.dur+49)/50.0f)/((srcobj->base->durability+49)/50.0f)) * 2000.0f); //20两积分
									//}
								}
								else if(srcobj->base->id == 882) // 龙之魔力（蓝-13）
								{
									if(srcobj->base->durability)
									{
										real_price = (DWORD)(((float)((srcobj->data.dur+49)/50.0f)/((srcobj->base->durability+49)/50.0f)) * 20000.0f);
									}
								}
								else if(srcobj->base->id == 930) // 神之魔力（蓝-13）
								{
									if(srcobj->base->durability)
									{
										real_price = (DWORD)(((float)((srcobj->data.dur+49)/50.0f)/((srcobj->base->durability+49)/50.0f)) * 30000.0f);
									}
								}
								else if(srcobj->base->id == 861) // 天之魔力（蓝-13）
								{
									if(srcobj->base->durability)
									{
										real_price = (DWORD)(((float)((srcobj->data.dur+49)/50.0f)/((srcobj->base->durability+49)/50.0f)) * 40000.0f);
									}
								}
								else if(srcobj->base->id == 881) // 龙之生命（包裹回血）
								{
									if(srcobj->base->durability)
									{
										real_price = (DWORD)(((float)((srcobj->data.dur+49)/50.0f)/((srcobj->base->durability+49)/50.0f)) * 2000.0f);
									}
								}
								else if(srcobj->base->id == 929) // 神之生命（包裹回血）
								{
									if(srcobj->base->durability)
									{
										real_price = (DWORD)(((float)((srcobj->data.dur+49)/50.0f)/((srcobj->base->durability+49)/50.0f)) * 3000.0f);
									}
								}
								else if(srcobj->base->id == 860) // 天之生命（包裹回血）
								{
									if(srcobj->base->durability)
									{
										real_price = (DWORD)(((float)((srcobj->data.dur+49)/50.0f)/((srcobj->base->durability+49)/50.0f)) * 4000.0f);
									}
								}
								else if(srcobj->base->id == 874) // 龙之活力（血-13）
								{
									if(srcobj->base->durability)
									{
										real_price = (DWORD)(((float)((srcobj->data.dur+49)/50.0f)/((srcobj->base->durability+49)/50.0f)) * 2000.0f);
									}
								}
								else if(srcobj->base->id == 942) // 神之活力（血-13）
								{
									if(srcobj->base->durability)
									{
										real_price = (DWORD)(((float)((srcobj->data.dur+49)/50.0f)/((srcobj->base->durability+49)/50.0f)) * 3000.0f);
									}
								}
								else if(srcobj->base->id == 953) // 天之活力（血-13）
								{
									if(srcobj->base->durability)
									{
										real_price = (DWORD)(((float)((srcobj->data.dur+49)/50.0f)/((srcobj->base->durability+49)/50.0f)) * 4000.0f);
									}
								}
								else if(srcobj->base->id == 1272) // 幸运丹
								{
									if(srcobj->base->durability)
									{
										real_price = (DWORD)(((float)((srcobj->data.dur+49)/50.0f)/((srcobj->base->durability+49)/50.0f)) * 2000.0f);
									}
								}
								else
								{
									price = get_sell_price_by_dur(srcobj);

									if(srcobj->base->maxnum && (srcobj->base->kind == ItemType_Arrow/* || srcobj->base->kind == ItemType_Arrow2*/)) //箭支类
									{
										if(srcobj->base->durability)
										{
											real_price = (DWORD)(getGoodnessPrice((srcobj->data.dwNum * (price)),false));
										}
									}
									else
									{
										if(srcobj->base->durability)
										{
											real_price = (DWORD)(getGoodnessPrice((srcobj->data.dwNum * (price)),false));
										}
										else
										{
											real_price = (DWORD)(getGoodnessPrice((srcobj->data.dwNum * (price)),false));
										}
									}
								}
								//soke 卖物品获得银子
							    Channel::sendMoney(this, Cmd::INFO_TYPE_GAME, real_price, "卖%s得到银子", srcobj->name );
							    Zebra::logger->trace("[交易:玩家------>商店]用户(%s,%u)卖%s得到银子%d", name, id, srcobj->name, real_price);
							    packs.addMoney(real_price,"卖东西");
							}

							//soke 卖物品给金子
							else if(srcobj->data.price>0 && srcobj->base->kind == ItemType_SOULSTONE)
							{
								//DWORD price = 0;
								DWORD real_price = 0;
								if(srcobj->base->id == 680 || (srcobj->base->id >= 748 && srcobj->base->id <= 751))
								{
									real_price = (DWORD)(float)(srcobj->data.dwNum * 50.0f);
								}

								//soke 卖魂魄石获得金子
								Zebra::logger->trace("[交易:玩家------>商店]用户(%s,%u)卖%s得到金子%d", name, id, srcobj->name, real_price);
								packs.addGold(real_price,"卖东西");
							}

							//soke 卖装备给经验
							if (srcobj->data.exp && srcobj->base->kind != ItemType_Pack ) 
							{
								Zebra::logger->trace("[交易:玩家------>商店]%s(%u) 卖装备增加经验 %u", name, id, srcobj->data.exp);
							//	addExp(srcobj->data.exp);
								addExp(1); //soke 1点经验
							}

							zObject::logger(srcobj->createid,srcobj->data.qwThisID,srcobj->base->name,srcobj->data.dwNum,srcobj->data.dwNum,0,npc_dwNpcDataID,NULL,this->id,this->name,"sell_npc",srcobj->base,srcobj->data.kind,srcobj->data.upgrade);
							packs.removeObject(srcobj); //notify and delete
							if(packs.equip.needRecalc)
							{
								notifyEquipChange();
								setupCharBase();
								Cmd::stMainUserDataUserCmd  userinfo;
								full_t_MainUserData(userinfo.data);
								sendCmdToMe(&userinfo,sizeof(userinfo));

								sendMeToNine();
							}

						}
						else
						{
							Zebra::logger->alarm("[交易:玩家------>商店]不能在这里卖这样物品 %u, %u, %s",
									npc_dwNpcDataID, srcobj->data.dwObjectID, srcobj->data.strName);
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，小店不收购%s",srcobj->base->name);
						}
					}
					else
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "本人不与大魔头交易");
				}
				return true;
			}
			break;
		case SELLHORSE_NPCTRADE_USERCMD_PARAMETER:
			{
				stSellHorseNpcTradeUserCmd *ptCmd=(stSellHorseNpcTradeUserCmd *)rev;

				//放逐
				if (ptCmd->action) { 
					if (horse.horse()) {
						horse.mount(false);
						horse.putAway();
						horse.horse(0);
						Zebra::logger->trace("[卖马]%s 放逐了他的马", name);
					}

					return true;
				}

				//卖
				if(this->getGoodnessState() != Cmd::GOODNESS_6)
				{
					if(!horse.horse()) 
					{
						return Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你还没有马匹，卖什么马?");
					}

					zObjectB *base = objectbm.get(horse.horse());
					if (!base) 
					{
						Zebra::logger->warn("[卖马]用户马匹在道具表中不存在,请检查道具基本表!");
						return true;
					}
					NpcTrade::NpcItem item;
					item.id = base->id;
					item.kind = base->kind;
					item.lowLevel = 0;
					item.level = base->needlevel;
					item.action = NpcTrade::NPC_SELL_OBJECT;
					if (NpcTrade::getInstance().verifyNpcAction(npc_dwNpcDataID, item)) 
					{
						//soke 修改卖马获得积分
						if(base->price>0)	
						{
							DWORD price = (DWORD)(getGoodnessPrice((base->price),false)*0.5f + 0.99f);
							char info[MAX_CHATINFO];
							sprintf(info, "卖%s得到积分", base->name);
							Zebra::logger->trace("[卖马]用户(%s,%u)卖%s得到积分%d", name, id, base->name, price);
							packs.addMoney(price,"卖东西", info);
						}

						/*if(base->price>0)	
						{
							DWORD price = (DWORD)(getGoodnessPrice(base->price,false)*0.5f + 0.99f);
							char info[MAX_CHATINFO];
							sprintf(info, "卖%s得到银子", base->name);
							Zebra::logger->trace("[卖马]用户(%s,%u)卖%s得到银子%d", name, id, base->name, price);
							packs.addMoney(price,"卖东西", info);
						}*/

						horse.putAway();
						//horse.sendData();
						horse.mount(false);
						horse.horse(0);
						
					}
					else 
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "对不起，小店不收购%s", base->name);
					}
				}
				else Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "本人不与大魔头交易");
				return true;
			}
			break;		
		case STORE_INFO_NPCTRADE_USERCMD_PARAMETER:
			{
				stStoreInfoNpcTradeUserCmd* cmd = (stStoreInfoNpcTradeUserCmd*)rev;
				if ((signed char)cmd->page < 0 ||  cmd->page >= packs.store.days.size() ) return false;

				packs.store.days[cmd->page] += *(BYTE*)(&cmd->day[0]);
				packs.store.notify(this);
				return true;
			}
			break;
		case REPAIROBJECT_GOLD_NPCTRADE_USERCMD_PARAMETER:
			{
				DWORD price = 0;
	
				stRepairObjectGoldNpcTradeUserCmd *cmd = (stRepairObjectGoldNpcTradeUserCmd *)rev;
				if (cmd->id == 0) 
				{
					//compute cost
					RepairCost cost;
					packs.equip.execEvery(cost);
					price = zObject::RepairMoney2RepairGold(cost.cost());
					//价格为0时拒绝修里
					if(!price)
					{
						return true;
					}
					
					//test money
					if (!packs.checkGold(price)) 
					{
						return Channel::sendSys(this,  Cmd::INFO_TYPE_FAIL, "你的金子不足，不能全部修理!");
					}
	
					//repair equip
					RepairEquipUseGold repair(this);
					packs.equip.execEvery(repair);
	
				}
				else 
				{
					zObject* ob = packs.uom.getObjectByThisID(cmd->id);
					if (!ob || !ob->base->recast || /*!ob->data.dur ||// */ !ob->data.maxdur) return false;
					
					price = zObject::RepairMoney2RepairGold(get_repair_price(ob));
	
					//test money
					if (!packs.checkGold(price)) 
					{
						return Channel::sendSys(this,  Cmd::INFO_TYPE_FAIL, "你的金子子不足，不能修理这件物品!");
					}
	
					ob->data.dur = ob->data.maxdur;	
					Cmd::stDurabilityUserCmd ret;
					ret.dwThisID = ob->data.qwThisID;
					ret.dwDur = ob->data.dur;
					ret.dwMaxDur = ob->data.maxdur;
					sendCmdToMe(&ret , sizeof(ret));
				}

				if (!packs.removeGold(price,"修理")) 
				{
					Zebra::logger->fatal("用户(%s)修理装备时金子子计算错误!", name);
				}
			}
			break;
		case REPAIROBJECT_NPCTRADE_USERCMD_PARAMETER:
			{
				DWORD price = 0;
	
				stRepairObjectNpcTradeUserCmd *cmd = (stRepairObjectNpcTradeUserCmd *)rev;
#if 0
				NpcTrade::NpcItem item;
				item.id = 0 /*base->id*/;
				item.kind = 0 /*base->kind*/;
				item.lowLevel = 0;
				item.level = 0 /*base->needlevel*/;
				item.action = NpcTrade::NPC_REPAIR_OBJECT;
				if (!NpcTrade::getInstance().verifyNpcAction(npc_dwNpcDataID, item)) 
				{
					return false;
				}
#endif
				if (cmd->gem_id) 
				{
					zObject* gem_ob = packs.uom.getObjectByThisID(cmd->gem_id);
					if (!gem_ob || gem_ob->base->kind != ItemType_Repair ) return true;

					zObject* ob = packs.uom.getObjectByThisID(cmd->id);
					if (!ob || !ob->base->recast) return true;					

					//repair equip
					//每次修复两点最大耐久,同时增加当前耐久至最大耐久
					ob->data.maxdur += 100;
					if(ob->data.maxdur > ob->base->durability)
					{
						ob->data.maxdur = ob->base->durability;
					}
					ob->data.dur = ob->data.maxdur;	
					Cmd::stDurabilityUserCmd ret;
					ret.dwThisID = ob->data.qwThisID;
					ret.dwDur = ob->data.dur;
					ret.dwMaxDur = ob->data.maxdur;
					sendCmdToMe(&ret , sizeof(ret));

					packs.removeObject(gem_ob); //notify and delete

					return true;
				}
				
				if (cmd->id == 0) 
				{
					//compute cost
					RepairCost cost;
					packs.equip.execEvery(cost);
					price = cost.cost();
					//价格为0时拒绝修里
					if(!price)
					{
						return true;
					}
					
					//soke 修理全部装备扣银子
					
					//test money
					if (!packs.checkMoney(price)) 
					{
					     return Channel::sendSys(this,  Cmd::INFO_TYPE_FAIL, "您的银子不足，不能全部修理!");
					}
					//repair equip
					RepairEquip repair(this);
					packs.equip.execEvery(repair);
				}
				else 
				{
					zObject* ob = packs.uom.getObjectByThisID(cmd->id);
					if (!ob || !ob->base->recast || !ob->data.dur || !ob->data.maxdur) return false;
					
					price = get_repair_price(ob);
					if(!price)
					{
						return true;
					}
	
					//soke 修理单件装备先扣银子
					if (charbase.money >= price) 
					{
					   //test money
					   if (!packs.checkMoney(price)) 
					   {
						   return Channel::sendSys(this,  Cmd::INFO_TYPE_FAIL, "您的银子不足，不能修理这件物品!");
					   }
				    }
	
					//repair equip
					//武器，盾，帽子，衣服，护腕，腰带，靴子，戒指，手镯，项链需要消耗最大耐久
					/*
					switch(ob->base->kind)
					{
						case ItemType_ClothBody:
						case ItemType_FellBody:
						case ItemType_MetalBody:
						case ItemType_Blade:
						case ItemType_Sword:
						case ItemType_Axe:
						case ItemType_Hammer:
						case ItemType_Staff:
						case ItemType_Crossbow:
						case ItemType_Fan:
						case ItemType_Stick:
						case ItemType_Shield:
						case ItemType_Helm:
						case ItemType_Caestus:
						case ItemType_Cuff:
						case ItemType_Shoes:
						case ItemType_Necklace:
						case ItemType_Fing:
						case ItemType_FashionBody:
							{
								//消耗最大耐久公式
								DWORD reduce = ((DWORD)((((float)(ob->data.maxdur-ob->data.dur))/ob->base->durability)*10)+1)*5;
								if(reduce > ob->data.maxdur)
								{
									ob->data.maxdur=0;
								}
								else
								{
									ob->data.maxdur -=reduce;
								}
							}
							break;
						default:
							break;
					}
					// */
					ob->data.dur = ob->data.maxdur;	
					Cmd::stDurabilityUserCmd ret;
					ret.dwThisID = ob->data.qwThisID;
					ret.dwDur = ob->data.dur;
					ret.dwMaxDur = ob->data.maxdur;
					sendCmdToMe(&ret , sizeof(ret));
				}

				//soke 修理单件装备扣银子
				if (!packs.removeMoney(price,"修理")) 
				{
					if (!packs.removeMoney(price,"修理")) 
				   {
					   Zebra::logger->fatal("用户(%s)修理装备时银子计算错误!", name);
				   }
				}
				
				/*
				else 
				{
					Zebra::logger->trace("[修理装备]用户(%s)修理装备花费银子(%d)",name, price);
				}
				// */
			}
			break;

		case START_SELL_USERCMD_PARAMETER:
			if (this->charbase.level <20)
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "等级低于20，不能摆摊!");
				return true;
			}
			if (this->isSitdown())
			{
				this->standup();
			}
			if (this->isFly()) //soke 飞行状态下禁止摆摊
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "飞行状态下，不能摆摊!");
				return false;
			}
			if (this->isWabao()) //soke 挖宝状态下禁止摆摊
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "挖宝状态下，不能摆摊!");
				return false;
			}
			if (this->isDance()) //soke 跳舞状态下禁止摆摊
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "跳舞状态下，不能摆摊!");
				return false;
			}
			if (privatestore.step() == PrivateStore::NONE) 
			{
/*
				if (!scene->checkZoneType(getPos(), ZoneTypeDef::ZONE_PRIVATE_STORE)) 
				{
					return Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "这里不能摆摊!");
				}
*/
				Cmd::stCanSellTradeUserCmd cmd;
				if (!scene->checkZoneType(getPos(), ZoneTypeDef::ZONE_PRIVATE_STORE)) 
				{
					cmd.status = 1;
					sendCmdToMe(&cmd, sizeof(cmd));
					return true;
				}	

				cmd.status = 0;
				sendCmdToMe(&cmd, sizeof(cmd));
				privatestore.step(PrivateStore::START, this);
				Zebra::logger->trace("[交易:玩家------>摆摊]用户(%s)请求开始摆摊", name);
				return true;
			}
			if (privatestore.step() == PrivateStore::START) {
				horse.mount(false, false);
				privatestore.step(PrivateStore::BEGIN, this);
				sendMeToNine();
				Zebra::logger->trace("[交易:玩家------>摆摊]用户(%s)开始摆摊", name);
				return true;
			}
			break;
		case FINISH_SELL_USERCMD_PARAMETER:
			{
				privatestore.step(PrivateStore::NONE, this);
				Zebra::logger->trace("[交易:玩家------>摆摊]用户(%s)完成摆摊", name);
				sendMeToNine();
			}
			break;
		case REQUEST_ADD_OBJECT_SELL_USERCMD_PARAMETER:
			{
				stRequestAddObjectSellUserCmd *cmd = (stRequestAddObjectSellUserCmd *)rev;
				for(int i = 0; i < cmd->num && i < 200; i++)
				{
					if (privatestore.step() != PrivateStore::START ) 
					{
						return true;
					}
					if (cmd->list[i].x > PrivateStore::WIDTH || cmd->list[i].y > PrivateStore::HEIGHT) 
					{
						Zebra::logger->alarm("[交易:玩家------>摆摊]%s(%ld)请求摆摊的物品坐标非法",name,id);	
						return true;	
					}

					zObject* ob = packs.uom.getObjectByThisID(cmd->list[i].qwThisID);
					if (!ob) {
						Zebra::logger->alarm("[交易:玩家------>摆摊]%s(%ld)请求摆摊的物品不存在",name,id);	
						return true;
					}

					if (mask.is_use(ob)) 
					{
						return Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "请先解除该蒙面巾!");
					}

					if (ob->data.bind || ob->data.dwObjectID == 800 || ob->base->kind == ItemType_Quest) 
					{
						Zebra::logger->alarm("[交易:玩家------>摆摊]%s(%ld)用户试图使用不能摆摊的物品",name,id);						
						return true;
					}


					if (ob->data.pos.loc() != Cmd::OBJECTCELLTYPE_COMMON && ob->data.pos.loc()!=Cmd::OBJECTCELLTYPE_PACKAGE) 
					{
						Zebra::logger->alarm("[交易:玩家------>摆摊]%s(%ld)用户试图摆摊不在主副包裹中的物品",name,id);
						return true;
					}

					privatestore.add(ob, cmd->list[i].price, cmd->list[i].x, cmd->list[i].y);
					Zebra::logger->trace("[交易:玩家------>摆摊]用户%s添加摆摊物品%s(%d)" , name, ob->data.strName, ob->data.qwThisID);
				}
			}
			break;
		case ADD_OBJECT_SELL_USERCMD_PARAMETER:
			{
				stAddObjectSellUserCmd *cmd = (stAddObjectSellUserCmd *)rev;
				if (privatestore.step() != PrivateStore::START ) 
				{
					return true;
				}
				if (cmd->x > PrivateStore::WIDTH || cmd->y > PrivateStore::HEIGHT) 
				{
					Zebra::logger->alarm("[交易:玩家------>摆摊]%s(%ld)请求摆摊的物品坐标非法",name,id);	
					return true;	
				}

				zObject* ob = packs.uom.getObjectByThisID(cmd->object.qwThisID);
				if (!ob) 
				{
					Zebra::logger->alarm("[交易:玩家------>摆摊]%s(%ld)请求摆摊的物品不存在",name,id);	
					return true;
				}

	/*
				if (ob->base->kind == ItemType_Money && cmd->object.dwNum > ob->data.dwNum  )
				{
					Zebra::logger->alarm("%s(%ld)请求交易的物品数量非法",name,id);	
					return true;
				}
	*/
				if (mask.is_use(ob)) {
					return Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "请先解除该蒙面巾!");
				}

				if (ob->data.bind || ob->data.dwObjectID == 800 || ob->base->kind == ItemType_Quest) 
				{
					Zebra::logger->alarm("[交易:玩家------>摆摊]%s(%ld)用户试图使用不能摆摊的物品",name,id);						
					return true;
				}
	

				if (ob->data.pos.loc() != Cmd::OBJECTCELLTYPE_COMMON && ob->data.pos.loc()!=Cmd::OBJECTCELLTYPE_PACKAGE) 
				{
					Zebra::logger->alarm("[交易:玩家------>摆摊]%s(%ld)用户试图摆摊不在主副包裹中的物品",name,id);
					return true;
				}

				privatestore.add(ob, cmd->price, cmd->x, cmd->y);
				Zebra::logger->trace("[交易:玩家------>摆摊]用户%s添加摆摊物品%s(%d)" , name, ob->data.strName, ob->data.qwThisID);
			}
			break;
		case REMOVE_OBJECT_SELL_USERCMD_PARAMETER:
			{
				stRemoveObjectSellUserCmd*cmd = (stRemoveObjectSellUserCmd *)rev;
				if (privatestore.step() != PrivateStore::START) 
				{
					return true;
				}

				privatestore.remove(cmd->object_id);

				//just for trace, it's very ugly
				zObject* ob = packs.uom.getObjectByThisID(cmd->object_id);
				if (ob) {
					Zebra::logger->trace("[交易:玩家------>摆摊]用户%s移除摆摊物品%s(%d)" , name, ob->data.strName, ob->data.qwThisID);
				}
				
			}
			break;
		case REQUEST_SELL_INFO_USERCMD_PARAMETER:
			{
				stRequestSellInfoUserCmd *cmd=(stRequestSellInfoUserCmd *)rev;

				if(cmd->temp_id == tempid) 
				{
					//Zebra::logger->alarm("[交易:玩家<------摆摊]%s(%ld)请求自己的摆摊信息", name, id);
					return true;
				}
				
				SceneUser *target = scene->getUserByTempID(cmd->temp_id);
				if (!target || target->privatestore.step() != PrivateStore::BEGIN) 
				{
					Zebra::logger->alarm("[交易:玩家<------摆摊]%s(%ld)请求摆摊信息的用户不存在或者没有摆摊", name, id);
					return true;
				}

				if ( abs(pos.x- target->getPos().x) > (SCREEN_WIDTH ) || abs(pos.y-target->getPos().y) > (SCREEN_HEIGHT))  
				{
					return true;
				}

				//Zebra::logger->trace("[交易:玩家<------摆摊]%s(%ld)请求%s(%ld)摆摊信息", name, id, target->name, target->id);
				target->privatestore.show(this);
			}
			break;

		case REQUEST_SELL_BUY_USERCMD_PARAMETER:
			{
				do_trade_rs_cmd(rev, cmdLen);
				return true;
			}

			break;
		case UPDATE_STORE_PASS_USERCMD_PARAMETER:
			/*
			{
				stUpdateStorePassUserCmd * cmd = (stUpdateStorePassUserCmd*)rev;
				if (strncmp(cmd->oldpass, charbase.pass, sizeof(charbase.pass)) == 0) 
				{
					strncpy(charbase.pass, cmd->newpass, sizeof(charbase.pass));
					return true;
				}

				return Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "密码错误，不能修改密码!")
			}
			*/
			break;
		default:
			break;
	}
	return false;
}

bool RepairCost::exec(zObject* ob)
{
	if (!ob || !ob->base->recast || !ob->data.maxdur ) return true;
	_cost += get_repair_price(ob);
	
	return true;
}

bool RepairEquipUseGold::exec(zObject* ob)
{
	if (!ob || !ob->base->recast || !ob->data.maxdur) return true;

	ob->data.dur = ob->data.maxdur;

	Cmd::stDurabilityUserCmd ret;
	ret.dwThisID = ob->data.qwThisID;
	ret.dwDur = ob->data.dur;
	ret.dwMaxDur = ob->data.maxdur;
	_user->sendCmdToMe(&ret, sizeof(ret));

	return true;
}
bool RepairEquip::exec(zObject* ob)
{
	if (!ob || !ob->base->recast || !ob->data.maxdur) return true;
	//武器，盾，帽子，衣服，护腕，腰带，靴子，戒指，手镯，项链需要消耗最大耐久
	/*
	switch(ob->base->kind)
	{
		case ItemType_ClothBody:
		case ItemType_FellBody:
		case ItemType_MetalBody:
		case ItemType_Blade:
		case ItemType_Sword:
		case ItemType_Axe:
		case ItemType_Hammer:
		case ItemType_Staff:
		case ItemType_Crossbow:
		case ItemType_Fan:
		case ItemType_Stick:
		case ItemType_Shield:
		case ItemType_Helm:
		case ItemType_Caestus:
		case ItemType_Cuff:
		case ItemType_Shoes:
		case ItemType_Necklace:
		case ItemType_Fing:
		case ItemType_FashionBody:
			{
				//消耗最大耐久公式
				DWORD reduce = ((DWORD)((((float)(ob->data.maxdur-ob->data.dur))/ob->base->durability)*10)+1)*5;
				if(reduce > ob->data.maxdur)
				{
					ob->data.maxdur=0;
				}
				else
				{
					ob->data.maxdur -=reduce;
				}
			}
			break;
		default:
			break;
	}
	// */
	ob->data.dur = ob->data.maxdur;

	Cmd::stDurabilityUserCmd ret;
	ret.dwThisID = ob->data.qwThisID;
	ret.dwDur = ob->data.dur;
	ret.dwMaxDur = ob->data.maxdur;
	_user->sendCmdToMe(&ret, sizeof(ret));

	return true;
}
