/**
 * \file
 * \version  $Id: zebraclientService.h  $
 * \author  
 * \date 
 * \brief 机器人客户端主程序
 *
 * 
 */
#ifndef _ZEBRACLIENTSERVICE_H_
#define _ZEBRACLIENTSERVICE_H_

#include "zType.h"
#include "zebraclientService.h"
#include "zebraclient.h"
#include "loginClient.h"
#include "zSubNetService.h"
class zebraclientService : public zService
{

	public:

		~zebraclientService()
		{
		}

		static zebraclientService &getInstance()
		{
			if (NULL == instance)
				instance = new zebraclientService();

			return *instance;
		}

		/**
		 * \brief 释放类的唯一实例
		 *
		 */
		static void delInstance()
		{
			if (instance)
			{
				delete instance;
				instance = NULL;
			}
		}

		bool init();
		bool serviceCallback();
		void final();
		
		/*
		bool test();
		
		bool createAcc();
		bool changePasswd();
		*/
		void setLoopRun()
		{
			loop = true;
		}
		bool loopRun();

	private:

		static zebraclientService *instance;

		zebraclientService() : zService("测试客户端")
		{
			loop = false;
		}

		bool loop;
		LoginClient *loginClient; 

};
#endif
