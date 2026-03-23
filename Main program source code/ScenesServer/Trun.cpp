#include <math.h>
#include <sstream>
#include "SceneUser.h"
#include "Scene.h"
#include "Chat.h"
#include "QuestEvent.h"
#include "QuestTable.h"
#include "RebuildObject.h"
#include "SessionClient.h"
#include "Command.h"
#include "SessionCommand.h"
#include "SceneManager.h"
#include "ScenesServer.h"
#include "script.h"
#include "Horse.h"

//zise 2016-11-12 转生功能
bool SceneUser::doTurnCmd(const Cmd::stTurnUserCmd *ptCmd,unsigned int cmdLen)
{
	using namespace Cmd;
	switch(ptCmd->byParam)
	{
	case SET_TRUN_REQUEST_PARAMETER:
		{

			DWORD dMaxRound = 1;
			if(charbase.round >= dMaxRound )
			{
			    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"你已经达到最大转生上限 %d转，不能转生!",dMaxRound);
				return false;
			}

			WORD zslevel=atoi(Zebra::global["servermax_level"].c_str());
			if(charbase.level < zslevel)
			{
			    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"你还没有达到可以转生的等级!");
				return false;
			}

			//检查是否有职务
			if(king || unionMaster || septMaster || emperor)
			{
			    Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"请确定身上的职务已经辞掉!");
				return false;
			}


			//让用户下马
			horse.mount(false);
			// 清除功勋和文采值
			this->charbase.grace = 0;
			this->charbase.exploit = 0;
					
			//回到清源村 
			//Gm::gomap_Gm(this,"name=新手村 ");
			Gm::goHome(this,NULL);
			Gm::goTo_Gm(this,"120,93");

			//清除技能,回复基础值,增加点数 桃子转生后等级
			charbase.level = 160;

			int round = charbase.round;
			round += 1;

			charbase.points = charbase.level*3 + round*atoi(Zebra::global["trun_point_rate"].c_str());

		    charbase.wdProperty[0]=0;
		    charbase.wdProperty[1]=0;
			charbase.wdProperty[2]=0;
			charbase.wdProperty[3]=0;
			charbase.wdProperty[4]=0;
			this->setupCharBase();

			usm.clear();
			charbase.skillpoint = atoi(Zebra::global["trun_skill_rate"].c_str())*round;
            Cmd::stClearSkillUserCmd send1;
            sendCmdToMe(&send1,sizeof(send1));
            skillValue.init();

			setupCharBase();

			charbase.round = round;

			Cmd::stMainUserDataUserCmd  userinfo;
			full_t_MainUserData(userinfo.data);
			sendCmdToMe(&userinfo,sizeof(userinfo));

			Zebra::logger->error("round = %d",this->charbase.round);
		    Zebra::logger->error("收到转生结果检查消息");

			Channel::sendSys(this, Cmd::INFO_TYPE_FAIL,"恭喜你转生成功!");
			return true;
		}
		break;
	default:
		break;
	}
	return true;
}

