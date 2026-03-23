/**
 * \file
 * \version  $Id: zTimer.h  $
 * \author  
 * \date 
 * \brief 计时器声明文件
 *
 */

#ifndef _ZTIMER_H_
#define _ZTIMER_H_

#include <queue>
#include <sstream>

#include "zMutex.h"
#include "zThread.h"
#include "zTime.h"
#include "zNoncopyable.h"
/**
 * \brief 定时器,计时精度为毫秒
 *
 * 其内部有一个单独的处理线程。可以添加多个任务。由于多个任务共用一个线程，在执行的时候阻塞会影响其它任务的执行。
 */
class zTimer
{
	private:
		class zTimerThread;
		class zTimerQueue;
	public:
		/**
		 * \brief 计时器任务基类,用户必须继承实现#run函数 
		 *
		 */
		class zTimerTask : private zNoncopyable
		{
			friend class zTimer;
			friend class zTimerThread;
			friend class zTimerQueue;

			private:
				void schedExecTime(const zRTime &tv);
			protected:

			zMutex taskMutex;				/**< 任务互斥锁 */	
			int period;						/**< 任务间隔,如果为0表示一次性任务 */
			zRTime nextExecTime;			/**< 下一次执行任务的时间 */
			enum
			{
				VIRGIN,						/**< 新任务 */
				SCHEDULED,					/**< 已经添加到计时器中的任务 */
				EXECUTED,					/**< 已经执行国的任务 */
				CANCELLED					/**< 被取消的任务 */
			}
			state;							/**< 任务状态 */

			zTimerTask();

			public:
			bool cancel();
			bool operator > (const zTimerTask & right)const;

			virtual ~zTimerTask();
			virtual void run() =0;
		};

		zTimer(const std::string &name="Timer-" + ((std::ostringstream &)(std::ostringstream()<<getID())).str() );
		~zTimer();
		bool schedule(zTimerTask *task, zRTime &tv);
		bool schedule(zTimerTask *task, int delay);
		bool scheduleAtDelay(zTimerTask *task, int delay ,int period);
		bool scheduleAtDelay(zTimerTask *task, zRTime &tv,int period);
		bool scheduleAtRate(zTimerTask *task, int delay ,int period);
		bool scheduleAtRate(zTimerTask *task, zRTime &tv,int period);
		void cancel();
		
	private:
		/**
		 * \brief 计时器的任务队列,插入排序 
		 *
		 */
		class zTimerQueue
		{
			friend class zTimer;
			friend class zTimerThread;
			private:
				std::vector <zTimerTask *> queue;
				zMutex qmutex;
				bool canAddTask;

			public:

				void push(zTimerTask * p)
				{
					std::vector <zTimerTask *> ::iterator it=queue.end();
					for(it = queue.begin();it!=queue.end();it++)
					{
						if(**it > *p)
						{
							break;
						}
					}
					queue.insert(it,p);
				}

				zTimerTask * top()
				{
					if(queue.empty())
						return  (zTimerTask *)0;
					else
						return queue[0];
				}

				void pop()
				{
					if(!queue.empty())
						queue.erase(queue.begin());
				}

				int size()
				{
					return queue.size();
				}

				bool empty()
				{
					return queue.empty();
				}
		};


		/**
		 * \brief 计时器的任务执行线程，检查任务队列，如果任务满足执行条件，执行 
		 *
		 */
		class zTimerThread:public zThread
		{
			private:
				zTimerQueue *taskQueue;
			public:
				zTimerThread(const std::string & name,zTimerQueue *queue);
				~zTimerThread();
				virtual void run();
		};


		zTimerQueue taskqueue;	/**< 执行任务队列,在#thread前初始化 */
		zTimerThread thread;	/**< 执行任务线程,在#taskqueue后初始化 */ 

		static int id;	/**< 一个计数器，在提供默认计时器名字时起作用 */ 
		static int getID();
		bool addTask(zTimerTask *task, zRTime &tv,int period);
};

#endif
