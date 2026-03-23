/**
 * \file
 * \version  $Id: base64.cpp 609 2005-02-25 12:53:20Z song $
 * \author  宋仕良,songsiliang@netease.com
 * \date 2004年12月29日 11时15分02秒 CST
 * \brief base64编码解码程序
 *
 */


#include <iostream>
#include <string>

#include "zType.h"
#include "zArg.h"
#include "zBase64.h"
#include "Zebra.h"

static struct argp_option base64_options[] =
{
	//{"method",	'm',	"encode/decode",	0,	"Encode or decode string",		0},
	{0,			0,		0,					0,	0,								0}
};

static error_t base64_parse_opt(int key, char *arg, struct argp_state *state)
{
	switch (key)
	{
		/*case 'm':
			{
				Zebra::global["method"]=arg;
			}
			break;*/
		case ARGP_KEY_NO_ARGS:
			argp_usage(state);
			break;
		case ARGP_KEY_ARG:
			if (state->arg_num >= 1) {
				/* Too many arguments. */
				argp_usage(state);
			}
			else
			{
				/*if ("encode" == Zebra::global["method"])
				{*/
					std::string s;
					Zebra::base64_encrypt(arg, s);
					Zebra::logger->debug("%s", s.c_str());
				/*}
				else if ("decode" == Zebra::global["method"])
				{
					std::string s;
					Zebra::base64_decrypt(arg, s);
					Zebra::logger->debug("%s", s.c_str());
				}
				else
					argp_usage(state);*/
			}
			break;
		case ARGP_KEY_END:
			if (state->arg_num < 1) {
				/* Not enough arguments. */
				argp_usage(state);
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
static char base64_doc[] = "\nbase64\n" "\tBase64编码解码工具。";

/**
 * \brief 程序的版本信息
 *
 */
const char *argp_program_version = "Program version :\t" VERSION_STRING\
									"\nBuild version   :\t" _S(BUILD_STRING);

int main(int argc, char *argv[])
{
	Zebra::logger=new zLogger();

	//解析命令行参数
	zArg::getArg()->add(base64_options, base64_parse_opt, 0, base64_doc);
	zArg::getArg()->parse(argc, argv);

	return EXIT_SUCCESS;
}

