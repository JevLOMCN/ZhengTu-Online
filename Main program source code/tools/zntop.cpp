/**
 * \file
 * \version  $Id: zntop.cpp  $
 * \author 
 * \date 
 * \brief 实现流量监控程序
 *
 * 
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termcap.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <getopt.h>
#include <argp.h>
#include <string.h>

#include <iostream>
#include <string>
#include <vector>

#include "zService.h"
#include "zArg.h"
#include "Zebra.h"
#include "zTime.h"
#include "CollectServerInfo.h"

/**
 * \brief 向终端输出字符串
 *
 */
#define PUTP(x) (tputs(x, 1, putchar))


/**
 * \brief 网卡流量监控程序类，实时流量统计，可以计时写文件
 *
 * 这个类使用了Singleton设计模式，保证了一个进程环境只有一个类的实例。
 */
class zntopService : public zService
{

	public:

		/**
		 * \brief 析购函数
		 *
		 */
		~zntopService()
		{ 
			if (out) fclose(out);
		}

		/**
		 * \brief 获取类唯一的实例
		 *
		 * \return 类的唯一实例
		 */
		static zntopService &getInstance()
		{
			if (NULL == instance)
				instance = new zntopService();

			return *instance;
		}

		/**
		 * \brief 释放类的唯一实例
		 *
		 */
		static void delInstance()
		{
			if (instance)
			{
				delete instance;
				instance = NULL;
			}
		}

		bool setup_terminal();
		void update();
		bool do_key(char c);

		/**
		 * \brief 清楚屏幕
		 *
		 */
		void clear_screen()
		{
			printf("%s", cl);
		}

		static void window_size(int signo);

	private:

		zntopService();

		bool init();
		bool serviceCallback();
		void final();

		static zntopService *instance;

		static const int BUFF_LEN = 200;		/**< 缓冲大小 */
		static const long sleeptime = 1000;		/**< 刷新终端显示之间的间隔，毫秒 */

		zTime started;							/**< 开始时间 */

		struct termios savetty;					/**< 保存的终端信息 */
		struct termios rawtty;					/**< 保存的原始状态终端信息 */
		char *cm;
		char *cl;
		char *top_clrtobot;
		char *top_clrtoeol;
		char *ho;
		char *md;
		char *me;
		char *mr;
		int lines;								/**< 终端行数 */
		int cols;								/**< 终端列数 */

		zTime writeFileTime;					/**< 最后写文件时间 */
		std::string out_filename;				/**< 写流量信息到文件的文件名称 */
		int out_interval;						/**< 写文件间隔频率 */
		FILE *out;								/**< 打开的文件指针 */

};

/**
 * \brief 构造函数
 *
 */
zntopService::zntopService()
	: zService("网卡流量监控")
{
	out= NULL;
	out_filename = Zebra::global["outfile"];
	if (atoi(Zebra::global["interval"].c_str()) > 0)
	{
		out_interval = atoi(Zebra::global["interval"].c_str());
	}
	else
		out_interval = 6;
}

/**
 * \brief 类的唯一实例
 *
 */
zntopService *zntopService::instance = NULL;

/**
 * \brief 初始化程序
 *
 * \return 初始化是否成功
 */
bool zntopService::init()
{
	if (!zService::init())
		return false;

	Zebra::logger->debug(__PRETTY_FUNCTION__);

	//argp_parse(&argp, argc, argv, 0, 0, 0);

	if (!setup_terminal())
	{
		tcsetattr(0, TCSAFLUSH, &savetty);
		PUTP(tgoto(cm, 0, lines - 1));
		fputs("\r\n", stdout);
		if (out)
		{
			fclose(out);
			out = NULL;
		}
		return false;
	}

	window_size(0);
	clear_screen();

	::signal(SIGWINCH, window_size);
	::signal(SIGCONT, window_size);

	return true;
}

/**
 * \brief 程序主回调函数
 *
 * \return 回调是否成功
 */
bool zntopService::serviceCallback()
{
	struct timeval tv;
	fd_set in;
	char c;
	bool retval = true;

	ServerInfoSingleton::instance().getNetStat();
	ServerInfoSingleton::instance().getCpuInfo();

	update();

	tv.tv_sec = sleeptime / 1000;
	tv.tv_usec = (sleeptime % 1000) * 1000;

	FD_ZERO(&in);
	FD_SET(0, &in);
	if (TEMP_FAILURE_RETRY(::select(1, &in, NULL, NULL, &tv)) > 0 && read(0, &c, 1) == 1)
	{
		// keyboard pressed
		retval = do_key(c);
	}
	else
	{
		// no input, just wait for some time
		usleep(sleeptime * 1000);
	}

	return retval;
}

