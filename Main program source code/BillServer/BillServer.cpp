/**
 * \file
 * \version  $Id: BillServer.cpp  $
 * \author  
 * \date 
 * \brief zebra项目计费服务器
 *
 */

#include "zSubNetService.h"
#include "Zebra.h"
#include "zMisc.h"
#include "BillServer.h"
#include "BillTask.h"
#include "zDBConnPool.h"
#include "zConfile.h"
#include "BillTaskManager.h"
#include "BillManager.h"
#include "BillSessionManager.h"
#include "TimeTick.h"
#include "zMetaData.h"
#include "BillCallback.h"
#include "BillUser.h"
#include "BillUserManager.h"
#include "StockConsign.h"

zDBConnPool *BillService::dbConnPool = NULL;

BillService *BillService::instance = NULL;

MetaData* BillService::metaData = NULL;

zLogger* BillService::tradelog = NULL;

bool action(const BillData *bd);

/**
 * \brief 初始化网络服务器程序
 *
 * 实现了虚函数<code>zService::init</code>
 *
 * \return 是否成功
 */
bool BillService::init()
{
	//Zebra::logger->debug(__PRETTY_FUNCTION__);

	dbConnPool = zDBConnPool::newInstance(NULL);
	if (NULL == dbConnPool
			|| !dbConnPool->putURL(0, Zebra::global["mysql"].c_str(), false))
	{
		Zebra::logger->error("连接数据库失败");
		return false;
	}

	metaData = MetaData::newInstance("");

	if (NULL == metaData
			|| !metaData->init(Zebra::global["mysql"]))
	{
		Zebra::logger->error("连接数据库失败");
		return false;
	}

	tradelog = new zLogger("tradelog");

	//设置日志级别
	tradelog->setLevel(Zebra::global["log"]);
	//设置写本地日志文件
	if ("" != Zebra::global["gold_tradelog"])
	{
		tradelog->addLocalFileLog(Zebra::global["gold_tradelog"]);
		//tradelog->removeConsoleLog();
	}

	//初始化连接线程池
	int state = state_none;
	Zebra::to_lower(Zebra::global["initThreadPoolState"]);
	if ("repair" == Zebra::global["initThreadPoolState"]
			|| "maintain" == Zebra::global["initThreadPoolState"])
		state = state_maintain;
	taskPool = new zTCPTaskPool(atoi(Zebra::global["threadPoolCapacity"].c_str()), state);
	if (NULL == taskPool
			|| !taskPool->init())
		return false;

	//soke 原版 start
	strncpy(pstrIP, zSocket::getIPByIfName(Zebra::global["ifname"].c_str()), MAX_IP_LENGTH - 1);
    //soke 原版 end
	
	//soke 绑定IP （如果绑定IP,注释上面的1行原版）
	//strcpy(pstrIP, "192.168.62.128");
	//soke 绑定IP end
	
	
	//Zebra::logger->debug(__PRETTY_FUNCTION__);
	BillCallback bc;
	bc.action = action;

	/*
	GameZone_t gameZone;
	gameZone.game = 0;
	gameZone.zone = atoi(Zebra::global["zone"].c_str());
	// */

	if (!::Bill_init(Zebra::global["billServerList"].c_str() , Zebra::global["tradelogfilename"].c_str() , &bc) )
	//	|| !::Bill_addserver(Zebra::global["BillServerIP"].c_str(),	atoi(Zebra::global["BillServerPort"].c_str())))
	{
		Zebra::logger->error("连接BILL服务器失败");
		return false;
	}
	ConsignGoldManager::getInstance()->init();
	ConsignMoneyManager::getInstance()->init();
	ConsignHistoryManager::getInstance()->init();
	BillTimeTick::getInstance().start();

	if (!zSubNetService::init())
	{
		return false;
	}

	
	return true;
}

/**
 * \brief 新建立一个连接任务
 *
 * 实现纯虚函数<code>zNetService::newTCPTask</code>
 *
 * \param sock TCP/IP连接
 * \param addr 地址
 */
void BillService::newTCPTask(const int sock, const struct sockaddr_in *addr)
{
	//Zebra::logger->debug(__PRETTY_FUNCTION__);
	BillTask *tcpTask = new BillTask(taskPool, sock, addr);
	if (NULL == tcpTask)
		//内存不足，直接关闭连接
		TEMP_FAILURE_RETRY(::close(sock));
	else if(!taskPool->addVerify(tcpTask))
	{
		//得到了一个正确连接，添加到验证队列中
		SAFE_DELETE(tcpTask);
	}
}

