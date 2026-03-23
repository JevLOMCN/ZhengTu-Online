/**
 * \file
 * \version  $Id: zSceneManager.h $
 * \author  
 * \date 
 * \brief 场景管理器定义
 */

#ifndef _ZSCENEMANAGER_H_
#define _ZSCENEMANAGER_H_

#include "zScene.h"
#include "zEntryManager.h"

/**
 * \brief 场景管理器
 *
 * 以名字和临时ID索引,没有ID索引，因为场景可能重复
 */
class zSceneManager:public zEntryManager<zEntryID, zEntryTempID, zEntryName>
{
	protected:
		/**
		 * \brief 访问管理器的互斥锁
		 */
		zRWLock rwlock;

		zScene * getSceneByName( const char * name)
		{
			rwlock.rdlock();
			zScene *ret =(zScene *)getEntryByName(name);
			rwlock.unlock();
			return ret;
		}

		zScene * getSceneByID(DWORD id)
		{
			rwlock.rdlock();
			zScene *ret =(zScene *)getEntryByID(id);
			rwlock.unlock();
			return ret;
		}
		
		zScene * getSceneByTempID( DWORD tempid)
		{
			rwlock.rdlock();
			zScene *ret =(zScene *)getEntryByTempID(tempid);
			rwlock.unlock();
			return ret;
		}

		template <class YourSceneEntry>
		bool execEveryScene(execEntry<YourSceneEntry> &exec)
		{
			rwlock.rdlock();
			bool ret=execEveryEntry<>(exec);
			rwlock.unlock();
			return ret;
		}

		/**
		 * \brief 移出符合条件的角色
		 * \param pred 条件断言
		 */
		template <class YourSceneEntry>
		void removeScene_if(removeEntry_Pred<YourSceneEntry> &pred)
		{
			rwlock.wrlock();
			removeEntry_if<>(pred);
			rwlock.unlock();
		}

	public:
		/**
		 * \brief 构造函数
		 */
		zSceneManager()
		{
		}

		/**
		 * \brief 析构函数
		 */
		virtual ~zSceneManager()
		{
			clear();
		}

};
#endif
