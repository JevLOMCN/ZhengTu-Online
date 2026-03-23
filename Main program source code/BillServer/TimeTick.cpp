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
#include "BillUserManager.h"
#include "BillServer.h"
#include "BillCallback.h"
#include "BillUserManager.h"
#include "StockConsign.h"
#include "BillTaskManager.h"
#include "BillClientManager.h"

BillTimeTick *BillTimeTick::instance = NULL;
zRTime BillTimeTick::currentTime;
Timer BillTimeTick::_one_min(60); 
Timer BillTimeTick::_one_sec(1); 
void BillTimeTick::run()
{
	while(!isFinal())
	{
		zThread::msleep(50);
		currentTime.now();
		if(_one_sec(currentTime))
		{
			::Bill_timeAction();

			BillUserManager::getInstance()->update();
			if(_one_min(currentTime))
			{
				ConsignHistoryManager::getInstance()->update();
			}
		}
		BillTaskManager::getInstance().execEvery();
		BillClientManager::getInstance().execEvery();
	}
}

