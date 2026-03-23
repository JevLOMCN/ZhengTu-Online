/**
 * \file
 * \version  $Id: TimeTick.cpp  $
 * \author  
 * \date 
 * \brief 时间回调函数
 */


#include <iostream>
#include <string>
#include <vector>
#include <ext/hash_map>

#include "Zebra.h"
#include "zThread.h"
#include "TimeTick.h"
#include "ScenesServer.h"
#include "Command.h"
#include "SceneNpc.h"
#include "SceneUserManager.h"
#include "Scene.h"
#include "SceneManager.h"
#include "QuestEvent.h"
#include "QuestTable.h"
#include "CountryDare.h"
#include "SceneTaskManager.h"
#include "SessionClient.h"
#include "RecordClient.h"
#include "FuBenMgr.h" //副本
#define MAX_CMD_GROUP 10
zRTime SceneTimeTick::currentTime;
SceneTimeTick *SceneTimeTick::instance = NULL;

/**
 * \brief 地图回调函数
 */
struct EverySceneEntryAction : public SceneCallBack
{
	const DWORD group;
	EverySceneEntryAction(const DWORD group) : group(group) {}
	bool exec(Scene *scene)
	{
		if (scene)
		{
			if(scene->SceneEntryAction(SceneTimeTick::currentTime, group))
			{
				/**
				 * 这里如果需要动态加载卸载地图会死锁哦
				 * ^-^ 不会死锁，现在单线程，场景管理器没有锁，只是要注意遍历的容器的迭代器
				 */
				SceneTaskManager::getInstance().execEvery();
			}
		}
		return true;
	}
};

/**
 * \brief 把特殊npc加到ai处理列表中
 */
/*class AddSpecialNpcCallBack : public specialNpcCallBack
{
	private:
		MonkeyNpcs &affectNpc;
		const DWORD group;
		const bool _every;
	public:
		AddSpecialNpcCallBack(MonkeyNpcs &affectNpc, const DWORD g, const bool every):affectNpc(affectNpc),group(g),_every(every){}
		bool exec(SceneNpc * npc)
		{
			if (_every || npc->tempid%MAX_NPC_GROUP==group)
			{
				affectNpc.insert(npc);
			}
			return true;
		}
};*/

/**
 * \brief 时间循环，发送定时事件，处理受影响npc ai等
 */
void SceneTimeTick::run()
{
	const int timeout_value = 500;
	const int timeout_value2 = 300;
	DWORD step = 0;
	int t = 0;
	while(!isFinal())
	{
		zThread::msleep((10-t)>0?(10-t):1);
		//获取当前时间
		currentTime.now();

		if (_five_sec(currentTime)) {
			OnTimer event(1);
			EventTable::instance().execute(event);
			ScenesService::getInstance().checkAndReloadConfig();
		}

		sessionClient->doCmd();
		recordClient->doCmd();
		SceneTaskManager::getInstance().execEvery();

		//specialNpc
		//MonkeyNpcs affectNpc;
		//AddSpecialNpcCallBack asncb(affectNpc, step, t > timeout_value2);
		//SceneNpcManager::getMe().execAllSpecialNpc(asncb);
		SceneNpc::AI(currentTime, SceneNpcManager::getMe().getSepcialNpc(),step,t > timeout_value2);

		//汉国国家镖车处理
		if(ScenesService::getInstance().han_biaoche!=NULL)//镖车存在
		{
			ScenesService::getInstance().han_biaoche->skillStatusM.clearActiveSkillStatusOnlyUseToStatus48();
			if (ScenesService::getInstance().han_biaoche->checkMoveTime(SceneTimeTick::currentTime) && ScenesService::getInstance().han_biaoche->canMove())
			{
				zPos pos;
				pos = zPos(865, 572);
				Scene * scene = SceneManager::getInstance().getSceneByName("汉国·王城");
				if(ScenesService::getInstance().hanbiaoche_type == 0)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(723, 713);
				if(ScenesService::getInstance().hanbiaoche_type == 1)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(785, 808);
				if(ScenesService::getInstance().hanbiaoche_type == 2)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				pos = zPos(833, 876);
				if(ScenesService::getInstance().hanbiaoche_type == 3)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				pos = zPos(913, 858);
				if(ScenesService::getInstance().hanbiaoche_type == 4)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				pos = zPos(939, 939);
				if(ScenesService::getInstance().hanbiaoche_type == 5)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				if(ScenesService::getInstance().hanbiaoche_type == 6)
				{
					Scene * s = SceneManager::getInstance().getSceneByName("汉国·南郊");
					if (!s)
					{
						Zebra::logger->info("doChangeMapAI:npc跳转地图失败,未找到地图汉国·东郊");
						return;
					}
					if(ScenesService::getInstance().han_biaoche->changeMap(s, zPos(26, 24)))
					{
						ScenesService::getInstance().hanbiaoche_type++;
						for (SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); iter != SceneManager::getInstance().country_info.end();
						 iter++)
						{
							Cmd::Session::t_countryNotify_SceneSession send;
							bzero(send.info, sizeof(send.info));
							sprintf(send.info, "汉国的国家镖车出现在南郊");
							send.dwCountryID = iter->second.id;
							sessionClient->sendCmd(&send, sizeof(send));
						}
						Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"汉国的国家镖车出现在南郊");
					}
				}
				Scene * scene2 = SceneManager::getInstance().getSceneByName("汉国·南郊");

				pos = zPos(55,49);
				if(ScenesService::getInstance().hanbiaoche_type == 7)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(76,58);
				if(ScenesService::getInstance().hanbiaoche_type == 8)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(81,46);
				if(ScenesService::getInstance().hanbiaoche_type == 9)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(87,52);
				if(ScenesService::getInstance().hanbiaoche_type == 10)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(81,58);
				if(ScenesService::getInstance().hanbiaoche_type == 11)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(87,72);
				if(ScenesService::getInstance().hanbiaoche_type == 12)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					ScenesService::getInstance().han_biaoche->warp(pos);
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(89,91);
				if(ScenesService::getInstance().hanbiaoche_type == 13)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				pos = zPos(128,114);
				if(ScenesService::getInstance().hanbiaoche_type == 14)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				pos = zPos(161,108);
				if(ScenesService::getInstance().hanbiaoche_type == 15)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(172,89);
				if(ScenesService::getInstance().hanbiaoche_type == 16)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				pos = zPos(218,52);
				if(ScenesService::getInstance().hanbiaoche_type == 17)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(260,106);
				if(ScenesService::getInstance().hanbiaoche_type == 18)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(278,93);
				if(ScenesService::getInstance().hanbiaoche_type == 19)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(295,109);
				if(ScenesService::getInstance().hanbiaoche_type == 20)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(311,157);
				if(ScenesService::getInstance().hanbiaoche_type == 21)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(310,183);
				if(ScenesService::getInstance().hanbiaoche_type == 22)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(332,180);
				if(ScenesService::getInstance().hanbiaoche_type == 23)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				
				pos = zPos(337,151);
				if(ScenesService::getInstance().hanbiaoche_type == 24)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(356,142);
				if(ScenesService::getInstance().hanbiaoche_type == 25)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(370,116);
				if(ScenesService::getInstance().hanbiaoche_type == 26)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(381,116);
				if(ScenesService::getInstance().hanbiaoche_type == 27)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				if(ScenesService::getInstance().hanbiaoche_type == 28)
				{
					Scene * s = SceneManager::getInstance().getSceneByName("汉国·边境");
					if (!s)
					{
						Zebra::logger->info("doChangeMapAI:npc跳转地图失败,未找到地图汉国·边境");
						return;
					}
					if(ScenesService::getInstance().han_biaoche->changeMap(s, zPos(31,277)))
					{
						ScenesService::getInstance().hanbiaoche_type = 29;
						for (SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); iter != SceneManager::getInstance().country_info.end();
						 iter++)
						{
							Cmd::Session::t_countryNotify_SceneSession send;
							bzero(send.info, sizeof(send.info));
							sprintf(send.info, "汉国的国家镖车出现在边境");
							send.dwCountryID = iter->second.id;
							sessionClient->sendCmd(&send, sizeof(send));
						}
						Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"汉国的国家镖车出现在边境");
					}
				
				}
				Scene * scene3 = SceneManager::getInstance().getSceneByName("汉国·边境");
				pos = zPos(44,260);
				if(ScenesService::getInstance().hanbiaoche_type == 29)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(64,259);
				if(ScenesService::getInstance().hanbiaoche_type == 30)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(72,242);
				if(ScenesService::getInstance().hanbiaoche_type == 31)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(77,224);
				if(ScenesService::getInstance().hanbiaoche_type == 32)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(104,197);
				if(ScenesService::getInstance().hanbiaoche_type == 33)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(130,183);
				if(ScenesService::getInstance().hanbiaoche_type == 34)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(167,185);
				if(ScenesService::getInstance().hanbiaoche_type == 35)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(177,168);
				if(ScenesService::getInstance().hanbiaoche_type == 36)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(197,157);
				if(ScenesService::getInstance().hanbiaoche_type == 37)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				pos = zPos(219,176);
				if(ScenesService::getInstance().hanbiaoche_type == 38)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				pos = zPos(232,212);
				if(ScenesService::getInstance().hanbiaoche_type == 39)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				pos = zPos(245,195);
				if(ScenesService::getInstance().hanbiaoche_type == 40)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(259,196);
				if(ScenesService::getInstance().hanbiaoche_type == 41)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(261,216);
				if(ScenesService::getInstance().hanbiaoche_type == 42)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(288,241);
				if(ScenesService::getInstance().hanbiaoche_type == 43)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(311,237);
				if(ScenesService::getInstance().hanbiaoche_type == 44)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(325,259);
				if(ScenesService::getInstance().hanbiaoche_type == 45)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(349,254);
				if(ScenesService::getInstance().hanbiaoche_type == 46)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(364,244);
				if(ScenesService::getInstance().hanbiaoche_type == 47)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(361,222);
				if(ScenesService::getInstance().hanbiaoche_type == 48)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(362,194);
				if(ScenesService::getInstance().hanbiaoche_type == 49)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(363,179);
				if(ScenesService::getInstance().hanbiaoche_type == 50)
				{
					if(!ScenesService::getInstance().han_biaoche->gotoFindPath(ScenesService::getInstance().han_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche->getPos(), pos, 2)))
					{
						for (SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); iter != SceneManager::getInstance().country_info.end();
						 iter++)
						{
							Cmd::Session::t_countryNotify_SceneSession send;
							bzero(send.info, sizeof(send.info));
							sprintf(send.info, "汉国的国家镖车已安全送达到边境梁镖头处");
							send.dwCountryID = iter->second.id;
							sessionClient->sendCmd(&send, sizeof(send));
						}
						Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"汉国的国家镖车已安全送达到边境梁镖头处");
						ScenesService::getInstance().han_biaoche->setClearState();
						ScenesService::getInstance().han_biaoche = NULL;
						ScenesService::getInstance().hanbiaoche_type = 0;
						//国家镖车奖励发放
						SceneUserManager::getMe().GuoJiaBiaoCheJiangLi1(scene3);
					}
				}
			}
			
		}
		else if(ScenesService::getInstance().han_biaoche2!=NULL)
		{
			ScenesService::getInstance().han_biaoche2->skillStatusM.clearActiveSkillStatusOnlyUseToStatus48();
			if (ScenesService::getInstance().han_biaoche2->checkMoveTime(SceneTimeTick::currentTime) && ScenesService::getInstance().han_biaoche2->canMove())
			{
				zPos pos;
				pos = zPos(865, 572);
				Scene * scene = SceneManager::getInstance().getSceneByName("汉国·王城");
				if(ScenesService::getInstance().hanbiaoche_type == 0)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(723, 713);
				if(ScenesService::getInstance().hanbiaoche_type == 1)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(785, 808);
				if(ScenesService::getInstance().hanbiaoche_type == 2)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				pos = zPos(833, 876);
				if(ScenesService::getInstance().hanbiaoche_type == 3)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				pos = zPos(913, 858);
				if(ScenesService::getInstance().hanbiaoche_type == 4)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				pos = zPos(939, 939);
				if(ScenesService::getInstance().hanbiaoche_type == 5)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				if(ScenesService::getInstance().hanbiaoche_type == 6)
				{
					Scene * s = SceneManager::getInstance().getSceneByName("汉国·南郊");
					if (!s)
					{
						Zebra::logger->info("doChangeMapAI:npc跳转地图失败,未找到地图汉国·东郊");
						return;
					}
					if(ScenesService::getInstance().han_biaoche2->changeMap(s, zPos(26, 24)))
					{
						ScenesService::getInstance().hanbiaoche_type++;
						for (SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); iter != SceneManager::getInstance().country_info.end();
						 iter++)
						{
							Cmd::Session::t_countryNotify_SceneSession send;
							bzero(send.info, sizeof(send.info));
							sprintf(send.info, "汉国的国家镖车出现在南郊");
							send.dwCountryID = iter->second.id;
							sessionClient->sendCmd(&send, sizeof(send));
						}
						Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"汉国的国家镖车出现在南郊");
					}
				}
				Scene * scene2 = SceneManager::getInstance().getSceneByName("汉国·南郊");

				pos = zPos(55,49);
				if(ScenesService::getInstance().hanbiaoche_type == 7)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(76,58);
				if(ScenesService::getInstance().hanbiaoche_type == 8)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(81,46);
				if(ScenesService::getInstance().hanbiaoche_type == 9)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(87,52);
				if(ScenesService::getInstance().hanbiaoche_type == 10)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(81,58);
				if(ScenesService::getInstance().hanbiaoche_type == 11)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(87,72);
				if(ScenesService::getInstance().hanbiaoche_type == 12)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					ScenesService::getInstance().han_biaoche2->warp(pos);
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(89,91);
				if(ScenesService::getInstance().hanbiaoche_type == 13)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				pos = zPos(128,114);
				if(ScenesService::getInstance().hanbiaoche_type == 14)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				pos = zPos(161,108);
				if(ScenesService::getInstance().hanbiaoche_type == 15)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(172,89);
				if(ScenesService::getInstance().hanbiaoche_type == 16)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				pos = zPos(218,52);
				if(ScenesService::getInstance().hanbiaoche_type == 17)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(260,106);
				if(ScenesService::getInstance().hanbiaoche_type == 18)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(278,93);
				if(ScenesService::getInstance().hanbiaoche_type == 19)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(295,109);
				if(ScenesService::getInstance().hanbiaoche_type == 20)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(311,157);
				if(ScenesService::getInstance().hanbiaoche_type == 21)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(310,183);
				if(ScenesService::getInstance().hanbiaoche_type == 22)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(332,180);
				if(ScenesService::getInstance().hanbiaoche_type == 23)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				
				pos = zPos(337,151);
				if(ScenesService::getInstance().hanbiaoche_type == 24)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(356,142);
				if(ScenesService::getInstance().hanbiaoche_type == 25)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(370,116);
				if(ScenesService::getInstance().hanbiaoche_type == 26)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(381,116);
				if(ScenesService::getInstance().hanbiaoche_type == 27)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				if(ScenesService::getInstance().hanbiaoche_type == 28)
				{
					Scene * s = SceneManager::getInstance().getSceneByName("汉国·边境");
					if (!s)
					{
						Zebra::logger->info("doChangeMapAI:npc跳转地图失败,未找到地图汉国·边境");
						return;
					}
					if(ScenesService::getInstance().han_biaoche2->changeMap(s, zPos(31,277)))
					{
						ScenesService::getInstance().hanbiaoche_type = 29;
						for (SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); iter != SceneManager::getInstance().country_info.end();
						 iter++)
						{
							Cmd::Session::t_countryNotify_SceneSession send;
							bzero(send.info, sizeof(send.info));
							sprintf(send.info, "汉国的国家镖车出现在边境");
							send.dwCountryID = iter->second.id;
							sessionClient->sendCmd(&send, sizeof(send));
						}
						Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"汉国的国家镖车出现在边境");
					}
				
				}
				Scene * scene3 = SceneManager::getInstance().getSceneByName("汉国·边境");
				pos = zPos(44,260);
				if(ScenesService::getInstance().hanbiaoche_type == 29)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(64,259);
				if(ScenesService::getInstance().hanbiaoche_type == 30)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(72,242);
				if(ScenesService::getInstance().hanbiaoche_type == 31)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(77,224);
				if(ScenesService::getInstance().hanbiaoche_type == 32)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(104,197);
				if(ScenesService::getInstance().hanbiaoche_type == 33)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(130,183);
				if(ScenesService::getInstance().hanbiaoche_type == 34)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(167,185);
				if(ScenesService::getInstance().hanbiaoche_type == 35)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(177,168);
				if(ScenesService::getInstance().hanbiaoche_type == 36)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(197,157);
				if(ScenesService::getInstance().hanbiaoche_type == 37)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				pos = zPos(219,176);
				if(ScenesService::getInstance().hanbiaoche_type == 38)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				pos = zPos(232,212);
				if(ScenesService::getInstance().hanbiaoche_type == 39)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}

				pos = zPos(245,195);
				if(ScenesService::getInstance().hanbiaoche_type == 40)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(259,196);
				if(ScenesService::getInstance().hanbiaoche_type == 41)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(261,216);
				if(ScenesService::getInstance().hanbiaoche_type == 42)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(288,241);
				if(ScenesService::getInstance().hanbiaoche_type == 43)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(311,237);
				if(ScenesService::getInstance().hanbiaoche_type == 44)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(325,259);
				if(ScenesService::getInstance().hanbiaoche_type == 45)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(349,254);
				if(ScenesService::getInstance().hanbiaoche_type == 46)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(364,244);
				if(ScenesService::getInstance().hanbiaoche_type == 47)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(361,222);
				if(ScenesService::getInstance().hanbiaoche_type == 48)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(362,194);
				if(ScenesService::getInstance().hanbiaoche_type == 49)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().hanbiaoche_type++;;
					}
				}
				pos = zPos(363,179);
				if(ScenesService::getInstance().hanbiaoche_type == 50)
				{
					if(!ScenesService::getInstance().han_biaoche2->gotoFindPath(ScenesService::getInstance().han_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().han_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().han_biaoche2->getPos(), pos, 2)))
					{
						for (SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); iter != SceneManager::getInstance().country_info.end();
						 iter++)
						{
							Cmd::Session::t_countryNotify_SceneSession send;
							bzero(send.info, sizeof(send.info));
							sprintf(send.info, "汉国的国家镖车(已被劫掠)已安全送达到边境梁镖头处");
							send.dwCountryID = iter->second.id;
							sessionClient->sendCmd(&send, sizeof(send));
						}
						Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"汉国的国家镖车(已被劫掠)已安全送达到边境梁镖头处");
						ScenesService::getInstance().han_biaoche2->setClearState();
						ScenesService::getInstance().han_biaoche2 = NULL;
						ScenesService::getInstance().hanbiaoche_type = 0;
						//国家镖车奖励发放
						SceneUserManager::getMe().GuoJiaBiaoCheJiangLi2(scene3);
					}
				}
			}
		}

		//楚国国家镖车处理
		if(ScenesService::getInstance().chu_biaoche!=NULL)//镖车存在
		{
			ScenesService::getInstance().chu_biaoche->skillStatusM.clearActiveSkillStatusOnlyUseToStatus48();
			if (ScenesService::getInstance().chu_biaoche->checkMoveTime(SceneTimeTick::currentTime) && ScenesService::getInstance().chu_biaoche->canMove())
			{
				zPos pos;
				pos = zPos(865, 572);
				Scene * scene = SceneManager::getInstance().getSceneByName("楚国·王城");
				if(ScenesService::getInstance().chubiaoche_type == 0)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(723, 713);
				if(ScenesService::getInstance().chubiaoche_type == 1)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(785, 808);
				if(ScenesService::getInstance().chubiaoche_type == 2)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				pos = zPos(833, 876);
				if(ScenesService::getInstance().chubiaoche_type == 3)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				pos = zPos(913, 858);
				if(ScenesService::getInstance().chubiaoche_type == 4)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				pos = zPos(939, 939);
				if(ScenesService::getInstance().chubiaoche_type == 5)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				if(ScenesService::getInstance().chubiaoche_type == 6)
				{
					Scene * s = SceneManager::getInstance().getSceneByName("楚国·南郊");
					if (!s)
					{
						Zebra::logger->info("doChangeMapAI:npc跳转地图失败,未找到地图楚国·东郊");
						return;
					}
					if(ScenesService::getInstance().chu_biaoche->changeMap(s, zPos(26, 24)))
					{
						ScenesService::getInstance().chubiaoche_type++;
						for (SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); iter != SceneManager::getInstance().country_info.end();
						 iter++)
						{
							Cmd::Session::t_countryNotify_SceneSession send;
							bzero(send.info, sizeof(send.info));
							sprintf(send.info, "楚国的国家镖车出现在南郊");
							send.dwCountryID = iter->second.id;
							sessionClient->sendCmd(&send, sizeof(send));
						}
						Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"楚国的国家镖车出现在南郊");
					}
				}
				Scene * scene2 = SceneManager::getInstance().getSceneByName("楚国·南郊");

				pos = zPos(55,49);
				if(ScenesService::getInstance().chubiaoche_type == 7)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(76,58);
				if(ScenesService::getInstance().chubiaoche_type == 8)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(81,46);
				if(ScenesService::getInstance().chubiaoche_type == 9)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(87,52);
				if(ScenesService::getInstance().chubiaoche_type == 10)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(81,58);
				if(ScenesService::getInstance().chubiaoche_type == 11)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(87,72);
				if(ScenesService::getInstance().chubiaoche_type == 12)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					ScenesService::getInstance().chu_biaoche->warp(pos);
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(89,91);
				if(ScenesService::getInstance().chubiaoche_type == 13)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				pos = zPos(128,114);
				if(ScenesService::getInstance().chubiaoche_type == 14)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				pos = zPos(161,108);
				if(ScenesService::getInstance().chubiaoche_type == 15)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(172,89);
				if(ScenesService::getInstance().chubiaoche_type == 16)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				pos = zPos(218,52);
				if(ScenesService::getInstance().chubiaoche_type == 17)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(260,106);
				if(ScenesService::getInstance().chubiaoche_type == 18)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(278,93);
				if(ScenesService::getInstance().chubiaoche_type == 19)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(295,109);
				if(ScenesService::getInstance().chubiaoche_type == 20)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(311,157);
				if(ScenesService::getInstance().chubiaoche_type == 21)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(310,183);
				if(ScenesService::getInstance().chubiaoche_type == 22)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(332,180);
				if(ScenesService::getInstance().chubiaoche_type == 23)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				
				pos = zPos(337,151);
				if(ScenesService::getInstance().chubiaoche_type == 24)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(356,142);
				if(ScenesService::getInstance().chubiaoche_type == 25)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(370,116);
				if(ScenesService::getInstance().chubiaoche_type == 26)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(381,116);
				if(ScenesService::getInstance().chubiaoche_type == 27)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				if(ScenesService::getInstance().chubiaoche_type == 28)
				{
					Scene * s = SceneManager::getInstance().getSceneByName("楚国·边境");
					if (!s)
					{
						Zebra::logger->info("doChangeMapAI:npc跳转地图失败,未找到地图楚国·边境");
						return;
					}
					if(ScenesService::getInstance().chu_biaoche->changeMap(s, zPos(31,277)))
					{
						ScenesService::getInstance().chubiaoche_type = 29;
						for (SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); iter != SceneManager::getInstance().country_info.end();
						 iter++)
						{
							Cmd::Session::t_countryNotify_SceneSession send;
							bzero(send.info, sizeof(send.info));
							sprintf(send.info, "楚国的国家镖车出现在边境");
							send.dwCountryID = iter->second.id;
							sessionClient->sendCmd(&send, sizeof(send));
						}
						Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"楚国的国家镖车出现在边境");
					}
				
				}
				Scene * scene3 = SceneManager::getInstance().getSceneByName("楚国·边境");
				pos = zPos(44,260);
				if(ScenesService::getInstance().chubiaoche_type == 29)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(64,259);
				if(ScenesService::getInstance().chubiaoche_type == 30)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(72,242);
				if(ScenesService::getInstance().chubiaoche_type == 31)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(77,224);
				if(ScenesService::getInstance().chubiaoche_type == 32)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(104,197);
				if(ScenesService::getInstance().chubiaoche_type == 33)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(130,183);
				if(ScenesService::getInstance().chubiaoche_type == 34)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(167,185);
				if(ScenesService::getInstance().chubiaoche_type == 35)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(177,168);
				if(ScenesService::getInstance().chubiaoche_type == 36)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(197,157);
				if(ScenesService::getInstance().chubiaoche_type == 37)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				pos = zPos(219,176);
				if(ScenesService::getInstance().chubiaoche_type == 38)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				pos = zPos(232,212);
				if(ScenesService::getInstance().chubiaoche_type == 39)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				pos = zPos(245,195);
				if(ScenesService::getInstance().chubiaoche_type == 40)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(259,196);
				if(ScenesService::getInstance().chubiaoche_type == 41)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(261,216);
				if(ScenesService::getInstance().chubiaoche_type == 42)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(288,241);
				if(ScenesService::getInstance().chubiaoche_type == 43)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(311,237);
				if(ScenesService::getInstance().chubiaoche_type == 44)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(325,259);
				if(ScenesService::getInstance().chubiaoche_type == 45)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(349,254);
				if(ScenesService::getInstance().chubiaoche_type == 46)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(364,244);
				if(ScenesService::getInstance().chubiaoche_type == 47)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(361,222);
				if(ScenesService::getInstance().chubiaoche_type == 48)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(362,194);
				if(ScenesService::getInstance().chubiaoche_type == 49)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(363,179);
				if(ScenesService::getInstance().chubiaoche_type == 50)
				{
					if(!ScenesService::getInstance().chu_biaoche->gotoFindPath(ScenesService::getInstance().chu_biaoche->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche->getPos(), pos, 2)))
					{
						for (SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); iter != SceneManager::getInstance().country_info.end();
						 iter++)
						{
							Cmd::Session::t_countryNotify_SceneSession send;
							bzero(send.info, sizeof(send.info));
							sprintf(send.info, "楚国的国家镖车已安全送达到边境梁镖头处");
							send.dwCountryID = iter->second.id;
							sessionClient->sendCmd(&send, sizeof(send));
						}
						Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"楚国的国家镖车已安全送达到边境梁镖头处");
						ScenesService::getInstance().chu_biaoche->setClearState();
						ScenesService::getInstance().chu_biaoche = NULL;
						ScenesService::getInstance().chubiaoche_type = 0;
						//国家镖车奖励发放
						SceneUserManager::getMe().GuoJiaBiaoCheJiangLi3(scene3);
					}
				}
			}
			
		}
		else if(ScenesService::getInstance().chu_biaoche2!=NULL)
		{
			ScenesService::getInstance().chu_biaoche2->skillStatusM.clearActiveSkillStatusOnlyUseToStatus48();
			if (ScenesService::getInstance().chu_biaoche2->checkMoveTime(SceneTimeTick::currentTime) && ScenesService::getInstance().chu_biaoche2->canMove())
			{
				zPos pos;
				pos = zPos(865, 572);
				Scene * scene = SceneManager::getInstance().getSceneByName("楚国·王城");
				if(ScenesService::getInstance().chubiaoche_type == 0)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(723, 713);
				if(ScenesService::getInstance().chubiaoche_type == 1)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(785, 808);
				if(ScenesService::getInstance().chubiaoche_type == 2)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				pos = zPos(833, 876);
				if(ScenesService::getInstance().chubiaoche_type == 3)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				pos = zPos(913, 858);
				if(ScenesService::getInstance().chubiaoche_type == 4)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				pos = zPos(939, 939);
				if(ScenesService::getInstance().chubiaoche_type == 5)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				if(ScenesService::getInstance().chubiaoche_type == 6)
				{
					Scene * s = SceneManager::getInstance().getSceneByName("楚国·南郊");
					if (!s)
					{
						Zebra::logger->info("doChangeMapAI:npc跳转地图失败,未找到地图楚国·东郊");
						return;
					}
					if(ScenesService::getInstance().chu_biaoche2->changeMap(s, zPos(26, 24)))
					{
						ScenesService::getInstance().chubiaoche_type++;
						for (SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); iter != SceneManager::getInstance().country_info.end();
						 iter++)
						{
							Cmd::Session::t_countryNotify_SceneSession send;
							bzero(send.info, sizeof(send.info));
							sprintf(send.info, "楚国的国家镖车出现在南郊");
							send.dwCountryID = iter->second.id;
							sessionClient->sendCmd(&send, sizeof(send));
						}
						Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"楚国的国家镖车出现在南郊");
					}
				}
				Scene * scene2 = SceneManager::getInstance().getSceneByName("楚国·南郊");

				pos = zPos(55,49);
				if(ScenesService::getInstance().chubiaoche_type == 7)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(76,58);
				if(ScenesService::getInstance().chubiaoche_type == 8)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(81,46);
				if(ScenesService::getInstance().chubiaoche_type == 9)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(87,52);
				if(ScenesService::getInstance().chubiaoche_type == 10)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(81,58);
				if(ScenesService::getInstance().chubiaoche_type == 11)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(87,72);
				if(ScenesService::getInstance().chubiaoche_type == 12)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					ScenesService::getInstance().chu_biaoche2->warp(pos);
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(89,91);
				if(ScenesService::getInstance().chubiaoche_type == 13)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				pos = zPos(128,114);
				if(ScenesService::getInstance().chubiaoche_type == 14)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				pos = zPos(161,108);
				if(ScenesService::getInstance().chubiaoche_type == 15)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(172,89);
				if(ScenesService::getInstance().chubiaoche_type == 16)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				pos = zPos(218,52);
				if(ScenesService::getInstance().chubiaoche_type == 17)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(260,106);
				if(ScenesService::getInstance().chubiaoche_type == 18)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(278,93);
				if(ScenesService::getInstance().chubiaoche_type == 19)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(295,109);
				if(ScenesService::getInstance().chubiaoche_type == 20)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(311,157);
				if(ScenesService::getInstance().chubiaoche_type == 21)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(310,183);
				if(ScenesService::getInstance().chubiaoche_type == 22)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(332,180);
				if(ScenesService::getInstance().chubiaoche_type == 23)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				
				pos = zPos(337,151);
				if(ScenesService::getInstance().chubiaoche_type == 24)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(356,142);
				if(ScenesService::getInstance().chubiaoche_type == 25)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(370,116);
				if(ScenesService::getInstance().chubiaoche_type == 26)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(381,116);
				if(ScenesService::getInstance().chubiaoche_type == 27)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene2->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				if(ScenesService::getInstance().chubiaoche_type == 28)
				{
					Scene * s = SceneManager::getInstance().getSceneByName("楚国·边境");
					if (!s)
					{
						Zebra::logger->info("doChangeMapAI:npc跳转地图失败,未找到地图楚国·边境");
						return;
					}
					if(ScenesService::getInstance().chu_biaoche2->changeMap(s, zPos(31,277)))
					{
						ScenesService::getInstance().chubiaoche_type = 29;
						for (SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); iter != SceneManager::getInstance().country_info.end();
						 iter++)
						{
							Cmd::Session::t_countryNotify_SceneSession send;
							bzero(send.info, sizeof(send.info));
							sprintf(send.info, "楚国的国家镖车出现在边境");
							send.dwCountryID = iter->second.id;
							sessionClient->sendCmd(&send, sizeof(send));
						}
						Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"楚国的国家镖车出现在边境");
					}
				
				}
				Scene * scene3 = SceneManager::getInstance().getSceneByName("楚国·边境");
				pos = zPos(44,260);
				if(ScenesService::getInstance().chubiaoche_type == 29)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(64,259);
				if(ScenesService::getInstance().chubiaoche_type == 30)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(72,242);
				if(ScenesService::getInstance().chubiaoche_type == 31)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(77,224);
				if(ScenesService::getInstance().chubiaoche_type == 32)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(104,197);
				if(ScenesService::getInstance().chubiaoche_type == 33)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(130,183);
				if(ScenesService::getInstance().chubiaoche_type == 34)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(167,185);
				if(ScenesService::getInstance().chubiaoche_type == 35)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(177,168);
				if(ScenesService::getInstance().chubiaoche_type == 36)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(197,157);
				if(ScenesService::getInstance().chubiaoche_type == 37)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				pos = zPos(219,176);
				if(ScenesService::getInstance().chubiaoche_type == 38)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				pos = zPos(232,212);
				if(ScenesService::getInstance().chubiaoche_type == 39)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}

				pos = zPos(245,195);
				if(ScenesService::getInstance().chubiaoche_type == 40)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(259,196);
				if(ScenesService::getInstance().chubiaoche_type == 41)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(261,216);
				if(ScenesService::getInstance().chubiaoche_type == 42)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(288,241);
				if(ScenesService::getInstance().chubiaoche_type == 43)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(311,237);
				if(ScenesService::getInstance().chubiaoche_type == 44)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(325,259);
				if(ScenesService::getInstance().chubiaoche_type == 45)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(349,254);
				if(ScenesService::getInstance().chubiaoche_type == 46)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(364,244);
				if(ScenesService::getInstance().chubiaoche_type == 47)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(361,222);
				if(ScenesService::getInstance().chubiaoche_type == 48)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(362,194);
				if(ScenesService::getInstance().chubiaoche_type == 49)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						ScenesService::getInstance().chubiaoche_type++;;
					}
				}
				pos = zPos(363,179);
				if(ScenesService::getInstance().chubiaoche_type == 50)
				{
					if(!ScenesService::getInstance().chu_biaoche2->gotoFindPath(ScenesService::getInstance().chu_biaoche2->getPos(),pos))
					{
						ScenesService::getInstance().chu_biaoche2->goTo(pos);
					}
					
					if ((scene3->zPosShortRange(ScenesService::getInstance().chu_biaoche2->getPos(), pos, 2)))
					{
						for (SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); iter != SceneManager::getInstance().country_info.end();
						 iter++)
						{
							Cmd::Session::t_countryNotify_SceneSession send;
							bzero(send.info, sizeof(send.info));
							sprintf(send.info, "楚国的国家镖车(已被劫掠)已安全送达到边境梁镖头处");
							send.dwCountryID = iter->second.id;
							sessionClient->sendCmd(&send, sizeof(send));
						}
						Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"楚国的国家镖车(已被劫掠)已安全送达到边境梁镖头处");
						ScenesService::getInstance().chu_biaoche2->setClearState();
						ScenesService::getInstance().chu_biaoche2 = NULL;
						ScenesService::getInstance().chubiaoche_type = 0;
						//国家镖车奖励发放
						SceneUserManager::getMe().GuoJiaBiaoCheJiangLi4(scene3);
					}
				}
			}
		}

		//250 usec
		EverySceneEntryAction esea(step);
		//对所有地图调用回调函数
		SceneManager::getInstance().execEveryScene(esea);
		if (atoi(Zebra::global["daoban"].c_str()) != 0)
		{	
			Channel::sendAllInfo(Cmd::INFO_TYPE_YANSE1,"太特么惨了，居然是个盗版服，未经授权，随时关服，你们被坑了！！！！太惨了");
			Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEJROLL,"玩到盗版服这可咋整，老G也不愿意花大钱，就知道割你们韭菜！！");
			Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5, "此服为盗版服，未经授权，随时关服，你们被坑了！！！！太惨了");
			Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5, "此服为盗版服，未经授权，随时关服，你们被坑了！！！！太惨了");
			Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5, "此服为盗版服，未经授权，随时关服，你们被坑了！！！！太惨了");
			Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5, "此服为盗版服，未经授权，随时关服，你们被坑了！！！！太惨了");
			Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5, "此服为盗版服，未经授权，随时关服，你们被坑了！！！！太惨了");
			Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5, "此服为盗版服，未经授权，随时关服，你们被坑了！！！！太惨了");
			Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5, "此服为盗版服，未经授权，随时关服，你们被坑了！！！！太惨了");
			Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5, "此服为盗版服，未经授权，随时关服，你们被坑了！！！！太惨了");
			Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5, "此服为盗版服，未经授权，随时关服，你们被坑了！！！！太惨了");
			for (SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); iter != SceneManager::getInstance().country_info.end();
				 iter++)
			{
				Cmd::Session::t_countryNotify_SceneSession send;
				bzero(send.info, sizeof(send.info));
				sprintf(send.info, "此服为盗版服，未经授权，随时关服，你们被坑了！！！！太惨了");
				send.dwCountryID = iter->second.id;\
				send.infoType = Cmd::INFO_TYPE_FAIL;
				sessionClient->sendCmd(&send, sizeof(send));
			}
		}

