/**
 * \file
 * \version  $Id: zThread.cpp  $
 * \author  
 * \date 
 * \brief 实现类zThread
 *
 * 
 */


#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <mysql.h>

#include "zMisc.h"
#include "zThread.h"

/**
 * \brief 线程函数
 *
 * 在函数体里面会调用线程类对象实现的回调函数
 *
 * \param arg 传入线程的参数
 * \return 返回线程结束信息
 */
void *zThread::threadFunc(void *arg)
{
	zThread *thread = (zThread *)arg;

	//Zebra::logger->debug("%s", __PRETTY_FUNCTION__);
	//初始化随机数
	Zebra::seedp = time(NULL);
	//Zebra::logger->debug("%s: %u", __FUNCTION__, seedp);
	thread->mlock.lock();
	thread->alive = true;
	thread->cond.broadcast();
	thread->mlock.unlock();

	//设置线程信号处理句柄
	sigset_t sig_mask;
	sigfillset(&sig_mask);
	pthread_sigmask(SIG_SETMASK, &sig_mask, NULL);

	mysql_thread_init();

	//运行线程的主回调函数
	thread->run();

	mysql_thread_end();

	thread->mlock.lock();
	thread->alive = false;
	thread->cond.broadcast();
	thread->mlock.unlock();

	//如果不是joinable，需要回收线程资源
	if (!thread->isJoinable())
	{
		thread->mlock.lock();
		while(thread->alive)
			thread->cond.wait(thread->mlock);
		thread->mlock.unlock();
		SAFE_DELETE(thread);
	}

	pthread_exit(NULL);
}

/**
 * \brief 创建线程，启动线程
 *
 * \return 创建线程是否成功
 */
bool zThread::start()
{
	//线程已经创建运行，直接返回
	if (alive)
	{
		Zebra::logger->warn("线程 %s 已经创建运行，还在尝试运行线程", getThreadName().c_str());
		return true;
	}

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	if (!joinable) pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	if (0 != ::pthread_create(&thread, NULL, zThread::threadFunc, this)) 
	{
		Zebra::logger->error("创建线程 %s 失败", getThreadName().c_str());
		return false;
	}

	pthread_attr_destroy(&attr);

	mlock.lock();
	while(!alive)
		cond.wait(mlock);
	mlock.unlock();

	//Zebra::logger->debug("创建线程 %s 成功", getThreadName().c_str());

	return true;
}

/**
 * \brief 等待一个线程结束
 *
 */
void zThread::join()
{
	//Zebra::logger->debug("%s", __PRETTY_FUNCTION__);

	if (0 != thread && joinable)
	{
		::pthread_join(thread, NULL);
		thread = 0;
		mlock.lock();
		while(alive)
			cond.wait(mlock);
		mlock.unlock();
	}
}

/**
 * \brief 构造函数
 *
 */
zThreadGroup::zThreadGroup() : vts(), rwlock()
{
}

/**
 * \brief 析构函数
 *
 */
zThreadGroup::~zThreadGroup()
{
	joinAll();
}

/**
 * \brief 添加一个线程到分组中
 * \param thread 待添加的线程
 */
void zThreadGroup::add(zThread *thread)
{
	zRWLock_scope_wrlock scope_wrlock(rwlock);
	Container::iterator it = std::find(vts.begin(), vts.end(), thread);
	if (it == vts.end())
		vts.push_back(thread);
}

/**
 * \brief 按照index下标获取线程
 * \param index 下标编号
 * \return 线程
 */
zThread *zThreadGroup::getByIndex(const Container::size_type index)
{
	zRWLock_scope_rdlock scope_rdlock(rwlock);
	if (index >= vts.size())
		return NULL;
	else
		return vts[index];
}

/**
 * \brief 重载[]运算符，按照index下标获取线程
 * \param index 下标编号
 * \return 线程
 */
zThread *zThreadGroup::operator[] (const Container::size_type index)
{
	zRWLock_scope_rdlock scope_rdlock(rwlock);
	if (index >= vts.size())
		return NULL;
	else
		return vts[index];
}

/**
 * \brief 等待分组中的所有线程结束
 */
void zThreadGroup::joinAll()
{
	zRWLock_scope_wrlock scope_wrlock(rwlock);
	while(!vts.empty())
	{
		zThread *pThread = vts.back();
		vts.pop_back();
		if (pThread)
		{
			pThread->final();
			pThread->join();
			SAFE_DELETE(pThread);
		}
	}
}

/**
 * \brief 对容器中的所有元素调用回调函数
 * \param cb 回调函数实例
 */
void zThreadGroup::execAll(Callback &cb)
{
	zRWLock_scope_rdlock scope_rdlock(rwlock);
	for(Container::iterator it = vts.begin(); it != vts.end(); ++it)
	{
		cb.exec(*it);
	}
}

