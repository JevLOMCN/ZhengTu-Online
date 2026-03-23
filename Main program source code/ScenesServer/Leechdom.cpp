/**
 * \file
 * \version  $Id: Leechdom.cpp  $
 * \author 
 * \date 
 * \brief 药品,食物类物品使用
 *
 * 恢复法术值,体力值,生命值的物品
 */

#include "Leechdom.h"
#include "SceneUser.h"
#include "Chat.h"



/**
 * \brief 使用药品
 *
 *
 * \param obj: 使用物品指针 
 * \return 使用是否成功
 */
bool SceneUser::useLeechdom(zObject *obj)
{
	if (!useRes) return true;

	DWORD old_num=obj->data.dwNum;
	DWORD update = 0;
	switch(obj->base->leechdom.id)
	{
		case Leechdom_dam:
			{
			}
			break;
		case Leechdom_def:
			{
			}
			break;
		case Leechdom_poison:
			{
			}
			break;
		case Leechdom_sppersist:
			{
				leechdom.add(Leechdom_sppersist , 0 , obj->base->leechdom.time);
			}
			break;
		case Leechdom_spup:
			{
				DWORD temp = (DWORD)(charstate.maxsp * (obj->base->leechdom.effect/100.0f));
				charbase.sp=(charbase.sp+temp) >
					charstate.maxsp?charstate.maxsp:(charbase.sp+temp);
				update |= 0x04;
			}
			break;
		case Leechdom_spcostdown:
			{
			}
			break;
		case Leechdom_sp:
			{
				DWORD temp = obj->base->leechdom.effect;
				charbase.sp=(charbase.sp+temp) >
					charstate.maxsp?charstate.maxsp:(charbase.sp+temp);
				update |= 0x04;
			}
			break;
		case Leechdom_spresumeup:
			{
				leechdom.add(Leechdom_spresumeup , obj->base->leechdom.effect , obj->base->leechdom.time);
			}
			break;
		case Leechdom_hp:
		case Leechdom_hp5:
			{
			
				charbase.hp=(charbase.hp+obj->base->leechdom.effect) >
					charstate.maxhp?charstate.maxhp:(charbase.hp+obj->base->leechdom.effect);
				update |= 0x01;
			}
			break;
		case Leechdom_hppersist:
		case Leechdom_hppersist5:
			{
				leechdom.add(Leechdom_hppersist ,  obj->base->leechdom.effect , obj->base->leechdom.time);
			}
			break;
		case Leechdom_mp:
			{
				charbase.mp=(charbase.mp+obj->base->leechdom.effect) >
					charstate.maxmp?charstate.maxmp:(charbase.mp+obj->base->leechdom.effect);
				update |= 0x02;
			}
			break;
		case Leechdom_mppersist:
			{
				leechdom.add(Leechdom_mppersist , obj->base->leechdom.effect , obj->base->leechdom.time);
			}
			break;
		case Leechdom_hpmax:
			{
				DWORD need = 0;
				//soke 使用新的机制 
				if(obj->base->id == 931)
				{
					need = (charstate.maxhp-charbase.hp>50000)?50000:charstate.maxhp-charbase.hp;
				}
				else if(obj->base->id == 930)
				{
					need = (charstate.maxhp-charbase.hp>50000)?50000:charstate.maxhp-charbase.hp;
				}
				else if(obj->base->id == 929)
				{
					need = (charstate.maxhp-charbase.hp>50000)?50000:charstate.maxhp-charbase.hp;
				}
				else if(obj->base->id == 860)
				{
					need = (charstate.maxhp-charbase.hp>30000)?30000:charstate.maxhp-charbase.hp;
				}
				else if(obj->base->id == 881)
				{
					need = (charstate.maxhp-charbase.hp>3000)?3000:charstate.maxhp-charbase.hp;
				}
			//	else
			//	{
			//		need = (charstate.maxhp-charbase.hp>2500)?2500:charstate.maxhp-charbase.hp;
			//	}


				if (0==need)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你现在不需要");
					return true;
				}

				charbase.hp += ((DWORD)(obj->data.dur*100)>need)?need:obj->data.dur*100;

				DWORD minus = need>200?need:200;
				if (minus%100>50)
					minus = minus/100+1;
				else
					minus = minus/100;

				if (obj->data.dur>minus)
					obj->data.dur -= minus;
				else
				{
					obj->data.dur = 0;
					obj->data.dwNum = 0;
				}

				Cmd::stDurabilityUserCmd ret;
				ret.dwThisID = obj->data.qwThisID;
				ret.dwDur = obj->data.dur;
				ret.dwMaxDur = obj->data.maxdur;
				sendCmdToMe(&ret, sizeof(ret));

				update |= 0x01;
			}
			break;
		case Leechdom_mpmax:
			{
				DWORD need = charstate.maxmp-charbase.mp;
				if (0==need)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你现在不需要");
					return true;
				}

				charbase.mp += ((DWORD)(obj->data.dur*50)>need)?need:obj->data.dur*50;

				DWORD minus = need>50?need:50;
				if (minus%50>25)
					minus = minus/25+1;
				else
					minus = minus/25;

				if (obj->data.dur>minus)
					obj->data.dur -= minus;
				else
				{
					obj->data.dur = 0;
					obj->data.dwNum = 0;
				}

				Cmd::stDurabilityUserCmd ret;
				ret.dwThisID = obj->data.qwThisID;
				ret.dwDur = obj->data.dur;
				ret.dwMaxDur = obj->data.maxdur;
				sendCmdToMe(&ret, sizeof(ret));

				update |= 0x01;
			}
			break;
		case Leechdom_chocolate:
			{
				leechdom.add(Leechdom_hppersist , obj->base->leechdom.effect , obj->base->leechdom.time);
				leechdom.add(Leechdom_mppersist , obj->base->leechdom.effect , obj->base->leechdom.time);
			}
			break;
		default:
			{
				Zebra::logger->debug("不能识别的物品类型:%u" , obj->base->leechdom.id);
				return false;
			}
			break;
	}

	if(obj->base->leechdom.id!=Leechdom_hpmax && obj->base->leechdom.id!=Leechdom_mpmax && (int)obj->data.dwNum > 0)
		obj->data.dwNum--;

	if(obj->data.dwNum==0)//数量0
	{
		zObject::logger(obj->createid,obj->data.qwThisID,obj->data.strName,obj->data.dwNum,obj->data.dwNum,0,this->id,this->name,0,NULL,"用药",NULL,0,0);
		packs.removeObject(obj); //notify and delete
	}
	else
	{
		if(old_num != obj->data.dwNum)
		{
			Cmd::stRefCountObjectPropertyUserCmd send;
			send.qwThisID=obj->data.qwThisID;
			send.dwNum=obj->data.dwNum;
			sendCmdToMe(&send,sizeof(send));
		}
	}

	if (update) {
		//notify me
		Cmd::stSetHPAndMPDataUserCmd ret;
		ret.dwHP = charbase.hp;
		ret.dwMP = charbase.mp;
		//ret.dwSP = charbase.sp;
		this->sendCmdToMe(&ret , sizeof(ret));
	}
	if (update & 0x01) {
		this->team.sendtoTeamCharData(this);
	}

