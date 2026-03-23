/**
 * \file
 * \version  $Id: zSyncEvent.h  $
 * \author  
 * \date 
 * \brief 实现一个同步事件模型
 *
 * 
 */

#ifndef _zSyncEvent_h_
#define _zSyncEvent_h_

#include "zNoncopyable.h"
#include "zMutex.h"
#include "zCond.h"

/**
 * \brief 实现了同步事件类
 *
 * 	一个线程通过signal()来设置信号，另外一个线程通过wait()等待这个信号处理
 *
 */
class zSyncEvent : private zNoncopyable
{

	public:

		/**
		 * \brief 构造函数
		 *
		 * 用于创建一个对象实例
		 *
		 * \param initstate 初始状态
		 */
		zSyncEvent(const bool initstate = false) : state(initstate) {};

		/**
		 * \brief 析构函数
		 *
		 * 销毁一个对象实例
		 *
		 */
		~zSyncEvent() {};

		/**
		 * \brief 设置信号
		 *
		 * 发送一个事件信号，唤醒等待这个信号的线程
		 *
		 */
		void signal()
		{
			mutex.lock();
			while(state)
				cond2.wait(mutex);
			state = true;
			cond1.signal();
			mutex.unlock();
		}

		/**
		 * \brief 等待一个事件信号的到达
		 *
		 * 等待一个信号，直到有一个线程调用signal唤醒这个线程
		 *
		 */
		void wait()
		{
			mutex.lock();
			while(!state)
				cond1.wait(mutex);
			state = false;
			cond2.signal();
			mutex.unlock();
		}

	private:

		volatile bool state;		/**< 事件当前状态 */
		zMutex mutex;				/**< 互斥体 */
		zCond cond1, cond2;			/**< 条件变量 */

};

#endif

