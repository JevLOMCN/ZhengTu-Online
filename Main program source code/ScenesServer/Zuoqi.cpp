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

// 坐骑图鉴系统 by:醉梦
bool SceneUser::doZuoqiCmd(const Cmd::stZuoqiUserCmd *ptCmd, unsigned int cmdLen)
{
	using namespace Cmd;
	switch (ptCmd->byParam) //这里就是判断子命令了byParam
	{
		case ZUOQI_INFO:
		{
			Cmd::stZuoqiInfoReturnCmd rev;	
			int level = 1;
			if(charbase.zuoqitujianlevel>=0 && charbase.zuoqitujianlevel<10)
			{
				level = 1;
			}
			else if(charbase.zuoqitujianlevel>=10 && charbase.zuoqitujianlevel<20)
			{
				level = 2;
			}
			else if(charbase.zuoqitujianlevel>=20 && charbase.zuoqitujianlevel<30)
			{
				level = 3;
			}
			else if(charbase.zuoqitujianlevel>=30)
			{
				level = 4;
			}

			rev.level = level;	
			rev.exp = charbase.zuoqitujianexp;
			rev.num1 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].ll;
			rev.num2 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].zl;
			rev.num3 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].mj;
			rev.num4 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].js;
			rev.num5 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].tz;
			rev.num6 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].speed;
			rev.num7 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].wugong;
			rev.num8 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].mogong;
			rev.num9 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].wufang;
			rev.num10 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].mofang;
			rev.num11 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].pDam;
			rev.num12 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].mDam;
			rev.num13 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].pDef;
			rev.num14 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].mDef;
			rev.num15 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].hp;
			sendCmdToMe(&rev, sizeof(rev));			
		}
		break;
		case ZUOQI_LEVELUP:
		{
			
			stZuoqiLevelUpCmd *cmd = (stZuoqiLevelUpCmd *)(ptCmd);
			DWORD state = 0;
			Cmd::stZuoqiLevelUpReturnCmd rev;
			zObject *itemobj = this->packs.uom.getObjectByThisID(cmd->itemid);
			if (itemobj)
			{
				if (charbase.zuoqitujianlevel >= 40)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"您的坐骑图鉴已满级");
					return true;
				}

				if (itemobj->data.dwObjectID != fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].costID)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"升级坐骑图鉴需要物品战马精魄");
					return true;
				}

				DWORD objnum = 1;
				if (this->packs.removeMoney(fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].salary, "图鉴升级"))
				{
					DWORD exp = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].exp;
											// 升级
					charbase.zuoqitujianlevel += 1;
					if (charbase.zuoqitujianexp + exp >= 100)
					{
						charbase.zuoqitujianexp+=exp;
						charbase.zuoqitujianexp -= 100;
					}
					else
					{
						charbase.zuoqitujianexp += exp;
					}
					state = 1;
				}
				else
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"升级坐骑图鉴需要1锭银子");
					return true;
				}

				// 刷新用户数据
				this->setupCharBase();
				Cmd::stMainUserDataUserCmd userinfo;
				this->full_t_MainUserData(userinfo.data);
				sendCmdToMe(&userinfo, sizeof(userinfo));
				if (itemobj->data.dwNum > objnum) // soke 大于9朵直接-9
				{
					itemobj->data.dwNum -= objnum;
					Cmd::stRefCountObjectPropertyUserCmd send;
					send.qwThisID = itemobj->data.qwThisID;
					send.dwNum = itemobj->data.dwNum;
					sendCmdToMe(&send, sizeof(send));
				}
				else
				{
					this->packs.removeObject(itemobj);
				}
			}
			rev.state = state;
			sendCmdToMe(&rev, sizeof(rev));
		}
		break;
		case ZUOQI_LINGQU:
		{	
			DWORD num1 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].ll;
			DWORD num2 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].zl;
			DWORD num3 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].mj;
			DWORD num4 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].js;
			DWORD num5 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].tz;
			DWORD num6 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].speed;
			DWORD num7 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].wugong;
			DWORD num8 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].mogong;
			DWORD num9 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].wufang;
			DWORD num10 = fjconfig::getInstance().zuoqilist[charbase.zuoqitujianlevel].mofang;

			char msg[256];
			sprintf(msg,"id=3615 num=1 bind=1 str=%d inte=%d dex=%d spi=%d con=%d mvspeed=%d pdam=%d pdef=%d mdam=%d mdef=%d",num1,num2,num3,num4,num5,num6,num7,num8,num9,num10);
			Gm::fetch(this,msg);
			Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"领取成功");
		}
		break;

	}
	return true;
}



