/**
 * \file
 * \version  $Id: MiniTaskManager.h $
 * \author  
 * \date 
 * \brief 管理子连接的容器
 *
 * 
 */


#ifndef _MINITASKMANAGER_H_
#define _MINITASKMANAGER_H_

#include <iostream>
#include <ext/hash_map>

#include "MiniTask.h"
#include "zRWLock.h"

/**
 * \brief 计费服务器子连接管理器
 *
 */
class MiniTaskManager
{

	public:

		/**
		 * \brief 析构函数
		 *
		 */
		~MiniTaskManager() {};

		/**
		 * \brief 获取子连接管理器唯一实例
		 *
		 * \return 子连接唯一实例
		 */
		static MiniTaskManager &getInstance()
		{
			if (NULL == instance)
				instance = new MiniTaskManager();

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

		bool uniqueAdd(MiniTask *task);
		bool uniqueRemove(MiniTask *task);
		bool broadcastByID(const WORD wdServerID, const void *pstrCmd, int nCmdLen);
		MiniTask *getTaskByID(const WORD wdServerID);
		void execEvery();

	private:

		/**
		 * \brief 类的唯一实例指针
		 *
		 */
		static MiniTaskManager *instance;

		/**
		 * \brief 构造函数
		 *
		 */
		MiniTaskManager() {};

		/**
		 * \brief 定义容器类型
		 *
		 */
		typedef __gnu_cxx::hash_map<WORD, MiniTask *> MiniTaskHashmap;
		/**
		 * \brief 定义容器迭代器类型
		 *
		 */
		typedef MiniTaskHashmap::iterator MiniTaskHashmap_iterator;
		/**
		 * \brief 定义容器常量迭代器类型
		 *
		 */
		typedef MiniTaskHashmap::const_iterator MiniTaskHashmap_const_iterator;
		/**
		 * \brief 定义容器键值对类型
		 *
		 */
		typedef MiniTaskHashmap::value_type MiniTaskHashmap_pair;
		/**
		 * \brief 容器访问互斥变量
		 *
		 */
		zRWLock rwlock;
		/**
		 * \brief 声明一个容器，存放所有的子连接
		 *
		 */
		MiniTaskHashmap sessionTaskSet;

};

#endif