/**
 * \brief 服务程序结束
 *
 * 还原终端设置
 */
void zntopService::final()
{
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	tcsetattr(0, TCSAFLUSH, &savetty);
	PUTP(tgoto(cm, 0, lines - 1));
	fputs("\r\n", stdout);
	if (out)
	{
		fclose(out);
		out = NULL;
	}
}

/**
 * \brief 初始化终端
 *
 * return 初始化终端是否成功
 */
bool zntopService::setup_terminal()
{
	const char *termtype;
	struct termios newtty;

	termtype = getenv("TERM");
	if (!termtype)
		termtype = "vt100";

	if (1 != tgetent(NULL, termtype))
	{
		fprintf(stderr, "Unknown terminal \"%s\" i $TERM\n", termtype);
		exit(1);
	}

	cm = tgetstr("cm", 0);
	cl = tgetstr("cl", 0);
	top_clrtobot = tgetstr("cd", 0);
	top_clrtoeol = tgetstr("ce", 0);
	ho = tgetstr("ho", 0);
	md = tgetstr("md", 0);
	me = tgetstr("me", 0);
	mr = tgetstr("mr", 0);

	if (-1 == tcgetattr(0, &savetty))
	{
		perror("tcgetattr() failed");
		return false;
	}

	newtty = savetty;
	newtty.c_lflag &= ~ICANON;
	newtty.c_lflag &= ~ECHO;
	newtty.c_cc[VMIN] = 1;
	newtty.c_cc[VTIME] = 0;

	if (-1 == tcsetattr(0, TCSAFLUSH, &newtty))
	{
		printf("cannot put tty into raw mode\n");
		return false;
	}

	tcgetattr(0, &rawtty);

	return true;
}

/**
 * \brief 输出网卡流量统计信息到终端
 *
 */