#ifdef _TEST_DATA_LOG
	switch(obj->data.maxhp)
	{
		case Leechdom_sppersist:
		case Leechdom_spup:
		case Leechdom_sp:
		case Leechdom_spresumeup:
			{
				writeCharTest(Cmd::Record::SP_WRITEBACK);
			}
			break;
		case Leechdom_hp:
		case Leechdom_hp5:
		case Leechdom_hppersist:
		case Leechdom_hppersist5:
		case Leechdom_hpmax:
			{
				writeCharTest(Cmd::Record::HP_WRITEBACK);
			}
			break;
		case Leechdom_mp:
		case Leechdom_mppersist:
		case Leechdom_mpmax:
			{
				writeCharTest(Cmd::Record::MP_WRITEBACK);
			}
			break;
		default:
			break;
	}
#endif // _TEST_DATA_LOG测试数据

	return true;
}
/**
 * \brief 向定时器增加一钟持续类型
 *
 *
 * \param type: 药品类型
 * \param value: 数值
 * \param times: 持续时间
 */
void Leechdom::add(LeechdomType type , WORD value ,WORD times)
{
	LeechdomElement ele;
	ele.type = type;
	ele.value = value;
	ele.times = times;
	mlock.lock();
	element.push_back(ele);
	mlock.unlock();
}

