/**
 * \file
 * \version  $Id: DBAccessClient.cpp  $
 * \author  
 * \date 
 * \brief 定义登陆服务器客户端
 *
 * 
 */

#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"
#include "DBAccessClient.h"
#include "DBAccessClientManager.h"
#include "FLCommand.h"
#include "ServerManager.h"
#include "DBAccessCommand.h"
#include "GYListManager.h"
#include "LoginManager.h"

DBAccessClient::DBAccessClient(
		const std::string &ip, 
		const unsigned short port) : zTCPClientTask(ip, port)
{
}

DBAccessClient::~DBAccessClient()
{
}

int DBAccessClient::checkRebound()
{
	Zebra::logger->trace("DBAccessClient::checkRebound");
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
			using namespace Cmd::DBAccess;

			t_LogonDBAccess *ptCmd = (t_LogonDBAccess *)pstrCmd;
			if (CMD_LOGON == ptCmd->cmd
					&& PARA_LOGON == ptCmd->para)
			{
				Zebra::logger->debug("验证成功");
				return 1;
			}
			else
			{
				Zebra::logger->error("验证失败");
				return -1;
			}
		}
	}
	else
		return retcode;
}

void DBAccessClient::addToContainer()
{
	Zebra::logger->trace("DBAccessClient::addToContainer");
	DBAccessClientManager::getInstance().add(this);
}

void DBAccessClient::removeFromContainer()
{
	Zebra::logger->trace("DBAccessClient::removeFromContainer");
	DBAccessClientManager::getInstance().remove(this);
}

bool DBAccessClient::connect()
{
	Zebra::logger->trace("DBAccessClient::connect");
	if (!zTCPClientTask::connect())
		return false;

	using namespace Cmd::DBAccess;
	t_LogonDBAccess cmd;
	return sendCmd(&cmd, sizeof(cmd));
}

bool DBAccessClient::msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	Zebra::logger->trace("DBAccessClient::msgParse");
	using namespace Cmd::DBAccess;

	switch(ptNullCmd->cmd)
	{
		case CMD_LOGINSERVER:
			if (msgParse_loginServer(ptNullCmd, nCmdLen))
				return true;
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

bool DBAccessClient::msgParse_loginServer(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	Zebra::logger->trace("DBAccessClient::msgParse_loginServer");
	using namespace Cmd::DBAccess;

	switch(ptNullCmd->para)
	{
		case PARA_LOGINSERVER_SESSIONCHECK:
			{
				const t_LoginServer_SessionCheck *ptCmd = (t_LoginServer_SessionCheck *)ptNullCmd;
				LoginManager::getInstance().verifyReturn(ptCmd->session.loginTempID, ptCmd->retcode, ptCmd->session);
				return true;
			}
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

