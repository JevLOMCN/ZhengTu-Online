#include "BillUser.h"
#include "zMetaData.h"
#include "BillServer.h"
#include "StockConsign.h"
BillUser::BillUser(DWORD acc,DWORD logintemp,const char *count,const char *ip,BillTask *gate) : timestamp()
{
	state = WAIT_LOGIN;
	bzero(&tid,sizeof(tid));
	bzero(account,sizeof(account));
	gold=0;
	money=0;
	all_in_gold=0;
	all_in_money=0;
	all_tax_gold=0;
	all_tax_money=0;
	all_out_gold=0;
	all_out_money=0;
	goldlistNum=0;
	moneylistNum=0;
	id=acc;
	tempid=logintemp;
	strncpy(account,count,Cmd::UserServer::ID_MAX_LENGTH);
	gatewaytask=gate;
	bzero(password,sizeof(password));
	stock_login=false; 
	strncpy(client_ip,ip,sizeof(client_ip));
}




void BillUser::increaseGoldListNum()
{
	/*
	if(goldlistNum <10)
	{
		goldlistNum ++; 
		connHandleID handle = BillService::dbConnPool->getHandle();
		if ((connHandleID)-1 != handle)
		{
			FieldSet* balance = BillService::metaData->getFields("BALANCE");
			Record column,where;                           
			std::ostringstream oss;         
			if(balance)
			{
				oss <<"accid="<<this->id;
				column.put("goldlist",goldlistNum);
				where.put("accid", oss.str());
				if((unsigned int)-1 == BillService::dbConnPool->exeUpdate(handle, 
							balance, &column, &where))
				{
				}
			}
			//Zebra::logger->debug("%u交易金子单数量:%d",this->id,goldlistNum);
			BillService::dbConnPool->putHandle(handle);
		}
	}
	// */
}
void BillUser::increaseMoneyListNum()
{
	/*
	if(moneylistNum <10)
	{
		moneylistNum ++; 
		connHandleID handle = BillService::dbConnPool->getHandle();
		if ((connHandleID)-1 != handle)
		{
			FieldSet* balance = BillService::metaData->getFields("BALANCE");
			Record column,where;                           
			std::ostringstream oss;         
			if(balance)
			{
				oss <<"accid="<<this->id;
				where.put("accid", oss.str());
				column.put("moneylist",moneylistNum);
				if((unsigned int)-1 == BillService::dbConnPool->exeUpdate(handle, 
							balance, &column, &where))
				{
				}
			}
			//Zebra::logger->debug("%u交易银子单数量:%d",this->id,moneylistNum);
			BillService::dbConnPool->putHandle(handle);
		}
	}
	// */
}
void BillUser::decreaseGoldListNum()
{
	/*
	if(goldlistNum > 0)
	{
		goldlistNum --; 
		connHandleID handle = BillService::dbConnPool->getHandle();
		if ((connHandleID)-1 != handle)
		{
			FieldSet* balance = BillService::metaData->getFields("BALANCE");
			Record column,where;                           
			std::ostringstream oss;         
			if(balance)
			{
				oss <<"accid="<<this->id;
				where.put("accid", oss.str());
				column.put("goldlist",goldlistNum);
				if((unsigned int)-1 == BillService::dbConnPool->exeUpdate(handle, 
							balance, &column, &where))
				{
				}
			}
			//Zebra::logger->debug("%u交易金子单数量:%d",this->id,goldlistNum);
			BillService::dbConnPool->putHandle(handle);
		}
	}
	// */
}
void BillUser::decreaseMoneyListNum()
{
	/*
	if(moneylistNum > 0)
	{
		moneylistNum --; 
		connHandleID handle = BillService::dbConnPool->getHandle();
		if ((connHandleID)-1 != handle)
		{
			FieldSet* balance = BillService::metaData->getFields("BALANCE");
			Record column,where;                           
			std::ostringstream oss;         
			if(balance)
			{
				oss <<"accid="<<this->id;
				where.put("accid", oss.str());
				column.put("moneylist",moneylistNum);
				if((unsigned int)-1 == BillService::dbConnPool->exeUpdate(handle, 
							balance, &column, &where))
				{
				}
			}
			//Zebra::logger->debug("%u交易银子单数量:%d",this->id,moneylistNum);
			BillService::dbConnPool->putHandle(handle);
		}
	}
	// */
}
bool BillUser::checkStockLogin()
{
	if(stock_login)
	{
		return true;
	}
	else
	{
		Cmd::stReturnPasswordStockIserCmd ret; 
		ret.byReturn=Cmd::STOCK_LOGIN_NOTLOGIN;
		this->sendCmdToMe(&ret,sizeof(ret));
		Zebra::logger->debug("%s(%d)请先登陆",this->account,this->id);
	}
	return false;
}
bool BillUser::restorecard()
{
	connHandleID handle = BillService::dbConnPool->getHandle();

	if ((connHandleID)-1 != handle)
	{
		FieldSet* restore = BillService::metaData->getFields("RESTORECARD");
		RecordSet* recordset = NULL;
		std::ostringstream oss;         
		Record column,where;                           
		if(restore)
		{
			oss <<"accid="<<this->id;
			where.put("accid", oss.str());
			recordset = BillService::dbConnPool->exeSelect(handle, restore, NULL, &where);
			if (recordset && !recordset->empty())
			{
				Cmd::Bill::t_Return_ObjCard send;
				send.subatt = recordset->get(0)->get("subat");
				send.balance = recordset->get(0)->get("balance");
				send.byReturn = Cmd::REDEEM_SUCCESS;
				if((unsigned int)-1 == BillService::dbConnPool->exeDelete(handle,restore, &where))
				{
					Zebra::logger->debug("数据库错误!%s",__PRETTY_FUNCTION__);
				}      
				else
				{
					send.accid = this->id;			        /// 账号编号
					strncpy(send.account , account , Cmd::UserServer::ID_MAX_LENGTH);
					this->sendCmd(&send, sizeof(send));
					BillUser::logger("道具卡",this->id,recordset->get(0)->get("tid"),send.subatt,send.balance,send.byReturn,"道具卡补偿");
				}
			}
		}
		SAFE_DELETE(recordset);
		BillService::dbConnPool->putHandle(handle);
		return true;
	}
	return false;
}
bool BillUser::restoregold()
{
	connHandleID handle = BillService::dbConnPool->getHandle();

	if ((connHandleID)-1 != handle)
	{
		FieldSet* restore = BillService::metaData->getFields("RESTOREGOLD");
		RecordSet* recordset = NULL;
		std::ostringstream oss;         
		Record column,where;                           
		if(restore)
		{
			oss <<"accid="<<this->id;
			where.put("accid", oss.str());
			recordset = BillService::dbConnPool->exeSelect(handle, restore, NULL, &where);
			if (recordset && !recordset->empty())
			{
				Cmd::Bill::t_Redeem_Gold_Gateway send;
				send.dwGold = recordset->get(0)->get("restoregold");
				if((unsigned int)-1 == BillService::dbConnPool->exeDelete(handle,restore, &where))
				{
					Zebra::logger->debug("数据库错误!%s",__PRETTY_FUNCTION__);
				}      
				else
				{
					strncpy(send.account , account , Cmd::UserServer::ID_MAX_LENGTH);
					send.accid = id;			        /// 账号编号
					send.byReturn = Cmd::REDEEM_SUCCESS;
					send.dwBalance = 0;
					this->sendCmd(&send, sizeof(send));
					BillUser::logger("金币",this->id,recordset->get(0)->get("tid"),send.dwGold,send.dwBalance,send.byReturn,"点数换金币补偿");
				}
			}
		}
		SAFE_DELETE(recordset);
		BillService::dbConnPool->putHandle(handle);
		return true;
	}
	return false;
}
bool BillUser::login(const DWORD loginTempID)
{
	if(loginTempID == this->tempid && WAIT_LOGIN == this->state)
	{
		this->state = CONF_LOGIN;
		connHandleID handle = BillService::dbConnPool->getHandle();

		if ((connHandleID)-1 != handle)
		{
			FieldSet* balance = BillService::metaData->getFields("BALANCE");
			RecordSet* recordset = NULL;
			std::ostringstream oss;         
			Record column,where;                           
			if(balance)
			{
				oss <<"accid="<<this->id;
				where.put("accid", oss.str());
				recordset = BillService::dbConnPool->exeSelect(handle, balance, NULL, &where);
				if (recordset && !recordset->empty())
				{
						this->gold = recordset->get(0)->get("gold");
						this->money = recordset->get(0)->get("money");
						this->goldlistNum = recordset->get(0)->get("goldlist");
						this->all_in_gold=recordset->get(0)->get("allgoldin");
						this->all_in_money=recordset->get(0)->get("allmoneyin");
						this->all_out_gold=recordset->get(0)->get("allgoldout");
						this->all_tax_gold=recordset->get(0)->get("goldtax");
						this->all_tax_money=recordset->get(0)->get("moneytax");
						this->all_out_money=recordset->get(0)->get("allmoneyout");
						this->moneylistNum = recordset->get(0)->get("moneylist");
						strncpy(this->password,recordset->get(0)->get("password"),MAX_PASSWORD);
				}
			}
			SAFE_DELETE(recordset)
			BillService::dbConnPool->putHandle(handle);
		}
		return true;
	}
	return false;
}
bool BillUser::logout(const DWORD loginTempID)
{
	if(loginTempID == this->tempid && CONF_LOGIN == this->state)
	{
		//退出登陆的时候，需要等待2秒钟的时间，才能允许下一次登陆
		this->state = CONF_LOGOUT;
		this->timestamp.now();
		//this->timestamp -= (session_timeout_value - 2);
		return true;
	}
	return false;
}
void BillUser::end_tid()
{
	bzero(tid,sizeof(tid));
}
bool BillUser::check_tid(const char *t)
{
	if(tid[0] && strcmp(tid,t))
	{
		Zebra::logger->debug("%d兑换金币返回时没有正确的tid(%s,%s),可能该玩家已经退出",this->id,this->tid,t);
		return false;
	}
	return true;
}
bool BillUser::query_point(const BillData* bd)
{
	if(!check_tid(bd->tid))
	{
		return false;
	}
	Cmd::Bill::t_Return_Point send;
	
	strncpy(send.account , account , Cmd::UserServer::ID_MAX_LENGTH);
	send.accid = id;			        /// 账号编号

	//send.type = Cmd::TYPE_QUERY;

	if (bd->result == Cmd::UserServer::RET_OK)
	{
		send.dwPoint = bd->balance;
		send.byReturn = Cmd::REDEEM_SUCCESS;
	}
	else
	{
		send.dwPoint = 0;
		send.byReturn = Cmd::REDEEM_FAIL;
	}
	end_tid();
	return this->sendCmd(&send, sizeof(send));
}
bool BillUser::redeem_object_card_err(const BillData* bd)
{
	connHandleID handle = BillService::dbConnPool->getHandle();
	if ((connHandleID)-1 != handle)
	{
		FieldSet* restore = BillService::metaData->getFields("RESTORECARD");
		std::ostringstream oss;         
		Record column,where;                           
		column.clear();
		column.put("accid", bd->uid);
		column.put("tid", bd->tid);
		DWORD waitgold = 0;  // 待充金币数

		if (bd->result == Cmd::UserServer::RET_OK)
		{
			column.put("sbuat",bd->subat);
			column.put("balance",bd->balance);
			if((unsigned int)-1 == BillService::dbConnPool->exeInsert(handle, restore, &column))
			{
				Zebra::logger->debug("%d,%s,添加到补偿道具表失败",bd->uid,bd->tid);
			}
			else
			{
				BillUser::logger("道具卡",bd->uid,bd->tid,waitgold,bd->balance,bd->result,"道具卡添加到补偿表");
			}
		}
		return true;
	}
	return false;
}
bool BillUser::redeem_object_card(const BillData* bd)
{
	if(!check_tid(bd->tid))
	{
		return false;
	}
	Cmd::Bill::t_Return_ObjCard send;
	
	strncpy(send.account , account , Cmd::UserServer::ID_MAX_LENGTH);
	send.accid = id;			        /// 账号编号

	if (bd->result == Cmd::UserServer::RET_OK)
	{
		//Zebra::logger->debug("bd->subatt=%d",bd->subat);
		send.subatt = bd->subat;
		send.balance = bd->balance;
		send.byReturn = Cmd::REDEEM_SUCCESS;
	}
	else
	{
		send.subatt = 0;
		send.byReturn = Cmd::REDEEM_FAIL;
	}
	end_tid();
	return this->sendCmd(&send, sizeof(send));

}
bool BillUser::redeem_gold_err(const BillData* bd)
{
	connHandleID handle = BillService::dbConnPool->getHandle();
	if ((connHandleID)-1 != handle)
	{
		FieldSet* restore = BillService::metaData->getFields("RESTOREGOLD");
		std::ostringstream oss;         
		Record column,where;                           
		column.clear();
		column.put("accid", bd->uid);
		column.put("tid", bd->tid);
		int rate = REDEEM_RATE_GOLD;  // 金币与点数兑换比率
		DWORD waitgold = 0;  // 待充金币数

		if (bd->result == Cmd::UserServer::RET_OK)
		{
			column.put("restoregold",bd->point / rate);
			column.put("balance",bd->balance);
			if((unsigned int)-1 == BillService::dbConnPool->exeInsert(handle, restore, &column))
			{
				Zebra::logger->debug("%d,%s,添加到补偿金币表失败",bd->uid,bd->tid);
			}
			else
			{
				BillUser::logger("金币",bd->uid,bd->tid,waitgold,bd->balance,bd->result,"点数换金币添加到补偿表");
			}
		}
		return true;
	}
	return false;
}
bool BillUser::redeem_gold(const BillData* bd)
{
	if(!check_tid(bd->tid))
	{
		return false;
	}
	Cmd::Bill::t_Redeem_Gold_Gateway send;
	
	int rate = REDEEM_RATE_GOLD;  // 金币与点数兑换比率
	DWORD waitgold = 0;  // 待充金币数
	strncpy(send.account , account , Cmd::UserServer::ID_MAX_LENGTH);
	send.accid = id;			        /// 账号编号

	//send.type = Cmd::TYPE_QUERY;

	if (bd->result == Cmd::UserServer::RET_OK)
	{
		waitgold = bd->point / rate;
		// 金币服务器操作失败，记录兑换日志
		 // 帐号, TID, 交易结果，点数余额，金币余额
		send.byReturn = Cmd::REDEEM_SUCCESS;
		BillUser::logger("金币",this->id,bd->tid,waitgold,bd->balance,bd->result,"点数换金币");

	}
	else
	{
		send.byReturn = Cmd::REDEEM_FAIL;
	}

	if(send.byReturn == Cmd::REDEEM_SUCCESS)
	{
		send.dwBalance = bd->balance;
	}
	else
	{
		send.dwBalance = 0;
	}
	send.dwGold = waitgold;

	end_tid();
	this->sendCmd(&send, sizeof(send));

	// */
	return true;

}
bool BillUser::redeem_moth_card(const BillData* bd)
{
	if(!check_tid(bd->tid))
	{
		return false;
	}
	Cmd::Bill::t_Redeem_MonthCard_Gateway send;
	DWORD old_vip_time=0;
	
	Record column,where;                           
	std::ostringstream oss;         
	strncpy(send.account , account , Cmd::UserServer::ID_MAX_LENGTH);
	send.accid = id;			        /// 账号编号

	//send.type = Cmd::TYPE_QUERY;

	if (bd->result == Cmd::UserServer::RET_OK)
	{
		RecordSet* recordset = NULL;
		FieldSet* balance = BillService::metaData->getFields("BALANCE");

			
			oss << "accid=" << id;
			where.put("accid", oss.str());
			
			if (balance)
			{
				connHandleID handle = BillService::dbConnPool->getHandle();

				if ((connHandleID)-1 != handle)
				{
					recordset = BillService::dbConnPool->exeSelect(handle, balance, NULL, &where);

					if (recordset && !recordset->empty())
					{//更新已有金币记录
						oss.str("");

						old_vip_time = recordset->get(0)->get("monthcard");
						old_vip_time +=  30 * 24 * 60 * 60;
						column.put("monthcard", old_vip_time);

						if((unsigned int)-1 == BillService::dbConnPool->exeUpdate(handle, 
									balance, &column, &where))
						{
							send.byReturn = Cmd::REDEEM_FAIL;
						}      
						else
						{
							send.byReturn = Cmd::REDEEM_SUCCESS;
						}	
					}
					else
					{
						/*
						// 没有兑换记录，插入新的记录
						
						old_vip_time = time((time_t)NULL);
						old_vip_time +=  30 * 24 * 60 * 60;
						column.clear();
						column.put("account", account);
						column.put("accid", id);
						column.put("gold", 0);
						column.put("allgold", 0);
						column.put("monthcard", old_vip_time);
						column.put("allconsum", (int)0);

						if((unsigned int)-1 == BillService::dbConnPool->exeInsert(handle, balance, &column))
						{
							send.byReturn = Cmd::REDEEM_FAIL;
						}
						else
						{
							send.byReturn = Cmd::REDEEM_SUCCESS;
						}
						// */
						send.byReturn = Cmd::REDEEM_FAIL;
					}
					SAFE_DELETE(recordset)
					BillService::dbConnPool->putHandle(handle); 
				}
				else
				{
					send.byReturn = Cmd::REDEEM_BUSY;
				}
				SAFE_DELETE(recordset);
			}
		}
		else
		{
			send.byReturn = Cmd::REDEEM_FAIL;
		}

		// 金币服务器操作失败，记录兑换日志
		 // 帐号, TID, 交易结果，点数余额，金币余额
		BillUser::logger("月卡",this->id,bd->tid,0,bd->balance,bd->result,"点数换月卡");

	this->sendCmd(&send, sizeof(send));
	end_tid();

	// */
	return true;

}
bool BillUser::begin_tid(const char *t)
{
	if(tid[0]=='\0')
	{
		strncpy(tid, t, Cmd::UserServer::SEQ_MAX_LENGTH);
		return true;
	}
	return false;
}

