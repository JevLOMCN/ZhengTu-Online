/**
 * \file
 * \version  $Id: DBAccessClient.h  $
 * \author  
 * \date 
 * \brief 定义登陆服务器客户端
 *
 * 
 */

#ifndef _DBAccessClient_h_
#define _DBAccessClient_h_

#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"

class DBAccessClient : public zTCPClientTask
{

	public:

		DBAccessClient(
				const std::string &ip,
				const unsigned short port);
		~DBAccessClient();

		int checkRebound();
		void addToContainer();
		void removeFromContainer();
		bool connect();
		bool msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);

	private:

		bool msgParse_loginServer(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);

};

#endif

