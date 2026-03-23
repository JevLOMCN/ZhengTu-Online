/**
 * \file
 * \version  $Id: InfoClient.h  $
 * \author  
 * \date 
 * \brief 定义服务器信息收集的客户端连接
 */

#ifndef _InfoClient_h_
#define _InfoClient_h_

#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"
#include "NetType.h"

/**
 * \brief 信息收集客户端连接类
 */
class InfoClient : public zTCPClientTask
{

	public:

		InfoClient(
				const std::string &ip,
				const unsigned short port);
		~InfoClient();

		int checkRebound();
		void addToContainer();
		void removeFromContainer();
		bool connect();
		bool msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);

		const DWORD getTempID() const
		{
			return tempid;
		}

		const NetType getNetType() const
		{
			return netType;
		}

	private:
		const DWORD tempid;
		static DWORD tempidAllocator;
		NetType netType;

};

#endif

