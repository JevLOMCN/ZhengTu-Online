/**
 * \file
 * \version  $Id: zRegex.h  $
 * \author  
 * \date 
 * \brief 正则表达式类声明
 */

#ifndef _ZREGEX_H_
#define _ZREGEX_H_
#include <regex.h>
#include <string>

/**
 * \brief 正则表达式类，对regex进行了封装，对于正则表达式请参考man 7 regex.
 *
 * 本类支持子字符串匹配，但最多支持31个字串
 *
 * 本类非线程安全
 */
class zRegex
{
	private:
		/**
		 * \brief 错误信息存放处
		 */
		std::string errstr;
		/**
		 * \brief 错误代码
		 */
		int errcode;
		/**
		 * \brief 正则表达式句柄
		 */
		regex_t preg;
		/**
		 * \brief 要匹配的字符串 
		 */
		std::string smatch;
		/**
		 * \brief 表达式是否已编译 
		 */
		bool compiled;
		/**
		 * \brief 是否匹配 
		 */
		bool matched;
		/**
		 * \brief 子串匹配位置 
		 */
		regmatch_t rgm[32];

		/**
		 * \brief 自定义错误代码:标记错误 
		 */
		static const int REG_FLAGS;
		/**
		 * \brief 自定义错误代码:未编译错误
		 */
		static const int REG_COMP;
		/**
		 * \brief 自定义错误代码:未知错误
		 */
		static const int REG_UNKNOW;
		/**
		 * \brief 自定义错误代码:未进行匹配错误 
		 */
		static const int REG_MATCH;
	public:
		/**
		 * \brief 自定义标记:支持多行匹配，默认不支持
		 */
		static const int REG_MULTILINE;
		/**
		 * \brief 自定义标记:默认标记
		 */
		static const int REG_DEFAULT;
		zRegex();
		~zRegex();
		bool compile(const char * regex,int flags=REG_DEFAULT);
		bool match(const char *s);
		std::string &getSub(std::string &s,int sub=0);
		const std::string & getError();
};
#endif
