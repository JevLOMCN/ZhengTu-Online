#include "SceneUser.h"
#include "SceneNpcManager.h"
#include "NpcTrade.h"
#include "RebuildObject.h"
#include "ScenesServer.h"
#include "CharBase.h"
#include "Command.h"
#include <string>
#include "fjconfig.h"
#include "SessionClient.h"

// 家族BOSS系统 by:醉梦
bool SceneUser::doJiazuBossCmd(const Cmd::stJiazuBossUserCmd *ptCmd, unsigned int cmdLen)
{
	using namespace Cmd;
	switch (ptCmd->byParam) //这里就是判断子命令了byParam
	{
		case JIAZUBOSS_INFO:
		{
			if (this->charbase.septid !=0)
			{
				Cmd::Session::t_JiazuBoss_SceneSession send;
				send.userID = this->id;
				sessionClient->sendCmd(&send, sizeof(send));
			}
			else{
				Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "你还没有家族");
			}
		}
		break;
		case JIAZUBOSS_WEIYANG:
		{
			stJiazuBossWeiyangCmd *cmd = (stJiazuBossWeiyangCmd *)(ptCmd);	
			zObject *itemobj = this->packs.uom.getObjectByThisID(cmd->itemid);
			if (itemobj)
			{
			
				if (itemobj->data.dwNum > 1) // soke 大于9朵直接-9
				{
					itemobj->data.dwNum -= 1;
					Cmd::stRefCountObjectPropertyUserCmd send;
					send.qwThisID = itemobj->data.qwThisID;
					send.dwNum = itemobj->data.dwNum;
					sendCmdToMe(&send, sizeof(send));
				}
				else
				{
					this->packs.removeObject(itemobj);
				}
				Cmd::Session::t_WeiyangJiazuBoss_SceneSession send;
				send.userID = this->id;
				sessionClient->sendCmd(&send, sizeof(send));
			}
			
		}
		break;
		case JIAZUBOSS_ZHAOHUAN:
		{
			Cmd::Session::t_ZhaoHuanJiazuBoss_SceneSession send;
			send.userID = this->id;
			sessionClient->sendCmd(&send, sizeof(send));
		}
		break;

	}
	return true;
}



