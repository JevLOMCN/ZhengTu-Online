/**
 * \file
 * \version  $Id: BillTask.cpp  $
 * \author  
 * \date 
 * \brief 实现读档连接类
 *
 * 
 */


#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <ext/hash_map>

#include "zTCPServer.h"
#include "zTCPTask.h"
#include "zService.h"
#include "BillTask.h"
#include "Zebra.h"
#include "zDBConnPool.h"
#include "zMetaData.h"
#include "zString.h"
#include "BillCommand.h"
#include "BillServer.h"
#include "BillTaskManager.h"
#include "BillManager.h"
#include "BillCallback.h"
#include "BillSessionManager.h"
#include "BillUserManager.h"
#include "BillUser.h"


/**
 * \brief 验证登陆档案服务器的连接指令
 *
 * 如果验证不通过直接断开连接
 *
 * \param ptCmd 登陆指令
 * \return 验证是否成功
 */
bool BillTask::verifyLogin(const Cmd::Bill::t_LoginBill *ptCmd)
{
	using namespace Cmd::Bill;

	if (CMD_LOGIN == ptCmd->cmd
			&& PARA_LOGIN == ptCmd->para)
	{
		const Cmd::Super::ServerEntry *entry = BillService::getInstance().getServerEntry(ptCmd->wdServerID);
		char strIP[32];
		strncpy(strIP, getIP(), 31);
		if (entry
				&& ptCmd->wdServerType == entry->wdServerType
				&& 0 == strcmp(strIP, entry->pstrIP))
		{
			wdServerID = ptCmd->wdServerID;
			wdServerType = ptCmd->wdServerType;
			return true;
		}
	}

	return false;
}

/**
 * \brief 等待接受验证指令并进行验证
 *
 * 实现虚函数<code>zTCPTask::verifyConn</code>
 *
 * \return 验证是否成功，或者超时
 */
int BillTask::verifyConn()
{
	int retcode = mSocket.recvToBuf_NoPoll();
	if (retcode > 0)
	{
		unsigned char pstrCmd[zSocket::MAX_DATASIZE];
		int nCmdLen = mSocket.recvToCmd_NoPoll(pstrCmd, sizeof(pstrCmd));
		if (nCmdLen <= 0)
			//这里只是从缓冲取数据包，所以不会出错，没有数据直接返回
			return 0;
		else
		{
			using namespace Cmd::Bill;
			if (verifyLogin((t_LoginBill *)pstrCmd))
			{
				Zebra::logger->debug("客户端连接通过验证");
				veriry_ok=true;
				return 1;
			}
			else
			{
				Zebra::logger->error("客户端连接验证失败");
				return -1;
			}
		}
	}
	else
		return retcode;
}

bool BillTask::checkRecycle()
{
	if(recycle_state == 0)
	{
		return false;
	}
	if(recycle_state == 1)
	{
		BillUserManager::getInstance()->removeUserByGatewayID(this);
		recycle_state=2;
		return true;
	}
	return true;
}
/**
 * \brief 确认一个服务器连接的状态是可以回收的
 *
 * 当一个连接状态是可以回收的状态，那么意味着这个连接的整个生命周期结束，可以从内存中安全的删除了：）<br>
 * 实现了虚函数<code>zTCPTask::recycleConn</code>
 *
 * \return 是否可以回收
 */
int BillTask::recycleConn()
{
	if(veriry_ok)
	{
		switch(recycle_state)
		{
			case 0:
				{
					recycle_state=1;
					return 0;
				}
				break;
			case 1:
				{
					return 0;
				}
				break;
			case 2:
				{
					return 1;
				}
				break;
		}
	}
	return 1;
}
/*
{

		BillUserManager::getInstance()->removeUserByGatewayID(wdServerID);
	return 1;
}
// */

bool BillTask::uniqueAdd()
{
	return BillTaskManager::getInstance().uniqueAdd(this);
}

bool BillTask::uniqueRemove()
{
	return BillTaskManager::getInstance().uniqueRemove(this);
}

