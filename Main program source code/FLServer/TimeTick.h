/**
 * \file
 * \version  $Id: TimeTick.h  $
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
#include <sys/timeb.h>

#include "Zebra.h"
#include "zThread.h"
#include "zTime.h"

class FLTimeTick : public zThread
{

	public:

		~FLTimeTick() {};

		static FLTimeTick &getInstance()
		{
			if (NULL == instance)
				instance = new FLTimeTick();

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

		static FLTimeTick *instance;

		FLTimeTick() : zThread("TimeTick")
		{
		}

};

#endif