/**
 * \brief 解析来自服务器管理器的指令
 *
 * 这些指令是网关和服务器管理器交互的指令<br>
 * 实现了虚函数<code>zSubNetService::msgParse_SuperService</code>
 *
 * \param ptNullCmd 待解析的指令
 * \param nCmdLen 待解析的指令长度
 * \return 解析是否成功
 */
bool BillService::msgParse_SuperService(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Super;

	switch(ptNullCmd->para)
	{
		case PARA_BILL_NEWSESSION:
			{
				t_NewSession_Bill *ptCmd = (t_NewSession_Bill *)ptNullCmd;
				Cmd::Bill::t_NewSession_Gateway tCmd;
				BillTask *task=BillTaskManager::getInstance().getTaskByID(ptCmd->session.wdGatewayID);
				if(!task)
				{
					Zebra::logger->error("账号 %u登陆时网关已经关闭", ptCmd->session.accid);
					t_idinuse_Bill ret;
					ret.accid = ptCmd->session.accid;
					ret.loginTempID = ptCmd->session.loginTempID;
					ret.wdLoginID = ptCmd->session.wdLoginID;
					bcopy(ptCmd->session.name, ret.name, sizeof(ret.name));
					return sendCmdToSuperServer(&ret, sizeof(ret));
				}
				BillUser *pUser=new BillUser(ptCmd->session.accid,ptCmd->session.loginTempID,ptCmd->session.account,ptCmd->session.client_ip,task);
				if (!pUser || !BillUserManager::getInstance()->addUser(pUser))
				{
					//重复登陆验证
					Zebra::logger->error("账号已经登陆 %s(%u,%u)",ptCmd->session.account, ptCmd->session.accid,ptCmd->session.loginTempID);
					t_idinuse_Bill ret;
					ret.accid = ptCmd->session.accid;
					ret.loginTempID = ptCmd->session.loginTempID;
					ret.wdLoginID = ptCmd->session.wdLoginID;
					bcopy(ptCmd->session.name, ret.name, sizeof(ret.name));
					return sendCmdToSuperServer(&ret, sizeof(ret));
				}
				Zebra::logger->debug("帐号登陆%s(%d,%u)",pUser->account,pUser->id,pUser->tempid);
				//Zebra::logger->debug("%s: %u, %u, %s, %u", __FUNCTION__, ptCmd->session.accid, ptCmd->session.loginTempID, ptCmd->session.pstrIP, ptCmd->session.wdPort);
				tCmd.session = ptCmd->session;
				//Zebra::logger->debug("BillService: %u %u %u %u %u %u %u %u", ptCmd->session.des_key[0], ptCmd->session.des_key[1], ptCmd->session.des_key[2], ptCmd->session.des_key[3], ptCmd->session.des_key[4], ptCmd->session.des_key[5], ptCmd->session.des_key[6], ptCmd->session.des_key[7]);
				//bcopy(&ptCmd->session, &tCmd.session, sizeof(tCmd.session));

				return BillTaskManager::getInstance().broadcastByID(ptCmd->session.wdGatewayID, &tCmd, sizeof(tCmd));
			}
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

/**
 * \brief 结束网络服务器
 *
 * 实现了纯虚函数<code>zService::final</code>
 *
 */
void BillService::final()
{
	BillTimeTick::getInstance().final();
	BillTimeTick::getInstance().join();
	BillTimeTick::delInstance();

	if(taskPool)
	{
		SAFE_DELETE(taskPool);
	}
	BillManager::delInstance();

	BillTaskManager::delInstance();

	zSubNetService::final();

	::Bill_final();
	zDBConnPool::delInstance(&dbConnPool);

	Zebra::logger->debug(__PRETTY_FUNCTION__);
}

/**
 * \brief 命令行参数
 *
 */
static struct argp_option account_options[] =
{
	{"daemon",		'd',	0,			0,	"Run service as daemon",						0},
	{"log",			'l',	"level",	0,	"Log level",									0},
	{"logfilename",	'f',	"filename",	0,	"Log file name",								0},
	{"mysql",		'y',	"mysql",	0,	"MySQL[mysql://user:passwd@host:port/dbName]",	0},
	{"ifname",		'i',	"ifname",	0,	"Local network device",							0},
	{"server",		's',	"ip",		0,	"Super server ip address",						0},
	{"port",		'p',	"port",		0,	"Super server port number",						0},
	{0,				0,		0,			0,	0,												0}
};

/**
 * \brief 命令行参数解析器
 *
 * \param key 参数缩写
 * \param arg 参数值
 * \param state 参数状态
 * \return 返回错误代码
 */
static error_t account_parse_opt(int key, char *arg, struct argp_state *state)
{
	switch (key)
	{
		case 'd':
			{
				Zebra::global["daemon"] = "true";
			}
			break;
		case 'p':
			{
				Zebra::global["port"]=arg;
			}
			break;
		case 's':
			{
				Zebra::global["server"]=arg;
			}
			break;
		case 'l':
			{
				Zebra::global["log"]=arg;
			}
			break;
		case 'f':
			{
				Zebra::global["logfilename"]=arg;
			}
			break;
		case 'y':
			{
				Zebra::global["mysql"]=arg;
			}
			break;
		case 'i':
			{
				Zebra::global["ifname"]=arg;
			}
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

/**
 * \brief 简短描述信息
 *
 */
static char account_doc[] = "\nBillServer\n" "\t计费服务器。";

/**
 * \brief 程序的版本信息
 *
 */
const char *argp_program_version = "Program version :\t" VERSION_STRING\
									"\nBuild version   :\t" _S(BUILD_STRING)\
									"\nBuild time      :\t" __DATE__ ", " __TIME__;

/**
 * \brief 读取配置文件
 *
 */
class BillConfile:public zConfile
{
	bool parseYour(const xmlNodePtr node)
	{
		if (node)
		{
			xmlNodePtr child=parser.getChildNode(node,NULL);
			while(child)
			{
				parseNormal(child);
				child=parser.getNextNode(child,NULL);
			}
			return true;
		}
		else
			return false;
	}
};

/**
 * \brief 重新读取配置文件，为HUP信号的处理函数
 *
 */
void BillService::reloadConfig()
{
	Zebra::logger->debug("%s", __PRETTY_FUNCTION__);
	BillConfile rc;
	rc.parse("BillServer");
	//指令检测开关
	if(Zebra::global["cmdswitch"] == "true")
	{
		zTCPTask::analysis._switch = true;
		zTCPClient::analysis._switch=true;
	}
	else
	{
		zTCPTask::analysis._switch = false;
		zTCPClient::analysis._switch=false;
	}
}

/**
 * \brief 主程序入口
 *
 * \param argc 参数个数
 * \param argv 参数列表
 * \return 运行结果
 */
int main(int argc, char **argv)
{
	Zebra::logger=new zLogger("BillServer");

	//设置缺省参数
	Zebra::global["mysql"] = "mysql://Zebra:Zebra@192.168.1.162:3306/BillServer";
	Zebra::global["logfilename"] = "/tmp/billserver.log";
	Zebra::global["gold_tradelog"] = "/tmp/gold_tradelog.log";
	Zebra::global["tradelogfilename"] = "/home/tmp/billtrade.log";
	Zebra::global["billServerList"] = "BillServer/billServerList.xml";

	//解析配置文件参数
	BillConfile rc;
	if (!rc.parse("BillServer"))
		return EXIT_FAILURE;

	//指令检测开关
	if(Zebra::global["cmdswitch"] == "true")
	{
		zTCPTask::analysis._switch = true;
		zTCPClient::analysis._switch=true;
	}
	else
	{
		zTCPTask::analysis._switch = false;
		zTCPClient::analysis._switch=false;
	}
	//解析命令行参数
	zArg::getArg()->add(account_options, account_parse_opt, 0, account_doc);
	zArg::getArg()->parse(argc, argv);
	//Zebra::global.dump(std::cout);

	//设置日志级别
	Zebra::logger->setLevel(Zebra::global["log"]);
	//设置写本地日志文件
	if ("" != Zebra::global["logfilename"])
		Zebra::logger->addLocalFileLog(Zebra::global["logfilename"]);

	//是否以后台进程的方式运行
	if ("true" == Zebra::global["daemon"]) {
		Zebra::logger->info("Program will be run as a daemon");
		Zebra::logger->removeConsoleLog();
		daemon(1, 1);
	}

	BillService::getInstance().main();
	BillService::delInstance();

	return EXIT_SUCCESS;
}

bool action(const BillData *bd)
{
	//Zebra::logger->debug(__PRETTY_FUNCTION__);
	using namespace Cmd::UserServer;
	
	switch(bd->at)
	{
		case AT_CONSUME:
			{
				if (bd)
				{
					Zebra::logger->debug("consume(uid = %u, tid = %s) ... %s", 
							bd->uid, bd->tid, (bd->result == Cmd::UserServer::RET_OK)? "success" : "failure");
					BillUser *pUser = BillUserManager::getInstance()->getUserByID(bd->uid);
					if(pUser)
					{
						pUser->redeem_gold(bd);
					}
					else
					{
						BillUser::redeem_gold_err(bd);
					}
				}
				return true;
			}
			break;
		case AT_MCARD:
			{
				if (bd)
				{
					Zebra::logger->debug("consume(uid = %u, tid = %s) ... %s", 
							bd->uid, bd->tid, (bd->result == Cmd::UserServer::RET_OK)? "success" : "failure");
					BillUser *pUser = BillUserManager::getInstance()->getUserByID(bd->uid);
					if(pUser)
					{
						pUser->redeem_moth_card(bd);
					}
				}
				return true;
			}
			break;
		case AT_PCARD:
		case AT_SCARD:
			{
				if (bd)
				{
					BillUser *pUser = BillUserManager::getInstance()->getUserByID(bd->uid);
					if(pUser)
					{
						pUser->redeem_object_card(bd);
					}
					else
					{
						BillUser::redeem_object_card_err(bd);
					}
				}
				return true;
			}
		case AT_FILLIN:
			{
				Zebra::logger->debug("fillin(uid = %u, tid = %s) ... %s", 
					bd->uid, bd->tid, bd->result==Cmd::UserServer::RET_OK ? "success" : "failure"); 
				return true;
			}
			break;
		case AT_QBALANCE:
			{
				if(bd)
				{
					Zebra::logger->debug("qbanlance(uid = %u, tid = %s) ... %s", bd->uid, bd->tid, !bd->result ? "success" : "failure");
					BillUser *pUser = BillUserManager::getInstance()->getUserByID(bd->uid);
					if(pUser)
					{
						pUser->query_point(bd);
					}
				}
				return true;
			}
			break;
		default:
			return false;
	}
	
	return false;
}
/*
bool redeem_moth_card(const BillData* bd)
{
	Cmd::Bill::t_Redeem_MonthCard_Gateway send;
	DWORD old_vip_time=0;
	
	Record column,where;                           
	std::ostringstream oss;         
	BillSession bs = BillSessionManager::getInstance().get(bd->tid);
	if(!bs.accid)
	{
		Zebra::logger->debug("%s兑换金币返回时没有正确的BillSession,可能该玩家已经退出",bd->tid);
		return false;
	}
	strncpy(send.account , bs.account , Cmd::UserServer::ID_MAX_LENGTH);
	send.accid = bs.accid;			        /// 账号编号
	send.charid = bs.charid;				/// 角色ID

	//send.type = Cmd::TYPE_QUERY;

	if (bd->result == Cmd::UserServer::RET_OK)
	{
		RecordSet* recordset = NULL;
		FieldSet* balance = BillService::metaData->getFields("BALANCE");

			
		if (bs.accid != 0)
		{
			oss << "accid=" << bs.accid;
			where.put("accid", oss.str());
			oss.str("");

			oss << "charid=" << bs.charid;
			where.put("charid", oss.str());
			
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
							BillManager::getInstance().updateVipTime(bs.accid , old_vip_time);
							send.byReturn = Cmd::REDEEM_SUCCESS;
						}	
					}
					else
					{// 没有兑换记录，插入新的记录
						
						old_vip_time = time((time_t)NULL);
						old_vip_time +=  30 * 24 * 60 * 60;
						column.clear();
						column.put("account", bs.account);
						column.put("accid", bs.accid);
						column.put("charid", bs.charid);
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
							BillManager::getInstance().updateVipTime(bs.accid , old_vip_time);
							send.byReturn = Cmd::REDEEM_SUCCESS;
						}
					}
				}
				else
				{
					send.byReturn = Cmd::REDEEM_BUSY;
				}
			}
		}
		else
		{
			send.byReturn = Cmd::REDEEM_FAIL;
		}

		// 金币服务器操作失败，记录兑换日志
		 // 帐号, TID, 交易结果，点数余额，金币余额
		BillService::tradelog->trace("点数换月卡:----------------------------------------");
		BillService::tradelog->trace("点数换月卡:%u,%s,%d,%d,%u",
				bd->uid,
				bd->tid,
				bd->result,
				bd->balance,
				old_vip_time	
				);	

	}
	else
	{
		send.byReturn = Cmd::REDEEM_FAIL;
	}


	if (bs.task)
	{
		bs.task->sendCmd(&send, sizeof(send));
	}

	return true;

}
bool query_point(const BillData* bd)
{
	Cmd::Bill::t_Return_Point send;
	
	Record column,where;                           
	std::ostringstream oss;         
	BillSession bs = BillSessionManager::getInstance().get(bd->tid);
	if(!bs.accid)
	{
		Zebra::logger->debug("%s兑换金币返回时没有正确的BillSession,可能该玩家已经退出",bd->tid);
		return false;
	}
	strncpy(send.account , bs.account , Cmd::UserServer::ID_MAX_LENGTH);
	send.accid = bs.accid;			        /// 账号编号
	send.charid = bs.charid;				/// 角色ID

	//send.type = Cmd::TYPE_QUERY;

	if (bd->result == Cmd::UserServer::RET_OK)
	{
		send.dwPoint = bd->balance;
		send.byReturn = Cmd::REDEEM_SUCCESS;
	}
	else
	{
		send.byReturn = Cmd::REDEEM_FAIL;
	}
	if (bs.task)
	{
		bs.task->sendCmd(&send, sizeof(send));
	}
	return true;
}
bool redeem_gold(const BillData* bd)
{
	Cmd::Bill::t_Redeem_Gold_Gateway send;
	
	Record column,where;                           
	std::ostringstream oss;         
	int rate = REDEEM_RATE_GOLD;  // 金币与点数兑换比率
	double gold = 0.0;  // 待充金币数
	double last_gold = 0.0; // 上次金币余额
	BillSession bs = BillSessionManager::getInstance().get(bd->tid);
	if(!bs.accid)
	{
		Zebra::logger->debug("%s兑换金币返回时没有正确的BillSession,可能该玩家已经退出",bd->tid);
		return false;
	}
	strncpy(send.account , bs.account , Cmd::UserServer::ID_MAX_LENGTH);
	send.accid = bs.accid;			        /// 账号编号
	send.charid = bs.charid;				/// 角色ID

	//send.type = Cmd::TYPE_QUERY;

	if (bd->result == Cmd::UserServer::RET_OK)
	{
		RecordSet* recordset = NULL;
		FieldSet* balance = BillService::metaData->getFields("BALANCE");

			
		if (bs.accid != 0)
		{
			oss << "accid=" << bs.accid;
			where.put("accid", oss.str());
			oss.str("");

			oss << "charid=" << bs.charid;
			where.put("charid", oss.str());
			
			gold = bs.point /(double) rate;

			if (balance)
			{
				connHandleID handle = BillService::dbConnPool->getHandle();

				if ((connHandleID)-1 != handle)
				{
					recordset = BillService::dbConnPool->exeSelect(handle, balance, NULL, &where);

					if (recordset && !recordset->empty())
					{//更新已有金币记录
						oss.str("");

						last_gold = recordset->get(0)->get("gold");
						
						oss << "gold+" << gold;
						column.put("gold", oss.str());

						oss.str("");
						oss << "allgold+" << gold;
						column.put("allgold", oss.str());

						if((unsigned int)-1 == BillService::dbConnPool->exeUpdate(handle, 
									balance, &column, &where))
						{
							send.byReturn = Cmd::REDEEM_FAIL;
						}      
						else
						{
							last_gold = last_gold + gold;
							BillManager::getInstance().updateGold(bs.accid , last_gold);
							send.byReturn = Cmd::REDEEM_SUCCESS;
						}	
					}
					else
					{// 没有兑换记录，插入新的记录
						last_gold = 0;
						
						column.clear();
						column.put("account", bs.account);
						column.put("accid", bs.accid);
						column.put("charid", bs.charid);
						column.put("gold", gold);
						column.put("allgold", gold);
						column.put("monthcard", 0);
						column.put("allconsum", (int)0);

						if((unsigned int)-1 == BillService::dbConnPool->exeInsert(handle, balance, &column))
						{
							send.byReturn = Cmd::REDEEM_FAIL;
						}
						else
						{
							last_gold = gold;
							BillManager::getInstance().updateGold(bs.accid , last_gold);
							send.byReturn = Cmd::REDEEM_SUCCESS;
						}
					}
				}
				else
				{
					send.byReturn = Cmd::REDEEM_BUSY;
				}
			}
		}
		else
		{
			send.byReturn = Cmd::REDEEM_FAIL;
		}

		// 金币服务器操作失败，记录兑换日志
		 // 帐号, TID, 交易结果，点数余额，金币余额
		//BillService::tradelog->trace("点数换金币:----------------------------------------");
		BillService::tradelog->trace("点数换金币:%u,%s,%d,%d,%f",
				bd->uid,
				bd->tid,
				bd->result,
				bd->balance,
				last_gold
				);	

	}
	else
	{
		send.byReturn = Cmd::REDEEM_FAIL;
	}

	send.dwGold = (DWORD)last_gold;

	if (bs.task)
	{
		bs.task->sendCmd(&send, sizeof(send));
	}

	return true;

}


	// */
