/**
 * \file
 * \version  $Id: zTCPTaskPool.cpp  $
 * \author  
 * \date 
 * \brief 实现线程池类，用于处理多连接服务器
 *
 * 
 */


#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <assert.h>
#include <ext/pool_allocator.h>

#include "zSocket.h"
#include "zThread.h"
#include "zTCPTaskPool.h"
#include "Zebra.h"
#include "zTime.h"

int zTCPTaskPool::usleep_time=50000;										/**< 循环等待时间 */
/**
 * \brief 连接任务链表
 *
 */
typedef std::list<zTCPTask *, __gnu_cxx::__pool_alloc<zTCPTask *> > zTCPTaskContainer;

/**
 * \brief 连接任务链表叠代器
 *
 */
typedef zTCPTaskContainer::iterator zTCPTask_IT;

#ifdef _USE_EPOLL_
typedef std::vector<struct epoll_event> epollfdContainer;
#else
typedef std::vector<struct pollfd> pollfdContainer;
#endif

class zTCPTaskQueue
{
	public:
		zTCPTaskQueue() :_size(0) {}
		virtual ~zTCPTaskQueue() {}
		inline void add(zTCPTask *task)
		{
			mlock.lock();
			_queue.push(task);
			_size++;
			mlock.unlock();
		}
		inline void check_queue()
		{
			mlock.lock();
			while(!_queue.empty())
			{
				zTCPTask *task = _queue.front();
				_queue.pop();
				_add(task);
			}
			_size = 0;
			mlock.unlock();
		}
	protected:
		virtual void _add(zTCPTask *task) = 0;
		unsigned int _size;
	private:
		zMutex mlock;
		std::queue<zTCPTask *, std::deque<zTCPTask *, __gnu_cxx::__pool_alloc<zTCPTask *> > > _queue;
};

/**
 * \brief 处理TCP连接的验证，如果验证不通过，需要回收这个连接
 *
 */
class zVerifyThread : public zThread, public zTCPTaskQueue
{

	private:

		zTCPTaskPool *pool;
		zTCPTaskContainer tasks;	/**< 任务列表 */
		zTCPTaskContainer::size_type task_count;			/**< tasks计数(保证线程安全*/
#ifdef _USE_EPOLL_
		int kdpfd;
		epollfdContainer epfds;
#else
		pollfdContainer pfds;
#endif

		/**
		 * \brief 添加一个连接任务
		 * \param task 连接任务
		 */
		void _add(zTCPTask *task)
		{
			Zebra::logger->trace("zVerifyThread::_add");
#ifdef _USE_EPOLL_
			task->addEpoll(kdpfd, EPOLLIN | EPOLLERR | EPOLLPRI, (void *)task);
			tasks.push_back(task);
			task_count = tasks.size();;
			if (task_count > epfds.size())
			{
				epfds.resize(task_count + 16);
			}
#else
			struct pollfd pfd;
			task->fillPollFD(pfd, POLLIN | POLLERR | POLLPRI);
			tasks.push_back(task);
			task_count = tasks.size();;
			pfds.push_back(pfd);
#endif
		}

#ifdef _USE_EPOLL_
		void remove(zTCPTask *task)
		{
			Zebra::logger->trace("zVerifyThread::remove(EPOLL)");
			task->delEpoll(kdpfd, EPOLLIN | EPOLLERR | EPOLLPRI);
			tasks.remove(task);
			task_count = tasks.size();
		}
		void remove(zTCPTask_IT &it)
		{
			(*it)->delEpoll(kdpfd, EPOLLIN | EPOLLERR | EPOLLPRI);
			tasks.erase(it);
			task_count = tasks.size();
		}
#else
		void remove(zTCPTask_IT &it, int p)
		{
			int i;
			pollfdContainer::iterator iter;
			for(iter = pfds.begin(), i = 0; iter != pfds.end(); iter++, i++)
			{
				if (i == p)
				{
					pfds.erase(iter);
					tasks.erase(it);
					task_count = tasks.size();
					break;
				}
			}
		}
#endif

	public:

