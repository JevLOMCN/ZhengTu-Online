#include "SceneUser.h"
#include "SceneNpcManager.h"
#include "NpcTrade.h"
#include "RebuildObject.h"
#include "CharBase.h"
#include "Command.h"
#include "fjconfig.h"
#include <string>
// 生儿育女系统 儿女系统 by:醉梦
bool SceneUser::doErnvCmd(const Cmd::stErnvUserCmd *ptCmd, unsigned int cmdLen)
{
	using namespace Cmd;
	switch (ptCmd->byParam) // 这里就是判断子命令了byParam
	{
		case ERNVINFO_USERCMD:
		{
			Cmd::stErnvInfoReturn rev;
			rev.babylevel = charbase.babylevel;
			rev.babyexp = charbase.babyexp;
			rev.babyskill1 = charbase.babyskill1;
			rev.babyskill2 = charbase.babyskill2;
			rev.babyskill3 = charbase.babyskill3;
			rev.babyskill4 = charbase.babyskill4;
			rev.babyskill5 = charbase.babyskill5;
			rev.babyskillpoint = charbase.babyskillpoint;

			int num1 = 0;
			int num2 = 0;
			int num3 = 0;
			int num4 = 0;
			int num5 = 0;

			int ll = 0;
			int zl = 0;
			int js = 0;
			int mj = 0;
			int tz = 0;
			for (int i = 0; i < charbase.babylevel; i++)
			{
				num1 += fjconfig::getInstance().ernvlist[i].pDam;
				num2 += fjconfig::getInstance().ernvlist[i].mDam;
				num3 += fjconfig::getInstance().ernvlist[i].pDef;
				num4 += fjconfig::getInstance().ernvlist[i].mDef;
				num5 += fjconfig::getInstance().ernvlist[i].hp;

				ll += fjconfig::getInstance().ernvlist[i].ll;
				zl += fjconfig::getInstance().ernvlist[i].zl;
				js += fjconfig::getInstance().ernvlist[i].js;
				mj += fjconfig::getInstance().ernvlist[i].mj;
				tz += fjconfig::getInstance().ernvlist[i].tz;
			}

			rev.num1 = num1;
			rev.num2 = num2;
			rev.num3 = num3;
			rev.num4 = num4;
			rev.num5 = num5;

			rev.ll = ll;
			rev.zl = zl;
			rev.js = js;
			rev.mj = mj;
			rev.tz = tz;

			sendCmdToMe(&rev, sizeof(rev));
		}
		break;
		case ERNV_LEVELUP:
		{
			stErnvLevelup *cmd = (stErnvLevelup *)(ptCmd);
			DWORD state = 0;
			Cmd::stErnvLevelupReturn rev;
			zObject *itemobj = this->packs.uom.getObjectByThisID(cmd->itemUID);
			if (itemobj)
			{
				if (charbase.babylevel >= 50)
				{
					rev.state = 0;
					sendCmdToMe(&rev, sizeof(rev));
					return true;
				}

				if (itemobj->data.dwObjectID != fjconfig::getInstance().ernvlist[charbase.babylevel - 1].costID)
				{
					rev.state = 3;
					sendCmdToMe(&rev, sizeof(rev));
					return true;
				}
				DWORD objnum = 1;
				if (this->packs.removeMoney(fjconfig::getInstance().ernvlist[charbase.babylevel - 1].salary, "儿女升级"))
				{
					// objnum = fjconfig::getInstance().ernvlist[charbase.babylevel - 1].costNum;
					// if (itemobj->data.dwNum < objnum)
					// {
					// 	rev.state = 4;
					// 	sendCmdToMe(&rev, sizeof(rev));
					// 	return true;
					// }

					DWORD exp = fjconfig::getInstance().ernvlist[charbase.babylevel - 1].exp;
					if (charbase.babyexp + exp >= 2000)
					{
						// 升级
						charbase.babylevel += 1;
						charbase.babyexp+=exp;
						charbase.babyexp -= 2000;
						charbase.babyskillpoint += 1;
					}
					else
					{
						charbase.babyexp += exp;
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
		case ERNV_SKILL_LEVELUP:
		{
			stErnvSkillLevelup *cmd = (stErnvSkillLevelup *)(ptCmd);
			DWORD state = 0;
			Cmd::stErnvSkillLevelupReturn rev;
			if(charbase.babyskillpoint <= 0)
			{
				rev.state = 0;
				sendCmdToMe(&rev, sizeof(rev));
				return true;
			}	
			else{
				switch (cmd->skill)
				{
					case 1:
					{
						if(charbase.babyskill1 >= 10)
						{
							rev.state = 0;
							sendCmdToMe(&rev, sizeof(rev));
							return true;
						}
						charbase.babyskill1 += 1;
						charbase.babyskillpoint -= 1;
					}
					break;
					case 2:
					{
						if(charbase.babyskill2 >= 10)
						{
							rev.state = 0;
							sendCmdToMe(&rev, sizeof(rev));
							return true;
						}
						charbase.babyskill2 += 1;
						charbase.babyskillpoint -= 1;
					}
					break;
					case 3:
					{
						if(charbase.babyskill3 >= 10)
						{
							rev.state = 0;
							sendCmdToMe(&rev, sizeof(rev));
							return true;
						}
						charbase.babyskill3 += 1;
						charbase.babyskillpoint -= 1;
					}
					break;
					case 4:
					{
						if(charbase.babyskill4 >= 10)
						{
							rev.state = 0;
							sendCmdToMe(&rev, sizeof(rev));
							return true;
						}
						charbase.babyskill4 += 1;
						charbase.babyskillpoint -= 1;
					}
					break;
					case 5:
					{
						if(charbase.babyskill5 >= 10)
						{
							rev.state = 0;
							sendCmdToMe(&rev, sizeof(rev));
							return true;
						}
						charbase.babyskill5 += 1;
						charbase.babyskillpoint -= 1;
					}
					break;

				}
			}
			rev.state = 1;
			sendCmdToMe(&rev, sizeof(rev));
			this->setupCharBase();
			Cmd::stMainUserDataUserCmd userinfo;
			this->full_t_MainUserData(userinfo.data);
			sendCmdToMe(&userinfo, sizeof(userinfo));
			return true;
		}
		break;
		case ERNVPUT_USERCMD:
		{
			stErnvPutUserCmd *cmd = (stErnvPutUserCmd *)(ptCmd);
			if(cmd->state == 1)
			{
				summonPet(5983, Cmd::PET_TYPE_XCARTOON);
				Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"你放出了你的宝宝");
			}
			else{
				shouhuibaby();
				Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"你收回了你的宝宝");
				
			}
			
		}
	}
	return true;
}
