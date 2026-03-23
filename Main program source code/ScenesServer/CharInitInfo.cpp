/**
 * \file
 * \version  $Id: CharInitInfo.cpp  $
 * \author  
 * \date 
 * \brief 初始角色信息
 *
 * 
 */

#include <vector>
#include <ext/hash_map>
#include "zType.h"
#include "zRWLock.h"
#include "zXMLParser.h"
#include "Zebra.h"
#include "CharInitInfo.h"

CharInitInfo *CharInitInfo::instance = NULL;

/**
 * \brief 初始化新手信息(读取配置文件charinitinfoFile.xml)
 *
 *
 * \return 初始化是否成功
 */
bool CharInitInfo::init()
{
	final();

	rwlock.wrlock();
	zXMLParser xml;
	if (!xml.initFile(Zebra::global["charinitinfoFile"]))
	{
		Zebra::logger->error("加载角色初始信息配置文件 %s 失败", Zebra::global["charinitinfoFile"].c_str());
		return false;
	}

	xmlNodePtr root = xml.getRootNode("CharInfo");
	if (root)
	{
		xmlNodePtr node = xml.getChildNode(root, "object");
		while(node)
		{
			if (strcmp((char *)node->name, "object") == 0)
			{
				InitObject object;
				xml.getNodePropNum(node, "id", &object.id, sizeof(object.id));
				xml.getNodePropStr(node, "name", object.name, sizeof(object.name));
				xml.getNodePropNum(node, "localeID", &object.localeID, sizeof(object.localeID));
				xml.getNodePropNum(node, "number", &object.num, sizeof(object.num));
				xml.getNodePropNum(node, "profession", &object.profession, sizeof(object.profession));
				//Zebra::logger->debug("%u, %s, %u, %u, %u", object.id, object.name, object.localeID, object.num, object.profession);
				objects.insert(ObjectsContainer::value_type(object.profession, object));
			}

			node = xml.getNextNode(node, NULL);
		}
		Zebra::logger->info("初始化初始物品系统成功");
		rwlock.unlock();
		return true;
	}
	rwlock.unlock();

	Zebra::logger->error("加载角色初始信息配置文件 %s 失败", Zebra::global["charinitinfoFile"].c_str());
	return false;
}

/**
 * \brief 根据职业得到新手物品
 *
 *
 * \param profession: 职业
 * \param objs: 该职业的新手物品(输出)
 */
void CharInitInfo::get(const WORD profession, InitObjectVector &objs)
{
	rwlock.rdlock();
	//__gnu_cxx::pair<ObjectsContainer::const_iterator, ObjectsContainer::const_iterator> os = objects.equal_range(profession);
	for(ObjectsContainer::const_iterator it = objects.begin();it!=objects.end();it++)
	{
		if(profession & it->second.profession)
			objs.push_back((*it).second);

	}
	/*
	for(ObjectsContainer::const_iterator it = os.first; it != os.second; it++)
	{
		objs.push_back((*it).second);
	}
	// */
	rwlock.unlock();

	//Zebra::logger->debug("%u, %u", profession, objs.size());
}

/**
 * \brief 清空新手物品列表
 *
 *
 */
void CharInitInfo::final()
{
	rwlock.wrlock();
	objects.clear();
	rwlock.unlock();
}

