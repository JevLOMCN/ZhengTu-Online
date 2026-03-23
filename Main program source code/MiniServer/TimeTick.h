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

#include "Zebra.h"
#include "zTime.h"
#include "zThread.h"

class MiniTimeTick : public zThread
{

	public:

		~MiniTimeTick() {};

		/// 当前时间
		static zRTime currentTime;
		static Timer _1_min;
		static MiniTimeTick &getInstance()
		{
			if (NULL == instance)
				instance = new MiniTimeTick();

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

		static MiniTimeTick *instance;

		MiniTimeTick() : zThread("TimeTick") {};

};

#endif

