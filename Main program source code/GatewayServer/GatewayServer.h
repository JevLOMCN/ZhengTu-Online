/**
 * \file
 * \version  $Id: GatewayServer.h  $
 * \author  
 * \date 
 * \brief zebra项目Gateway服务器,负责用户指令检查转发、加密解密等
 */

#ifndef _GatewayServer_h_
#define _GatewayServer_h_

#include "zSubNetService.h"
#include "Zebra.h"
#include "zMisc.h"
#include "SuperCommand.h"
#include "SessionClient.h"
#include "CountryInfo.h"
#include <vector>
#include <string>


/**
 * \brief 定义网关服务类
 *
 * 这个类使用了Singleton设计模式，保证了一个进程中只有一个类的实例
 *
 */
class GatewayService : public zSubNetService
{

	public:

		bool msgParse_SuperService(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);

		const int getPoolSize() const
		{
			if(taskPool)
			{
			return taskPool->getSize();
			}
			else
			{
				return 0;
			}
		}

		const int getPoolState() const
		{
			return taskPool->getState();
		}

		bool notifyLoginServer();

		/**
		 * \brief 校验客户端版本号
		 */
		DWORD verify_client_version;
		
		/**
		 * \brief 虚析构函数
		 *
		 */
		~GatewayService()
		{
			if (sessionClient)
			{
				sessionClient->final();
				sessionClient->join();
				SAFE_DELETE(sessionClient);
			}
			
		}

		/**
		 * \brief 返回类的唯一实例
		 *
		 * \return 类的唯一实例
		 */
		static GatewayService &getInstance()
		{
			if (NULL == instance)
				instance = new GatewayService();

			return *instance;
		}

		/**
		 * \brief 释放类的唯一实例
		 *
		 */
		static void delInstance()
		{
			//关闭线程池
			if (taskPool)
			{
				taskPool->final();
				SAFE_DELETE(taskPool);
			}
		
			SAFE_DELETE(instance);
		}

		void reloadConfig();
		bool isSequeueTerminate() 
		{
			return taskPool == NULL;
		}

		bool jpeg_passport;

	private:

		/**
		 * \brief 类的唯一实例指针
		 *
		 */
		static GatewayService *instance;

		static zTCPTaskPool *taskPool;				/**< TCP连接池的指针 */


		//国家名称(地图)信息

		/**
		 * \brief 构造函数
		 *
		 */
		GatewayService() : zSubNetService("网关服务器", GATEWAYSERVER)
		{
			jpeg_passport = false;
			rolereg_verify = true;
			taskPool = NULL;
		}

		bool init();
		void newTCPTask(const int sock, const struct sockaddr_in *addr);
		void final();

		/**
		 * \brief 确认服务器初始化成功，即将进入主回调函数
		 *
		 * 向服务器发送t_Startup_OK指令来确认服务器启动成功
		 * 并且通知所有登陆服务器，这台网关服务器准备好了
		 *
		 * \return 确认是否成功
		 */
		virtual bool validate()
		{
			zSubNetService::validate();

			return notifyLoginServer();
		}


	public:
		CountryInfo country_info;
		bool rolereg_verify;
		static bool service_gold;
		static bool service_stock;


};

#endif

