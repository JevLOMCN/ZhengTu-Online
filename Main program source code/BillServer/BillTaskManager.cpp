/**
 * \file
 * \version  $Id: BillTaskManager.cpp  $
 * \author  
 * \date 
 * \brief 管理子连接的容器
 *
 * 
 */


#include <iostream>

#include "BillTask.h"
#include "BillTaskManager.h"

BillTaskManager *BillTaskManager::instance = NULL;

/**
 * \brief 向唯一性验证容器中添加一个子连接任务
 *
 * \param task 子连接任务
 * \return 添加连接是否成功
 */
bool BillTaskManager::uniqueAdd(BillTask *task)
{
	BillTaskHashmap_const_iterator it;
	rwlock.wrlock();
	it = sessionTaskSet.find(task->getID());
	if (it != sessionTaskSet.end())
	{
		Zebra::logger->error(__PRETTY_FUNCTION__);
		rwlock.unlock();
		return false;
	}
	sessionTaskSet.insert(BillTaskHashmap_pair(task->getID(), task));
	rwlock.unlock();
	return true;
}

/**
 * \brief 从唯一性容器中移除一个子连接任务
 *
 * \param task 子连接任务
 * \return 移除是否成功
 */
bool BillTaskManager::uniqueRemove(BillTask *task)
{
	BillTaskHashmap_iterator it;
	rwlock.wrlock();
	it = sessionTaskSet.find(task->getID());
	if (it != sessionTaskSet.end())
	{
		sessionTaskSet.erase(it);
	}
	else
		Zebra::logger->warn(__PRETTY_FUNCTION__);
	rwlock.unlock();
	return true;
}

/**
 * \brief 根据服务器编号广播指令
 *
 * \param wdServerID 待广播指令的服务器编号
 * \param pstrCmd 待广播的指令
 * \param nCmdLen 指令长度
 * \return 广播是否成功
 */
bool BillTaskManager::broadcastByID(const WORD wdServerID, const void *pstrCmd, int nCmdLen)
{
	bool retval = true;
	BillTaskHashmap_iterator it;
	rwlock.rdlock();
	it = sessionTaskSet.find(wdServerID);
	if (it != sessionTaskSet.end())
	{
		retval = it->second->sendCmd(pstrCmd, nCmdLen);
	}
	rwlock.unlock();
	return retval;
}

/**
 * \brief 根据服务器编号查找task
 *
 * \param wdServerID 待查找的服务器编号
 * \return 广播是否成功
 */
BillTask *BillTaskManager::getTaskByID(const WORD wdServerID)
{
	BillTask *ret=NULL;
	BillTaskHashmap_iterator it;
	rwlock.rdlock();
	it = sessionTaskSet.find(wdServerID);
	if (it != sessionTaskSet.end())
	{
		ret = it->second;
	}
	rwlock.unlock();
	return ret;
}
void BillTaskManager::execEvery()
{
	BillTaskHashmap_iterator it;
	BillTask *task=NULL;
	rwlock.rdlock();
	it = sessionTaskSet.begin();
	for (; it != sessionTaskSet.end() ; it ++)
	{
		task=it->second;
		if(!task->checkRecycle())
		{
			task->doCmd();
		}
	}
	rwlock.unlock();

}
