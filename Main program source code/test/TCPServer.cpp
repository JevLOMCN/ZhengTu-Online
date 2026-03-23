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

class TCPServerTask:public zTCPTask
{
	public:
		// 
		int verifyConn()
		{
			int retcode = mSocket.recvToBuf_NoPoll();
			if ( retcode > 0)
			{
				unsigned char pstrCmd[zSocket::MAX_DATASIZE];
				int nCmdlen = mSocket.recvToCmd_NoPoll(pstrCmd, sizeof(pstrCmd));
				if (nCmdlen <= 0)
				{
					return 0;
				}
				else
				{
					using namespace Cmd::Info;
					t_LoginCmd *ptCmd = (t_LoginCmd *)pstrCmd;
					if (CMD_LOGIN == ptCmd->cmd && PARA_LOGIN == ptCmd->para)
					{
						Zebra::logger->debug("superServer连接通过验证");
						return 1;
					}
					else
					{
						return -1;
					}
				}
			}
			else
			{
				return retcode;
			}
		}

		//
		bool msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
		{
			Zebra::logger->trace("TCPServerTask::msgParse");

			switch(ptNullCmd->cmd)
			{	

			}
			return true;
		}

		
		TCPServerTask(zTCPTaskPool *pool, const int sock, 
				const struct sockaddr_in *addr
				) : zTCPTask(pool, sock, addr) 
		{
			
		}
		
		~TCPServerTask() { }

};

class TCPServer:public zNetService
{
	public:

		const int getPoolSize() const
		{
			return taskPool->getSize();
		}

		const int getPoolState() const
		{
			return taskPool->getState();
		}

		/**
		 * \brief 析构函数
		 */
		~TCPServer()
		{
			//关闭线程池
			if (taskPool)
			{
				taskPool->final();
				SAFE_DELETE(taskPool);
			}
		}

		/**
		 * \brief 获取类的唯一实例
		 *	使用了singleton设计模式,保证了进程中只有一个实例
		 * \return 类的唯一实例
		 */
		static TCPServer &getInstance()
		{
			if (NULL == instance)
			{
				instance = new TCPServer();
			}

			return *instance;
		}

		/**
		 * \brief 释放类的唯一实例
		 */
		static void delInstance()
		{
			SAFE_DELETE(instance);
		}
	
		
	private:

		/**
		 * \brief 类的唯一实例
		 */
		static TCPServer *instance;

		zTCPTaskPool *taskPool;				/**< TCP连接池的指针 */

		/**
		 * \brief 构造函数
		 */
		TCPServer() : zNetService("TCPServer")
		{
			taskPool = NULL;
			bindport = 0;
		}

		bool init()
		{
			//初始化连接线程池
			int state = state_none;
			Zebra::to_lower(Zebra::global["initThreadPoolState"]);
			if ("repair" == Zebra::global["initThreadPoolState"]
			|| "maintain" == Zebra::global["initThreadPoolState"])
			state = state_maintain;
			taskPool = new zTCPTaskPool(atoi(Zebra::global["threadPoolCapacity"].c_str()), state);
			if (NULL == taskPool
				|| !taskPool->init())
				return false;

			bindport = atoi(Zebra::global["bindport"].c_str());
			if (!zNetService::init(bindport))
			{
			return false;
			}
		}

		void newTCPTask(const int sock, const struct sockaddr_in *addr)
		{
			TCPServerTask *tcpTask = new TCPServerTask(taskPool, sock, addr);
			if (NULL == tcpTask)
				//内存不足，直接关闭连接
				TEMP_FAILURE_RETRY(::close(sock));
			else if(!taskPool->addVerify(tcpTask))
			{
				//得到了一个正确连接，添加到验证队列中
				SAFE_DELETE(tcpTask);
			}
		}

		void final()
		{
			zNetService::final();
		}

		/**
		 * \brief 区服务器端口
		 */
		unsigned short bindport;
}

int main(int argc, char **argv)
{
	Zebra::logger=new zLogger("TCPServer");

	//设置缺省参数
	Zebra::global["bindport"] = "9903";
	Zebra::global["logfilename"] = "/tmp/infoserver.log";
	Zebra::global["dbCount"] = "16";
	Zebra::global["tableCount"] = "16";

	TCPServer::getInstance().main();
	TCPServer::delInstance();


	return 0;
}