		/**
		 * \brief 构造函数
		 * \param pool 所属的连接池
		 * \param name 线程名称
		 */
		zVerifyThread(
				zTCPTaskPool *pool,
				const std::string &name = std::string("zVerifyThread"))
			: zThread(name), pool(pool)
		{
			task_count = 0;
#ifdef _USE_EPOLL_
			kdpfd = epoll_create(256);
			assert(-1 != kdpfd);
			epfds.resize(256);
#endif
		}

		/**
		 * \brief 析构函数
		 *
		 */
		~zVerifyThread()
		{
#ifdef _USE_EPOLL_
			TEMP_FAILURE_RETRY(::close(kdpfd));
#endif
		}

		void run();

};

/**
 * \brief 等待接受验证指令，并进行验证
 *
 */
void zVerifyThread::run()
{
	Zebra::logger->trace("zVerifyThread::run");
#ifdef _USE_EPOLL_
	zRTime currentTime;
	zTCPTask_IT it, next;

	while(!isFinal())
	{
		currentTime.now();

		check_queue();
		if (!tasks.empty())
		{
			for(it = tasks.begin(), next = it, next++; it != tasks.end(); it = next, next++)
			{
				zTCPTask *task = *it;
				if (task->checkVerifyTimeout(currentTime))
				{
					//超过指定时间验证还没有通过，需要回收连接
					remove(it);
					task->resetState();
					pool->addRecycle(task);
				}
			}

			int retcode = epoll_wait(kdpfd, &epfds[0], task_count, 0);
			if (retcode > 0)
			{
				for(int i = 0; i < retcode; i++)
				{
					zTCPTask *task = (zTCPTask *)epfds[i].data.ptr;
					if (epfds[i].events & (EPOLLERR | EPOLLPRI))
					{
						//套接口出现错误
						Zebra::logger->error("套接口错误");
						remove(task);
						task->resetState();
						pool->addRecycle(task);
					}
					else if (epfds[i].events & EPOLLIN)
					{
						switch(task->verifyConn())
						{
							case 1:
								//验证成功
								remove(task);
								//再做唯一性验证
								if (task->uniqueAdd())
								{
									//唯一性验证成功，获取下一个状态
									Zebra::logger->debug("客户端唯一性验证成功(%s:%u)",task->getIP(),task->getPort());
									task->setUnique();
									pool->addSync(task);
								}
								else
								{
									//唯一性验证失败，回收连接任务
									Zebra::logger->debug("客户端唯一性验证失败(%s:%u)",task->getIP(),task->getPort());
									task->resetState();
									pool->addRecycle(task);
								}
								break;
							case 0:
								//超时，下面会处理
								break;
							case -1:
								//验证失败，回收任务
								Zebra::logger->debug("客户端连接验证失败(%s:%u)",task->getIP(),task->getPort());
								remove(task);
								task->resetState();
								pool->addRecycle(task);
								break;
						}
					}
				}
			}
		}

		zThread::msleep(50);
	}

	//把所有等待验证队列中的连接加入到回收队列中，回收这些连接
	for(it = tasks.begin(), next = it, next++; it != tasks.end(); it = next, next++)
	{
		zTCPTask *task = *it;
		remove(it);
		task->resetState();
		pool->addRecycle(task);
	}
#else
	zRTime currentTime;
	zTCPTask_IT it, next;
	pollfdContainer::size_type i;

	while(!isFinal())
	{
		currentTime.now();

		check_queue();
		if (!pfds.empty())
		{
			for(i = 0, it = tasks.begin(), next = it, next++; it != tasks.end(); it = next, next++, i++)
			{
				zTCPTask *task = *it;
				if (task->checkVerifyTimeout(currentTime))
				{
					//超过指定时间验证还没有通过，需要回收连接
					remove(it, i--);
					task->resetState();
					pool->addRecycle(task);
				}
			}

			for(i = 0; i < pfds.size(); i++);
			{
				pfds[i].revents = 0;
			}

			if (TEMP_FAILURE_RETRY(::poll(&pfds[0], pfds.size(), 0)) > 0)
			{
				for(i = 0, it = tasks.begin(), next = it, next++; it != tasks.end(); it = next, next++, i++)
				{
					zTCPTask *task = *it;
					if (pfds[i].revents & (POLLERR | POLLPRI))
					{
						//套接口出现错误
						remove(it, i--);
						task->resetState();
						pool->addRecycle(task);
					}
					else if (pfds[i].revents & POLLIN)
					{
						switch(task->verifyConn())
						{
							case 1:
								//验证成功
								remove(it, i--);
								//再做唯一性验证
								if (task->uniqueAdd())
								{
									//唯一性验证成功，获取下一个状态
									Zebra::logger->debug("客户端唯一性验证成功");
									task->setUnique();
									pool->addSync(task);
								}
								else
								{
									//唯一性验证失败，回收连接任务
									Zebra::logger->debug("客户端唯一性验证失败");
									task->resetState();
									pool->addRecycle(task);
								}
								break;
							case 0:
								//超时，下面会处理
								break;
							case -1:
								//验证失败，回收任务
								remove(it, i--);
								task->resetState();
								pool->addRecycle(task);
								break;
						}
					}
				}
			}
		}

		zThread::msleep(50);
	}

	//把所有等待验证队列中的连接加入到回收队列中，回收这些连接
	for(i = 0, it = tasks.begin(), next = it, next++; it != tasks.end(); it = next, next++, i++)
	{
		zTCPTask *task = *it;
		remove(it, i--);
		task->resetState();
		pool->addRecycle(task);
	}
#endif
}

