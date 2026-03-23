/**
 * \file
 * \version  $Id: FLClientManager.h  $
 * \author  
 * \date 
 * \brief 定义统一用户平台客户端连接的管理容器
 */


#ifndef _FLClientManager_h_
#define _FLClientManager_h_

#include <ext/hash_map>

#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"
#include "FLClient.h"
#include "zTime.h"
#include "zRWLock.h"

/**
 * \brief 统一用户平台登陆服务器的客户端连接类管理器
 */
class FLClientManager
{

	public:

		~FLClientManager();

		/**
		 * \brief 获取类的唯一实例
		 * \return 类的唯一实例引用
		 */
		static FLClientManager &getInstance()
		{
			if (NULL == instance)
				instance = new FLClientManager();

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
		void add(FLClient *flClient);
		void remove(FLClient *flClient);
		void broadcast(const void *pstrCmd, int nCmdLen);
		void sendTo(const WORD tempid, const void *pstrCmd, int nCmdLen);

	private:

		FLClientManager();
		static FLClientManager *instance;

		/**
		 * \brief 客户端连接管理池
		 */
		zTCPClientTaskPool *flClientPool;
		/**
		 * \brief 进行断线重连检测的时间记录
		 */
		zTime actionTimer;

		/**
		 * \brief 存放连接已经成功的连接容器类型
		 */
		typedef __gnu_cxx::hash_map<WORD, FLClient *> FLClientContainer;
		typedef FLClientContainer::iterator iter;
		typedef FLClientContainer::const_iterator const_iter;
		typedef FLClientContainer::value_type value_type;
		/**
		 * \brief 存放连接已经成功的连接容器
		 */
		FLClientContainer allClients;
		/**
		 * \brief 容器访问读写锁
		 */
		zRWLock rwlock;

};

#endif

