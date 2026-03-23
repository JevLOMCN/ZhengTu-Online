/**
 *  * \file
 *   * \version  $Id: fjconfig.cpp  $
 *    * \author 
 *     * \date 
 *      * \brief 商城随身商店
 *       *
 *        * 
 *         */
//yikey
#include <vector>
#include "zType.h"
#include "zXMLParser.h"
#include "fjconfig.h"
#include "Zebra.h"
fjconfig *fjconfig::instance = NULL;

bool fjconfig::init()
{

	fjlist.clear();

    zXMLParser xml;
	if (!xml.initFile(Zebra::global["shenJianConfig"]))
	{
		Zebra::logger->error("加载神剑配置文件 %s 失败", Zebra::global["shenJianConfig"].c_str());
		return false;
	}

	xmlNodePtr config = xml.getRootNode("config");

	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{
						
						SJ sj;
						xml.getNodePropNum(point, "level", &sj.level, sizeof(sj.level));
						xml.getNodePropNum(point, "value", &sj.value, sizeof(sj.value));
						xml.getNodePropNum(point, "costID", &sj.costID, sizeof(sj.costID));
						xml.getNodePropNum(point, "costNum", &sj.costNum, sizeof(sj.costNum));
						xml.getNodePropNum(point, "salary", &sj.salary, sizeof(sj.salary));
						xml.getNodePropNum(point, "pDam", &sj.pDam, sizeof(sj.pDam));
						xml.getNodePropNum(point, "mDam", &sj.mDam, sizeof(sj.mDam));
						xml.getNodePropNum(point, "pDef", &sj.pDef, sizeof(sj.pDef));
						xml.getNodePropNum(point, "mDef", &sj.mDef, sizeof(sj.mDef));
						xml.getNodePropNum(point, "hp", &sj.hp, sizeof(sj.hp));
						fjlist.push_back(sj);
						point = xml.getNextNode(point,NULL);

					}
				
				}
			}

		}
	}

	Zebra::logger->info("初始化神剑配置成功");
	return true;

	
}

bool fjconfig::initztz() //你打眼一瞅 这个方法能看明白在干嘛吗  这是服务端读取xml的方法 和上午写的客户端是一样的你稍等我  几分钟  然后我打电话给你说
{

	ztzlist.clear();

    zXMLParser xml;
	if (!xml.initFile(Zebra::global["zhengtuzhuaconfig"]))
	{
		Zebra::logger->error("加载征途传配置文件 %s 失败", Zebra::global["zhengtuzhuaconfig"].c_str());
		return false;
	}

	xmlNodePtr config = xml.getRootNode("config");

	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{
						
						ZTZ sj;
						xml.getNodePropNum(point, "level", &sj.level, sizeof(sj.level));
						xml.getNodePropNum(point, "value", &sj.value, sizeof(sj.value));
						xml.getNodePropNum(point, "costID", &sj.costID, sizeof(sj.costID));
						xml.getNodePropNum(point, "costNum", &sj.costNum, sizeof(sj.costNum));
						xml.getNodePropNum(point, "salary", &sj.salary, sizeof(sj.salary));
						xml.getNodePropNum(point, "pDam", &sj.pDam, sizeof(sj.pDam));
						xml.getNodePropNum(point, "mDam", &sj.mDam, sizeof(sj.mDam));
						xml.getNodePropNum(point, "pDef", &sj.pDef, sizeof(sj.pDef));
						xml.getNodePropNum(point, "mDef", &sj.mDef, sizeof(sj.mDef));
						xml.getNodePropNum(point, "hp", &sj.hp, sizeof(sj.hp));
						ztzlist.push_back(sj);
						point = xml.getNextNode(point,NULL);

					}
				
				}
			}

		}
	}

	Zebra::logger->info("初始化征途传配置成功");
	return true;	
}

