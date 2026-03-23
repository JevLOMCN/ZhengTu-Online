/**
 * \file
 * \version  $Id: FLServer.cpp  $
 * \author  
 * \date 
 * \brief zebra项目登陆服务器，负责登陆，建立帐号、档案等功能
 *
 */

#include "Zebra.h"
#include "zMisc.h"
#include "FLServer.h"
#include "zMNetService.h"
#include "LoginTask.h"
#include "LoginManager.h"
#include "zArg.h"
#include "zConfile.h"
#include "GYListManager.h"
#include "Command.h"
#include "ServerTask.h"
#include "ServerManager.h"
#include "PingTask.h"
#include "ServerACL.h"
#include "DBAccessClient.h"
#include "DBAccessClientManager.h"
#include "TimeTick.h"
#include "InfoClient.h"
#include "InfoClientManager.h"


zDBConnPool * FLService::dbConnPool = NULL;
FLService *FLService::instance = NULL;

/**
 * \brief 初始化网络服务器程序
 *
 * 实现了虚函数<code>zService::init</code>
 *
 * \return 是否成功
 */
bool FLService::init()
{
	Zebra::logger->trace("FLService::init");	
#if 1
	dbConnPool = zDBConnPool::newInstance(NULL);

	if (NULL == dbConnPool
			|| !dbConnPool->putURL(0, Zebra::global["mysql"].c_str(), false))
	{
		Zebra::logger->error("连接数据库失败 %s", Zebra::global["mysql"].c_str());
		return false;
	}
#endif

	/*
	if (!DBAccessClientManager::getInstance().init())
		return false;
	*/
	if (!zMNetService::init())
	{
		return false;
	}

	
	if (!ServerACLSingleton::instance().init())
		return false;

	if (!InfoClientManager::getInstance().init())
		return false;
	

	//初始化连接线程池
	int state = state_none;
	Zebra::to_lower(Zebra::global["initThreadPoolState"]);
	if ("repair" == Zebra::global["initThreadPoolState"]
			|| "maintain" == Zebra::global["initThreadPoolState"])
		state = state_maintain;
	
	loginTaskPool = new zTCPTaskPool(atoi(Zebra::global["threadPoolCapacity"].c_str()), state);
	if (NULL == loginTaskPool
			|| !loginTaskPool->init())
		return false;
	
	serverTaskPool = new zTCPTaskPool(atoi(Zebra::global["threadPoolCapacity"].c_str()), state);
	if (NULL == serverTaskPool
			|| !serverTaskPool->init())
		return false;
	
	pingTaskPool = new zTCPTaskPool(atoi(Zebra::global["threadPoolCapacity"].c_str()), state);
	if (NULL == pingTaskPool
			|| !pingTaskPool->init())
		return false;

	login_port = atoi(Zebra::global["login_port"].c_str());
	inside_port = atoi(Zebra::global["inside_port"].c_str());
	ping_port  = atoi(Zebra::global["ping_port"].c_str());
	
	if (!zMNetService::bind("登陆端口", login_port)
			|| !zMNetService::bind("内部服务端口", inside_port)
			|| !zMNetService::bind("PING端口", ping_port))
	{
		return false;
	}

	FLTimeTick::getInstance().start();

#if 0
	GameZone_t zone;
	Zebra::logger->debug("sizeof(GameZone_t) = %u", sizeof(GameZone_t));
	zone.id = 0x0000ffff;
	Zebra::logger->debug("id = 0x%08x, game = 0x%04x, zone = 0x%04x", zone.id, zone.game, zone.zone);
#endif

	return true;
}

/**
 * \brief 新建立一个连接任务
 * 实现纯虚函数<code>zMNetService::newTCPTask</code>
 * \param sock TCP/IP连接
 * \param srcPort 连接来源端口
 * \return 新的连接任务
 */
