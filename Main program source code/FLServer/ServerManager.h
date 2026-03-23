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

		bool uniqueAdd(ServerTask *task);
		bool uniqueRemove(ServerTask *task);
		bool broadcast(const GameZone_t &gameZone, const void *pstrCmd, int nCmdLen);

	private:

		/**
		 * \brief 构造函数
		 *
		 */
		ServerManager() {};

		/**
		 * \brief 类的唯一实例指针
		 *
		 */
		static ServerManager *instance;

		/**
		 * \brief hash函数
		 *
		 */
		struct GameZone_hash
		{
			size_t operator()(const GameZone_t &gameZone) const
			{
				__gnu_cxx::hash<DWORD> H;
				return H(gameZone.id);
			}
		};
		/**
		 * \brief 定义了服务器的唯一性验证容器类型
		 * 
		 **/
		typedef __gnu_cxx::hash_map<const GameZone_t, ServerTask *, GameZone_hash> ServerTaskContainer;
		/**
		 * \brief 定义容器的迭代器类型
		 *
		 */
		typedef ServerTaskContainer::iterator ServerTaskContainer_iterator;
		/**
		 * \brief 定义了容器的常量迭代器类型
		 *
		 */
		typedef ServerTaskContainer::const_iterator ServerTaskContainer_const_iterator;
		/**
		 * \brief 定义了容器的键值对类型
		 *
		 */
		typedef ServerTaskContainer::value_type ServerTaskContainer_value_type;
		/**
		 * \brief 容器访问的互斥变量
		 *
		 */
		zMutex mlock;
		/**
		 * \brief 唯一性容器实例
		 *
		 */
		ServerTaskContainer taskUniqueContainer;

};

#endif