/**
 * \brief 死亡时需要清除所有药品效果
 *
 */
void Leechdom::clear()
{
	mlock.lock();
	element.clear();
	mlock.unlock();
}
/**
 * \brief 定时刷新冷时间
 *
 */
void Leechdom::checkCooling()
{
	if((int)damcooling > 0)
		damcooling --;
	if((int)defcooling > 0)
		defcooling --;
	if((int)hpcooling > 0)
		hpcooling --;
	if((int)spcooling > 0)
		spcooling --;
	if((int)mpcooling > 0)
		mpcooling --;
}
/**
 * \brief 定时刷新药品数值作用
 *
 *
 * \param pUser: 用户
 * \param update: 改变的用户属性值(二进制表示)
 * \return update
 */
DWORD Leechdom::fresh(SceneUser *pUser , DWORD &update)
{
	checkCooling();
	Leechdom_iterator iter;
	mlock.lock();
	for(iter = element.begin() ; iter != element.end() ; )
	{
		if((*iter).times == 0)
		{
			iter = element.erase(iter);
			continue;
		}
		LeechdomElement &ele = *iter;
		if((int)ele.times > 0) 
		{
			ele.times --;
		}
		switch(ele.type)
		{
			case Leechdom_dam:
				{
				}
				break;
			case Leechdom_def:
				{
				}
				break;
			case Leechdom_poison:
				{
				}
				break;
			case Leechdom_sppersist:
				{
					if(ele.times == 0)
					{
						sppersist = 0;
					}
				}
				break;
			case Leechdom_spcostdown:
				{
				}
				break;
			case Leechdom_spresumeup:
				{
					pUser->charbase.sp=(pUser->charbase.sp+ele.value) >
						pUser->charstate.maxsp?pUser->charstate.maxsp:(pUser->charbase.sp+ele.value);
					update |= 0x04;
				}
				break;
			case Leechdom_hppersist:
				{
					pUser->charbase.hp=(pUser->charbase.hp+ele.value) >
						pUser->charstate.maxhp?pUser->charstate.maxhp:(pUser->charbase.hp+ele.value);
					update |= 0x01;
				}
				break;
			case Leechdom_mppersist:
				{
					pUser->charbase.mp=(pUser->charbase.mp+ele.value) >
						pUser->charstate.maxmp?pUser->charstate.maxmp:(pUser->charbase.mp+ele.value);
					update |= 0x02;
				}
				break;
			default:
				break;
		}
		iter ++;
	}
	mlock.unlock();
	return update;
}

/**
 * \brief 检查是否到冷却期
 *
 *
 * \param type: 类型
 * \return 是否可以再次使用该类药品
 */
bool Leechdom::isCooling(DWORD type)
{
	switch(type)
	{
		case Leechdom_dam:
			{
				if(damcooling == 0)
				{
					damcooling = 3;
					return false;
				}
			}
			break;
		case Leechdom_def:
			{
				if(defcooling == 0)
				{
					defcooling = 3;
					return false;
				}
			}
			break;
		case Leechdom_poison:
			{
			}
			break;
		case Leechdom_sppersist:
		case Leechdom_spup:
		case Leechdom_spcostdown:
		case Leechdom_sp:
		case Leechdom_spresumeup:
			{
				if(spcooling == 0)
				{
					spcooling = 3;
					return false;
				}
			}
			break;
		case Leechdom_hp:
		case Leechdom_hppersist:
		case Leechdom_hpmax:
		case Leechdom_chocolate:
			{
				if(hpcooling == 0)
				{
					hpcooling = 3;
					return false;
				}
			}
			break;
		case Leechdom_hp5://醉梦 药水作用13
		case Leechdom_hppersist5://醉梦 药水作用14
			{
				if(hpcooling == 0)
				{
					hpcooling = 30;
					return false;
				}
			}
			break;
		case Leechdom_mp:
		case Leechdom_mppersist:
			{
				if(mpcooling == 0)
				{
					mpcooling = 3;
					return false;
				}
			}
			break;
		case Leechdom_mpmax:
			{
				return false;
			}
			break;
		default:
			break;
	}
	return true;
}
// */
