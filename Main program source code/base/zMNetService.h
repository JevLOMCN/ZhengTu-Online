/**
 * \file
 * \version  $Id: zMNetService.h  $
 * \author  
 * \date 
 * \brief 实现网络服务器的框架代码
 *
 * 这个类比较通用一点，再创建比较一般的网络服务器程序的时候是比较有用
 * 
 */

#ifndef _zMNetService_h_
#define _zMNetService_h_

#include <iostream>
#include <string>
#include <ext/numeric>

#include "zService.h"
#include "zThread.h"
#include "zSocket.h"
#include "zMTCPServer.h"

/**
 * \brief 网络服务器类
 * 实现了网络服务器框架代码，这个类比较通用一点
 */
class zMNetService : public zService
{

	public:

		/**
		 * \brief 虚析构函数
		 */
		virtual ~zMNetService() { instance = NULL; };

		/**
		 * \brief 根据得到的TCP/IP连接获取一个连接任务
		 * \param sock TCP/IP套接口
		 * \param srcPort 由于服务器绑定了多个端口，这个参数指定连接来自那个绑定端口
		 * \return 新建立的连接任务
		 */
		virtual void newTCPTask(const int sock, const unsigned short srcPort) = 0;

		/**
		 * \brief 绑定服务到某个端口
		 * \param name 待绑定端口名称
		 * \param port 待绑定的端口
		 * \return 绑定是否成功
		 */
		bool bind(const std::string &name, const unsigned short port)
		{
			if (tcpServer)
				return tcpServer->bind(name, port);
			else
				return false;
		}

	protected:

		/**
		 * \brief 构造函数
		 * 受保护的构造函数，实现了Singleton设计模式，保证了一个进程中只有一个类实例
		 * \param name 名称
		 */
		zMNetService(const std::string &name) : zService(name)
		{
			instance = this;

			serviceName = name;
			tcpServer = NULL;
		}

		bool init();
		bool serviceCallback();
		void final();

	private:

		static zMNetService *instance;		/**< 类的唯一实例指针，包括派生类，初始化为空指针 */
		std::string serviceName;			/**< 网络服务器名称 */
		zMTCPServer *tcpServer;				/**< TCP服务器实例指针 */

};

#endif

