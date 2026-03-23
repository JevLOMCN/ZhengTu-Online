/**
 * \file
 * \version  $Id: ScreenIndex.h $
 * \author  
 * \date 
 * \brief 屏索引
 *
 * 
 */
#ifndef _SCREENINDEX_H_
#define _SCREENINDEX_H_
#include "GateUser.h"
#include <set>
#include <ext/hash_map>
#include <vector>
#include "zEntryManager.h"
#include "zRWLock.h"
#include "zSceneEntry.h"


/**
 * \brief 网关屏索引
 *
 */
class ScreenIndex :private zNoncopyable
{
	private:
		//读写锁
		zRWLock wrlock;

		typedef std::set<GateUser *, std::less<GateUser *>, __gnu_cxx::__pool_alloc<GateUser *> > SceneEntry_SET;
		typedef __gnu_cxx::hash_map<DWORD, SceneEntry_SET> PosIMapIndex;
		/**
		 * \brief map索引容器
		 */
		PosIMapIndex index;
		SceneEntry_SET all;

		///横向多少屏幕
		const DWORD screenx;
		///纵向多少屏幕
		const DWORD screeny;
		const DWORD screenMax;
		//在加载的时候计算九屏关系并保存
		typedef __gnu_cxx::hash_map<DWORD, zPosIVector> NineScreen_map;
		typedef NineScreen_map::iterator NineScreen_map_iter;
		typedef NineScreen_map::value_type NineScreen_map_value_type;
		NineScreen_map ninescreen;
		NineScreen_map direct_screen[8];
		NineScreen_map reversedirect_screen[8];
		inline const zPosIVector &getNineScreen(const zPosI &posi);
		inline const zPosIVector &getDirectScreen(const zPosI &posi, const int direct);
		inline const zPosIVector &getReverseDirectScreen(const zPosI &posi, const int direct);
	public:
		/**
		 ** \brief 构造函数
		 **/
		ScreenIndex(const DWORD x , const DWORD y);
		template <class YourNpcEntry>
		void execAllOfScreen(const DWORD screen,execEntry<YourNpcEntry> &callback);
		void sendCmdToNine(const DWORD posi, const void *pstrCmd, const int nCmdLen);
		void sendCmdToDirect(const zPosI posi, const int direct, const void *pstrCmd, const int nCmdLen);
		void sendCmdToReverseDirect(const zPosI posi, const int direct, const void *pstrCmd, const int nCmdLen);
		void sendCmdToAll(const void *pstrCmd, const int nCmdLen);
		void sendCmdToNineExceptMe(const DWORD posi, const DWORD exceptme_id, const void *pstrCmd, const int nCmdLen);
		bool refresh(GateUser *e,const DWORD newScreen);
		void removeGateUser(GateUser *e);
};
/**
 * \brief 网关地图索引
 *
 */
typedef std::map<DWORD , ScreenIndex*> MapIndex;
typedef MapIndex::iterator MapIndexIter;
#endif


