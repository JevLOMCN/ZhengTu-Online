/**
 * \file
 * \version  $Id: TimeTick.h  $
 * \author  
 * \date
 * \brief 
 *
 * 
 */
#ifndef _TimeTick_h_
#define _TimeTick_h_

#include <iostream>
#include <string>

#include "Zebra.h"
#include "zThread.h"



class ZebraClientTimeTick : public zThread
{

	public:

		~ZebraClientTimeTick() {};

		static ZebraClientTimeTick &getInstance()
		{
			if (NULL == instance)
				instance = new ZebraClientTimeTick();

			return *instance;
		}

		/**
		 * 		 * \brief 释放类的唯一实例
		 * 		 		 *
		 * 		 		 		 */
		static void delInstance()
		{
			if (instance)
			{
				delete instance;
				instance = NULL;
			}
		}

		void run();

	private:

		static ZebraClientTimeTick *instance;

		ZebraClientTimeTick() : zThread("TimeTick") {};

};

#endif