bool fjconfig::initshengxiao() 
{
	shengxiaolist.clear();
    zXMLParser xml;
	if (!xml.initFile(Zebra::global["12BeastsConfig"]))
	{
		Zebra::logger->error("加载十二生肖配置文件 %s 失败", Zebra::global["12BeastsConfig"].c_str());
		return false;
	}
	xmlNodePtr config = xml.getRootNode("config");
	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						SHENGXIAO sj;
						xml.getNodePropNum(point, "level", &sj.level, sizeof(sj.level));
						xml.getNodePropNum(point, "value", &sj.value, sizeof(sj.value));
						xml.getNodePropNum(point, "costID", &sj.costID, sizeof(sj.costID));
						xml.getNodePropNum(point, "costNum", &sj.costNum, sizeof(sj.costNum));
						xml.getNodePropNum(point, "salary", &sj.salary, sizeof(sj.salary));
						xml.getNodePropNum(point, "pDam", &sj.pDam, sizeof(sj.pDam));
						xml.getNodePropNum(point, "mDam", &sj.mDam, sizeof(sj.mDam));
						xml.getNodePropNum(point, "pDef", &sj.pDef, sizeof(sj.pDef));
						xml.getNodePropNum(point, "mDef", &sj.mDef, sizeof(sj.mDef));
						xml.getNodePropNum(point, "hp", &sj.hp, sizeof(sj.hp));
						shengxiaolist.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}
		}
	}
	Zebra::logger->info("初始化十二生肖配置成功");
	return true;	
}

bool fjconfig::initshengqi() 
{
	shengqilist.clear();
    zXMLParser xml;
	if (!xml.initFile(Zebra::global["ShengQiConfig"]))
	{
		Zebra::logger->error("加载圣器配置文件 %s 失败", Zebra::global["ShengQiConfig"].c_str());
		return false;
	}
	xmlNodePtr config = xml.getRootNode("config");
	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						SHENGQI sj;
						xml.getNodePropNum(point, "level", &sj.level, sizeof(sj.level));
						xml.getNodePropNum(point, "value", &sj.value, sizeof(sj.value));
						xml.getNodePropNum(point, "costID", &sj.costID, sizeof(sj.costID));
						xml.getNodePropNum(point, "costNum", &sj.costNum, sizeof(sj.costNum));
						xml.getNodePropNum(point, "salary", &sj.salary, sizeof(sj.salary));
						xml.getNodePropNum(point, "pDam", &sj.pDam, sizeof(sj.pDam));
						xml.getNodePropNum(point, "mDam", &sj.mDam, sizeof(sj.mDam));
						xml.getNodePropNum(point, "pDef", &sj.pDef, sizeof(sj.pDef));
						xml.getNodePropNum(point, "mDef", &sj.mDef, sizeof(sj.mDef));
						xml.getNodePropNum(point, "hp", &sj.hp, sizeof(sj.hp));
						shengqilist.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}
		}
	}
	Zebra::logger->info("初始化圣器配置成功");
	return true;	
}

bool fjconfig::initjingmai() 
{
	jingmailist.clear();
    zXMLParser xml;
	if (!xml.initFile(Zebra::global["JingMaiConfig"]))
	{
		Zebra::logger->error("加载经脉配置文件 %s 失败", Zebra::global["JingMaiConfig"].c_str());
		return false;
	}
	xmlNodePtr config = xml.getRootNode("config");
	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						JINGMAI sj;
						xml.getNodePropNum(point, "level", &sj.level, sizeof(sj.level));
						xml.getNodePropNum(point, "value", &sj.value, sizeof(sj.value));
						xml.getNodePropNum(point, "costID", &sj.costID, sizeof(sj.costID));
						xml.getNodePropNum(point, "costNum", &sj.costNum, sizeof(sj.costNum));
						xml.getNodePropNum(point, "salary", &sj.salary, sizeof(sj.salary));
						xml.getNodePropNum(point, "pDam", &sj.pDam, sizeof(sj.pDam));
						xml.getNodePropNum(point, "mDam", &sj.mDam, sizeof(sj.mDam));
						xml.getNodePropNum(point, "pDef", &sj.pDef, sizeof(sj.pDef));
						xml.getNodePropNum(point, "mDef", &sj.mDef, sizeof(sj.mDef));
						xml.getNodePropNum(point, "hp", &sj.hp, sizeof(sj.hp));
						jingmailist.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}
		}
	}
	Zebra::logger->info("初始化经脉配置成功");
	return true;	
}

