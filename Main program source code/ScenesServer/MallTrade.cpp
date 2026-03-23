/**
 *  * \file
 *   * \version  $Id: MallTrade.cpp  $
 *    * \author 
 *     * \date 
 *      * \brief 商城随身商店
 *       *
 *        * 
 *         */
//yikey
#include <ext/hash_map>

#include "zType.h"
#include "zRWLock.h"
#include "zXMLParser.h"
#include "MallTrade.h" //新商城
#include "Zebra.h"

MallTrade *MallTrade::instance = NULL;

bool MallTrade::init()
{
	final();

	zXMLParser mallxml;

	std::string cfgxml("Config/Mall.xml");

	if(!mallxml.initFile(cfgxml.c_str())){
		Zebra::logger->error("加载商城交易文件 %S 失败", Zebra::global["npctradefile"].c_str());
		return false;
	}

	xmlNodePtr root = mallxml.getRootNode("wares");

	if(root){
		rwlock.wrlock();

		xmlNodePtr node = mallxml.getChildNode(root, NULL);
		
		while(node){
			MallItem items;

			if(strcmp((char *)node->name, "item") == 0){

				mallxml.getNodePropNum(node, "id", &items.id, sizeof(items.id));
				mallxml.getNodePropNum(node, "kind", &items.kind, sizeof(items.kind));
				mallxml.getNodePropNum(node, "action", &items.action, sizeof(items.action));

				mallitems.insert(MallItemMultimap::value_type(items.id,items));
			}
			node = mallxml.getNextNode(node,NULL);
		}
	}
	rwlock.unlock();
	Zebra::logger->info("初始化商城系统成功");
	
	return true;
}

bool MallTrade::verifyMallTrade(const MallItem &mallitem)
{
	rwlock.rdlock();
	for(MallItemMultimap::const_iterator Items = mallitems.begin(); Items != mallitems.end(); Items++)
	{
		if(Items->second.id == mallitem.id){
			if(Items->second.action == mallitem.action)
				return true;
		}
	}
	rwlock.unlock();

	return false;
}

void MallTrade::final()
{
	rwlock.wrlock();
	mallitems.clear();
	rwlock.unlock();
}
