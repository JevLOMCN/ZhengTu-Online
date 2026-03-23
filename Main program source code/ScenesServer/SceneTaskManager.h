/**
 * \file
 * \version  $Id: SceneTaskManager.h  $
 * \author  
 * \date 
 * \brief 管理子连接的容器
 *
 * 
 */


#ifndef _SceneTaskManager_h_
#define _SceneTaskManager_h_

#include <iostream>
#include <ext/hash_map>

#include "SceneTask.h"
#include "zNoncopyable.h"
#include "zEntryManager.h"
#include "zRWLock.h"

/**
 * \brief 场景服务器子连接管理器
 *
 */
class SceneTaskManager : private zNoncopyable
{

	public:

		/**
		 * \brief 析构函数
		 *
		 */
		~SceneTaskManager() {};

		/**
		 * \brief 获取场景子连接管理器唯一实例
		 *
		 * \return 场景子连接管理器唯一实例
		 */
		static SceneTaskManager &getInstance()
		{
			if (NULL == instance)
				instance = new SceneTaskManager();

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

		bool uniqueAdd(SceneTask *task);
		bool uniqueRemove(SceneTask *task);
		SceneTask *uniqueGet(WORD wdServerID);
		void execEvery();
		bool broadcastCmd(const void *pstrCmd, const int nCmdLen);
		template <class YourEntry>
		void execEverySceneTask(execEntry<YourEntry> &exec)
		{
			rwlock.rdlock();
			SceneTaskHashmap_iterator it;
			it = sceneTaskSet.begin();
			for (; it != sceneTaskSet.end() ; it ++)
			{
				exec.exec(it->second);
			}
			rwlock.unlock();
		}

	private:

		/**
		 * \brief 类的唯一实例指针
		 *
		 */
		static SceneTaskManager *instance;

		/**
		 * \brief 构造函数
		 *
		 */
		SceneTaskManager() {};

		/**
		 * \brief 子连接管理容器类型
		 *
		 */
		typedef __gnu_cxx::hash_map<WORD, SceneTask *> SceneTaskHashmap;
		/**
		 * \brief 定义容器迭代器类型
		 *
		 */
		typedef SceneTaskHashmap::iterator SceneTaskHashmap_iterator;
		/**
		 * \brief 定义容器常量迭代器类型
		 *
		 */
		typedef SceneTaskHashmap::const_iterator SceneTaskHashmap_const_iterator;
		/**
		 * \brief 定义容器键值对类型
		 *
		 */
		typedef SceneTaskHashmap::value_type SceneTaskHashmap_pair;
		/**
		 * \brief 容器访问互斥变量
		 *
		 */
		zRWLock rwlock;
		/**
		 * \brief 子连接管理容器类型
		 *
		 */
		SceneTaskHashmap sceneTaskSet;

};

#endif

