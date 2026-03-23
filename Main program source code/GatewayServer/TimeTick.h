/**
 * \file
 * \version  $Id: TimeTick.h $
 * \author  
 * \date 
 * \brief 时间回调函数
 *
 * 
 */


#ifndef _TimeTick_h_
#define _TimeTick_h_

#include <iostream>
#include <string>

#include "Zebra.h"
#include "zThread.h"
#include "zTime.h"

/**
 * \brief 网关定时器线程
 *
 */
class GatewayTimeTick : public zThread
{

	public:

		static zRTime currentTime;

		~GatewayTimeTick() {};

		/**
		 * \brief 得到唯一实例
		 *
		 *
		 * \return 唯一实例
		 */
		static GatewayTimeTick &getInstance()
		{
			if (NULL == instance)
				instance = new GatewayTimeTick();

			return *instance;
		}

		/**
		 * \brief 释放类的唯一实例
		 *
		 */
		static void delInstance()
		{
			SAFE_DELETE(instance);
		}

		void run();

	private:

		static GatewayTimeTick *instance;

		GatewayTimeTick() : zThread("TimeTick"), one_second(1) {};

		Timer one_second;

};

#endif

