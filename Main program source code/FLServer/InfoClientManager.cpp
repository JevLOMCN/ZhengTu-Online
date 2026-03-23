/**
 * \file
 * \version  $Id: InfoClientManager.cpp  $
 * \author  
 * \date 
 * \brief 定义服务器信息采集连接的客户端管理容器
 */


#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"
#include "InfoClient.h"
#include "InfoClientManager.h"
#include "zXMLParser.h"

/**
 * \brief 类的唯一实例指针
 */
InfoClientManager *InfoClientManager::instance = NULL;

/**
 * \brief 构造函数
 */
InfoClientManager::InfoClientManager()
{
	infoClientPool = NULL;
}

/**
 * \brief 析构函数
 */
InfoClientManager::~InfoClientManager()
{
	SAFE_DELETE(infoClientPool);
}

/**
 * \brief 初始化管理器
 * \return 初始化是否成功
 */
bool InfoClientManager::init()
{
	Zebra::logger->trace("InfoClientManager::init");
	infoClientPool = new zTCPClientTaskPool();
	if (NULL == infoClientPool
			|| !infoClientPool->init())
		return false;

//	Zebra::logger->debug("InfoServer: %s, %s", Zebra::global["InfoServer"].c_str(), Zebra::global["InfoPort"].c_str());
//	infoClientPool->put(new InfoClient(Zebra::global["InfoServer"], atoi(Zebra::global["InfoPort"].c_str())));

	return true;
}

/**
 * \brief 周期间隔进行连接的断线重连工作
 * \param ct 当前时间
 */
void InfoClientManager::timeAction(const zTime &ct)
{
	//soke 取消日志显示
//	Zebra::logger->trace("InfoClientManager::timeAction");
	if (actionTimer.elapse(ct) > 4)
	{
		if (infoClientPool)
			infoClientPool->timeAction(ct);
		actionTimer = ct;
	}
}

/**
 * \brief 向容器中添加已经成功的连接
 * \param infoClient 待添加的连接
 */
void InfoClientManager::add(InfoClient *infoClient)
{
	Zebra::logger->trace("InfoClientManager::add");
	if (infoClient)
	{
		mlock.lock();
		allClients.push_back(infoClient);
		mlock.unlock();
	}
}

/**
 * \brief 从容器中移除断开的连接
 * \param infoClient 待移除的连接
 */
void InfoClientManager::remove(InfoClient *infoClient)
{
	Zebra::logger->trace("InfoClientManager::remove");
	if (infoClient)
	{
		mlock.lock();
		InfoClientContainer::iterator it = find(allClients.begin(), allClients.end(), infoClient);
		if (it != allClients.end())
		{
			allClients.erase(it);
		}
		mlock.unlock();
	}
}

/**
 * \brief 向成功的所有连接广播指令
 * \param pstrCmd 待广播的指令
 * \param nCmdLen 待广播指令的长度
 */
bool InfoClientManager::broadcast(const void *pstrCmd, int nCmdLen)
{
	Zebra::logger->trace("InfoClientManager::broadcast");
	bool retval = false;
	mlock.lock();
	for(InfoClientContainer::iterator it = allClients.begin(); it != allClients.end(); ++it)
	{
		retval = retval | (*it)->sendCmd(pstrCmd, nCmdLen);
	}
	mlock.unlock();
	return retval;
}