void zntopService::update()
{
	time_t interval;

	PUTP(tgoto(cm, 0, 0));

	PUTP(ho);
	//  PUTP(md);
	PUTP(top_clrtoeol);
	interval = started.elapse();
	if (interval >= (60 * 60))
		printf("uptime %lu hours, %lu minutes, %lu seconds\n",
				interval / (60 * 60),
				(interval % (60 * 60)) / 60,
				(interval % (60 * 60)) % 60);
	else if (interval >= 60)
		printf("uptime %lu minutes, %lu seconds\n", interval / 60, interval % 60);
	else
		printf("uptime %lu seconds\n", interval);
	printf("%zu network interface founded\n\n", ServerInfoSingleton::instance().if_vector.size());
	//  PUTP(me);

	PUTP(top_clrtoeol);
	printf("Cpu(s): %7s %7s %7s %7s %7s %7s %7s\n", "user", "nice", "system", "irq", "softirq", "iowait", "idle");
	printf("Cpu   : %6.1f%% %6.1f%% %6.1f%% %6.1f%% %6.1f%% %6.1f%% %6.1f%%",
			(float)ServerInfoSingleton::instance().cpuInfo.CpuUsage[ServerInfoSingleton::instance().cpuInfo.CpuNum].usage.u / 10.0,
			(float)ServerInfoSingleton::instance().cpuInfo.CpuUsage[ServerInfoSingleton::instance().cpuInfo.CpuNum].usage.n / 10.0,
			(float)ServerInfoSingleton::instance().cpuInfo.CpuUsage[ServerInfoSingleton::instance().cpuInfo.CpuNum].usage.s / 10.0,
			(float)ServerInfoSingleton::instance().cpuInfo.CpuUsage[ServerInfoSingleton::instance().cpuInfo.CpuNum].usage.x / 10.0,
			(float)ServerInfoSingleton::instance().cpuInfo.CpuUsage[ServerInfoSingleton::instance().cpuInfo.CpuNum].usage.y / 10.0,
			(float)ServerInfoSingleton::instance().cpuInfo.CpuUsage[ServerInfoSingleton::instance().cpuInfo.CpuNum].usage.w / 10.0,
			(float)ServerInfoSingleton::instance().cpuInfo.CpuUsage[ServerInfoSingleton::instance().cpuInfo.CpuNum].usage.i / 10.0);
	printf("\n");
	for(ServerInfo::CpuInfo::size_type i = 0; i < ServerInfoSingleton::instance().cpuInfo.CpuNum; i++)
	{
		PUTP(top_clrtoeol);
		printf("Cpu%-3zu: %6.1f%% %6.1f%% %6.1f%% %6.1f%% %6.1f%% %6.1f%% %6.1f%%",
				i,
				(float)ServerInfoSingleton::instance().cpuInfo.CpuUsage[i].usage.u / 10.0,
				(float)ServerInfoSingleton::instance().cpuInfo.CpuUsage[i].usage.n / 10.0,
				(float)ServerInfoSingleton::instance().cpuInfo.CpuUsage[i].usage.s / 10.0,
				(float)ServerInfoSingleton::instance().cpuInfo.CpuUsage[i].usage.x / 10.0,
				(float)ServerInfoSingleton::instance().cpuInfo.CpuUsage[i].usage.y / 10.0,
				(float)ServerInfoSingleton::instance().cpuInfo.CpuUsage[i].usage.w / 10.0,
				(float)ServerInfoSingleton::instance().cpuInfo.CpuUsage[i].usage.i / 10.0);
		printf("\n");
	}

	PUTP(mr);
	PUTP(top_clrtoeol);
	printf("%-8s %-24s %-24s ", "if", "rx(bps)", "tx(bps)");
	printf("\n");
	PUTP(me);

	PUTP(md);
	for(ServerInfo::NetIfInfo::size_type i = 0; i < ServerInfoSingleton::instance().if_vector.size(); i++)
	{
		char buffer[24];
		PUTP(top_clrtoeol);
		printf("%-8s ", ServerInfoSingleton::instance().if_vector[i].name);

		bzero(buffer, sizeof(buffer));
		if (ServerInfoSingleton::instance().if_vector[i].rx_speed > (1024 * 1024))
			snprintf(&buffer[strlen(buffer)], sizeof(buffer) - strlen(buffer) - 1, "%.2fM", ServerInfoSingleton::instance().if_vector[i].rx_speed / (1024 * 1024));
		else if (ServerInfoSingleton::instance().if_vector[i].rx_speed > (1024))
			snprintf(&buffer[strlen(buffer)], sizeof(buffer) - strlen(buffer) - 1, "%.2fK", ServerInfoSingleton::instance().if_vector[i].rx_speed / (1024));
		else
			snprintf(&buffer[strlen(buffer)], sizeof(buffer) - strlen(buffer) - 1, "%.0f", ServerInfoSingleton::instance().if_vector[i].rx_speed);

		if (ServerInfoSingleton::instance().if_vector[i].rx_speed_max > (1024 * 1024))
			snprintf(&buffer[strlen(buffer)], sizeof(buffer) - strlen(buffer) - 1, "/%.2fM", ServerInfoSingleton::instance().if_vector[i].rx_speed_max / (1024 * 1024));
		else if (ServerInfoSingleton::instance().if_vector[i].rx_speed_max > (1024))
			snprintf(&buffer[strlen(buffer)], sizeof(buffer) - strlen(buffer) - 1, "/%.2fK", ServerInfoSingleton::instance().if_vector[i].rx_speed_max / (1024));
		else
			snprintf(&buffer[strlen(buffer)], sizeof(buffer) - strlen(buffer) - 1, "/%.0f", ServerInfoSingleton::instance().if_vector[i].rx_speed_max);
		printf("%-24s ", buffer);

		bzero(buffer, sizeof(buffer));
		if (ServerInfoSingleton::instance().if_vector[i].tx_speed > (1024 * 1024))
			snprintf(&buffer[strlen(buffer)], sizeof(buffer) - strlen(buffer) - 1, "%.2fM", ServerInfoSingleton::instance().if_vector[i].tx_speed / (1024 * 1024));
		else if (ServerInfoSingleton::instance().if_vector[i].tx_speed > (1024))
			snprintf(&buffer[strlen(buffer)], sizeof(buffer) - strlen(buffer) - 1, "%.2fK", ServerInfoSingleton::instance().if_vector[i].tx_speed / (1024));
		else
			snprintf(&buffer[strlen(buffer)], sizeof(buffer) - strlen(buffer) - 1, "%.0f", ServerInfoSingleton::instance().if_vector[i].tx_speed);

		if (ServerInfoSingleton::instance().if_vector[i].tx_speed_max > (1024 * 1024))
			snprintf(&buffer[strlen(buffer)], sizeof(buffer) - strlen(buffer) - 1, "/%.2fM", ServerInfoSingleton::instance().if_vector[i].tx_speed_max / (1024 * 1024));
		else if (ServerInfoSingleton::instance().if_vector[i].tx_speed_max > (1024))
			snprintf(&buffer[strlen(buffer)], sizeof(buffer) - strlen(buffer) - 1, "/%.2fK", ServerInfoSingleton::instance().if_vector[i].tx_speed_max / (1024));
		else
			snprintf(&buffer[strlen(buffer)], sizeof(buffer) - strlen(buffer) - 1, "/%.0f", ServerInfoSingleton::instance().if_vector[i].tx_speed_max);
		printf("%-24s ", buffer);

		printf("\n");
	}
	PUTP(me);

	printf("\n");

	PUTP(mr);
	PUTP(top_clrtoeol);
	printf("%-8s %-24s %-24s ", "if", "total rx(bytes)", "total tx(bytes)");
	printf("\n");
	PUTP(me);

	PUTP(md);
	for(ServerInfo::NetIfInfo::size_type i = 0; i < ServerInfoSingleton::instance().if_vector.size(); i++)
	{
		PUTP(top_clrtoeol);
		printf("%-8s %-24Lu %-24Lu ", ServerInfoSingleton::instance().if_vector[i].name, ServerInfoSingleton::instance().if_vector[i].current.rx_bytes, ServerInfoSingleton::instance().if_vector[i].current.tx_bytes);
		printf("\n");
	}
	PUTP(me);

	printf("\n");

	PUTP(mr);
	PUTP(top_clrtoeol);
	printf("%-8s %-24s %-24s ", "if", "rx packets", "tx packets");
	printf("\n");
	PUTP(me);

	PUTP(md);
	for(ServerInfo::NetIfInfo::size_type i = 0; i < ServerInfoSingleton::instance().if_vector.size(); i++)
	{
		PUTP(top_clrtoeol);
		printf("%-8s %-24Lu %-24Lu ", ServerInfoSingleton::instance().if_vector[i].name, ServerInfoSingleton::instance().if_vector[i].rx_packets, ServerInfoSingleton::instance().if_vector[i].tx_packets);
		printf("\n");
	}
	PUTP(me);

	PUTP(tgoto(cm, 0, 1));
	printf("\n");
	PUTP(top_clrtoeol);

	//写文件
	{
		zTime currentTime;
		if (writeFileTime.elapse(currentTime) >= out_interval)
		{
			writeFileTime = currentTime;
			if (NULL == out && out_filename.length())
			{
				out = fopen(out_filename.c_str(), "a+");
				if (out)
				{
					if (out) fprintf(out, "%-24s ", "time");
					for(ServerInfo::NetIfInfo::size_type i = 0; i < ServerInfoSingleton::instance().if_vector.size(); i++)
					{
						if (out) fprintf(out, "%-8s %-8s ", ServerInfoSingleton::instance().if_vector[i].name, ServerInfoSingleton::instance().if_vector[i].name);
					}
					if (out) fprintf(out, "\n");
				}
			}
			if (out)
			{
				char buffer[64];
				struct tm tmnow;
				time_t now = currentTime.sec();
				localtime_r(&now, &tmnow);
				
				strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S", &tmnow);
				fprintf(out, "%-24s ", buffer);
			}
			for(ServerInfo::NetIfInfo::size_type i = 0; i < ServerInfoSingleton::instance().if_vector.size(); i++)
			{
				if (out) fprintf(out, "%-8.0f %-8.0f ", ServerInfoSingleton::instance().if_vector[i].rx_speed, ServerInfoSingleton::instance().if_vector[i].tx_speed);
			}
			if (out) fprintf(out, "\n");
			if (out) fflush(out);
		}
	}
}

