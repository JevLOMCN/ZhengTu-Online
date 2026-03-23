#include "SceneUser.h"
#include "SceneNpcManager.h"
#include "NpcTrade.h"
#include "RebuildObject.h"
#include "ScenesServer.h"
#include "CharBase.h"
#include "Command.h"
#include <string>
// 天下第一 Sc天下第一 天下第一系统 by:醉梦
bool SceneUser::doTianxiaCmd(const Cmd::stTianxiaUserCmd *ptCmd, unsigned int cmdLen)
{
	using namespace Cmd;
	switch (ptCmd->byParam) //这里就是判断子命令了byParam
	{
		case TIANXIA_BAOMING:
		{
			struct tm tv1;
			time_t timValue = time(NULL);
			zRTime::getLocalTime(tv1, timValue);
			if (tv1.tm_hour < 18)
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"每周18点才可报名天下第一大赛");	
				return false;
			}
			if(this->getLevel() < 190)
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"您的等级不够，190级才可报名天下第一大赛");	
				return false;
			}
			

			bool isbaoming = false;
			for (int i = 0; i < 32; i++)
			{
				if(ScenesService::getInstance().tianxia[i].userid == this->id)
				{
					isbaoming = true;
					break;
				}
			}
			if(isbaoming)
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"您已经报名了天下第一大赛");	
				return false;
			}

			bool baoming = false;
			for (int i = 0; i < 32; i += 2)
			{
				if(ScenesService::getInstance().tianxia[i].userid == 0)
				{
					ScenesService::getInstance().tianxia[i].userid=this->id;
					bcopy(this->name,ScenesService::getInstance().tianxia[i].username,MAX_NAMESIZE);
					baoming = true;
					break;
				}
			}
			if(!baoming)
			{	
				for (int i = 1; i < 32; i += 2)
				{
					if(ScenesService::getInstance().tianxia[i].userid == 0)
					{
						ScenesService::getInstance().tianxia[i].userid=this->id;
						bcopy(this->name,ScenesService::getInstance().tianxia[i].username,MAX_NAMESIZE);
						baoming = true;
						break;
					}
				}
			}
			
			if(baoming)
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"报名天下第一大赛成功");	
				Cmd::stTianxiaReturnInfoCmd Send;
				memcpy(Send.tianxia,ScenesService::getInstance().tianxia,sizeof(Send.tianxia));
				
				struct tm tv1;
				time_t timValue = time(NULL);
				zRTime::getLocalTime(tv1, timValue);
				if (tv1.tm_hour < 22)
				{
					//8点之前 报名人显示为神秘人
					for (int i = 0; i < 32; i++)
					{
						sprintf(Send.tianxia[i].username,"神秘人[开赛显示]");
						
					}
				}
				sendCmdToMe(&Send, sizeof(Send));
			}
			else{
				Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"报名天下第一大赛失败，人数已满");	
			}
			
		}
		break;
		case TIANXIA_BAOMING_BACK:
		{
			bool baoming = false;
			for (int i = 0; i < 32; i++)
			{
				if(ScenesService::getInstance().tianxia[i].userid == this->id)
				{
					ScenesService::getInstance().tianxia[i].userid=0;
					bcopy("",ScenesService::getInstance().tianxia[i].username,MAX_NAMESIZE);
					baoming = true;
					break;
				}
			}
			if(baoming)
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"退赛天下第一大赛成功");	
				Cmd::stTianxiaReturnInfoCmd Send;
				memcpy(Send.tianxia,ScenesService::getInstance().tianxia,sizeof(Send.tianxia));
				sendCmdToMe(&Send, sizeof(Send));
			}
			else{
				Channel::sendSys(this, Cmd::INFO_TYPE_GAME,"退赛天下第一大赛失败");	
			}
			
		}
		break;
		case TIANXIA_INFO:
		{
			Cmd::stTianxiaReturnInfoCmd Send;
			memcpy(Send.tianxia,ScenesService::getInstance().tianxia,sizeof(Send.tianxia));
			
			struct tm tv1;
			time_t timValue = time(NULL);
			zRTime::getLocalTime(tv1, timValue);
			if (tv1.tm_hour < 22)
			{
				//8点之前 报名人显示为神秘人
				for (int i = 0; i < 32; i++)
				{
					sprintf(Send.tianxia[i].username,"神秘人[开赛显示]");
					
				}
			}
			sendCmdToMe(&Send, sizeof(Send));
		}
		break;
		case TIANXIA_DUANWEI://天下第一段位奖励
		{
			int duanweilevel = this->charbase.duanweiexp/1000;

			if(duanweilevel <1)
			{
				Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"您的段位不够，无法领取奖励");	
				return false;
			}

			if(this->charbase.duanweilingqu < duanweilevel)
			{
				switch (duanweilevel)
				{
					case 1:
						{
							Gm::fetch(this,"id=35000 num=50 bind=1");//自己改下每个级别的奖励就行了
							Gm::fetch(this,"id=50000 num=100 bind=1");
							Gm::fetch(this,"id=3540 num=100 bind=1");
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"段位奖励领取成功！");
							Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,"段位奖励领取成功！");
						}
						break;
					case 2:
						{
							Gm::fetch(this,"id=35000 num=100 bind=1");//自己改下每个级别的奖励就行了
							Gm::fetch(this,"id=50000 num=300 bind=1");
							Gm::fetch(this,"id=3540 num=200 bind=1");
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"段位奖励领取成功！");
							Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,"段位奖励领取成功！");
						}
						break;
					case 3:
						{
							Gm::fetch(this,"id=35000 num=200 bind=1");//自己改下每个级别的奖励就行了
							Gm::fetch(this,"id=50000 num=500 bind=1");
							Gm::fetch(this,"id=3540 num=400 bind=1");
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"段位奖励领取成功！");
							Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,"段位奖励领取成功！");
						}
						break;
					case 4:
						{
							Gm::fetch(this,"id=35000 num=300 bind=1");//自己改下每个级别的奖励就行了
							Gm::fetch(this,"id=50000 num=700 bind=1");
							Gm::fetch(this,"id=3540 num=600 bind=1");
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"段位奖励领取成功！");
							Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,"段位奖励领取成功！");
						}
						break;
					case 5:
						{
							Gm::fetch(this,"id=35000 num=400 bind=1");//自己改下每个级别的奖励就行了
							Gm::fetch(this,"id=50000 num=900 bind=1");
							Gm::fetch(this,"id=3540 num=800 bind=1");
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"段位奖励领取成功！");
							Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,"段位奖励领取成功！");
						}
						break;
					case 6:
						{
							Gm::fetch(this,"id=35000 num=500 bind=1");//自己改下每个级别的奖励就行了
							Gm::fetch(this,"id=50000 num=1100 bind=1");
							Gm::fetch(this,"id=3540 num=1000 bind=1");
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"段位奖励领取成功！");
							Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,"段位奖励领取成功！");
						}
						break;
					case 7:
						{
							Gm::fetch(this,"id=35000 num=600 bind=1");//自己改下每个级别的奖励就行了
							Gm::fetch(this,"id=50000 num=1300 bind=1");
							Gm::fetch(this,"id=3540 num=1200 bind=1");
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"段位奖励领取成功！");
							Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,"段位奖励领取成功！");
						}
						break;
					case 8:
						{
							Gm::fetch(this,"id=35000 num=700 bind=1");//自己改下每个级别的奖励就行了
							Gm::fetch(this,"id=50000 num=1500 bind=1");
							Gm::fetch(this,"id=3540 num=1400 bind=1");
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"段位奖励领取成功！");
							Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,"段位奖励领取成功！");
						}
						break;
					case 9:
						{
							Gm::fetch(this,"id=35000 num=800 bind=1");//自己改下每个级别的奖励就行了
							Gm::fetch(this,"id=50000 num=1700 bind=1");
							Gm::fetch(this,"id=3540 num=1600 bind=1");
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"段位奖励领取成功！");
							Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,"段位奖励领取成功！");
						}
						break;
					case 10:
						{
							Gm::fetch(this,"id=35000 num=900 bind=1");//自己改下每个级别的奖励就行了
							Gm::fetch(this,"id=50000 num=2000 bind=1");
							Gm::fetch(this,"id=3540 num=1800 bind=1");
							Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"段位奖励领取成功！");
							Channel::sendSys(this, Cmd::INFO_TYPE_EXP5,"段位奖励领取成功！");
						}
						break;
					//后面复制继续写就行
				}
				this->charbase.duanweilingqu = duanweilevel;
			}
			else{
				Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"你已经领取过当前段位等级奖励了");	
			}
		}
		break;
	}
	return true;
}