/**
 * \brief 等待其它线程同步验证这个连接，如果失败或者超时，都需要回收连接
 *
 */
class zSyncThread : public zThread, public zTCPTaskQueue
{

	private:

		zTCPTaskPool *pool;
		zTCPTaskContainer tasks;	/**< 任务列表 */

		void _add(zTCPTask *task)
		{
			tasks.push_front(task);
		}

	public:

		/**
		 * \brief 构造函数
		 * \param pool 所属的连接池
		 * \param name 线程名称
		 */
		zSyncThread(
				zTCPTaskPool *pool,
				const std::string &name = std::string("zSyncThread"))
			: zThread(name), pool(pool)
			{}

		/**
		 * \brief 析构函数
		 *
		 */
		~zSyncThread() {};

		void run();

};

/**
 * \brief 等待其它线程同步验证这个连接
 *
 */
void zSyncThread::run()
{
	Zebra::logger->trace("zSyncThread::run");
	zTCPTask_IT it;

	while(!isFinal())
	{
		check_queue();

		if (!tasks.empty())
		{
			for(it = tasks.begin(); it != tasks.end();)
			{
				zTCPTask *task = *it;
				switch(task->waitSync())
				{
					case 1:
						//等待其它线程同步验证成功
						it = tasks.erase(it);
						if (!pool->addOkay(task))
						{
							task->resetState();
							pool->addRecycle(task);
						}
						break;
					case 0:
						it++;
						break;
					case -1:
						//等待其它线程同步验证失败，需要回收连接
						it = tasks.erase(it);
						task->resetState();
						pool->addRecycle(task);
						break;
				}
			}
		}

		zThread::msleep(200);
	}

	//把所有等待同步验证队列中的连接加入到回收队列中，回收这些连接
	for(it = tasks.begin(); it != tasks.end();)
	{
		zTCPTask *task = *it;
		it = tasks.erase(it);
		task->resetState();
		pool->addRecycle(task);
	}
}

/**
 * \brief TCP连接的主处理线程，一般一个线程带几个TCP连接，这样可以显著提高效率
 *
 */
class zOkayThread : public zThread, public zTCPTaskQueue
{

	private:

		zTCPTaskPool *pool;
		zTCPTaskContainer tasks;	/**< 任务列表 */
		zTCPTaskContainer::size_type task_count;			/**< tasks计数(保证线程安全*/
#ifdef _USE_EPOLL_
		int kdpfd;
		epollfdContainer epfds;
#else
		pollfdContainer pfds;
#endif

