/**
 * \file
 * \version  $Id: GatewayTaskManager.cpp  $
 * \author 
 * \date 
 * \brief 管理子连接的容器
 *
 * 
 */


#include <iostream>

#include "GatewayTask.h"
#include "GatewayTaskManager.h"
#include "zXMLParser.h"

///网关连接管理器实例
GatewayTaskManager *GatewayTaskManager::instance = NULL;

GatewayTaskManager::GatewayTaskManager()
{
}

GatewayTaskManager::~GatewayTaskManager()
{
}

/**
 * \brief 向容器中添加一个子连接
 *
 * \param task 子连接任务
 * \return 添加是否成功
 */
bool GatewayTaskManager::uniqueAdd(GatewayTask *task)
{
	GatewayTaskHashmap_const_iterator it;
	rwlock.wrlock();
	it = gatewayTaskSet.find(task->getACCID());
	if (it != gatewayTaskSet.end())
	{
		rwlock.unlock();
		return false;
	}
	gatewayTaskSet.insert(GatewayTaskHashmap_pair(task->getACCID(), task));
	rwlock.unlock();
	return true;
}

/**
 * \brief 从容器中删除一个子连接
 *
 * \param task 子连接任务
 * \return 删除是否成功
 */
bool GatewayTaskManager::uniqueRemove(GatewayTask *task)
{
	GatewayTaskHashmap_iterator it;
	rwlock.wrlock();
	it = gatewayTaskSet.find(task->getACCID());
	if (it != gatewayTaskSet.end())
	{
		gatewayTaskSet.erase(it);
	}
	else
		Zebra::logger->warn(__PRETTY_FUNCTION__);
	rwlock.unlock();
	return true;
}

/**
 * \brief 设置某一个连接计费验证是否通过
 * \param accid 账号
 * \param ok 计费炎症是否通过
 */
void GatewayTaskManager::accountVerifyOK(const DWORD accid, const bool ok)
{
	GatewayTaskHashmap_iterator it;
	rwlock.rdlock();
	it = gatewayTaskSet.find(accid);
	if (it != gatewayTaskSet.end())
	{
		it->second->accountVerifyOK(ok);
	}
	rwlock.unlock();
}

/**
 * \brief 遍历容器中的所有元素，执行某一个操作
 * \param callback 待执行的回调函数
 */
void GatewayTaskManager::execAll(GatewayTaskCallback &callback)
{
	rwlock.rdlock();
	for(GatewayTaskHashmap_iterator it = gatewayTaskSet.begin(); it != gatewayTaskSet.end(); ++it)
	{
		if (!callback.exec(it->second))
			break;
	}
	rwlock.unlock();
}

