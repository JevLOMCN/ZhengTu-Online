/**
 * \file
 * \version  $Id: InfoClientManager.h $
 * \author  
 * \date 
 * \brief 定义服务器信息采集连接的客户端管理容器
 */


#ifndef _InfoClientManager_h_
#define _InfoClientManager_h_

#include <vector>

#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"
#include "InfoClient.h"
#include "zTime.h"

/**
 * \brief 定义服务器信息采集连接的客户端管理容器
 */
class InfoClientManager
{

	public:

		~InfoClientManager();

		/**
		 * \brief 获取类的唯一实例
		 * \return 类的唯一实例引用
		 */
		static InfoClientManager &getInstance()
		{
			if (NULL == instance)
				instance = new InfoClientManager();

			return *instance;
		}

		/**
		 * \brief 销毁类的唯一实例
		 */
		static void delInstance()
		{
			SAFE_DELETE(instance);
		}

		bool init();
		void timeAction(const zTime &ct);
		void add(InfoClient *infoClient);
		void remove(InfoClient *infoClient);
		bool broadcast(const void *pstrCmd, int nCmdLen);

	private:

		InfoClientManager();
		static InfoClientManager *instance;

		/**
		 * \brief 客户端连接管理池
		 */
		zTCPClientTaskPool *infoClientPool;
		/**
		 * \brief 进行断线重连检测的时间记录
		 */
		zTime actionTimer;

		/**
		 * \brief 存放连接已经成功的连接容器类型
		 */
		typedef std::vector<InfoClient *> InfoClientContainer;
		/**
		 * \brief 存放连接已经成功的连接容器
		 */
		InfoClientContainer allClients;
		/**
		 * \brief 容器访问互斥变量
		 */
		zMutex mlock;

};

#endif

