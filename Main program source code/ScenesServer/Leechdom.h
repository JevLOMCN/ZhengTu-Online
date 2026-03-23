/**
 * \file
 * \version  $Id: Leechdom.h $
 * \author  
 * \date 
 * \brief 药品,食物类物品使用
 *
 * 恢复法术值,体力值,生命值的物品
 * 
 */

#ifndef _LEECHDOM_H_
#define _LEECHDOM_H_
#include <vector>
#include "zType.h"
#include "zMutex.h"

#ifndef _LeechdomType_
#define _LeechdomType_
enum LeechdomType
{
	Leechdom_default,
	Leechdom_dam,//增加伤害力
	Leechdom_def,//增加防御力
	Leechdom_poison,//药物使人持续中毒
	Leechdom_sppersist,//物品使人体力值保持当前值不变
	Leechdom_spup,//药物增加人体力值百分比
	Leechdom_spcostdown,//减慢体力消耗速度
	Leechdom_spresumeup,//加快体力恢复速度
	Leechdom_hp,//一次性恢复生命
	Leechdom_hppersist,//持续恢复生命
	Leechdom_mp,//一次性恢复法术值
	Leechdom_mppersist,//持续恢复法术
	Leechdom_sp,//一次性恢复体力值
	Leechdom_hp5,//一次性恢复生命
	Leechdom_hppersist5,//持续恢复生命
	Leechdom_hpmax,//消耗耐久一次加满HP
	Leechdom_mpmax,//消耗耐久一次加满MP
	Leechdom_chocolate,//巧克力
};
#endif

struct SceneUser;
struct LeechdomElement
{
	LeechdomElement()
	{
		type = Leechdom_default;
		value = 0;
		times = 0;
	}
	LeechdomType type;
	WORD value;
	WORD times;
};
/**
 * \brief 药品使用
 *
 */
struct Leechdom
{
	Leechdom()
	{
		damcooling = 0;
		defcooling = 0;
		hpcooling = 0;
		spcooling = 0;
		mpcooling = 0;
		sppersist = 0;
	}
	void add(LeechdomType type , WORD value ,WORD times);
	void clear();
	DWORD fresh(SceneUser *pUser , DWORD &update);
	bool isCooling(DWORD type);
	void checkCooling();
	private:
	typedef std::vector<LeechdomElement> Leechdom_vector;
	typedef Leechdom_vector::iterator Leechdom_iterator;
	Leechdom_vector element;
	zMutex mlock;
	/// 伤害类物品冷却时间
	WORD damcooling;
	/// 防御类物品冷却时间
	WORD defcooling;
	/// 恢复hp类物品冷却时间
	WORD hpcooling;
	/// 恢复sp类物品冷却时间
	WORD spcooling;
	/// 恢复mp类物品冷却时间
	WORD mpcooling;
	WORD sppersist;
};

#endif
