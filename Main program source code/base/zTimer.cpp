#include "zTimer.h"
#include <algorithm>

/**
 * \brief 计时器构造函数
 *
 * \param name 计时器名字，如果不添，默认为"Timer"
 */
zTimer::zTimer(const std::string &name): taskqueue(),thread(name,&taskqueue)
{
	taskqueue.canAddTask=true;
	thread.start();
}

/**
 * \brief 计时器析构函数
 *
 */
zTimer::~zTimer()
{
	cancel();
}

int zTimer::id(0);

/**
 * \brief 得到一个数字ID ,用于自动分配计时器名字 
 *
 * \return ID 
 */
int zTimer::getID()
{
	return (id++);
}

/**
 * \brief 停掉这个计时器所有的任务
 *
 */
void zTimer::cancel()
{
	taskqueue.qmutex.lock();
	taskqueue.canAddTask=false;
	for(int i=0;i<taskqueue.size();i++)
	{
		taskqueue.queue[i]->cancel();
	}
	taskqueue.qmutex.unlock();
}

/**
 * \brief 添加一个一次性任务
 * \param task 要执行的任务 
 * \param tv 任务的执行时间 
 * \return 添加是否成功
 */
bool zTimer::schedule(zTimerTask *task, zRTime &tv)
{
	return addTask(task,tv,0);
}

/**
 * \brief 添加一个一次性任务
 * \param task 要执行的任务 
 * \param delay delay毫秒后执行任务 
 * \return 添加是否成功
 */
bool zTimer::schedule(zTimerTask *task, int delay)
{
	zRTime tv(delay);
	return addTask(task,tv,0);
}

/**
 * \brief 添加一个周期任务,延迟模式
 *
 * 关于延迟模式：
 *
 * 延迟模式指第一次执行任务开始到第二次执行任务开始时的间隔是固定的，为period
 * \param task 要执行的任务 
 * \param delay delay毫秒后开始执行任务 
 * \param period 任务周期,单位毫秒
 * \return 添加是否成功
 */
bool zTimer::scheduleAtDelay(zTimerTask *task, int delay ,int period)
{
	if(delay < 0 || period < 0) return false;
	zRTime tv(delay);
	return addTask(task,tv,-period);
}

/**
 * \brief 添加一个周期任务,延迟模式
 *
 * 关于延迟模式：
 *
 * 延迟模式指第一次执行任务开始到第二次执行任务开始时的间隔是固定的，为period
 * \param task 要执行的任务 
 * \param tv 任务开始执行的时间 
 * \param period 任务周期,单位毫秒
 * \return 添加是否成功
 */
bool zTimer::scheduleAtDelay(zTimerTask *task, zRTime &tv,int period)
{
	if(period<0) return false;
	return addTask(task,tv,-period);
}

/**
 * \brief 添加一个周期任务,频率模式
 *
 * 关于频率模式：
 *
 * 频率模式指每次执行任务的时间频率是固定的，为period
 * \param task 要执行的任务 
 * \param delay delay毫秒后开始执行任务 
 * \param period 任务周期,单位毫秒
 * \return 添加是否成功
 */
bool zTimer::scheduleAtRate(zTimerTask *task, int delay ,int period)
{
	if(delay < 0 || period < 0) return false;
	zRTime ctv(delay);
	return addTask(task,ctv,period);
}

/**
 * \brief 添加一个周期任务,频率模式
 *
 * 关于频率模式：
 *
 * 频率模式指每次执行任务的时间频率是固定的，为period
 * \param task 要执行的任务 
 * \param tv 任务开始执行的时间 
 * \param period 任务周期,单位毫秒
 * \return 添加是否成功
 */
bool zTimer::scheduleAtRate(zTimerTask *task, zRTime &tv,int period)
{
	if(period<0) return false;
	return addTask(task,tv,period);
}

