/**
 * \file
 * \version  $Id: TCPClient.cpp  $
 * \author 
 * \date 
 * \brief 测试TCP客户端类。
 *
 * 
 */

#include <unistd.h>
#include <iostream>
#include <queue>
#include <list>
#include <vector>
#include <iterator>
#include <sstream>
#include <ext/hash_map>

#include "zMisc.h"
#include "zTCPClient.h"
#include "zMutex.h"
#include "zCond.h"
#include "zRWLock.h"
#include "zSyncEvent.h"
#include "zService.h"

class TCPClient : public zTCPClient
{

	public:
		
		TCPClient(const std::string &name) : zTCPClient(name) {};

		void run()
		{
			if (NULL == pSocket) return;

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
						if (!pSocket->sendCmd(pstrCmd, nCmdLen))
							//发送指令失败
							break;
					}
					else
						msgParse(ptNullCmd, nCmdLen);
				}
				else if (-1 == nCmdLen)
				{
					//接收指令失败
					Zebra::logger->error("%s\t%u", __PRETTY_FUNCTION__, nCmdLen);
					break;
				}
			}
		}

		bool msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
		{
			return true;
		}

};

class SendThread : public zThread
{

	public:

		SendThread(const std::string &name, TCPClient *tcpClient) : zThread(name) { this->tcpClient = tcpClient; };

		void run()
		{
			while(!isFinal())
			{
				static BYTE opcode = 1;
				char buffer[zSocket::MAX_DATASIZE];
				Cmd::t_NullCmd *ptCmd = (Cmd::t_NullCmd *)buffer;
				ptCmd->cmd = opcode;
				ptCmd->para = opcode;
				int nCmdLen = zMisc::randBetween(sizeof(Cmd::t_NullCmd), sizeof(buffer));
				if (!tcpClient->sendCmd(ptCmd, nCmdLen))
				{
					Zebra::logger->error("send error!");
					break;
				}
				opcode++; if (opcode == 255) opcode = 1;
			}
		}

	private:
		TCPClient *tcpClient;

};

class TCPClientService : public zService
{

	public:

		static TCPClientService &getInstance()
		{
			if (NULL == instance)
				instance = new TCPClientService("测试程序");

			return *instance;
		}

		/**
		 * \brief 释放类的唯一实例
		 *
		 */
		static void delInstance()
		{
			if (instance)
			{
				delete instance;
				instance = NULL;
			}
		}

	private:

		static TCPClientService *instance;

		TCPClientService(const std::string &name) : zService(name) {};

		bool init();
		bool serviceCallback();
		void final();

		static const int max_client = 8;
		TCPClient *tcpClient[max_client];
		SendThread *sendThread[max_client];

};

TCPClientService *TCPClientService::instance = NULL;

bool TCPClientService::init()
{
	if (!zService::init())
		return false;

	for(int i = 0; i < max_client; i++) 
	{
		std::ostringstream name;
		name << "客户端" << "[" << i << "]";
		tcpClient[i] = new TCPClient(name.str());
		tcpClient[i]->connect("192.168.8.129", 9000);
		//缺省使用空指令作为验证指令
		Cmd::t_NullCmd tCmd;
		tcpClient[i]->sendCmd(&tCmd, sizeof(tCmd));
		tcpClient[i]->start();
		name.str(std::string(""));
		name << "客户端发送线程" << "[" << i << "]";
		sendThread[i] = new SendThread(name.str(), tcpClient[i]);
		sendThread[i]->start();
	}

	return true;
}

bool TCPClientService::serviceCallback()
{
	zThread::sleep(1);
	Zebra::logger->debug("%s", __PRETTY_FUNCTION__);

	return true;
}

void TCPClientService::final()
{
	Zebra::logger->debug("%s", __PRETTY_FUNCTION__);

	for(int i = 0; i < max_client; i++)
	{
		sendThread[i]->final();
		sendThread[i]->join();
		tcpClient[i]->final();
		tcpClient[i]->join();
		delete tcpClient[i];
		delete sendThread[i];
	}
}

int main(int argc, char **argv)
{
	Zebra::logger=new zLogger();

	TCPClientService::getInstance().main();
	TCPClientService::delInstance();

	return EXIT_SUCCESS;
}