/**
 * \brief 检测是否需要退出
 *
 *
 * \param current 当前时间
 * \return 0表示超时退出,1表示正常游戏,2表示正常退出
 */
DWORD BillUser::loginTimeOut(zTime current) 
{
	if(CONF_LOGOUT == state)
	{
		return CONF_LOGOUT; 
	}
	else if (WAIT_LOGIN == state && timestamp.elapse(current) >= session_timeout_value)
	{
		return WAIT_LOGIN_TIMEOUT;
	}
	return CONF_LOGIN;
}
bool BillUser::usermsgParseScene(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd;
	using namespace Cmd::Bill;
	switch(ptNullCmd->cmd)
	{
		case CMD_STOCK:
			{
				switch(ptNullCmd->para)
				{
					case PARA_STOCK_SAVE:
						{
							t_Stock_Save *rev=(t_Stock_Save*)ptNullCmd;

							/*
							if(rev->dwGold)
							{
								BillUser::logger("金币",this->id,this->account,this->gold,rev->dwGold,1,"股票金币冲值");
							}
							if(rev->dwMoney)
							{
								BillUser::logger("银币",this->id,this->account,this->money,rev->dwMoney,1,"股票银币冲值");
							}
							connHandleID handle = BillService::dbConnPool->getHandle();
							if ((connHandleID)-1 != handle)
							{
								FieldSet* balance = BillService::metaData->getFields("BALANCE");
								Record column,where;                           
								std::ostringstream oss;         
								if(balance)
								{
									DWORD temp_gold=gold + rev->dwGold;
									DWORD temp_money=money + rev->dwMoney;
									oss <<"accid="<<this->id;
									column.put("gold",temp_gold);
									column.put("money",temp_money);
									if((unsigned int)-1 == BillService::dbConnPool->exeUpdate(handle, 
												balance, &column, &where))
									{
									}
									else
									{
										if(rev->dwGold)
										{
											BillUser::logger("金币",this->id,this->account,this->gold,rev->dwGold,1,"股票金币冲值");
										}
										if(rev->dwMoney)
										{
											BillUser::logger("银币",this->id,this->account,this->money,rev->dwMoney,1,"股票银币冲值");
										}
										gold = temp_gold;
										money = temp_money;
										stReturnFundStockUserCmd send;
										send.dwGold=gold;
										send.dwMoney=money;
										sendCmdToMe(&send,sizeof(send));
									}
									Zebra::logger->debug("%s(%d)股票冲值,金币%d,银币%d",this->account,this->id,rev->dwGold,rev->dwMoney);
								}
								BillService::dbConnPool->putHandle(handle);
							}
							// */
							if(rev->dwGold)
							{
								addGold(rev->dwGold,"股票金币冲值",true);
							}
							if(rev->dwMoney)
							{
								addMoney(rev->dwMoney,"股票银币冲值",true);
							}
							stReturnFundStockUserCmd send;
							send.dwGold=gold;
							send.dwMoney=money;
							sendCmdToMe(&send,sizeof(send));
							return true;
						}
						break;
					default:
						break;
				}
			}
			break;
		default:
			break;
	}
	return false;
}
bool BillUser::usermsgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd;
	switch(ptNullCmd->cmd)
	{
		case STOCK_BILL_USERCMD:
			{
				switch(ptNullCmd->para)
				{
					case PASSWORD_STOCKPARA:
						{
							stPassowrdStockUserCmd *rev=(stPassowrdStockUserCmd*)ptNullCmd;
							stReturnPasswordStockIserCmd ret; 
							if(this->password[0])
							{
								if(!strncmp(password , rev->byPawword,sizeof(rev->byPawword)))
								{
									if(rev->byNew1[0] && !strncmp(rev->byNew1 , rev->byNew2,sizeof(rev->byNew1)))
									{
										connHandleID handle = BillService::dbConnPool->getHandle();
										if ((connHandleID)-1 != handle)
										{
											FieldSet* balance = BillService::metaData->getFields("BALANCE");
											Record column,where;                           
											std::ostringstream oss;         
											if(balance)
											{
												oss <<"accid="<<this->id;
												column.put("password",rev->byNew1);
												where.put("accid", oss.str());
												if((unsigned int)-1 == BillService::dbConnPool->exeUpdate(handle, 
															balance, &column, &where))
												{
													ret.byReturn = STOCK_SERVER_WRONG;
													Zebra::logger->debug("%d,%s,修改密码服务器错误",this->id,this->account);
												}
												else
												{
													bzero(this->password,sizeof(this->password));
													strncpy(this->password,rev->byNew1,sizeof(rev->byNew1));
													ret.byReturn = STOCK_CHANGE_OK;
													Zebra::logger->debug("%d,%s,修改密码成功",this->id,this->account);
												}
											}
											else
											{
												ret.byReturn = STOCK_SERVER_WRONG;
												Zebra::logger->debug("%d,%s,修改密码得到句柄错误",this->id,this->account);
											}
											BillService::dbConnPool->putHandle(handle);
										}
									}
									else
									{
										if(rev->byNew1[0])
										{
											ret.byReturn = STOCK_DIFF;
											Zebra::logger->debug("%d,%s,修改密码两次输入不一致",this->id,this->account);
										}
										else
										{
											ret.byReturn = STOCK_LOGIN_OK;
											Zebra::logger->debug("%d,%s,登陆成功",this->id,this->account);
										}
									}
								}
								else
								{
									ret.byReturn=STOCK_ERROR;
									Zebra::logger->debug("%d,%s,登陆密码错误",this->id,this->account);
								}
							}
							else/// 新帐号
							{// 没有兑换记录，插入新的记录
								connHandleID handle = BillService::dbConnPool->getHandle();
								if ((connHandleID)-1 != handle)
								{
									FieldSet* balance = BillService::metaData->getFields("BALANCE");
									RecordSet* recordset = NULL;
									std::ostringstream oss;         
									Record column,where;                           
									if(rev->byNew1[0] && !strncmp(rev->byNew1 , rev->byNew2,sizeof(rev->byNew1)))
									{
										if(balance)
										{
											oss <<"accid="<<this->id;
											where.put("accid", oss.str());
											recordset = BillService::dbConnPool->exeSelect(handle, balance, NULL, &where);
											if (!recordset || (recordset && recordset->empty()))
											{
												column.clear();
												column.put("account", account);
												column.put("accid", id);
												column.put("gold", 0);
												column.put("money", 0);
												column.put("allgoldin", 0);
												column.put("allgoldout", 0);
												column.put("allmoneyin", 0);
												column.put("allmoneyout", 0);
												column.put("monthcard", 0);
												column.put("password", rev->byNew1);

												if((unsigned int)-1 == BillService::dbConnPool->exeInsert(handle, balance, &column))
												{
													ret.byReturn = STOCK_SERVER_WRONG;
													Zebra::logger->debug("%d,%s,启用帐号服务器数据库插入错误",this->id,this->account);
												}
												ret.byReturn = STOCK_OPEN_OK;
												strncpy(this->password,rev->byNew1,sizeof(rev->byNew1));
												Zebra::logger->debug("%d,%s,启用帐号成功",this->id,this->account);
											}
											else
											{
												ret.byReturn =STOCK_EXIST;
												Zebra::logger->debug("%d,%s,帐号已经启用",this->id,this->account);
											}
											SAFE_DELETE(recordset)
										}
										else
										{
											ret.byReturn = STOCK_SERVER_WRONG;
											Zebra::logger->debug("%d,%s,启用帐号服务器错误",this->id,this->account);
										}
									}
									else
									{
										if(rev->byNew1[0])
										{
											ret.byReturn = STOCK_DIFF;
											Zebra::logger->debug("%d,%s,启用帐号密码不一致",this->id,this->account);
										}
										else
										{
											ret.byReturn = STOCK_NONE;
											Zebra::logger->debug("%d,%s,启用帐号密码不能为空",this->id,this->account);
										}
									}
									BillService::dbConnPool->putHandle(handle);
								}
								else
								{
									ret.byReturn = STOCK_SERVER_WRONG;
									Zebra::logger->debug("%d,%s,启用帐号未得到服务器句柄",this->id,this->account);
								}
							}
							//TODO 登陆成功STOCK_LOGIN_OK,STOCK_CHANGE_OK,STOCK_OPEN_OK
							if(ret.byReturn == STOCK_LOGIN_OK || ret.byReturn ==  STOCK_CHANGE_OK  || ret.byReturn ==  STOCK_OPEN_OK)
							{
								stock_login=true; 
								//Cmd::Bill::t_Stock_Logout out;
								//this->sendCmdToScene(&out,sizeof(out));

							}
							this->sendCmdToMe(&ret,sizeof(ret));
							return true;
						}
						break;
					case QUERY_FUND_STOCKPARA:
						{
							if(!checkStockLogin())
							{
								return true;
							}
							stReturnFundStockUserCmd send;
							send.dwGold=gold;
							send.dwMoney=money;
							sendCmdToMe(&send,sizeof(send));
							Zebra::logger->debug("%s(%d)请求股票帐号余额",this->account,this->id);
							return true;
						}
						break;
					case TRANSFER_FUND_FETCH_STOCKPARA:
						{
							if(!checkStockLogin())
							{
								return true;
							}
							stTransferFundStockFetchUserCmd *rev = (stTransferFundStockFetchUserCmd*)ptNullCmd; 
							if(gold < rev->dwGold || money < rev->dwMoney)
							{
								Zebra::logger->debug("%s(%d)股票套现数值错误,可能是外挂,金币%d,银币%d",this->account,this->id,rev->dwGold,rev->dwMoney);
								return true;
							}
							/*
							   connHandleID handle = BillService::dbConnPool->getHandle();
							   if ((connHandleID)-1 != handle)
							   {
							   FieldSet* balance = BillService::metaData->getFields("BALANCE");
							   Record column,where;                           
							   std::ostringstream oss;         
							   if(balance)
							   {
							   DWORD temp_gold = gold - rev->dwGold;
							   DWORD temp_money = money - rev->dwMoney;
							   oss <<"accid="<<this->id;
							   where.put("accid", oss.str());
							   column.put("gold",temp_gold);
							   column.put("money",temp_money);
							   if((unsigned int)-1 == BillService::dbConnPool->exeUpdate(handle, 
							   balance, &column, &where))
							   {
							   }
							   else
							   {
							   if(rev->dwGold)
							   {
							   BillUser::logger("金币",this->id,this->account,this->gold,rev->dwGold,0,"股票金币套现");
							   }
							   if(rev->dwMoney)
							   {
							   BillUser::logger("银币",this->id,this->account,this->money,rev->dwMoney,0,"股票银币套现");
							   }
							   gold = temp_gold;
							   money = temp_money;

							   if(rev->dwGold)
							   {
							   removeGold(rev->dwGold,"股票金币套现",true)
							   }
							   if(rev->dwMoney)
							   {
							   removeMoney(rev->dwMoney,"股票银币套现",true)
							   }
							   Cmd::Bill::t_Stock_Fetch fetch;
							   fetch.dwGold=rev->dwGold;
							   fetch.dwMoney=rev->dwMoney;
							   this->sendCmdToScene(&fetch,sizeof(fetch));

							/// 通知客户端股票帐号数据变化
							stReturnFundStockUserCmd send;
							send.dwGold=gold;
							send.dwMoney=money;
							sendCmdToMe(&send,sizeof(send));
							//}
							}
							BillService::dbConnPool->putHandle(handle);
							}
							// */
							if(rev->dwGold)
							{
								removeGold(rev->dwGold,"股票金币套现",true);
							}
							if(rev->dwMoney)
							{
								removeMoney(rev->dwMoney,"股票银币套现",true);
							}
							Cmd::Bill::t_Stock_Fetch fetch;
							fetch.dwGold=rev->dwGold;
							fetch.dwMoney=rev->dwMoney;
							this->sendCmdToScene(&fetch,sizeof(fetch));

							/// 通知客户端股票帐号数据变化
							stReturnFundStockUserCmd send;
							send.dwGold=gold;
							send.dwMoney=money;
							sendCmdToMe(&send,sizeof(send));
							//Zebra::logger->debug("%s(%d)股票套现,金币%d,银币%d",this->account,this->id,rev->dwGold,rev->dwMoney);
							return true;
						}
						break;
					case PUT_LIST_STOCKPARA:
						{
							if(!checkStockLogin())
							{
								return true;
							}
							stPutListStockUserCmd *rev = (stPutListStockUserCmd*)ptNullCmd; 
							if(rev->byType == Cmd::STOCK_GOLD && this->goldlistNum > 10)
							{
								stReturnPasswordStockIserCmd ret;
								ret.byReturn = Cmd::STOCK_GOLDLIST_MAX;
								this->sendCmdToMe(&ret,sizeof(ret));
								return true;
							}
							if(rev->byType == Cmd::STOCK_MONEY && this->moneylistNum > 10)
							{
								stReturnPasswordStockIserCmd ret;
								ret.byReturn = Cmd::STOCK_GOLDLIST_MAX;
								this->sendCmdToMe(&ret,sizeof(ret));
								return true;
							}
							this->putList(rev->dwNum,rev->dwPrice,rev->byType);
							Zebra::logger->debug("%s(%d)委托单,类型%s,数量%d,价格:%d",this->account,this->id,rev->byType==STOCK_MONEY?"银币":"金币",rev->dwNum,rev->dwPrice);
							return true;
						}
						break;
						/// 请求自己未成交委托单
					case REQUEST_CONSIGN_LIST_STOCKPARA:
						{
							if(!checkStockLogin())
							{
								return true;
							}
							Cmd::stConsignCleanListStockUserCmd ret;
							this->sendCmdToMe(&ret,sizeof(ret));
							ConsignGoldManager::getInstance()->sendWaitDataToUser(this);
							//ConsignMoneyManager::getInstance()->sendWaitDataToUser(this);
						}
						break;
						//请求前10位标价和数量
					case REQUEST_FIRSTTEN_LIST_STOCKPARA:
						{
							ConsignGoldManager::getInstance()->sendFirstFiveToUser(this);
							ConsignMoneyManager::getInstance()->sendFirstFiveToUser(this);
							return true;
						}
						break;
						//撤销卖单
					case CONSIGN_CANCEL_GOLD_STOCKPARA:
						{
							stConsignCancelGoldStockUserCmd *rev=(stConsignCancelGoldStockUserCmd*)ptNullCmd;
							ConsignGoldManager::getInstance()->cancelList(this,rev->dwNum);
							return true;
						}
						break;
						//撤销买单
					case CONSIGN_CANCEL_MONEY_STOCKPARA:
						{
							stConsignCancelMoneyStockUserCmd *rev=(stConsignCancelMoneyStockUserCmd*)ptNullCmd;
							ConsignMoneyManager::getInstance()->cancelList(this,rev->dwNum);
							return true;
						}
						break;
						// 撤单
					case REQUEST_CANCEL_LIST_STOCKPARA:
						{
							if(!checkStockLogin())
							{
								return true;
							}
							stRequestCancelListStockUserCmd *rev=(stRequestCancelListStockUserCmd*)ptNullCmd;
							if(rev->byType == Cmd::STOCK_GOLD)
							{
								ConsignGoldManager::getInstance()->cancelList(this,rev->id);
							}
							else if(rev->byType == Cmd::STOCK_MONEY)
							{
								ConsignMoneyManager::getInstance()->cancelList(this,rev->id);
							}
							return true;
						}
						break;
					case REQUEST_HISTORY_STOCKPARA:
						{
							stRequstHistoryStockUserCmd *rev=(stRequstHistoryStockUserCmd*)ptNullCmd;
							ConsignHistoryManager::getInstance()->sendDataToUser(this,rev->begintime,rev->num);
							return true;
						}
						break;
					case REQUEST_SELF_HISTORY_STOCKPARA:
						{
							Zebra::logger->debug("%u请求自己历史数据",this->id);
							if(!checkStockLogin())
							{
								return true;
							}
							stRequstSelfHistoryStockUserCmd *rev=(stRequstSelfHistoryStockUserCmd*)ptNullCmd;
							ConsignHistoryManager::getInstance()->sendSelfDataToUser(this,rev->begintime,rev->num);
							return true;
						}
						break;
					case PARA_CANCELLISTALL_STOCKPARA:
						{
							//t_CancelListAllStock_GateScene *rev=(t_CancelListAllStock_GateScene*)ptNullCmd; 
							Consign::cancelListAll();

						}
						break;
					case REQUEST_STATE_STOCKPARA:
						{
							stRequestStateStockUserCmd ret;
							if(password[0])
								ret.active=true;
							else
								ret.active=false;
							if(stock_login)
								ret.login=true;
							else
								ret.login=false;;
							sendCmdToMe(&ret,sizeof(ret));
							return true;
						}
						break;
					default:
						break;
				}
			}
			break;
		default:
			break;
	}
	return false;
}
#define SOTCK_MIN_NUM 100 //股票最小交易单位(手)
#define SOTCK_TAX 10 //股票没笔税收
bool BillUser::putList(DWORD num , DWORD price, unsigned char type)
{
	Zebra::logger->debug("%s:提交%s单,数量%d,价格:%d",this->account,type?"money":"gold",num,price);
	if(!num || !price)
	{
		return false;
	}
	DWORD min = getRealMinTime();
	bool bret=false;
	connHandleID handle = BillService::dbConnPool->getHandle();
	if ((connHandleID)-1 != handle)
	{
		FieldSet* balance=NULL;
		if(type == Cmd::STOCK_GOLD)
		{
			balance = BillService::metaData->getFields("CONSIGNGOLD");
			//没笔交易的最小金币单位(手)
			if(num%SOTCK_MIN_NUM)
			{
				bret=false;
			}
			else if(num >=100 && removeGold(DWORD(num * 0.02f),"委托税",false,true))
			{
				if(removeGold(num,"委托卖单"))
				{
					bret =true;
				}
				else
				{
					addGold(DWORD(num * 0.02f),"返还委托税",false,true); 
				}
			}
		}
		else if(type == Cmd::STOCK_MONEY)
		{
			balance = BillService::metaData->getFields("CONSIGNMONEY");
			//没笔交易的最小金币单位(手)
			if((num/price)%SOTCK_MIN_NUM)
			{
				bret=false;
			}
			else if(num >=100 && removeMoney(DWORD(num * 0.02f),"委托税",false,true))
			{
				if(removeMoney(num,"委托买单"))
				{
					bret =true;
				}
				else
				{
					addMoney(DWORD(num * 0.02f),"返还委托税",false,true); 
				}
			}
		}
		if(bret)
		{
			Record column;                           
			std::ostringstream oss;         
			if(balance)
			{
				column.put("accid", this->id);
				column.put("num",num);
				column.put("price",price);
				column.put("time",min);
				if((unsigned int)-1 == BillService::dbConnPool->exeInsert(handle, balance, &column))
				{
				}
				else
				{
					if(type == Cmd::STOCK_GOLD)
					{
						//this->increaseGoldListNum();
						BillUser::logger("股票金币",this->id,this->account,num,price,0,"委托卖");
						ConsignGoldManager::getInstance()->trade();
					}
					else if(type == Cmd::STOCK_MONEY)
					{
						//this->increaseMoneyListNum();
						BillUser::logger("股票银币",this->id,this->account,num,price,0,"委托买");
						ConsignMoneyManager::getInstance()->trade();
					}
					else
					{
						Zebra::logger->debug("%s(%d)未识别的股票类型%d",this->account,this->id,type);
					}
				}
			}
		}
		else
		{
			Zebra::logger->debug("%s(%d)扣除费用失败或者交易单位不正确",this->account,this->id);
		}
		BillService::dbConnPool->putHandle(handle);
	}
	if(bret)
	{
		ConsignHistoryManager::getInstance()->update();
	}
	return bret;
}
bool BillUser::removeGold(DWORD num,const char *disc,bool transfer , bool tax)
{
	bool bret = false;
	if(num > this->gold)
	{
		Zebra::logger->debug("%s(%d)扣除金币失败,需要%d,现有%d,描述:%s",this->account,this->id,num,this->gold,disc);
		return bret;
	}
	connHandleID handle = BillService::dbConnPool->getHandle();
	if ((connHandleID)-1 != handle)
	{
		FieldSet* balance = BillService::metaData->getFields("BALANCE");
		Record column,where;                           
		std::ostringstream oss;         
		if(balance)
		{
			DWORD temp_gold = gold - num;
			DWORD temp_all = 0;
			DWORD temp_tax = all_tax_gold;
			if(transfer)
			{
				temp_all = all_out_gold + num;
			}
			else
			{
				temp_all = all_out_gold;
				if(tax)
				{
					temp_tax = all_tax_gold + num;
				}
			}
			oss <<"accid="<<this->id;
			where.put("accid", oss.str());
			column.put("gold",temp_gold);
			column.put("goldtax",temp_tax);
			column.put("allgoldout",temp_all);
			if((unsigned int)-1 == BillService::dbConnPool->exeUpdate(handle, 
						balance, &column, &where))
			{
			}
			else
			{
				gold = temp_gold;
				all_out_gold=temp_all;
				all_tax_gold = temp_tax;
				BillUser::logger("金币",this->id,this->account,this->gold,num,0,disc);
				bret=true;
				/// 通知客户端股票帐号数据变化
				Cmd::stReturnFundStockUserCmd send;
				send.dwGold=gold;
				send.dwMoney=money;
				sendCmdToMe(&send,sizeof(send));
			}
		}
		BillService::dbConnPool->putHandle(handle);
	}
	return bret;
}
bool BillUser::removeMoney(DWORD num,const char *disc,bool transfer , bool tax)
{
	bool bret = false;
	if(num > this->money)
	{
		Zebra::logger->debug("%s(%d)扣除银币失败,需要%d,现有%d,描述:%s",this->account,this->id,num,this->money,disc);
		return bret;
	}
	connHandleID handle = BillService::dbConnPool->getHandle();
	if ((connHandleID)-1 != handle)
	{
		FieldSet* balance = BillService::metaData->getFields("BALANCE");
		Record column,where;                           
		std::ostringstream oss;         
		if(balance)
		{
			DWORD temp_money = money - num;
			DWORD temp_all = 0;
			DWORD temp_tax = all_tax_money;
			if(transfer)
			{
				temp_all = all_out_money + num;
			}
			else
			{
				temp_all = all_out_money;
				if(tax)
				{
					temp_tax = all_tax_money + num;
				}
			}
			oss <<"accid="<<this->id;
			where.put("accid", oss.str());
			column.put("money",temp_money);
			column.put("moneytax",temp_tax);
			column.put("allmoneyout",temp_all);
			if((unsigned int)-1 == BillService::dbConnPool->exeUpdate(handle, 
						balance, &column, &where))
			{
			}
			else
			{
				money = temp_money;
				all_out_money = temp_all;
				all_tax_money = temp_tax;
				BillUser::logger("银币",this->id,this->account,this->money,num,0,disc);
				bret=true;
				/// 通知客户端股票帐号数据变化
				Cmd::stReturnFundStockUserCmd send;
				send.dwGold=gold;
				send.dwMoney=money;
				sendCmdToMe(&send,sizeof(send));
			}
		}
		BillService::dbConnPool->putHandle(handle);
	}
	return bret;
}
bool BillUser::addGold(DWORD num,const char *disc,bool transfer , bool tax)
{
	bool bret = false;
	connHandleID handle = BillService::dbConnPool->getHandle();
	if ((connHandleID)-1 != handle)
	{
		FieldSet* balance = BillService::metaData->getFields("BALANCE");
		Record column,where;                           
		std::ostringstream oss;         
		if(balance)
		{
			DWORD temp_gold = gold + num;
			DWORD temp_all = 0;
			DWORD temp_tax = all_tax_gold;
			if(transfer)
			{
				temp_all = all_in_gold + num;
			}
			else
			{
				temp_all = all_in_gold;
				if(tax)
				{
					temp_tax = all_tax_gold - num;
				}
			}
			oss <<"accid="<<this->id;
			where.put("accid", oss.str());
			column.put("gold",temp_gold);
			column.put("goldtax",temp_tax);
			column.put("allgoldin",temp_all);
			if((unsigned int)-1 == BillService::dbConnPool->exeUpdate(handle, 
						balance, &column, &where))
			{
			}
			else
			{
				gold = temp_gold;
				all_in_gold = temp_all;
				all_tax_gold = temp_tax;
				BillUser::logger("金币",this->id,this->account,this->gold,num,1,disc);
				bret=true;
				/// 通知客户端股票帐号数据变化
				Cmd::stReturnFundStockUserCmd send;
				send.dwGold=gold;
				send.dwMoney=money;
				sendCmdToMe(&send,sizeof(send));
			}
		}
		BillService::dbConnPool->putHandle(handle);
	}
	return bret;
}
bool BillUser::addMoney(DWORD num,const char *disc,bool transfer , bool tax)
{
	bool bret = false;
	connHandleID handle = BillService::dbConnPool->getHandle();
	if ((connHandleID)-1 != handle)
	{
		FieldSet* balance = BillService::metaData->getFields("BALANCE");
		Record column,where;                           
		std::ostringstream oss;         
		if(balance)
		{
			DWORD temp_money = money + num;
			DWORD temp_all = 0;
			DWORD temp_tax = all_tax_money;
			if(transfer)
			{
				temp_all = all_in_money + num;
			}
			else
			{
				temp_all = all_in_money;
				if(tax)
				{
					temp_tax = all_tax_money - num;
				}
			}
			oss <<"accid="<<this->id;
			where.put("accid", oss.str());
			column.put("money",temp_money);
			column.put("moneytax",temp_tax);
			column.put("allmoneyin",temp_all);
			if((unsigned int)-1 == BillService::dbConnPool->exeUpdate(handle, 
						balance, &column, &where))
			{
			}
			else
			{
				money = temp_money;
				all_in_money=temp_all;
				all_tax_money = temp_tax;
				BillUser::logger("银币",this->id,this->account,this->money,num,1,disc);
				bret=true;
				/// 通知客户端股票帐号数据变化
				Cmd::stReturnFundStockUserCmd send;
				send.dwGold=gold;
				send.dwMoney=money;
				sendCmdToMe(&send,sizeof(send));
			}
		}
		BillService::dbConnPool->putHandle(handle);
	}
	return bret;
}
/**
 * \brief 得到当前真实时间,返回精度是分钟
 */
DWORD BillUser::getRealMinTime()
{
	time_t timValue = time(NULL);
	timValue +=8*60*60;
	return timValue/60;
}
/**
 * \brief 金币相关log
 * \param coin_type 	货币类型
 * \param acc 			用户accid
 * \param act 			帐号名称
 * \param cur 			当前数量
 * \param change		本次操作变化量
 * \param type	 		错作类型(1表示增,0表示减)
 * \param action	 	描述
 */
bool BillUser::logger(const char *coin_type,DWORD acc,const char *act,DWORD cur,DWORD change,DWORD type,const char *action)
{
	return BillService::tradelog->trace("%s,%d,%s,%d,%d,%d,%s",coin_type,acc,act,cur,change,type,action);
}
const char *BillUser::getIp()
{
	//Zebra::logger->debug("ClientIp:%s",client_ip);
	return client_ip;
}
