/**
 * \file
 * \version  $Id: zConfile.h  $
 * \author  
 * \date 
 * \brief 配置文件解析器声明
 */

#ifndef _ZCONFILE_H_
#define _ZCONFILE_H_
#include <string>
#include "zXMLParser.h"
/**
 * \brief 配置文件解析器
 *
 * 此类必须继承使用。本类实现了全局参数的解析标记为\<global\>\</global\>
 * 并把解析的参数保存在一个全局的参数容器global中。
 *
 * 如果用户有自己的配置,用户应该实现自己的参数解析。
 *
 */
class zConfile
{
	protected:
		/**
		 * \brief xml解析器
		 */
		zXMLParser parser;
		/**
		 * \brief 配置文件名称
		 *
		 */
		std::string confile;

		bool globalParse(const xmlNodePtr node);
		bool parseNormal(const xmlNodePtr node);
		bool parseSuperServer(const xmlNodePtr node);
		virtual bool parseYour(const xmlNodePtr node)=0;

	public:
		zConfile(const char *confile="config.xml");
		virtual ~zConfile();
		bool parse(const char *name);
};
#endif
