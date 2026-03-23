/**
 * \file
 * \version  $Id: SuperServer.cpp  $
 * \author  
 * \date 
 * \brief 实现服务器管理器
 *
 * 对一个区中的所有服务器进行管理
 * 
 */

#include <iostream>
#include <string>
#include <ext/numeric>

#include "zService.h"
#include "zThread.h"
#include "zSocket.h"
#include "zTCPServer.h"
#include "zNetService.h"
#include "ServerTask.h"
#include "zDBConnPool.h"
#include "SuperServer.h"
#include "Zebra.h"
#include "zMisc.h"
#include "zArg.h"
#include "zConfile.h"
#include "TimeTick.h"
#include "ServerManager.h"
#include "FLClient.h"
#include "FLClientManager.h"
#include "RoleregClient.h"
#include "RoleregClientManager.h"
#include "RoleregCache.h"
#include "InfoClient.h"
#include "InfoClientManager.h"

zDBConnPool *SuperService::dbConnPool = NULL;

SuperService *SuperService::instance = NULL;

/**
 * \brief 从数据库中获取服务器信息
 *
 * 如果数据库中没有服务器管理器的信息，需要初始化一条记录
 *
 */
bool SuperService::getServerInfo()
{
	static const dbCol col_define[] =
	{
		{"ID",zDBConnPool::DB_WORD,sizeof(WORD)},
		{"TYPE",zDBConnPool::DB_WORD,sizeof(WORD)},
		{"NAME",zDBConnPool::DB_STR,sizeof(char[MAX_NAMESIZE])},
		{"IP",zDBConnPool::DB_STR,sizeof(char[MAX_IP_LENGTH])},
		{"PORT",zDBConnPool::DB_WORD,sizeof(WORD)},
		{"EXTIP",zDBConnPool::DB_STR,sizeof(char[MAX_IP_LENGTH])},
		{"EXTPORT",zDBConnPool::DB_WORD,sizeof(WORD)},
		{NULL, 0, 0}
	};
	struct
	{
		WORD wdServerID;
		WORD wdServerType;
		char pstrName[MAX_NAMESIZE];
		char pstrIP[MAX_IP_LENGTH];
		WORD wdPort;
		char pstrExtIP[MAX_IP_LENGTH];
		WORD wdExtPort;
	} __attribute__ ((packed))
	data, *pData = NULL;
	char where[32];

	connHandleID handle = dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能从数据库连接池获取连接句柄");
		return false;
	}
	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "`TYPE`=%u", SUPERSERVER);
	unsigned int retcode = dbConnPool->exeSelect(handle, "`SERVERLIST`", col_define, where, NULL,(unsigned char **)&pData);
	if ((unsigned int)1 == retcode && pData)
	{
		//只有一条满足条件的记录
		if (strcmp(pstrIP, pData->pstrIP) == 0)
		{
			wdServerID = pData->wdServerID;
			strncpy(pstrName, pData->pstrName, sizeof(pstrName) - 1);
			wdPort = pData->wdPort;
			wdExtPort = pData->wdExtPort;
			SAFE_DELETE_VEC(pData);

			Zebra::logger->debug("ServerID=%u IP=%s Port=%u",wdServerID, pstrIP, wdPort);
		}
		else
		{
			Zebra::logger->error("数据库中的记录不符合：%s, %s", pstrIP, pData->pstrIP);
			SAFE_DELETE_VEC(pData);
			dbConnPool->putHandle(handle);
			return false;
		}
	}
	else if(0 == retcode)
	{
		//数据库中没有记录，根据一些缺省信息，生成一条记录
		strncpy(pstrName, "服务器管理器", sizeof(pstrName) - 1);
		wdExtPort = wdPort = 10000;

		bzero(&data, sizeof(data));
		data.wdServerID = wdServerID;
		data.wdServerType = wdServerType;
		strncpy(data.pstrName, pstrName, sizeof(data.pstrName) - 1);
		strncpy(data.pstrIP, pstrIP, sizeof(data.pstrIP) - 1);
		data.wdPort = wdPort;
		strncpy(data.pstrExtIP, pstrExtIP, sizeof(data.pstrExtIP) - 1);
		data.wdExtPort = wdExtPort;
		if ((unsigned int)-1 == dbConnPool->exeInsert(handle, "`SERVERLIST`", col_define, (const unsigned char *)&data))
		{
			Zebra::logger->error("向数据库中插入服务器信息记录失败");
			dbConnPool->putHandle(handle);
			return false;
		}
	}
	else
	{
		//查询出错，或者记录太多
		Zebra::logger->error("查询数据出错，或者数据库中服务器管理器的记录太多，需要整理");
		SAFE_DELETE_VEC(pData);
		dbConnPool->putHandle(handle);
		return false;
	}
	dbConnPool->putHandle(handle);

	return true;
}

