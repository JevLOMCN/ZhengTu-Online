#include "SceneUser.h"
#include "SceneNpcManager.h"
#include "NpcTrade.h"
#include "RebuildObject.h"
#include "ScenesServer.h"
#include "CharBase.h"
#include "Command.h"
#include <string>
#include "fjconfig.h"
// 云天别墅系统
bool SceneUser::doBieshuCmd(const Cmd::stBieShuUserCmd *ptCmd, unsigned int cmdLen)
{
	using namespace Cmd;
	switch (ptCmd->byParam) //这里就是判断子命令了byParam
	{
		case BIESHU_INFO:
		{
			Cmd::stBieShuInfoReturnCmd rev;			
			rev.bieshu_level = charbase.bieshulevel;
			rev.bieshu_exp = charbase.bieshuexp;
			rev.bieshu_state = charbase.bieshustate;
			DWORD bieshu_gong = 0;//攻击加成
			DWORD bieshu_fang = 0;//防御加成
			DWORD bieshu_hp = 0;//生命加成
			for(int i=0;i<=charbase.bieshulevel;i++)
			{
				//这里不叠加 后续需要叠加取消注释 修改bieshulevel为i
				bieshu_gong+=fjconfig::getInstance().bieshulist[i].gong;
				bieshu_fang+=fjconfig::getInstance().bieshulist[i].fang;
				bieshu_hp+=fjconfig::getInstance().bieshulist[i].hp;
			}
			rev.bieshu_gong = bieshu_gong;
			rev.bieshu_fang = bieshu_fang;
			rev.bieshu_hp = bieshu_hp;
			sendCmdToMe(&rev, sizeof(rev));			
			
		}

		break;

		case BIESHU_JIHUO:
		{
			stBieShuJiHuoCmd *cmd = (stBieShuJiHuoCmd *)(ptCmd);
			DWORD state = 0;
			Cmd::stBieShuJiHuoReturnCmd rev;
			zObject *itemobj = this->packs.uom.getObjectByThisID(cmd->itemID);
			if (itemobj)
			{
				if (charbase.bieshulevel != 0)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"建造失败,您已建造过别墅了");
					rev.state = 0;
					sendCmdToMe(&rev, sizeof(rev));
					return true;
				}

				if (itemobj->data.dwObjectID != fjconfig::getInstance().bieshulist[charbase.bieshulevel].costID)
				{
					rev.state = 3;
					sendCmdToMe(&rev, sizeof(rev));
					return true;
				}

				DWORD objnum = 1;
				if (this->packs.removeMoney(fjconfig::getInstance().bieshulist[charbase.bieshulevel].salary, "别墅升级"))
				{
					DWORD exp = fjconfig::getInstance().bieshulist[charbase.bieshulevel].exp;
					if (charbase.bieshuexp + exp >= 10000)
					{
						// 升级
						charbase.bieshulevel += 1;
						charbase.bieshuexp+=exp;
						charbase.bieshuexp -= 10000;
					}
					else
					{
						charbase.bieshuexp += exp;
					}
					state = 1;
				}
				else
				{
					rev.state = 2;
					sendCmdToMe(&rev, sizeof(rev));
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

		case BIESHU_LEVELUP:
		{
			stBieShuLevelUpCmd *cmd = (stBieShuLevelUpCmd *)(ptCmd);
			DWORD state = 0;
			Cmd::stBieShuLevenUpReturnCmd rev;
			zObject *itemobj = this->packs.uom.getObjectByThisID(cmd->itemID);
			if (itemobj)
			{
				if (charbase.bieshulevel >= 80)
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"建造失败。别墅已满级");
					rev.state = 0;
					sendCmdToMe(&rev, sizeof(rev));
					return true;
				}

				if (itemobj->data.dwObjectID != fjconfig::getInstance().bieshulist[charbase.bieshulevel].costID)
				{
					rev.state = 3;
					sendCmdToMe(&rev, sizeof(rev));
					return true;
				}

				DWORD objnum = 1;
				if (this->packs.removeMoney(fjconfig::getInstance().bieshulist[charbase.bieshulevel].salary, "别墅升级"))
				{
					DWORD exp = fjconfig::getInstance().bieshulist[charbase.bieshulevel].exp;
					if (charbase.bieshuexp + exp >= 10000)
					{
						// 升级
						charbase.bieshulevel += 1;
						charbase.bieshuexp+=exp;
						charbase.bieshuexp -= 10000;
					}
					else
					{
						charbase.bieshuexp += exp;
					}
					state = 1;
				}
				else
				{
					rev.state = 2;
					sendCmdToMe(&rev, sizeof(rev));
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
	}
	return true;
}