#if 0
		if (0==step)
		{
			//20-25 usec
			sessionClient->doCmd();
			recordClient->doCmd();
			SceneTaskManager::getInstance().execEvery();
		}
#endif

		if (_one_min(currentTime))
		{//竞赛处理,一分钟判断一次全国竞赛
		
			CountryDareM::getMe().timer();
			//醉梦修复动态地图
			SceneManager::getInstance().timer();
			struct tm tv1;
			time_t timValue = time(NULL);
			zRTime::getLocalTime(tv1, timValue);

			if (tv1.tm_hour == 19) //世界BOSS提示
			{
				if (tv1.tm_min == 55)
				{
					for (SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); iter != SceneManager::getInstance().country_info.end();
						 iter++)
					{
						Cmd::Session::t_countryNotify_SceneSession send;
						bzero(send.info, sizeof(send.info));
						sprintf(send.info, "%d 分钟后将开启天降鸿福活动，在线玩家可获得奖励！", abs(60 - tv1.tm_min));
						send.dwCountryID = iter->second.id;
						sessionClient->sendCmd(&send, sizeof(send));
					}
					Channel::sendAllInfo(Cmd::INFO_TYPE_YANSE1,"%d 分钟后将开启天降鸿福活动，在线玩家可获得奖励！", abs(60 - tv1.tm_min));
				}
			}

			if (tv1.tm_hour == 22) // 世界BOSS提示
			{
				if (tv1.tm_min >=1  && tv1.tm_min <=9 )
				{
					for (SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); iter != SceneManager::getInstance().country_info.end();
						 iter++)
					{
						Cmd::Session::t_countryNotify_SceneSession send;
						bzero(send.info, sizeof(send.info));
						sprintf(send.info, "%d 分钟后世界BOSS即将出现在魔族巢穴1层，届时各位玩家可前往击杀！", abs(10 - tv1.tm_min));
						send.dwCountryID = iter->second.id;
						sessionClient->sendCmd(&send, sizeof(send));
					}
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"%d 分钟后世界BOSS即将出现在魔族巢穴1层，届时各位玩家可前往击杀！", abs(10 - tv1.tm_min));
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"%d 分钟后世界BOSS即将出现在魔族巢穴1层，届时各位玩家可前往击杀！", abs(10 - tv1.tm_min));
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"%d 分钟后世界BOSS即将出现在魔族巢穴1层，届时各位玩家可前往击杀！", abs(10 - tv1.tm_min));
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"%d 分钟后世界BOSS即将出现在魔族巢穴1层，届时各位玩家可前往击杀！", abs(10 - tv1.tm_min));
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"%d 分钟后世界BOSS即将出现在魔族巢穴1层，届时各位玩家可前往击杀！", abs(10 - tv1.tm_min));
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP5,"%d 分钟后世界BOSS即将出现在魔族巢穴1层，届时各位玩家可前往击杀！", abs(10 - tv1.tm_min));
				}
			
			}

			if (tv1.tm_hour==0 && (tv1.tm_min==1 || tv1.tm_min==2 || tv1.tm_min==3)) //国家BOSS 自行车 天下第一  迷宫 初始化
			{
				char a[16];
				Zebra::global["BOSS_CHU"]=a;
				Zebra::global["BOSS_HAN"]=a;
				char b[16]; 
				sprintf(b,"0");
				Zebra::global["BIKENUM1"]=b;
				Zebra::global["BIKENUM2"]=b;
				Zebra::global["BIKENUM3"]=b;
				Zebra::global["ZXCWC"]=b;
				Zebra::global["MIGONGNUM1"]=b;
				Zebra::global["MIGONGNUM2"]=b;
				Zebra::global["MIGONGNUM3"]=b;
				Zebra::global["MIGONGWC"]=b;
				//初始化天下第一
				for (int i = 0; i < 32; i++)
				{
					ScenesService::getInstance().tianxia[i].jifen=0;
					ScenesService::getInstance().tianxia[i].saidian=-1;
					ScenesService::getInstance().tianxia[i].userid = 0;
				}
				Zebra::logger->info("初始化天下第一系统成功");
				//初始化迷宫系统
				for (int i = 0; i < 25; i++)
				{
					ScenesService::getInstance().migong[i].ceng=i+1;
					ScenesService::getInstance().migong[i].password=zMisc::randBetween(1,3);
				}
				
				Zebra::logger->info("初始化迷宫系统成功");
			}

			//汉国国家镖车时间===========================================
			int hanguo_time1 = 20;
			int hanguo_time2 = 50;
			//楚国国家镖车时间===========================================
			int chuguo_time1 = 21;
			int chuguo_time2 = 20;

			//国家镖车 汉国时间点
			if(tv1.tm_hour == hanguo_time1)
			{
				if (tv1.tm_min ==  hanguo_time2)//国家镖车开始出车
				{
					//汉国先来
					if(!ScenesService::getInstance().han_biaoche)
					{
						char mapName[MAX_NAMESIZE];
						bzero(mapName, MAX_NAMESIZE);
						snprintf(mapName, MAX_NAMESIZE, "汉国·王城");
						Scene * scene = SceneManager::getInstance().getSceneByName(mapName);
						if (!scene)
						{
							Zebra::logger->debug("PARA_SUMMON_ALLY_NPC: 召唤npc时找不到地图 mapName=%s", mapName);
							return;
						}
						zNpcB *base = npcbm.get(54100);
						if (NULL == base)
						{
							Zebra::logger->debug("PARA_SUMMON_ALLY_NPC: 召唤npc时找不到NPC id=%d", 54100);
							return;
						}

						t_NpcDefine define;
						define.id = base->id;
						strcpy(define.name, base->name);
						define.pos = zPos(830, 532);
						define.num = 1;
						define.interval = 0; 
						define.initstate = zSceneEntry::SceneEntry_Normal;
						define.width = 2;
						define.height = 2;
						define.scriptID = 0;
						scene->initRegion(define.region, define.pos, define.width, define.height);
						ScenesService::getInstance().han_biaoche = scene->summonOneNpc<SceneNpc>(define, define.pos, base);

						for (SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); iter != SceneManager::getInstance().country_info.end();
						 iter++)
						{
							Cmd::Session::t_countryNotify_SceneSession send;
							bzero(send.info, sizeof(send.info));
							sprintf(send.info, "汉国的国家镖车已从王城郑镖头处出发，前往边境！");
							send.dwCountryID = iter->second.id;
							sessionClient->sendCmd(&send, sizeof(send));
						}
						Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"汉国的国家镖车已从王城郑镖头处出发，前往边境！");
						ScenesService::getInstance().hanbiaoche_type = 0;
					}					
				}
			}
			
			//国家镖车 楚国时间点
			if(tv1.tm_hour == chuguo_time1)
			{
				if (tv1.tm_min ==  chuguo_time2)//国家镖车开始出车
				{
					//楚国先来
					if(!ScenesService::getInstance().chu_biaoche)
					{
						char mapName[MAX_NAMESIZE];
						bzero(mapName, MAX_NAMESIZE);
						snprintf(mapName, MAX_NAMESIZE, "楚国·王城");
						Scene * scene = SceneManager::getInstance().getSceneByName(mapName);
						if (!scene)
						{
							Zebra::logger->debug("PARA_SUMMON_ALLY_NPC: 召唤npc时找不到地图 mapName=%s", mapName);
							return;
						}
						zNpcB *base = npcbm.get(54098);
						if (NULL == base)
						{
							Zebra::logger->debug("PARA_SUMMON_ALLY_NPC: 召唤npc时找不到NPC id=%d", 54098);
							return;
						}

						t_NpcDefine define;
						define.id = base->id;
						strcpy(define.name, base->name);
						define.pos = zPos(830, 532);
						define.num = 1;
						define.interval = 0; 
						define.initstate = zSceneEntry::SceneEntry_Normal;
						define.width = 2;
						define.height = 2;
						define.scriptID = 0;
						scene->initRegion(define.region, define.pos, define.width, define.height);
						ScenesService::getInstance().chu_biaoche = scene->summonOneNpc<SceneNpc>(define, define.pos, base);

						for (SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); iter != SceneManager::getInstance().country_info.end();
						 iter++)
						{
							Cmd::Session::t_countryNotify_SceneSession send;
							bzero(send.info, sizeof(send.info));
							sprintf(send.info, "楚国的国家镖车已从王城郑镖头处出发，前往边境！");
							send.dwCountryID = iter->second.id;
							sessionClient->sendCmd(&send, sizeof(send));
						}
						Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"楚国的国家镖车已从王城郑镖头处出发，前往边境！");
						ScenesService::getInstance().chubiaoche_type = 0;
					}					
				}
			}

			if (tv1.tm_hour == 22) // 天下第一大赛
			{
				if (tv1.tm_min == 05 )
				{
					for (SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); iter != SceneManager::getInstance().country_info.end();
						 iter++)
					{
						Cmd::Session::t_countryNotify_SceneSession send;
						bzero(send.info, sizeof(send.info));
						sprintf(send.info, "%d 分钟后将举办天下第一大赛，请各位参赛选手做好准备！", abs(10 - tv1.tm_min));
						send.dwCountryID = iter->second.id;
						sessionClient->sendCmd(&send, sizeof(send));
					}
					Channel::sendAllInfo(Cmd::INFO_TYPE_YANSE1,"%d 分钟后将举办天下第一大赛，请各位参赛选手做好准备！", abs(10 - tv1.tm_min));
				}
			}
			if (tv1.tm_hour == 22) // 天下第一大赛海选开始！
			{
				if (tv1.tm_min == 10)
				{
					for(int i=0;i<32;i++)
					{
						ScenesService::getInstance().tianxia[i].jifen =0;
					}
					
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一海选赛现在开始，即将传送各位参赛选手，让我们拭目以待！");	
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一海选赛现在开始，即将传送各位参赛选手，让我们拭目以待！");		
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一海选赛现在开始，即将传送各位参赛选手，让我们拭目以待！");		
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一海选赛现在开始，即将传送各位参赛选手，让我们拭目以待！");		
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一海选赛现在开始，即将传送各位参赛选手，让我们拭目以待！");													

					for (int i = 0; i < 32; i++)
					{
						if (ScenesService::getInstance().tianxia[i].userid == 0) // 遇到0代表下面没人了
						{
							int num = i;  // b报名总人数
							if (num % 2 != 0) // 人数为单数
							{
								// 那么直接让最后一名晋级
								int lastCharId = ScenesService::getInstance().tianxia[i-1].userid;
								SceneUser *pUser = SceneUserManager::getMe().getUserByID(lastCharId); // 最后一名玩家
								if (pUser)															  // 如果他在线
								{
									// 执行给他设置赛点为1代表晋级一次 十六强
									ScenesService::getInstance().tianxia[i-1].saidian = 1;
								}
								else
								{
									// 如果不在线，那么直接把他的赛点设置为0 淘汰
									ScenesService::getInstance().tianxia[i-1].saidian = 0;
								}
							}
						}
						else
						{
							int num = i + 1;  // 当前人数
							if (num % 2 == 0) // 人数为双数 i和 i- 1 传入
							{
								int AChar = ScenesService::getInstance().tianxia[i].userid;
								int BChar = ScenesService::getInstance().tianxia[i - 1].userid;
								SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
								SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
								if (pUserA && pUserB)											  // 如果两个玩家都在线
								{
									// 执行传送，开始比赛
									int zu = num / 2; // 总组数
									switch (zu)
									{
									case 1:
									{
									
										Gm::gomap(pUserA, "name=中立区·天下第一海选1组 pos=48,56");  //自动传送地图坐标
										Gm::gomap(pUserB, "name=中立区·天下第一海选1组 pos=69,55");  //自动传送地图坐标
									}
									break;
									case 2:
									{
										Gm::gomap(pUserA, "name=中立区·天下第一海选2组 pos=48,56");  //自动传送地图坐标
										Gm::gomap(pUserB, "name=中立区·天下第一海选2组 pos=69,55");  //自动传送地图坐标
									}
									break;
									case 3:
									{
										Gm::gomap(pUserA, "name=中立区·天下第一海选3组 pos=48,56");  //自动传送地图坐标
										Gm::gomap(pUserB, "name=中立区·天下第一海选3组 pos=69,55");  //自动传送地图坐标
									}
									break;
									case 4:
									{
										Gm::gomap(pUserA, "name=中立区·天下第一海选4组 pos=48,56");  //自动传送地图坐标
										Gm::gomap(pUserB, "name=中立区·天下第一海选4组 pos=69,55");  //自动传送地图坐标
									}
									break;
									case 5:
									{
										Gm::gomap(pUserA, "name=中立区·天下第一海选5组 pos=48,56");  //自动传送地图坐标
										Gm::gomap(pUserB, "name=中立区·天下第一海选5组 pos=69,55");  //自动传送地图坐标
									}
									break;
									case 6:
									{
										Gm::gomap(pUserA, "name=中立区·天下第一海选6组 pos=48,56");  //自动传送地图坐标
										Gm::gomap(pUserB, "name=中立区·天下第一海选6组 pos=69,55");  //自动传送地图坐标
									}
									break;
									case 7:
									{
										Gm::gomap(pUserA, "name=中立区·天下第一海选7组 pos=48,56");  //自动传送地图坐标
										Gm::gomap(pUserB, "name=中立区·天下第一海选7组 pos=69,55");  //自动传送地图坐标
									}
									break;
									case 8:
									{
										Gm::gomap(pUserA, "name=中立区·天下第一海选8组 pos=48,56");  //自动传送地图坐标
										Gm::gomap(pUserB, "name=中立区·天下第一海选8组 pos=69,55");  //自动传送地图坐标
									}
									break;
									case 9:
									{
										Gm::gomap(pUserA, "name=中立区·天下第一海选9组 pos=48,56");  //自动传送地图坐标
										Gm::gomap(pUserB, "name=中立区·天下第一海选9组 pos=69,55");  //自动传送地图坐标
									}
									break;
									case 10:
									{
										Gm::gomap(pUserA, "name=中立区·天下第一海选10组 pos=48,56");  //自动传送地图坐标
										Gm::gomap(pUserB, "name=中立区·天下第一海选10组 pos=69,55");  //自动传送地图坐标
									}
									break;
									case 11:
									{
										Gm::gomap(pUserA, "name=中立区·天下第一海选11组 pos=48,56");  //自动传送地图坐标
										Gm::gomap(pUserB, "name=中立区·天下第一海选11组 pos=69,55");  //自动传送地图坐标
									}
									break;
									case 12:
									{
										Gm::gomap(pUserA, "name=中立区·天下第一海选12组 pos=48,56");  //自动传送地图坐标
										Gm::gomap(pUserB, "name=中立区·天下第一海选12组 pos=69,55");  //自动传送地图坐标
									}
									break;
									case 13:
									{
										Gm::gomap(pUserA, "name=中立区·天下第一海选13组 pos=48,56");  //自动传送地图坐标
										Gm::gomap(pUserB, "name=中立区·天下第一海选13组 pos=69,55");  //自动传送地图坐标
									}
									break;
									case 14:
									{
										Gm::gomap(pUserA, "name=中立区·天下第一海选14组 pos=48,56");  //自动传送地图坐标
										Gm::gomap(pUserB, "name=中立区·天下第一海选14组 pos=69,55");  //自动传送地图坐标
									}
									break;
									case 15:
									{
										Gm::gomap(pUserA, "name=中立区·天下第一海选16组 pos=48,56");  //自动传送地图坐标
										Gm::gomap(pUserB, "name=中立区·天下第一海选16组 pos=69,55");  //自动传送地图坐标
									}
									break;
									case 16:
									{
										Gm::gomap(pUserA, "name=中立区·天下第一海选17组 pos=48,56");  //自动传送地图坐标
										Gm::gomap(pUserB, "name=中立区·天下第一海选17组 pos=69,55");  //自动传送地图坐标
									}
									break;
									}
									//设置玩家全体状态
									pUserA->pkMode = 1;
									pUserB->pkMode = 1;
									Channel::sendSys(pUserA , Cmd::INFO_TYPE_STATE, "PK切换模式为:全体模式.");
									Channel::sendSys(pUserB , Cmd::INFO_TYPE_STATE, "PK切换模式为:全体模式.");
									
									
								}
								else
								{
									if(!pUserA && !pUserB){
										ScenesService::getInstance().tianxia[i].saidian = 0;
										ScenesService::getInstance().tianxia[i-1].saidian = 0;			
									}
									else if (!pUserA) // 玩家A不在线
									{
										// 如果不在线，那么直接把他的赛点设置为0 淘汰 并设置玩家B赛点为1 晋级16强
										ScenesService::getInstance().tianxia[i].saidian = 0;
										ScenesService::getInstance().tianxia[i-1].saidian = 1;	
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");	
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！",SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country),pUserB->charbase.name );																					
									}
									else if(!pUserB)
									{ // 玩家B不在线
										// 如果不在线，那么直接把他的赛点设置为0 淘汰 并设置玩家A赛点为1 晋级16强
										ScenesService::getInstance().tianxia[i].saidian = 1;
										ScenesService::getInstance().tianxia[i-1].saidian = 0;			
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！",SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country),pUserA->charbase.name );												
									}
									
								}
							}
						}
					}
				}
			}
			if (tv1.tm_hour == 22) // 天下第一大赛海选结算！
			{
				if (tv1.tm_min == 14)
				{
					for (int i = 0; i < 32; i++)
					{
						if (ScenesService::getInstance().tianxia[i].userid != 0) 
						{
							int num = i + 1;  // 当前人数
							if (num % 2 == 0) // 人数为双数 i和 i- 1 传入
							{
								if(ScenesService::getInstance().tianxia[i].saidian !=1 && ScenesService::getInstance().tianxia[i-1].saidian!=1)
								{
									int AChar = ScenesService::getInstance().tianxia[i].userid;
									int BChar = ScenesService::getInstance().tianxia[i - 1].userid;
									SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
									SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B

									if(!pUserA && !pUserB){
										ScenesService::getInstance().tianxia[i].saidian = 0;
										ScenesService::getInstance().tianxia[i-1].saidian = 0;			
									}
									else if (!pUserA) // 玩家A不在线
									{
										Gm::gomap(pUserB, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
										// 如果不在线，那么直接把他的赛点设置为0 淘汰 并设置玩家B赛点为1 晋级16强
										ScenesService::getInstance().tianxia[i].saidian = 0;
										ScenesService::getInstance().tianxia[i-1].saidian = 1;	
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！",SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country),pUserB->charbase.name );												
									}
									else if(!pUserB)
									{ // 玩家B不在线
										// 如果不在线，那么直接把他的赛点设置为0 淘汰 并设置玩家A赛点为1 晋级16强
										Gm::gomap(pUserA, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
										ScenesService::getInstance().tianxia[i].saidian = 1;
										ScenesService::getInstance().tianxia[i-1].saidian = 0;	
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！",SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country),pUserA->charbase.name );														
									}

									if (pUserA && pUserB)	// 如果两个玩家都在线
									{
										Gm::gomap(pUserA, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
										Gm::gomap(pUserB, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
										if(ScenesService::getInstance().tianxia[i].jifen > ScenesService::getInstance().tianxia[i-1].jifen)
										{
											ScenesService::getInstance().tianxia[i].saidian = 1;
											ScenesService::getInstance().tianxia[i-1].saidian = 0;
											Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！",SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country),pUserA->charbase.name );												
											Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");	
											Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
											pUserB->addBindObjectNum(36005,1,0,0,1);
										}
										else if(ScenesService::getInstance().tianxia[i].jifen < ScenesService::getInstance().tianxia[i-1].jifen)
										{
											ScenesService::getInstance().tianxia[i].saidian = 0;
											ScenesService::getInstance().tianxia[i-1].saidian = 1;	
											
											Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！",SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country),pUserB->charbase.name );												
											Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");	
											Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
											pUserA->addBindObjectNum(36005,1,0,0,1);
										}
										else{
											if(pUserA->charbase.zhanli > pUserB->charbase.zhanli)
											{
												ScenesService::getInstance().tianxia[i].saidian = 1;
												ScenesService::getInstance().tianxia[i-1].saidian = 0;	
												Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！",SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country),pUserA->charbase.name );												
												Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");	
												Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");	
												pUserB->addBindObjectNum(36005,1,0,0,1);
											}
											else
											{
												ScenesService::getInstance().tianxia[i].saidian = 0;
												ScenesService::getInstance().tianxia[i-1].saidian = 1;	
												Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！",SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country),pUserB->charbase.name );												
												Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");	
												Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
												pUserA->addBindObjectNum(36005,1,0,0,1);
											}	
										}
									}		
								}			
							}
						}
					}
				}
			}
			if (tv1.tm_hour == 22) // 天下第一大赛初选开始！
			{
				if (tv1.tm_min == 18)
				{

					for(int i=0;i<32;i++)
					{
						ScenesService::getInstance().tianxia[i].jifen =0;
					}
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一初选赛现在开始，即将传送各位参赛选手，让我们拭目以待！");	
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一初选赛现在开始，即将传送各位参赛选手，让我们拭目以待！");		
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一初选赛现在开始，即将传送各位参赛选手，让我们拭目以待！");		
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一初选赛现在开始，即将传送各位参赛选手，让我们拭目以待！");		
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一初选赛现在开始，即将传送各位参赛选手，让我们拭目以待！");	


					//分组处理
					int a = 0;
					int b = 0;
					int aa=0;
					int bb=0;
					a=0;
					b=1;
					aa=4;
					bb=5;
					//第一组
					if(ScenesService::getInstance().tianxia[a].saidian == 1 || ScenesService::getInstance().tianxia[b].saidian == 1)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//匹配成功
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·天下第一初选1组 pos=48,56");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·天下第一初选1组 pos=69,55");  //自动传送地图坐标
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//A组没有选手,B组有选手
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家A
							if(BChar)
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}
					
					a=2;
					b=3;
					aa=6;
					bb=7;
					//第二组
					if(ScenesService::getInstance().tianxia[a].saidian == 1 || ScenesService::getInstance().tianxia[b].saidian == 1)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//匹配成功
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·天下第一初选2组 pos=48,56");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·天下第一初选2组 pos=69,55");  //自动传送地图坐标
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//A组没有选手,B组有选手
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家A
							if(BChar)
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}

					a=8;
					b=9;
					aa=12;
					bb=13;
					//第三组
					if(ScenesService::getInstance().tianxia[a].saidian == 1 || ScenesService::getInstance().tianxia[b].saidian == 1)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//匹配成功
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·天下第一初选3组 pos=48,56");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·天下第一初选3组 pos=69,55");  //自动传送地图坐标
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//A组没有选手,B组有选手
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家A
							if(BChar)
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}

					a=10;
					b=11;
					aa=14;
					bb=15;
					//第三组
					if(ScenesService::getInstance().tianxia[a].saidian == 1 || ScenesService::getInstance().tianxia[b].saidian == 1)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//匹配成功
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·天下第一初选4组 pos=48,56");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·天下第一初选4组 pos=69,55");  //自动传送地图坐标
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//A组没有选手,B组有选手
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家A
							if(BChar)
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}

						a=16;
						b=17;
						aa=20;
						bb=21;
					//第三组
					if(ScenesService::getInstance().tianxia[a].saidian == 1 || ScenesService::getInstance().tianxia[b].saidian == 1)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//匹配成功
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·天下第一初选5组 pos=48,56");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·天下第一初选5组 pos=69,55");  //自动传送地图坐标
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//A组没有选手,B组有选手
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家A
							if(BChar)
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}

					a=18;
					b=19;
					aa=22;
					bb=23;
					//第三组
					if(ScenesService::getInstance().tianxia[a].saidian == 1 || ScenesService::getInstance().tianxia[b].saidian == 1)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//匹配成功
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·天下第一初选6组 pos=48,56");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·天下第一初选6组 pos=69,55");  //自动传送地图坐标
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//A组没有选手,B组有选手
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家A
							if(BChar)
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}

					a=24;
					b=25;
					aa=28;
					bb=29;
					//第三组
					if(ScenesService::getInstance().tianxia[a].saidian == 1 || ScenesService::getInstance().tianxia[b].saidian == 1)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//匹配成功
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·天下第一初选7组 pos=48,56");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·天下第一初选7组 pos=69,55");  //自动传送地图坐标
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//A组没有选手,B组有选手
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家A
							if(BChar)
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}

					a=26;
					b=27;
					aa=30;
					bb=31;
					//第三组
					if(ScenesService::getInstance().tianxia[a].saidian == 1 || ScenesService::getInstance().tianxia[b].saidian == 1)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//匹配成功
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·天下第一初选8组 pos=48,56");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·天下第一初选8组 pos=69,55");  //自动传送地图坐标
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//A组没有选手,B组有选手
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家A
							if(BChar)
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}

				}
			}

			if (tv1.tm_hour == 22) // 天下第一大赛初选结算！
			{
				if (tv1.tm_min == 22)
				{
					//分组处理
					int a = 0;
					int b = 0;
					int aa=0;
					int bb=0;
					a=0;
					b=1;
					aa=4;
					bb=5;
					//第一组
					if(ScenesService::getInstance().tianxia[a].saidian == 1 || ScenesService::getInstance().tianxia[b].saidian == 1)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//匹配成功
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
							int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								if (ScenesService::getInstance().tianxia[A].jifen > ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[A].saidian = 2;
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserB->addBindObjectNum(36003,1,0,0,1);
								}
								else if (ScenesService::getInstance().tianxia[A].jifen < ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[B].saidian = 2;

									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserA->addBindObjectNum(36003,1,0,0,1);
								}
								else
								{
									if (pUserA->charbase.zhanli > pUserB->charbase.zhanli)
									{
										ScenesService::getInstance().tianxia[A].saidian = 2;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserB->addBindObjectNum(36003,1,0,0,1);
									}
									else
									{
										ScenesService::getInstance().tianxia[B].saidian = 2;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserA->addBindObjectNum(36003,1,0,0,1);
									}
								}
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
								int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
								ScenesService::getInstance().tianxia[A].saidian =2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
								int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
								ScenesService::getInstance().tianxia[B].saidian =2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}	
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//A组没有选手,B组有选手
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家A
							if(BChar)
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}
					

					a=2;
					b=3;
					aa=6;
					bb=7;
					//第二组
					if(ScenesService::getInstance().tianxia[a].saidian == 1 || ScenesService::getInstance().tianxia[b].saidian == 1)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//匹配成功
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
							int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								if (ScenesService::getInstance().tianxia[A].jifen > ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[A].saidian = 2;
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserB->addBindObjectNum(36003,1,0,0,1);
								}
								else if (ScenesService::getInstance().tianxia[A].jifen < ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[B].saidian = 2;

									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserA->addBindObjectNum(36003,1,0,0,1);
								}
								else
								{
									if (pUserA->charbase.zhanli > pUserB->charbase.zhanli)
									{
										ScenesService::getInstance().tianxia[A].saidian = 2;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserB->addBindObjectNum(36003,1,0,0,1);
									}
									else
									{
										ScenesService::getInstance().tianxia[B].saidian = 2;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserA->addBindObjectNum(36003,1,0,0,1);
									}
								}
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
								int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
								ScenesService::getInstance().tianxia[A].saidian =2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
								int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
								ScenesService::getInstance().tianxia[B].saidian =2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}	
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//A组没有选手,B组有选手
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家A
							if(BChar)
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}

					a=8;
					b=9;
					aa=12;
					bb=13;
					//第三组
					if(ScenesService::getInstance().tianxia[a].saidian == 1 || ScenesService::getInstance().tianxia[b].saidian == 1)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//匹配成功
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
							int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								if (ScenesService::getInstance().tianxia[A].jifen > ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[A].saidian = 2;
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserB->addBindObjectNum(36003,1,0,0,1);
								}
								else if (ScenesService::getInstance().tianxia[A].jifen < ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[B].saidian = 2;

									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserA->addBindObjectNum(36003,1,0,0,1);
								}
								else
								{
									if (pUserA->charbase.zhanli > pUserB->charbase.zhanli)
									{
										ScenesService::getInstance().tianxia[A].saidian = 2;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserB->addBindObjectNum(36003,1,0,0,1);
									}
									else
									{
										ScenesService::getInstance().tianxia[B].saidian = 2;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserA->addBindObjectNum(36003,1,0,0,1);
									}
								}
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
								int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
								ScenesService::getInstance().tianxia[A].saidian =2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
								int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
								ScenesService::getInstance().tianxia[B].saidian =2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}	
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//A组没有选手,B组有选手
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家A
							if(BChar)
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}
					

					a=10;
					b=11;
					aa=14;
					bb=15;
					//第四组
					if(ScenesService::getInstance().tianxia[a].saidian == 1 || ScenesService::getInstance().tianxia[b].saidian == 1)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//匹配成功
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
							int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								if (ScenesService::getInstance().tianxia[A].jifen > ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[A].saidian = 2;
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserB->addBindObjectNum(36003,1,0,0,1);
								}
								else if (ScenesService::getInstance().tianxia[A].jifen < ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[B].saidian = 2;

									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserA->addBindObjectNum(36003,1,0,0,1);
								}
								else
								{
									if (pUserA->charbase.zhanli > pUserB->charbase.zhanli)
									{
										ScenesService::getInstance().tianxia[A].saidian = 2;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserB->addBindObjectNum(36003,1,0,0,1);
									}
									else
									{
										ScenesService::getInstance().tianxia[B].saidian = 2;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserA->addBindObjectNum(36003,1,0,0,1);
									}
								}
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
								int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
								ScenesService::getInstance().tianxia[A].saidian =2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
								int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
								ScenesService::getInstance().tianxia[B].saidian =2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}	
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//A组没有选手,B组有选手
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家A
							if(BChar)
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}



					a=16;
					b=17;
					aa=20;
					bb=21;

					//第五组
					if(ScenesService::getInstance().tianxia[a].saidian == 1 || ScenesService::getInstance().tianxia[b].saidian == 1)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//匹配成功
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
							int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								if (ScenesService::getInstance().tianxia[A].jifen > ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[A].saidian = 2;
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserB->addBindObjectNum(36003,1,0,0,1);
								}
								else if (ScenesService::getInstance().tianxia[A].jifen < ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[B].saidian = 2;

									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserA->addBindObjectNum(36003,1,0,0,1);
								}
								else
								{
									if (pUserA->charbase.zhanli > pUserB->charbase.zhanli)
									{
										ScenesService::getInstance().tianxia[A].saidian = 2;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserB->addBindObjectNum(36003,1,0,0,1);
									}
									else
									{
										ScenesService::getInstance().tianxia[B].saidian = 2;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserA->addBindObjectNum(36003,1,0,0,1);
									}
								}
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
								int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
								ScenesService::getInstance().tianxia[A].saidian =2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
								int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
								ScenesService::getInstance().tianxia[B].saidian =2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}	
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//A组没有选手,B组有选手
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家A
							if(BChar)
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}



					a=18;
					b=19;
					aa=22;
					bb=23;

					//第六组
					if(ScenesService::getInstance().tianxia[a].saidian == 1 || ScenesService::getInstance().tianxia[b].saidian == 1)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//匹配成功
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
							int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								if (ScenesService::getInstance().tianxia[A].jifen > ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[A].saidian = 2;
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserB->addBindObjectNum(36003,1,0,0,1);
								}
								else if (ScenesService::getInstance().tianxia[A].jifen < ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[B].saidian = 2;

									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserA->addBindObjectNum(36003,1,0,0,1);
								}
								else
								{
									if (pUserA->charbase.zhanli > pUserB->charbase.zhanli)
									{
										ScenesService::getInstance().tianxia[A].saidian = 2;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserB->addBindObjectNum(36003,1,0,0,1);
									}
									else
									{
										ScenesService::getInstance().tianxia[B].saidian = 2;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserA->addBindObjectNum(36003,1,0,0,1);
									}
								}
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
								int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
								ScenesService::getInstance().tianxia[A].saidian =2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
								int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
								ScenesService::getInstance().tianxia[B].saidian =2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}	
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//A组没有选手,B组有选手
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家A
							if(BChar)
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}

					a=24;
					b=25;
					aa=28;
					bb=29;

					//第七组
					if(ScenesService::getInstance().tianxia[a].saidian == 1 || ScenesService::getInstance().tianxia[b].saidian == 1)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//匹配成功
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
							int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								if (ScenesService::getInstance().tianxia[A].jifen > ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[A].saidian = 2;
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserB->addBindObjectNum(36003,1,0,0,1);
								}
								else if (ScenesService::getInstance().tianxia[A].jifen < ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[B].saidian = 2;

									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserA->addBindObjectNum(36003,1,0,0,1);
								}
								else
								{
									if (pUserA->charbase.zhanli > pUserB->charbase.zhanli)
									{
										ScenesService::getInstance().tianxia[A].saidian = 2;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserB->addBindObjectNum(36003,1,0,0,1);
									}
									else
									{
										ScenesService::getInstance().tianxia[B].saidian = 2;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserA->addBindObjectNum(36003,1,0,0,1);
									}
								}
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
								int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
								ScenesService::getInstance().tianxia[A].saidian =2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
								int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
								ScenesService::getInstance().tianxia[B].saidian =2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}	
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//A组没有选手,B组有选手
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家A
							if(BChar)
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}


					a=26;
					b=27;
					aa=30;
					bb=31;
					//第八组
					if(ScenesService::getInstance().tianxia[a].saidian == 1 || ScenesService::getInstance().tianxia[b].saidian == 1)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//匹配成功
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
							int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								if (ScenesService::getInstance().tianxia[A].jifen > ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[A].saidian = 2;
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserB->addBindObjectNum(36003,1,0,0,1);
								}
								else if (ScenesService::getInstance().tianxia[A].jifen < ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[B].saidian = 2;

									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserA->addBindObjectNum(36003,1,0,0,1);
								}
								else
								{
									if (pUserA->charbase.zhanli > pUserB->charbase.zhanli)
									{
										ScenesService::getInstance().tianxia[A].saidian = 2;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserB->addBindObjectNum(36003,1,0,0,1);
										pUserB->addBindObjectNum(36003,1,0,0,1);
									}
									else
									{
										ScenesService::getInstance().tianxia[B].saidian = 2;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserA->addBindObjectNum(36003,1,0,0,1);
									}
								}
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
								int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
								ScenesService::getInstance().tianxia[A].saidian =2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								int A = ScenesService::getInstance().tianxia[a].saidian == 1?a:b;
								int B = ScenesService::getInstance().tianxia[aa].saidian == 1?aa:bb;
								ScenesService::getInstance().tianxia[B].saidian =2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int AChar = ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].userid:ScenesService::getInstance().tianxia[b].userid;
							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[a].saidian == 1?ScenesService::getInstance().tianxia[a].saidian=2:ScenesService::getInstance().tianxia[b].saidian=2;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}	
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 1 || ScenesService::getInstance().tianxia[bb].saidian == 1)
						{
							//A组没有选手,B组有选手
							int BChar = ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].userid:ScenesService::getInstance().tianxia[bb].userid;
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家A
							if(BChar)
							{
								ScenesService::getInstance().tianxia[aa].saidian == 1?ScenesService::getInstance().tianxia[aa].saidian=2:ScenesService::getInstance().tianxia[bb].saidian=2;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}
				}
			}
			if (tv1.tm_hour == 22) // 天下第一大赛复选开始！
			{
				if (tv1.tm_min == 24)
				{
					for(int i=0;i<32;i++)
					{
						ScenesService::getInstance().tianxia[i].jifen =0;
					}
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一复选赛现在开始，即将传送各位参赛选手，让我们拭目以待！");	
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一复选赛现在开始，即将传送各位参赛选手，让我们拭目以待！");		
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一复选赛现在开始，即将传送各位参赛选手，让我们拭目以待！");		
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一复选赛现在开始，即将传送各位参赛选手，让我们拭目以待！");		
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一复选赛现在开始，即将传送各位参赛选手，让我们拭目以待！");	


					//分组处理
					int a = 0;
					int b = 0;
					int c = 0;
					int d = 0;
					int aa=0;
					int bb=0;
					int cc=0;
					int dd=0;
					a=0;
					b=1;
					c=4;
					d=5;
					aa=8;
					bb=9;
					cc=12;
					dd=13;
					//第一组
					//第一组
					if(ScenesService::getInstance().tianxia[a].saidian == 2 || ScenesService::getInstance().tianxia[b].saidian == 2 || ScenesService::getInstance().tianxia[c].saidian == 2 || ScenesService::getInstance().tianxia[d].saidian == 2)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 2 || ScenesService::getInstance().tianxia[bb].saidian == 2 || ScenesService::getInstance().tianxia[cc].saidian == 2 || ScenesService::getInstance().tianxia[dd].saidian == 2)
						{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 2)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 2)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 2)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 2)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}


							if(ScenesService::getInstance().tianxia[aa].saidian == 2)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 2)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 2)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 2)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·天下第一复选1组 pos=48,56");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·天下第一复选1组 pos=69,55");  //自动传送地图坐标
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[A].saidian =3;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[B].saidian =3;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 2)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 2)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 2)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 2)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}


							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[A].saidian =3;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}
						}
					}
					else
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 2 || ScenesService::getInstance().tianxia[bb].saidian == 2)
						{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;

							if(ScenesService::getInstance().tianxia[aa].saidian == 2)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 2)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 2)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 2)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}

							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(BChar)
							{
								ScenesService::getInstance().tianxia[B].saidian =3;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}


					a=2;
					b=3;
					c=6;
					d=7;
					aa=10;
					bb=11;
					cc=14;
					dd=15;
					//第二组
					if(ScenesService::getInstance().tianxia[a].saidian == 2 || ScenesService::getInstance().tianxia[b].saidian == 2 || ScenesService::getInstance().tianxia[c].saidian == 2 || ScenesService::getInstance().tianxia[d].saidian == 2)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 2 || ScenesService::getInstance().tianxia[bb].saidian == 2 || ScenesService::getInstance().tianxia[cc].saidian == 2 || ScenesService::getInstance().tianxia[dd].saidian == 2)
						{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 2)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 2)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 2)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 2)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}


							if(ScenesService::getInstance().tianxia[aa].saidian == 2)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 2)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 2)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 2)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·天下第一复选2组 pos=48,56");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·天下第一复选2组 pos=69,55");  //自动传送地图坐标
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[A].saidian =3;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[B].saidian =3;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 2)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 2)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 2)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 2)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}


							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[A].saidian =3;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}
						}
					}
					else
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 2 || ScenesService::getInstance().tianxia[bb].saidian == 2)
						{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;

							if(ScenesService::getInstance().tianxia[aa].saidian == 2)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 2)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 2)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 2)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}

							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(BChar)
							{
								ScenesService::getInstance().tianxia[B].saidian =3;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}			


					a=16;
					b=17;
					c=20;
					d=21;
					aa=24;
					bb=25;
					cc=28;
					dd=29;
					//第二组
					if(ScenesService::getInstance().tianxia[a].saidian == 2 || ScenesService::getInstance().tianxia[b].saidian == 2 || ScenesService::getInstance().tianxia[c].saidian == 2 || ScenesService::getInstance().tianxia[d].saidian == 2)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 2 || ScenesService::getInstance().tianxia[bb].saidian == 2 || ScenesService::getInstance().tianxia[cc].saidian == 2 || ScenesService::getInstance().tianxia[dd].saidian == 2)
						{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 2)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 2)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 2)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 2)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}


							if(ScenesService::getInstance().tianxia[aa].saidian == 2)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 2)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 2)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 2)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·天下第一复选3组 pos=48,56");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·天下第一复选3组 pos=69,55");  //自动传送地图坐标
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[A].saidian =3;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[B].saidian =3;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 2)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 2)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 2)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 2)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}


							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[A].saidian =3;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}
						}
					}
					else
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 2 || ScenesService::getInstance().tianxia[bb].saidian == 2)
						{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;

							if(ScenesService::getInstance().tianxia[aa].saidian == 2)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 2)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 2)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 2)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}

							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(BChar)
							{
								ScenesService::getInstance().tianxia[B].saidian =3;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}					

					a=18;
					b=19;
					c=22;
					d=23;
					aa=26;
					bb=27;
					cc=30;
					dd=31;
					//第二组
					if(ScenesService::getInstance().tianxia[a].saidian == 2 || ScenesService::getInstance().tianxia[b].saidian == 2 || ScenesService::getInstance().tianxia[c].saidian == 2 || ScenesService::getInstance().tianxia[d].saidian == 2)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 2 || ScenesService::getInstance().tianxia[bb].saidian == 2 || ScenesService::getInstance().tianxia[cc].saidian == 2 || ScenesService::getInstance().tianxia[dd].saidian == 2)
						{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 2)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 2)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 2)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 2)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}


							if(ScenesService::getInstance().tianxia[aa].saidian == 2)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 2)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 2)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 2)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·天下第一复选4组 pos=48,56");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·天下第一复选4组 pos=69,55");  //自动传送地图坐标
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[A].saidian =3;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[B].saidian =3;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 2)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 2)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 2)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 2)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}


							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[A].saidian =3;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}
						}
					}
					else
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 2 || ScenesService::getInstance().tianxia[bb].saidian == 2)
						{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;

							if(ScenesService::getInstance().tianxia[aa].saidian == 2)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 2)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 2)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 2)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}

							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(BChar)
							{
								ScenesService::getInstance().tianxia[B].saidian =3;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}					
					

				}
			}

			if (tv1.tm_hour == 22) // 天下第一大赛复选结算！
			{
				if (tv1.tm_min == 28)
				{
					//分组处理
					int a = 0;
					int b = 0;
					int c = 0;
					int d = 0;
					int aa=0;
					int bb=0;
					int cc=0;
					int dd=0;
					a=0;
					b=1;
					c=4;
					d=5;
					aa=8;
					bb=9;
					cc=12;
					dd=13;
					//第一组
					if(ScenesService::getInstance().tianxia[a].saidian == 2 || ScenesService::getInstance().tianxia[b].saidian == 2 || ScenesService::getInstance().tianxia[c].saidian == 2 || ScenesService::getInstance().tianxia[d].saidian == 2)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 2 || ScenesService::getInstance().tianxia[bb].saidian == 2 || ScenesService::getInstance().tianxia[cc].saidian == 2 || ScenesService::getInstance().tianxia[dd].saidian == 2)
						{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 2)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 2)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 2)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 2)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}


							if(ScenesService::getInstance().tianxia[aa].saidian == 2)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 2)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 2)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 2)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								if (ScenesService::getInstance().tianxia[A].jifen > ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[A].saidian = 3;								
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserB->addBindObjectNum(36003,1,0,0,1);
								}
								else if (ScenesService::getInstance().tianxia[A].jifen < ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[B].saidian = 3;
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserA->addBindObjectNum(36003,1,0,0,1);
								}
								else
								{
									if (pUserA->charbase.zhanli > pUserB->charbase.zhanli)
									{
										ScenesService::getInstance().tianxia[A].saidian = 3;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserB->addBindObjectNum(36003,1,0,0,1);
									}
									else
									{
										ScenesService::getInstance().tianxia[B].saidian = 3;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserA->addBindObjectNum(36003,1,0,0,1);
									}
								}
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[A].saidian = 3;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[B].saidian = 3;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							int A=0;
							int B=0;
							int AChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 2)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 2)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 2)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 2)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[A].saidian=3;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}	
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 2 || ScenesService::getInstance().tianxia[bb].saidian == 2 || ScenesService::getInstance().tianxia[cc].saidian == 2 ||ScenesService::getInstance().tianxia[dd].saidian == 2 )
						{
							int B=0;
							int BChar = 0;

							if(ScenesService::getInstance().tianxia[aa].saidian == 2)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 2)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 2)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 2)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}

							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(BChar)
							{
								ScenesService::getInstance().tianxia[B].saidian=3;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}




					a=2;
					b=3;
					c=5;
					d=7;
					aa=10;
					bb=11;
					cc=14;
					dd=15;
					//第一组
					if(ScenesService::getInstance().tianxia[a].saidian == 2 || ScenesService::getInstance().tianxia[b].saidian == 2 || ScenesService::getInstance().tianxia[c].saidian == 2 || ScenesService::getInstance().tianxia[d].saidian == 2)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 2 || ScenesService::getInstance().tianxia[bb].saidian == 2 || ScenesService::getInstance().tianxia[cc].saidian == 2 || ScenesService::getInstance().tianxia[dd].saidian == 2)
						{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 2)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 2)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 2)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 2)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}


							if(ScenesService::getInstance().tianxia[aa].saidian == 2)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 2)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 2)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 2)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								if (ScenesService::getInstance().tianxia[A].jifen > ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[A].saidian = 3;								
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserB->addBindObjectNum(36003,1,0,0,1);
								}
								else if (ScenesService::getInstance().tianxia[A].jifen < ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[B].saidian = 3;
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserA->addBindObjectNum(36003,1,0,0,1);
								}
								else
								{
									if (pUserA->charbase.zhanli > pUserB->charbase.zhanli)
									{
										ScenesService::getInstance().tianxia[A].saidian = 3;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserB->addBindObjectNum(36003,1,0,0,1);
									}
									else
									{
										ScenesService::getInstance().tianxia[B].saidian = 3;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserA->addBindObjectNum(36003,1,0,0,1);
									}
								}
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[A].saidian = 3;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[B].saidian = 3;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							int A=0;
							int B=0;
							int AChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 2)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 2)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 2)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 2)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[A].saidian=3;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}	
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 2 || ScenesService::getInstance().tianxia[bb].saidian == 2 || ScenesService::getInstance().tianxia[cc].saidian == 2 ||ScenesService::getInstance().tianxia[dd].saidian == 2 )
						{
							int B=0;
							int BChar = 0;

							if(ScenesService::getInstance().tianxia[aa].saidian == 2)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 2)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 2)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 2)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}

							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(BChar)
							{
								ScenesService::getInstance().tianxia[B].saidian=3;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}


					a=16;
					b=17;
					c=20;
					d=21;
					aa=24;
					bb=25;
					cc=28;
					dd=29;
					//第一组
					if(ScenesService::getInstance().tianxia[a].saidian == 2 || ScenesService::getInstance().tianxia[b].saidian == 2 || ScenesService::getInstance().tianxia[c].saidian == 2 || ScenesService::getInstance().tianxia[d].saidian == 2)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 2 || ScenesService::getInstance().tianxia[bb].saidian == 2 || ScenesService::getInstance().tianxia[cc].saidian == 2 || ScenesService::getInstance().tianxia[dd].saidian == 2)
						{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 2)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 2)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 2)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 2)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}


							if(ScenesService::getInstance().tianxia[aa].saidian == 2)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 2)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 2)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 2)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								if (ScenesService::getInstance().tianxia[A].jifen > ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[A].saidian = 3;								
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserB->addBindObjectNum(36003,1,0,0,1);
								}
								else if (ScenesService::getInstance().tianxia[A].jifen < ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[B].saidian = 3;
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserA->addBindObjectNum(36003,1,0,0,1);
								}
								else
								{
									if (pUserA->charbase.zhanli > pUserB->charbase.zhanli)
									{
										ScenesService::getInstance().tianxia[A].saidian = 3;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserB->addBindObjectNum(36003,1,0,0,1);
									}
									else
									{
										ScenesService::getInstance().tianxia[B].saidian = 3;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserA->addBindObjectNum(36003,1,0,0,1);
									}
								}
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[A].saidian = 3;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[B].saidian = 3;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							int A=0;
							int B=0;
							int AChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 2)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 2)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 2)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 2)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[A].saidian=3;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}	
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 2 || ScenesService::getInstance().tianxia[bb].saidian == 2 || ScenesService::getInstance().tianxia[cc].saidian == 2 ||ScenesService::getInstance().tianxia[dd].saidian == 2 )
						{
							int B=0;
							int BChar = 0;

							if(ScenesService::getInstance().tianxia[aa].saidian == 2)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 2)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 2)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 2)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}

							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(BChar)
							{
								ScenesService::getInstance().tianxia[B].saidian=3;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}



					a=18;
					b=19;
					c=22;
					d=23;
					aa=26;
					bb=27;
					cc=30;
					dd=31;
					//第一组
					if(ScenesService::getInstance().tianxia[a].saidian == 2 || ScenesService::getInstance().tianxia[b].saidian == 2 || ScenesService::getInstance().tianxia[c].saidian == 2 || ScenesService::getInstance().tianxia[d].saidian == 2)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 2 || ScenesService::getInstance().tianxia[bb].saidian == 2 || ScenesService::getInstance().tianxia[cc].saidian == 2 || ScenesService::getInstance().tianxia[dd].saidian == 2)
						{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 2)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 2)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 2)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 2)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}


							if(ScenesService::getInstance().tianxia[aa].saidian == 2)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 2)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 2)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 2)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								if (ScenesService::getInstance().tianxia[A].jifen > ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[A].saidian = 3;								
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserB->addBindObjectNum(36003,1,0,0,1);
								}
								else if (ScenesService::getInstance().tianxia[A].jifen < ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[B].saidian = 3;
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserA->addBindObjectNum(36003,1,0,0,1);
								}
								else
								{
									if (pUserA->charbase.zhanli > pUserB->charbase.zhanli)
									{
										ScenesService::getInstance().tianxia[A].saidian = 3;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserB->addBindObjectNum(36003,1,0,0,1);
									}
									else
									{
										ScenesService::getInstance().tianxia[B].saidian = 3;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserA->addBindObjectNum(36003,1,0,0,1);
									}
								}
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[A].saidian = 3;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[B].saidian = 3;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							int A=0;
							int B=0;
							int AChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 2)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 2)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 2)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 2)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[A].saidian=3;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}	
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 2 || ScenesService::getInstance().tianxia[bb].saidian == 2 || ScenesService::getInstance().tianxia[cc].saidian == 2 ||ScenesService::getInstance().tianxia[dd].saidian == 2 )
						{
							int B=0;
							int BChar = 0;

							if(ScenesService::getInstance().tianxia[aa].saidian == 2)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 2)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 2)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 2)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}

							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(BChar)
							{
								ScenesService::getInstance().tianxia[B].saidian=3;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}

				}
				
			}
			if (tv1.tm_hour == 22) // 天下第一大赛半决赛开始！
			{
				if (tv1.tm_min == 32)
				{
					for(int i=0;i<32;i++)
					{
						ScenesService::getInstance().tianxia[i].jifen =0;
					}
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一半决赛现在开始，即将传送各位参赛选手，让我们拭目以待！");	
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一半决赛现在开始，即将传送各位参赛选手，让我们拭目以待！");		
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一半决赛现在开始，即将传送各位参赛选手，让我们拭目以待！");		
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一半决赛现在开始，即将传送各位参赛选手，让我们拭目以待！");		
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一半决赛现在开始，即将传送各位参赛选手，让我们拭目以待！");	
					//分组处理
					int a = 0;
					int b = 0;
					int c = 0;
					int d = 0;
					int e=0;
					int f=0;
					int g=0;
					int h=0;

					int aa = 0;
					int bb = 0;
					int cc = 0;
					int dd = 0;
					int ee=0;
					int ff=0;
					int gg=0;
					int hh=0;
					a=0;
					b=1;
					c=4;
					d=5;
					e=8;
					f=9;
					g=12;
					h=13;
					aa=16;
					bb=17;
					cc=20;
					dd=21;
					ee=24;
					ff=25;
					gg=28;
					hh=29;
					//第一组
					//第一组
					if(ScenesService::getInstance().tianxia[a].saidian == 3 || ScenesService::getInstance().tianxia[b].saidian == 3 || ScenesService::getInstance().tianxia[c].saidian == 3 || ScenesService::getInstance().tianxia[d].saidian == 3
					|| ScenesService::getInstance().tianxia[e].saidian == 3 || ScenesService::getInstance().tianxia[f].saidian == 3 || ScenesService::getInstance().tianxia[g].saidian == 3 || ScenesService::getInstance().tianxia[h].saidian == 3)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 3 || ScenesService::getInstance().tianxia[bb].saidian == 3 || ScenesService::getInstance().tianxia[cc].saidian == 3 || ScenesService::getInstance().tianxia[dd].saidian == 3
						|| ScenesService::getInstance().tianxia[ee].saidian == 3 || ScenesService::getInstance().tianxia[ff].saidian == 3 || ScenesService::getInstance().tianxia[gg].saidian == 3 || ScenesService::getInstance().tianxia[hh].saidian == 3)
					{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 3)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 3)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 3)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 3)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}
							else if(ScenesService::getInstance().tianxia[e].saidian == 3)
							{
								A=e;
								AChar = ScenesService::getInstance().tianxia[e].userid;
							}
							else if(ScenesService::getInstance().tianxia[f].saidian == 3)
							{
								A=f;
								AChar = ScenesService::getInstance().tianxia[f].userid;
							}
							else if(ScenesService::getInstance().tianxia[g].saidian == 3)
							{
								A=g;
								AChar = ScenesService::getInstance().tianxia[g].userid;
							}
							else if(ScenesService::getInstance().tianxia[h].saidian == 3)
							{
								A=h;
								AChar = ScenesService::getInstance().tianxia[h].userid;
							}
							

							if(ScenesService::getInstance().tianxia[aa].saidian == 3)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 3)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 3)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 3)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}
							else if(ScenesService::getInstance().tianxia[ee].saidian == 3)
							{
								B=ee;
								BChar = ScenesService::getInstance().tianxia[ee].userid;
							}
							else if(ScenesService::getInstance().tianxia[ff].saidian == 3)
							{
								B=ff;
								BChar = ScenesService::getInstance().tianxia[ff].userid;
							}
							else if(ScenesService::getInstance().tianxia[gg].saidian == 3)
							{
								B=gg;
								BChar = ScenesService::getInstance().tianxia[gg].userid;
							}
							else if(ScenesService::getInstance().tianxia[hh].saidian == 3)
							{
								B=hh;
								BChar = ScenesService::getInstance().tianxia[hh].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·天下第一半决赛1组 pos=48,56");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·天下第一半决赛1组 pos=69,55");  //自动传送地图坐标
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[A].saidian =4;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");						
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[B].saidian =4;
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int A=0;
							int AChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 3)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 3)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 3)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 3)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}
							else if(ScenesService::getInstance().tianxia[e].saidian == 3)
							{
								A=e;
								AChar = ScenesService::getInstance().tianxia[e].userid;
							}
							else if(ScenesService::getInstance().tianxia[f].saidian == 3)
							{
								A=f;
								AChar = ScenesService::getInstance().tianxia[f].userid;
							}
							else if(ScenesService::getInstance().tianxia[g].saidian == 3)
							{
								A=g;
								AChar = ScenesService::getInstance().tianxia[g].userid;
							}
							else if(ScenesService::getInstance().tianxia[h].saidian == 3)
							{
								A=h;
								AChar = ScenesService::getInstance().tianxia[h].userid;
							}
							

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[A].saidian =4;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}
						}
					}
					else
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 3 || ScenesService::getInstance().tianxia[bb].saidian == 3 || ScenesService::getInstance().tianxia[cc].saidian == 3 || ScenesService::getInstance().tianxia[dd].saidian == 3
						|| ScenesService::getInstance().tianxia[ee].saidian == 3 || ScenesService::getInstance().tianxia[ff].saidian == 3 || ScenesService::getInstance().tianxia[gg].saidian == 3 || ScenesService::getInstance().tianxia[hh].saidian == 3)
					{
							int B=0;
							int BChar = 0;
							
							if(ScenesService::getInstance().tianxia[aa].saidian == 3)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 3)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 3)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 3)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}
							else if(ScenesService::getInstance().tianxia[ee].saidian == 3)
							{
								B=ee;
								BChar = ScenesService::getInstance().tianxia[ee].userid;
							}
							else if(ScenesService::getInstance().tianxia[ff].saidian == 3)
							{
								B=ff;
								BChar = ScenesService::getInstance().tianxia[ff].userid;
							}
							else if(ScenesService::getInstance().tianxia[gg].saidian == 3)
							{
								B=gg;
								BChar = ScenesService::getInstance().tianxia[gg].userid;
							}
							else if(ScenesService::getInstance().tianxia[hh].saidian == 3)
							{
								B=hh;
								BChar = ScenesService::getInstance().tianxia[hh].userid;
							}

							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(BChar)
							{
								ScenesService::getInstance().tianxia[B].saidian =4;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}


					a=2;
					b=3;
					c=6;
					d=7;
					e=10;
					f=11;
					g=14;
					h=15;
					aa=18;
					bb=19;
					cc=22;
					dd=23;
					ee=26;
					ff=27;
					gg=30;
					hh=31;
					//第一组
					//第一组
					if(ScenesService::getInstance().tianxia[a].saidian == 3 || ScenesService::getInstance().tianxia[b].saidian == 3 || ScenesService::getInstance().tianxia[c].saidian == 3 || ScenesService::getInstance().tianxia[d].saidian == 3
					|| ScenesService::getInstance().tianxia[e].saidian == 3 || ScenesService::getInstance().tianxia[f].saidian == 3 || ScenesService::getInstance().tianxia[g].saidian == 3 || ScenesService::getInstance().tianxia[h].saidian == 3)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 3 || ScenesService::getInstance().tianxia[bb].saidian == 3 || ScenesService::getInstance().tianxia[cc].saidian == 3 || ScenesService::getInstance().tianxia[dd].saidian == 3
						|| ScenesService::getInstance().tianxia[ee].saidian == 3 || ScenesService::getInstance().tianxia[ff].saidian == 3 || ScenesService::getInstance().tianxia[gg].saidian == 3 || ScenesService::getInstance().tianxia[hh].saidian == 3)
					{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 3)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 3)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 3)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 3)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}
							else if(ScenesService::getInstance().tianxia[e].saidian == 3)
							{
								A=e;
								AChar = ScenesService::getInstance().tianxia[e].userid;
							}
							else if(ScenesService::getInstance().tianxia[f].saidian == 3)
							{
								A=f;
								AChar = ScenesService::getInstance().tianxia[f].userid;
							}
							else if(ScenesService::getInstance().tianxia[g].saidian == 3)
							{
								A=g;
								AChar = ScenesService::getInstance().tianxia[g].userid;
							}
							else if(ScenesService::getInstance().tianxia[h].saidian == 3)
							{
								A=h;
								AChar = ScenesService::getInstance().tianxia[h].userid;
							}
							

							if(ScenesService::getInstance().tianxia[aa].saidian == 3)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 3)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 3)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 3)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}
							else if(ScenesService::getInstance().tianxia[ee].saidian == 3)
							{
								B=ee;
								BChar = ScenesService::getInstance().tianxia[ee].userid;
							}
							else if(ScenesService::getInstance().tianxia[ff].saidian == 3)
							{
								B=ff;
								BChar = ScenesService::getInstance().tianxia[ff].userid;
							}
							else if(ScenesService::getInstance().tianxia[gg].saidian == 3)
							{
								B=gg;
								BChar = ScenesService::getInstance().tianxia[gg].userid;
							}
							else if(ScenesService::getInstance().tianxia[hh].saidian == 3)
							{
								B=hh;
								BChar = ScenesService::getInstance().tianxia[hh].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·天下第一半决赛2组 pos=48,56");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·天下第一半决赛2组 pos=69,55");  //自动传送地图坐标
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[A].saidian =4;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[B].saidian =4;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							//A组有选手,B组没有选手
							int A=0;
							int AChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 3)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 3)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 3)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 3)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}
							else if(ScenesService::getInstance().tianxia[e].saidian == 3)
							{
								A=e;
								AChar = ScenesService::getInstance().tianxia[e].userid;
							}
							else if(ScenesService::getInstance().tianxia[f].saidian == 3)
							{
								A=f;
								AChar = ScenesService::getInstance().tianxia[f].userid;
							}
							else if(ScenesService::getInstance().tianxia[g].saidian == 3)
							{
								A=g;
								AChar = ScenesService::getInstance().tianxia[g].userid;
							}
							else if(ScenesService::getInstance().tianxia[h].saidian == 3)
							{
								A=h;
								AChar = ScenesService::getInstance().tianxia[h].userid;
							}
							

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[A].saidian =4;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}
						}
					}
					else
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 3 || ScenesService::getInstance().tianxia[bb].saidian == 3 || ScenesService::getInstance().tianxia[cc].saidian == 3 || ScenesService::getInstance().tianxia[dd].saidian == 3
						|| ScenesService::getInstance().tianxia[ee].saidian == 3 || ScenesService::getInstance().tianxia[ff].saidian == 3 || ScenesService::getInstance().tianxia[gg].saidian == 3 || ScenesService::getInstance().tianxia[hh].saidian == 3)
					{
							int B=0;
							int BChar = 0;
							
							if(ScenesService::getInstance().tianxia[aa].saidian == 3)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 3)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 3)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 3)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}
							else if(ScenesService::getInstance().tianxia[ee].saidian == 3)
							{
								B=ee;
								BChar = ScenesService::getInstance().tianxia[ee].userid;
							}
							else if(ScenesService::getInstance().tianxia[ff].saidian == 3)
							{
								B=ff;
								BChar = ScenesService::getInstance().tianxia[ff].userid;
							}
							else if(ScenesService::getInstance().tianxia[gg].saidian == 3)
							{
								B=gg;
								BChar = ScenesService::getInstance().tianxia[gg].userid;
							}
							else if(ScenesService::getInstance().tianxia[hh].saidian == 3)
							{
								B=hh;
								BChar = ScenesService::getInstance().tianxia[hh].userid;
							}

							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(BChar)
							{
								ScenesService::getInstance().tianxia[B].saidian =4;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}
				}
			}
			if (tv1.tm_hour == 22) // 天下第一大赛半决赛结算！
			{
				if (tv1.tm_min == 36)
				{
					//分组处理
					int a = 0;
					int b = 0;
					int c = 0;
					int d = 0;
					int e=0;
					int f=0;
					int g=0;
					int h=0;

					int aa = 0;
					int bb = 0;
					int cc = 0;
					int dd = 0;
					int ee=0;
					int ff=0;
					int gg=0;
					int hh=0;
					a=0;
					b=1;
					c=4;
					d=5;
					e=8;
					f=9;
					g=12;
					h=13;
					aa=16;
					bb=17;
					cc=20;
					dd=21;
					ee=24;
					ff=25;
					gg=28;
					hh=29;
					//第一组
					//第一组
					if(ScenesService::getInstance().tianxia[a].saidian == 3 || ScenesService::getInstance().tianxia[b].saidian == 3 || ScenesService::getInstance().tianxia[c].saidian == 3 || ScenesService::getInstance().tianxia[d].saidian == 3
					|| ScenesService::getInstance().tianxia[e].saidian == 3 || ScenesService::getInstance().tianxia[f].saidian == 3 || ScenesService::getInstance().tianxia[g].saidian == 3 || ScenesService::getInstance().tianxia[h].saidian == 3)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 3 || ScenesService::getInstance().tianxia[bb].saidian == 3 || ScenesService::getInstance().tianxia[cc].saidian == 3 || ScenesService::getInstance().tianxia[dd].saidian == 3
						|| ScenesService::getInstance().tianxia[ee].saidian == 3 || ScenesService::getInstance().tianxia[ff].saidian == 3 || ScenesService::getInstance().tianxia[gg].saidian == 3 || ScenesService::getInstance().tianxia[hh].saidian == 3)
					{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 3)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 3)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 3)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 3)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}
							else if(ScenesService::getInstance().tianxia[e].saidian == 3)
							{
								A=e;
								AChar = ScenesService::getInstance().tianxia[e].userid;
							}
							else if(ScenesService::getInstance().tianxia[f].saidian == 3)
							{
								A=f;
								AChar = ScenesService::getInstance().tianxia[f].userid;
							}
							else if(ScenesService::getInstance().tianxia[g].saidian == 3)
							{
								A=g;
								AChar = ScenesService::getInstance().tianxia[g].userid;
							}
							else if(ScenesService::getInstance().tianxia[h].saidian == 3)
							{
								A=h;
								AChar = ScenesService::getInstance().tianxia[h].userid;
							}
							

							if(ScenesService::getInstance().tianxia[aa].saidian == 3)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 3)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 3)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 3)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}
							else if(ScenesService::getInstance().tianxia[ee].saidian == 3)
							{
								B=ee;
								BChar = ScenesService::getInstance().tianxia[ee].userid;
							}
							else if(ScenesService::getInstance().tianxia[ff].saidian == 3)
							{
								B=ff;
								BChar = ScenesService::getInstance().tianxia[ff].userid;
							}
							else if(ScenesService::getInstance().tianxia[gg].saidian == 3)
							{
								B=gg;
								BChar = ScenesService::getInstance().tianxia[gg].userid;
							}
							else if(ScenesService::getInstance().tianxia[hh].saidian == 3)
							{
								B=hh;
								BChar = ScenesService::getInstance().tianxia[hh].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								if (ScenesService::getInstance().tianxia[A].jifen > ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[A].saidian = 4;								
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserB->addBindObjectNum(36002,1,0,0,1);
								}
								else if (ScenesService::getInstance().tianxia[A].jifen < ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[B].saidian = 4;
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserA->addBindObjectNum(36002,1,0,0,1);
								}
								else
								{
									if (pUserA->charbase.zhanli > pUserB->charbase.zhanli)
									{
										ScenesService::getInstance().tianxia[A].saidian = 4;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserB->addBindObjectNum(36002,1,0,0,1);
									}
									else
									{
										ScenesService::getInstance().tianxia[B].saidian = 4;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserA->addBindObjectNum(36002,1,0,0,1);	
									}
								}
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[A].saidian = 4;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[B].saidian = 4;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
						int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 3)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 3)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 3)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 3)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}
							else if(ScenesService::getInstance().tianxia[e].saidian == 3)
							{
								A=e;
								AChar = ScenesService::getInstance().tianxia[e].userid;
							}
							else if(ScenesService::getInstance().tianxia[f].saidian == 3)
							{
								A=f;
								AChar = ScenesService::getInstance().tianxia[f].userid;
							}
							else if(ScenesService::getInstance().tianxia[g].saidian == 3)
							{
								A=g;
								AChar = ScenesService::getInstance().tianxia[g].userid;
							}
							else if(ScenesService::getInstance().tianxia[h].saidian == 3)
							{
								A=h;
								AChar = ScenesService::getInstance().tianxia[h].userid;
							}
							

							if(ScenesService::getInstance().tianxia[aa].saidian == 3)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 3)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 3)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 3)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}
							else if(ScenesService::getInstance().tianxia[ee].saidian == 3)
							{
								B=ee;
								BChar = ScenesService::getInstance().tianxia[ee].userid;
							}
							else if(ScenesService::getInstance().tianxia[ff].saidian == 3)
							{
								B=ff;
								BChar = ScenesService::getInstance().tianxia[ff].userid;
							}
							else if(ScenesService::getInstance().tianxia[gg].saidian == 3)
							{
								B=gg;
								BChar = ScenesService::getInstance().tianxia[gg].userid;
							}
							else if(ScenesService::getInstance().tianxia[hh].saidian == 3)
							{
								B=hh;
								BChar = ScenesService::getInstance().tianxia[hh].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[A].saidian=4;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}	
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 3 || ScenesService::getInstance().tianxia[bb].saidian == 3 || ScenesService::getInstance().tianxia[cc].saidian == 3 || ScenesService::getInstance().tianxia[dd].saidian == 3
						|| ScenesService::getInstance().tianxia[ee].saidian == 3 || ScenesService::getInstance().tianxia[ff].saidian == 3 || ScenesService::getInstance().tianxia[gg].saidian == 3 || ScenesService::getInstance().tianxia[hh].saidian == 3)
						{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 3)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 3)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 3)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 3)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}
							else if(ScenesService::getInstance().tianxia[e].saidian == 3)
							{
								A=e;
								AChar = ScenesService::getInstance().tianxia[e].userid;
							}
							else if(ScenesService::getInstance().tianxia[f].saidian == 3)
							{
								A=f;
								AChar = ScenesService::getInstance().tianxia[f].userid;
							}
							else if(ScenesService::getInstance().tianxia[g].saidian == 3)
							{
								A=g;
								AChar = ScenesService::getInstance().tianxia[g].userid;
							}
							else if(ScenesService::getInstance().tianxia[h].saidian == 3)
							{
								A=h;
								AChar = ScenesService::getInstance().tianxia[h].userid;
							}
							

							if(ScenesService::getInstance().tianxia[aa].saidian == 3)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 3)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 3)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 3)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}
							else if(ScenesService::getInstance().tianxia[ee].saidian == 3)
							{
								B=ee;
								BChar = ScenesService::getInstance().tianxia[ee].userid;
							}
							else if(ScenesService::getInstance().tianxia[ff].saidian == 3)
							{
								B=ff;
								BChar = ScenesService::getInstance().tianxia[ff].userid;
							}
							else if(ScenesService::getInstance().tianxia[gg].saidian == 3)
							{
								B=gg;
								BChar = ScenesService::getInstance().tianxia[gg].userid;
							}
							else if(ScenesService::getInstance().tianxia[hh].saidian == 3)
							{
								B=hh;
								BChar = ScenesService::getInstance().tianxia[hh].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(BChar)
							{
								ScenesService::getInstance().tianxia[B].saidian=4;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}


					a=2;
					b=3;
					c=6;
					d=7;
					e=10;
					f=11;
					g=14;
					h=15;
					aa=18;
					bb=19;
					cc=22;
					dd=23;
					ee=26;
					ff=27;
					gg=30;
					hh=31;
					//第一组
					//第一组
					if(ScenesService::getInstance().tianxia[a].saidian == 3 || ScenesService::getInstance().tianxia[b].saidian == 3 || ScenesService::getInstance().tianxia[c].saidian == 3 || ScenesService::getInstance().tianxia[d].saidian == 3
					|| ScenesService::getInstance().tianxia[e].saidian == 3 || ScenesService::getInstance().tianxia[f].saidian == 3 || ScenesService::getInstance().tianxia[g].saidian == 3 || ScenesService::getInstance().tianxia[h].saidian == 3)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 3 || ScenesService::getInstance().tianxia[bb].saidian == 3 || ScenesService::getInstance().tianxia[cc].saidian == 3 || ScenesService::getInstance().tianxia[dd].saidian == 3
						|| ScenesService::getInstance().tianxia[ee].saidian == 3 || ScenesService::getInstance().tianxia[ff].saidian == 3 || ScenesService::getInstance().tianxia[gg].saidian == 3 || ScenesService::getInstance().tianxia[hh].saidian == 3)
					{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 3)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 3)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 3)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 3)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}
							else if(ScenesService::getInstance().tianxia[e].saidian == 3)
							{
								A=e;
								AChar = ScenesService::getInstance().tianxia[e].userid;
							}
							else if(ScenesService::getInstance().tianxia[f].saidian == 3)
							{
								A=f;
								AChar = ScenesService::getInstance().tianxia[f].userid;
							}
							else if(ScenesService::getInstance().tianxia[g].saidian == 3)
							{
								A=g;
								AChar = ScenesService::getInstance().tianxia[g].userid;
							}
							else if(ScenesService::getInstance().tianxia[h].saidian == 3)
							{
								A=h;
								AChar = ScenesService::getInstance().tianxia[h].userid;
							}
							

							if(ScenesService::getInstance().tianxia[aa].saidian == 3)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 3)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 3)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 3)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}
							else if(ScenesService::getInstance().tianxia[ee].saidian == 3)
							{
								B=ee;
								BChar = ScenesService::getInstance().tianxia[ee].userid;
							}
							else if(ScenesService::getInstance().tianxia[ff].saidian == 3)
							{
								B=ff;
								BChar = ScenesService::getInstance().tianxia[ff].userid;
							}
							else if(ScenesService::getInstance().tianxia[gg].saidian == 3)
							{
								B=gg;
								BChar = ScenesService::getInstance().tianxia[gg].userid;
							}
							else if(ScenesService::getInstance().tianxia[hh].saidian == 3)
							{
								B=hh;
								BChar = ScenesService::getInstance().tianxia[hh].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								if (ScenesService::getInstance().tianxia[A].jifen > ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[A].saidian = 4;								
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserB->addBindObjectNum(36002,1,0,0,1);	
								}
								else if (ScenesService::getInstance().tianxia[A].jifen < ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[B].saidian = 4;
									Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
									pUserA->addBindObjectNum(36002,1,0,0,1);	
								}
								else
								{
									if (pUserA->charbase.zhanli > pUserB->charbase.zhanli)
									{
										ScenesService::getInstance().tianxia[A].saidian = 4;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserB->addBindObjectNum(36002,1,0,0,1);	
									}
									else
									{
										ScenesService::getInstance().tianxia[B].saidian = 4;
										Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】恭喜您战胜对手，已为您自动晋级，请等待下一轮开始！");
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，不要气馁，再接再厉。");
										pUserA->addBindObjectNum(36002,1,0,0,1);	
									}
								}
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[A].saidian = 4;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[B].saidian = 4;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
							
						}
						else{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 3)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 3)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 3)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 3)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}
							else if(ScenesService::getInstance().tianxia[e].saidian == 3)
							{
								A=e;
								AChar = ScenesService::getInstance().tianxia[e].userid;
							}
							else if(ScenesService::getInstance().tianxia[f].saidian == 3)
							{
								A=f;
								AChar = ScenesService::getInstance().tianxia[f].userid;
							}
							else if(ScenesService::getInstance().tianxia[g].saidian == 3)
							{
								A=g;
								AChar = ScenesService::getInstance().tianxia[g].userid;
							}
							else if(ScenesService::getInstance().tianxia[h].saidian == 3)
							{
								A=h;
								AChar = ScenesService::getInstance().tianxia[h].userid;
							}
							

							if(ScenesService::getInstance().tianxia[aa].saidian == 3)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 3)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 3)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 3)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}
							else if(ScenesService::getInstance().tianxia[ee].saidian == 3)
							{
								B=ee;
								BChar = ScenesService::getInstance().tianxia[ee].userid;
							}
							else if(ScenesService::getInstance().tianxia[ff].saidian == 3)
							{
								B=ff;
								BChar = ScenesService::getInstance().tianxia[ff].userid;
							}
							else if(ScenesService::getInstance().tianxia[gg].saidian == 3)
							{
								B=gg;
								BChar = ScenesService::getInstance().tianxia[gg].userid;
							}
							else if(ScenesService::getInstance().tianxia[hh].saidian == 3)
							{
								B=hh;
								BChar = ScenesService::getInstance().tianxia[hh].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[A].saidian=4;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");			
							}	
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 3 || ScenesService::getInstance().tianxia[bb].saidian == 3 || ScenesService::getInstance().tianxia[cc].saidian == 3 || ScenesService::getInstance().tianxia[dd].saidian == 3
						|| ScenesService::getInstance().tianxia[ee].saidian == 3 || ScenesService::getInstance().tianxia[ff].saidian == 3 || ScenesService::getInstance().tianxia[gg].saidian == 3 || ScenesService::getInstance().tianxia[hh].saidian == 3)
						{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 3)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 3)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 3)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 3)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}
							else if(ScenesService::getInstance().tianxia[e].saidian == 3)
							{
								A=e;
								AChar = ScenesService::getInstance().tianxia[e].userid;
							}
							else if(ScenesService::getInstance().tianxia[f].saidian == 3)
							{
								A=f;
								AChar = ScenesService::getInstance().tianxia[f].userid;
							}
							else if(ScenesService::getInstance().tianxia[g].saidian == 3)
							{
								A=g;
								AChar = ScenesService::getInstance().tianxia[g].userid;
							}
							else if(ScenesService::getInstance().tianxia[h].saidian == 3)
							{
								A=h;
								AChar = ScenesService::getInstance().tianxia[h].userid;
							}
							

							if(ScenesService::getInstance().tianxia[aa].saidian == 3)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 3)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 3)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 3)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}
							else if(ScenesService::getInstance().tianxia[ee].saidian == 3)
							{
								B=ee;
								BChar = ScenesService::getInstance().tianxia[ee].userid;
							}
							else if(ScenesService::getInstance().tianxia[ff].saidian == 3)
							{
								B=ff;
								BChar = ScenesService::getInstance().tianxia[ff].userid;
							}
							else if(ScenesService::getInstance().tianxia[gg].saidian == 3)
							{
								B=gg;
								BChar = ScenesService::getInstance().tianxia[gg].userid;
							}
							else if(ScenesService::getInstance().tianxia[hh].saidian == 3)
							{
								B=hh;
								BChar = ScenesService::getInstance().tianxia[hh].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(BChar)
							{
								ScenesService::getInstance().tianxia[B].saidian=4;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，已为您自动晋级，请等待下一轮开始！");		
							}
						}
					}

				}
			}
			if (tv1.tm_hour == 22) // 天下第一大赛总决赛开始！
			{
				if (tv1.tm_min == 40)
				{
					for(int i=0;i<32;i++)
					{
						ScenesService::getInstance().tianxia[i].jifen =0;
					}

					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一总决赛现在开始，即将传送各位参赛选手，让我们拭目以待！");	
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一总决赛现在开始，即将传送各位参赛选手，让我们拭目以待！");		
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一总决赛现在开始，即将传送各位参赛选手，让我们拭目以待！");		
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一总决赛现在开始，即将传送各位参赛选手，让我们拭目以待！");		
					Channel::sendAllInfo(Cmd::INFO_TYPE_EXP2,"天下第一总决赛现在开始，即将传送各位参赛选手，让我们拭目以待！");	
					//分组处理
					int a = 0;
					int b = 0;
					int c = 0;
					int d = 0;
					int e=0;
					int f=0;
					int g=0;
					int h=0;
					int i = 0;
					int j = 0;
					int k = 0;
					int l = 0;
					int m=0;
					int n=0;
					int o=0;
					int p=0;

					int aa = 0;
					int bb = 0;
					int cc = 0;
					int dd = 0;
					int ee=0;
					int ff=0;
					int gg=0;
					int hh=0;
					int ii = 0;
					int jj = 0;
					int kk = 0;
					int ll = 0;
					int mm=0;
					int nn=0;
					int oo=0;
					int pp=0;
					a=0;
					b=1;
					c=4;
					d=5;
					e=8;
					f=9;
					g=12;
					h=13;
					i=16;
					j=17;
					k=20;
					l=21;
					m=24;
					n=25;
					o=28;
					p=29;
					
					aa=2;
					bb=3;
					cc=6;
					dd=7;
					ee=10;
					ff=11;
					gg=14;
					hh=15;
					ii=18;
					jj=19;
					kk=22;
					ll=23;
					mm=26;
					nn=27;
					oo=30;
					pp=31;
					//第一组
					//第一组
					if(ScenesService::getInstance().tianxia[a].saidian == 4 || ScenesService::getInstance().tianxia[b].saidian == 4 || ScenesService::getInstance().tianxia[c].saidian == 4 || ScenesService::getInstance().tianxia[d].saidian == 4
					|| ScenesService::getInstance().tianxia[e].saidian == 4 || ScenesService::getInstance().tianxia[f].saidian == 4 || ScenesService::getInstance().tianxia[g].saidian == 4 || ScenesService::getInstance().tianxia[h].saidian == 4
					||ScenesService::getInstance().tianxia[o].saidian == 4 || ScenesService::getInstance().tianxia[i].saidian == 4 || ScenesService::getInstance().tianxia[j].saidian == 4 || ScenesService::getInstance().tianxia[k].saidian == 4
					|| ScenesService::getInstance().tianxia[p].saidian == 4 || ScenesService::getInstance().tianxia[l].saidian == 4 || ScenesService::getInstance().tianxia[m].saidian == 4 || ScenesService::getInstance().tianxia[n].saidian == 4)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 4 || ScenesService::getInstance().tianxia[bb].saidian == 4 || ScenesService::getInstance().tianxia[cc].saidian == 4 || ScenesService::getInstance().tianxia[dd].saidian == 4
						|| ScenesService::getInstance().tianxia[ee].saidian == 4 || ScenesService::getInstance().tianxia[ff].saidian == 4 || ScenesService::getInstance().tianxia[gg].saidian == 4 || ScenesService::getInstance().tianxia[hh].saidian == 4
						||ScenesService::getInstance().tianxia[ii].saidian == 4 || ScenesService::getInstance().tianxia[jj].saidian == 4 || ScenesService::getInstance().tianxia[kk].saidian == 4 || ScenesService::getInstance().tianxia[ll].saidian == 4
						|| ScenesService::getInstance().tianxia[mm].saidian == 4 || ScenesService::getInstance().tianxia[nn].saidian == 4 || ScenesService::getInstance().tianxia[oo].saidian == 4 || ScenesService::getInstance().tianxia[pp].saidian == 4)
					{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 4)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 4)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 4)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 4)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}
							else if(ScenesService::getInstance().tianxia[e].saidian == 4)
							{
								A=e;
								AChar = ScenesService::getInstance().tianxia[e].userid;
							}
							else if(ScenesService::getInstance().tianxia[f].saidian == 4)
							{
								A=f;
								AChar = ScenesService::getInstance().tianxia[f].userid;
							}
							else if(ScenesService::getInstance().tianxia[g].saidian == 4)
							{
								A=g;
								AChar = ScenesService::getInstance().tianxia[g].userid;
							}
							else if(ScenesService::getInstance().tianxia[h].saidian == 4)
							{
								A=h;
								AChar = ScenesService::getInstance().tianxia[h].userid;
							}
							else if(ScenesService::getInstance().tianxia[i].saidian == 4)
							{
								A=i;
								AChar = ScenesService::getInstance().tianxia[i].userid;
							}
							else if(ScenesService::getInstance().tianxia[j].saidian == 4)
							{
								A=j;
								AChar = ScenesService::getInstance().tianxia[j].userid;
							}
							else if(ScenesService::getInstance().tianxia[k].saidian == 4)
							{
								A=k;
								AChar = ScenesService::getInstance().tianxia[k].userid;
							}
							else if(ScenesService::getInstance().tianxia[l].saidian == 4)
							{
								A=l;
								AChar = ScenesService::getInstance().tianxia[l].userid;
							}
							else if(ScenesService::getInstance().tianxia[m].saidian == 4)
							{
								A=m;
								AChar = ScenesService::getInstance().tianxia[m].userid;
							}
							else if(ScenesService::getInstance().tianxia[n].saidian == 4)
							{
								A=n;
								AChar = ScenesService::getInstance().tianxia[n].userid;
							}
							else if(ScenesService::getInstance().tianxia[o].saidian == 4)
							{
								A=o;
								AChar = ScenesService::getInstance().tianxia[o].userid;
							}
							else if(ScenesService::getInstance().tianxia[p].saidian == 4)
							{
								A=p;
								AChar = ScenesService::getInstance().tianxia[p].userid;
							}
							

							if(ScenesService::getInstance().tianxia[aa].saidian == 4)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 4)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 4)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 4)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}
							else if(ScenesService::getInstance().tianxia[ee].saidian == 4)
							{
								B=ee;
								BChar = ScenesService::getInstance().tianxia[ee].userid;
							}
							else if(ScenesService::getInstance().tianxia[ff].saidian == 4)
							{
								B=ff;
								BChar = ScenesService::getInstance().tianxia[ff].userid;
							}
							else if(ScenesService::getInstance().tianxia[gg].saidian == 4)
							{
								B=gg;
								BChar = ScenesService::getInstance().tianxia[gg].userid;
							}
							else if(ScenesService::getInstance().tianxia[hh].saidian == 4)
							{
								B=hh;
								BChar = ScenesService::getInstance().tianxia[hh].userid;
							}
							else if(ScenesService::getInstance().tianxia[ii].saidian == 4)
							{
								B=ii;
								BChar = ScenesService::getInstance().tianxia[ii].userid;
							}
							else if(ScenesService::getInstance().tianxia[jj].saidian == 4)
							{
								B=jj;
								BChar = ScenesService::getInstance().tianxia[jj].userid;
							}
							else if(ScenesService::getInstance().tianxia[kk].saidian == 4)
							{
								B=kk;
								BChar = ScenesService::getInstance().tianxia[kk].userid;
							}
							else if(ScenesService::getInstance().tianxia[ll].saidian == 4)
							{
								B=ll;
								BChar = ScenesService::getInstance().tianxia[ll].userid;
							}
							else if(ScenesService::getInstance().tianxia[mm].saidian == 4)
							{
								B=mm;
								BChar = ScenesService::getInstance().tianxia[mm].userid;
							}
							else if(ScenesService::getInstance().tianxia[nn].saidian == 4)
							{
								B=nn;
								BChar = ScenesService::getInstance().tianxia[nn].userid;
							}
							else if(ScenesService::getInstance().tianxia[oo].saidian == 4)
							{
								B=oo;
								BChar = ScenesService::getInstance().tianxia[oo].userid;
							}
							else if(ScenesService::getInstance().tianxia[pp].saidian == 4)
							{
								B=pp;
								BChar = ScenesService::getInstance().tianxia[pp].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·天下第一总决赛1组 pos=48,56");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·天下第一总决赛1组 pos=69,55");  //自动传送地图坐标
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[A].saidian =5;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，恭喜您赢得天下第一总决赛冠军！");
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);				
								pUserA->addBindObjectNum(36000,1,0,0,1);	
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[B].saidian =5;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，恭喜您赢得天下第一总决赛冠军！");
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);	
								pUserB->addBindObjectNum(36000,1,0,0,1);
							}
							
						}
						else{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 4)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 4)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 4)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 4)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}
							else if(ScenesService::getInstance().tianxia[e].saidian == 4)
							{
								A=e;
								AChar = ScenesService::getInstance().tianxia[e].userid;
							}
							else if(ScenesService::getInstance().tianxia[f].saidian == 4)
							{
								A=f;
								AChar = ScenesService::getInstance().tianxia[f].userid;
							}
							else if(ScenesService::getInstance().tianxia[g].saidian == 4)
							{
								A=g;
								AChar = ScenesService::getInstance().tianxia[g].userid;
							}
							else if(ScenesService::getInstance().tianxia[h].saidian == 4)
							{
								A=h;
								AChar = ScenesService::getInstance().tianxia[h].userid;
							}
							else if(ScenesService::getInstance().tianxia[i].saidian == 4)
							{
								A=i;
								AChar = ScenesService::getInstance().tianxia[i].userid;
							}
							else if(ScenesService::getInstance().tianxia[j].saidian == 4)
							{
								A=j;
								AChar = ScenesService::getInstance().tianxia[j].userid;
							}
							else if(ScenesService::getInstance().tianxia[k].saidian == 4)
							{
								A=k;
								AChar = ScenesService::getInstance().tianxia[k].userid;
							}
							else if(ScenesService::getInstance().tianxia[l].saidian == 4)
							{
								A=l;
								AChar = ScenesService::getInstance().tianxia[l].userid;
							}
							else if(ScenesService::getInstance().tianxia[m].saidian == 4)
							{
								A=m;
								AChar = ScenesService::getInstance().tianxia[m].userid;
							}
							else if(ScenesService::getInstance().tianxia[n].saidian == 4)
							{
								A=n;
								AChar = ScenesService::getInstance().tianxia[n].userid;
							}
							else if(ScenesService::getInstance().tianxia[o].saidian == 4)
							{
								A=o;
								AChar = ScenesService::getInstance().tianxia[o].userid;
							}
							else if(ScenesService::getInstance().tianxia[p].saidian == 4)
							{
								A=p;
								AChar = ScenesService::getInstance().tianxia[p].userid;
							}
							

							if(ScenesService::getInstance().tianxia[aa].saidian == 4)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 4)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 4)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 4)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}
							else if(ScenesService::getInstance().tianxia[ee].saidian == 4)
							{
								B=ee;
								BChar = ScenesService::getInstance().tianxia[ee].userid;
							}
							else if(ScenesService::getInstance().tianxia[ff].saidian == 4)
							{
								B=ff;
								BChar = ScenesService::getInstance().tianxia[ff].userid;
							}
							else if(ScenesService::getInstance().tianxia[gg].saidian == 4)
							{
								B=gg;
								BChar = ScenesService::getInstance().tianxia[gg].userid;
							}
							else if(ScenesService::getInstance().tianxia[hh].saidian == 4)
							{
								B=hh;
								BChar = ScenesService::getInstance().tianxia[hh].userid;
							}
							else if(ScenesService::getInstance().tianxia[ii].saidian == 4)
							{
								B=ii;
								BChar = ScenesService::getInstance().tianxia[ii].userid;
							}
							else if(ScenesService::getInstance().tianxia[jj].saidian == 4)
							{
								B=jj;
								BChar = ScenesService::getInstance().tianxia[jj].userid;
							}
							else if(ScenesService::getInstance().tianxia[kk].saidian == 4)
							{
								B=kk;
								BChar = ScenesService::getInstance().tianxia[kk].userid;
							}
							else if(ScenesService::getInstance().tianxia[ll].saidian == 4)
							{
								B=ll;
								BChar = ScenesService::getInstance().tianxia[ll].userid;
							}
							else if(ScenesService::getInstance().tianxia[mm].saidian == 4)
							{
								B=mm;
								BChar = ScenesService::getInstance().tianxia[mm].userid;
							}
							else if(ScenesService::getInstance().tianxia[nn].saidian == 4)
							{
								B=nn;
								BChar = ScenesService::getInstance().tianxia[nn].userid;
							}
							else if(ScenesService::getInstance().tianxia[oo].saidian == 4)
							{
								B=oo;
								BChar = ScenesService::getInstance().tianxia[oo].userid;
							}
							else if(ScenesService::getInstance().tianxia[pp].saidian == 4)
							{
								B=pp;
								BChar = ScenesService::getInstance().tianxia[pp].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[A].saidian =5;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，恭喜您赢得天下第一总决赛冠军！");	
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
								pUserA->addBindObjectNum(36000,1,0,0,1);			
							}
						}
					}
					else
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 4 || ScenesService::getInstance().tianxia[bb].saidian == 4 || ScenesService::getInstance().tianxia[cc].saidian == 4 || ScenesService::getInstance().tianxia[dd].saidian == 4
						|| ScenesService::getInstance().tianxia[ee].saidian == 4 || ScenesService::getInstance().tianxia[ff].saidian == 4 || ScenesService::getInstance().tianxia[gg].saidian == 4 || ScenesService::getInstance().tianxia[hh].saidian == 4
						||ScenesService::getInstance().tianxia[ii].saidian == 4 || ScenesService::getInstance().tianxia[jj].saidian == 4 || ScenesService::getInstance().tianxia[kk].saidian == 4 || ScenesService::getInstance().tianxia[ll].saidian == 4
						|| ScenesService::getInstance().tianxia[mm].saidian == 4 || ScenesService::getInstance().tianxia[nn].saidian == 4 || ScenesService::getInstance().tianxia[oo].saidian == 4 || ScenesService::getInstance().tianxia[pp].saidian == 4)
					{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 4)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 4)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 4)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 4)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}
							else if(ScenesService::getInstance().tianxia[e].saidian == 4)
							{
								A=e;
								AChar = ScenesService::getInstance().tianxia[e].userid;
							}
							else if(ScenesService::getInstance().tianxia[f].saidian == 4)
							{
								A=f;
								AChar = ScenesService::getInstance().tianxia[f].userid;
							}
							else if(ScenesService::getInstance().tianxia[g].saidian == 4)
							{
								A=g;
								AChar = ScenesService::getInstance().tianxia[g].userid;
							}
							else if(ScenesService::getInstance().tianxia[h].saidian == 4)
							{
								A=h;
								AChar = ScenesService::getInstance().tianxia[h].userid;
							}
							else if(ScenesService::getInstance().tianxia[i].saidian == 4)
							{
								A=i;
								AChar = ScenesService::getInstance().tianxia[i].userid;
							}
							else if(ScenesService::getInstance().tianxia[j].saidian == 4)
							{
								A=j;
								AChar = ScenesService::getInstance().tianxia[j].userid;
							}
							else if(ScenesService::getInstance().tianxia[k].saidian == 4)
							{
								A=k;
								AChar = ScenesService::getInstance().tianxia[k].userid;
							}
							else if(ScenesService::getInstance().tianxia[l].saidian == 4)
							{
								A=l;
								AChar = ScenesService::getInstance().tianxia[l].userid;
							}
							else if(ScenesService::getInstance().tianxia[m].saidian == 4)
							{
								A=m;
								AChar = ScenesService::getInstance().tianxia[m].userid;
							}
							else if(ScenesService::getInstance().tianxia[n].saidian == 4)
							{
								A=n;
								AChar = ScenesService::getInstance().tianxia[n].userid;
							}
							else if(ScenesService::getInstance().tianxia[o].saidian == 4)
							{
								A=o;
								AChar = ScenesService::getInstance().tianxia[o].userid;
							}
							else if(ScenesService::getInstance().tianxia[p].saidian == 4)
							{
								A=p;
								AChar = ScenesService::getInstance().tianxia[p].userid;
							}
							

							if(ScenesService::getInstance().tianxia[aa].saidian == 4)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 4)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 4)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 4)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}
							else if(ScenesService::getInstance().tianxia[ee].saidian == 4)
							{
								B=ee;
								BChar = ScenesService::getInstance().tianxia[ee].userid;
							}
							else if(ScenesService::getInstance().tianxia[ff].saidian == 4)
							{
								B=ff;
								BChar = ScenesService::getInstance().tianxia[ff].userid;
							}
							else if(ScenesService::getInstance().tianxia[gg].saidian == 4)
							{
								B=gg;
								BChar = ScenesService::getInstance().tianxia[gg].userid;
							}
							else if(ScenesService::getInstance().tianxia[hh].saidian == 4)
							{
								B=hh;
								BChar = ScenesService::getInstance().tianxia[hh].userid;
							}
							else if(ScenesService::getInstance().tianxia[ii].saidian == 4)
							{
								B=ii;
								BChar = ScenesService::getInstance().tianxia[ii].userid;
							}
							else if(ScenesService::getInstance().tianxia[jj].saidian == 4)
							{
								B=jj;
								BChar = ScenesService::getInstance().tianxia[jj].userid;
							}
							else if(ScenesService::getInstance().tianxia[kk].saidian == 4)
							{
								B=kk;
								BChar = ScenesService::getInstance().tianxia[kk].userid;
							}
							else if(ScenesService::getInstance().tianxia[ll].saidian == 4)
							{
								B=ll;
								BChar = ScenesService::getInstance().tianxia[ll].userid;
							}
							else if(ScenesService::getInstance().tianxia[mm].saidian == 4)
							{
								B=mm;
								BChar = ScenesService::getInstance().tianxia[mm].userid;
							}
							else if(ScenesService::getInstance().tianxia[nn].saidian == 4)
							{
								B=nn;
								BChar = ScenesService::getInstance().tianxia[nn].userid;
							}
							else if(ScenesService::getInstance().tianxia[oo].saidian == 4)
							{
								B=oo;
								BChar = ScenesService::getInstance().tianxia[oo].userid;
							}
							else if(ScenesService::getInstance().tianxia[pp].saidian == 4)
							{
								B=pp;
								BChar = ScenesService::getInstance().tianxia[pp].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(BChar)
							{
								ScenesService::getInstance().tianxia[B].saidian =5;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，恭喜您赢得天下第一总决赛冠军！");
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
								pUserB->addBindObjectNum(36000,1,0,0,1);	
							}
						}
					}
					
				}
			}
			if (tv1.tm_hour == 22) // 天下第一大赛总决赛结算！
			{
				if (tv1.tm_min == 44)
				{
					//分组处理
					int a = 0;
					int b = 0;
					int c = 0;
					int d = 0;
					int e=0;
					int f=0;
					int g=0;
					int h=0;
					int i = 0;
					int j = 0;
					int k = 0;
					int l = 0;
					int m=0;
					int n=0;
					int o=0;
					int p=0;

					int aa = 0;
					int bb = 0;
					int cc = 0;
					int dd = 0;
					int ee=0;
					int ff=0;
					int gg=0;
					int hh=0;
					int ii = 0;
					int jj = 0;
					int kk = 0;
					int ll = 0;
					int mm=0;
					int nn=0;
					int oo=0;
					int pp=0;
					a=0;
					b=1;
					c=4;
					d=5;
					e=8;
					f=9;
					g=12;
					h=13;
					i=16;
					j=17;
					k=20;
					l=21;
					m=24;
					n=25;
					o=28;
					p=29;
					
					aa=2;
					bb=3;
					cc=6;
					dd=7;
					ee=10;
					ff=11;
					gg=14;
					hh=15;
					ii=18;
					jj=19;
					kk=22;
					ll=23;
					mm=26;
					nn=27;
					oo=30;
					pp=31;
					//第一组
					//第一组
					if(ScenesService::getInstance().tianxia[a].saidian == 4 || ScenesService::getInstance().tianxia[b].saidian == 4 || ScenesService::getInstance().tianxia[c].saidian == 4 || ScenesService::getInstance().tianxia[d].saidian == 4
					|| ScenesService::getInstance().tianxia[e].saidian == 4 || ScenesService::getInstance().tianxia[f].saidian == 4 || ScenesService::getInstance().tianxia[g].saidian == 4 || ScenesService::getInstance().tianxia[h].saidian == 4
					||ScenesService::getInstance().tianxia[o].saidian == 4 || ScenesService::getInstance().tianxia[i].saidian == 4 || ScenesService::getInstance().tianxia[j].saidian == 4 || ScenesService::getInstance().tianxia[k].saidian == 4
					|| ScenesService::getInstance().tianxia[p].saidian == 4 || ScenesService::getInstance().tianxia[l].saidian == 4 || ScenesService::getInstance().tianxia[m].saidian == 4 || ScenesService::getInstance().tianxia[n].saidian == 4)
					{
						if(ScenesService::getInstance().tianxia[aa].saidian == 4 || ScenesService::getInstance().tianxia[bb].saidian == 4 || ScenesService::getInstance().tianxia[cc].saidian == 4 || ScenesService::getInstance().tianxia[dd].saidian == 4
						|| ScenesService::getInstance().tianxia[ee].saidian == 4 || ScenesService::getInstance().tianxia[ff].saidian == 4 || ScenesService::getInstance().tianxia[gg].saidian == 4 || ScenesService::getInstance().tianxia[hh].saidian == 4
						||ScenesService::getInstance().tianxia[ii].saidian == 4 || ScenesService::getInstance().tianxia[jj].saidian == 4 || ScenesService::getInstance().tianxia[kk].saidian == 4 || ScenesService::getInstance().tianxia[ll].saidian == 4
						|| ScenesService::getInstance().tianxia[mm].saidian == 4 || ScenesService::getInstance().tianxia[nn].saidian == 4 || ScenesService::getInstance().tianxia[oo].saidian == 4 || ScenesService::getInstance().tianxia[pp].saidian == 4)
					{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 4)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 4)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 4)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 4)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}
							else if(ScenesService::getInstance().tianxia[e].saidian == 4)
							{
								A=e;
								AChar = ScenesService::getInstance().tianxia[e].userid;
							}
							else if(ScenesService::getInstance().tianxia[f].saidian == 4)
							{
								A=f;
								AChar = ScenesService::getInstance().tianxia[f].userid;
							}
							else if(ScenesService::getInstance().tianxia[g].saidian == 4)
							{
								A=g;
								AChar = ScenesService::getInstance().tianxia[g].userid;
							}
							else if(ScenesService::getInstance().tianxia[h].saidian == 4)
							{
								A=h;
								AChar = ScenesService::getInstance().tianxia[h].userid;
							}
							else if(ScenesService::getInstance().tianxia[i].saidian == 4)
							{
								A=i;
								AChar = ScenesService::getInstance().tianxia[i].userid;
							}
							else if(ScenesService::getInstance().tianxia[j].saidian == 4)
							{
								A=j;
								AChar = ScenesService::getInstance().tianxia[j].userid;
							}
							else if(ScenesService::getInstance().tianxia[k].saidian == 4)
							{
								A=k;
								AChar = ScenesService::getInstance().tianxia[k].userid;
							}
							else if(ScenesService::getInstance().tianxia[l].saidian == 4)
							{
								A=l;
								AChar = ScenesService::getInstance().tianxia[l].userid;
							}
							else if(ScenesService::getInstance().tianxia[m].saidian == 4)
							{
								A=m;
								AChar = ScenesService::getInstance().tianxia[m].userid;
							}
							else if(ScenesService::getInstance().tianxia[n].saidian == 4)
							{
								A=n;
								AChar = ScenesService::getInstance().tianxia[n].userid;
							}
							else if(ScenesService::getInstance().tianxia[o].saidian == 4)
							{
								A=o;
								AChar = ScenesService::getInstance().tianxia[o].userid;
							}
							else if(ScenesService::getInstance().tianxia[p].saidian == 4)
							{
								A=p;
								AChar = ScenesService::getInstance().tianxia[p].userid;
							}
							

							if(ScenesService::getInstance().tianxia[aa].saidian == 4)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 4)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 4)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 4)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}
							else if(ScenesService::getInstance().tianxia[ee].saidian == 4)
							{
								B=ee;
								BChar = ScenesService::getInstance().tianxia[ee].userid;
							}
							else if(ScenesService::getInstance().tianxia[ff].saidian == 4)
							{
								B=ff;
								BChar = ScenesService::getInstance().tianxia[ff].userid;
							}
							else if(ScenesService::getInstance().tianxia[gg].saidian == 4)
							{
								B=gg;
								BChar = ScenesService::getInstance().tianxia[gg].userid;
							}
							else if(ScenesService::getInstance().tianxia[hh].saidian == 4)
							{
								B=hh;
								BChar = ScenesService::getInstance().tianxia[hh].userid;
							}
							else if(ScenesService::getInstance().tianxia[ii].saidian == 4)
							{
								B=ii;
								BChar = ScenesService::getInstance().tianxia[ii].userid;
							}
							else if(ScenesService::getInstance().tianxia[jj].saidian == 4)
							{
								B=jj;
								BChar = ScenesService::getInstance().tianxia[jj].userid;
							}
							else if(ScenesService::getInstance().tianxia[kk].saidian == 4)
							{
								B=kk;
								BChar = ScenesService::getInstance().tianxia[kk].userid;
							}
							else if(ScenesService::getInstance().tianxia[ll].saidian == 4)
							{
								B=ll;
								BChar = ScenesService::getInstance().tianxia[ll].userid;
							}
							else if(ScenesService::getInstance().tianxia[mm].saidian == 4)
							{
								B=mm;
								BChar = ScenesService::getInstance().tianxia[mm].userid;
							}
							else if(ScenesService::getInstance().tianxia[nn].saidian == 4)
							{
								B=nn;
								BChar = ScenesService::getInstance().tianxia[nn].userid;
							}
							else if(ScenesService::getInstance().tianxia[oo].saidian == 4)
							{
								B=oo;
								BChar = ScenesService::getInstance().tianxia[oo].userid;
							}
							else if(ScenesService::getInstance().tianxia[pp].saidian == 4)
							{
								B=pp;
								BChar = ScenesService::getInstance().tianxia[pp].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA && pUserB)	// 如果两个玩家都在线
							{
								Gm::gomap(pUserA, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								Gm::gomap(pUserB, "name=中立区·皇城 pos=832,699");  //自动传送地图坐标
								if (ScenesService::getInstance().tianxia[A].jifen > ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[A].saidian = 5;								
									Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
									Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
									Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！");
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，赢得天下第一总决赛亚军！！");
									pUserA->addBindObjectNum(36000,1,0,0,1);
									pUserB->addBindObjectNum(36001,1,0,0,1);
									
								}
								else if (ScenesService::getInstance().tianxia[A].jifen < ScenesService::getInstance().tianxia[B].jifen)
								{
									ScenesService::getInstance().tianxia[B].saidian = 5;
									Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
									Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手晋级下一轮！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
									Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！");
									Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，赢得天下第一总决赛亚军！！");
									pUserB->addBindObjectNum(36000,1,0,0,1);
									pUserA->addBindObjectNum(36001,1,0,0,1);
								}
								else
								{
									if (pUserA->charbase.zhanli > pUserB->charbase.zhanli)
									{
										ScenesService::getInstance().tianxia[A].saidian = 5;
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！");
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，赢得天下第一总决赛亚军！！");
										pUserA->addBindObjectNum(36000,1,0,0,1);
										pUserB->addBindObjectNum(36001,1,0,0,1);
									}
									else
									{
										ScenesService::getInstance().tianxia[B].saidian = 5;
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
										Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
										Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！!");
										Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】您被对手击败，已淘汰，赢得天下第一总决赛亚军！！");
										pUserB->addBindObjectNum(36000,1,0,0,1);
										pUserA->addBindObjectNum(36001,1,0,0,1);
									}
								}
							}
							else if(pUserA && !pUserB)	// 如果玩家A在线
							{
								ScenesService::getInstance().tianxia[A].saidian = 5;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，恭喜您赢得天下第一总决赛冠军!");
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
								pUserA->addBindObjectNum(36000,1,0,0,1);				
											
							}
							else if(!pUserA && pUserB)	// 如果玩家B在线
							{
								ScenesService::getInstance().tianxia[B].saidian = 5;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，恭喜您赢得天下第一总决赛冠军!");	
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
								pUserB->addBindObjectNum(36000,1,0,0,1);	
							}
							
						}
						else{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 4)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 4)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 4)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 4)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}
							else if(ScenesService::getInstance().tianxia[e].saidian == 4)
							{
								A=e;
								AChar = ScenesService::getInstance().tianxia[e].userid;
							}
							else if(ScenesService::getInstance().tianxia[f].saidian == 4)
							{
								A=f;
								AChar = ScenesService::getInstance().tianxia[f].userid;
							}
							else if(ScenesService::getInstance().tianxia[g].saidian == 4)
							{
								A=g;
								AChar = ScenesService::getInstance().tianxia[g].userid;
							}
							else if(ScenesService::getInstance().tianxia[h].saidian == 4)
							{
								A=h;
								AChar = ScenesService::getInstance().tianxia[h].userid;
							}
							else if(ScenesService::getInstance().tianxia[i].saidian == 4)
							{
								A=i;
								AChar = ScenesService::getInstance().tianxia[i].userid;
							}
							else if(ScenesService::getInstance().tianxia[j].saidian == 4)
							{
								A=j;
								AChar = ScenesService::getInstance().tianxia[j].userid;
							}
							else if(ScenesService::getInstance().tianxia[k].saidian == 4)
							{
								A=k;
								AChar = ScenesService::getInstance().tianxia[k].userid;
							}
							else if(ScenesService::getInstance().tianxia[l].saidian == 4)
							{
								A=l;
								AChar = ScenesService::getInstance().tianxia[l].userid;
							}
							else if(ScenesService::getInstance().tianxia[m].saidian == 4)
							{
								A=m;
								AChar = ScenesService::getInstance().tianxia[m].userid;
							}
							else if(ScenesService::getInstance().tianxia[n].saidian == 4)
							{
								A=n;
								AChar = ScenesService::getInstance().tianxia[n].userid;
							}
							else if(ScenesService::getInstance().tianxia[o].saidian == 4)
							{
								A=o;
								AChar = ScenesService::getInstance().tianxia[o].userid;
							}
							else if(ScenesService::getInstance().tianxia[p].saidian == 4)
							{
								A=p;
								AChar = ScenesService::getInstance().tianxia[p].userid;
							}
							

							if(ScenesService::getInstance().tianxia[aa].saidian == 4)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 4)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 4)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 4)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}
							else if(ScenesService::getInstance().tianxia[ee].saidian == 4)
							{
								B=ee;
								BChar = ScenesService::getInstance().tianxia[ee].userid;
							}
							else if(ScenesService::getInstance().tianxia[ff].saidian == 4)
							{
								B=ff;
								BChar = ScenesService::getInstance().tianxia[ff].userid;
							}
							else if(ScenesService::getInstance().tianxia[gg].saidian == 4)
							{
								B=gg;
								BChar = ScenesService::getInstance().tianxia[gg].userid;
							}
							else if(ScenesService::getInstance().tianxia[hh].saidian == 4)
							{
								B=hh;
								BChar = ScenesService::getInstance().tianxia[hh].userid;
							}
							else if(ScenesService::getInstance().tianxia[ii].saidian == 4)
							{
								B=ii;
								BChar = ScenesService::getInstance().tianxia[ii].userid;
							}
							else if(ScenesService::getInstance().tianxia[jj].saidian == 4)
							{
								B=jj;
								BChar = ScenesService::getInstance().tianxia[jj].userid;
							}
							else if(ScenesService::getInstance().tianxia[kk].saidian == 4)
							{
								B=kk;
								BChar = ScenesService::getInstance().tianxia[kk].userid;
							}
							else if(ScenesService::getInstance().tianxia[ll].saidian == 4)
							{
								B=ll;
								BChar = ScenesService::getInstance().tianxia[ll].userid;
							}
							else if(ScenesService::getInstance().tianxia[mm].saidian == 4)
							{
								B=mm;
								BChar = ScenesService::getInstance().tianxia[mm].userid;
							}
							else if(ScenesService::getInstance().tianxia[nn].saidian == 4)
							{
								B=nn;
								BChar = ScenesService::getInstance().tianxia[nn].userid;
							}
							else if(ScenesService::getInstance().tianxia[oo].saidian == 4)
							{
								B=oo;
								BChar = ScenesService::getInstance().tianxia[oo].userid;
							}
							else if(ScenesService::getInstance().tianxia[pp].saidian == 4)
							{
								B=pp;
								BChar = ScenesService::getInstance().tianxia[pp].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(pUserA)
							{
								ScenesService::getInstance().tianxia[A].saidian=4;
								Channel::sendSys(pUserA, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，恭喜您赢得天下第一总决赛冠军!");	
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserA->charbase.country), pUserA->charbase.name);
								pUserA->addBindObjectNum(36000,1,0,0,1);		
							}	
						}
					}
					else{
						if(ScenesService::getInstance().tianxia[aa].saidian == 4 || ScenesService::getInstance().tianxia[bb].saidian == 4 || ScenesService::getInstance().tianxia[cc].saidian == 4 || ScenesService::getInstance().tianxia[dd].saidian == 4
						|| ScenesService::getInstance().tianxia[ee].saidian == 4 || ScenesService::getInstance().tianxia[ff].saidian == 4 || ScenesService::getInstance().tianxia[gg].saidian == 4 || ScenesService::getInstance().tianxia[hh].saidian == 4
						||ScenesService::getInstance().tianxia[ii].saidian == 4 || ScenesService::getInstance().tianxia[jj].saidian == 4 || ScenesService::getInstance().tianxia[kk].saidian == 4 || ScenesService::getInstance().tianxia[ll].saidian == 4
						|| ScenesService::getInstance().tianxia[mm].saidian == 4 || ScenesService::getInstance().tianxia[nn].saidian == 4 || ScenesService::getInstance().tianxia[oo].saidian == 4 || ScenesService::getInstance().tianxia[pp].saidian == 4)
					{
							int A=0;
							int B=0;
							int AChar = 0;
							int BChar = 0;
							//匹配成功
							if(ScenesService::getInstance().tianxia[a].saidian == 4)
							{
								A=a;
								AChar = ScenesService::getInstance().tianxia[a].userid;
							}
							else if(ScenesService::getInstance().tianxia[b].saidian == 4)
							{
								A=b;
								AChar = ScenesService::getInstance().tianxia[b].userid;
							}
							else if(ScenesService::getInstance().tianxia[c].saidian == 4)
							{
								A=c;
								AChar = ScenesService::getInstance().tianxia[c].userid;
							}
							else if(ScenesService::getInstance().tianxia[d].saidian == 4)
							{
								A=d;
								AChar = ScenesService::getInstance().tianxia[d].userid;
							}
							else if(ScenesService::getInstance().tianxia[e].saidian == 4)
							{
								A=e;
								AChar = ScenesService::getInstance().tianxia[e].userid;
							}
							else if(ScenesService::getInstance().tianxia[f].saidian == 4)
							{
								A=f;
								AChar = ScenesService::getInstance().tianxia[f].userid;
							}
							else if(ScenesService::getInstance().tianxia[g].saidian == 4)
							{
								A=g;
								AChar = ScenesService::getInstance().tianxia[g].userid;
							}
							else if(ScenesService::getInstance().tianxia[h].saidian == 4)
							{
								A=h;
								AChar = ScenesService::getInstance().tianxia[h].userid;
							}
							else if(ScenesService::getInstance().tianxia[i].saidian == 4)
							{
								A=i;
								AChar = ScenesService::getInstance().tianxia[i].userid;
							}
							else if(ScenesService::getInstance().tianxia[j].saidian == 4)
							{
								A=j;
								AChar = ScenesService::getInstance().tianxia[j].userid;
							}
							else if(ScenesService::getInstance().tianxia[k].saidian == 4)
							{
								A=k;
								AChar = ScenesService::getInstance().tianxia[k].userid;
							}
							else if(ScenesService::getInstance().tianxia[l].saidian == 4)
							{
								A=l;
								AChar = ScenesService::getInstance().tianxia[l].userid;
							}
							else if(ScenesService::getInstance().tianxia[m].saidian == 4)
							{
								A=m;
								AChar = ScenesService::getInstance().tianxia[m].userid;
							}
							else if(ScenesService::getInstance().tianxia[n].saidian == 4)
							{
								A=n;
								AChar = ScenesService::getInstance().tianxia[n].userid;
							}
							else if(ScenesService::getInstance().tianxia[o].saidian == 4)
							{
								A=o;
								AChar = ScenesService::getInstance().tianxia[o].userid;
							}
							else if(ScenesService::getInstance().tianxia[p].saidian == 4)
							{
								A=p;
								AChar = ScenesService::getInstance().tianxia[p].userid;
							}
							

							if(ScenesService::getInstance().tianxia[aa].saidian == 4)
							{
								B=aa;
								BChar = ScenesService::getInstance().tianxia[aa].userid;
							}
							else if(ScenesService::getInstance().tianxia[bb].saidian == 4)
							{
								B=bb;
								BChar = ScenesService::getInstance().tianxia[bb].userid;
							}
							else if(ScenesService::getInstance().tianxia[cc].saidian == 4)
							{
								B=cc;
								BChar = ScenesService::getInstance().tianxia[cc].userid;
							}
							else if(ScenesService::getInstance().tianxia[dd].saidian == 4)
							{
								B=dd;
								BChar = ScenesService::getInstance().tianxia[dd].userid;
							}
							else if(ScenesService::getInstance().tianxia[ee].saidian == 4)
							{
								B=ee;
								BChar = ScenesService::getInstance().tianxia[ee].userid;
							}
							else if(ScenesService::getInstance().tianxia[ff].saidian == 4)
							{
								B=ff;
								BChar = ScenesService::getInstance().tianxia[ff].userid;
							}
							else if(ScenesService::getInstance().tianxia[gg].saidian == 4)
							{
								B=gg;
								BChar = ScenesService::getInstance().tianxia[gg].userid;
							}
							else if(ScenesService::getInstance().tianxia[hh].saidian == 4)
							{
								B=hh;
								BChar = ScenesService::getInstance().tianxia[hh].userid;
							}
							else if(ScenesService::getInstance().tianxia[ii].saidian == 4)
							{
								B=ii;
								BChar = ScenesService::getInstance().tianxia[ii].userid;
							}
							else if(ScenesService::getInstance().tianxia[jj].saidian == 4)
							{
								B=jj;
								BChar = ScenesService::getInstance().tianxia[jj].userid;
							}
							else if(ScenesService::getInstance().tianxia[kk].saidian == 4)
							{
								B=kk;
								BChar = ScenesService::getInstance().tianxia[kk].userid;
							}
							else if(ScenesService::getInstance().tianxia[ll].saidian == 4)
							{
								B=ll;
								BChar = ScenesService::getInstance().tianxia[ll].userid;
							}
							else if(ScenesService::getInstance().tianxia[mm].saidian == 4)
							{
								B=mm;
								BChar = ScenesService::getInstance().tianxia[mm].userid;
							}
							else if(ScenesService::getInstance().tianxia[nn].saidian == 4)
							{
								B=nn;
								BChar = ScenesService::getInstance().tianxia[nn].userid;
							}
							else if(ScenesService::getInstance().tianxia[oo].saidian == 4)
							{
								B=oo;
								BChar = ScenesService::getInstance().tianxia[oo].userid;
							}
							else if(ScenesService::getInstance().tianxia[pp].saidian == 4)
							{
								B=pp;
								BChar = ScenesService::getInstance().tianxia[pp].userid;
							}

							SceneUser *pUserA = SceneUserManager::getMe().getUserByID(AChar); // 玩家A
							SceneUser *pUserB = SceneUserManager::getMe().getUserByID(BChar); // 玩家B
							if(BChar)
							{
								ScenesService::getInstance().tianxia[B].saidian=4;
								Channel::sendSys(pUserB, Cmd::INFO_TYPE_EXP5, "【天下第一赛事】你的对手不在线，恭喜您赢得天下第一总决赛冠军!");	
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
								Channel::sendAllInfo(Cmd::INFO_TYPE_ZISEYROLL, "哇！%s的%s在天下第一赛事中晋级战胜对手赢得天下第一总决赛冠军！！", SceneManager::getInstance().getCountryNameByCountryID(pUserB->charbase.country), pUserB->charbase.name);
								pUserB->addBindObjectNum(36000,1,0,0,1);	
							}
						}
					}

				}
			}
			if (Zebra::global["world_quiz"] == "true")
			{
				struct tm tv1;
				time_t timValue = time(NULL);
				zRTime::getLocalTime(tv1, timValue);

				if (tv1.tm_hour==19)
				{
					if (tv1.tm_min==5 || tv1.tm_min == 10 || tv1.tm_min==15)
					{
						for (SceneManager::CountryMap_iter iter=SceneManager::getInstance().
								country_info.begin(); iter!=SceneManager::getInstance().country_info.end(); 
								iter++)
						{       
							Cmd::Session::t_countryNotify_SceneSession send;
							bzero(send.info, sizeof(send.info));
							sprintf(send.info, "%d 分钟后举办智力竞赛", abs(20-tv1.tm_min));
							send.dwCountryID = iter->second.id;
							sessionClient->sendCmd(&send, sizeof(send));	
						}
					}

					if (!quiz && tv1.tm_min>=20 && tv1.tm_min<25)
					{
						quiz = true;
						Cmd::Session::t_createQuiz_SceneSession send;

						send.active_time  = 30;
						send.ready_time   = 1;
						send.type = 0; 
						send.subject_type = 0;
						sessionClient->sendCmd(&send, sizeof(send));	
					}
				}

				if (tv1.tm_hour==12)
				{
					if (tv1.tm_min==5 || tv1.tm_min==10 || tv1.tm_min == 15)
					{
						for (SceneManager::CountryMap_iter iter=SceneManager::getInstance().
								country_info.begin(); iter!=SceneManager::getInstance().country_info.end(); 
								iter++)
						{       
							Cmd::Session::t_countryNotify_SceneSession send;
							bzero(send.info, sizeof(send.info));
							sprintf(send.info, "%d 分钟后举办智力竞赛", abs(20-tv1.tm_min));
							send.dwCountryID = iter->second.id;
							sessionClient->sendCmd(&send, sizeof(send));	
						}
					}

					if (!quiz && tv1.tm_min>=20 && tv1.tm_min<25)
					{
						quiz = true;
						Cmd::Session::t_createQuiz_SceneSession send;

						send.active_time  = 30;
						send.ready_time   = 1;
						send.type = 0; 
						send.subject_type = 0;
						sessionClient->sendCmd(&send, sizeof(send));	
					}
				}

#if 0
				if (tv1.tm_hour==22)
				{
					if (tv1.tm_min==35 || tv1.tm_min==40 || tv1.tm_min == 45)
					{
						for (SceneManager::CountryMap_iter iter=SceneManager::getInstance().
								country_info.begin(); iter!=SceneManager::getInstance().country_info.end(); 
								iter++)
						{       
							Cmd::Session::t_countryNotify_SceneSession send;
							bzero(send.info, sizeof(send.info));
							sprintf(send.info, "%d 分钟后举办智力竞赛", abs(50-tv1.tm_min));
							send.dwCountryID = iter->second.id;
							sessionClient->sendCmd(&send, sizeof(send));	
						}
					}

					if (!quiz && tv1.tm_min>=50 && tv1.tm_min<55)
					{
						quiz = true;
						Cmd::Session::t_createQuiz_SceneSession send;

						send.active_time  = 30;
						send.ready_time   = 1;
						send.type = 0; 
						send.subject_type = 0;
						sessionClient->sendCmd(&send, sizeof(send));	
					}
				}
#endif

				if (tv1.tm_hour==13 || tv1.tm_hour==20 || tv1.tm_hour==0)
				{
					quiz = false;
				}
			}

			//刷新所有全局变量
			if (GlobalVar::server_id()) { //ugly, TO BE FIXED

				ALLVARS(update);
				ALLVARS(save);
			}

			SceneManager::getInstance().checkUnloadOneScene();
		}

		step = (++step) % MAX_NPC_GROUP;

		zRTime e;
		t = currentTime.elapse(e);
		if (t > timeout_value)
		{
			Zebra::logger->debug("---------- 1次循环用时 %u 毫秒----------", t);
		}
#ifdef __MY_FUNCTIONTIME_WRAPPER__
		My_FunctionTime_wrapper::my_func.reset(currentTime, t > timeout_value);
#endif
	}
}

#ifdef __MY_FUNCTIONTIME_WRAPPER__
My_FunctionTime My_FunctionTime_wrapper::my_func(600);

My_FunctionTime_wrapper::My_FunctionTime_wrapper(const std::string &func) : _func(func)
{
	gettimeofday(&_tv_1, NULL);
}

My_FunctionTime_wrapper::~My_FunctionTime_wrapper()
{
	struct timeval _tv_2;
	gettimeofday(&_tv_2, NULL);
	my_func.inc(_func, 1000000L * (_tv_2.tv_sec - _tv_1.tv_sec) + _tv_2.tv_usec - _tv_1.tv_usec);
}
#endif

