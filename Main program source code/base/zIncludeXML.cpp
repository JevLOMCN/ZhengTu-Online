/**
 * \file
 * \version  $Id: zIncludeXML.cpp  $
 * \author  
 * \date 2
 * \brief IncludeXML实现
 */

#include "zType.h"
#include "zIncludeXML.h"
#include <string.h>

zXMLParser zIncludeXML::parser;
zXMLParser zIncludeXML::childparser;

/**
 * \brief 扩展XML文件 
 *
 * \param xmlFileName 要扩展的xml文件
 * \param xmlStr 扩展后的XML内容
 * \return 返回xmlStr
 */
std::string &zIncludeXML::expand(const std::string &xmlFileName,std::string &xmlStr)
{
	if(parser.initFile(xmlFileName))
	{
		xmlNodePtr root=parser.getRootNode(NULL);
		if(expand(&parser,root))
			parser.dump(xmlStr);
	}
	return xmlStr;
}

/**
 * \brief 扩展XML内容 
 *
 * \param xmlStr 要XML内容，扩展后的内容也放在此处
 * \return 返回xmlStr
 */
std::string &zIncludeXML::expand(std::string &xmlStr)
{
	char *xmlstr=new char[xmlStr.size()+1];
	if (xmlstr)
	{
		strncpy(xmlstr,xmlStr.c_str(),xmlStr.size());
		if(parser.initStr(xmlstr))
		{
			xmlNodePtr root=parser.getRootNode(NULL);
			if(expand(&parser,root))
				parser.dump(xmlStr);
		}
		SAFE_DELETE_VEC(xmlstr);
	}
	return xmlStr;
}

/**
 * \brief 扩展
 *
 * \param parser 要扩展的XML解析器
 * \param parent 要扩展的节点
 * \return 成功返回true ,否则返回false
 */
bool zIncludeXML::expand(zXMLParser *parser,xmlNodePtr parent)
{
	if(parser==NULL || parent == NULL) return false;
	xmlNodePtr child=parser->getChildNode(parent,NULL);
	while(child!=NULL)
	{
		xmlNodePtr nextNode=parser->getNextNode(child,NULL);
		if(parser->getChildNodeNum(child,NULL)>0)
			expand(parser,child);
		else if(strcmp((char *)child->name,"include")==0)
		{
			char buf[1024];
			if(parser->getNodePropStr(child,"filename",buf,1024))
			{
				if(childparser.initFile(buf))
				{
					xmlNodePtr childroot=childparser.getRootNode(NULL);
					xmlAddNextSibling(child,childroot);
				}
				childparser.final();
			}
			xmlUnlinkNode(child);
			xmlFreeNode(child);
		}
		child=nextNode;
	}
	return true;
}
