/**
 * \file
 * \version  $Id: ServerManager.cpp  $
 * \author  
 * \date 
 * \brief 实现服务器管理容器
 *
 * 这个容器包括全局容器和唯一性验证容器
 * 
 */


#include <iostream>
#include <list>

#include "ServerTask.h"
#include "zMutex.h"
#include "zNoncopyable.h"
#include "ServerManager.h"
#include "Zebra.h"

ServerManager *ServerManager::instance = NULL;

/**
 * \brief 添加一个服务器连接任务到容器中
 *
 * \param task 服务器连接任务
 */
void ServerManager::addServer(ServerTask *task)
{
	Zebra::logger->trace("ServerManager::addServer()");
	if (task)
	{
		mutex.lock();		
		container.push_front(task);
		mutex.unlock();
	}
}

/**
 * \brief 从容器中删除一个服务器连接任务
 *
 * \param task 服务器连接任务
 */
void ServerManager::removeServer(ServerTask *task)
{
	Zebra::logger->trace("ServerManager::removeServer()");
	if (task)
	{
		mutex.lock();		
		container.remove(task);
		mutex.unlock();
	}
}

/**
 * \brief 根据编号查找一个服务器连接任务
 *
 * \param wdServerID 服务器编号
 * \return 返回适合的服务器连接任务
 */
ServerTask *ServerManager::getServer(WORD wdServerID)
{
	Zebra::logger->trace("ServerManager::getServer(serverid=%u)",wdServerID);
	Containter_const_iterator it;
	ServerTask *retval = NULL;

	mutex.lock();
	for(it = container.begin(); it != container.end(); it++)
	{
		if ((*it)->getID() == wdServerID)
		{
			retval = *it;
			break;
		}
	}
	mutex.unlock();

	return retval;
}

/**
 * \brief 把一个服务器连接任务添加到唯一性容器中
 *
 * \param task 服务器连接任务
 * \return 添加是否成功，也就是唯一性验证是否成功
 */
bool ServerManager::uniqueAdd(ServerTask *task)
{
	Zebra::logger->trace("ServerManager::uniqueVerify id=%u",task->getID());
	ServerTaskHashmap_const_iterator it;
	mutex.lock();
	it = taskUniqueContainer.find(task->getID());
	if (it != taskUniqueContainer.end())
	{
		Zebra::logger->error("%s", __PRETTY_FUNCTION__);
		mutex.unlock();
		return false;
	}
	taskUniqueContainer.insert(ServerTaskHashmap_pair(task->getID(), task));
	mutex.unlock();
	return true;
}

/**
 * \brief 验证这个服务器是否已经启动
 *
 * \param wdServerID 服务器编号
 * \return 验证是否成功
 */
bool ServerManager::uniqueVerify(const WORD wdServerID)
{
	Zebra::logger->trace("ServerManager::uniqueVerify wdServerID=%u",wdServerID);
	ServerTaskHashmap_const_iterator it;
	mutex.lock();
	it = taskUniqueContainer.find(wdServerID);
	if (it != taskUniqueContainer.end())
	{
		mutex.unlock();
		return false;
	}
	mutex.unlock();
	return true;
}

/**
 * \brief 从唯一性容器中删除一个连接任务
 *
 * \param task 服务器连接任务
 * \return 删除是否成功
 */
bool ServerManager::uniqueRemove(ServerTask *task)
{
	Zebra::logger->trace("ServerManager::uniqueRemove id=%u",task->getID());
	ServerTaskHashmap_iterator it;
	mutex.lock();
	it = taskUniqueContainer.find(task->getID());
	if (it != taskUniqueContainer.end())
	{
		taskUniqueContainer.erase(it);
	}
	else
		Zebra::logger->warn("%s", __PRETTY_FUNCTION__);
	mutex.unlock();
	return true;
}

/**
 * \brief 向容器中所有的服务器广播指令
 *
 * \param pstrCmd 待广播的指令
 * \param nCmdLen 指令长度
 * \return 广播是否成功
 */
bool ServerManager::broadcast(const void *pstrCmd, int nCmdLen)
{
	bool retval = true;

	mutex.lock();
	for(Containter_const_iterator it = container.begin(); it != container.end(); it++)
	{
		if (!(*it)->sendCmd(pstrCmd, nCmdLen))
			retval = false;
	}
	mutex.unlock();

	return retval;
}

/**
 * \brief 根据服务器编号广播指令
 *
 * \param wdServerID 待广播指令的服务器编号
 * \param pstrCmd 待广播的指令
 * \param nCmdLen 指令长度
 * \return 广播是否成功
 */
bool ServerManager::broadcastByID(const WORD wdServerID, const void *pstrCmd, int nCmdLen)
{
	Zebra::logger->trace("ServerManager::broadcastByID(serverid=%u)",wdServerID);
	bool retval = false;

	mutex.lock();
	for(Containter_const_iterator it = container.begin(); it != container.end(); it++)
	{
		if ((*it)->getID() == wdServerID)
		{
			retval = (*it)->sendCmd(pstrCmd, nCmdLen);
			break;
		}
	}
	mutex.unlock();

	return retval;
}

/**
 * \brief 根据服务器类型广播指令
 *
 * \param wdType 待广播指令的服务器类型
 * \param pstrCmd 待广播的指令
 * \param nCmdLen 指令长度
 * \return 广播是否成功
 */
bool ServerManager::broadcastByType(const WORD wdType, const void *pstrCmd, int nCmdLen)
{
	bool retval = true;

	mutex.lock();
	for(Containter_const_iterator it = container.begin(); it != container.end(); it++)
	{		
		if ((*it)->getType() == wdType
				&& !(*it)->sendCmd(pstrCmd, nCmdLen))
			retval = false;
	}
	mutex.unlock();

	return retval;
}

/**
 * \brief 统计一个区的在线人数
 * \return 得到一个区的当前总在线人数
 */
const DWORD ServerManager::caculateOnlineNum()
{
	DWORD retval = 0;

	mutex.lock();
	for(Containter_const_iterator it = container.begin(); it != container.end(); it++)
	{
		if ((*it)->getType() == GATEWAYSERVER)
			retval += (*it)->getOnlineNum();
	}
	mutex.unlock();

	return retval;
}

/**
 * \brief 收到notifyOther回复
 * \param srcID 源服务器编号
 * \param wdServerID 目的服务器编号
 */
void ServerManager::responseOther(const WORD srcID, const WORD wdServerID)
{
	Zebra::logger->trace("ServerManager::responseOther(srcid=%u,wdServerID=%u)",srcID,wdServerID);
	ServerTaskHashmap_const_iterator it;
	mutex.lock();
	it = taskUniqueContainer.find(srcID);//taskUniqueContainer.find(srcID);
	if (it != taskUniqueContainer.end())
	{
		if (it->second)
			it->second->responseOther(wdServerID);
	}else
	{
		Zebra::logger->debug("ServerManager::responseOther find srcid=%u",srcID);
	}
	mutex.unlock();
}

