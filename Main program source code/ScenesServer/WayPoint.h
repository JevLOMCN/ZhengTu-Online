#ifndef _WAYPOINT_H_
#define _WAYPOINT_H_
#include "zSceneEntry.h"
#include "zXMLParser.h"

/**
 * \brief 点结构
 */
struct Point
{
	/// 名称
	char name[MAX_NAMESIZE];

	/// 位置
	zPos pos;

	/**
	 * \brief 构造函数初始化
	 */
	Point() : pos()
	{
		bzero(name, sizeof(name));
	}

	/**
	 * \brief 拷贝构造
	 * \param point 点
	 */
	Point(const Point &point)
	{
		bcopy(point.name, name, sizeof(name));
		pos = point.pos;
	}

	/**
	 * \brief 重载操作符，拷贝点
	 * \param point 被拷贝的点
	 */
	Point & operator= (const Point &point)
	{
		bcopy(point.name, name, sizeof(name));
		pos = point.pos;
		return *this;
	}
};

class WayPointM;

/**
 * \brief 传点定义
 */
struct WayPoint
{
	friend class WayPointM;
	private:

		/// 目的坐标点集合
		std::vector<Point> dest;

		/// 目的坐标点数目
		WORD destC;
	public:
		/// 传送坐标点集合
		std::vector<zPos> point;
		/// 传送坐标点数目
		WORD pointC;

        /**
        * \brief 重载操作符，拷贝传点
        * \param from 被拷贝的对象
        */
        WayPoint & operator= (const WayPoint &from)
        {
            dest = from.dest;
            destC = from.destC;
            point = from.point;
            pointC = from.pointC;
            return *this;
        }

        bool init(zXMLParser *parser,const xmlNodePtr node , DWORD countryid);
        WayPoint();
        const Point *getRandDest() const;
        const zPos *getRandPoint() const;
};

class WayPointM
{
	private:
		/// 传点集合
		std::vector<WayPoint> wps;
	public:
        /**
        * \brief 重载操作符，拷贝传点
        * \param from 被拷贝的对象
        */
        WayPointM & operator= (const WayPointM &from)
        {
            wps = from.wps;
            return *this;
        }

		bool addWayPoint(const WayPoint &wp);
		const WayPoint *getWayPoint(const zPos &pos) const;
		const WayPoint *getWayPoint(const char *filename) const;
		const WayPoint *getRandWayPoint() const;
};

#endif
