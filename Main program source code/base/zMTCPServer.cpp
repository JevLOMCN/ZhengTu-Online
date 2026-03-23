/**
 * \file
 * \version  $Id: zMTCPServer.cpp  $
 * \author  
 * \date 
 * \brief 实现类zMTCPServer
 *
 * 
 */

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

#include "zSocket.h"
#include "zMTCPServer.h"
#include "Zebra.h"

/**
 * \brief 构造函数，用于构造一个服务器zMTCPServer对象
 * \param name 服务器名称
 */
zMTCPServer::zMTCPServer(const std::string &name) : name(name)
{
	Zebra::logger->trace("zMTCPServer::zMTCPServer");
#ifdef _USE_EPOLL_
	kdpfd = epoll_create(1);
	assert(-1 != kdpfd);
	epfds.resize(8);
#else
	pfds.resize(8);
#endif
}

/**
 * \brief 析构函数，用于销毁一个zMTCPServer对象
 */
zMTCPServer::~zMTCPServer() 
{
	Zebra::logger->trace("zMTCPServer::~zMTCPServer");
#ifdef _USE_EPOLL_
	TEMP_FAILURE_RETRY(::close(kdpfd));
#endif

	for(Sock2Port_const_iterator it = mapper.begin(); it != mapper.end(); it++)
	{
		if (-1 != it->first)
		{
			::shutdown(it->first, SHUT_RD);
			TEMP_FAILURE_RETRY(::close(it->first));
		}
	}
	mapper.clear();
}

/**
 * \brief 绑定监听服务到某一个端口
 * \param name 绑定端口名称
 * \param port 具体绑定的端口
 * \return 绑定是否成功
 */
bool zMTCPServer::bind(const std::string &name, const unsigned short port) 
{
	Zebra::logger->trace("zMTCPServer::bind");
	zMutex_scope_lock scope_lock(mlock);
	struct sockaddr_in addr;
	int sock;

	for(Sock2Port_const_iterator it = mapper.begin(); it != mapper.end(); it++)
	{
		if (it->second == port)
		{
			Zebra::logger->warn("端口 %u 已经绑定服务");
			return false;
		}
	}

	sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == sock) 
	{
		Zebra::logger->error("创建套接口失败");
		return false;
	}

	//设置套接口为可重用状态
	int reuse = 1;
	if (-1 == ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse))) 
	{
		Zebra::logger->error("不能设置套接口为可重用状态");
		TEMP_FAILURE_RETRY(::close(sock));
		return false;
	}

	//设置套接口发送接收缓冲，并且服务器的必须在accept之前设置
	socklen_t window_size = 128 * 1024;
	if (-1 == ::setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &window_size, sizeof(window_size)))
	{
		TEMP_FAILURE_RETRY(::close(sock));
		return false;
	}
	if (-1 == ::setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &window_size, sizeof(window_size)))
	{
		TEMP_FAILURE_RETRY(::close(sock));
		return false;
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	int retcode = ::bind(sock, (struct sockaddr *) &addr, sizeof(addr));
	if (-1 == retcode) 
	{
		Zebra::logger->error("不能绑定服务器端口");
		TEMP_FAILURE_RETRY(::close(sock));
		return false;
	}

	retcode = ::listen(sock, MAX_WAITQUEUE);
	if (-1 == retcode) 
	{
		Zebra::logger->error("监听套接口失败");
		TEMP_FAILURE_RETRY(::close(sock));
		return false;
	}

#ifdef _USE_EPOLL_
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = sock;
	assert(0 == epoll_ctl(kdpfd, EPOLL_CTL_ADD, sock, &ev));
#else
	pfds[mapper.size()].fd = sock;
	pfds[mapper.size()].events = POLLIN;
	pfds[mapper.size()].revents = 0;
#endif

	mapper.insert(Sock2Port_value_type(sock, port));
#ifdef _USE_EPOLL_
	if (mapper.size() > epfds.size())
	{
		epfds.resize(mapper.size() + 8);
	}
#else
	if (mapper.size() > pfds.size())
	{
		pfds.resize(mapper.size() + 8);
	}
#endif

	Zebra::logger->info("服务器 %s:%u 端口初始化绑定成功", name.c_str(), port);

	return true;
}

/**
 * \brief 接受客户端的连接
 * \param res 返回的连接集合
 * \return 接收到的连接个数
 */
int zMTCPServer::accept(Sock2Port &res)
{
	//soke 取消日志显示
//	Zebra::logger->trace("zMTCPServer::accept");
	zMutex_scope_lock scope_lock(mlock);
	int retval = 0;

#ifdef _USE_EPOLL_
	int rc = epoll_wait(kdpfd, &epfds[0], mapper.size(), T_MSEC);
	if (rc > 0)
	{
		for(int i = 0; i < rc; i++)
		{
			if (epfds[i].events & EPOLLIN)
			{
				res.insert(Sock2Port_value_type(TEMP_FAILURE_RETRY(::accept(epfds[i].data.fd, NULL, NULL)), mapper[epfds[i].data.fd]));
				retval++;
			}
		}
	}
#else
	for(Sock2Port::size_type i = 0; i < mapper.size(); i++)
		pfds[i].revents = 0;
	int rc = TEMP_FAILURE_RETRY(::poll(&pfds[0], mapper.size(), T_MSEC));
	if (rc > 0)
	{
		for(Sock2Port::size_type i = 0; i < mapper.size(); i++)
		{
			if (pfds[i].revents & POLLIN)
			{
				res.insert(Sock2Port_value_type(TEMP_FAILURE_RETRY(::accept(pfds[i].fd, NULL, NULL)), mapper[pfds[i].fd]));
				retval++;
			}
		}
	}
#endif

	return retval;
}

