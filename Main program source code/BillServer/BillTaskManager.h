/**
 * \file
 * \version  $Id: BillTaskManager.h  $
 * \author  
 * \date 
 * \brief 管理子连接的容器
 *
 * 
 */


#ifndef _BillTaskManager_h_
#define _BillTaskManager_h_

#include <iostream>
#include <ext/hash_map>

#include "BillTask.h"
#include "zRWLock.h"

/**
 * \brief 计费服务器子连接管理器
 *
 */
class BillTaskManager
{

	public:

		/**
		 * \brief 析构函数
		 *
		 */
		~BillTaskManager() {};

		/**
		 * \brief 获取子连接管理器唯一实例
		 *
		 * \return 子连接唯一实例
		 */
		static BillTaskManager &getInstance()
		{
			if (NULL == instance)
				instance = new BillTaskManager();

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

		bool uniqueAdd(BillTask *task);
		bool uniqueRemove(BillTask *task);
		bool broadcastByID(const WORD wdServerID, const void *pstrCmd, int nCmdLen);
		BillTask *getTaskByID(const WORD wdServerID);
		void execEvery();

	private:

		/**
		 * \brief 类的唯一实例指针
		 *
		 */
		static BillTaskManager *instance;

		/**
		 * \brief 构造函数
		 *
		 */
		BillTaskManager() {};

		/**
		 * \brief 定义容器类型
		 *
		 */
		typedef __gnu_cxx::hash_map<WORD, BillTask *> BillTaskHashmap;
		/**
		 * \brief 定义容器迭代器类型
		 *
		 */
		typedef BillTaskHashmap::iterator BillTaskHashmap_iterator;
		/**
		 * \brief 定义容器常量迭代器类型
		 *
		 */
		typedef BillTaskHashmap::const_iterator BillTaskHashmap_const_iterator;
		/**
		 * \brief 定义容器键值对类型
		 *
		 */
		typedef BillTaskHashmap::value_type BillTaskHashmap_pair;
		/**
		 * \brief 容器访问互斥变量
		 *
		 */
		zRWLock rwlock;
		/**
		 * \brief 声明一个容器，存放所有的子连接
		 *
		 */
		BillTaskHashmap sessionTaskSet;

};

#endif

