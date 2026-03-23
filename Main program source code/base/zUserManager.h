/**
 * \file
 * \version  $Id: zUserManager.h  $
 * \author  
 * \date 
 * \brief 角色管理器定义
 */

#ifndef _USERMANAGER_H_
#define _USERMANAGER_H_

#include "zUser.h"
#include "zEntryManager.h"

/**
 * \brief 角色管理器
 *
 * 实现了ID、临时ID和名字的索引,所以这些值不能重复
 */
class zUserManager:public zEntryManager< zEntryID, zEntryTempID, zEntryName>
{
	protected:
		/**
		 * \brief 管理器访问互斥锁
		 */
		zRWLock rwlock;

	public:
		/**
		 * \brief 构造函数
		 */
		zUserManager()
		{
		}

		/**
		 * \brief 析构函数
		 */
		virtual ~zUserManager()
		{
			clear();
		}

		/**
		 * \brief 根据角色名字得到角色
		 * \param name 角色名字
		 * \return 角色指针,如果返回NULL表示没找到角色
		 */
		zUser * getUserByName( const char * name)
		{
			rwlock.rdlock();
			zUser *ret =(zUser *)getEntryByName(name);
			rwlock.unlock();
			return ret;
		}

		/**
		 * \brief 根据角色ID得到角色
		 * \param id 角色ID
		 * \return 角色指针,如果返回NULL表示没找到角色
		 */
		zUser * getUserByID( DWORD id)
		{
			rwlock.rdlock();
			zUser *ret =(zUser *)getEntryByID(id);
			rwlock.unlock();
			return ret;
		}

		/**
		 * \brief 根据角色临时ID得到角色
		 * \param tempid 角色临时ID
		 * \return 角色指针,如果返回NULL表示没找到角色
		 */
		zUser * getUserByTempID( DWORD tempid)
		{
			rwlock.rdlock();
			zUser *ret =(zUser *)getEntryByTempID(tempid);
			rwlock.unlock();
			return ret;
		}

		/**
		 * \brief 添加角色
		 * \param user 角色
		 * \return 添加是否成功
		 */
		bool addUser(zSceneEntry *user)
		{
			rwlock.wrlock();
//			Zebra::logger->debug("%s(%x) really insert into user manager", user->name, user);			
			bool ret =addEntry((zEntry *)user);
			rwlock.unlock();
			return ret;
		}

		/**
		 * \brief 移出角色
		 * \param user 角色
		 */
		void removeUser(zSceneEntry *user)
		{
			rwlock.wrlock();
//			Zebra::logger->debug("%s(%x) really removed from user manager", user->name, user);
			removeEntry((zEntry *)user);
			rwlock.unlock();
		}

		/**
		 * \brief 移出符合条件的角色
		 * \param pred 条件断言
		 */
		template <class YourUserEntry>
		void removeUser_if(removeEntry_Pred<YourUserEntry> &pred)
		{
			rwlock.wrlock();
			removeEntry_if<>(pred);
			rwlock.unlock();
		}

		/**
		 * \brief 对每个用户执行
		 * \param exec 执行接口
		 */
		template <class YourUserEntry>
		bool execEveryUser(execEntry<YourUserEntry> &exec)
		{
			rwlock.rdlock();
			bool ret=execEveryEntry<>(exec);
			rwlock.unlock();
			return ret;
		}
};
#endif