bool fjconfig::initTouxian() 
{
	touxianlist.clear();
    zXMLParser xml;
	if (!xml.initFile(Zebra::global["TouxianConfig"]))
	{
		Zebra::logger->error("加载头衔配置文件 %s 失败", Zebra::global["TouxianConfig"].c_str());
		return false;
	}
	xmlNodePtr config = xml.getRootNode("config");
	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						TOUXIAN sj;
						xml.getNodePropNum(point, "level", &sj.level, sizeof(sj.level));
						xml.getNodePropNum(point, "exp", &sj.exp, sizeof(sj.exp));
						xml.getNodePropNum(point, "costID", &sj.costID, sizeof(sj.costID));
						xml.getNodePropNum(point, "salary", &sj.salary, sizeof(sj.salary));
						xml.getNodePropNum(point, "pDam", &sj.pDam, sizeof(sj.pDam));
						xml.getNodePropNum(point, "mDam", &sj.mDam, sizeof(sj.mDam));
						xml.getNodePropNum(point, "pDef", &sj.pDef, sizeof(sj.pDef));
						xml.getNodePropNum(point, "mDef", &sj.mDef, sizeof(sj.mDef));
						xml.getNodePropNum(point, "hp", &sj.hp, sizeof(sj.hp));
						touxianlist.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}
		}
	}
	Zebra::logger->info("初始化头衔配置成功");
	return true;	
}


bool fjconfig::initChenghao() 
{
	chenghaolist.clear();
    zXMLParser xml;
	if (!xml.initFile(Zebra::global["ChenghaoConfig"]))
	{
		Zebra::logger->error("加载称号配置文件 %s 失败", Zebra::global["ChenghaoConfig"].c_str());
		return false;
	}
	xmlNodePtr config = xml.getRootNode("config");
	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						CHENGHAO sj;
						xml.getNodePropNum(point, "id", &sj.id, sizeof(sj.id));
						xml.getNodePropNum(point, "stateID", &sj.stateID, sizeof(sj.stateID));
						xml.getNodePropNum(point, "pDam", &sj.pDam, sizeof(sj.pDam));
						xml.getNodePropNum(point, "mDam", &sj.mDam, sizeof(sj.mDam));
						xml.getNodePropNum(point, "pDef", &sj.pDef, sizeof(sj.pDef));
						xml.getNodePropNum(point, "mDef", &sj.mDef, sizeof(sj.mDef));
						xml.getNodePropNum(point, "hp", &sj.hp, sizeof(sj.hp));
						chenghaolist.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}
		}
	}
	Zebra::logger->info("初始化称号配置成功");
	return true;	
}


//儿女配置文件加载
bool fjconfig::initErnv() 
{
	ernvlist.clear();
    zXMLParser xml;
	if (!xml.initFile(Zebra::global["ErnvConfig"]))
	{
		Zebra::logger->error("加载生儿育女配置文件 %s 失败", Zebra::global["ErnvConfig"].c_str());
		return false;
	}
	xmlNodePtr config = xml.getRootNode("config");
	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						ERNV sj;
						xml.getNodePropNum(point, "level", &sj.level, sizeof(sj.level));
						xml.getNodePropNum(point, "exp", &sj.exp, sizeof(sj.exp));
						xml.getNodePropNum(point, "costID", &sj.costID, sizeof(sj.costID));
						xml.getNodePropNum(point, "salary", &sj.salary, sizeof(sj.salary));
						xml.getNodePropNum(point, "pDam", &sj.pDam, sizeof(sj.pDam));
						xml.getNodePropNum(point, "mDam", &sj.mDam, sizeof(sj.mDam));
						xml.getNodePropNum(point, "pDef", &sj.pDef, sizeof(sj.pDef));
						xml.getNodePropNum(point, "mDef", &sj.mDef, sizeof(sj.mDef));
						xml.getNodePropNum(point, "hp", &sj.hp, sizeof(sj.hp));
						xml.getNodePropNum(point, "ll", &sj.ll, sizeof(sj.ll));
						xml.getNodePropNum(point, "zl", &sj.zl, sizeof(sj.zl));
						xml.getNodePropNum(point, "js", &sj.js, sizeof(sj.js));
						xml.getNodePropNum(point, "mj", &sj.mj, sizeof(sj.mj));
						xml.getNodePropNum(point, "tz", &sj.tz, sizeof(sj.tz));
						xml.getNodePropNum(point, "skillpoint", &sj.skillpoint, sizeof(sj.skillpoint));
						ernvlist.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}
		}
	}
	Zebra::logger->info("初始化生儿育女配置成功");
	return true;	
}


