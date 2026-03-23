/**
 * \file
 * \version  $Id: TimeTick.cpp $
 * \author  
 * \date
 * \brief 
 *
 * 
 */

#include <iostream>
#include <string>

#include "Zebra.h"
#include "zThread.h"
#include "TimeTick.h"
#include "zebraclient.h"
#include "zebraclientService.h"
#include "zebraclientManager.h"

ZebraClientTimeTick *ZebraClientTimeTick::instance = NULL;

void ZebraClientTimeTick::run()
{
	while(!isFinal())
	{
		zThread::msleep(50);
		ZebraClientManager::getInstance()->timeAction();

	}
}

