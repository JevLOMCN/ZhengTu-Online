/**
 * \file
 * \version  $Id: FLClientManager.cpp  $
 * \author  
 * \date 
 * \brief 实现统一用户平台客户端连接的管理容器
 */


#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"
#include "FLClient.h"
#include "FLClientManager.h"
#include "zXMLParser.h"

/**
 * \brief 类的唯一实例指针
 */
FLClientManager *FLClientManager::instance = NULL;

/**
 * \brief 构造函数
 */
FLClientManager::FLClientManager()
{
	flClientPool = NULL;
}

/**
 * \brief 析构函数
 */
FLClientManager::~FLClientManager()
{
	SAFE_DELETE(flClientPool);
}

/**
 * \brief 初始化管理器
 * \return 初始化是否成功
 */
bool FLClientManager::init()
{
	Zebra::logger->trace("FLClientManager::init");
	flClientPool = new zTCPClientTaskPool();
	if (NULL == flClientPool
			|| !flClientPool->init())
		return false;

	zXMLParser xml;
	if (!xml.initFile(Zebra::global["loginServerListFile"]))
	{
		Zebra::logger->error("加载统一用户平台登陆服务器列表文件 %s 失败", Zebra::global["loginServerListFile"].c_str());
		return false;
	}
	xmlNodePtr root = xml.getRootNode("Zebra");
	if (root)
	{
		xmlNodePtr zebra_node = xml.getChildNode(root, "LoginServerList");
		while(zebra_node)
		{
			if (strcmp((char *)zebra_node->name, "LoginServerList") == 0)
			{
				xmlNodePtr node = xml.getChildNode(zebra_node, "server");
				while(node)
				{
					if (strcmp((char *)node->name, "server") == 0)
					{
						Zebra::global["FLServer"] = "";
						Zebra::global["FLPort"] = "";
						if (xml.getNodePropStr(node, "ip", Zebra::global["FLServer"])
								&& xml.getNodePropStr(node, "port", Zebra::global["FLPort"]))
						{
							Zebra::logger->debug("LoginServer: %s, %s", Zebra::global["FLServer"].c_str(), Zebra::global["FLPort"].c_str());
							flClientPool->put(new FLClient(Zebra::global["FLServer"], atoi(Zebra::global["FLPort"].c_str())));
						}
					}

					node = xml.getNextNode(node, NULL);
				}
			}

			zebra_node = xml.getNextNode(zebra_node, NULL);
		}
	}

	Zebra::logger->info("加载统一用户平台登陆服务器列表文件成功");
	return true;
}

/**
 * \brief 周期间隔进行连接的断线重连工作
 * \param ct 当前时间
 */
void FLClientManager::timeAction(const zTime &ct)
{
	Zebra::logger->trace("FLClientManager::timeAction");
	if (actionTimer.elapse(ct) > 4)
	{
		if (flClientPool)
			flClientPool->timeAction(ct);
		actionTimer = ct;
	}
}

/**
 * \brief 向容器中添加已经成功的连接
 * \param flClient 待添加的连接
 */
void FLClientManager::add(FLClient *flClient)
{
	Zebra::logger->trace("FLClientManager::add");
	if (flClient)
	{
		zRWLock_scope_wrlock scope_wrlock(rwlock);
		const_iter it = allClients.find(flClient->getTempID());
		if (it == allClients.end())
		{
			allClients.insert(value_type(flClient->getTempID(), flClient));
		}
	}
}

/**
 * \brief 从容器中移除断开的连接
 * \param flClient 待移除的连接
 */
void FLClientManager::remove(FLClient *flClient)
{
	Zebra::logger->trace("FLClientManager::remove");
	if (flClient)
	{
		zRWLock_scope_wrlock scope_wrlock(rwlock);
		iter it = allClients.find(flClient->getTempID());
		if (it != allClients.end())
		{
			allClients.erase(it);
		}
	}
}

/**
 * \brief 向成功的所有连接广播指令
 * \param pstrCmd 待广播的指令
 * \param nCmdLen 待广播指令的长度
 */
void FLClientManager::broadcast(const void *pstrCmd, int nCmdLen)
{
	Zebra::logger->trace("FLClientManager::broadcast");
	zRWLock_scope_rdlock scope_rdlock(rwlock);
	for(iter it = allClients.begin(); it != allClients.end(); ++it)
	{
		it->second->sendCmd(pstrCmd, nCmdLen);
	}
}

/**
 * \brief 向指定的成功连接广播指令
 * \param tempid 待广播指令的连接临时编号
 * \param pstrCmd 待广播的指令
 * \param nCmdLen 待广播指令的长度
 */
void FLClientManager::sendTo(const WORD tempid, const void *pstrCmd, int nCmdLen)
{
	Zebra::logger->trace("FLClientManager::sendTo");
	zRWLock_scope_rdlock scope_rdlock(rwlock);
	iter it = allClients.find(tempid);
	if (it != allClients.end())
	{
		it->second->sendCmd(pstrCmd, nCmdLen);
	}
}

