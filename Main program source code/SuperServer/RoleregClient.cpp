/**
 * \file
 * \version  $Id: RoleregClient.cpp  $
 * \author  
 * \date 
 * \brief 定义角色名称唯一性验证服务连接的客户端
 */

#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"
#include "RoleregClient.h"
#include "SuperServer.h"
#include "SuperCommand.h"
#include "ServerManager.h"
#include "RoleregClientManager.h"
#include "RoleregCommand.h"

/**
 * \brief 构造函数
 * \param ip 服务器地址
 * \param port 服务器端口
 */
RoleregClient::RoleregClient(
		const std::string &ip, 
		const unsigned short port) : zTCPClientTask(ip, port, false), netType(NetType_near)
{
}

/**
 * \brief 析构函数
 *
 */
RoleregClient::~RoleregClient()
{
}

int RoleregClient::checkRebound()
{
	int retcode = pSocket->recvToBuf_NoPoll();
	if (retcode > 0)
	{
		unsigned char pstrCmd[zSocket::MAX_DATASIZE];
		int nCmdLen = pSocket->recvToCmd_NoPoll(pstrCmd, sizeof(pstrCmd));
		if (nCmdLen <= 0)
			//这里只是从缓冲取数据包，所以不会出错，没有数据直接返回
			return 0;
		else
		{
			using namespace Cmd::RoleReg;

			t_LoginRoleReg_OK *ptCmd = (t_LoginRoleReg_OK *)pstrCmd;
			if (CMD_LOGIN == ptCmd->cmd
					&& PARA_LOGIN_OK == ptCmd->para)
			{
				Zebra::logger->debug("登陆roleRegServer成功，收到区的编号：%u(%u, %u), %s, %u",
						ptCmd->gameZone.id,
						ptCmd->gameZone.game,
						ptCmd->gameZone.zone,
						ptCmd->name,
						ptCmd->netType);
				netType = (ptCmd->netType == 0 ? NetType_near : NetType_far);
				/*
				   SuperService::getInstance().setZoneID(ptCmd->gameZone);
				   SuperService::getInstance().setZoneName(ptCmd->name);
				 */
				return 1;
			}
			else
			{
				Zebra::logger->error("登陆roleRegServer失败");
				return -1;
			}
		}
	}
	else
		return retcode;
}

void RoleregClient::addToContainer()
{
	RoleregClientManager::getInstance().add(this);
}

void RoleregClient::removeFromContainer()
{
	RoleregClientManager::getInstance().remove(this);
}

bool RoleregClient::connect()
{
	if (!zTCPClientTask::connect())
		return false;

	using namespace Cmd::RoleReg;
	t_LoginRoleReg cmd;
	strcpy(cmd.strIP, SuperService::getInstance().getIP());
	cmd.port = SuperService::getInstance().getPort();
	return sendCmd(&cmd, sizeof(cmd));
}

bool RoleregClient::msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::RoleReg;

	switch(ptNullCmd->cmd)
	{
		case CMD_ROLEREG:
			{
				switch(ptNullCmd->para)
				{
					case PARA_CHARNAME_ROLEREG:
						{
							using namespace Cmd::Super;
							t_Charname_Rolereg *ptCmd = (t_Charname_Rolereg *)ptNullCmd;
							if (ptCmd->state & ROLEREG_STATE_TEST)
							{
								t_Charname_Gateway cmd;
								cmd.wdServerID = ptCmd->wdServerID;
								cmd.accid = ptCmd->accid;
								bcopy(ptCmd->name, cmd.name, sizeof(cmd.name));
								cmd.state = ptCmd->state;
								ServerManager::getInstance().broadcastByID(cmd.wdServerID, &cmd, sizeof(cmd));
							}
							if (ptCmd->state & ROLEREG_STATE_WRITE)
							{
								if (ptCmd->state & ROLEREG_STATE_OK)
									Zebra::logger->error("回写角色成功：%u, %s", ptCmd->accid, ptCmd->name);
								else
									Zebra::logger->error("回写角色失败：%u, %s", ptCmd->accid, ptCmd->name);
							}
							if (ptCmd->state & ROLEREG_STATE_CLEAN)
							{
								if (ptCmd->state & ROLEREG_STATE_OK)
									Zebra::logger->error("删除角色成功：%u, %s", ptCmd->accid, ptCmd->name);
								else
									Zebra::logger->error("删除角色失败：%u, %s", ptCmd->accid, ptCmd->name);
							}

							return true;
						}
						break;
				}
			}
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

