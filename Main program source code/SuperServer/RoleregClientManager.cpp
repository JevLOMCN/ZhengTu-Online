/**
 * \file
 * \version  $Id: RoleregClientManager.cpp  $
 * \author  
 * \date 
 * \brief 定义角色名称唯一性验证服务连接的客户端管理容器
 */


#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"
#include "RoleregClient.h"
#include "RoleregClientManager.h"
#include "zXMLParser.h"

/**
 * \brief 类的唯一实例指针
 */
RoleregClientManager *RoleregClientManager::instance = NULL;

/**
 * \brief 构造函数
 */
RoleregClientManager::RoleregClientManager()
{
	roleregClientPool = NULL;
}

/**
 * \brief 析构函数
 */
RoleregClientManager::~RoleregClientManager()
{
	SAFE_DELETE(roleregClientPool);
}

/**
 * \brief 初始化管理器
 * \return 初始化是否成功
 */
bool RoleregClientManager::init()
{
	roleregClientPool = new zTCPClientTaskPool();
	if (NULL == roleregClientPool
			|| !roleregClientPool->init())
		return false;

	zXMLParser xml;
	if (!xml.initFile(Zebra::global["loginServerListFile"]))
	{
		Zebra::logger->error("加载统一用户平台RoleregServer列表文件 %s 失败", Zebra::global["loginServerListFile"].c_str());
		return false;
	}
	xmlNodePtr root = xml.getRootNode("Zebra");
	if (root)
	{
		xmlNodePtr zebra_node = xml.getChildNode(root, "RoleregServerList");
		while(zebra_node)
		{
			if (strcmp((char *)zebra_node->name, "RoleregServerList") == 0)
			{
				xmlNodePtr node = xml.getChildNode(zebra_node, "server");
				while(node)
				{
					if (strcmp((char *)node->name, "server") == 0)
					{
						Zebra::global["RoleregServer"] = "";
						Zebra::global["RoleregPort"] = "";
						if (xml.getNodePropStr(node, "ip", Zebra::global["RoleregServer"])
								&& xml.getNodePropStr(node, "port", Zebra::global["RoleregPort"]))
						{
							Zebra::logger->debug("RoleregServer: %s, %s", Zebra::global["RoleregServer"].c_str(), Zebra::global["RoleregPort"].c_str());
							roleregClientPool->put(new RoleregClient(Zebra::global["RoleregServer"], atoi(Zebra::global["RoleregPort"].c_str())));

						}
					}

					node = xml.getNextNode(node, NULL);
				}
			}

			zebra_node = xml.getNextNode(zebra_node, NULL);
		}
	}

	Zebra::logger->info("加载统一用户平台RoleregServer列表文件成功");
	return true;
}

/**
 * \brief 周期间隔进行连接的断线重连工作
 * \param ct 当前时间
 */
void RoleregClientManager::timeAction(const zTime &ct)
{
	if (actionTimer.elapse(ct) > 4)
	{
		if (roleregClientPool)
			roleregClientPool->timeAction(ct);
		actionTimer = ct;
	}
}

/**
 * \brief 向容器中添加已经成功的连接
 * \param roleregClient 待添加的连接
 */
void RoleregClientManager::add(RoleregClient *roleregClient)
{
	if (roleregClient)
	{
		zRWLock_scope_wrlock scope_wrlock(rwlock);
		const_iter it = find(allClients.begin(), allClients.end(), roleregClient);
		if (it == allClients.end())
		{
			allClients.insert(roleregClient);
		}
	}
}

/**
 * \brief 从容器中移除断开的连接
 * \param roleregClient 待移除的连接
 */
void RoleregClientManager::remove(RoleregClient *roleregClient)
{
	if (roleregClient)
	{
		zRWLock_scope_wrlock scope_wrlock(rwlock);
		iter it = find(allClients.begin(), allClients.end(), roleregClient);
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
bool RoleregClientManager::broadcastOne(const void *pstrCmd, int nCmdLen)
{
	zRWLock_scope_rdlock scope_rdlock(rwlock);
	for(iter it = allClients.begin(); it != allClients.end(); ++it)
	{
		if ((*it)->sendCmd(pstrCmd, nCmdLen))
			return true;
	}
	return false;
}

