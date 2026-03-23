/**
 * \file
 * \version  $Id: zTCPClient.h  $
 * \author  
 * \date 
 * \brief TCP客户端封装
 *
 * 
 */

#ifndef _zTCPClient_h_
#define _zTCPClient_h_

#include <pthread.h>
#include <string>

#include "zSocket.h"
#include "zThread.h"

/**
 * \brief TCP客户端
 *
 * 封装了一些TCP客户端的逻辑，比如建立连接等等，在实际应用中，需要派生这个类，并重载解析指令的函数msgParse
 *
 */
class zTCPClient : public zThread, public zProcessor
{

	public:

		/**
		 * \brief 构造函数，创建实例对象，初始化对象成员
		 *
		 *
		 * \param name 名称
		 * \param ip 地址
		 * \param port 端口
		 * \param compress 底层数据传输是否支持压缩
		 */
		zTCPClient(
				const std::string &name, 
				const std::string &ip = "127.0.0.1", 
				const unsigned short port = 80,
				const bool compress = false) 
			: zThread(name), ip(ip), port(port), pSocket(NULL), compress(compress) {};

		/**
		 * \brief 析构函数，销毁对象
		 *
		 */
		~zTCPClient() 
		{
			close();
		}

		bool connect();

		/**
		 * \brief 建立一个到服务器的TCP连接，指定服务器的IP地址和端口
		 *
		 *
		 * \param ip 服务器的IP地址
		 * \param port 服务器的端口
		 * \return 连接是否成功
		 */
		bool connect(const char *ip, const unsigned short port)
		{
			this->ip = ip;
			this->port = port;
			return connect();
		}

		/**
		 * \brief 关闭客户端连接
		 *
		 */
		virtual void close()
		{
			SAFE_DELETE(pSocket);
		}

		virtual bool sendCmd(const void *pstrCmd, const int nCmdLen);

		/**
		 * \brief 设置服务器IP地址
		 *
		 *
		 * \param ip 设置的服务器IP地址
		 */
		void setIP(const char *ip)
		{
			this->ip = ip;
		}

		/**
		 * \brief 获取服务器IP地址
		 *
		 *
		 * \return 返回地址
		 */
		const char *getIP() const
		{
			return ip.c_str();
		}

		/**
		 * \brief 设置服务器端口
		 *
		 *
		 * \param port 设置的服务器端口
		 */
		void setPort(const unsigned short port)
		{
			this->port = port;
		}

		/**
		 * \brief 获取服务器端口
		 *
		 *
		 * \return 返回端口
		 */
		const unsigned short getPort() const
		{
			return port;
		}

		virtual void run();
		//指令分析
		static CmdAnalysis analysis;

	protected:

		std::string ip;									/**< 服务器地址 */
		unsigned short port;							/**< 服务器端口 */
		zSocket *pSocket;								/**< 底层套接口 */

		const bool compress;							/**< 是否支持压缩 */

}; 

class zTCPBufferClient : public zTCPClient
{

	public:

		zTCPBufferClient(
				const std::string &name, 
				const std::string &ip = "127.0.0.1", 
				const unsigned short port = 80,
				const bool compress = false,
				const int usleep_time = 50000) 
			: zTCPClient(name, ip, port, compress), usleep_time(usleep_time), _buffered(false) { }

		void close()
		{
			sync();
			zTCPClient::close();
		}

		void run();
		bool sendCmd(const void *pstrCmd, const int nCmdLen);
		void setUsleepTime(const int utime)
		{
			usleep_time = utime;
		}

	private :

		bool ListeningRecv();
		bool ListeningSend();
		void sync();

		int usleep_time;
		volatile bool _buffered;

};

#endif