bool fjconfig::initHuishou() 
{
	huishoulist.clear();
    zXMLParser xml;
	if (!xml.initFile(Zebra::global["HuishouConfig"]))
	{
		Zebra::logger->error("加载头衔配置文件 %s 失败", Zebra::global["HuishouConfig"].c_str());
		return false;
	}
	xmlNodePtr config = xml.getRootNode("config");
	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						HUISHOU sj;
						xml.getNodePropNum(point, "objectID", &sj.objectID, sizeof(sj.objectID));
						xml.getNodePropNum(point, "price", &sj.price, sizeof(sj.price));
						xml.getNodePropNum(point, "type", &sj.type, sizeof(sj.type));
						xml.getNodePropStr(point, "name", &sj.name, sizeof(sj.name));
						huishoulist.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}
		}
	}
	Zebra::logger->info("初始化回收系统配置成功");
	return true;	
}



bool fjconfig::initBieshu() 
{
	bieshulist.clear();
    zXMLParser xml;
	if (!xml.initFile(Zebra::global["BieshuConfig"]))
	{
		Zebra::logger->error("加载云天别墅配置文件 %s 失败", Zebra::global["BieshuConfig"].c_str());
		return false;
	}
	xmlNodePtr config = xml.getRootNode("config");
	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						BIESHU sj;
						xml.getNodePropNum(point, "level", &sj.level, sizeof(sj.level));
						xml.getNodePropNum(point, "exp", &sj.exp, sizeof(sj.exp));
						xml.getNodePropNum(point, "costID", &sj.costID, sizeof(sj.costID));
						xml.getNodePropNum(point, "salary", &sj.salary, sizeof(sj.salary));
						xml.getNodePropNum(point, "gong", &sj.gong, sizeof(sj.gong));
						xml.getNodePropNum(point, "fang", &sj.fang, sizeof(sj.fang));
						xml.getNodePropNum(point, "hp", &sj.hp, sizeof(sj.hp));
						bieshulist.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}
		}
	}
	Zebra::logger->info("初始化云天别墅配置成功");
	return true;	
}



bool fjconfig::initXiulian() 
{
	xiulianlist.clear();
    zXMLParser xml;
	if (!xml.initFile(Zebra::global["XiulianConfig"]))
	{
		Zebra::logger->error("加载能力修炼配置文件 %s 失败", Zebra::global["XiulianConfig"].c_str());
		return false;
	}
	xmlNodePtr config = xml.getRootNode("config");
	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						XIULIAN sj;
						xml.getNodePropNum(point, "level", &sj.level, sizeof(sj.level));
						xml.getNodePropNum(point, "exp", &sj.exp, sizeof(sj.exp));
						xml.getNodePropNum(point, "salary", &sj.salary, sizeof(sj.salary));
						xml.getNodePropNum(point, "pDam", &sj.pDam, sizeof(sj.pDam));
						xml.getNodePropNum(point, "mDam", &sj.mDam, sizeof(sj.mDam));
						xml.getNodePropNum(point, "pDef", &sj.pDef, sizeof(sj.pDef));
						xml.getNodePropNum(point, "mDef", &sj.mDef, sizeof(sj.mDef));
						xml.getNodePropNum(point, "hp", &sj.hp, sizeof(sj.hp));
						xiulianlist.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}
		}
	}
	Zebra::logger->info("初始化能力修炼配置成功");
	return true;	
}



