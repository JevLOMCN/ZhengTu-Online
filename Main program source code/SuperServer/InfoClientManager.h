/**
 * \file
 * \version  $Id: InfoClientManager.h  $
 * \author  
 * \date 
 * \brief 定义服务器信息采集连接的客户端管理容器
 */


#ifndef _InfoClientManager_h_
#define _InfoClientManager_h_

#include <map>
#include <set>

#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"
#include "InfoClient.h"
#include "zTime.h"
#include "zRWLock.h"

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
		bool broadcastOne(const void *pstrCmd, int nCmdLen);
		bool sendTo(const DWORD tempid, const void *pstrCmd, int nCmdLen);

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
		typedef std::map<const DWORD, InfoClient *> InfoClient_map;
		typedef InfoClient_map::iterator iter;
		typedef InfoClient_map::const_iterator const_iter;
		typedef InfoClient_map::value_type value_type;
		/**
		 * \brief 存放连接已经成功的连接容器
		 */
		InfoClient_map allClients;

		struct lt_client
		{
			bool operator()(InfoClient *s1, InfoClient *s2) const
			{
				return s1->getNetType() < s2->getNetType();
			}
		};
		typedef std::multiset<InfoClient *, lt_client> InfoClient_set;
		InfoClient_set setter;

		/**
		 * \brief 容器访问读写锁
		 */
		zRWLock rwlock;

};

#endif

