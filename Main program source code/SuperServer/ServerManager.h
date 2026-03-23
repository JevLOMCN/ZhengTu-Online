/**
 * \file
 * \version  $Id: ServerManager.h  $
 * \author  
 * \date 
 * \brief 定义服务器管理容器
 *
 * 这个容器包括全局容器和唯一性验证容器
 * 
 */


#ifndef _ServerManager_h_
#define _ServerManager_h_

#include <iostream>
#include <list>
#include <ext/hash_map>

#include "ServerTask.h"
#include "zMutex.h"
#include "zNoncopyable.h"

/**
 * \brief 服务器管理容器类
 *
 * 这个容器包括全局容器和唯一性验证容器
 *
 */
class ServerManager : zNoncopyable
{

	public:

		/**
		 * \brief 缺省析构函数
		 *
		 */
		~ServerManager() {};

		/**
		 * \brief 获取类的唯一实例
		 *
		 * 这个类使用了Singleton设计模式，保证了一个进程中只有一个类的实例
		 *
		 */
		static ServerManager &getInstance()
		{
			if (NULL == instance)
				instance = new ServerManager();

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

		void addServer(ServerTask *task);
		void removeServer(ServerTask *task);
		ServerTask *getServer(WORD wdServerID);
		bool uniqueAdd(ServerTask *task);
		bool uniqueVerify(const WORD wdServerID);
		bool uniqueRemove(ServerTask *task);
		bool broadcast(const void *pstrCmd, int nCmdLen);
		bool broadcastByID(const WORD wdServerID, const void *pstrCmd, int nCmdLen);
		bool broadcastByType(const WORD wdType, const void *pstrCmd, int nCmdLen);
		const DWORD caculateOnlineNum();
		void responseOther(const WORD srcID, const WORD wdServerID);

	private:

		/**
		 * \brief 定义服务器容器类型
		 *
		 */
		typedef std::list<ServerTask *> Container;
		/**
		 * \brief 定义服务器容器类型的迭代器
		 *
		 */
		typedef Container::iterator Container_iterator;
		/**
		 * \brief 定义服务器容器类型的常量迭代器
		 *
		 */
		typedef Container::const_iterator Containter_const_iterator;
		/**
		 * \brief 定义了服务器的唯一性验证容器类型
		 * 
		 **/
		typedef __gnu_cxx::hash_map<WORD, ServerTask *> ServerTaskHashmap;
		/**
		 * \brief 定义容器的迭代器类型
		 *
		 */
		typedef ServerTaskHashmap::iterator ServerTaskHashmap_iterator;
		/**
		 * \brief 定义了容器的常量迭代器类型
		 *
		 */
		typedef ServerTaskHashmap::const_iterator ServerTaskHashmap_const_iterator;
		/**
		 * \brief 定义了容器的键值对类型
		 *
		 */
		typedef ServerTaskHashmap::value_type ServerTaskHashmap_pair;
		/**
		 * \brief 容器访问的互斥变量
		 *
		 */
		zMutex mutex;
		/**
		 * \brief 服务器全局容器的实例
		 *
		 */
		Container container;
		/**
		 * \brief 唯一性容器实例
		 *
		 */
		ServerTaskHashmap taskUniqueContainer;

		/**
		 * \brief 类的唯一实例指针
		 *
		 */
		static ServerManager *instance;

		/**
		 * \brief 构造函数
		 *
		 */
		ServerManager() {};

};

#endif

