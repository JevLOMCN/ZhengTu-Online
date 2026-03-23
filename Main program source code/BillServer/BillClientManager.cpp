/**
 * \file
 * \version  $Id: BillClientManager.cpp  $
 * \author  
 * \date 
 * \brief 实现统一用户平台客户端连接的管理容器
 */


#include <string>

#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"
#include "BillCache.h"
#include "BillCallback.h"
#include "BillClient.h"
#include "BillClientManager.h"
#include "zXMLParser.h"

zLogger *BillClientManager::tlogger = NULL;
int		BillClientManager::source = -1;

/**
 * \brief 构造函数
 */
BillClientManager::BillClientManager() : billClientPool(NULL), actionTimer(), maxID(0)
{
}

/**
 * \brief 析构函数
 */
BillClientManager::~BillClientManager()
{
	SAFE_DELETE(billClientPool);
}

/**
 * \brief 初始化管理器
 * \param bc 计费回调函数
 * \return 初始化是否成功
 */
bool BillClientManager::init(const std::string &confile, const std::string &tradelog, BillCallback &bc)
{
	Zebra::logger->debug(__PRETTY_FUNCTION__);
	this->bc.action = bc.action;

	billClientPool = new zTCPClientTaskPool();
	if (NULL == billClientPool
			|| !billClientPool->init())
		return false;

	zXMLParser xml;
	if (!xml.initFile(confile))
	{
		Zebra::logger->error("加载统一用户平台计费服务器列表文件 %s 失败", confile.c_str());
		return false;
	}
	xmlNodePtr root = xml.getRootNode("Zebra");
	if (root)
	{
		xmlNodePtr source_node = xml.getChildNode(root, "sourceID");
		xml.getNodeContentNum(source_node, &source, sizeof(source));
		xmlNodePtr zebra_node = xml.getChildNode(root, "BillServerList");
		while(zebra_node)
		{
			if (strcmp((char *)zebra_node->name, "BillServerList") == 0)
			{
				xmlNodePtr server_node = xml.getChildNode(zebra_node, "server");
				while(server_node)
				{
					if (strcmp((char *)server_node->name, "server") == 0)
					{
						xmlNodePtr node = xml.getChildNode(server_node, "entry");
						while(node)
						{
							if (strcmp((char *)node->name, "entry") == 0)
							{
								Zebra::global["unifyBillServer"] = "";
								Zebra::global["unifyBillPort"] = ""; 
								if (xml.getNodePropStr(node, "ip", Zebra::global["unifyBillServer"])
										&& xml.getNodePropStr(node, "port", Zebra::global["unifyBillPort"]))
								{
									Zebra::logger->debug("unifyBillServer: %s, %s",
											Zebra::global["unifyBillServer"].c_str(),
											Zebra::global["unifyBillPort"].c_str());
									billClientPool->put(new BillClient(Zebra::global["unifyBillServer"],
												atoi(Zebra::global["unifyBillPort"].c_str()), this->bc, maxID));
								}
							}

							node = xml.getNextNode(node, NULL);
						}

						maxID++;
					}

					server_node = xml.getNextNode(server_node, NULL);
				}
			}

			zebra_node = xml.getNextNode(zebra_node, NULL);
		}
	}

	//初始化交易记录的log
	BillClientManager::tlogger = new zLogger("ClientTrade");
	//设置交易日志级别debug
	BillClientManager::tlogger->setLevel("debug");
	//设置写本地日志文件
	if ("" != tradelog)
		BillClientManager::tlogger->addLocalFileLog(tradelog);

	Zebra::logger->debug("tradelogfilename = %s,source = %d", tradelog.c_str(),source);

	return true;
}

/**
 * \brief 周期间隔进行连接的断线重连工作
 * \param ct 当前时间
 */
void BillClientManager::timeAction(const zTime &ct)
{
	if (actionTimer.elapse(ct) >= 4)
	{
		if (billClientPool)
			billClientPool->timeAction(ct);
		actionTimer = ct;
	}
}

/**
 * \brief 向容器中添加已经成功的连接
 * \param billClient 待添加的连接
 */
void BillClientManager::add(BillClient *billClient)
{
	if (billClient)
	{
		rwlock.wrlock();
		allClients.insert(billClient);
		rwlock.unlock();
	}
}

/**
 * \brief 从容器中移除断开的连接
 * \param billClient 待移除的连接
 */
void BillClientManager::remove(BillClient *billClient)
{
	if (billClient)
	{
		rwlock.wrlock();
		std::pair<iter, iter> sp = allClients.equal_range(billClient);
		for(iter it = sp.first; it != sp.second; ++it)
		{
			if (billClient == (*it))
			{
				allClients.erase(it);
				rwlock.unlock();
				return;
			}
		}
		rwlock.unlock();
	}
}

bool BillClientManager::action(BillData *bd)
{
	if (maxID)
	{
		rwlock.rdlock();
		for(iter it = allClients.begin(); it != allClients.end(); ++it)
		{
			if (bd->uid % maxID == (*it)->getID()
					&& (*it)->action(bd))
			{
				rwlock.unlock();
				return true;
			}
		}
		rwlock.unlock();
	}
	return false;
}
void BillClientManager::execEvery()
{
	iter it;
	BillClient *task=NULL;
	rwlock.rdlock();
	it = allClients.begin();
	for (; it != allClients.end() ; it ++)
	{
		task=*it;
		task->doCmd();
	}
	rwlock.unlock();

}
