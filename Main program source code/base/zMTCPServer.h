/**
 * \file
 * \version  $Id: zMTCPServer.h  $
 * \author  
 * \date 
 * \brief 封装TCP的服务器监听模块
 *
 * 
 */

#ifndef _zMTCPServer_h_
#define _zMTCPServer_h_

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <map>

#include "zNoncopyable.h"
#include "zSocket.h"
#include "zMutex.h"

/**
 * \brief zMTCPServer类，封装了服务器监听模块，可以方便的创建一个服务器对象，等待客户端的连接
 * 可以同时监听多个端口
 */
class zMTCPServer : private zNoncopyable
{

	public:

		typedef std::map<int, unsigned short> Sock2Port;
		typedef Sock2Port::value_type Sock2Port_value_type;
		typedef Sock2Port::iterator Sock2Port_iterator;
		typedef Sock2Port::const_iterator Sock2Port_const_iterator;

		zMTCPServer(const std::string &name);
		~zMTCPServer() ;

		bool bind(const std::string &name, const unsigned short port);
		int accept(Sock2Port &res);

	private:

		static const int T_MSEC =2100;			/**< 轮询超时，毫秒 */
		static const int MAX_WAITQUEUE = 2000;	/**< 最大等待队列 */

		std::string name;						/**< 服务器名称 */
		Sock2Port mapper;
		zMutex mlock;
#ifdef _USE_EPOLL_
		int kdpfd;
		std::vector<struct epoll_event> epfds;
#else
		std::vector<struct pollfd> pfds;
#endif

}; 


#endif
