/**
 * \file
 * \version  $Id: zService.cpp  $
 * \author  
 * \date 
 * \brief 实现服务器框架类
 *
 * 
 */


#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <assert.h>
#include <signal.h>
#include <unistd.h>

#include "zSocket.h"
#include "zTCPServer.h"
#include "zService.h"
#include "zProperties.h"
#include "zMisc.h"
#include "Zebra.h"


/**
 * \brief CTRL + C等信号的处理函数，结束程序
 *
 * \param signum 信号编号
 */
static void ctrlcHandler(int signum)
{
	Zebra::logger->trace("ctrlcHandler");
	//如果没有初始化zService实例，表示出错
	zService *instance = zService::serviceInstance();
	instance->Terminate();
}

/**
 * \brief HUP信号处理函数
 *
 * \param signum 信号编号
 */
static void hupHandler(int signum)
{
	Zebra::logger->trace("hupHandler");
	//如果没有初始化zService实例，表示出错
	zService *instance = zService::serviceInstance();
	instance->reloadConfig();
}

zService *zService::serviceInst = NULL;

/**
 * \brief 初始化服务器程序，子类需要实现这个函数
 *
 * \return 是否成功
 */
bool zService::init()
{
	Zebra::logger->trace("zService::init");
	//存储环境变量
	int i = 0;
	while(environ[i])
	{
		std::string s(environ[i++]);
		std::vector<std::string> v;
		Zebra::stringtok(v, s, "=", 1);
		if (!v.empty() && v.size() == 2)
			env[v[0]] = v[1];
	}
	//env.dump(std::cout);

	//设置信号处理
	struct sigaction sig;

	sig.sa_handler = ctrlcHandler;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = 0;
	sigaction(SIGINT, &sig, NULL);
	sigaction(SIGQUIT, &sig, NULL);
	sigaction(SIGABRT, &sig, NULL);
	sigaction(SIGTERM, &sig, NULL);
	sig.sa_handler = hupHandler;
	sigaction(SIGHUP, &sig, NULL);
	sig.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sig, NULL);

	//初始化随机数
	srand(time(NULL));
	Zebra::seedp = time(NULL);
	
	return true;
}

/**
 * \brief 服务程序框架的主函数
 */
void zService::main()
{
		Zebra::logger->trace("zService::main");
		//初始化程序，并确认服务器启动成功
		if (init()
		&& validate())
		{
			//运行主回调线程
			while(!isTerminate())
			{
				if (!serviceCallback())
				{
					break;
				}
			}
		}

		//结束程序，释放相应的资源
		final();
}

