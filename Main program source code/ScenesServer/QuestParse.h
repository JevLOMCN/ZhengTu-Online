/**
 * \file
 * \version	$Id: QuestParse.h  $
 * \author	
 * \date		2005-04-07
 * \brief		脚本关键词列表
 * 
 */
 
#ifndef __QUEST_PARSE_H__
#define __QUEST_PARSE_H__

#include "zXMLParser.h"

#include <string>
#include <sstream>


/**
 * \brief 关键词列表类
 *
 * 存储了关键词的列表,并提供了对应的操作.
 *
 */
class Parse
{
public:

	/**     
	 * \brief 取得xml节点内容
	 *
	 * 遍历关键词列表,取得他们对应的值
	 *      
	 * \param xml: 目标xml文件
	 * \param node: 目标节点
	 * \return 当前总是返回true
	 */     	
	bool parse (zXMLParser& xml, xmlNodePtr& node)
	{		
		for(iterator it=_kvs.begin(); it!=_kvs.end(); ++it) {
			if (!xml.getNodePropStr(node, it->first.c_str(), it->second)) {	
				_kvs[it->first] = "0";
				//return false;
			}
		}

		return true;
	}
	

	/**     
	 * \brief 设置一个关键词
	 *
	 * 在关键词列表中增加一个关键词,对应的值为"0"
	 *      
	 * \param key: 关键词名称
	 * \return 无
	 */     	
	void key(const std::string& key) 
	{
		_kvs[key] = "0";
	}

	/**     
	 * \brief 取值
	 *
	 * 在关键词列表中搜寻对应的关键词,并返回对应的值,没找到返回零值
	 *      
	 * \param key: 关键词名称
	 * \param value: 取得的值
	 * \return 无
	 */  
	template <typename T>
	void value(const std::string& key, T& value)
	{
		std::stringstream os(_kvs[key]);	
		os >> value;
	}
	
	
private:
	typedef std::map<std::string, std::string> KV;
	typedef KV::iterator iterator;
	KV _kvs;	
};

#endif
