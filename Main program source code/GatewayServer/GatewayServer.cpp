/**
 * \file
 * \version  $Id: GatewayServer.cpp $
 * \author  
 * \date
 * \brief zebra项目Gateway服务器,负责用户指令检查转发、加密解密等
 */

#include "zSubNetService.h"
#include "Zebra.h"
#include "zMisc.h"
#include "GatewayServer.h"
#include "GatewayTask.h"
#include "BillClient.h"
#include "SceneClient.h"
#include "RecordClient.h"
#include "GateUserManager.h"
#include "zConfile.h"
#include "LoginSessionManager.h"
#include "GatewayTaskManager.h"
#include "TimeTick.h"
#include "RoleregCommand.h"
#include "RecordClient.h"
#include "MiniClient.h"
#include "zXMLParser.h"
#include "SceneClientManager.h"

GatewayService *GatewayService::instance = NULL;
zTCPTaskPool * GatewayService::taskPool = NULL;
bool GatewayService::service_gold=true;
bool GatewayService::service_stock=true;
unsigned int merge_version = 0;

/**
 * \brief 初始化网络服务器程序
 *
 * 实现了虚函数<code>zService::init</code>
 *
 * \return 是否成功
 */
bool GatewayService::init()
{
	Zebra::logger->trace("GatewayService::init");
	verify_client_version = atoi(VERSION_STRING);
	Zebra::logger->info("服务器版本号:%d",verify_client_version);
	//如果服务器不要求校对版本就不对版本
	/*
	if(verify_client_version==0)
		verify_client_version=Cmd::GAME_VERSION;
	// */	
	//Zebra::logger->debug(__PRETTY_FUNCTION__);
	
	//加载国家名称(地图)信息
	if(!country_info.init())
	{
		Zebra::logger->error("加载地图名称失败!");
	}

	//初始化连接线程池
	int state = state_none;
	Zebra::to_lower(Zebra::global["initThreadPoolState"]);
	if ("repair" == Zebra::global["initThreadPoolState"]
			|| "maintain" == Zebra::global["initThreadPoolState"])
		state = state_maintain;

	taskPool = new zTCPTaskPool(4096/*atoi(Zebra::global["threadPoolCapacity"].c_str())*/, state, 65000);
	if (NULL == taskPool
			|| !taskPool->init())
		return false;

	//soke 原版
	strncpy(pstrIP, zSocket::getIPByIfName(Zebra::global["ifname"].c_str()), MAX_IP_LENGTH - 1);
    // end

	//soke 绑定IP （如果绑定IP,注释上面的1行原版）
	
	//strcpy(pstrIP, "192.168.62.128");
	

	Zebra::logger->debug("GatewayService::init() %s", pstrIP);

	if (!zSubNetService::init())
	{
		return false;
	}

	const Cmd::Super::ServerEntry *serverEntry = NULL;
	
	//连接Session服务器
	serverEntry = getServerEntryByType(SESSIONSERVER);
	if (NULL == serverEntry)
	{
		Zebra::logger->error("不能找到Session服务器相关信息，不能连接Session服务器");
		return false;
	}
	sessionClient = new SessionClient("Session服务器", serverEntry->pstrExtIP, serverEntry->wdExtPort);
	if (NULL == sessionClient)
	{
		Zebra::logger->error("没有足够内存，不能建立Session服务器客户端实例");
		return false;
	}
	if (!sessionClient->connectToSessionServer())
	{
		Zebra::logger->error("连接Session服务器失败 %s", __PRETTY_FUNCTION__);
		//return false;
	}
	sessionClient->start();

	//连接计费服务器
	serverEntry = getServerEntryByType(BILLSERVER);
	if (NULL == serverEntry)
	{
		Zebra::logger->error("不能找到计费服务器相关信息，不能连接计费服务器");
		return false;
	}
	accountClient = new BillClient("计费服务器", serverEntry->pstrExtIP, serverEntry->wdExtPort,serverEntry->wdServerID);
	if (NULL == accountClient)
	{
		Zebra::logger->error("没有足够内存，不能建立计费服务器客户端实例");
		return false;
	}
	if (!accountClient->connectToBillServer())
	{
		Zebra::logger->error("连接计费服务器失败 %s", __PRETTY_FUNCTION__);
		return false;
	}
	accountClient->start();

	//连接所有的场景服务器
	serverEntry = getServerEntryByType(SCENESSERVER);
	if(serverEntry)
	{
		if (!SceneClientManager::getInstance().init())
			return false;
	}	

	//连接所有的档案服务器
	serverEntry = getServerEntryByType(RECORDSERVER);
	if (NULL == serverEntry)
	{
		Zebra::logger->error("不能找到Record服务器相关信息，不能连接Record服务器");
		return false;
	}
	recordClient = new RecordClient("Record服务器", serverEntry->pstrExtIP, serverEntry->wdExtPort);
	if (NULL == recordClient)
	{
		Zebra::logger->error("没有足够内存，不能建立Record服务器客户端实例");
		return false;
	}
	if (!recordClient->connectToRecordServer())
	{
		Zebra::logger->error("连接Record服务器失败 %s", __PRETTY_FUNCTION__);
		return false;
	}
	recordClient->start();

	//连接小游戏服务器
	serverEntry = getServerEntryByType(MINISERVER);
	if (NULL == serverEntry)
	{
		Zebra::logger->error("不能找到小游戏服务器相关信息，不能连接小游戏服务器");
		return false;
	}
	miniClient = new MiniClient("小游戏服务器", serverEntry->pstrExtIP, serverEntry->wdExtPort,serverEntry->wdServerID);
	if (NULL == miniClient)
	{
		Zebra::logger->error("没有足够内存，不能建立小游戏服务器客户端实例");
		return false;
	}
	if (!miniClient->connectToMiniServer())
	{
		Zebra::logger->error("连接小游戏服务器失败 %s", __PRETTY_FUNCTION__);
		return false;
	}
	miniClient->start();

	if(!GateUserManager::getInstance()->init())
		return false;

	GatewayTimeTick::getInstance().start();

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
void GatewayService::newTCPTask(const int sock, const struct sockaddr_in *addr)
{
	Zebra::logger->trace("GatewayService::newTCPTask");
	GatewayTask *tcpTask = new GatewayTask(taskPool, sock, addr);
	if (NULL == tcpTask)
		//内存不足，直接关闭连接
		TEMP_FAILURE_RETRY(::close(sock));
	else if(!taskPool->addVerify(tcpTask))
	{
		//得到了一个正确连接，添加到验证队列中
		SAFE_DELETE(tcpTask);
	}
}

bool GatewayService::notifyLoginServer()
{
	Zebra::logger->trace("GatewayService::notifyLoginServer");
	using namespace Cmd::Super;
	t_GYList_Gateway tCmd;

	tCmd.wdServerID = wdServerID;
	bcopy(pstrExtIP, tCmd.pstrIP, sizeof(tCmd.pstrIP));
	tCmd.wdPort = wdExtPort;
	if(!GatewayService::getInstance().isTerminate())
	{
		tCmd.wdNumOnline = getPoolSize();
		Zebra::logger->debug("网关目前在线人数:%d",tCmd.wdNumOnline);
	}
	else
	{
		tCmd.wdNumOnline = 0;
	}
	tCmd.state = getPoolState();
	tCmd.zoneGameVersion = verify_client_version;

	return sendCmdToSuperServer(&tCmd, sizeof(tCmd));
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
bool GatewayService::msgParse_SuperService(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	Zebra::logger->trace("GatewayService::msgParse_SuperService");
	using namespace Cmd::Super;

	if (CMD_GATEWAY == ptNullCmd->cmd)
	{
		switch(ptNullCmd->para)
		{
			case PARA_GATEWAY_RQGYLIST:
				{
					//t_GYList_Gateway *ptCmd = (t_GYList_Gateway *)ptNullCmd;

					return notifyLoginServer();
				}
				break;
			case PARA_CHARNAME_GATEWAY:
				{
					using namespace Cmd::RoleReg;
					t_Charname_Gateway *rev = (t_Charname_Gateway *)ptNullCmd;
					GateUser *pUser=GateUserManager::getInstance()->getUserByAccID(rev->accid);
					if(pUser
							&& pUser->isCreateState()
							&& rev->state & ROLEREG_STATE_TEST)
					{
						if (rev->state & ROLEREG_STATE_HAS)
						{
							//创建角色失败，角色名称重复
							pUser->nameRepeat();
							Zebra::logger->trace("角色名重复 GatewayService::msgParse_SuperService");
						}
						else
						{
							if (!recordClient->sendCmd(&pUser->createCharCmd,sizeof(pUser->createCharCmd)))
								return false;
						}
					}

					return true;
				}
				break;
		}
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
void GatewayService::final()
{
	Zebra::logger->trace("GatewayService::final");
	GatewayTimeTick::getInstance().final();
	GatewayTimeTick::getInstance().join();
	GatewayTimeTick::delInstance();
	GateUserManager::getInstance()->removeAllUser(); 

	if(taskPool)
	{
		taskPool->final();
		SAFE_DELETE(taskPool);
	}
	//必须放再taskPool之后处理,否则会down机
	//SceneClientManager::getInstance().final();
	SceneClientManager::delInstance();
	// */
	if(sessionClient)
	{
		sessionClient->final();
		sessionClient->join();
		SAFE_DELETE(sessionClient);
	}
	if(recordClient)
	{
		recordClient->final();
		recordClient->join();
		SAFE_DELETE(recordClient);
	}
	zSubNetService::final();

	GatewayTaskManager::delInstance();

	LoginSessionManager::delInstance();

	GateUserManager::delInstance();

}

/**
 * \brief 命令行参数
 *
 */
static struct argp_option gateway_options[] =
{
	{"daemon",		'd',	0,			0,	"Run service as daemon",						0},
	{"log",			'l',	"level",	0,	"Log level",									0},
	{"logfilename",	'f',	"filename",	0,	"Log file name",								0},
	{"mysql",		'y',	"mysql",	0,	"MySQL[mysql://user:passwd@host:port/dbName]",	0},
	{"ifname",		'i',	"ifname",	0,	"Local network device",							0},
	{"server",		's',	"ip",		0,	"Super server ip address",						0},
	{"port",		'p',	"port",		0,	"Super server port number",						0},
	{"maintain",	'm',	0,			0,	"Run service as maintain mode",					0},
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
static error_t gateway_parse_opt(int key, char *arg, struct argp_state *state)
{
	switch (key)
	{
		case 'd':
			{
				Zebra::global["daemon"] = "true";
			}
			break;
		case 'm':
			{
				Zebra::global["initThreadPoolState"]="maintain";
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
static char gateway_doc[] = "\nGatewayServer\n" "\t网关服务器。";

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
class GatewayConfile:public zConfile
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
void GatewayService::reloadConfig()
{
	Zebra::logger->debug("%s", __PRETTY_FUNCTION__);
	GatewayConfile gc;
	gc.parse("GatewayServer");
	if ("1" == Zebra::global["rolereg_verify"])
		GatewayService::getInstance().rolereg_verify = true;
	else
		GatewayService::getInstance().rolereg_verify = false;
	if ("true" == Zebra::global["jpeg_passport"])
		GatewayService::getInstance().jpeg_passport = true;
	else
		GatewayService::getInstance().jpeg_passport = false;
	
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
	
	if(!country_info.reload())
	{
		Zebra::logger->error("重新加载国家配置!");
	}

	merge_version = atoi(Zebra::global["merge_version"].c_str());
	#ifdef _ZJW_DEBUG
	Zebra::logger->debug("[合区]: 重新加载合区版本号", merge_version);
	#endif	
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
	Zebra::logger=new zLogger("GatewayServer");

	//设置缺省参数
	Zebra::global["logfilename"] = "/tmp/gatewayserver.log";
///////////////////////////////////////////////////////////
    Zebra::global["sceneinfofile"] = "scenesinfo.xml"; //soke
  //Zebra::global["sceneinfofile"] = "ScenesServer/scenesinfo.xml";
///////////////////////////////////////////////////////////
	Zebra::global["countryorder"] = "0";

	//解析配置文件参数
	GatewayConfile gc;
	if (!gc.parse("GatewayServer"))
		return EXIT_FAILURE;

	//Zebra::logger->debug("%s", Zebra::global["rolereg_verify"].c_str());
	if ("1" == Zebra::global["rolereg_verify"])
		GatewayService::getInstance().rolereg_verify = true;
	else
		GatewayService::getInstance().rolereg_verify = false;
	if ("true" == Zebra::global["jpeg_passport"])
		GatewayService::getInstance().jpeg_passport = true;
	else
		GatewayService::getInstance().jpeg_passport = false;

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

	merge_version = atoi(Zebra::global["merge_version"].c_str());

	//解析命令行参数
	zArg::getArg()->add(gateway_options, gateway_parse_opt, 0, gateway_doc);
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

	GatewayService::getInstance().main();
	//GatewayService::delInstance();

	return EXIT_SUCCESS;
}

