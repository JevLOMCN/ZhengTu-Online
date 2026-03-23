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

#include "ShuanQ.h"

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

		//250 usec
		EverySceneEntryAction esea(step);
		//对所有地图调用回调函数
		SceneManager::getInstance().execEveryScene(esea);

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

