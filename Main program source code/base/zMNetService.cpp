/**
 * \file
 * \version 
 * \author  
 * \date 
 * \brief 实现网络服务器
 *
 * 
 */

#include <iostream>
#include <string>
#include <ext/numeric>

#include "zService.h"
#include "zThread.h"
#include "zSocket.h"
#include "zMTCPServer.h"
#include "zMNetService.h"
#include "Zebra.h"

zMNetService *zMNetService::instance = NULL;

/**
 * \brief 初始化服务器程序
 *
 * 实现<code>zService::init</code>的虚函数
 *
 * \return 是否成功
 */
bool zMNetService::init()
{
	Zebra::logger->trace("zMNetService::init");

	if (!zService::init())
		return false;

	//初始化服务器
	tcpServer = new zMTCPServer(serviceName);
	if (NULL == tcpServer)
		return false;

	return true;
}

/**
 * \brief 网络服务程序的主回调函数
 *
 * 实现虚函数<code>zService::serviceCallback</code>，主要用于监听服务端口，如果返回false将结束程序，返回true继续执行服务
 *
 * \return 回调是否成功
 */
bool zMNetService::serviceCallback()
{
	//soke 取消日志显示
//	Zebra::logger->trace("zMNetService::serviceCallback");

	zMTCPServer::Sock2Port res;
	if (tcpServer->accept(res) > 0) 
	{
		for(zMTCPServer::Sock2Port_const_iterator it = res.begin(); it != res.end(); it++)
		{
			if (it->first >= 0)
			{
				//接收连接成功，处理连接
				newTCPTask(it->first, it->second);
			}
		}
	}

	return true;
}

/**
 * \brief 结束网络服务器程序
 *
 * 实现纯虚函数<code>zService::final</code>，回收资源
 *
 */
void zMNetService::final()
{
	Zebra::logger->trace("zMNetService::final");
	SAFE_DELETE(tcpServer);
}