		void _add(zTCPTask *task)
		{
#ifdef _USE_EPOLL_
			task->addEpoll(kdpfd, EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLPRI, (void *)task);
			tasks.push_back(task);
			task_count = tasks.size();
			if (task_count > epfds.size())
			{
				epfds.resize(task_count + 16);
			}
#else
			struct pollfd pfd;
			task->fillPollFD(pfd, POLLIN | POLLOUT | POLLERR | POLLPRI);
			tasks.push_back(task);
			task_count = tasks.size();
			pfds.push_back(pfd);
#endif
			task->ListeningRecv(false);
		}

#ifdef _USE_EPOLL_
		void remove(zTCPTask_IT &it)
		{
			(*it)->delEpoll(kdpfd, EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLPRI);
			tasks.erase(it);
			task_count = tasks.size();
		}
#else
		void remove(zTCPTask_IT &it, int p)
		{
			int i;
			pollfdContainer::iterator iter;
			for(iter = pfds.begin(), i = 0; iter != pfds.end(); iter++, i++)
			{
				if (i == p)
				{
					pfds.erase(iter);
					tasks.erase(it);
					task_count = tasks.size();
					break;
				}
			}
		}
#endif

	public:

		static const zTCPTaskContainer::size_type connPerThread = 512;	/**< 每个线程带的连接数量 */

		/**
		 * \brief 构造函数
		 * \param pool 所属的连接池
		 * \param name 线程名称
		 */
		zOkayThread(
				zTCPTaskPool *pool,
				const std::string &name = std::string("zOkayThread"))
			: zThread(name), pool(pool)
		{
			task_count = 0;
#ifdef _USE_EPOLL_
			kdpfd = epoll_create(connPerThread);
			assert(-1 != kdpfd);
			epfds.resize(connPerThread);
#endif
		}

		/**
		 * \brief 析构函数
		 *
		 */
		~zOkayThread()
		{
#ifdef _USE_EPOLL_
			TEMP_FAILURE_RETRY(::close(kdpfd));
#endif
		}

		void run();

		/**
		 * \brief 返回连接任务的个数
		 * \return 这个线程处理的连接任务数
		 */
		const zTCPTaskContainer::size_type size() const
		{
			return task_count + _size;
		}

};

/**
 * \brief 主处理线程，回调处理连接的输入输出指令
 *
 */
