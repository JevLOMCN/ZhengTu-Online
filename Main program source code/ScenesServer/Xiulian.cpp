#include "SceneUser.h"
#include "SceneNpcManager.h"
#include "NpcTrade.h"
#include "RebuildObject.h"
#include "ScenesServer.h"
#include "CharBase.h"
#include "Command.h"
#include <string>
#include "fjconfig.h"
// 能力修炼系统 by:醉梦
bool SceneUser::doXiulianCmd(const Cmd::stXiulianUserCmd *ptCmd, unsigned int cmdLen)
{
	int max_xiulianlevel = 800;
	using namespace Cmd;
	switch (ptCmd->byParam) //这里就是判断子命令了byParam
	{
		case XIULIAN_INFO:
		{
			Cmd::stXiulianInfoReturnCmd rev;			
			rev.xiulianexp = charbase.xiulianexp;
			rev.level1 = charbase.xiulianlevel1;
			rev.level2 = charbase.xiulianlevel2;
			rev.level3 = charbase.xiulianlevel3;
			rev.level4 = charbase.xiulianlevel4;
			rev.level5 = charbase.xiulianlevel5;
			
			rev.exp1 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel1].exp;
			rev.exp2 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel2].exp;
			rev.exp3 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel3].exp;
			rev.exp4 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel4].exp;
			rev.exp5 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel5].exp;
			
			rev.money1 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel1].salary;
			rev.money2 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel2].salary;
			rev.money3 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel3].salary;
			rev.money4 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel4].salary;
			rev.money5 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel5].salary;
			

			int num1 = 0;
			int num2 = 0;
			int num3 = 0;
			int num4 = 0;
			int num5 = 0;

			for(int i=0;i<=charbase.xiulianlevel1;i++)
			{
				num1 += fjconfig::getInstance().xiulianlist[i].pDam;
			}
			for(int i=0;i<=charbase.xiulianlevel2;i++)
			{
				num2 += fjconfig::getInstance().xiulianlist[i].mDam;
			}
			for(int i=0;i<=charbase.xiulianlevel3;i++)
			{
				num3 += fjconfig::getInstance().xiulianlist[i].pDef;
			}
			for(int i=0;i<=charbase.xiulianlevel4;i++)
			{
				num4 += fjconfig::getInstance().xiulianlist[i].mDef;
			}
			for(int i=0;i<=charbase.xiulianlevel5;i++)
			{
				num5 += fjconfig::getInstance().xiulianlist[i].hp;
			}
			rev.num1 = num1;
			rev.num2 = num2;
			rev.num3 = num3;
			rev.num4 = num4;
			rev.num5 = num5;
			sendCmdToMe(&rev, sizeof(rev));			
			
		}

		break;

		case XIULIAN_START:
		{
			stXiulianStartCmd *cmd = (stXiulianStartCmd *)(ptCmd);
			DWORD state = 0;
			Cmd::stXiulianReturnCmd rev;
			switch(cmd->type)
			{
				case 1:
				{
					if (charbase.xiulianlevel1 >= max_xiulianlevel)
					{
						rev.state = 0;//满级
						sendCmdToMe(&rev, sizeof(rev));
						return false;
					}
					if(charbase.xiulianexp >= fjconfig::getInstance().xiulianlist[charbase.xiulianlevel1].exp)
					{
						if(this->packs.removeMoney(fjconfig::getInstance().xiulianlist[charbase.xiulianlevel1].salary, "能力修炼"))
						{
							charbase.xiulianexp -= fjconfig::getInstance().xiulianlist[charbase.xiulianlevel1].exp;
							charbase.xiulianlevel1 += 1;
							state = 1;//成功
						}
						else{
							rev.state = 3;//银子不足
							sendCmdToMe(&rev, sizeof(rev));
							return false;
						}
					}
					else{
						rev.state = 2;//经验不足
						sendCmdToMe(&rev, sizeof(rev));
						return false;
					}
				}
				break;
				case 2:
				{
					if (charbase.xiulianlevel2 >= max_xiulianlevel)
					{
						rev.state = 0;//满级
						sendCmdToMe(&rev, sizeof(rev));
						return false;
					}
					if(charbase.xiulianexp >= fjconfig::getInstance().xiulianlist[charbase.xiulianlevel2].exp)
					{
						if(this->packs.removeMoney(fjconfig::getInstance().xiulianlist[charbase.xiulianlevel2].salary, "能力修炼"))
						{
							charbase.xiulianexp -= fjconfig::getInstance().xiulianlist[charbase.xiulianlevel2].exp;
							charbase.xiulianlevel2 += 1;
							state = 1;//成功
						}
						else{
							rev.state = 3;//银子不足
							sendCmdToMe(&rev, sizeof(rev));
							return false;
						}
					}
					else{
						rev.state = 2;//经验不足
						sendCmdToMe(&rev, sizeof(rev));
						return false;
					}
				}
				break;
				case 3:
				{
					if (charbase.xiulianlevel3 >= max_xiulianlevel)
					{
						rev.state = 0;//满级
						sendCmdToMe(&rev, sizeof(rev));
						return false;
					}
					if(charbase.xiulianexp >= fjconfig::getInstance().xiulianlist[charbase.xiulianlevel3].exp)
					{
						if(this->packs.removeMoney(fjconfig::getInstance().xiulianlist[charbase.xiulianlevel3].salary, "能力修炼"))
						{

							charbase.xiulianexp -= fjconfig::getInstance().xiulianlist[charbase.xiulianlevel3].exp;
							charbase.xiulianlevel3 += 1;
							state = 1;//成功
						}
						else{
							rev.state = 3;//银子不足
							sendCmdToMe(&rev, sizeof(rev));
							return false;
						}
					}
					else{
						rev.state = 2;//经验不足
						sendCmdToMe(&rev, sizeof(rev));
						return false;
					}
				}
				break;
				case 4:
				{
					if (charbase.xiulianlevel4 >= max_xiulianlevel)
					{
						rev.state = 0;//满级
						sendCmdToMe(&rev, sizeof(rev));
						return false;
					}
					if(charbase.xiulianexp >= fjconfig::getInstance().xiulianlist[charbase.xiulianlevel4].exp)
					{
						if(this->packs.removeMoney(fjconfig::getInstance().xiulianlist[charbase.xiulianlevel4].salary, "能力修炼"))
						{
							
							charbase.xiulianexp -= fjconfig::getInstance().xiulianlist[charbase.xiulianlevel4].exp;
							charbase.xiulianlevel4 += 1;
							state = 1;//成功
						}
						else{
							rev.state = 3;//银子不足
							sendCmdToMe(&rev, sizeof(rev));
							return false;
						}
					}
					else{
						rev.state = 2;//经验不足
						sendCmdToMe(&rev, sizeof(rev));
						return false;
					}
				}
				break;
				case 5:
				{
					if (charbase.xiulianlevel5 >= max_xiulianlevel)
					{
						rev.state = 0;//满级
						sendCmdToMe(&rev, sizeof(rev));
						return false;
					}
					if(charbase.xiulianexp >= fjconfig::getInstance().xiulianlist[charbase.xiulianlevel5].exp)
					{
						if(this->packs.removeMoney(fjconfig::getInstance().xiulianlist[charbase.xiulianlevel5].salary, "能力修炼"))
						{
							
							charbase.xiulianexp -= fjconfig::getInstance().xiulianlist[charbase.xiulianlevel5].exp;
							charbase.xiulianlevel5 += 1;
							state = 1;//成功
						}
						else{
							rev.state = 3;//银子不足
							sendCmdToMe(&rev, sizeof(rev));
							return false;
						}
					}
					else{
						rev.state = 2;//经验不足
						sendCmdToMe(&rev, sizeof(rev));
						return false;
					}
				}
				break;
			}
		
				
			// 刷新用户数据
			this->setupCharBase();
			Cmd::stMainUserDataUserCmd userinfo;
			this->full_t_MainUserData(userinfo.data);
			sendCmdToMe(&userinfo, sizeof(userinfo));
			
			rev.state = state;
			sendCmdToMe(&rev, sizeof(rev));
		}
		break;
		case XIULIAN_RESET:
		{
			int exp = 0;
			for(int i = 0;i < charbase.xiulianlevel1;i++)
			{
				exp += fjconfig::getInstance().xiulianlist[i].exp;
			}
			for(int i = 0;i < charbase.xiulianlevel2;i++)
			{
				exp += fjconfig::getInstance().xiulianlist[i].exp;
			}
			for(int i = 0;i < charbase.xiulianlevel3;i++)
			{
				exp += fjconfig::getInstance().xiulianlist[i].exp;
			}
			for(int i = 0;i < charbase.xiulianlevel4;i++)
			{
				exp += fjconfig::getInstance().xiulianlist[i].exp;
			}
			for(int i = 0;i < charbase.xiulianlevel5;i++)
			{
				exp += fjconfig::getInstance().xiulianlist[i].exp;
			}

			charbase.xiulianexp+=exp;
			charbase.xiulianlevel1 = 0;
			charbase.xiulianlevel2 = 0;
			charbase.xiulianlevel3 = 0;
			charbase.xiulianlevel4 = 0;
			charbase.xiulianlevel5 = 0;

			Cmd::stXiulianInfoReturnCmd rev;			
			rev.xiulianexp = charbase.xiulianexp;
			rev.level1 = charbase.xiulianlevel1;
			rev.level2 = charbase.xiulianlevel2;
			rev.level3 = charbase.xiulianlevel3;
			rev.level4 = charbase.xiulianlevel4;
			rev.level5 = charbase.xiulianlevel5;
			
			rev.exp1 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel1].exp;
			rev.exp2 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel2].exp;
			rev.exp3 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel3].exp;
			rev.exp4 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel4].exp;
			rev.exp5 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel5].exp;
			
			rev.money1 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel1].salary;
			rev.money2 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel2].salary;
			rev.money3 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel3].salary;
			rev.money4 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel4].salary;
			rev.money5 = fjconfig::getInstance().xiulianlist[charbase.xiulianlevel5].salary;
			

			int num1 = 0;
			int num2 = 0;
			int num3 = 0;
			int num4 = 0;
			int num5 = 0;

			for(int i=0;i<=charbase.xiulianlevel1;i++)
			{
				num1 += fjconfig::getInstance().xiulianlist[i].pDam;
			}
			for(int i=0;i<=charbase.xiulianlevel2;i++)
			{
				num2 += fjconfig::getInstance().xiulianlist[i].mDam;
			}
			for(int i=0;i<=charbase.xiulianlevel3;i++)
			{
				num3 += fjconfig::getInstance().xiulianlist[i].pDef;
			}
			for(int i=0;i<=charbase.xiulianlevel4;i++)
			{
				num4 += fjconfig::getInstance().xiulianlist[i].mDef;
			}
			for(int i=0;i<=charbase.xiulianlevel5;i++)
			{
				num5 += fjconfig::getInstance().xiulianlist[i].hp;
			}
			rev.num1 = num1;
			rev.num2 = num2;
			rev.num3 = num3;
			rev.num4 = num4;
			rev.num5 = num5;
			sendCmdToMe(&rev, sizeof(rev));	
			
			this->setupCharBase();
			Cmd::stMainUserDataUserCmd userinfo;
			this->full_t_MainUserData(userinfo.data);
			sendCmdToMe(&userinfo, sizeof(userinfo));
		}
		break;
	}
	return true;
}