/**
 * \brief 处理按键
 * 
 * 相当于处理功能键
 *
 */
bool zntopService::do_key(char c)
{
	if ('q' == c)
		return false;

	return true;
}

/**
 * \brief 设置终端大小
 *
 * 同时也用于处理窗口缩放的信号
 *
 * \param signo 信号
 */
void zntopService::window_size(int signo)
{
	struct winsize ws;

	if ((ioctl(1, TIOCGWINSZ, &ws) != -1)
			&& (ws.ws_col > 73)
			&& (ws.ws_row > 7))
	{
		zntopService::getInstance().cols = ws.ws_col;
		zntopService::getInstance().lines = ws.ws_row;
	}
	else
	{
		zntopService::getInstance().cols = tgetnum("co");
		zntopService::getInstance().lines = tgetnum("li");
	}

	zntopService::getInstance().clear_screen();
}

/**
 * \brief 参数定义
 *
 */
static const struct argp_option zntop_options[] = {
	{"outfile",		'f',	"filename",	0,	"Output filename.",											0},
	{"interval",	'i',	"seconds",	0,	"Output interval(seconds), minimum value is 2 seconds.",	0},
	{0,				0,		0,			0,	0,															0}
};

/**
 * \brief 参数解析处理函数
 *
 * \param key 关键字
 * \param arg 值
 * \param state 状态
 * \return 解析是否成功
 */
