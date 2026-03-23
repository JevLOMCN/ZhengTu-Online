/**
 * \file
 * \version  $Id: SessionTaskManager.h  $
 * \author 
 * \date 
 * \brief 管理子连接的容器
 *
 * 
 */


#ifndef _SessionTaskManager_h_
#define _SessionTaskManager_h_

#include <iostream>
#include <list>
#include <ext/hash_map>

#include "SessionTask.h"
#include "zNoncopyable.h"
#include "zRWLock.h"

/**
 * \brief Session服务器子连接管理器
 *
 */
class SessionTaskManager : private zNoncopyable
{

	public:

		/**
		 * \brief 析构函数
		 *
		 */
		~SessionTaskManager() {};

		/**
		 * \brief 获取子连接管理器唯一实例
		 *
		 * \return 子连接唯一实例
		 */
		static SessionTaskManager &getInstance()
		{
			if (NULL == instance)
				instance = new SessionTaskManager();

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

		void addSessionTask(SessionTask *task);
		void removeSessionTask(SessionTask *task);
		bool uniqueAdd(SessionTask *task);
		bool uniqueRemove(SessionTask *task);
		bool broadcastGateway(const void *pstrCmd, int nCmdLen);
		bool broadcastScene(const void *pstrCmd, int nCmdLen);
		bool sendCmdToWorld(const void *pstrCmd, int nCmdLen);
		bool broadcastByID(const WORD wdServerID, const void *pstrCmd, int nCmdLen);
		void execEvery();
		bool sendCmdToCountry(DWORD country,const void *pstrCmd, int nCmdLen);

	private:

		/**
		 * \brief 类的唯一实例指针
		 *
		 */
		static SessionTaskManager *instance;

		/**
		 * \brief 构造函数
		 *
		 */
		SessionTaskManager() {};

		/**
		 * \brief 定义容器类型
		 *
		 */
		typedef __gnu_cxx::hash_map<WORD, SessionTask *> SessionTaskHashmap;
		/**
		 * \brief 定义容器迭代器类型
		 *
		 */
		typedef SessionTaskHashmap::iterator SessionTaskHashmap_iterator;
		/**
		 * \brief 定义容器常量迭代器类型
		 *
		 */
		typedef SessionTaskHashmap::const_iterator SessionTaskHashmap_const_iterator;
		/**
		 * \brief 定义容器键值对类型
		 *
		 */
		typedef SessionTaskHashmap::value_type SessionTaskHashmap_pair;
		/**
		 * \brief 定义容器类型
		 *
		 */
		typedef std::list<SessionTask *> TaskContainer;
		/**
		 * \brief 定义容器迭代器类型
		 *
		 */
		typedef TaskContainer::iterator TaskContainer_iterator;
		/**
		 * \brief 容器访问互斥变量
		 *
		 */
		zRWLock rwlock;
		//zMutex mlock;
		/**
		 * \brief 声明一个容器，存放所有的子连接
		 *
		 */
		SessionTaskHashmap sessionTaskSet;
		/**
		 * \brief 与场景服务器连接的链表
		 *
		 */
		TaskContainer sceneTaskList;
		/**
		 * \brief 与网关服务器连接的链表
		 *
		 */
		TaskContainer gatewayTaskList;

};

#endif

