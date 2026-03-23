/**
 * \file
 * \version  $Id: RoleregClientManager.h  $
 * \author  
 * \date 
 * \brief 定义角色名称唯一性验证服务连接的客户端管理容器
 */


#ifndef _RoleregClientManager_h_
#define _RoleregClientManager_h_

#include <set>

#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"
#include "RoleregClient.h"
#include "zTime.h"
#include "zRWLock.h"

/**
 * \brief 定义角色名称唯一性验证服务连接的客户端管理容器
 */
class RoleregClientManager
{

	public:

		~RoleregClientManager();

		/**
		 * \brief 获取类的唯一实例
		 * \return 类的唯一实例引用
		 */
		static RoleregClientManager &getInstance()
		{
			if (NULL == instance)
				instance = new RoleregClientManager();

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
		void add(RoleregClient *roleregClient);
		void remove(RoleregClient *roleregClient);
		bool broadcastOne(const void *pstrCmd, int nCmdLen);

	private:

		RoleregClientManager();
		static RoleregClientManager *instance;

		/**
		 * \brief 客户端连接管理池
		 */
		zTCPClientTaskPool *roleregClientPool;
		/**
		 * \brief 进行断线重连检测的时间记录
		 */
		zTime actionTimer;

		struct lt_client
		{
			bool operator()(RoleregClient *s1, RoleregClient *s2) const
			{
				return s1->getNetType() < s2->getNetType();
			}
		};
		/**
		 * \brief 存放连接已经成功的连接容器类型
		 */
		typedef std::multiset<RoleregClient *, lt_client> RoleregClient_set;
		typedef RoleregClient_set::iterator iter;
		typedef RoleregClient_set::const_iterator const_iter;
		/**
		 * \brief 存放连接已经成功的连接容器
		 */
		RoleregClient_set allClients;
		/**
		 * \brief 容器访问读写锁
		 */
		zRWLock rwlock;

};

#endif