void zOkayThread::run()
{
	Zebra::logger->trace("zOkayThread::run");
#ifdef _USE_EPOLL_
	zRTime currentTime;
	zTCPTask_IT it, next;

	int kdpfd_r;
	epollfdContainer epfds_r;
	kdpfd_r = epoll_create(256);
	assert(-1 != kdpfd_r);
	epfds.resize(256);
	DWORD fds_count_r = 0;
	int time = pool->usleep_time;
	bool check=false;
	while(!isFinal())
	{
		currentTime.now();

		if (check) 
		{
			check_queue();
			if (!tasks.empty())
			{
				for(it = tasks.begin(), next = it, next++; it != tasks.end(); it = next, next++)
				{
					zTCPTask *task = *it;

					//检查测试信号指令
					task->checkSignal(currentTime);

					if (task->isTerminateWait())
					{
						task->Terminate();
					}
					if (task->isTerminate())
					{
						if (task->isFdsrAdd())
						{
							task->delEpoll(kdpfd_r, EPOLLIN | EPOLLERR | EPOLLPRI);
							fds_count_r --;
						}
						remove(it);
						// state_sync -> state_okay
						/*
						 * whj
						 * 先设置状态再添加容器,
						 * 否则会导致一个task同时在两个线程中的危险情况
						 */
						task->getNextState();
						pool->addRecycle(task);
					}
					else
					{
						if(!task->isFdsrAdd())
						{
							task->addEpoll(kdpfd_r, EPOLLIN | EPOLLERR | EPOLLPRI, (void *)task);
							task->fdsrAdd(); 
							fds_count_r++;
							if (fds_count_r > epfds_r.size())
							{
								epfds_r.resize(fds_count_r + 16);
							}
						}
					}
				}
			}
			check=false;
		}

		if(fds_count_r)
		{
			struct timeval _tv_1;
			struct timeval _tv_2;
			gettimeofday(&_tv_1,NULL);
			int retcode = epoll_wait(kdpfd_r, &epfds_r[0], fds_count_r, time/1000);
			if (retcode > 0)
			{
				for(int i = 0; i < retcode; i++)
				{
					zTCPTask *task = (zTCPTask *)epfds_r[i].data.ptr;
					if (epfds_r[i].events & (EPOLLERR | EPOLLPRI))
					{
						//套接口出现错误
						Zebra::logger->debug("%s: 套接口异常错误", __PRETTY_FUNCTION__);
						task->Terminate(zTCPTask::terminate_active);
						check=true;
					}
					else
					{
						if (epfds_r[i].events & EPOLLIN)
						{
							//套接口准备好了读取操作
							if (!task->ListeningRecv(true))
							{
								Zebra::logger->debug("%s: 套接口读操作错误", __PRETTY_FUNCTION__);
								task->Terminate(zTCPTask::terminate_active);
								check=true;
							}
						}
					}
					epfds_r[i].events=0; 
				}
			}
			gettimeofday(&_tv_2,NULL);
			int end=_tv_2.tv_sec*1000000 + _tv_2.tv_usec;
			int begin= _tv_1.tv_sec*1000000 + _tv_1.tv_usec;
			time = time - (end - begin);
		}
		else
		{
			zThread::usleep(time);
			time = 0;
		}
		if(check)
		{
			if(time <=0)
			{
				time = 0;
			}
			continue;
		}
		if (time <=0)
		{
			if (!tasks.empty())
			{
				int retcode = epoll_wait(kdpfd, &epfds[0], task_count, 0);
				if (retcode > 0)
				{
					for(int i = 0; i < retcode; i++)
					{
						zTCPTask *task = (zTCPTask *)epfds[i].data.ptr;
						if (epfds[i].events & (EPOLLERR | EPOLLPRI))
						{
							//套接口出现错误
							Zebra::logger->debug("%s: 套接口异常错误", __PRETTY_FUNCTION__);
							task->Terminate(zTCPTask::terminate_active);
						}
						else
						{
							if (epfds[i].events & EPOLLIN)
							{
								//套接口准备好了读取操作
								if (!task->ListeningRecv(true))
								{
									Zebra::logger->debug("%s: 套接口读操作错误", __PRETTY_FUNCTION__);
									task->Terminate(zTCPTask::terminate_active);
								}
							}
							if (epfds[i].events & EPOLLOUT)
							{
								//套接口准备好了写入操作
								if (!task->ListeningSend())
								{
									Zebra::logger->debug("%s: 套接口写操作错误", __PRETTY_FUNCTION__);
									task->Terminate(zTCPTask::terminate_active);
								}
							}
						}
						epfds[i].events=0; 
					}
				}
				time = pool->usleep_time;
			}
			check=true;
		}

		//zThread::usleep(pool->usleep_time);
	}

	//把所有任务队列中的连接加入到回收队列中，回收这些连接
	for(it = tasks.begin(), next = it, next++; it != tasks.end(); it = next, next++)
	{
		zTCPTask *task = *it;
		remove(it);
		// state_sync -> state_okay
		/*
		 * whj
		 * 先设置状态再添加容器,
		 * 否则会导致一个task同时在两个线程中的危险情况
		 */
		task->getNextState();
		pool->addRecycle(task);
	}
	TEMP_FAILURE_RETRY(::close(kdpfd_r));
#else
	zRTime currentTime;
	zTCPTask_IT it, next;
	pollfdContainer::size_type i;

	int time = pool->usleep_time;
	pollfdContainer::iterator iter_r;
	pollfdContainer pfds_r;
	zTCPTaskContainer tasks_r;    
	bool check=false;
	while(!isFinal())
	{
		currentTime.now();

		check_queue();
		if(check)
		{
			if (!pfds.empty())
			{
				for(i = 0, it = tasks.begin(), next = it, next++; it != tasks.end(); it = next, next++, i++)
				{
					zTCPTask *task = *it;
					//检查测试信号指令
					task->checkSignal(currentTime);

					if (task->isTerminateWait())
					{
						task->Terminate();
					}
					if (task->isTerminate())
					{
						unsigned int j=0;
						for(iter_r = pfds_r.begin(); iter_r != pfds_r.end(); iter_r++, j++)
						{
							if (i == j)
							{
								pfds_r.erase(iter_r);
								//zTCPTaskContainer::iterator iter_del = tasks_r.find(task);
								//if(iter_del)
								//tasks_r.erase(iter_del);
								tasks_r.remove(task);
								break;
							}
						}
						remove(it, i--);
						// state_sync -> state_okay
						/*
						 * whj
						 * 先设置状态再添加容器,
						 * 否则会导致一个task同时在两个线程中的危险情况
						 */
						task->getNextState();
						pool->addRecycle(task);
					}
					else
					{
						pfds[i].revents = 0;
					}
				}
			}
			check=false;
		}

		if(!tasks_r.empty())
		{
			struct timeval _tv_1;
			struct timeval _tv_2;
			gettimeofday(&_tv_1,NULL);
			int retcode = TEMP_FAILURE_RETRY(::poll(&pfds_r[0], pfds_r.size(), time/1000));
			if (retcode > 0)
			{
				for(i = 0, it = tasks_r.begin(); it != tasks_r.end(); it++, i++)
				{
					zTCPTask *task = *it;
					if (pfds_r[i].revents & (POLLERR | POLLPRI))
					{
						//套接口出现错误
						Zebra::logger->debug("%s: 套接口异常错误", __PRETTY_FUNCTION__);
						task->Terminate(zTCPTask::terminate_active);
						check=true;
					}
					else
					{
						if (pfds_r[i].revents & POLLIN)
						{
							//套接口准备好了读取操作
							if (!task->ListeningRecv(true))
							{
								Zebra::logger->debug("%s: 套接口读操作错误", __PRETTY_FUNCTION__);
								task->Terminate(zTCPTask::terminate_active);
								check=true;
							}
						}
					}
					pfds_r[i].revents=0; 
				}
			}
			gettimeofday(&_tv_2,NULL);
			int end=_tv_2.tv_sec*1000000 + _tv_2.tv_usec;
			int begin= _tv_1.tv_sec*1000000 + _tv_1.tv_usec;
			time = time - (end - begin);
		}
		else
		{
			zThread::usleep(time);
			time = 0;
		}
		if(check)
		{
			if(time <=0)
			{
				time = 0;
			}
			continue;
		}
		if(time <=0/* && !tasks.empty()*/)
		{
			if (!pfds.empty())
			{
				if (TEMP_FAILURE_RETRY(::poll(&pfds[0], pfds.size(), 0)) > 0)
				{
					for(i = 0, it = tasks.begin(); it != tasks.end(); it++, i++)
					{
						zTCPTask *task = *it;
						if (pfds[i].revents & (POLLERR | POLLPRI))
						{
							//套接口出现错误
							Zebra::logger->debug("%s: 套接口异常错误", __PRETTY_FUNCTION__);
							task->Terminate(zTCPTask::terminate_active);
						}
						else
						{
							if (pfds[i].revents & POLLIN)
							{
								//套接口准备好了读取操作
								if (!task->ListeningRecv(true))
								{
									Zebra::logger->debug("%s: 套接口读操作错误", __PRETTY_FUNCTION__);
									task->Terminate(zTCPTask::terminate_active);
								}
							}
							if (pfds[i].revents & POLLOUT)
							{
								//套接口准备好了写入操作
								if (!task->ListeningSend())
								{
									Zebra::logger->debug("%s: 套接口写操作错误", __PRETTY_FUNCTION__);
									task->Terminate(zTCPTask::terminate_active);
								}
							}
						}
						pfds[i].revents=0; 
					}
				}
			}
			time = pool->usleep_time;
		}
		check=true;

		//zThread::usleep(pool->usleep_time);
	}

	//把所有任务队列中的连接加入到回收队列中，回收这些连接
	for(i = 0, it = tasks.begin(), next = it, next++; it != tasks.end(); it = next, next++, i++)
	{
		zTCPTask *task = *it;
		remove(it, i--);
		// state_sync -> state_okay
		/*
		 * whj
		 * 先设置状态再添加容器,
		 * 否则会导致一个task同时在两个线程中的危险情况
		 */
		task->getNextState();
		pool->addRecycle(task);
	}
#endif
}

