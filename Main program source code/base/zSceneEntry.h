/**
 * \file
 * \version  $Id: zSceneEntry.h  $
 * \author  
 * \date 
 * \brief 场景上物件定义
 */

#ifndef _ZSCENEENTRY_H_
#define _ZSCENEENTRY_H_
#include <vector>
#include <list>
#include <ext/pool_allocator.h>
#include "zEntry.h"
#include "Command.h"
#include "zMisc.h"

#pragma pack(1)
/**
 * \brief 用于偏移计算的坐标值
 */
struct zAdjust
{
	int x;		/**< 横坐标*/
	int y;		/**< 纵坐标*/
};
/**
 * \brief 场景坐标
 */
struct zPos
{
	DWORD x;		/**< 横坐标*/
	DWORD y;		/**< 纵坐标*/
	/**
	 * \brief 构造函数
	 *
	 */
	zPos()
	{
		x = 0;
		y = 0;
	}
	/**
	 * \brief 构造函数
	 *
	 */
	zPos(const DWORD x, const DWORD y)
	{
		this->x = x;
		this->y = y;
	}
	/**
	 * \brief 拷贝构造函数
	 *
	 */
	zPos(const zPos &pos)
	{
		x = pos.x;
		y = pos.y;
	}
	/**
	 * \brief 赋值操作符号
	 *
	 */
	zPos & operator= (const zPos &pos)
	{
		x = pos.x;
		y = pos.y;
		return *this;
	}
	/**
	 * \brief 重载+运算符号
	 *
	 */
	const zPos & operator+ (const zPos &pos)
	{
		x += pos.x;
		y += pos.y;
		return *this;
	}
	/**
	 * \brief 重载+运算符号
	 * 对坐标进行修正
	 */
	const zPos & operator+ (const zAdjust &adjust)
	{
		x += adjust.x;
		y += adjust.y;
		return *this;
	}
	/**
	 * \brief 重载+=运算符号
	 *
	 */
	const zPos & operator+= (const zPos &pos)
	{
		x += pos.x;
		y += pos.y;
		return *this;
	}
	/**
	 * \brief 重载+=运算符号
	 * 对坐标进行修正
	 */
	const zPos & operator+= (const zAdjust &adjust)
	{
		x += adjust.x;
		y += adjust.y;
		return *this;
	}
	/**
	 * \brief 重载-运算符号
	 *
	 */
	const zPos & operator- (const zPos &pos)
	{
		x -= pos.x;
		y -= pos.y;
		return *this;
	}
	/**
	 * \brief 重载-运算符号
	 * 对坐标进行修正
	 */
	const zPos & operator- (const zAdjust &adjust)
	{
		x -= adjust.x;
		y -= adjust.y;
		return *this;
	}
	/**
	 * \brief 重载-=运算符号
	 *
	 */
	const zPos & operator-= (const zPos &pos)
	{
		x -= pos.x;
		y -= pos.y;
		return *this;
	}
	/**
	 * \brief 重载-=运算符号
	 * 对坐标进行修正
	 */
	const zPos & operator-= (const zAdjust &adjust)
	{
		x -= adjust.x;
		y -= adjust.y;
		return *this;
	}
	/**
	 * \brief 重载==逻辑运算符号
	 *
	 */
	const bool operator== (const zPos &pos) const
	{
		return (x == pos.x && y == pos.y);
	}
	/**
	 * \brief 重载>逻辑运算符号
	 *
	 */
	const bool operator> (const zPos &pos) const
	{
		return (x > pos.x && y > pos.y);
	}
	/**
	 * \brief 重载>=逻辑运算符号
	 *
	 */
	const bool operator>= (const zPos &pos) const
	{
		return (x >= pos.x && y >= pos.y);
	}
	/**
	 * \brief 重载<逻辑运算符号
	 *
	 */
	const bool operator< (const zPos &pos) const
	{
		return (x < pos.x && y < pos.y);
	}
	/**
	 * \brief 重载<=逻辑运算符号
	 *
	 */
	const bool operator<= (const zPos &pos) const
	{
		return (x <= pos.x && y <= pos.y);
	}
	/**
	 * \brief 以自身为中心点，获取到另外一个坐标的方向
	 * \param pos 另外一个坐标点
	 * \return 方向
	 */
	const int getDirect(const zPos &pos) const
	{
		using namespace Cmd;
		if (x == pos.x && y > pos.y)
		{
			return _DIR_UP;
		}
		else if (x < pos.x && y > pos.y)
		{
			return _DIR_UPRIGHT;
		}
		else if (x < pos.x && y == pos.y)
		{
			return _DIR_RIGHT;
		}
		else if (x < pos.x && y < pos.y)
		{
			return _DIR_RIGHTDOWN;
		}
		else if (x == pos.x && y < pos.y)
		{
			return _DIR_DOWN;
		}
		else if (x > pos.x && y < pos.y)
		{
			return _DIR_DOWNLEFT;
		}
		else if (x > pos.x && y == pos.y)
		{
			return _DIR_LEFT;
		}
		else if (x > pos.x && y > pos.y)
		{
			return _DIR_LEFTUP;
		}

		return _DIR_WRONG;
	}
	///////////////////////////////////////////////////////
    /**
     * \brief 以自身为中心点，得到半径之内矩形中的一个随机点
     */
    zPos getRandPosByRadius(DWORD radius)
    {
        zPos randPos;
        int x = this->x - radius;
        if ( x < 0 ) x = 0;
        int y = this->y - radius;
        if ( y < 0 ) y = 0;

        randPos.x = zMisc::randBetween(x, this->x + radius );
        randPos.y = zMisc::randBetween(y, this->y + radius );
        return randPos;
    }
	///////////////////////////////////////////////////////
};
/**
 * \brief 半屏坐标
 *
 */