void FLService::newTCPTask(const int sock, const unsigned short srcPort)
{	
	Zebra::logger->trace("FLService::newTCPTask");
	
	if (srcPort == login_port)
	{
		//客户端登陆验证连接
		zTCPTask *tcpTask = new LoginTask(loginTaskPool, sock);
		if (NULL == tcpTask)
			TEMP_FAILURE_RETRY(::close(sock));
		else if(!loginTaskPool->addVerify(tcpTask))
		{
			SAFE_DELETE(tcpTask);
		}
	}
	else if (srcPort == inside_port)
	{
		//每个区的服务器管理器连接
		zTCPTask *tcpTask = new ServerTask(serverTaskPool, sock);
		if (NULL == tcpTask)
			TEMP_FAILURE_RETRY(::close(sock));
		else if(!serverTaskPool->addVerify(tcpTask))
		{
			SAFE_DELETE(tcpTask);
		}
	}
	else if(srcPort == ping_port)
	{
		// 获取PING服务器列表
		zTCPTask *tcpTask = new PingTask(serverTaskPool, sock);
		if (NULL == tcpTask)
			TEMP_FAILURE_RETRY(::close(sock));
		else if(!pingTaskPool->addVerify(tcpTask))
		{
			SAFE_DELETE(tcpTask);
		}
	}
	else
		TEMP_FAILURE_RETRY(::close(sock));
}

/**
 * \brief 结束网络服务器
 *
 * 实现了纯虚函数<code>zService::final</code>
 *
 */
void FLService::final()
{
	Zebra::logger->trace("FLService::final");
	zMNetService::final();

	ServerManager::delInstance();
	LoginManager::delInstance();
	GYListManager::delInstance();

	FLTimeTick::getInstance().final();
	FLTimeTick::getInstance().join();
	FLTimeTick::delInstance();

	InfoClientManager::delInstance();

}

/**
 * \brief 命令行参数
 *
 */
static struct argp_option login_options[] =
{
	{"daemon",		'd',	0,			0,	"Run service as daemon",						0},
	{"log",			'l',	"level",	0,	"Log level",									0},
	{"logfilename",	'f',	"filename",	0,	"Log file name",								0},
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
static error_t login_parse_opt(int key, char *arg, struct argp_state *state)
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
static char login_doc[] = "\nFLServer\n" "\t登陆服务器。";

/**
 * \brief 程序的版本信息
 *
 */
const char *argp_program_version = "Program version :\t" VERSION_STRING\
									"\nBuild version   :\t" _S(BUILD_STRING);

/**
 * \brief 读取配置文件
 *
 */
class LoginConfile:public zConfile
{
	bool parseYour(const xmlNodePtr node)
	{
		if (node)
		{
			xmlNodePtr child=parser.getChildNode(node,NULL);
			while(child)
			{
				if(strcasecmp((char *)child->name,"InfoServer")==0)
				{
					char buf[64];
					if(parser.getNodeContentStr(child,buf,64))
					{
						Zebra::global["InfoServer"]=buf;
						if(parser.getNodePropStr(child,"port",buf,64))
							Zebra::global["InfoPort"]=buf;
					}
				}
				else
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
void FLService::reloadConfig()
{
	Zebra::logger->trace("FLService::reloadConfig");
	LoginConfile sc;
	sc.parse("FLServer");
	if ("true" == Zebra::global["jpeg_passport"])
		FLService::getInstance().jpeg_passport = true;
	else
		FLService::getInstance().jpeg_passport = false;
	ServerACLSingleton::instance().init();
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
	Zebra::logger=new zLogger("FLServer");

	//设置缺省参数
	Zebra::global["login_port"] = "7000";
	Zebra::global["inside_port"] = "7001";
	Zebra::global["ping_port"] = "7002";
	Zebra::global["dbAccessServerListFile"] = "dbAccessServerList.xml";
	Zebra::global["log"] = "debug";
	Zebra::global["logfilename"] = "/tmp/flserver.log";
	Zebra::global["zoneInfoFile"] = "zoneInfo.xml";
	Zebra::global["InfoServer"]="192.168.2.11";
	Zebra::global["InfoPort"]="9903";
	Zebra::global["jpeg_passport"]="true";

	//解析配置文件参数
	LoginConfile sc;
	if (!sc.parse("FLServer"))
		return EXIT_FAILURE;
	if ("true" == Zebra::global["jpeg_passport"])
		FLService::getInstance().jpeg_passport = true;
	else
		FLService::getInstance().jpeg_passport = false;
	if(atoi(Zebra::global["maxGatewayUser"].c_str()))
	{
		LoginManager::maxGatewayUser = atoi(Zebra::global["maxGatewayUser"].c_str());
	}

	//解析命令行参数
	zArg::getArg()->add(login_options, login_parse_opt, 0, login_doc);
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

	FLService::getInstance().main();
	FLService::delInstance();

	return EXIT_SUCCESS;
}

