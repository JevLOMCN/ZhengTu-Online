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
#include <sys/timeb.h>

#include "Zebra.h"
#include "zThread.h"
#include "TimeTick.h"
#include "FLServer.h"
#include "Zebra.h"
#include "DBAccessClient.h"
#include "DBAccessClientManager.h"
#include "InfoClient.h"
#include "InfoClientManager.h"
#include "LoginManager.h"

FLTimeTick *FLTimeTick::instance = NULL;

struct LoginTimeout : public LoginManager::LoginTaskCallback
{	
	const zTime &ct;
	LoginTimeout(const zTime &ct) : ct(ct) {}
	void exec(LoginTask *lt)
	{
		if (lt->timeout(ct))
			lt->Terminate();
	}
};

void FLTimeTick::run()
{
	Zebra::logger->trace("FLTimeTick::run()");
	while(!isFinal())
	{
		zThread::sleep(1);

		zTime ct;
		//DBAccessClientManager::getInstance().timeAction(ct);
		InfoClientManager::getInstance().timeAction(ct);
		if (ct.sec() % 10 == 0)
		{
			LoginTimeout cb(ct);
			LoginManager::getInstance().execAll(cb);
		}
	}
}

