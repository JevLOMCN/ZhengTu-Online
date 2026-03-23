/**
 * \file
 * \version  $Id: zHttpTaskPool.cpp  $
 * \author  
 * \date 
 * \brief 定义实现轻量级(lightweight)的http服务框架
 */


#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <assert.h>

#include "zSocket.h"
#include "zThread.h"
#include "zHttpTaskPool.h"
#include "Zebra.h"
#include "zTime.h"

/**
 * \brief 轻量级http服务的主处理线程
 */
class zHttpThread : public zThread
{

	private:

		/**
		 * \brief http连接任务链表类型
		 */
		typedef std::list<zHttpTask *, __gnu_cxx::__pool_alloc<zHttpTask *> > zHttpTaskContainer;

#ifdef _USE_EPOLL_
		/**
		 * \brief epoll事件结构向量类型
		 */
		typedef std::vector<struct epoll_event> epollfdContainer;
#else
		/**
		 * \brief poll事件结构向量类型
		 */
		typedef std::vector<struct pollfd> pollfdContainer;
#endif

		zHttpTaskPool *pool;		/**< 所属的池 */
		zRTime currentTime;			/**< 当前时间 */
		zMutex mutex;				/**< 互斥变量 */
		zHttpTaskContainer tasks;	/**< 任务列表 */

#ifdef _USE_EPOLL_
		int kdpfd;
		epollfdContainer epfds;
		epollfdContainer::size_type fds_count;
#else
		pollfdContainer pfds;
#endif

	public:

		/**
		 * \brief 构造函数
		 * \param pool 所属的连接池
		 * \param name 线程名称
		 */
		zHttpThread(
				zHttpTaskPool *pool,
				const std::string &name = std::string("zHttpThread"))
			: zThread(name), pool(pool), currentTime()
			{
#ifdef _USE_EPOLL_
				kdpfd = epoll_create(256);
				assert(-1 != kdpfd);
				epfds.resize(256);
				fds_count = 0;
#endif
			}

		/**
		 * \brief 析构函数
		 */
		~zHttpThread()
		{
#ifdef _USE_EPOLL_
			TEMP_FAILURE_RETRY(::close(kdpfd));
#endif
		}

		void run();

		/**
		 * \brief 添加一个连接任务
		 * \param task 连接任务
		 */
		void add(zHttpTask *task)
		{
			mutex.lock();
#ifdef _USE_EPOLL_
			task->addEpoll(kdpfd, EPOLLIN | EPOLLERR | EPOLLPRI, (void *)task);
			tasks.push_back(task);
			fds_count++;
			if (fds_count > epfds.size())
			{
				epfds.resize(fds_count + 16);
			}
#else
			struct pollfd pfd;
			task->fillPollFD(pfd, POLLIN | POLLERR | POLLPRI);
			tasks.push_back(task);
			pfds.push_back(pfd);
#endif
			mutex.unlock();
		}

#ifdef _USE_EPOLL_
		typedef zHttpTask* zHttpTaskP;
		void remove(zHttpTaskP &task)
		{
			task->delEpoll(kdpfd, EPOLLIN | EPOLLERR | EPOLLPRI);
			tasks.remove(task);
			SAFE_DELETE(task);
			fds_count--;
		}
		void remove(zHttpTaskContainer::iterator &it)
		{
			zHttpTask *task = *it;
			task->delEpoll(kdpfd, EPOLLIN | EPOLLERR | EPOLLPRI);
			tasks.erase(it);
			SAFE_DELETE(task);
			fds_count--;
		}
#else
		void remove(zHttpTaskContainer::iterator &it, int p)
		{
			int i;
			pollfdContainer::iterator iter;
			for(iter = pfds.begin(), i = 0; iter != pfds.end(); iter++, i++)
			{
				if (i == p)
				{
					zHttpTask *task = *it;
					pfds.erase(iter);
					tasks.erase(it);
					SAFE_DELETE(task);
					break;
				}
			}
		}
#endif

};

/**
 * \brief 线程主回调函数
 */
