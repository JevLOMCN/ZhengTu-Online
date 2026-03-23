/**
 * \file
 * \version  $Id: zTCPClient.cpp  $
 * \author  
 * \date 
 * \brief 实现类zTCPClient，TCP连接客户端。
 *
 * 
 */


#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>

#include "zTCPClient.h"
#include "Zebra.h"

CmdAnalysis zTCPClient::analysis("Client指令发送统计",600);
/**
 * \brief 建立一个到服务器的TCP连接
 *
 *
 * \return 连接是否成功
 */
bool zTCPClient::connect()
{
	Zebra::logger->trace("zTCPClient::connect");
	int retcode;
	int nSocket;
	struct sockaddr_in addr;

	nSocket = ::socket(PF_INET, SOCK_STREAM, 0);
	if (-1 == nSocket)
	{
		Zebra::logger->error("创建套接口失败");
		return false;
	}

	//设置套接口发送接收缓冲，并且客户端的必须在connect之前设置
	socklen_t window_size = 128 * 1024;
	retcode = ::setsockopt(nSocket, SOL_SOCKET, SO_RCVBUF, &window_size, sizeof(window_size));
	if (-1 == retcode)
	{
		TEMP_FAILURE_RETRY(::close(nSocket));
		return false;
	}
	retcode = ::setsockopt(nSocket, SOL_SOCKET, SO_SNDBUF, &window_size, sizeof(window_size));
	if (-1 == retcode)
	{
		TEMP_FAILURE_RETRY(::close(nSocket));
		return false;
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip.c_str());
	addr.sin_port = htons(port);

	retcode = TEMP_FAILURE_RETRY(::connect(nSocket, (struct sockaddr *) &addr, sizeof(addr)));
	if (-1 == retcode)
	{
		Zebra::logger->error("创建到服务器(%s:%u) 的连接失败", ip.c_str(), port);
		TEMP_FAILURE_RETRY(::close(nSocket));
		return false;
	}

	pSocket = new zSocket(nSocket, &addr, compress);
	if (NULL == pSocket)
	{
		Zebra::logger->fatal("没有足够的内存，不能创建zSocket实例");
		TEMP_FAILURE_RETRY(::close(nSocket));
		return false;
	}

	Zebra::logger->info("创建到服务器(%s:%u)的连接成功", ip.c_str(), port);

	return true;
}

/**
 * \brief 向套接口发送指令
 *
 *
 * \param pstrCmd 待发送的指令
 * \param nCmdLen 待发送指令的大小
 * \return 发送是否成功
 */
bool zTCPClient::sendCmd(const void *pstrCmd, const int nCmdLen)
{
	Zebra::logger->trace("zTCPClient::sendCmd");
	if (NULL == pSocket) 
		return false;
	else
	{
		/*
		Cmd::t_NullCmd *ptNullCmd = (Cmd::t_NullCmd *)pstrCmd;
		analysis.add(ptNullCmd->cmd,ptNullCmd->para,nCmdLen);
		// */
		return pSocket->sendCmd(pstrCmd, nCmdLen);
	}
}
/**
 * \brief 重载zThread中的纯虚函数，是线程的主回调函数，用于处理接收到的指令
 *
 */
void zTCPClient::run()
{
	Zebra::logger->trace("zTCPClient::run");
	while(!isFinal())
	{
		unsigned char pstrCmd[zSocket::MAX_DATASIZE];
		int nCmdLen;

		nCmdLen = pSocket->recvToCmd(pstrCmd, zSocket::MAX_DATASIZE, false);
		if (nCmdLen > 0) 
		{
			Cmd::t_NullCmd *ptNullCmd = (Cmd::t_NullCmd *)pstrCmd;
			if (Cmd::CMD_NULL == ptNullCmd->cmd
					&& Cmd::PARA_NULL == ptNullCmd->para)
			{
				//Zebra::logger->debug("客户端收到测试信号");
				if (!sendCmd(pstrCmd, nCmdLen))
				{
					//发送指令失败，退出循环，结束线程
					break;
				}
			}
			else
				msgParse(ptNullCmd, nCmdLen);
		}
		else if (-1 == nCmdLen)
		{
			//接收指令失败，退出循环，结束线程
			Zebra::logger->error("接收指令失败，关闭 %s", getThreadName().c_str());
			break;
		}
	}
}

bool zTCPBufferClient::sendCmd(const void *pstrCmd, const int nCmdLen)
{
//	Zebra::logger->trace("zTCPBufferClient::sendCmd");
	if (pSocket)
		return pSocket->sendCmd(pstrCmd, nCmdLen, _buffered);
	else
		return false;
}

