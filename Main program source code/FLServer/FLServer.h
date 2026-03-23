/**
 * \file
 * \version  $Id: FLServer.h  $
 * \author  
 * \date 
 * \brief zebra项目登陆服务器，负责登陆，建立帐号、档案等功能
 *
 */

#ifndef _FLServer_h_
#define _FLServer_h_

#include "Zebra.h"
#include "zMisc.h"
#include "zMNetService.h"
#include "zTCPTaskPool.h"
#include "zDBConnPool.h"

/**
 * \brief 定义登陆服务类
 *
 * 登陆服务，负责登陆，建立帐号、档案等功能<br>
 * 这个类使用了Singleton设计模式，保证了一个进程中只有一个类的实例
 *
 */
class FLService : public zMNetService
{

	public:

		/**
		 * \brief 虚析构函数
		 *
		 */
		~FLService()
		{
			instance = NULL;

			if (loginTaskPool)
			{
				loginTaskPool->final();
				SAFE_DELETE(loginTaskPool);
			}
			
			if (serverTaskPool)
			{
				serverTaskPool->final();
				SAFE_DELETE(serverTaskPool);
			}
			
			if (pingTaskPool)
			{
				pingTaskPool->final();
				SAFE_DELETE(pingTaskPool);
			}
		}

		/**
		 * \brief 返回唯一的类实例
		 *
		 * \return 唯一的类实例
		 */
		static FLService &getInstance()
		{
			if (NULL == instance)
				instance = new FLService();

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

		/**
		 * \brief 获取连接池中的连接数
		 * \return 连接数
		 */
		const int getPoolSize() const
		{
			return loginTaskPool->getSize();
		}

		/**
		 * \brief 获取服务器类型
		 * \return 服务器类型
		 */
		const WORD getType() const
		{
			return LOGINSERVER;
		}

		void reloadConfig();

		bool jpeg_passport;
		
		static zDBConnPool *dbConnPool;

	private:

		/**
		 * \brief 类的唯一实例指针
		 *
		 */
		static FLService *instance;

		/**
		 * \brief 构造函数
		 *
		 */
		FLService() : zMNetService("登陆服务器")
		{
			jpeg_passport = false;
			login_port = 0;
			inside_port = 0;
			ping_port = 0;
			loginTaskPool = NULL;
			serverTaskPool = NULL;
			pingTaskPool = NULL;
		}

		bool init();
		void newTCPTask(const int sock, const unsigned short srcPort);
		void final();

		unsigned short login_port;
		unsigned short inside_port;
		unsigned short ping_port;

		zTCPTaskPool* loginTaskPool;
		zTCPTaskPool* serverTaskPool;
		zTCPTaskPool* pingTaskPool;
};

#endif

