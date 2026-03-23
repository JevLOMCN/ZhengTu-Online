/**
 * \file
 * \version  $Id: zConfile.cpp  $
 * \author 
 * \date 
 * \brief 配置文件解析器定义,
 */

#include <string>

#include "zConfile.h"
#include "Zebra.h"
#include "zBase64.h"

/**
 * \brief 构造函数
 * \param confile 配置文件名字
 */
zConfile::zConfile(const char *confile)
{
	this->confile = confile;
}

/**
 * \brief 析构函数
 */
zConfile::~zConfile()
{
	parser.final();
}

/**
 * \brief 全局解析函数
 * \param node 全局配置节点
 * \return 解析是否成功
 */
bool zConfile::globalParse(const xmlNodePtr node)
{
	xmlNodePtr child=parser.getChildNode(node,NULL);
	while(child)
	{
		if(strcasecmp((char *)child->name,"superserver")==0)
			parseSuperServer(child);
		else
			parseNormal(child);
		child=parser.getNextNode(child,NULL);
	}
	return true;
}

/**
 * \brief 普通参数解析,只是简单的把参数放入global容器中
 * \param node 要解析的节点
 * \return 解析是否成功
 */
bool zConfile::parseNormal(const xmlNodePtr node)
{
	char buf[128];
	if(parser.getNodeContentStr(node,buf,128))
	{
		Zebra::global[(char *)node->name]=buf;
		if (0 == strcmp((char *)node->name, "mysql")
				&& parser.getNodePropStr(node,"encode",buf,128)
				&& 0 == strcasecmp(buf, "yes"))
		{
			std::string tmpS;
			Zebra::base64_decrypt(Zebra::global[(char *)node->name], tmpS);
			Zebra::global[(char *)node->name]=tmpS;
		}
		return true;
	}
	else
		return false;
}

/**
 * \brief SuperServer参数解析，会在global容器中放入两个参数
 *
 * server SuperServer地址
 *
 * port SuperServer端口
 * \param node SuperServer参数节点
 * \return 解析是否成功
 */
bool zConfile::parseSuperServer(const xmlNodePtr node)
{
	char buf[64];
	if(parser.getNodeContentStr(node,buf,64))
	{
		Zebra::global["server"]=buf;
		if(parser.getNodePropStr(node,"port",buf,64))
			Zebra::global["port"]=buf;
		else
			Zebra::global["port"]="10000";
		return true;
	}
	else
		return false;
}

/**
 * \brief 开始解析配置文件
 *
 * \param name 使用者自己参数的定义节点名字
 * \return 解析是否成功
 */
bool zConfile::parse(const char *name)
{
	if (parser.initFile(confile))
	{
		xmlNodePtr root=parser.getRootNode("Zebra");
		if(root)
		{
			xmlNodePtr globalNode=parser.getChildNode(root,"global");
			if(globalNode)
			{
				if(!globalParse(globalNode))
					return false;
			}
			else
				Zebra::logger->warn("无全局配置段落.");
			xmlNodePtr otherNode=parser.getChildNode(root,name);
			if(otherNode)
			{
				if(!parseYour(otherNode))
					return false;
			}
			else
				Zebra::logger->warn("无 %s 配置段落.",name);
			return true;
		}
	}
	return false;
}
