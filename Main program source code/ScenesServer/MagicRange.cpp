/**
 * \file
 * \version  $Id: MagicRange.cpp  $
 * \author  
 * \date 
 * \brief 群攻技能范围定义
 *
 * 
 */

#include <vector>
#include <ext/hash_map>
#include "zType.h"
#include "zRWLock.h"
#include "zXMLParser.h"
#include "Zebra.h"
#include "MagicRange.h"

/**
 * \brief 根据相对坐标得到绝对坐标
 *
 *
 * \param center: 相对坐标中心点
 * \param dir:相对坐标方向
 * \return 绝对坐标
 */
zPos RelativePos::getAbsolutePos(const zPos &center , DWORD dir)
{
	zPos pos;

	switch(dir)
	{
		case 0:
		case 1:
			{
				pos.x = x;
				pos.y = y;
			}
			break;
		case 2:
		case 3:
			{
				pos.x = -y;
				pos.y = x;
			}
			break;
		case 4:
		case 5:
			{
				pos.x = -x;
				pos.y = -y;
			}
			break;
		case 6:
		case 7:
			{
				pos.x = y;
				pos.y = -x;
			}
			break;
		default:
			{
				pos.x = x;
				pos.y = y;
			}
			break;
	}
	pos.x += center.x;
	pos.y += center.y;
	return pos;
}

/**
 * \brief 唯一实例
 *
 */
MagicRangeInit *MagicRangeInit::instance = NULL;

/**
 * \brief 初始化技能范围
 *
 *
 * \return 初始化是否成功
 */
bool MagicRangeInit::init()
{
	final();

	rwlock.wrlock();
	zXMLParser xml;
	if (!xml.initFile(Zebra::global["magicrangefile"]))
	{
		Zebra::logger->error("加载魔法攻击范围文件 %s 失败", Zebra::global["magicrangefile"].c_str());
		return false;
	}

	xmlNodePtr root = xml.getRootNode("MagicRange");
	if (root)
	{
		xmlNodePtr node = xml.getChildNode(root, "range");
		while(node)
		{
			if (strcmp((char *)node->name, "range") == 0)
			{
				DWORD type;
				DWORD dir;
				DWORD num;
				SMagicRange range_vector;
				range_vector.num = 0;
				xml.getNodePropNum(node, "type", &type, sizeof(type));
				xml.getNodePropNum(node, "dir", &dir, sizeof(dir));
				xml.getNodePropNum(node, "num", &num, sizeof(num));
				range_vector.num = num;
				xmlNodePtr subnode = xml.getChildNode(node , "pos");
				while(subnode)
				{
					if(strcmp((char*)subnode->name , "pos") == 0)
					{
						RelativePos pos;
						xml.getNodePropNum(subnode , "x" , &pos.x , sizeof(pos.x));
						xml.getNodePropNum(subnode , "y" , &pos.y , sizeof(pos.y));
						xml.getNodePropNum(subnode , "w" , &pos.w , sizeof(pos.w));
						range_vector.lib.push_back(pos);
					}
					subnode = xml.getNextNode(subnode, NULL);
				}
				ranges.insert(MagicRangeContainer::value_type(magicrange_hash(type , dir) , range_vector));
			}

			node = xml.getNextNode(node, NULL);
		}
		Zebra::logger->info("初始化攻击范围成功");
		rwlock.unlock();
		return true;
	}
	rwlock.unlock();

	Zebra::logger->error("加载角攻击范围配置文件 %s 失败", Zebra::global["MagicRange"].c_str());
	return false;
}

/**
 * \brief 更具群攻类型和方向得到攻击坐标点
 *
 *
 * \param type: 群攻类型
 * \param dir: 方向
 * \param range:范围坐标(输出)
 */
void MagicRangeInit::get(const DWORD type , const DWORD dir , SMagicRange &range)
{
	rwlock.rdlock();
	MagicRangeContainer::const_iterator it = ranges.find(magicrange_hash(type , dir));
	if(it == ranges.end())
	{
		it = ranges.begin();
	}
	range = it->second;
	rwlock.unlock();
}

/**
 * \brief 卸载
 *
 */
void MagicRangeInit::final()
{
	rwlock.wrlock();
	ranges.clear();
	rwlock.unlock();
}

/**
 * \brief 构造函数,更具群攻类型和方向得到攻击范围
 *
 *
 * \param type: 群攻类型
 * \param dir: 方向
 */
MagicPos::MagicPos(const DWORD type , const DWORD dir)
{
	MagicRangeInit::getInstance().get(type , dir % 2 , range);
}
/**
 * \brief 执行每一个群攻作用点
 *
 *
 * \param pos: 回调函数
 */
void MagicPos::execEvery(MagicPosExec &pos)
{
	for(MagicRange::iterator iter = range.lib.begin(); iter != range.lib.end() ; iter ++)
	{
		pos.exec(*iter);
	}
}
