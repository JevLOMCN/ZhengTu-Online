/**
 * \file
 * \version  $Id: zArg.h  $
 * \author  
 * \date 
 * \brief 参数解析类的定义。
 *
 * 
 */

#ifndef _ZARG_H_
#define _ZARG_H_

#include <argp.h>
#include "zProperties.h"
#include "zNoncopyable.h"

/**
 * \brief 参数分析函数定义
 */
typedef error_t(* argsParser)(int key, char *arg, struct argp_state *state) ;

/**
 * \brief 参数类,一个进程只有一个
 *
 * 建议所有参数保存在zArg::args中，它的使用参见#zProperties.
 *
 * 建议用继承实现某一类的程序的默认参数.
 *
 * 关于参数分析的arg_options和分析函数，参见info argp_parse
 */
class zArg:private zNoncopyable
{
	friend error_t zparse_opt(int, char *, struct argp_state *);
	protected:
		/**
		 * \brief 参数分析数据结构
		 */
		struct argp argp;
		/**
		 * \brief 用户定义的参数分析函数
		 */
		argsParser user_parser;
		/**
		 * \brief 参数分析器的唯一实例指针
		 */
		zArg();
		~zArg();
	private:
		static zArg * argInstance;
		void addOptions(const struct argp_option *options);
		/**
		 * \brief 所有参数选项指针
		 */
		struct argp_option *alloptions;

	public:
		static zArg *getArg();
		static void removeArg();

		bool add(const struct argp_option *options=0,argsParser func=0,const char *args_doc=0,const char *doc=0);
		bool parse(int argc ,char *argv[]);
};
#endif
