/**
 * \file
 * \version  $Id: SessionTaskManager.cpp $
 * \author  
 * \date 
 * \brief 管理子连接的容器
 *
 * 
 */


#include <iostream>

#include "SessionTask.h"
#include "zNoncopyable.h"
#include "SessionTaskManager.h"

SessionTaskManager *SessionTaskManager::instance = NULL;

/**
 * \brief 添加一个子连接到容器中
 *
 * 如果子连接是网关，需要添加到网关连接链表中
 * 如果子连接是场景服务器，需要添加到场景连接链表中
 *
 * \param task 子连接任务
 */
void SessionTaskManager::addSessionTask(SessionTask *task)
{
	rwlock.wrlock();
	if (GATEWAYSERVER == task->getType())
	{
		gatewayTaskList.push_back(task);
	}
	else if (SCENESSERVER == task->getType())
	{
		sceneTaskList.push_back(task);
	}
	rwlock.unlock();
}

/**
 * \brief 从容器中移除子连接任务
 *
 * 如果子连接是网关，需要从网关链表中移除
 * 如果子连接是场景服务器，需要从场景连接链表中移除
 *
 * \param task 子连接任务
 */
void SessionTaskManager::removeSessionTask(SessionTask *task)
{
	rwlock.wrlock();
	if (GATEWAYSERVER == task->getType())
	{
		gatewayTaskList.remove(task);
	}
	else if (SCENESSERVER == task->getType())
	{
		sceneTaskList.remove(task);
	}
	rwlock.unlock();
}

/**
 * \brief 向唯一性验证容器中添加一个子连接任务
 *
 * \param task 子连接任务
 * \return 添加连接是否成功
 */
bool SessionTaskManager::uniqueAdd(SessionTask *task)
{
	SessionTaskHashmap_const_iterator it;
	rwlock.wrlock();
	it = sessionTaskSet.find(task->getID());
	if (it != sessionTaskSet.end())
	{
		Zebra::logger->error(__PRETTY_FUNCTION__);
		rwlock.unlock();
		return false;
	}
	sessionTaskSet.insert(SessionTaskHashmap_pair(task->getID(), task));
	rwlock.unlock();
	return true;
}

/**
 * \brief 从唯一性容器中移除一个子连接任务
 *
 * \param task 子连接任务
 * \return 移除是否成功
 */
bool SessionTaskManager::uniqueRemove(SessionTask *task)
{
	SessionTaskHashmap_iterator it;
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
 * \brief 向所有的网关连接广播指令
 *
 * \param pstrCmd 待广播的指令
 * \param nCmdLen 指令长度
 * \return 广播是否成功
 */
bool SessionTaskManager::broadcastGateway(const void *pstrCmd, int nCmdLen)
{
	bool retval = true;
	TaskContainer_iterator it;
	rwlock.rdlock();
	for(it = gatewayTaskList.begin(); it != gatewayTaskList.end(); it++)
	{
		retval = (retval && (*it)->sendCmd(pstrCmd, nCmdLen));
	}
	rwlock.unlock();
	return retval;
}
/**
 * \brief 向所有的网关发送世界指令
 *
 * \param pstrCmd 待广播的指令
 * \param nCmdLen 指令长度
 * \return 广播是否成功
 */
bool SessionTaskManager::sendCmdToWorld(const void *pstrCmd, int nCmdLen)
{
	using namespace Cmd::Session;
	char buf[zSocket::MAX_DATASIZE];
	t_Session_ForwardWorld * sfw =(t_Session_ForwardWorld*)buf; 
	constructInPlace(sfw);
	sfw->size=nCmdLen;
	bcopy(pstrCmd,sfw->data,nCmdLen);
	return broadcastGateway(sfw,sizeof(t_Session_ForwardWorld) + sfw->size);
}
/**
 * \brief 向所有的网关发送国家指令
 *
 * \param pstrCmd 待广播的指令
 * \param nCmdLen 指令长度
 * \return 广播是否成功
 */
bool SessionTaskManager::sendCmdToCountry(DWORD country,const void *pstrCmd, int nCmdLen)
{
	using namespace Cmd::Session;
	char buf[zSocket::MAX_DATASIZE];
	t_Session_ForwardCountry * sfc =(t_Session_ForwardCountry*)buf; 
	constructInPlace(sfc);
	sfc->size=nCmdLen;
	sfc->dwCountry=country;
	bcopy(pstrCmd,sfc->data,nCmdLen);
	return broadcastGateway(sfc,sizeof(t_Session_ForwardCountry) + sfc->size);
}

/**
 * \brief 向所有的场景服务器连接广播指令
 *
 * \param pstrCmd 待广播的指令
 * \param nCmdLen 指令长度
 * \return 广播是否成功
 */
bool SessionTaskManager::broadcastScene(const void *pstrCmd, int nCmdLen)
{
	bool retval = true;
	TaskContainer_iterator it;
	rwlock.rdlock();
	for(it = sceneTaskList.begin(); it != sceneTaskList.end(); it++)
	{
		retval = (retval && (*it)->sendCmd(pstrCmd, nCmdLen));
	}
	rwlock.unlock();
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
bool SessionTaskManager::broadcastByID(const WORD wdServerID, const void *pstrCmd, int nCmdLen)
{
	bool retval = true;
	SessionTaskHashmap_iterator it;
	rwlock.rdlock();
	it = sessionTaskSet.find(wdServerID);
	if (it != sessionTaskSet.end())
	{
		retval = it->second->sendCmd(pstrCmd, nCmdLen);
	}
	rwlock.unlock();
	return retval;
}

void SessionTaskManager::execEvery()
{
	SessionTaskHashmap_iterator it;
	SessionTask *task=NULL;
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