bool fjconfig::initZuoqi() 
{
	zuoqilist.clear();
    zXMLParser xml;
	if (!xml.initFile(Zebra::global["ZuoQiTuJianConfig"]))
	{
		Zebra::logger->error("加载坐骑图鉴配置文件 %s 失败", Zebra::global["ZuoQiTuJianConfig"].c_str());
		return false;
	}
	xmlNodePtr config = xml.getRootNode("config");
	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						ZUOQI sj;
						xml.getNodePropNum(point, "level", &sj.level, sizeof(sj.level));
						xml.getNodePropNum(point, "exp", &sj.exp, sizeof(sj.exp));
						xml.getNodePropNum(point, "costID", &sj.costID, sizeof(sj.costID));
						xml.getNodePropNum(point, "salary", &sj.salary, sizeof(sj.salary));
						xml.getNodePropNum(point, "ll", &sj.ll, sizeof(sj.ll));
						xml.getNodePropNum(point, "zl", &sj.zl, sizeof(sj.zl));
						xml.getNodePropNum(point, "js", &sj.js, sizeof(sj.js));
						xml.getNodePropNum(point, "mj", &sj.mj, sizeof(sj.mj));
						xml.getNodePropNum(point, "tz", &sj.tz, sizeof(sj.tz));
						xml.getNodePropNum(point, "speed", &sj.speed, sizeof(sj.speed));
						xml.getNodePropNum(point, "wugong", &sj.wugong, sizeof(sj.wugong));
						xml.getNodePropNum(point, "mogong", &sj.mogong, sizeof(sj.mogong));
						xml.getNodePropNum(point, "wufang", &sj.wufang, sizeof(sj.wufang));
						xml.getNodePropNum(point, "mofang", &sj.mofang, sizeof(sj.mofang));
						xml.getNodePropNum(point, "pDam", &sj.pDam, sizeof(sj.pDam));
						xml.getNodePropNum(point, "mDam", &sj.mDam, sizeof(sj.mDam));
						xml.getNodePropNum(point, "pDef", &sj.pDef, sizeof(sj.pDef));
						xml.getNodePropNum(point, "mDef", &sj.mDef, sizeof(sj.mDef));
						xml.getNodePropNum(point, "hp", &sj.hp, sizeof(sj.hp));
						zuoqilist.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}
		}
	}
	Zebra::logger->info("初始化坐骑图鉴配置成功");
	return true;	
}



//时装魔盒
bool fjconfig::initMohe() 
{
	mohelist.clear();
    zXMLParser xml;
	if (!xml.initFile(Zebra::global["MagicBoxConfig"]))
	{
		Zebra::logger->error("加载魔盒配置文件 %s 失败", Zebra::global["MagicBoxConfig"].c_str());
		return false;
	}
	xmlNodePtr config = xml.getRootNode("config");
	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						MOHE sj;
						xml.getNodePropNum(point, "level", &sj.level, sizeof(sj.level));
						xml.getNodePropNum(point, "exp", &sj.exp, sizeof(sj.exp));
						xml.getNodePropNum(point, "pDam", &sj.pDam, sizeof(sj.pDam));
						xml.getNodePropNum(point, "mDam", &sj.mDam, sizeof(sj.mDam));
						xml.getNodePropNum(point, "pDef", &sj.pDef, sizeof(sj.pDef));
						xml.getNodePropNum(point, "mDef", &sj.mDef, sizeof(sj.mDef));
						xml.getNodePropNum(point, "hp", &sj.hp, sizeof(sj.hp));
						mohelist.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}
		}
	}
	Zebra::logger->info("初始化魔盒配置成功");
	return true;	
}


