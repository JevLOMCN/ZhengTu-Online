/**
 * \file
 * \version  $Id: DBAccessClientManager.cpp  $
 * \author  
 * \date 
 * \brief 实现统一用户平台账号管理客户连接的管理器
 */

#include <algorithm>

#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"
#include "DBAccessClient.h"
#include "DBAccessClientManager.h"
#include "zXMLParser.h"
#include "zMisc.h"

DBAccessClientManager *DBAccessClientManager::instance = NULL;

DBAccessClientManager::DBAccessClientManager()
{
	dbAccessClientPool = NULL;
}

DBAccessClientManager::~DBAccessClientManager()
{
	SAFE_DELETE(dbAccessClientPool);
}

bool DBAccessClientManager::init()
{
	dbAccessClientPool = new zTCPClientTaskPool();
	if (NULL == dbAccessClientPool
			|| !dbAccessClientPool->init())
		return false;

	zXMLParser xml;
	if (!xml.initFile(Zebra::global["dbAccessServerListFile"]))
	{
		Zebra::logger->error("加载统一用户平台用户数据访问中间件服务器列表 %s 失败", Zebra::global["dbAccessServerListFile"].c_str());
		return false;
	}
	xmlNodePtr root = xml.getRootNode("DBAccessServerList");
	if (root)
	{
		xmlNodePtr node = xml.getChildNode(root, "server");
		while(node)
		{
			if (strcmp((char *)node->name, "server") == 0)
			{
				Zebra::global["DBAccessServer"] = "";
				Zebra::global["DBAccessPort"] = "";
				if (xml.getNodePropStr(node, "ip", Zebra::global["DBAccessServer"])
						&& xml.getNodePropStr(node, "port", Zebra::global["DBAccessPort"]))
				{
					Zebra::logger->debug("DBAccessServer: %s, %s", Zebra::global["DBAccessServer"].c_str(), Zebra::global["DBAccessPort"].c_str());
					dbAccessClientPool->put(new DBAccessClient(Zebra::global["DBAccessServer"], atoi(Zebra::global["DBAccessPort"].c_str())));
				}
			}

			node = xml.getNextNode(node, NULL);
		}
	}

	Zebra::logger->info("加载统一用户平台用户数据访问中间件服务器列表成功");
	return true;
}

void DBAccessClientManager::timeAction(const zTime &ct)
{
	if (actionTimer.elapse(ct) > 4)
	{
		if (dbAccessClientPool)
			dbAccessClientPool->timeAction(ct);
		actionTimer = ct;
	}
}

void DBAccessClientManager::add(DBAccessClient *dbAccessClient)
{
	if (dbAccessClient)
	{
		mlock.lock();
		allClients.push_back(dbAccessClient);
		mlock.unlock();
	}
}

void DBAccessClientManager::remove(DBAccessClient *dbAccessClient)
{
	if (dbAccessClient)
	{
		mlock.lock();
		DBAccessClientContainer::iterator it = std::find(allClients.begin(), allClients.end(), dbAccessClient);
		if (it != allClients.end())
		{
			allClients.erase(it);
		}
		mlock.unlock();
	}
}

bool DBAccessClientManager::broadcast(const void *pstrCmd, int nCmdLen)
{
	bool retval = false;
	mlock.lock();
	if (!allClients.empty())
	{
		DBAccessClientContainer::size_type i = zMisc::randBetween(0, allClients.size() - 1);
		retval = allClients[i]->sendCmd(pstrCmd, nCmdLen);
	}
	mlock.unlock();
	return retval;
}