error_t zntop_parse_opt(int key, char *arg, struct argp_state * state)
{
	switch (key)
	{
		case 'f':
			Zebra::global["outfile"]=arg;
			break;
		case 'i':
			Zebra::global["interval"]=arg;
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
static char zntop_doc[] = "\nzntop\n" "\t网卡流量实时监控程序。";

/**
 * \brief 程序的版本信息
 *
 */
const char *argp_program_version = "Program version :\t" VERSION_STRING\
									"\nBuild version   :\t" _S(BUILD_STRING);


int main(int argc, char **argv)
{
	Zebra::logger=new zLogger();

	//测试一维数组以及二维数组的效率
#if 0
	const unsigned int high = 100, weight = 100, maxcount = 1000;
	std::vector<unsigned int> vec(high * weight, 999);
	{
		FunctionTime func_time(0,__PRETTY_FUNCTION__,"整个任务遍历需要的时间" , 32);
		for(unsigned int k = 0; k < maxcount; k++)
		{
			for(unsigned int i = 0; i < high; i++)
			{
				for(unsigned int j = 0; j < weight; j++)
				{
					unsigned int haha = vec[i * high +j];
					haha = haha * haha;
				}
			}
		}
	}

	unsigned int *one_dim = new unsigned int [high * weight];
	for(unsigned int i = 0; i < high * weight; i++)
	{
			one_dim[i] = 999;
	}
	{
		FunctionTime func_time(0,__PRETTY_FUNCTION__,"整个任务遍历需要的时间" , 32);
		for(unsigned int k = 0; k < maxcount; k++)
		{
			for(unsigned int i = 0; i < high; i++)
			{
				for(unsigned int j = 0; j < weight; j++)
				{
					unsigned int haha = one_dim[i * high +j];
					haha = haha * haha;
				}
			}
		}
	}
	unsigned int **two_dim = new unsigned int *[high];
	for(unsigned int i = 0; i < high; i++)
	{
		two_dim[i] = new unsigned int[weight];
		for(unsigned int j = 0; j < weight; j++)
		{
			two_dim[i][j] = 999;
		}
	}
	{
		FunctionTime func_time(0,__PRETTY_FUNCTION__,"整个任务遍历需要的时间" , 32);
		for(unsigned int k = 0; k < maxcount; k++)
		{
			for(unsigned int i = 0; i < high; i++)
			{
				for(unsigned int j = 0; j < weight; j++)
				{
					unsigned int haha = two_dim[i][j];
					haha = haha * haha;
				}
			}
		}
	}
	{
		FunctionTime func_time(0,__PRETTY_FUNCTION__,"整个任务遍历需要的时间" , 32);
		for(unsigned int k = 0; k < maxcount; k++)
		{
			for(unsigned int i = 0; i < high; i++)
			{
				for(unsigned int j = 0; j < weight; j++)
				{
					unsigned int haha = *(*(two_dim + i) + j);
					haha = haha * haha;
				}
			}
		}
	}

	return true;
#endif
	//解析命令行参数
	zArg::getArg()->add(zntop_options, zntop_parse_opt, 0, zntop_doc);
	zArg::getArg()->parse(argc, argv);
	//Zebra::global.dump(std::cout);

	//std::string xmlStr;
	//std::cout << "(" << xmlStr.length() << ")" << xmlStr << std::endl;
	//std::cout << "=========================" << std::endl;
	//ServerInfoSingleton::instance().getServerInfo(xmlStr);
	//sleep(2);
	zntopService::getInstance().main();
	zntopService::delInstance();

	return EXIT_SUCCESS;
}