/**
 * \brief 连接回收线程，回收所有无用的TCP连接，释放相应的资源
 *
 */
class zRecycleThread : public zThread, public zTCPTaskQueue
{

	private:

		zTCPTaskPool *pool;
		zTCPTaskContainer tasks;	/**< 任务列表 */

		void _add(zTCPTask *task)
		{
			tasks.push_front(task);
		}

	public:

		/**
		 * \brief 构造函数
		 * \param pool 所属的连接池
		 * \param name 线程名称
		 */
		zRecycleThread(
				zTCPTaskPool *pool,
				const std::string &name = std::string("zRecycleThread"))
			: zThread(name), pool(pool)
			{}

		/**
		 * \brief 析构函数
		 *
		 */
		~zRecycleThread() {};

		void run();

};

/**
 * \brief 连接回收处理线程，在删除内存空间之前需要保证recycleConn返回1
 *
 */
void zRecycleThread::run()
{
	Zebra::logger->trace("zRecycleThread::run");
	zTCPTask_IT it;

	while(!isFinal())
	{
		check_queue();

		if (!tasks.empty())
		{
			for(it = tasks.begin(); it != tasks.end();)
			{
				zTCPTask *task = *it;
				switch(task->recycleConn())
				{
					case 1:
						//回收处理完成可以释放相应的资源
						it = tasks.erase(it);
						if (task->isUnique())
							//如果已经通过了唯一性验证，从全局唯一容器中删除
							task->uniqueRemove();
						task->getNextState();
						SAFE_DELETE(task);
						break;
					case 0:
						//回收超时，下次再处理
						it++;
						break;
				}
			}
		}

		zThread::msleep(200);
	}

	//回收所有的连接
	for(it = tasks.begin(); it != tasks.end();)
	{
		//回收处理完成可以释放相应的资源
		zTCPTask *task = *it;
		it = tasks.erase(it);
		if (task->isUnique())
			//如果已经通过了唯一性验证，从全局唯一容器中删除
			task->uniqueRemove();
		task->getNextState();
		SAFE_DELETE(task);
	}
}


