#ifndef _SKILLBASE_H_
#define _SKILLBASE_H_

#include "zType.h"
#include "zDatabase.h"
#pragma pack(1)

typedef struct _Skill
{
	DWORD	skillid;					//技能ID
	DWORD	level;						//技能等级
//	DWORD	maxlevel;					//技能等级
//	DWORD	nextlevel;					//需要角色等级
//	DWORD	needtype;					// 需要职业
//	DWORD	five;						// 五行属性
//	DWORD	firstfive;					// 最初五行点数
	//DWORD	nextfive;					// 五行点数
}t_Skill;


#pragma pack()
namespace SkillDef
{
	enum ATTACK_TYPE
	{
		AttackNear = 0,
		AttackFly = 1,
		AttackDirect = 2
	};

	enum
	{
		AttackNine = 1,
		AttackEight = 2,
		AttackThree = 3,
		AttackFive_1 = 4,
		AttackFive_2 = 5,
		AttackFive_3 = 6,
		AttackCross = 7

	};
	enum 	SKILL_STATUS
	{
		SKILL_default, //0默认为错误类型
		SKILL_dvalue, //伤害值
		SKILL_reflect, //反弹
		SKILL_sdam, //技能攻击力
		SKILL_mdam,//法术攻击力
		SKILL_maxmp,//法术值上限
		SKILL_mpspeed,//法术值回复速度
		SKILL_mppersist,//法术值持续
		SKILL_mp,				//法术值
		SKILL_mpdef,		//法术防御
		SKILL_sp,				//体力值
		SKILL_maxsp,//体力值上限
		SKILL_sppersist,//体力值持续
		SKILL_spspeed,	//体力值回复速度
		SKILL_hp,				//生命值
		SKILL_maxhp,		//生命值上限
		SKILL_hppersist,//生命值持续
		SKILL_hpspeed,	//生命值回复速度
		SKILL_mvspeed,	//移动速度
		SKILL_pdam,			//物理攻击力
		SKILL_atrating,	//命中率
		SKILL_skillrating,//技能命中率
		SKILL_akdodge, //闪避率
		SKILL_cancel, //状态消除
		SKILL_aspeed,	//攻击速度
		SKILL_relive,//复活
		SKILL_hitback,//击退
		SKILL_change,//攻击目标变为施法者
		SKILL_topet,//变为随机小动物
		SKILL_dam2ddef,//攻防转换
		SKILL_poison, //中毒状态
		SKILL_petrify, //石化状态
		SKILL_blind, //失明状态
		SKILL_chaos, //混乱状态
		SKILL_cold, //冰冻状态
		SKILL_palsy,//麻痹状态
		SKILL_bleeding,//流血状态
		SKILL_sevenp,//降低陷入七大状态几率
		SKILL_coldp, //减少陷入冰冻状态几率
		SKILL_poisonp, //减少陷入中毒状态几率
		SKILL_petrifyp, //减少陷入石化状态几率
		SKILL_blindp, //减少陷入失明状态几率
		SKILL_chaosp, //减少陷入混乱状态几率
		SKILL_mgspeed, //减少技能施放间隔
		SKILL_cancelatt //不能攻击施法者
	};
#define TARGET_SELF 1
#define TARGET_FRIEND 2
#define TARGET_ENEMY 4
#define TARGET_NPC 8
#define TARGET_TERRASURFACE 16
#define TARGET_PET 32
#define TARGET_SUMMON 64

#define SKILL_CENTER_TYPE_SELF  2
#define SKILL_CENTER_TYPE_MOUSE 1

#define SKILLINVALID 0 
#define SKILLNORMAL 377
#define NORMALBOW 378
///////////////////////////////////////////
// modif by snip3r 技能加点上限改为10
//#define MAX_SKILLLEVEL 10
#define MAX_SKILLLEVEL 7
///////////////////////////////////////////

};

#endif
