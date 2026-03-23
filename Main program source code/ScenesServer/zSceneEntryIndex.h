/**
 * \file
 * \version  $Id: zSceneEntryIndex.h  $
 * \author  
 * \date 
 * \brief 地图物件屏索引声明
 */

#ifndef _ZSCENEENTRYINDEX_H_
#define _ZSCENEENTRYINDEX_H_
#include <set>
#include <map>
#include <ext/hash_map>
#include <ext/pool_allocator.h>
#include "zSceneEntry.h"

#define MAX_NPC_GROUP 5
class SceneUser;
class SceneNpc;
/**
 * \brief 场景物件的回调类
 *
 * 只要是用于遍历一屏物件,而对某些物件操作
 *
 * 使用者必须要实现exec方法
 */
struct zSceneEntryCallBack
{
	virtual bool exec(zSceneEntry *entry)=0;
	virtual ~zSceneEntryCallBack(){};
};

/**
 * \brief 场景物件屏索引
 */
class zSceneEntryIndex : private zNoncopyable
{

	protected:

		///场景宽和高
		zPos sceneWH;
		///最大屏索引编号
		DWORD screenMax;
		///横向多少屏幕
		DWORD screenx;
		///纵向多少屏幕
		DWORD screeny;

	private:

#ifdef _POOL_ALLOC_
		typedef std::set<zSceneEntry *, std::less<zSceneEntry *>, __gnu_cxx::__pool_alloc<zSceneEntry *> > SceneEntry_SET;
		typedef std::set<SceneNpc *, std::less<SceneNpc *>, __gnu_cxx::__pool_alloc<SceneNpc *> > Npc_Index;
#else
		typedef std::set<zSceneEntry *, std::less<zSceneEntry *> > SceneEntry_SET;
		typedef std::set<SceneNpc *, std::less<SceneNpc *> > Npc_Index;
#endif


		typedef __gnu_cxx::hash_map<zPosI, SceneEntry_SET> PosIMapIndex;
		typedef __gnu_cxx::hash_map<DWORD, Npc_Index> SpecialNpc_Index;

		/**
		 * \brief map索引容器
		 */
		PosIMapIndex index[zSceneEntry::SceneEntry_MAX];
		/// 所有物件的索引
		SceneEntry_SET all[zSceneEntry::SceneEntry_MAX];
		/// 对于特殊npc的索引
		SpecialNpc_Index special_index;
		/// 对于功能npc的索引
		Npc_Index functionNpc;

#ifdef _POOL_ALLOC_
		typedef std::map<DWORD, int, std::less<DWORD>, __gnu_cxx::__pool_alloc<std::pair<const DWORD, int> > > PosiEffectMap;
#else
		typedef std::map<DWORD, int, std::less<DWORD> > PosiEffectMap;
#endif

		typedef PosiEffectMap::iterator PosiEffectMap_iter;
		PosiEffectMap posiEffect[MAX_NPC_GROUP];

		//在加载的时候计算九屏关系并保存
		typedef __gnu_cxx::hash_map<DWORD, zPosIVector> NineScreen_map;
		typedef NineScreen_map::iterator NineScreen_map_iter;
		typedef NineScreen_map::const_iterator NineScreen_map_const_iter;
		typedef NineScreen_map::value_type NineScreen_map_value_type;
		NineScreen_map ninescreen;
		NineScreen_map direct_screen[8];
		NineScreen_map reversedirect_screen[8];

		void freshEffectPosi(const zPosI oldposi, const zPosI newposi);

	public:

		/**
		 * \brief 构造函数
		 */
		zSceneEntryIndex() {}
		/**
		 * \brief 析构函数
		 */
		virtual ~zSceneEntryIndex() {}

		virtual void freshGateScreenIndex(SceneUser *pUser , const DWORD screen) = 0;

		bool refresh(zSceneEntry *e,const zPos & newPos);
		bool removeSceneEntry(zSceneEntry *e);
		void execAllOfScreen(const zPosI screen,zSceneEntryCallBack &callback);
		void execAllOfScreen(const zSceneEntry::SceneEntryType type,const zPosI screen,zSceneEntryCallBack &callback);
		void execAllOfScene(zSceneEntryCallBack &callback);
		void execAllOfScene(const zSceneEntry::SceneEntryType type,zSceneEntryCallBack &callback);
		void execAllOfScene_npc(const DWORD id, zSceneEntryCallBack &callback);
		void execAllOfScene_functionNpc(zSceneEntryCallBack &callback);