const zPos zPosHalfScreen(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
#pragma pack()

/**
 * \brief 场景屏坐标
 */
typedef DWORD zPosI;

/**
 * \brief 存放屏编号的向量
 *
 */
//#ifdef _POOL_ALLOC_
typedef std::vector<zPosI, __gnu_cxx::__pool_alloc<zPosI> > zPosIVector;
typedef std::vector<zPos, __gnu_cxx::__pool_alloc<zPos> > zPosVector;
//#else
//typedef std::vector<zPosI> zPosIVector;
//typedef std::vector<zPos> zPosVector;
//#endif

typedef std::list<WORD, __gnu_cxx::__pool_alloc<WORD> > SceneEntryStateList;

class zSceneEntryIndex;
/**
 * \brief 场景上物件，比如人物,NPC,建筑，地上物品等
 *
 * 作用有两个
 *
 * 1.建立屏索引
 * 2.定义阻挡
 */
struct zSceneEntry:public zEntry
{
	friend class zSceneEntryIndex;
	public:
	/**
	 * \brief 物件类型
	 */
	enum SceneEntryType
	{
		SceneEntry_Player,	/**< 玩家角色*/
		SceneEntry_NPC,		/**< NPC*/
		SceneEntry_Build,	/**< 建筑*/
		SceneEntry_Object,	/**< 地上物品*/
		SceneEntry_Pet,		/**< 宠物*/
		SceneEntry_MAX
	};
	/**
	 * \brief 物件状态
	 *
	 */
	enum SceneEntryState
	{
		SceneEntry_Normal,		/**< 普通状态 */
		SceneEntry_Death,		/**< 死亡状态 */
		SceneEntry_Hide			/**< 隐藏状态 */
	};
	/**
	 * \brief 坐标转化
	 * \param screenWH 地图宽和高
	 * \param pos 源坐标
	 * \param posi 目的坐标
	 */
	static void zPos2zPosI(const zPos &screenWH, const zPos &pos, zPosI &posi)
	{
		posi=((screenWH.x+SCREEN_WIDTH-1)/SCREEN_WIDTH) * (pos.y/SCREEN_HEIGHT) + (pos.x/SCREEN_WIDTH);
	}
	protected:
		/**
	 	 * \brief 构造函数
	 	 */
		zSceneEntry(SceneEntryType type, const SceneEntryState state = SceneEntry_Normal):sceneentrytype(type),sceneentrystate(state)
		{
			bzero(byState, sizeof(byState));
			dir = Cmd::_DIR_DOWN;
			inserted=false;
		}

		/**
	 	 * \brief 坐标
	 	 */
		zPos pos;
		/**
	 	 * \brief 屏坐标
	 	 */
		zPosI posi;
		/**
		 * \brief 方向
		 *
		 */
		BYTE dir;

		zPos lastPos1;
		zPos lastPos2;

	private:
		/**
	 	 * \brief 物件类型
	 	 */
		const SceneEntryType sceneentrytype;
		/**
		 * \brief 物件状态
		 *
		 */
		SceneEntryState sceneentrystate;
		/**
	 	 * \brief 物件是否在场景上
	 	 */
		bool inserted;

		/**
	 	 * \brief 设置物件坐标
		 * \param screenWH 场景的宽高
		 * \param newPos 物件的新坐标
		 * \return 坐标超出场景宽高返回false,否则返回true
	 	 */
		bool setPos(const zPos &screenWH,const zPos &newPos)
		{
			if(screenWH.x>newPos.x && screenWH.y>newPos.y)
			{
				pos=newPos;
				zPos2zPosI(screenWH, newPos, posi);
				return true;
			}
			else
				return false;
		}

	private:
		/**
		 * \brief 物件状态，与魔法等相关的
		 * 这种状态是外观可以表现的，带上某种状态客户端就可以以一种方式来表现
		 * 详细的状态参见Command.h中
		 */
		BYTE byState[(Cmd::MAX_STATE + 7) / 8];
	protected:
		SceneEntryStateList stateList;
	public:
		/**
	 	 * \brief 填充物件状态
		 * \param state 填充位置
		 * \return 状态个数
	 	 */
		inline BYTE full_UState(WORD *state)
		{
			BYTE ret = stateList.size();
			SceneEntryStateList::iterator iter = stateList.begin();
			for(int i=0 ; i < ret ; i ++)
			{
				state[i] = *iter;
				iter ++;
			}
			return ret;
		}
		/**
	 	 * \brief 填充物件所有状态
		 * \param state 填充位置
	 	 */
		inline void full_all_UState(void *state)
		{
			bcopy(byState,state,sizeof(byState));
		}

		/**
	 	 * \brief 得到物件坐标
		 * \return 物件坐标
	 	 */
		inline const zPos &getPos() const
		{
			return pos;
		}

		/**
	 	 * \brief 得到物件刚才的坐标
		 * \return 物件坐标
	 	 */
		inline const zPos &getOldPos1() const
		{
			return lastPos1;
		}

		/**
	 	 * \brief 得到物件刚才的坐标
		 * \return 物件坐标
	 	 */
		inline const zPos &getOldPos2() const
		{
			return lastPos2;
		}

		/**
	 	 * \brief 得到物件屏坐标
		 * \return 物件屏坐标
	 	 */
		inline const zPosI &getPosI() const
		{ 
			return posi;
		}
		/**
	 	 * \brief 测试物件是否在场景中
		 * \return 物件在场景中返回true,否则返回false
	 	 */
		inline bool hasInScene() const
		{ 
			return inserted;
		}

		/**
	 	 * \brief 得到物件类型
		 * \return 物件类型
	 	 */
		inline const SceneEntryType & getType() const
		{
			return sceneentrytype;
		}

		/**
		 * \brief 获取物件状态
		 * \return 状态
		 */
		inline const SceneEntryState & getState() const
		{
			return sceneentrystate;
		}

		/**
		 * \brief 设置物件状态
		 * \param state 需要设置的状态
		 */
		void setState(const SceneEntryState & state)
		{
			sceneentrystate = state;
		}

		/**
		 * \brief 获取方向
		 * \return 方向
		 */
		inline const BYTE getDir() const
		{
			return dir % 8;
		}

		/**
		 * \brief 设置方向
		 * \param dir 方向
		 */
		void setDir(const BYTE dir)
		{
			this->dir = dir % 8;
		}

		/**
		 * \brief 检查某种状态是否设置
		 * \param state 待检查的状态
		 * \return 这种状态是否已经设置
		 */
		inline bool issetUState(const int state) const
		{
			return Cmd::isset_state(byState, state);
		}

		/**
		 * \brief 设置某种状态
		 * \param state 待设置的状态
		 * \return 如果已经设置该状态返回false,否则返回true
		 */
		inline bool setUState(const int state)
		{
			if(!issetUState(state))
			{
				stateList.push_back(state);
				Cmd::set_state(byState, state);
				return true;
			}
			return false;
		}

		/**
		 * \brief 清除某种状态
		 * \param state 待清除的状态
		 * \return 如果已经设置该状态返回true,否则返回false
		 */
		inline bool clearUState(const int state)
		{
			Cmd::clear_state(byState, state);
			SceneEntryStateList::iterator iter = stateList.begin();
			for( ; iter != stateList.end() ; ++iter)
			{
				if(*iter == state)
				{
					stateList.erase(iter);
					return true;
				}
			}
			return false;
		}
};
#endif