/**
 * \brief 解析来自各个服务器连接的指令
 *
 * \param ptNullCmd 待处理的指令
 * \param nCmdLen 指令长度
 * \return 处理是否成功
 */
bool BillTask::msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	return MessageQueue::msgParse(ptNullCmd , nCmdLen);
}


bool BillTask::cmdMsgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Bill;

	switch(ptNullCmd->cmd)
	{
		case CMD_GATE:
			{
				switch(ptNullCmd->para)
				{
					case PARA_GATE_LOGOUT:
						{
							t_Logout_Gateway *ptCmd = (t_Logout_Gateway *)ptNullCmd;

							Zebra::logger->debug("账号退出登陆 %u,%u", ptCmd->accid,ptCmd->loginTempID);
							BillUser *pUser=BillUserManager::getInstance()->getUserByID(ptCmd->accid);
							if(pUser)
							{
								pUser->logout(ptCmd->loginTempID);
							}

							return true;
						}
						break;
					case PARA_GATE_LOGINVERIFY:
						{
							t_LoginVerify_Gateway *ptCmd = (t_LoginVerify_Gateway *)ptNullCmd;
							t_LoginVerify_Gateway_Return tCmd;

							tCmd.accid = ptCmd->accid;
							tCmd.loginTempID = ptCmd->loginTempID;
							BillUser *pUser=BillUserManager::getInstance()->getUserByID(ptCmd->accid);
							if(pUser && pUser->login(ptCmd->loginTempID))
							{
								//验证成功
								tCmd.retcode = 1;
							}
							else
							{
								//验证失败
								tCmd.retcode = 0;
							}

							return sendCmd(&tCmd, sizeof(tCmd));
						}
						/*
						   case PARA_GATE_QUERY_GOLD:
						   {
						   query_gold((Cmd::Bill::t_Query_Gold_GateMoney*)ptNullCmd);
						   return true;
						   }
						   break;
						   case PARA_GATE_TRADE_GOLD: 
						   {
						   trade_gold((Cmd::Bill::t_Trade_Gold_GateMoney*)ptNullCmd);
						   return true;
						   }
						   break;
						   case PARA_GATE_CHANGE_GOLD:
						   {
						   return true;
						   }
						   break;
						// */
					default:
						break;
				}
			}
			break;
		case CMD_FORWARD:
			{
				switch(ptNullCmd->para)
				{
					case PARA_SCENE_FORWARD_BILL:
						{
							t_Scene_ForwardBill *ptCmd = (t_Scene_ForwardBill *)ptNullCmd;
							BillUser *pUser=BillUserManager::getInstance()->getUserByID(ptCmd->dwAccid);
							if(pUser)
							{
								pUser->usermsgParseScene((Cmd::t_NullCmd *)ptCmd->data,ptCmd->size);
							}
						}
						break;
					case PARA_FORWARD_BILL:
						{
							t_Bill_ForwardBill *ptCmd = (t_Bill_ForwardBill *)ptNullCmd;
							BillUser *pUser=BillUserManager::getInstance()->getUserByID(ptCmd->dwAccid);
							if(pUser)
							{
								pUser->usermsgParse((Cmd::t_NullCmd *)ptCmd->data,ptCmd->size);
							}
						}
						break;
					default:
					break;
				}
				return true;
			}
			break;
		case CMD_REDEEM:
			{
				//补偿金币
				switch(ptNullCmd->para)
				{
					case PARA_REQUEST_GATE_REDEEM_GOLD:
						{
							t_Request_Redeem_Gold_Gateway *ptCmd = (t_Request_Redeem_Gold_Gateway *)ptNullCmd;
							BillUser *pUser=BillUserManager::getInstance()->getUserByID(ptCmd->accid);
							if(pUser)
							{
								if(!ptCmd->point)
								{
									Zebra::logger->debug("兑换指令点数为0(%s)",ptCmd->account);
									return true;
								}
								BillData bd;
								bzero(&bd, sizeof(bd));
								//strncpy(bd.account, ptCmd->account, Cmd::UserServer::ID_MAX_LENGTH);
								bd.uid=ptCmd->accid;
								bd.at = Cmd::UserServer::AT_CONSUME;
								bd.point = ptCmd->point;
								strncpy(bd.ip,pUser->getIp(),sizeof(bd.ip));

								if (Bill_action(&bd))
								{
									if(!pUser->begin_tid(bd.tid))
									{
										Zebra::logger->debug("添加兑换金币流水错误%s",ptCmd->account);
									}
									else
									{
										return true;
									}
								}
								else
								{
									Zebra::logger->debug("兑换金币错误Bill_action%s",ptCmd->account);
								}
								t_Redeem_Gold_Gateway rgg; 
								strncpy(rgg.account, ptCmd->account, Cmd::UserServer::ID_MAX_LENGTH);
								rgg.accid=ptCmd->accid;			        /// 账号编号
								rgg.charid=ptCmd->charid;				/// 角色ID
								rgg.dwGold=0;				/// 	当前拥有金币数
								rgg.dwBalance=0;				/// 	当前拥有金币数
								rgg.byReturn= Cmd::REDEEM_FAIL;	//返回类型
								sendCmd(&rgg, sizeof(rgg));
							}
							else
							{
								Zebra::logger->debug("收到兑换金币指令,但是用户不存在%s(%d)",ptCmd->account,ptCmd->accid);
							}
							return true;
						}
						break;
					case PARA_REQUEST_GATE_REDEEM_MONTH_CARD:
						{
							t_Request_Redeem_MonthCard_Gateway *ptCmd = (t_Request_Redeem_MonthCard_Gateway *)ptNullCmd;
							BillUser *pUser=BillUserManager::getInstance()->getUserByID(ptCmd->accid);
							if(pUser)
							{
								BillData bd;
								bzero(&bd, sizeof(bd));
								//strncpy(bd.account, ptCmd->account, Cmd::UserServer::ID_MAX_LENGTH);
								bd.uid=ptCmd->accid;
								bd.at = Cmd::UserServer::AT_MCARD;
								strncpy(bd.ip,pUser->getIp(),sizeof(bd.ip));

								if (Bill_action(&bd))
								{
									if(!pUser->begin_tid(bd.tid))
									{
										Zebra::logger->debug("添加兑换月卡流水错误%s",ptCmd->account);
									}
									else
									{
										return true;
									}
								}
								else
								{
									Zebra::logger->debug("兑换月卡错误Bill_action%s",ptCmd->account);
								}
								t_Redeem_MonthCard_Gateway rgg; 
								strncpy(rgg.account, ptCmd->account, Cmd::UserServer::ID_MAX_LENGTH);
								rgg.accid=ptCmd->accid;			        /// 账号编号
								rgg.charid=ptCmd->charid;				/// 角色ID
								rgg.dwNum=0;				/// 	当前拥有金币数
								rgg.dwBalance=0;				/// 	当前拥有金币数
								rgg.byReturn= Cmd::REDEEM_FAIL;	//返回类型
								sendCmd(&rgg, sizeof(rgg));
							}
							else
							{
								Zebra::logger->debug("收到兑换月卡指令,但是用户不存在%s(%d)",ptCmd->account,ptCmd->accid);
							}
							return true;
						}
						break;
					case PARA_GATE_REQUECT_CARD_GOLD:
						{
							 
							t_Request_Card_Gold_Gateway *ptCmd = (t_Request_Card_Gold_Gateway *)ptNullCmd;
							t_Return_Card_Gold rcg; 
							strncpy(rcg.account, ptCmd->account, Cmd::UserServer::ID_MAX_LENGTH);
							rcg.accid=ptCmd->accid;			        /// 账号编号
							rcg.charid=ptCmd->charid;				/// 角色ID
							//rcg.dwMonthCard = BillManager::getInstance().getVipTime(ptCmd->accid);	//月卡
							//rcg.dwGold = (DWORD)BillManager::getInstance().getGold(ptCmd->accid);	//金币
							rcg.byReturn= Cmd::REDEEM_SUCCESS;	//返回类型
							sendCmd(&rcg, sizeof(rcg));
							return true;
						}
						break;
					case PARA_GATE_REQUECT_POINT:
						{
							 
							t_Request_Point_Gateway *ptCmd = (t_Request_Point_Gateway *)ptNullCmd;
							BillUser *pUser=BillUserManager::getInstance()->getUserByID(ptCmd->accid);
							if(pUser)
							{
								pUser->restoregold();
								BillData bd;
								bzero(&bd, sizeof(bd));
								//strncpy(bd.account, ptCmd->account, Cmd::UserServer::ID_MAX_LENGTH);
								bd.uid=ptCmd->accid;
								bd.at = Cmd::UserServer::AT_QBALANCE;
								strncpy(bd.ip,pUser->getIp(),sizeof(bd.ip));

								if (Bill_action(&bd))
								{
									Zebra::logger->debug("tradeSN:%s",bd.tid);
									if(!pUser->begin_tid(bd.tid))
									{
										Zebra::logger->debug("添加查询剩余点数流水错误%s",ptCmd->account);
									}
									else
									{
										return true;
									}
								}
								else
								{
									Zebra::logger->debug("查询剩余点数错误Bill_action%s",ptCmd->account);
								}
								t_Return_Point rpg; 
								strncpy(rpg.account, ptCmd->account, Cmd::UserServer::ID_MAX_LENGTH);
								rpg.accid=ptCmd->accid;			        /// 账号编号
								rpg.charid=ptCmd->charid;				/// 角色ID
								rpg.dwPoint=0;				/// 	当前点数
								rpg.byReturn= Cmd::REDEEM_FAIL;	//返回类型
								sendCmd(&rpg, sizeof(rpg));
							}
							else
							{
								Zebra::logger->debug("收到查询月卡点数指令,但是用户不存在%s(%d)",ptCmd->account,ptCmd->accid);
							}
							return true;
						}
						break;
					case PARA_GATE_CONSUME_CARD:
						{
							stConSumeCardCard_Gateway *ptCmd = (stConSumeCardCard_Gateway *)ptNullCmd; 
							//Zebra::logger->debug("%s(%d)请求消费道具卡:%s",this->account,this->id,rev->cardid);
							BillUser *pUser=BillUserManager::getInstance()->getUserByID(ptCmd->accid);
							if(pUser)
							{
								pUser->restorecard();
								BillData bd;
								bzero(&bd, sizeof(bd));
								//strncpy(bd.account, ptCmd->account, Cmd::UserServer::ID_MAX_LENGTH);
								bd.uid=ptCmd->accid;
								switch(ptCmd->type)
								{
									case Cmd::ZONE_CARD_OBJECT:
										{
											bd.at = Cmd::UserServer::AT_PCARD;
										}
										break;
									case Cmd::ZONE_CARD_PROFRESSION:
										{
											bd.at = Cmd::UserServer::AT_SCARD;
										}
										break;
									default:
										break;
								}
								strncpy(bd.ip,pUser->getIp(),sizeof(bd.ip));
								bzero(bd.cardid,sizeof(bd.cardid));
								strncpy(bd.cardid,ptCmd->cardid,sizeof(ptCmd->cardid));

								if (Bill_action(&bd))
								{
									Zebra::logger->debug("tradeSN:%s",bd.tid);
									if(!pUser->begin_tid(bd.tid))
									{
										Zebra::logger->debug("添加消费道具卡流水错误%s",ptCmd->account);
									}
									else
									{
										return true;
									}
								}
								else
								{
									Zebra::logger->debug("消费道具卡错误Bill_action%s",ptCmd->account);
								}
								t_Return_Point rpg; 
								strncpy(rpg.account, ptCmd->account, Cmd::UserServer::ID_MAX_LENGTH);
								rpg.accid=ptCmd->accid;			        /// 账号编号
								rpg.dwPoint=0;				/// 	当前点数
								rpg.byReturn= Cmd::REDEEM_FAIL;	//返回类型
								sendCmd(&rpg, sizeof(rpg));
							}
							else
							{
								Zebra::logger->debug("收到查询月卡点数指令,但是用户不存在%s(%d)",ptCmd->account,ptCmd->accid);
							}
							return true;
							Zebra::logger->debug("请求消费道具卡:%s",ptCmd->cardid);
							return  true;
						}
						break;
				}
			}
			break;
		default:
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

/**
 * \brief 发送命令给场景用户
 *
 * \param id 用户id
 * \param pstrCmd 命令指令
 * \param nCmdLen 命令长度
 */
bool BillTask::sendCmdToScene(DWORD id, const void *pstrCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Bill;
	using namespace Cmd;
	
	BYTE buf[zSocket::MAX_DATASIZE];
	t_Bill_ForwardBillToScene *scmd=(t_Bill_ForwardBillToScene *)buf;
	constructInPlace(scmd);
	
	scmd->id=id;
	scmd->size=nCmdLen;
	bcopy(pstrCmd,scmd->data,nCmdLen);
	return sendCmd(scmd, sizeof(t_Bill_ForwardBillToScene)+nCmdLen);
}

/**
 * \brief 发送命令给用户
 *
 * \param id 用户id
 * \param pstrCmd 命令指令
 * \param nCmdLen 命令长度
 */
bool BillTask::sendCmdToUser(DWORD id, const void *pstrCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Bill;
	using namespace Cmd;
	
	BYTE buf[zSocket::MAX_DATASIZE];
	t_Bill_ForwardUser *scmd=(t_Bill_ForwardUser *)buf;
	constructInPlace(scmd);
	
	scmd->dwAccid=id;
	scmd->size=nCmdLen;
	bcopy(pstrCmd,scmd->data,nCmdLen);
	return sendCmd(scmd, sizeof(t_Bill_ForwardUser)+nCmdLen);
}

 

/**
 * \brief 查询金币余额
 *
 *
 * \param cmd 查询命令
 */
	/*
void BillTask::query_gold(const Cmd::Bill::t_Query_Gold_GateMoney* cmd)
{
	RecordSet* recordset = NULL;

	Cmd::stReturnQueryGold send;
	send.type = Cmd::TYPE_QUERY;
	
	FieldSet* balance = BillService::metaData->getFields("BALANCE");

	Record where;
	std::ostringstream oss;
	oss << "accid=" << cmd->accid;
	where.put("accid", oss.str());
	oss.str("");

	oss << "charid=" << cmd->charid;
	where.put("charid", oss.str());

	if (balance)
	{

		connHandleID handle = BillService::dbConnPool->getHandle();

		if ((connHandleID)-1 != handle)
		{
			recordset = BillService::dbConnPool->exeSelect(handle, balance, NULL, &where);

			if (recordset && !recordset->empty())
			{
				Record* result = recordset->get(0);
				if (result)
				{
					send.gold = result->get("gold");
					send.state = Cmd::QUERY_SUCCESS;
				}
				else
				{
					send.state = Cmd::QUERY_EMPTY;
				}
				SAFE_DELETE(recordset)
			}
			else
			{
				send.state = Cmd::QUERY_EMPTY;
			}
		}
		else
		{
			send.state = Cmd::QUERY_BUSY;
		}
	}
	else
	{
		send.state = Cmd::QUERY_FAIL;
	}

	sendCmdToUser(cmd->accid, &send, sizeof(send));	
	return;
}
	// */


/**
 * \brief 金币消费
 *
 *
 * \param cmd 金币交易命令 
 * \return 
 */
/*
void BillTask::trade_gold(const Cmd::Bill::t_Trade_Gold_GateMoney* cmd)
{
	RecordSet* recordset = NULL;
	Cmd::Bill::t_Return_Trade_Gold_GateMoney send;

	double gold = 0.0;     // 余额
	double allconsum = 0.0; // 累积消费额
	char   account[Cmd::UserServer::ID_MAX_LENGTH+1] = "";
	
	FieldSet* balance = BillService::metaData->getFields("BALANCE");

	send.accid 	= cmd->accid;
	send.charid 	= cmd->charid;
	send.object_id	= cmd->object_id;
	send.object_num	= cmd->object_num;

	Record where;
	std::ostringstream oss;
	oss << "accid=" << cmd->accid;
	where.put("accid", oss.str());
	oss.str("");

	oss << "charid=" << cmd->charid;
	where.put("charid", oss.str());

	mlock.lock();

	if (balance)
	{

		connHandleID handle = BillService::dbConnPool->getHandle();

		if ((connHandleID)-1 != handle)
		{
			recordset = BillService::dbConnPool->exeSelect(handle, balance, NULL, &where);

			if (recordset && !recordset->empty())
			{
				Record* result = recordset->get(0);
				if (result)
				{
					gold = result->get("gold");
					allconsum = result->get("allconsum");
					strncpy(account, result->get("account"), Cmd::UserServer::ID_MAX_LENGTH);

					if (gold > 0.001)
					{
						send.state = Cmd::Bill::QUERY_SUCCESS;
					}
					else
					{
						send.state = Cmd::Bill::QUERY_EMPTY;
					}
				}
				else
				{
					send.state = Cmd::Bill::QUERY_EMPTY;
				}
				SAFE_DELETE(recordset)
			}
			else
			{
				send.state = Cmd::Bill::QUERY_EMPTY;
			}

			BillService::dbConnPool->putHandle(handle);
		}
		else
		{
			send.state = Cmd::Bill::QUERY_BUSY;
		}

	}
	else
	{
		send.state = Cmd::Bill::QUERY_FAIL;
	}


	if (send.state == Cmd::Bill::QUERY_SUCCESS)
	{// 查询成功
		if (send.gold >= cmd->gold)
		{// 余额充足，扣除金币，并做交易日志
			connHandleID handle = BillService::dbConnPool->getHandle();

			if ((connHandleID)-1 != handle)
			{
				Record rec;
				rec.put("gold", gold - cmd->gold);
				rec.put("allconsum", allconsum + cmd->gold);
			
				if((unsigned int)-1 == BillService::dbConnPool->exeUpdate(handle, balance, &rec, &where))
				{
					send.state = Cmd::Bill::QUERY_FAIL;	
				}
				else
				{
					this->trade_log(cmd, account, gold-cmd->gold);
					send.gold = gold - cmd->gold;
					send.state = Cmd::Bill::QUERY_SUCCESS;
				}
				
				BillService::dbConnPool->putHandle(handle);
			}
			else
			{
				send.state = Cmd::Bill::QUERY_BUSY;
			}
			
		}
	}

	mlock.unlock();

	sendCmd(&send, sizeof(send));	
	return;
}
// */


/**
 * \brief 点数换金币
 *
 *
 * \param cmd 查询命令
 */
/*
void BillTask::redeem_gold(const Cmd::Bill::t_Change_Gold_GateMoney* cmd)
{
	BillData bd;
	bzero(&bd, sizeof(bd));
	strncpy(bd.account, cmd->account, Cmd::UserServer::ID_MAX_LENGTH);
	bd.at = AT_CONSUME;
	bd.consume.point = cmd->point;
	
	if (Bill_action(&bd))
	{
		BillSessionManager::getInstance().add(bd.tid, cmd, this);
	}
	else
	{
		Cmd::Bill::t_Return_Query_Gold_GateMoney send;
		send.gold = 0;
		send.state = Cmd::Bill::QUERY_FAIL;
		sendCmd(&send, sizeof(send));
	}
}
// */


/**
 * \brief 记录交易日志
 *
 *
 * \param  cmd 交易命令
 * \param  account 用户帐号
 * \param  gold  帐户余额
 *
 * \return 
 */
/*
void BillTask::trade_log(const Cmd::Bill::t_Trade_Gold_GateMoney* cmd, const char* account, double gold)
{
	// 帐号,ACCID，CHARID--(物品ID，物品数量，交易的金币数量, 金币余额)
	BillService::tradelog->trace("金币交易:----------------------------------------");
	BillService::tradelog->trace("金币交易:%s,%d,%d,%d,%d,%d,%d", 
			account,
			cmd->accid,
			cmd->charid,
			cmd->object_id,
			cmd->object_num,
			cmd->gold,
			gold
			);
}
// */