		void setSceneWH(const zPos sceneWH, const DWORD screenx, const DWORD screeny, const DWORD screenMax);
		zSceneEntry *getSceneEntryByPos(zSceneEntry::SceneEntryType type, const zPos &pos, const bool bState = true, const zSceneEntry::SceneEntryState byState = zSceneEntry::SceneEntry_Normal);
		void execAllOfEffectNpcScreen(const DWORD group, zSceneEntryCallBack &callback);

		/**
		 * \brief 获取地图宽
		 * 单位是格点
		 * \return 地图宽
		 */
		const DWORD width() const { return sceneWH.x; }
		/**
		 * \brief 获取地图高
		 * 单位是格点
		 * \return 地图高
		 */
		const DWORD height() const { return sceneWH.y; }
		/**
		 * \brief 获取地图宽
		 * 单位是屏
		 * \return 地图宽
		 */
		const DWORD getScreenX() const { return screenx; }
		/**
		 * \brief 获取地图高
		 * 单位是屏
		 * \return 地图高
		 */
		const DWORD getScreenY() const { return screeny; }
		/**
		 * \brief 坐标转化
		 * 绝对坐标转化为屏编号相对坐标
		 * \param p 绝对坐标系统 
		 * \param pi 相对坐标系统
		 * \return 返回转化后的相对坐标系统
		 */
		zPosI &zPos2zPosI(const zPos &p, zPosI &pi) const
		{
			zSceneEntry::zPos2zPosI(sceneWH,p,pi);
			return pi;
		}
		/**
		 * \brief 重新计算坐标，保证不越界
		 * \param pos 待验证的坐标
		 */
		const void zPosRevaluate(zPos &pos) const
		{
			if (pos.x & 0x80000000)
				pos.x = 0;
			if (pos.y & 0x80000000)
				pos.y = 0;
			if (pos.x >= sceneWH.x)
				pos.x = sceneWH.x - 1;
			if (pos.y >= sceneWH.y)
				pos.y = sceneWH.y - 1;
		}
		/**
		 * \brief 验证坐标是否合法，是否越界
		 * \param pos 坐标
		 * \return 是否合法
		 */
		const bool zPosValidate(const zPos &pos) const { return pos.x < sceneWH.x && pos.y < sceneWH.y; }
		/**
		 * \brief 验证坐标是否合法，是否越界
		 * \param posi 坐标
		 * \return 是否合法
		 */
		const bool zPosIValidate(const zPosI &posi) const { return posi < screenMax; }
		/**
		 * \brief 检查两个坐标编号是否有9屏关系
		 * \param one 第一个编号
		 * \param two 第二个编号
		 * \return 是否有9屏关系
		 */
		bool checkTwoPosIInNine(const zPosI one , const zPosI two) const
		{
			int oneX, oneY , twoX ,twoY;
			oneX = one % screenx;
			oneY = one / screenx;
			twoX = two % screenx;
			twoY = two / screenx;
			if(abs(oneX - twoX) <=1 && abs(oneY - twoY) <=1)
			{
				return true;
			}
			return false;
		}
		/**
		 * \brief 判断两点之间距离是否超出了半径
		 * \param pos1 起点
		 * \param pos2 终点
		 * \param radius 半径
		 * \return 是否超出范围
		 */
		const bool zPosShortRange(const zPos &pos1, const zPos &pos2, const int radius) const
		{
			if (abs(pos1.x - pos2.x) <= radius
					&& abs(pos1.y - pos2.y) <= radius)
				return true;
			else
				return false;
		}
		/**
		 * \brief 判断两点之间距离是否超出了矩形范围
		 * \param pos1 起点
		 * \param pos2 终点
		 * \param wide 矩形宽
		 * \param height 矩形高
		 * \return 是否超出范围
		 */
		const bool zPosShortRange(const zPos &pos1, const zPos &pos2, const int wide, const int height) const
		{
			if (abs(pos1.x - pos2.x) <= wide
					&& abs(pos1.y - pos2.y) <= height)
				return true;
			else
				return false;
		}
		/*
		 * \brief 得到两点的距离
		 * 这个距离只是作为比较判断，不是作为实际的距离
		 * \param pos1 位置1
		 * \param pos2 位置2
		 * \return 距离
		 */
		unsigned int getDistance(zPos pos1, zPos pos2) const { return abs(pos1.x-pos2.x)+abs(pos1.y-pos2.y); }
		/**
		 * \brief 方向取反
		 * \param direct 方向
		 * \return 相反的方向
		 */
		int getReverseDirect(const int direct) const { return (direct + 4) % 8; }
		/**
		 * \brief 根据两屏判断九个方向
		 * \param posiorg 原点屏编号
		 * \param posinew 结束点屏编号
		 * \return 方向编号，如果为-1表示不是九个方向之一
		 */
		int getScreenDirect(const zPosI posiorg,const zPosI posinew) const
		{
			using namespace Cmd;
			unsigned int orgX = posiorg % screenx;
			unsigned int orgY = posiorg / screenx;
			unsigned int newX = posinew % screenx;
			unsigned int newY = posinew / screenx;
			if (orgX == newX && orgY == newY+1)
			{
				return _DIR_UP;
			}
			else if (orgX + 1 == newX && orgY == newY +1)
			{
				return _DIR_UPRIGHT;
			}
			else if (orgX +1 == newX && orgY == newY)
			{
				return _DIR_RIGHT;
			}
			else if (orgX +1 == newX && orgY +1 == newY)
			{
				return _DIR_RIGHTDOWN;
			}
			else if (orgX == newX && orgY +1 == newY)
			{
				return _DIR_DOWN;
			}
			else if (orgX == 1 + newX && orgY +1 == newY)
			{
				return _DIR_DOWNLEFT;
			}
			else if (orgX == 1 + newX && orgY == newY)
			{
				return _DIR_LEFT;
			}
			else if (orgX == 1 + newX && orgY == 1 + newY)
			{
				return _DIR_LEFTUP;
			}
			else
				return _DIR_WRONG;
		}

