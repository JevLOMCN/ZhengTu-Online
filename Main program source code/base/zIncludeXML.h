/**
 * \file
 * \version  $Id: zIncludeXML.h  $
 * \author  
 * \date 
 * \brief include模式XML扩展声明 
 */

#ifndef _ZINCLUDEXML_H_
#define _ZINCLUDEXML_H_

#include <string>
#include "zXMLParser.h"
#include "zNoncopyable.h"

/**
 * \brief zIncludeXML是用来扩展自定义XML的工具
 *
 * include节点格式为<include filename="xxx.xml" /> xxx.xml是你要包括的XML文件路径.
 *
 * include节点不能包含子节点，因为当扩展后将删除此节点.
 *
 * 此类非线程安全
 */
class zIncludeXML:private zNoncopyable
{
	private:
		/**
		 * \brief 构造函数 
		 */
		zIncludeXML(){};
		/**
		 * \brief 析构函数 
		 */
		~zIncludeXML(){};
		/**
		 * \brief 要扩展XML的解析器 
		 */
		static zXMLParser parser; 
		/**
		 * \brief 要包含XML文件的解析器 
		 */
		static zXMLParser childparser; 
		static bool expand(zXMLParser *parser,xmlNodePtr parent);
	public:
		static std::string &expand(const std::string &xmlFileName,std::string &xmlStr);
		static std::string &expand(std::string &xmlStr);
};
#endif
