/**
 * \file
 * \version  $Id: TimeTick.cpp $
 * \author  
 * \date 
 * \brief 时间回调函数
 *
 * 
 */


#include <iostream>
#include <string>

#include "Zebra.h"
#include "zThread.h"
#include "zTime.h"
#include "TimeTick.h"
#include "LoginSessionManager.h"
#include "GatewayServer.h"
#include "GatewayTaskManager.h"

zRTime GatewayTimeTick::currentTime;
GatewayTimeTick *GatewayTimeTick::instance = NULL;

struct GatewayTaskCheckTime : public GatewayTaskManager::GatewayTaskCallback
{
	bool exec(GatewayTask *gt)
	{
		return gt->checkTime(GatewayTimeTick::currentTime);
	}
};

/**
 * \brief 线程主函数
 *
 */
void GatewayTimeTick::run()
{
    int nSeconds;

    nSeconds = 0;
	while(!isFinal())
	{
		zThread::sleep(1);

		//获取当前时间
		currentTime.now();

		if (one_second(currentTime) ) 
		{
			LoginSessionManager::getInstance().update(currentTime);

			GatewayTaskCheckTime gtct;
			GatewayTaskManager::getInstance().execAll(gtct);

        if (nSeconds++ > 400)
        {
        //300 seconds
        nSeconds = 0;
		//soke 网关每400秒统计一次在线人数
		Zebra::logger->trace("----------当前在线人数:%d ----------",GatewayService::getInstance().getPoolSize());
		}
	}
  }
}

