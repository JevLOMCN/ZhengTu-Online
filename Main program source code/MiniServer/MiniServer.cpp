/**
 * \file
 * \version  $Id: MiniServer.cpp  $
 * \author  
 * \date 
 * \brief zebra项目计费服务器
 *
 */

#include "zSubNetService.h"
#include "Zebra.h"
#include "zMisc.h"
#include "MiniServer.h"
#include "MiniTask.h"
#include "zDBConnPool.h"
#include "zConfile.h"
#include "MiniTaskManager.h"
#include "TimeTick.h"
#include "zMetaData.h"
#include "MiniUser.h"
#include "MiniUserManager.h"
#include "MiniHall.h"

zDBConnPool *MiniService::dbConnPool = NULL;
MiniService *MiniService::instance = NULL;
MetaData* MiniService::metaData = NULL;

/**
 * \brief 初始化网络服务器程序
 *
 * 实现了虚函数<code>zService::init</code>
 *
 * \return 是否成功
 */
bool MiniService::init()
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

	/*
	tradelog = new zLogger("minilog");

	//设置日志级别
	tradelog->setLevel(Zebra::global["log"]);
	//设置写本地日志文件
	if ("" != Zebra::global["gold_tradelog"])
	{
		tradelog->addLocalFileLog(Zebra::global["gold_tradelog"]);
		//tradelog->removeConsoleLog();
	}
	*/

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

	strncpy(pstrIP, zSocket::getIPByIfName(Zebra::global["ifname"].c_str()), MAX_IP_LENGTH - 1);

	MiniTimeTick::getInstance().start();
	if (!MiniHall::getMe().init()) return false;

	if (!zSubNetService::init())
		return false;

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
void MiniService::newTCPTask(const int sock, const struct sockaddr_in *addr)
{
	//Zebra::logger->debug(__PRETTY_FUNCTION__);
	MiniTask *tcpTask = new MiniTask(taskPool, sock, addr);
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
bool MiniService::msgParse_SuperService(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Super;

	return true;

	//Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	//return false;
}

/**
 * \brief 结束网络服务器
 *
 * 实现了纯虚函数<code>zService::final</code>
 *
 */
void MiniService::final()
{
	MiniTimeTick::getInstance().final();
	MiniTimeTick::getInstance().join();
	MiniTimeTick::delInstance();

	if(taskPool)
	{
		SAFE_DELETE(taskPool);
	}

	MiniTaskManager::delInstance();
	MiniUserManager::delInstance();

	zSubNetService::final();

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
static char account_doc[] = "\nMiniServer\n" "\t小游戏服务器。";

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
class MiniConfile:public zConfile
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
void MiniService::reloadConfig()
{
	Zebra::logger->debug("%s", __PRETTY_FUNCTION__);
	MiniConfile rc;
	rc.parse("MiniServer");
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
	Zebra::logger=new zLogger("MiniServer");

	//设置缺省参数
	Zebra::global["mysql"] = "mysql://Zebra:Zebra@192.168.1.162:3306/MiniServer";
	Zebra::global["logfilename"] = "/tmp/miniserver.log";

	//解析配置文件参数
	MiniConfile rc;
	if (!rc.parse("MiniServer"))
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

	MiniService::getInstance().main();
	MiniService::delInstance();

	return EXIT_SUCCESS;
}