/**
 * \brief 初始化网络服务器程序
 *
 * 实现纯虚函数<code>zService::init</code>
 *
 * \return 是否成功
 */
bool SuperService::init()
{
	Zebra::logger->trace("SuperService::init");

	dbConnPool = zDBConnPool::newInstance(NULL);
	if (NULL == dbConnPool
			|| !dbConnPool->putURL(0, Zebra::global["mysql"].c_str(), false))
	{
		Zebra::logger->error("连接数据库失败");
		return false;
	}

	/////////////////////////////////////////////////////////////////////////
	//soke 原版
	strncpy(pstrIP, zSocket::getIPByIfName(Zebra::global["ifname"].c_str()), MAX_IP_LENGTH - 1);
	Zebra::logger->debug("%s", pstrIP);

	//soke 绑定IP(如果是绑定IP，注释上面的2行原版)
	
	//strcpy(pstrIP, "192.168.62.128");

	/////////////////////////////////////////////////////////////////////////


	if (!getServerInfo())
		return false;

	if (!FLClientManager::getInstance().init())
		return false;

	if (!RoleregClientManager::getInstance().init())
		return false;

	if (!InfoClientManager::getInstance().init())
		return false;

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

	if (!zNetService::init(wdPort))
	{
		return false;
	}

	SuperTimeTick::getInstance().start();

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
void SuperService::newTCPTask(const int sock, const struct sockaddr_in *addr)
{
	ServerTask *tcpTask = new ServerTask(taskPool, sock, addr);
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
 * \brief 结束网络服务器
 *
 * 实现纯虚函数<code>zService::final</code>
 *
 */
void SuperService::final()
{
	SuperTimeTick::getInstance().final();
	SuperTimeTick::getInstance().join();
	SuperTimeTick::delInstance();

	if(taskPool)
	{
		taskPool->final();
		SAFE_DELETE(taskPool);
	}
	InfoClientManager::delInstance();

	zNetService::final();

	ServerManager::delInstance();

	FLClientManager::delInstance();

	RoleregClientManager::delInstance();

	RoleregCache::delInstance();

	zDBConnPool::delInstance(&dbConnPool);

	Zebra::logger->debug("%s", __PRETTY_FUNCTION__);
}

/**
 * \brief 命令行参数
 *
 */
static struct argp_option super_options[] =
{
	{"daemon",		'd',	0,			0,	"Run service as daemon",						0},
	{"log",			'l',	"level",	0,	"Log level",									0},
	{"logfilename",	'f',	"filename",	0,	"Log file name",								0},
	{"mysql",		'y',	"mysql",	0,	"MySQL[mysql://user:passwd@host:port/dbName]",	0},
	{"ifname",		'i',	"ifname",	0,	"Local network device",							0},
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
static error_t super_parse_opt(int key, char *arg, struct argp_state *state)
{
	switch (key)
	{
		case 'd':
			{
				Zebra::global["daemon"] = "true";
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
static char super_doc[] = "\nSuperServer\n" "\t服务器管理器。";

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
class SuperConfile:public zConfile
{
	bool parseYour(const xmlNodePtr node)
	{
		if(node)
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
void SuperService::reloadConfig()
{
	Zebra::logger->debug("%s", __PRETTY_FUNCTION__);
	SuperConfile sc;
	sc.parse("SuperServer");
	//指令检测开关
	if(Zebra::global["cmdswitch"] == "true")
	{
		zTCPTask::analysis._switch = true;
	}
	else
	{
		zTCPTask::analysis._switch = false;
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
	Zebra::logger=new zLogger("SuperServer");

	//设置缺省参数
	Zebra::global["loginServerListFile"] = "SuperServer/loginServerList.xml";
	Zebra::global["mysql"] = "mysql://Zebra:Zebra@192.168.1.162:3306/SuperServer";
	Zebra::global["logfilename"] = "/tmp/superserver.log";

	//解析配置文件参数
	SuperConfile sc;
	if (!sc.parse("SuperServer"))
		return EXIT_FAILURE;

	//指令检测开关
	if(Zebra::global["cmdswitch"] == "true")
	{
		zTCPTask::analysis._switch = true;
	}
	else
	{
		zTCPTask::analysis._switch = false;
	}
	//解析命令行参数
	zArg::getArg()->add(super_options, super_parse_opt, 0, super_doc);
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

	SuperService::getInstance().main();
	SuperService::delInstance();

	return EXIT_SUCCESS;
}

