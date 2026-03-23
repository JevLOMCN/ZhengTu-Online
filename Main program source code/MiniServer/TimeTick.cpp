/**
 * \file
 * \version  $Id: TimeTick.cpp  $
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
#include "TimeTick.h"
#include "MiniServer.h"
#include "MiniHall.h"
#include "MiniUserManager.h"
#include "MiniTaskManager.h"

MiniTimeTick *MiniTimeTick::instance = NULL;
zRTime MiniTimeTick::currentTime;
Timer MiniTimeTick::_1_min(600); 
void MiniTimeTick::run()
{
	while(!isFinal())
	{
		zThread::msleep(50);
		currentTime.now();

		if(_1_min(currentTime))
			MiniUserManager::getMe().update();

		MiniTaskManager::getInstance().execEvery();

		MiniHall::getMe().timer();
	}
}

