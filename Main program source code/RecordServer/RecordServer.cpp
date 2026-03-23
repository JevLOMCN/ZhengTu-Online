/**
 * \file
 * \version  $Id: RecordServer.cpp  $
 * \author  
 * \date 
 * \brief zebra项目档案服务器，用于创建、储存和读取档案
 *
 */

#include "zSubNetService.h"
#include "Zebra.h"
#include "zMisc.h"
#include "RecordServer.h"
#include "RecordTask.h"
#include "zDBConnPool.h"
#include "zConfile.h"
#include "RecordSessionManager.h"

zDBConnPool *RecordService::dbConnPool = NULL;

RecordService *RecordService::instance = NULL;

/**
 * \brief 初始化网络服务器程序
 *
 * 实现了虚函数<code>zService::init</code>
 *
 * \return 是否成功
 */
bool RecordService::init()
{
	Zebra::logger->trace("RecordService::init");

	dbConnPool = zDBConnPool::newInstance(NULL);
	if (NULL == dbConnPool
			|| !dbConnPool->putURL(0, Zebra::global["mysql"].c_str(), false))
	{
		Zebra::logger->error("连接数据库失败");
		return false;
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

    //soke 征途原版配置
	strncpy(pstrIP, zSocket::getIPByIfName(Zebra::global["ifname"].c_str()), MAX_IP_LENGTH - 1);
	
    //soke 修改绑定IP start--
	
/*	char checkIP[MAX_IP_LENGTH];

	strncpy(checkIP, zSocket::getIPByIfName(Zebra::global["ifname"].c_str()), MAX_IP_LENGTH - 1);

	if(strcmp(checkIP, "192.168.62.128") != 0)
//	if(strcmp(checkIP, "192.168.0.201") != 0&&strcmp(checkIP, "192.3.171.169") != 0&&strcmp(checkIP, "222.189.238.76") != 0)
	{
		return false;
	}

	//soke 修改成127.0.0.1
	strcpy(pstrIP, "192.168.62.128");*/
	
	//soke 修改绑定IP end--	


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
void RecordService::newTCPTask(const int sock, const struct sockaddr_in *addr)
{
	Zebra::logger->trace("RecordService::newTCPTask");
	RecordTask *tcpTask = new RecordTask(taskPool, sock, addr);
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
bool RecordService::msgParse_SuperService(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	Zebra::logger->trace("RecordService::msgParse_SuperService");
	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

/**
 * \brief 结束网络服务器
 *
 * 实现了纯虚函数<code>zService::final</code>
 *
 */
void RecordService::final()
{
	Zebra::logger->trace("RecordService::final");
	while(!RecordSessionManager::getInstance().empty())
	{
		zThread::msleep(10);
	}
	if(taskPool)
	{
		taskPool->final();
		SAFE_DELETE(taskPool);
	}
	zSubNetService::final();

	RecordSessionManager::delInstance();

	zDBConnPool::delInstance(&dbConnPool);

	Zebra::logger->debug(__PRETTY_FUNCTION__);
}

/**
 * \brief 命令行参数
 *
 */
static struct argp_option record_options[] =
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
static error_t record_parse_opt(int key, char *arg, struct argp_state *state)
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
static char record_doc[] = "\nRecordServer\n" "\t档案服务器。";

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
class RecordConfile:public zConfile
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
void RecordService::reloadConfig()
{
	Zebra::logger->debug("%s", __PRETTY_FUNCTION__);
	RecordConfile rc;
	rc.parse("RecordServer");
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
	Zebra::logger=new zLogger("RecordServer");

	//设置缺省参数
	Zebra::global["mysql"] = "mysql://Zebra:Zebra@192.168.1.162:3306/RecordServer";
	Zebra::global["logfilename"] = "/tmp/recordserver.log";

	//解析配置文件参数
	RecordConfile rc;
	if (!rc.parse("RecordServer"))
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
	zArg::getArg()->add(record_options, record_parse_opt, 0, record_doc);
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

	RecordService::getInstance().main();
	RecordService::delInstance();

	return EXIT_SUCCESS;
}

