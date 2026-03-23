/**
 * \file
 * \version  $Id: BillClient.cpp $
 * \author  
 * \date 
 * \brief 定义计费服务器连接客户端
 *
 */

#include <unistd.h>
#include <iostream>

#include "zTCPClient.h"
#include "BillCommand.h"
#include "BillClient.h"
#include "GatewayServer.h"
#include "SceneClient.h"
#include "GateUserManager.h"
#include "Zebra.h"
#include "LoginSessionManager.h"
#include "GatewayTaskManager.h"

/**
 * \brief 计费服务器连接客户端
 *
 * 一个区中只有一个计费服务器，所以这里只需要保留一个指针，不需要连接管理器之类的东东
 *
 */
BillClient *accountClient = NULL;

/**
 * \brief 建立到Bill服务器的连接
 *
 * \return 连接是否成功
 */
bool BillClient::connectToBillServer()
{
	if (!connect())
	{
		Zebra::logger->error("连接Bill服务器失败");
		return false;
	}

	using namespace Cmd::Bill;
	t_LoginBill tCmd;
	tCmd.wdServerID = GatewayService::getInstance().getServerID();
	tCmd.wdServerType = GatewayService::getInstance().getServerType();

	return sendCmd(&tCmd, sizeof(tCmd));
}

/**
 * \brief 重载zThread中的纯虚函数，是线程的主回调函数，用于处理接收到的指令
 *
 */
void BillClient::run()
{
	zTCPBufferClient::run();

	while(!GatewayService::getInstance().isTerminate())
	{
		while(!connect())
		{
			Zebra::logger->error("连接计费服务器失败");
			zThread::msleep(1000);
		}
		Cmd::Super::t_restart_ServerEntry_NotifyOther notify;
		notify.srcID=GatewayService::getInstance().getServerID();
		notify.dstID=this->getServerID();
		GatewayService::getInstance().sendCmdToSuperServer(&notify, sizeof(notify));
		zThread::msleep(2000);
		connect();
		using namespace Cmd::Bill;
		t_LoginBill tCmd;
		tCmd.wdServerID = GatewayService::getInstance().getServerID();
		tCmd.wdServerType = GatewayService::getInstance().getServerType();

		if(sendCmd(&tCmd, sizeof(tCmd)))
		{
			zTCPBufferClient::run();
		}
			// */
		zThread::msleep(1000);
	}
	//与Bill之间的连接断开，需要关闭服务器
	GatewayService::getInstance().Terminate();
}

/**
 * \brief 解析来自Bill服务器的所有指令
 *
 * \param ptNullCmd 待解析的指令
 * \param nCmdLen 待解析的指令长度
 * \return 解析是否成功
 */