/**
 * \brief 返回连接池中子连接个数
 *
 */
const int zTCPTaskPool::getSize()
{
//	Zebra::logger->trace("zTCPTaskPool::getSize");
	struct MyCallback : zThreadGroup::Callback
	{
		int size;
		MyCallback() : size(0) {}
		void exec(zThread *e)
		{
			zOkayThread *pOkayThread = (zOkayThread *)e;
			size += pOkayThread->size();
		}
	};
	MyCallback mcb;
	okayThreads.execAll(mcb);
	return mcb.size;
}

/**
 * \brief 把一个TCP连接添加到验证队列中，因为存在多个验证队列，需要按照一定的算法添加到不同的验证处理队列中
 *
 * \param task 一个连接任务
 */
bool zTCPTaskPool::addVerify(zTCPTask *task)
{

	Zebra::logger->trace("zTCPTaskPool::addVerify");
	//因为存在多个验证队列，需要按照一定的算法添加到不同的验证处理队列中
	static unsigned int hashcode = 0;
	zVerifyThread *pVerifyThread = (zVerifyThread *)verifyThreads.getByIndex(hashcode++ % maxVerifyThreads);
	if (pVerifyThread)
	{
		// state_sync -> state_okay
		/*
		 * whj
		 * 先设置状态再添加容器,
		 * 否则会导致一个task同时在两个线程中的危险情况
		 */
		task->getNextState();
		pVerifyThread->add(task);
	}
	return true;
}

/**
 * \brief 把一个通过验证的TCP连接添加到等待同步验证队列中
 *
 * \param task 一个连接任务
 */
void zTCPTaskPool::addSync(zTCPTask *task)
{
	Zebra::logger->trace("zTCPTaskPool::addSync");
	// state_sync -> state_okay
	/*
	 * whj
	 * 先设置状态再添加容器,
	 * 否则会导致一个task同时在两个线程中的危险情况
	 */
	task->getNextState();
	syncThread->add(task);
}

/**
 * \brief 把一个通过验证的TCP处理队列中
 *
 * \param task 一个连接任务
 * \return 添加是否成功
 */
