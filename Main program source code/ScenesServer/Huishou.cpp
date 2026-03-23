#include "SceneUser.h"
#include "SceneNpcManager.h"
#include "NpcTrade.h"
#include "RebuildObject.h"
#include "ScenesServer.h"
#include "CharBase.h"
#include "Command.h"
#include <string>
#include "fjconfig.h"
// 道具回收系统 by:醉梦
bool SceneUser::doHuishouCmd(const Cmd::stHuishouUserCmd *ptCmd, unsigned int cmdLen)
{
	using namespace Cmd;
	switch (ptCmd->byParam) //这里就是判断子命令了byParam
	{
		case HUISHOU_INFO:
		{
			Cmd::stHuishouInfoReturnCmd rev;
			for(int i = 0;i < 100;i++)
			{
				rev.huishou[i].objectID =0;
				rev.huishou[i].price = 0;
				rev.huishou[i].type = 0;
				bcopy("",rev.huishou[i].name,MAX_NAMESIZE);
			}
			
			int object_num = fjconfig::getInstance().huishoulist.size();
			for(int i = 0;i < object_num;i++)
			{
				rev.huishou[i].objectID = fjconfig::getInstance().huishoulist[i].objectID;
				rev.huishou[i].price = fjconfig::getInstance().huishoulist[i].price;
				rev.huishou[i].type = fjconfig::getInstance().huishoulist[i].type;
				bcopy(fjconfig::getInstance().huishoulist[i].name,rev.huishou[i].name,MAX_NAMESIZE);
			}
			sendCmdToMe(&rev, sizeof(rev));			
			
		}
		break;
		case HUISHOU_START:
		{
			stHuishouStart *cmd = (stHuishouStart *)ptCmd;
			zObject *itemobj = this->packs.uom.getObjectByThisID(cmd->itemUID);
			if (itemobj)
			{
				bool isok = false;
				int type = 0;
				int price = 0;
				for(int i=0;i<fjconfig::getInstance().huishoulist.size();i++)
				{
					if(itemobj->data.dwObjectID == fjconfig::getInstance().huishoulist[i].objectID)
					{
						isok = true;
						type = fjconfig::getInstance().huishoulist[i].type;
						price = fjconfig::getInstance().huishoulist[i].price;
						break;
					}
				}
				if(isok)
				{  
					if(type == 1)
					{
						this->packs.addMoney(price*itemobj->data.dwNum,"道具回收");
					}
					else if(type == 2)
					{
						this->packs.addGold(price*itemobj->data.dwNum,"道具回收");
					}
					else if(type == 3){
						this->packs.addSalary(price*itemobj->data.dwNum,"道具回收");
					}
					else{
						Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"回收错误，未知类型");	
						return false;
					}
					this->packs.removeObject(itemobj);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"道具回收成功");		
					Cmd::stHuishouReturn Send;
					Send.state=1;
					sendCmdToMe(&Send, sizeof(Send));
				}
				else{
					Cmd::stHuishouReturn Send;
					Send.state=0;
					sendCmdToMe(&Send, sizeof(Send));
				}


				
		
			}
			
		}
		break;
		
	}
	return true;
}
