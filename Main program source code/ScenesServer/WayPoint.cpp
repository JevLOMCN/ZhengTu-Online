#include "WayPoint.h"
#include "zMisc.h"
#include "Zebra.h"

/**
 * \brief 构造函数，清空自己
 */
WayPoint::WayPoint()
{
	bzero(this,sizeof(WayPoint));
}

/**
 * \brief 初始化加载xml跳转点配置文件
 * \param parser 分析器
 * \param node xml节点
 * \param countryid 国家id
 * \return true 加载成功 false 加载失败
 */
bool WayPoint::init(zXMLParser *parser,const xmlNodePtr node , DWORD countryid)
{
	if(parser==NULL || node==NULL) return false;
	xmlNodePtr cnode=parser->getChildNode(node,NULL);
	while(cnode!=NULL)
	{
		bool ret=false;
		if(xmlStrcmp(cnode->name, (const xmlChar *)"point")==0)
		{
			zPos pos;
			if(parser->getNodePropNum(cnode,"x",&pos.x,sizeof(pos.x)) &&
					parser->getNodePropNum(cnode,"y",&pos.y,sizeof(pos.y)))
			{
				ret=true;
				point.push_back(pos);
				pointC++;
			}
		}
		else if(xmlStrcmp(cnode->name, (const xmlChar *)"dest")==0)
		{
			Point p;
			char tempName[MAX_NAMESIZE];
			if(parser->getNodePropNum(cnode,"x",&p.pos.x,sizeof(p.pos.x)) &&
					parser->getNodePropNum(cnode,"y",&p.pos.y,sizeof(p.pos.y)) &&
					parser->getNodePropStr(cnode,"name",tempName,MAX_NAMESIZE))
			{
				std::string fileName; 
				char temp[20];
				bzero(temp , sizeof(temp));
				DWORD country=0;

				/// 为了支持公共国家的概念^_^
				parser->getNodePropNum(cnode,"country",&country,sizeof(country));
				if(country)
				{
					sprintf(temp , "%d" , country);
				}
				else
				{
					sprintf(temp , "%d" , countryid);
				}
				fileName = temp;
				fileName+= ".";
				fileName+= tempName;
				strncpy(p.name , fileName.c_str() , MAX_NAMESIZE);
				ret=true;
				dest.push_back(p);
				destC++;
			}
		}
		if(ret)
			cnode=parser->getNextNode(cnode,NULL);
		else
		{
			//return false;
			Zebra::logger->warn("xml跳转点配置有错误数据");
			cnode=parser->getNextNode(cnode,NULL);
		}
	}
	return true;
}

/**
 * \brief 随便找一个跳转点
 * \return 跳转点
 */
const zPos *WayPoint::getRandPoint() const
{
	return &point[zMisc::randBetween(0,pointC-1)];
}

/**
 * \brief 获得一个随机的目的点
 * \return 目的点
 */
const Point *WayPoint::getRandDest() const
{
	return &dest[zMisc::randBetween(0,destC-1)];
}

/**
 * \brief 增加一个跳转点
 * \param wp 跳转点
 * \return true 
 */
bool WayPointM::addWayPoint(const WayPoint &wp)
{
	wps.push_back(wp);
	return true;
}

/**
 * \brief 随机获得跳转点
 * \return 如果没有跳转点则返回NULL, 否则返回跳转点。
 */
const WayPoint *WayPointM::getRandWayPoint() const
{
	if(!wps.empty())
		return &wps[zMisc::randBetween(0,wps.size()-1)];
	else
		return NULL;
}

/**
 * \brief 根据目的地名称获取跳转点
 * \param filename 目的地名称
 * \return 跳转点对象
 */
const WayPoint *WayPointM::getWayPoint(const char *filename) const
{
	for(int i=0, n=wps.size();i<n;i++)
	{
		for(int j=0;j<wps[i].destC;j++)
			if(strncmp(wps[i].dest[j].name,filename,MAX_NAMESIZE)==0)
			{
				return &wps[i];
			}
	}
	return NULL;
}

/**
 * \brief 获得指定位置上的跳转点对象
 * \param pos 位置
 * \return 跳转点对象
 */
const WayPoint *WayPointM::getWayPoint(const zPos &pos) const
{
	for(int i=0, n=wps.size();i<n;i++)
	{
		for(int j=0;j<wps[i].pointC;j++)
			if(wps[i].point[j]==pos)
			{
				return &wps[i];
			}
	}
	return NULL;
}