void zHttpThread::run()
{
#ifdef _USE_EPOLL_
	zHttpTaskContainer::iterator it, next;

	while(!isFinal())
	{
		mutex.lock();
		if (!tasks.empty())
		{
			int retcode = epoll_wait(kdpfd, &epfds[0], fds_count, 0);
			if (retcode > 0)
			{
				for(int i = 0; i < retcode; i++)
				{
					zHttpTask *task = (zHttpTask *)epfds[i].data.ptr;
					if (epfds[i].events & (EPOLLERR | EPOLLPRI))
					{
						//套接口出现错误
						remove(task);
					}
					else if (epfds[i].events & EPOLLIN)
					{
						switch(task->httpCore())
						{
							case 1:		//接收成功
							case -1:	//接收失败
								remove(task);
								break;
							case 0:		//接收超时，
								break;
						}
					}
				}
			}

			currentTime.now();
			for(it = tasks.begin(), next = it, next++; it != tasks.end(); it = next, next++)
			{
				zHttpTask *task = *it;
				if (task->checkHttpTimeout(currentTime))
				{
					//超过指定时间验证还没有通过，需要回收连接
					remove(it);
				}
			}
		}
		mutex.unlock();

		zThread::msleep(50);
	}

	//把所有等待验证队列中的连接加入到回收队列中，回收这些连接
	for(it = tasks.begin(), next = it, next++; it != tasks.end(); it = next, next++)
	{
		remove(it);
	}
#else
	zHttpTaskContainer::iterator it, next;
	pollfdContainer::size_type i;

	while(!isFinal())
	{
		mutex.lock();
		if (!pfds.empty())
		{
			for(i = 0; i < pfds.size(); i++);
			{
				pfds[i].revents = 0;
			}

			if (TEMP_FAILURE_RETRY(::poll(&pfds[0], pfds.size(), 0)) > 0)
			{
				for(i = 0, it = tasks.begin(), next = it, next++; it != tasks.end(); it = next, next++, i++)
				{
					zHttpTask *task = *it;
					if (pfds[i].revents & (POLLERR | POLLPRI))
					{
						//套接口出现错误
						remove(it, i--);
					}
					else if (pfds[i].revents & POLLIN)
					{
						switch(task->httpCore())
						{
							case 1:		//接收成功
							case -1:	//接收失败
								remove(it, i--);
								break;
							case 0:		//接收超时，
								break;
						}
					}
				}
			}

			currentTime.now();
			for(i = 0, it = tasks.begin(), next = it, next++; it != tasks.end(); it = next, next++, i++)
			{
				zHttpTask *task = *it;
				if (task->checkHttpTimeout(currentTime))
				{
					//超过指定时间验证还没有通过，需要回收连接
					remove(it, i--);
				}
			}
		}
		mutex.unlock();

		zThread::msleep(50);
	}

	//把所有等待验证队列中的连接加入到回收队列中，回收这些连接
	for(i = 0, it = tasks.begin(), next = it, next++; it != tasks.end(); it = next, next++, i++)
	{
		remove(it, i--);
	}
#endif
}

/**
 * \brief 把一个TCP连接添加到验证队列中，因为存在多个验证队列，需要按照一定的算法添加到不同的验证处理队列中
 * \param task 一个连接任务
 */
bool zHttpTaskPool::addHttp(zHttpTask *task)
{
	//因为存在多个验证队列，需要按照一定的算法添加到不同的验证处理队列中
	static unsigned int hashcode = 0;
	zHttpThread *pHttpThread = (zHttpThread *)httpThreads.getByIndex(hashcode++ % maxHttpThreads);
	if (pHttpThread)
		pHttpThread->add(task);
	return true;
}

/**
 * \brief 初始化线程池，预先创建各种线程
 * \return 初始化是否成功
 */
bool zHttpTaskPool::init()
{
	//创建初始化验证线程
	for(int i = 0; i < maxHttpThreads; i++)
	{
		std::ostringstream name;
		name << "zHttpThread[" << i << "]";
		zHttpThread *pHttpThread = new zHttpThread(this, name.str());
		if (NULL == pHttpThread)
			return false;
		if (!pHttpThread->start())
			return false;
		httpThreads.add(pHttpThread);
	}

	return true;
}

/**
 * \brief 释放线程池，释放各种资源，等待各种线程退出
 */
void zHttpTaskPool::final()
{
	httpThreads.joinAll();
}

