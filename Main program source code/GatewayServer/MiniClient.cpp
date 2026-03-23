/**
 * \file
 * \version  $Id: MiniClient.cpp  $
 * \author  
 * \date 
 * \brief 定义计费服务器连接客户端
 *
 */

#include <unistd.h>
#include <iostream>

#include "zTCPClient.h"
#include "MiniCommand.h"
#include "MiniClient.h"
#include "GatewayServer.h"
//#include "SceneClient.h"
#include "GateUserManager.h"
#include "Zebra.h"
//#include "LoginSessionManager.h"
#include "GatewayTaskManager.h"

/**
 * \brief 计费服务器连接客户端
 *
 * 一个区中只有一个计费服务器，所以这里只需要保留一个指针，不需要连接管理器之类的东东
 *
 */
MiniClient *miniClient = NULL;

/**
 * \brief 建立到Mini服务器的连接
 *
 * \return 连接是否成功
 */
bool MiniClient::connectToMiniServer()
{
	if (!connect())
	{
		Zebra::logger->error("连接Mini服务器失败");
		return false;
	}

	using namespace Cmd::Mini;
	t_LoginMini tCmd;
	tCmd.wdServerID = GatewayService::getInstance().getServerID();
	tCmd.wdServerType = GatewayService::getInstance().getServerType();

	return sendCmd(&tCmd, sizeof(tCmd));
}

/**
 * \brief 重载zThread中的纯虚函数，是线程的主回调函数，用于处理接收到的指令
 *
 */
void MiniClient::run()
{
	zTCPBufferClient::run();

	while(!GatewayService::getInstance().isTerminate())
	{
		while(!connect())
		{
			Zebra::logger->error("连接小游戏服务器失败");
			zThread::msleep(1000);
		}
		Cmd::Super::t_restart_ServerEntry_NotifyOther notify;
		notify.srcID=GatewayService::getInstance().getServerID();
		notify.dstID=this->getServerID();
		GatewayService::getInstance().sendCmdToSuperServer(&notify, sizeof(notify));
		zThread::msleep(2000);
		connect();
		using namespace Cmd::Mini;
		t_LoginMini tCmd;
		tCmd.wdServerID = GatewayService::getInstance().getServerID();
		tCmd.wdServerType = GatewayService::getInstance().getServerType();

		if(sendCmd(&tCmd, sizeof(tCmd)))
		{
			zTCPBufferClient::run();
		}
			// */
		zThread::msleep(1000);
	}
	//与Mini之间的连接断开，不需要关闭服务器
	//GatewayService::getInstance().Terminate();
}

/**
 * \brief 解析来自Mini服务器的所有指令
 *
 * \param ptNullCmd 待解析的指令
 * \param nCmdLen 待解析的指令长度
 * \return 解析是否成功
 */
bool MiniClient::msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Mini;
	using namespace Cmd;

	if (CMD_GATE == ptNullCmd->cmd)
	{
		switch(ptNullCmd->para)
		{
			default:
				break;
		}
		return false;
	}

	if (CMD_FORWARD == ptNullCmd->cmd)
	{
		switch(ptNullCmd->para)
		{
			case PARA_MINI_FORWARD_USER:
				{
					t_Mini_ForwardUser *rev = (t_Mini_ForwardUser *)ptNullCmd;
					GateUser *pUser=(GateUser *)GateUserManager::getInstance()->getUserByID(rev->id);
					if(!pUser || !pUser->sendCmd(rev->data, rev->size))
						Zebra::logger->debug("转发MINI服务器 %u的%u %u消息失败", rev->id, ((Cmd::stNullUserCmd *)rev->data)->byCmd, ((Cmd::stNullUserCmd *)rev->data)->byParam);
					return true;
				}
				break;
				/*
				   case PARA_FORWARD_MINI_TO_SCENE:
				   {
				   t_Mini_ForwardMiniToScene *rev =(t_Mini_ForwardMiniToScene*)ptNullCmd;
				   GateUser *pUser=(GateUser *)GateUserManager::getInstance()->getUserByAccID(rev->id);
				   if(!pUser || !pUser->forwardSceneMini((const Cmd::stNullUserCmd*)rev->data,(unsigned int)rev->size))
				   {
				   Zebra::logger->debug("转发MINI服务器帐号%ld的场景%u %u消息失败",rev->id,
				   ((Cmd::stNullUserCmd *)rev->data)->byCmd,((Cmd::stNullUserCmd *)rev->data)->byParam);
				   }
				   }
				   break;
				   */
		}
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

