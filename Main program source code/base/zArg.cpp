/**
 * \file
 * \version  $Id: zArg.cpp  $
 * \author  
 * \date 
 * \brief 参数解析类的定义。
 *
 * 
 */

#include "zArg.h"
#include "Zebra.h"
#include <string.h>

/**
 * \brief 参数BUG报告地址
 */
const char *argp_program_bug_address = "<631768533@qq.com, xztgc@vip.qq.com>";

/**
 * \brief 默认参数描述
 */
static const char zebra_args_doc[] = "";
/**
 * \brief 默认参数描述
 */
static const char zebra_doc[] = "this is default argument document.";
/**
 * \brief 默认可用选项
 */
static struct argp_option zebra_options[] =
{
	{0,	0,	0,	0,	0, 0}
};
/**
 * \brief 默认的分析函数,参见info argp_parse;
 */
error_t zparse_opt(int key, char *arg, struct argp_state *state)
{
	switch (key)
	{
		default:
			if(zArg::getArg()->user_parser!=0)
				return zArg::getArg()->user_parser(key,arg,state);
			else
				return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

zArg *zArg::argInstance(0);

/**
 * \brief 构造函数，初始化默认参数分析数据
 */
zArg::zArg()
{
		user_parser=0;
		alloptions=0;

		argp.children=0;
		argp.help_filter=0;
		argp.argp_domain=0;

		argp.parser=zparse_opt;
		argp.args_doc=zebra_args_doc;
		argp.doc=zebra_doc;
		addOptions(zebra_options);
}

/**
 * \brief 析构函数
 */
zArg::~zArg()
{
	SAFE_DELETE_VEC(alloptions);
}

/**
 * \brief 得到参数分析器
 * \return 参数分析器指针
 */
zArg *zArg::getArg()
{
	if(argInstance==0)
		argInstance=new zArg();
	return argInstance;
}

/**
 * \brief 删除参数分析器
 */
void zArg::removeArg()
{
	SAFE_DELETE(argInstance);
}

/**
 * \brief 添加参数选项
 * \param options 要添加的参数选项数组
 */
void zArg::addOptions(const struct argp_option *options)
{
	if(options==0) return;

	int ucount=0;
	while(options[ucount].name!=0)
		ucount++;

	if(alloptions==0)
	{
		alloptions=new struct argp_option[ucount+1];
		memcpy(alloptions,options,sizeof(argp_option)*(ucount+1));

	}
	else
	{
		int ocount=0;
		while(alloptions[ocount].name!=0)
			ocount++;

		//std::cout << "ucount:" << ucount << std::endl;
		//std::cout << "ocount:" << ocount << std::endl;
		//std::cout << "allcount" << ucount+ocount+1 << std::endl;
		struct argp_option *otemp=alloptions;
		alloptions=new argp_option[ucount+ocount+1];
		if (ocount > 0) memcpy(alloptions,otemp,sizeof(argp_option)*ocount);
		memcpy(alloptions+ocount,options,sizeof(argp_option)*(ucount+1));
		SAFE_DELETE_VEC(otemp);
	}
	argp.options=alloptions;
}

/**
 * \brief 添加自己的参数选项和分析器，及参数文档，如果省略用默认值
 * \param options 自己的参数选项
 * \param func 自己参数选项的分析函数
 * \param args_doc 参数选项详细文档
 * \param doc 必要参数选项文档
 * \return 始终返回true
 */
bool zArg::add(const struct argp_option *options,argsParser func,const char *args_doc,const char *doc)
{
	if(func!=0)
		user_parser=func;
	if(options!=0)
		addOptions(options);
	if(args_doc!=0)
		argp.args_doc=args_doc;
	if(doc!=0)
		argp.doc=doc;
	return true;
}

/**
 * \brief 参数分析
 * \param argc 参数个数
 * \param argv 参数列表
 * \return 始终返回true
 */
bool zArg::parse(int argc ,char *argv[])
{
	argp_parse(&argp, argc, argv, 0, 0, 0);
	return true;
}