bool zTCPBufferClient::ListeningRecv()
{
//	Zebra::logger->trace("zTCPBufferClient::ListeningRecv");
	int retcode = pSocket->recvToBuf_NoPoll();
	if (-1 == retcode)
	{
		Zebra::logger->error("%s", __PRETTY_FUNCTION__);
		return false;
	}
	else
	{
		do
		{
			unsigned char pstrCmd[zSocket::MAX_DATASIZE];
			int nCmdLen = pSocket->recvToCmd_NoPoll(pstrCmd, sizeof(pstrCmd));
			if (nCmdLen <= 0)
				//这里只是从缓冲取数据包，所以不会出错，没有数据直接返回
				break;
			else
			{
				Cmd::t_NullCmd *ptNullCmd = (Cmd::t_NullCmd *)pstrCmd;
				if (Cmd::CMD_NULL == ptNullCmd->cmd
						&& Cmd::PARA_NULL == ptNullCmd->para)
				{
					//Zebra::logger->debug("客户端收到测试信号");
					if (!sendCmd(pstrCmd, nCmdLen))
					{
						//发送指令失败，退出循环，结束线程
						return false;
					}
				}
				else
					msgParse(ptNullCmd, nCmdLen);
			}
		}
		while(true);
	}
	return true;
}

bool zTCPBufferClient::ListeningSend()
{	
//	Zebra::logger->trace("zTCPBufferClient::ListeningSend");
	if (pSocket)
		return pSocket->sync();
	else
		return false;
}

void zTCPBufferClient::sync()
{
//	Zebra::logger->trace("zTCPBufferClient::sync");
	if (pSocket)
		pSocket->force_sync();
}

void zTCPBufferClient::run()
{	
#if _USE_EPOLL_
	Zebra::logger->trace("zTCPBufferClient::run(EPOLL)");
	while(!isFinal())
        {
                unsigned char pstrCmd[zSocket::MAX_DATASIZE];
                int nCmdLen;

                nCmdLen = pSocket->recvToCmd(pstrCmd, zSocket::MAX_DATASIZE, false);
                if (nCmdLen > 0)
                {
                        Cmd::t_NullCmd *ptNullCmd = (Cmd::t_NullCmd *)pstrCmd;
                        if (Cmd::CMD_NULL == ptNullCmd->cmd
                                        && Cmd::PARA_NULL == ptNullCmd->para)
                        {
                                //Zebra::logger->debug("客户端收到测试信号");
                                if (!sendCmd(pstrCmd, nCmdLen))
                                {
                                        //发送指令失败，退出循环，结束线程
                                        break;
                                }
                        }
                        else
                                msgParse(ptNullCmd, nCmdLen);
                }
                else if (-1 == nCmdLen)
                {
                        //接收指令失败，退出循环，结束线程
                        Zebra::logger->error("接收指令失败，关闭 %s", getThreadName().c_str());
                        break;
                }
        }


#else
	Zebra::logger->trace("zTCPBufferClient::run(POLL)");
	_buffered = true;
	struct pollfd pfds;
	struct pollfd pfds_r;
	pSocket->fillPollFD(pfds, POLLIN | POLLOUT | POLLERR | POLLPRI);
	pSocket->fillPollFD(pfds_r, POLLIN | POLLERR | POLLPRI);
	int time=usleep_time;
	while(!isFinal())
	{
		struct timeval _tv_1;
		struct timeval _tv_2;
		gettimeofday(&_tv_1,NULL);
		if (TEMP_FAILURE_RETRY(::poll(&pfds_r, 1, time/1000)) > 0)
		{
			if (pfds_r.revents & (POLLERR | POLLPRI))
			{
				//套接口出现错误
				Zebra::logger->fatal("%s: 套接口错误", __PRETTY_FUNCTION__);
				break;
			}
			else
			{
				if (pfds_r.revents & POLLIN)
				{
					//套接口准备好了读取操作
					if (!ListeningRecv())
					{
						Zebra::logger->debug("%s: 套接口读操作错误", __PRETTY_FUNCTION__);
						break;
					}
				}
			}
		}
		gettimeofday(&_tv_2,NULL);
		int end=_tv_2.tv_sec*1000000 + _tv_2.tv_usec;
		int begin= _tv_1.tv_sec*1000000 + _tv_1.tv_usec;
		time = time - (end - begin);
		if(time <= 0)
		{
			if (TEMP_FAILURE_RETRY(::poll(&pfds, 1, 0)) > 0)
			{
				if (pfds.revents & (POLLERR | POLLPRI))
				{
					//套接口出现错误
					Zebra::logger->fatal("%s: 套接口错误", __PRETTY_FUNCTION__);
					break;
				}
				else
				{
					if (pfds.revents & POLLIN)
					{
						//套接口准备好了读取操作
						if (!ListeningRecv())
						{
							Zebra::logger->debug("%s: 套接口读操作错误", __PRETTY_FUNCTION__);
							break;
						}
					}
					if (pfds.revents & POLLOUT)
					{
						//套接口准备好了写入操作
						if (!ListeningSend())
						{
							Zebra::logger->debug("%s: 套接口写操作错误", __PRETTY_FUNCTION__);
							break;
						}
					}
				}
			}
			time = usleep_time;
		}
		//zThread::usleep(usleep_time);
	}

	//保证缓冲的数据发送完成
	sync();
	_buffered = false;
#endif
}