bool BillClient::msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Bill;
	using namespace Cmd;

	if (CMD_GATE == ptNullCmd->cmd)
	{
		switch(ptNullCmd->para)
		{
			case PARA_GATE_LOGINVERIFY_RETURN:
				{
					t_LoginVerify_Gateway_Return *ptCmd = (t_LoginVerify_Gateway_Return *)ptNullCmd;

					if (ptCmd->retcode)
					{
						Zebra::logger->debug("登陆计费验证通过%u",ptCmd->accid);
						GatewayTaskManager::getInstance().accountVerifyOK(ptCmd->accid, true);
					}
					else
					{
						Zebra::logger->error("登陆计费验证没有通过%u",ptCmd->accid);
						GatewayTaskManager::getInstance().accountVerifyOK(ptCmd->accid, false);
					}

					return true;
				}
				break;
			case PARA_GATE_NEWSESSION:
				{
					t_NewSession_Gateway *ptCmd = (t_NewSession_Gateway *)ptNullCmd;
					Cmd::Super::t_NewSession_Gateway tCmd;

					Zebra::logger->info("登陆信息：%u, %u, %s, %u", ptCmd->session.accid, ptCmd->session.loginTempID, ptCmd->session.pstrIP, ptCmd->session.wdPort);
					//Zebra::logger->info("get key：%u, %u, %u, %u, %u, %u, %u, %u", ptCmd->session.des_key[0], ptCmd->session.des_key[1], ptCmd->session.des_key[2], ptCmd->session.des_key[3], ptCmd->session.des_key[4], ptCmd->session.des_key[5], ptCmd->session.des_key[6], ptCmd->session.des_key[7]);
					LoginSessionManager::getInstance().put(ptCmd->session);
					tCmd.session = ptCmd->session;
					//bcopy(&ptCmd->session, &tCmd.session, sizeof(tCmd.session));

					return GatewayService::getInstance().sendCmdToSuperServer(&tCmd, sizeof(tCmd));
				}
				break;
			default:
				break;
		}
		return false;
	}
	
	if (CMD_REDEEM == ptNullCmd->cmd)
	{
		switch(ptNullCmd->para)
		{
			case PARA_GATE_REDEEM_GOLD:
				{
					t_Redeem_Gold_Gateway *ptCmd=(t_Redeem_Gold_Gateway *)ptNullCmd; 
					GateUser *pUser=GateUserManager::getInstance()->getUserByAccID(ptCmd->accid);
					if(pUser)
					{
						stRedeemGold rd;
						rd.dwNum = ptCmd->dwGold;
						rd.dwBalance = ptCmd->dwBalance;
						rd.byReturn = ptCmd->byReturn;
						pUser->forwardScene(&rd,sizeof(rd));
						//Zebra::logger->debug("Bill转发到场景的兑换金币指令");
					}
					return true;
				}
				break;
			case PARA_GATE_REDEEM_MONTH_CARD:
				{
					t_Redeem_MonthCard_Gateway *ptCmd=(t_Redeem_MonthCard_Gateway *)ptNullCmd; 
					GateUser *pUser=GateUserManager::getInstance()->getUserByAccID(ptCmd->accid);
					if(pUser)
					{
						stRedeemMonthCard rd;
						rd.byReturn = ptCmd->byReturn;
						rd.dwNum = ptCmd->dwNum;
						rd.dwBalance = ptCmd->dwBalance;
						pUser->forwardScene(&rd,sizeof(rd));
						if(ptCmd->byReturn ==Cmd::REDEEM_SUCCESS) 
						{
							pUser->setVip(ptCmd->dwNum>0?true:false);
						}
						//Zebra::logger->debug("Bill转发到场景的兑换月卡指令");
					}
					return true;
				}
				break;
			case PARA_GATE_RETURN_CARD_GOLD:
				{
					t_Return_Card_Gold *ptCmd=(t_Return_Card_Gold *)ptNullCmd; 
					GateUser *pUser=GateUserManager::getInstance()->getUserByAccID(ptCmd->accid);
					if(pUser)
					{
						stReturnCardAndGold rc;
						rc.byReturn = ptCmd->byReturn;
						rc.dwGold = ptCmd->dwGold;
						rc.dwMonthCard = ptCmd->dwMonthCard;
						pUser->forwardScene(&rc,sizeof(rc));
						if(ptCmd->byReturn ==Cmd::REDEEM_SUCCESS) 
						{
							pUser->setVip(ptCmd->dwMonthCard>0?true:false);
						}
						//Zebra::logger->debug("Bill转发到场景的查询金币月卡指令");
					}
					return true;
				}
				break;
			case PARA_GATE_RETURN_POINT:
				{
					 
					t_Return_Point *ptCmd=(t_Return_Point *)ptNullCmd; 
					GateUser *pUser=GateUserManager::getInstance()->getUserByAccID(ptCmd->accid);
					if(pUser)
					{
						stReturnRequestPoint rc;
						rc.byReturn = ptCmd->byReturn;
						rc.dwPoint = ptCmd->dwPoint;
						pUser->forwardScene(&rc,sizeof(rc));
					}
					return true;
				}
				break;
			case PARA_GATE_RETURN_CARD:
				{
					t_Return_ObjCard *ptCmd=(t_Return_ObjCard *)ptNullCmd; 
					GateUser *pUser=GateUserManager::getInstance()->getUserByAccID(ptCmd->accid);
					if(pUser)
					{
						stReturnConSumeCardCard  ccc;
						ccc.byReturn = ptCmd->byReturn;
						ccc.byType = ptCmd->subatt;
						ccc.balance = ptCmd->balance;
						pUser->forwardScene(&ccc,sizeof(ccc));
					}
				}
				break;
			default:
				break;
		}
		return false;
	}
	if (CMD_FORWARD == ptNullCmd->cmd)
	{
		switch(ptNullCmd->para)
		{
			case PARA_FORWARD_USER:
				{
					t_Bill_ForwardUser *rev=(t_Bill_ForwardUser *)ptNullCmd;
					GateUser *pUser=(GateUser *)GateUserManager::getInstance()->getUserByAccID(rev->dwAccid);
					if(!pUser ||  !pUser->sendCmd(rev->data,rev->size))
						Zebra::logger->debug("转发BILL服务器帐号%ld的%u %u消息失败",rev->dwAccid,
								((Cmd::stNullUserCmd *)rev->data)->byCmd,((Cmd::stNullUserCmd *)rev->data)->byParam);
					else
					{       
						//Zebra::logger->debug("转发BILL服务器帐号%ld的%u %u消息成功",rev->dwID,
						//		((Cmd::stNullUserCmd *)rev->data)->byCmd,((Cmd::stNullUserCmd *)rev->data)->byParam);
					}
					return true;
				}
				break;
			case PARA_FORWARD_BILL_TO_SCENE:
				{
					t_Bill_ForwardBillToScene *rev =(t_Bill_ForwardBillToScene*)ptNullCmd;
					GateUser *pUser=(GateUser *)GateUserManager::getInstance()->getUserByAccID(rev->id);
					if(!pUser || !pUser->forwardSceneBill((const Cmd::stNullUserCmd*)rev->data,(unsigned int)rev->size))
					{
						Zebra::logger->debug("转发BILL服务器帐号%ld的场景%u %u消息失败",rev->id,
								((Cmd::stNullUserCmd *)rev->data)->byCmd,((Cmd::stNullUserCmd *)rev->data)->byParam);
					}
				}
				break;

		}
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