		/**
		 * \brief 查找我在另一个坐标点的大致方向
		 * \param myPos 我的位置
		 * \param otherPos 对方的位置
		 * \return 我在otherPos的相对方向
		 */
		static int getCompDir(const zPos &myPos, const zPos &otherPos)
		{
			int x = otherPos.x - myPos.x;
			int y = otherPos.y - myPos.y;
			int absx = abs(x);
			int absy = abs(y);
			if (absx > absy && absy < absx/2)
				y = 0;
			else if (absx < absy && absx < absy/2)
				x = 0;

			int bkDir = 0;

			if (x < 0 &&  y < 0)
				bkDir = 7;
			else if (x < 0 && 0 == y)
				bkDir = 6;
			else if (x < 0 && y > 0)
				bkDir = 5;
			else if (0 == x && y > 0)
				bkDir = 4;
			else if (x > 0 && y > 0)
				bkDir = 3;
			else if (x > 0 && 0 == y)
				bkDir = 2;
			else if (x > 0 && y < 0)
				bkDir = 1;
			else if (0 == x && y < 0)
				bkDir = 0;
			return bkDir;
		}
		const zPosIVector &getScreenByRange(const zPos &pos, const int range);
		/**
		 * \brief 以中心屏获取周围9屏的屏幕编号
		 * \param posi 中心屏
		 * \param pv 输出的屏编号向量
		 */
		const zPosIVector &getNineScreen(const zPosI &posi)
		{
			NineScreen_map_const_iter iter = ninescreen.find((DWORD)posi);
			if( iter != ninescreen.end())
			{
				return iter->second;
			}
			//如果出现异常返回0编号的屏索引
			return ninescreen[(DWORD)-1];
		}

		/**
		 * \brief 以中心屏获取向前的3屏或者5屏的屏幕编号
		 * \param posi 中心屏
		 * \param direct 方向
		 * \return 输出的屏编号向量
		 */
		const zPosIVector &getDirectScreen(const zPosI &posi, const int dir)
		{
			NineScreen_map_const_iter iter = direct_screen[dir].find((DWORD)posi);
			if( iter != direct_screen[dir].end())
			{
				return iter->second;
			}
			//如果出现异常返回0编号的屏索引
			return direct_screen[dir][(DWORD)-1];
		}

		/**
		 * \brief 以中心屏获取向后的3屏或者5屏的屏幕编号
		 * \param posi 中心屏
		 * \param direct 方向
		 * \return 输出的屏编号向量
		 */
		const zPosIVector &getReverseDirectScreen(const zPosI &posi, const int dir)
		{
			NineScreen_map_const_iter iter = reversedirect_screen[dir].find((DWORD)posi);
			if( iter != reversedirect_screen[dir].end())
			{
				return iter->second;
			}
			//如果出现异常返回0编号的屏索引
			return reversedirect_screen[dir][(DWORD)-1];
		}

};

#endif

