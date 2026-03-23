#include "SceneUser.h"
#include "SceneNpcManager.h"
#include "NpcTrade.h"
#include "RebuildObject.h"
#include <string>
#include <math.h>




bool SceneUser::npcTradeGold(Cmd::stBuyObjectNpcTradeUserCmd *ptCmd,zObjectB *base, BYTE itemlevel)
{
	SceneNpc * n = SceneNpcManager::getMe().getNpcByTempID(npc_dwNpcTempID);
	////////////////////////////////////////////////////////////////////////
	//soke 随身商店金子购买
	if (!n && npc_dwNpcTempID != 100000000)
	////////////////////////////////////////////////////////////////////////
	{
			Zebra::logger->debug("[交易:玩家<------金子商店]%s 交易时，找不到该npc tempID=%u", name, npc_dwNpcTempID);
			return false;
	}
	DWORD need_gold=0;
	
	//soke 物品等级价格 比如材料等级
	float levelopt = pow(5,itemlevel);
	if(base && charbase.gold >= base->price*ptCmd->dwNum*levelopt)
	{
		NpcTrade::NpcItem item;
		item.id = base->id;
		item.kind = base->kind;
		item.lowLevel = 0;
		item.level = base->needlevel;
		item.itemlevel = itemlevel;
		item.action = NpcTrade::NPC_BUY_OBJECT;
		if (NpcTrade::getInstance().verifyNpcAction(npc_dwNpcDataID, item))
		{
			//zObject *o = zObject::create(base, ptCmd->dwNum);
			zObject* o = NULL;
			if (base->recast)
			{
				o = zObject::create(base, ptCmd->dwNum, 0);
				if (o && itemlevel)
				{
					do
					{
						//Upgrade::upgrade(*this, o, 0); //must success
					}
					while (--itemlevel>0);
				}
			}else
			{
				o = zObject::create(base, ptCmd->dwNum, itemlevel);
			}
			if(o)
			{
				o->data.bind = 1;
				Combination2 callback(this, o);
				packs.main.execEvery(callback);
				if (packs.equip.pack(EquipPack::L_PACK)) packs.equip.pack(EquipPack::L_PACK)->execEvery(callback);
				if (packs.equip.pack(EquipPack::R_PACK)) packs.equip.pack(EquipPack::R_PACK)->execEvery(callback);

				int free = 0;
				if (o->data.dwNum)
				{

					if (packs.addObject(o, true, AUTO_PACK)) 
					{
						free = o->data.dwNum;
					//	//金子购买特定类型物品需要绑定
						
						//soke 金子购买所有物品都是绑定的
					    //o->data.bind = 1;
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
					//soke 修改支持金子购买材料等级
				    //DWORD need=base->price*count;
					
					DWORD need = base->price*count*(DWORD)((float)levelopt);
					if(charbase.gold >=need)
					{
						need_gold=need;
					}
					else
					{
						need_gold=charbase.gold;
					}
					if(need_gold)
					{
						std::string disc = "买东西:"; 
						disc += o->base->name;
						if (!packs.removeGold(need_gold,disc.c_str())) {
							Zebra::logger->fatal("[交易:玩家<------商店]用户(%s)买%s时金币计算错误!", name, o->base->name);
						}
					}
					////////////////////////////////////////////////////////////////////////////////////////////////////
	                //soke 随身商店金子购买 
					if( npc_dwNpcTempID == 100000000 )
					{
						if(need_gold)
					    {
							Zebra::logger->trace("[交易:玩家<------随身商店]用户(%s,%u)买%s(%d)个花费金子%d文",name,id,o->name,count,need_gold);
						    Channel::sendGold(this, Cmd::INFO_TYPE_GAME , need_gold,"得到物品 %s(%d)个 , 花费金子",o->name , count);
					    }
					}
                    else
					{
					    zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,count,1,n->id,n->name,this->id,this->name,"buy_npc",o->base,o->data.kind,o->data.upgrade);
					    if(need_gold)
					    {
							Zebra::logger->trace("[交易:玩家<------金子商店]用户(%s,%u)买%s(%d)个花费金子%d文",name,id,o->name,count,need_gold);
						    Channel::sendGold(this, Cmd::INFO_TYPE_GAME , need_gold,"得到物品 %s(%d)个 , 花费金子",o->name , count);
					    }
					}
				}
				if (!free) 
				{ //package is full
					//Channel::sendSys(this , Cmd::INFO_TYPE_FAIL, "你的包裹已满");
					zObject::destroy(o);
				}									
			}
		}
	}
	else
	{
		Channel::sendSys(this,Cmd::INFO_TYPE_GAME,"您的金子不足");
	}
	return true;
}
