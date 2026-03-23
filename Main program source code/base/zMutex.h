/**
 * \file
 * \version  $Id: zMutex.h  $
 * \author  
 * \date 
 * \brief 互斥体的封装，主要是为了使用方便
 *
 * 
 */

#ifndef _zMutex_h_
#define _zMutex_h_

#include <pthread.h>
#include <iostream>

#include "zNoncopyable.h"

/**
 * \brief 互斥体，封装了系统互斥体，避免了使用系统互斥体时候需要手工初始化和销毁互斥体对象的操作
 *
 */
class zMutex : private zNoncopyable
{

	friend class zCond;

	public:

		/**
		 * \brief 构造函数，构造一个互斥体对象
		 *
		 */
		zMutex(int kind = PTHREAD_MUTEX_FAST_NP) 
		{
			//std::cout << __PRETTY_FUNCTION__ << std::endl;
			pthread_mutexattr_t attr;
			::pthread_mutexattr_init(&attr);
			::pthread_mutexattr_settype(&attr, kind);
			::pthread_mutex_init(&mutex, &attr);
		}

		/**
		 * \brief 析构函数，销毁一个互斥体对象
		 *
		 */
		~zMutex()
		{
			//std::cout << __PRETTY_FUNCTION__ << std::endl;
			::pthread_mutex_destroy(&mutex);
		}

		/**
		 * \brief 加锁一个互斥体
		 *
		 */
		inline void lock()
		{
			::pthread_mutex_lock(&mutex);
		}

		/**
		 * \brief 解锁一个互斥体
		 *
		 */
		inline void unlock()
		{
			::pthread_mutex_unlock(&mutex);
		}

	private:

		pthread_mutex_t mutex;		/**< 系统互斥体 */

};

/**
 * \brief Wrapper
 * 方便在复杂函数中锁的使用
 */
class zMutex_scope_lock : private zNoncopyable
{

	public:

		/**
		 * \brief 构造函数
		 * 对锁进行lock操作
		 * \param m 锁的引用
		 */
		zMutex_scope_lock(zMutex &m) : mlock(m)
		{
			mlock.lock();
		}

		/**
		 * \brief 析购函数
		 * 对锁进行unlock操作
		 */
		~zMutex_scope_lock()
		{
			mlock.unlock();
		}

	private:

		/**
		 * \brief 锁的引用
		 */
		zMutex &mlock;

};

#endif

