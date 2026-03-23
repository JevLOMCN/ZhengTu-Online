#include "SceneUser.h"
#include "SceneNpcManager.h"
#include "NpcTrade.h"
#include "RebuildObject.h"
#include "ScenesServer.h"
#include "CharBase.h"
#include "Command.h"
#include "SkillStatusManager.h"
#include <string>
// 自行车系统 by:醉梦
bool SceneUser::doBikeCmd(const Cmd::stBikeUserCmd *ptCmd, unsigned int cmdLen)
{
	using namespace Cmd;
	switch (ptCmd->byParam) //这里就是判断子命令了byParam
	{
		case BIKE_START:
		{
			struct tm tv1;
			time_t timValue = time(NULL);
			zRTime::getLocalTime(tv1, timValue);
			if((tv1.tm_hour ==20 && tv1.tm_min >=30) || tv1.tm_hour >=21){//自行车时间 20:30
				if (!this->issetUState(333) && charbase.bikestate!=1)
				{
					if(charbase.bikecs >=1 )
					{
						Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"您今日已经完成自行车比赛");	
						return false;
					}

					if (((SceneUser *)this)->horse.mount())
					{
						((SceneUser *)this)->horse.mount(false);
					}
					((SceneUser *)this)->skillStatusM.clearActiveSkillStatusOnlyUseToStatus48();
					showCurrentEffect(333, true ); 
					charbase.bikestate=1;
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"报名成功，开始骑行");	
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
					this->charstate.movespeed  = 640;
					//刷新用户数据
					Cmd::stMainUserDataUserCmd  userinfo;
					this->full_t_MainUserData(userinfo.data);
					this->sendCmdToMe(&userinfo,sizeof(userinfo));


					//通知客户端比赛状态
					Cmd::stBikeStateCmd cmd1;
					cmd1.state = 1;
					sendCmdToMe(&cmd1, sizeof(cmd1));		
					

					
					Cmd::stBikeAdditemCmd cmd2;
					cmd2.itemID = 946+zMisc::randBetween(0, 3);
					sendCmdToMe(&cmd2, sizeof(cmd2));		

					// zObjectB *base = objectbm.get(946);
					// zObject* ob = zObject::create(base,1);
					// if (ob) 
					// {	
					// 	ob->data.pos = stObjectLocation(Cmd::OBJECTCELLTYPE_FUJIAN, 0, (WORD)0, (WORD)6);
					// 	packs.addObject(ob, false);
					// 	Cmd::stAddObjectPropertyUserCmd ret;
					// 	ret.byActionType = Cmd::EQUIPACTION_OBTAIN;
					// 	bcopy(&ob->data, &ret.object, sizeof(t_Object));
					// 	sendCmdToMe(&ret, sizeof(ret));		
					// }

					
				}
				else{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"您正在参加自行车比赛");	
				}
			}
			else{
				Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"不在比赛时间内");	
			}

			
		}
		break;
		case BIKE_DEL:
		{
			if (this->issetUState(333) || charbase.bikestate==1)
			{
				showCurrentEffect(333, false ); 
				charbase.bikestate=0;
				charbase.bikenum=0;
				Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"注销成功");	

				//通知客户端比赛状态
				Cmd::stBikeStateCmd cmd;
				cmd.state = 0;
				sendCmdToMe(&cmd, sizeof(cmd));		
			}
			else{
				Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"您没有参加自行车比赛");	
			}
		}
		break;
		case BIKE_SCNL:
		{
			if(charbase.bikestate!=1 || !this->issetUState(333))
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"您没有参加自行车比赛");	
				return false;
			}

			stBikeScnlCmd *cmd = (stBikeScnlCmd *)(ptCmd);
			if(cmd->npcid == 1)
			{
				if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 2)
			{
				if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 3)
			{
				if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 4)
			{
				if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 5)
			{
				if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 6)
			{
				if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 7)
			{
				if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 8)
			{
				if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 9)
			{
					if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 10)
			{
					if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 11)
			{
					if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 12)
			{
					if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 13)
			{
					if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 14)
			{
					if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 15)
			{
					if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 16)
			{
					if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 17)
			{
					if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 18)
			{
					if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 19)
			{
				if(charbase.bikenum+1 == cmd->npcid)
				{
					charbase.bikenum++;
					if (zMisc::selectByPercent(50)) // 几率 网管百分之
					{
						Cmd::stBikeAdditemCmd cmd;
						cmd.itemID = 946+zMisc::randBetween(0, 3);
						sendCmdToMe(&cmd, sizeof(cmd));	
					}
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);

				}
				else if(charbase.bikenum+1==20){
					char text[256];
					sprintf(text,"请前往 自行车赛发令员 完成比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,text);
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
			}
			else if(cmd->npcid == 20)
			{
				if(charbase.bikenum+1 == cmd->npcid)
				{

					//通知客户端比赛状态
					Cmd::stBikeStateCmd cmd;
					cmd.state = 0;
					sendCmdToMe(&cmd, sizeof(cmd));		
					//完成比赛
					charbase.bikecs=1;
					showCurrentEffect(333, false ); 
					charbase.bikestate=0;
					charbase.bikenum=0;				
					if(atoi(Zebra::global["BIKENUM1"].c_str())==0)
					{
						//第一

						char a[16];
						sprintf(a, "%d", 1);
						Zebra::global["BIKENUM1"]=a;
						SceneUser::addBindObjectNum(35103,1,0,0,1);			
						Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"恭喜%s的%s赢得世外桃源自行车大赛总冠军！！祝贺TA！！！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );
						Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL,"恭喜%s的%s赢得世外桃源自行车大赛总冠军！！祝贺TA！！！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );
								
					}
					else if(atoi(Zebra::global["BIKENUM2"].c_str())==0)
					{
						char a[16];
						sprintf(a, "%d", 1);
						Zebra::global["BIKENUM2"]=a;
						SceneUser::addBindObjectNum(35104,1,0,0,1);			
						Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"恭喜%s的%s赢得世外桃源自行车大赛亚军！！祝贺TA！！！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );
						Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL,"恭喜%s的%s赢得世外桃源自行车大赛亚军！！祝贺TA！！！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );
						

					}
					else if(atoi(Zebra::global["BIKENUM3"].c_str())==0)
					{
						//第三
						char a[16];
						sprintf(a, "%d", 1);
						Zebra::global["BIKENUM3"]=a;
						SceneUser::addBindObjectNum(35105,1,0,0,1);			
						Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"恭喜%s的%s赢得世外桃源自行车大赛季军！！祝贺TA！！！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );
						Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL,"恭喜%s的%s赢得世外桃源自行车大赛季军！！祝贺TA！！！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );
						
					}
					else{
						if(atoi(Zebra::global["ZXCWC"].c_str())>=10)
						{
							//参与奖
							SceneUser::addBindObjectNum(35106,1,0,0,1);			
							Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"%s的%s完成世外桃源自行车大赛，获得大量奖励！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );
							
						}
						else{
							SceneUser::addBindObjectNum(35107,1,0,0,1);			
							Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"%s的%s完成世外桃源自行车大赛，获得4-10名礼包！",SceneManager::getInstance().getCountryNameByCountryID(this->charbase.country),this->charbase.name );
						
						}
						
						
					}

					char a[16];
					sprintf(a, "%d", atoi(Zebra::global["ZXCWC"].c_str())+1);
					Zebra::global["ZXCWC"]=a;
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"恭喜您，完成今日自行车比赛");

				}
				else{
					char text[256];
					sprintf(text,"请前往 %d号赛车女郎 继续比赛", charbase.bikenum+1);
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
				}
				
			}
			
		}
		break;
		case BIKE_SKILL:
		{
			stBikeSkillCmd *cmd = (stBikeSkillCmd *)(ptCmd);
			//缩骨丹占用临时
			if(cmd->type==99)
			{
				if(this->issetUState(Cmd::USTATE_SUOGUDAN))
				{
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL, "您正在缩小状态，无法继续使用");
				}
				else{
					this->sendSkill(4000, 1);
					// useSkill(this, 4000);
					this->showCurrentEffect(USTATE_SUOGUDAN, true ); 
					Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "打开缩小效果成功！");
					// zObject *itemobj = this->packs.uom.getObjectByThisID(cmd->itemID);
					// if(itemobj)
					// {
					// 	if (itemobj->data.dwNum > 1) // soke 大于9朵直接-9
					// 	{
					// 		itemobj->data.dwNum-=1;
					// 		Cmd::stRefCountObjectPropertyUserCmd send;
					// 		send.qwThisID = itemobj->data.qwThisID;
					// 		send.dwNum = itemobj->data.dwNum;
					// 		sendCmdToMe(&send, sizeof(send));
					// 	}
					// 	else
					// 	{
					// 		this->packs.removeObject(itemobj);
					// 	}
					// }
					
				}
				return true;
				
			}
			if(cmd->type==100)
			{
				
				this->showCurrentEffect(USTATE_SUOGUDAN, false ); 
				Channel::sendSys(this, Cmd::INFO_TYPE_GAME, "解除缩小效果成功！");
				return true;
				
			}

			if(!this->isBike())
			{
					char text[256];
					sprintf(text,"您没有参加自行车比赛，无法使用");
					Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,text);	
					Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,text);
					return false;
			}
			
			if(cmd->type==1)
			{
				for(int i=0;i<10;i++)
				{
					SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(cmd->playerid[i]); // 玩家B
					if(pUser && pUser->isBike())
					{
						pUser->showCurrentEffect(USTATE_USER_BIKESKILL1, true ); 
					}

				}

				// zObject *itemobj = this->packs.uom.getObjectByThisID(cmd->itemID);
				// if (itemobj->data.dwNum > 1) // soke 大于9朵直接-9
				// {
				// 	itemobj->data.dwNum-=1;
				// 	Cmd::stRefCountObjectPropertyUserCmd send;
				// 	send.qwThisID = itemobj->data.qwThisID;
				// 	send.dwNum = itemobj->data.dwNum;
				// 	sendCmdToMe(&send, sizeof(send));
				// }
				// else
				// {
				// 	this->packs.removeObject(itemobj);
				// }
			}
			else if(cmd->type==2)
			{
				for(int i=0;i<10;i++)
				{
					SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(cmd->playerid[i]); // 玩家B
					if(pUser && pUser->isBike())
					{
						pUser->showCurrentEffect(USTATE_USER_BIKESKILL2, true ); 
					}

				}

				// zObject *itemobj = this->packs.uom.getObjectByThisID(cmd->itemID);
				// if (itemobj->data.dwNum > 1) // soke 大于9朵直接-9
				// {
				// 	itemobj->data.dwNum-=1;
				// 	Cmd::stRefCountObjectPropertyUserCmd send;
				// 	send.qwThisID = itemobj->data.qwThisID;
				// 	send.dwNum = itemobj->data.dwNum;
				// 	sendCmdToMe(&send, sizeof(send));
				// }
				// else
				// {
				// 	this->packs.removeObject(itemobj);
				// }


			}
			else if(cmd->type==3)
			{
				for(int i=0;i<10;i++)
				{
					SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(cmd->playerid[i]); // 玩家B
					if(pUser && pUser->isBike())
					{
						pUser->showCurrentEffect(USTATE_USER_BIKESKILL3, true ); 
					}

				}

				// zObject *itemobj = this->packs.uom.getObjectByThisID(cmd->itemID);
				// if(itemobj)
				// {
				// 	if (itemobj->data.dwNum > 1) // soke 大于9朵直接-9
				// 	{
				// 		itemobj->data.dwNum-=1;
				// 		Cmd::stRefCountObjectPropertyUserCmd send;
				// 		send.qwThisID = itemobj->data.qwThisID;
				// 		send.dwNum = itemobj->data.dwNum;
				// 		sendCmdToMe(&send, sizeof(send));
				// 	}
				// 	else
				// 	{
				// 		this->packs.removeObject(itemobj);
				// 	}
				// }
			}
			else if(cmd->type==4)
			{
				for(int i=0;i<10;i++)
				{
					SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(cmd->playerid[i]); // 玩家B
					if(pUser && pUser->isBike())
					{
						pUser->showCurrentEffect(USTATE_USER_BIKESKILL4, true ); 
					}

				}

				// zObject *itemobj = this->packs.uom.getObjectByThisID(cmd->itemID);
				// if(itemobj)
				// {
				// 	if (itemobj->data.dwNum > 1) // soke 大于9朵直接-9
				// 	{
				// 		itemobj->data.dwNum-=1;
				// 		Cmd::stRefCountObjectPropertyUserCmd send;
				// 		send.qwThisID = itemobj->data.qwThisID;
				// 		send.dwNum = itemobj->data.dwNum;
				// 		sendCmdToMe(&send, sizeof(send));
				// 	}
				// 	else
				// 	{
				// 		this->packs.removeObject(itemobj);
				// 	}
				// }
			}
			else if(cmd->type==5)//加速
			{
			
				this->showCurrentEffect(USTATE_USER_BIKESKILL5, true ); 
				//刷新用户数据
				this->sendSkill(3200, 1);
				// zObject *itemobj = this->packs.uom.getObjectByThisID(cmd->itemID);
				// if (itemobj->data.dwNum > 1) // soke 大于9朵直接-9
				// {
				// 	itemobj->data.dwNum-=1;
				// 	Cmd::stRefCountObjectPropertyUserCmd send;
				// 	send.qwThisID = itemobj->data.qwThisID;
				// 	send.dwNum = itemobj->data.dwNum;
				// 	sendCmdToMe(&send, sizeof(send));
				// }
				// else
				// {
				// 	this->packs.removeObject(itemobj);
				// }
			}
			else if(cmd->type==6)
			{
				for(int i=0;i<10;i++)
				{
					SceneUser *pUser = SceneUserManager::getMe().getUserByTempID(cmd->playerid[i]); // 玩家B
					if(pUser && pUser->isBike())
					{
						pUser->showCurrentEffect(USTATE_USER_BIKESKILL6, true ); 
					}

				}

				// zObject *itemobj = this->packs.uom.getObjectByThisID(cmd->itemID);
				// if(itemobj)
				// {
				// 	if (itemobj->data.dwNum > 1) // soke 大于9朵直接-9
				// 	{
				// 		itemobj->data.dwNum-=1;
				// 		Cmd::stRefCountObjectPropertyUserCmd send;
				// 		send.qwThisID = itemobj->data.qwThisID;
				// 		send.dwNum = itemobj->data.dwNum;
				// 		sendCmdToMe(&send, sizeof(send));
				// 	}
				// 	else
				// 	{
				// 		this->packs.removeObject(itemobj);
				// 	}
				// }
				
			}
		}
		break;
	}
	return true;
}
