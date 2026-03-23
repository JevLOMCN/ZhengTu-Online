/**
 * \file
 * \version  $Id: GatewayTaskManager.h  $
 * \author  
 * \date 
 * \brief 管理子连接的容器
 *
 * 
 */


#ifndef _GatewayTaskManager_h_
#define _GatewayTaskManager_h_

#include <iostream>
#include <ext/hash_map>

#include "zEntry.h"
#include "GatewayTask.h"
#include "zRWLock.h"

/**
 * \brief 服务器子连接管理器
 *
 */
class GatewayTaskManager
{

	public:

		/**
		 * \brief 回调函数
		 *
		 */
		typedef zEntryCallback<GatewayTask> GatewayTaskCallback;

		/**
		 * \brief 析构函数
		 *
		 */
		~GatewayTaskManager();

		/**
		 * \brief 获取子连接管理器唯一实例
		 *
		 * \return 子连接管理器唯一实例
		 */
		static GatewayTaskManager &getInstance()
		{
			if (NULL == instance)
				instance = new GatewayTaskManager();

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

		bool uniqueAdd(GatewayTask *task);
		bool uniqueRemove(GatewayTask *task);
		void accountVerifyOK(const DWORD accid, const bool ok);
		void execAll(GatewayTaskCallback &callback);

	private:

		/**
		 * \brief 类的唯一实例指针
		 *
		 */
		static GatewayTaskManager *instance;

		/**
		 * \brief 构造函数
		 *
		 */
		GatewayTaskManager();

		/**
		 * \brief 定义容器类型
		 *
		 */
		typedef __gnu_cxx::hash_map<DWORD, GatewayTask *> GatewayTaskHashmap;
		/**
		 * \brief 定义容器迭代器类型
		 *
		 */
		typedef GatewayTaskHashmap::iterator GatewayTaskHashmap_iterator;
		/**
		 * \brief 定义容器常量迭代器类型
		 *
		 */
		typedef GatewayTaskHashmap::const_iterator GatewayTaskHashmap_const_iterator;
		/**
		 * \brief 定义容器键值对类型
		 *
		 */
		typedef GatewayTaskHashmap::value_type GatewayTaskHashmap_pair;
		/**
		 * \brief 容器访问互斥变量
		 *
		 */
		zRWLock rwlock;
		/**
		 * \brief 子连接管理容器类型
		 *
		 */
		GatewayTaskHashmap gatewayTaskSet;

};

#endif

