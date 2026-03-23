/**
 * \file
 * \version  $Id: RoleregClient.h  $
 * \author  
 * \date 
 * \brief 定义角色名称唯一性验证服务连接的客户端
 */

#ifndef _RoleregClient_h_
#define _RoleregClient_h_

#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"
#include "NetType.h"

/**
 * \brief 角色名称唯一性验证客户端连接类
 */
class RoleregClient : public zTCPClientTask
{

	public:

		RoleregClient(
				const std::string &ip,
				const unsigned short port);
		~RoleregClient();

		int checkRebound();
		void addToContainer();
		void removeFromContainer();
		bool connect();
		bool msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);

		const NetType getNetType() const
		{
			return netType;
		}

	private:

		NetType netType;

};

#endif