bool fjconfig::initShizhuang() 
{
	shizhuangnanlist.clear();
	shizhuangnvlist.clear();
    zXMLParser xml;
	if (!xml.initFile(Zebra::global["MagicBoxShiZhuangConfig"]))
	{
		Zebra::logger->error("初始化魔盒【时装类】配置文件 %s 失败", Zebra::global["MagicBoxShiZhuangConfig"].c_str());
		return false;
	}
	xmlNodePtr config = xml.getRootNode("config");
	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						SHIZHUANG sj;
						xml.getNodePropStr(point, "name", &sj.name, sizeof(sj.name));
						xml.getNodePropNum(point, "exp", &sj.exp, sizeof(sj.exp));
						xml.getNodePropNum(point, "costID", &sj.costID, sizeof(sj.costID));
						xml.getNodePropNum(point, "jihuoID", &sj.jihuoID, sizeof(sj.jihuoID));
						xml.getNodePropNum(point, "jihuoNum", &sj.jihuoNum, sizeof(sj.jihuoNum));
						xml.getNodePropNum(point, "bodyNum", &sj.bodyNum, sizeof(sj.bodyNum));
						shizhuangnanlist.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}

			//加载女
			meridian = xml.getNextNode(meridian,NULL);
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						SHIZHUANG sj;
						xml.getNodePropStr(point, "name", &sj.name, sizeof(sj.name));
						xml.getNodePropNum(point, "exp", &sj.exp, sizeof(sj.exp));
						xml.getNodePropNum(point, "costID", &sj.costID, sizeof(sj.costID));
						xml.getNodePropNum(point, "jihuoID", &sj.jihuoID, sizeof(sj.jihuoID));
						xml.getNodePropNum(point, "jihuoNum", &sj.jihuoNum, sizeof(sj.jihuoNum));
						xml.getNodePropNum(point, "bodyNum", &sj.bodyNum, sizeof(sj.bodyNum));
						shizhuangnvlist.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}
		}
	}
	Zebra::logger->info("初始化魔盒【时装类】配置文件成功");
	return true;	
}


bool fjconfig::initShiZhuangLevel() 
{
	shizhuanglevellist.clear();
	
    zXMLParser xml;
	if (!xml.initFile(Zebra::global["MagicBoxShiZhuangLevelConfig"]))
	{
		Zebra::logger->error("初始化魔盒【时装进阶类】配置文件 %s 失败", Zebra::global["MagicBoxShiZhuangLevelConfig"].c_str());
		return false;
	}
	xmlNodePtr config = xml.getRootNode("config");
	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						SHIZHUANGLEVEL sj;
						xml.getNodePropNum(point, "level", &sj.level, sizeof(sj.level));
						xml.getNodePropNum(point, "exp", &sj.exp, sizeof(sj.exp));
						xml.getNodePropNum(point, "costID", &sj.costID, sizeof(sj.costID));
						xml.getNodePropNum(point, "costNum", &sj.costNum, sizeof(sj.costNum));
						xml.getNodePropNum(point, "wugong", &sj.wugong, sizeof(sj.wugong));
						xml.getNodePropNum(point, "mogong", &sj.mogong, sizeof(sj.mogong));
						xml.getNodePropNum(point, "hp", &sj.hp, sizeof(sj.hp));
						xml.getNodePropNum(point, "shengming", &sj.shengming, sizeof(sj.shengming));
						xml.getNodePropNum(point, "fashu", &sj.fashu, sizeof(sj.fashu));
						xml.getNodePropNum(point, "bang", &sj.bang, sizeof(sj.bang));
						shizhuanglevellist.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}
		}
	}
	Zebra::logger->info("初始化魔盒【时装进阶类】配置文件成功");
	return true;	
}


//时装魔盒披风
bool fjconfig::initPifeng() 
{
	pifenglist.clear();
    zXMLParser xml;
	if (!xml.initFile(Zebra::global["MagicBoxPiFengConfig"]))
	{
		Zebra::logger->error("初始化魔盒【披风类】配置文件 %s 失败", Zebra::global["MagicBoxPiFengConfig"].c_str());
		return false;
	}
	xmlNodePtr config = xml.getRootNode("config");
	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						PIFENG sj;
						xml.getNodePropStr(point, "name", &sj.name, sizeof(sj.name));
						xml.getNodePropNum(point, "exp", &sj.exp, sizeof(sj.exp));
						xml.getNodePropNum(point, "costID", &sj.costID, sizeof(sj.costID));
						xml.getNodePropNum(point, "jihuoID", &sj.jihuoID, sizeof(sj.jihuoID));
						xml.getNodePropNum(point, "jihuoNum", &sj.jihuoNum, sizeof(sj.jihuoNum));
						xml.getNodePropNum(point, "itemNum", &sj.itemNum, sizeof(sj.itemNum));
						pifenglist.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}
		}
	}
	Zebra::logger->info("初始化魔盒【披风类】配置文件成功");
	return true;	
}