bool zTCPTaskPool::addOkay(zTCPTask *task)
{
	Zebra::logger->trace("zTCPTaskPool::addOkay");
	//首先便利所有的线程，找出运行的并且连接数最少的线程，再找出没有启动的线程
	zOkayThread *min = NULL, *nostart = NULL;
	for(int i = 0; i < maxThreadCount; i++)
	{
		zOkayThread *pOkayThread = (zOkayThread *)okayThreads.getByIndex(i);
		if (pOkayThread)
		{
			if (pOkayThread->isAlive())
			{
				if (NULL == min || min->size() > pOkayThread->size())
					min = pOkayThread;
			}
			else
			{
				nostart = pOkayThread;
				break;
			}
		}
	}
	if (min && min->size() < zOkayThread::connPerThread)
	{
		// state_sync -> state_okay
		/*
		 * whj
		 * 先设置状态再添加容器,
		 * 否则会导致一个task同时在两个线程中的危险情况
		 */
		task->getNextState();
		//这个线程同时处理的连接数还没有到达上限
		min->add(task);
		return true;
	}
	if (nostart)
	{
		//线程还没有运行，需要创建线程，再把添加到这个线程的处理队列中
		if (nostart->start())
		{
			Zebra::logger->debug("zTCPTaskPool创建工作线程");
			// state_sync -> state_okay
			/*
			 * whj
			 * 先设置状态再添加容器,
			 * 否则会导致一个task同时在两个线程中的危险情况
			 */
			task->getNextState();
			//这个线程同时处理的连接数还没有到达上限
			nostart->add(task);
			return true;
		}
		else
			Zebra::logger->fatal("zTCPTaskPool不能创建工作线程");
	}

	Zebra::logger->fatal("zTCPTaskPool没有找到合适的线程来处理连接");
	//没有找到线程来处理这个连接，需要回收关闭连接
	return false;
}

/**
 * \brief 把一个TCP连接添加到回收处理队列中
 *
 * \param task 一个连接任务
 */
void zTCPTaskPool::addRecycle(zTCPTask *task)
{
	Zebra::logger->trace("zTCPTaskPool::addRecycle");
	recycleThread->add(task);
}


/**
 * \brief 初始化线程池，预先创建各种线程
 *
 * \return 初始化是否成功
 */
bool zTCPTaskPool::init()
{
	Zebra::logger->trace("zTCPTaskPool::init");
	//创建初始化验证线程
	for(int i = 0; i < maxVerifyThreads; i++)
	{
		std::ostringstream name;
		name << "zVerifyThread[" << i << "]";
		zVerifyThread *pVerifyThread = new zVerifyThread(this, name.str());
		if (NULL == pVerifyThread)
			return false;
		if (!pVerifyThread->start())
			return false;
		verifyThreads.add(pVerifyThread);
	}

	//创建初始化等待同步验证现程
	syncThread = new zSyncThread(this);
	if (syncThread && !syncThread->start())
		return false;

	//创建初始化主运行线程池
	maxThreadCount = (maxConns + zOkayThread::connPerThread - 1) / zOkayThread::connPerThread;
	Zebra::logger->debug("线程最大连接数容量%d,每线程连接数量%d,线程个数%d",maxConns,zOkayThread::connPerThread,maxThreadCount);
	for(int i = 0; i < maxThreadCount; i++)
	{
		std::ostringstream name;
		name << "zOkayThread[" << i << "]";
		zOkayThread *pOkayThread = new zOkayThread(this, name.str());
		if (NULL == pOkayThread)
			return false;
		if (i < minThreadCount && !pOkayThread->start())
			return false;
		okayThreads.add(pOkayThread);
	}

	//创建初始化回收线程池
	recycleThread = new zRecycleThread(this);
	if (recycleThread && !recycleThread->start())
		return false;

	return true;
}

/**
 * \brief 释放线程池，释放各种资源，等待各种线程退出
 *
 */
void zTCPTaskPool::final()
{
	Zebra::logger->trace("zTCPTaskPool::final");	
	verifyThreads.joinAll();

	if (syncThread)
	{
		syncThread->final();
		syncThread->join();
		SAFE_DELETE(syncThread);
	}

	okayThreads.joinAll();

	if (recycleThread)
	{
		recycleThread->final();
		recycleThread->join();
		SAFE_DELETE(recycleThread);
	}
}

