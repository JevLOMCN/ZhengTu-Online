#include "SceneUser.h"
#include "SceneNpcManager.h"
#include "NpcTrade.h"
#include "RebuildObject.h"
#include <string>
#include <math.h>




bool SceneUser::npcTradeSalary(Cmd::stBuyObjectSalaryTradeUserCmd *ptCmd,zObjectB *base, BYTE itemlevel)
{
	SceneNpc * n = SceneNpcManager::getMe().getNpcByTempID(npc_dwNpcTempID);
	if (!n)
	{
		Zebra::logger->debug("[交易:玩家<------工资商店]%s 交易时，找不到该npc tempID=%u", name, npc_dwNpcTempID);
		return false;
	}
	DWORD need_salary=0;
	
	//zise 物品等级价格 比如材料等级
	float levelopt = pow(5,itemlevel);
	//zise 2017-2-4 工资购买
	if(base && charbase.salary >= (base->price)*ptCmd->dwNum*levelopt)
	//if(base && charbase.salary >= base->price*ptCmd->dwNum*levelopt)
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
						//Upgrade::upgrade(*this, o, 100); //must success
					}
					while (--itemlevel>0);
				}
			}
			else
			{
				o = zObject::create(base, ptCmd->dwNum, itemlevel);
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
					//	o->checkBind();
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
					//zise 修改工资支持购买等级
					//DWORD need_salary = base->price*count;
					
					//zise 2017-2-4 修改工资购买
					DWORD need = (base->price)*count*(DWORD)((float)levelopt);
				//	DWORD need = base->price*count*(DWORD)((float)levelopt);
					if(charbase.salary >=need)
					{
						need_salary=need;
					}
					else
					{
						need_salary=charbase.salary;
					}
					if(need_salary)
					{
						std::string disc = "买东西:"; 
						disc += o->base->name;
						if (!packs.removeSalary(need_salary,disc.c_str())) 
						{
							Zebra::logger->fatal("[交易:玩家<------工资商店]用户(%s)买%s时工资计算错误!", name, o->base->name);
						}
					}
					zObject::logger(o->createid,o->data.qwThisID,o->data.strName,o->data.dwNum,count,1,n->id,n->name,this->id,this->name,"buy_npc",o->base,o->data.kind,o->data.upgrade);
					if(need_salary)
					{
                        Zebra::logger->trace("[交易:玩家<------工资商店]用户(%s,%u)买%s(%d)个花费工资%d文",name,id,o->name,count,need_salary);
						Channel::sendSalary(this, Cmd::INFO_TYPE_GAME , need_salary,"得到物品 %s(%d)个 , 花费工资",o->name , count);
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
		Channel::sendSys(this,Cmd::INFO_TYPE_GAME,"您的工资不足");
	}
	return true;
}