bool fjconfig::initPiFengLevel() 
{
	pifenglevellist.clear();
	
    zXMLParser xml;
	if (!xml.initFile(Zebra::global["MagicBoxPiFengLevelConfig"]))
	{
		Zebra::logger->error("初始化魔盒【披风进阶类】配置文件 %s 失败", Zebra::global["MagicBoxPiFengLevelConfig"].c_str());
		return false;
	}
	xmlNodePtr config = xml.getRootNode("config");
	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						PIFENGLEVEL sj;
						xml.getNodePropNum(point, "level", &sj.level, sizeof(sj.level));
						xml.getNodePropNum(point, "exp", &sj.exp, sizeof(sj.exp));
						xml.getNodePropNum(point, "costID", &sj.costID, sizeof(sj.costID));
						xml.getNodePropNum(point, "costNum", &sj.costNum, sizeof(sj.costNum));
						xml.getNodePropNum(point, "wugong", &sj.wugong, sizeof(sj.wugong));
						xml.getNodePropNum(point, "mogong", &sj.mogong, sizeof(sj.mogong));
						xml.getNodePropNum(point, "hp", &sj.hp, sizeof(sj.hp));
						xml.getNodePropNum(point, "shengming", &sj.shengming, sizeof(sj.shengming));
						xml.getNodePropNum(point, "fashu", &sj.fashu, sizeof(sj.fashu));
						xml.getNodePropNum(point, "bang", &sj.bang, sizeof(sj.bang));
						pifenglevellist.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}
		}
	}
	Zebra::logger->info("初始化魔盒【披风进阶类】配置文件成功");
	return true;	
}



//时装魔盒翅膀
bool fjconfig::initChibang() 
{
	chibanglist.clear();
    zXMLParser xml;
	if (!xml.initFile(Zebra::global["MagicBoxChiBangConfig"]))
	{
		Zebra::logger->error("初始化魔盒【翅膀类】配置文件 %s 失败", Zebra::global["MagicBoxChiBangConfig"].c_str());
		return false;
	}
	xmlNodePtr config = xml.getRootNode("config");
	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						CHIBANG sj;
						xml.getNodePropStr(point, "name", &sj.name, sizeof(sj.name));
						xml.getNodePropNum(point, "exp", &sj.exp, sizeof(sj.exp));
						xml.getNodePropNum(point, "costID", &sj.costID, sizeof(sj.costID));
						xml.getNodePropNum(point, "jihuoID", &sj.jihuoID, sizeof(sj.jihuoID));
						xml.getNodePropNum(point, "jihuoNum", &sj.jihuoNum, sizeof(sj.jihuoNum));
						xml.getNodePropNum(point, "itemNum", &sj.itemNum, sizeof(sj.itemNum));
						chibanglist.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}
		}
	}
	Zebra::logger->info("初始化魔盒【翅膀类】配置文件成功");
	return true;	
}



bool fjconfig::initChiBangLevel() 
{
	chibanglevellist.clear();
	
    zXMLParser xml;
	if (!xml.initFile(Zebra::global["MagicBoxChiBangLevelConfig"]))
	{
		Zebra::logger->error("初始化魔盒【翅膀进阶类】配置文件 %s 失败", Zebra::global["MagicBoxChiBangLevelConfig"].c_str());
		return false;
	}
	xmlNodePtr config = xml.getRootNode("config");
	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						CHIBANGLEVEL sj;
						xml.getNodePropNum(point, "level", &sj.level, sizeof(sj.level));
						xml.getNodePropNum(point, "exp", &sj.exp, sizeof(sj.exp));
						xml.getNodePropNum(point, "costID", &sj.costID, sizeof(sj.costID));
						xml.getNodePropNum(point, "costNum", &sj.costNum, sizeof(sj.costNum));
						xml.getNodePropNum(point, "wugong", &sj.wugong, sizeof(sj.wugong));
						xml.getNodePropNum(point, "mogong", &sj.mogong, sizeof(sj.mogong));
						xml.getNodePropNum(point, "hp", &sj.hp, sizeof(sj.hp));
						xml.getNodePropNum(point, "shengming", &sj.shengming, sizeof(sj.shengming));
						xml.getNodePropNum(point, "fashu", &sj.fashu, sizeof(sj.fashu));
						xml.getNodePropNum(point, "bang", &sj.bang, sizeof(sj.bang));
						chibanglevellist.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}
		}
	}
	Zebra::logger->info("初始化魔盒【翅膀进阶类】配置文件成功");
	return true;	
}