/**
 * \brief 添加任务
 *
 * \param task 任务指针 
 * \param tv 从何时开始执行任务 
 * \param period 执行周期，如果为0表示一次性任务
 * \return  是否添加成功
 */
bool zTimer::addTask(zTimerTask *task, zRTime &tv,int period)
{
	taskqueue.qmutex.lock();
	if(!taskqueue.canAddTask)
	{
		taskqueue.qmutex.unlock();
		return false;
	}

	if(task->state!=zTimerTask::VIRGIN)
	{
		taskqueue.qmutex.unlock();
		return false;
	}

	//初始化timerTask
	task->nextExecTime=tv;
	task->period=period;
	task->state=zTimerTask::SCHEDULED;

	//添加
	taskqueue.push(task);
	taskqueue.qmutex.unlock();
	return true;
}


/**
 * \brief 任务线程析构函数
 *
 */
zTimer::zTimerThread::~zTimerThread()
{
}

/**
 * \brief 任务线程析构函数
 *
 * \param name 线程名字 
 * \param queue 任务队列 
 */
zTimer::zTimerThread::zTimerThread(const std::string & name,zTimerQueue *queue)
:zThread(name,false)
{
	taskQueue=queue;
}

/**
 * \brief 参见 <code>zThread::run</code>
 *
 * 计时器执行函数，如果任务执行完毕，会把他从队列中删除
 */
void zTimer::zTimerThread::run()
{
	while(!isFinal())
	{

		taskQueue->qmutex.lock();
		if(taskQueue->empty() && !taskQueue->canAddTask)
		{
			taskQueue->qmutex.unlock();
			break;
		}

		if(taskQueue->empty())
		{
			taskQueue->qmutex.unlock();
			usleep(1000);
		}
		else
		{
			// 得到最先执行的task
			zTimerTask *task=taskQueue->top();
			if(task==NULL)
			{
				taskQueue->pop();
				taskQueue->qmutex.unlock();
				continue;
			}

			task->taskMutex.lock();
			if(task->state==zTimerTask::CANCELLED)
			{
				taskQueue->pop();
				task->taskMutex.unlock();
				taskQueue->qmutex.unlock();
				continue;
			}
			zRTime ctv;
			zRTime etv(task->nextExecTime);
			bool taskFired=(ctv>etv);
			if(taskFired)//如果任务可以执行了
			{
				if(task->period==0)
				{
					task->state=zTimerTask::EXECUTED;
					taskQueue->pop();
				}
				else
				{
					//重新排列此任务
					taskQueue->pop();
					//Zebra::logger->debug("%lu", task->period);
					task->schedExecTime(task->period>0?ctv:etv);
					taskQueue->push(task);
				}
			}

			task->taskMutex.unlock();
			taskQueue->qmutex.unlock();

			if(taskFired)
			{
				task->run();
			}
			else if(etv > ctv)
			{
				usleep(1000);
			}
		}
	}
}

/**
 * \brief 计时器任务构造函数
 *
 */
zTimer::zTimerTask::zTimerTask():nextExecTime()
{
	period=0;
	state=VIRGIN;
}

/**
 * \brief 计时器任务析构函数
 *
 */
zTimer::zTimerTask::~zTimerTask()
{
}

/**
 * \brief 重载操作符 > ，比较两个#zTimerTask大小
 *
 */
bool zTimer::zTimerTask::operator > (const zTimerTask &right) const
{
	return this->nextExecTime > right.nextExecTime;
}

/**
 * \brief 取消这个任务
 * \return 计划任务被成功取消
 */
bool zTimer::zTimerTask::cancel()
{
	taskMutex.lock();
	bool result = (state == SCHEDULED);
	state = CANCELLED;
	taskMutex.unlock();
	return result;
}

/**
 * \brief 重新设定任务执行时间
 *
 * \param tv 任务被执行的时间= tv + period 
 */
void zTimer::zTimerTask::schedExecTime(const zRTime &tv)
{
	nextExecTime = tv;
	nextExecTime.addDelay(abs(period));
}