//时装魔盒坐骑
bool fjconfig::initZuoqi2() 
{
	zuoqi2list.clear();
    zXMLParser xml;
	if (!xml.initFile(Zebra::global["MagicBoxZuoQiConfig"]))
	{
		Zebra::logger->error("初始化魔盒【坐骑类】配置文件 %s 失败", Zebra::global["MagicBoxZuoQiConfig"].c_str());
		return false;
	}
	xmlNodePtr config = xml.getRootNode("config");
	if(config)
	{
		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
		if(meridianMap)
		{
			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
			if(meridian)
			{
				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
				if(pointMap)
				{
					xmlNodePtr point = xml.getChildNode(pointMap,"point");
					while (point)
					{						
						ZUOQI2 sj;
						xml.getNodePropStr(point, "name", &sj.name, sizeof(sj.name));
						xml.getNodePropNum(point, "exp", &sj.exp, sizeof(sj.exp));
						xml.getNodePropNum(point, "npcID", &sj.npcID, sizeof(sj.npcID));
						xml.getNodePropNum(point, "jihuoID", &sj.jihuoID, sizeof(sj.jihuoID));
						xml.getNodePropNum(point, "jihuoNum", &sj.jihuoNum, sizeof(sj.jihuoNum));
						xml.getNodePropNum(point, "npcNum", &sj.npcNum, sizeof(sj.npcNum));
						zuoqi2list.push_back(sj);
						point = xml.getNextNode(point,NULL);
					}				
				}
			}
		}
	}
	Zebra::logger->info("初始化魔盒【坐骑类】配置文件成功");
	return true;	
}



// bool fjconfig::initZuoQiLevel() 
// {
// 	zuoqilevellist.clear();
	
//     zXMLParser xml;
// 	if (!xml.initFile(Zebra::global["MagicBoxZuoQiLevelConfig"]))
// 	{
// 		Zebra::logger->error("初始化魔盒【坐骑进阶类】配置文件 %s 失败", Zebra::global["MagicBoxZuoQiLevelConfig"].c_str());
// 		return false;
// 	}
// 	xmlNodePtr config = xml.getRootNode("config");
// 	if(config)
// 	{
// 		xmlNodePtr meridianMap = xml.getChildNode(config,"meridianMap");
// 		if(meridianMap)
// 		{
// 			xmlNodePtr meridian = xml.getChildNode(meridianMap,"meridian");
// 			if(meridian)
// 			{
// 				xmlNodePtr pointMap = xml.getChildNode(meridian,"pointMap");
// 				if(pointMap)
// 				{
// 					xmlNodePtr point = xml.getChildNode(pointMap,"point");
// 					while (point)
// 					{						
// 						ZUOQILEVEL sj;
// 						xml.getNodePropNum(point, "level", &sj.level, sizeof(sj.level));
// 						xml.getNodePropNum(point, "exp", &sj.exp, sizeof(sj.exp));
// 						xml.getNodePropNum(point, "costID", &sj.costID, sizeof(sj.costID));
// 						xml.getNodePropNum(point, "costNum", &sj.costNum, sizeof(sj.costNum));
// 						xml.getNodePropNum(point, "wugong", &sj.wugong, sizeof(sj.wugong));
// 						xml.getNodePropNum(point, "mogong", &sj.mogong, sizeof(sj.mogong));
// 						xml.getNodePropNum(point, "hp", &sj.hp, sizeof(sj.hp));
// 						xml.getNodePropNum(point, "shengming", &sj.shengming, sizeof(sj.shengming));
// 						xml.getNodePropNum(point, "fashu", &sj.fashu, sizeof(sj.fashu));
// 						xml.getNodePropNum(point, "bang", &sj.bang, sizeof(sj.bang));
// 						zuoqilevellist.push_back(sj);
// 						point = xml.getNextNode(point,NULL);
// 					}				
// 				}
// 			}
// 		}
// 	}
// 	Zebra::logger->info("初始化魔盒【坐骑进阶类】配置文件成功");
// 	return true;	
// }
