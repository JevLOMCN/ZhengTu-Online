#ifndef _SCENEPK_H_
#define _SCENEPK_H_

#include "zTime.h"
#include "SkillBase.h"
#include "SceneSkill.h"
#include "SkillManager.h"
#include <vector>
#include <map>
#include <ext/hash_map>
struct SceneUser;
struct SceneNpc;
struct SceneEntryPk;

//技能影响的状态数据
struct SkillState
{
	SkillState();
	void init();
	union
	{
		struct 
		{
			SDWORD dvalue;		//伤害值增加固定数值1
			SDWORD dvaluep;		//伤害值增加x%2
			SDWORD pdefence;	//物理防御数值变更 57，86
			SDWORD pdefencep;	//物理防御变更百分比
			SDWORD updam;		//物理攻击增加固定数值
			SDWORD updamp;		//物理攻击增加百分比
			SDWORD umdam;		//法术攻击增加固定数值
			SDWORD umdamp;		//法术攻击增加百分比
			SDWORD dpdam;		//物理攻击减少固定数值
			SDWORD dpdamp;		//物理攻击减少百分比
			SDWORD dmdam;		//法术攻击减少固定数值
			SDWORD dmdamp;		//法术攻击减少百分比
			SDWORD updef;		//物理防御增加固定数值
			SDWORD updefp;		//物理防御增加百分比
			SDWORD umdef;		//法术防御增加固定数值
			SDWORD umdefp;		//法术防御增加百分比
			SDWORD dpdef;		//物理防御减少固定数值
			SDWORD dpdefp;		//物理防御减少百分比
			SDWORD dmdef;		//法术防御减少固定数值
			SDWORD dmdefp;		//法术防御减少百分比
			SDWORD topet;		//变为随机小动物79
			SDWORD appenddam;	//产生额外伤害83
			SDWORD movespeed;	//移动速度变更百分比 16，56
			SDWORD mgspeed;		//减少技能施放间隔17
			SDWORD coldp;		//减少陷入冰冻状态几率18
			SDWORD poisonp;		//减少陷入中毒状态几率19
			SDWORD petrifyp;		//减少陷入石化状态几率20
			SDWORD blindp;		//减少陷入失明状态几率21
			SDWORD chaosp;		//减少陷入混乱状态几率22
			SDWORD atrating;		//命中率增加33,64
			SDWORD reduce_atrating;  // 命中率减少 ZJW加入
			SDWORD hpspeedup;	//生命值恢复速度增加34
			SDWORD mpspeedup;	//法术值恢复速度增加35
			SDWORD spspeedup;	//体力值恢复速度增加36
			SDWORD akdodge;		//闪避率上升37
			SDWORD reduce_akdodge;  // 闪避率下降
			SDWORD reflect;		//反弹45
			SDWORD reflectp;		//反弹x%46
			SDWORD reflect2;		//为反弹百分之几的敌人伤害50
			SDWORD mdefence;		//法术防御变更59，             // 40
			SDWORD mdefencep;	//法术防御变更百分比x%
			SDWORD uattackspeed;	//攻击速度提高白分比
			SDWORD dattackspeed;	//攻击速度降低白分比
			SDWORD sevendownp;	//降低陷入七大状态几率82
			SDWORD tsfdamp;		//伤害转移百分比
			SDWORD tsfdam;		//伤害转移数值
			SDWORD passdam;		//被动：额外伤害117
			SDWORD maxhp;       //生命值最大值变更
			SDWORD maxmp;       //法术值最大值变更
			SDWORD maxsp;       //体力值最大值变更
			SDWORD pupdam;		//物理攻击增加固定数值（被动）
			SDWORD pumdam;		//法术攻击增加固定数值（被动）
			SDWORD pupdef;		//物理防御增加固定数值（被动）
			SDWORD pumdef;		//法术防御增加固定数值（被动）
			SDWORD supdam;		//弓箭类增加物理攻击力固定值(长效)
			SDWORD spupdam;		//弓箭类增加物理攻击力固定值(被动)
			SDWORD spupdamp;		//弓箭类增加物理攻击力百分比(被动)
			SDWORD rpupdam;		//棍子类增加物理攻击力固定值(被动)
			SDWORD lupdam;		//长效物理攻击力增强固定值
			SDWORD satrating;	//弓箭类提升命中率
			SDWORD patrating;   // 增加命中率（被动）
			SDWORD pattackspeed;	//增加攻击速度（被动）
			SDWORD upattribute;	//属性点增加
			SDWORD tuling;      //土灵增强
			SDWORD kulou;		//骷髅弓手增强
			SDWORD tianbing;    //天兵增强
			SDWORD weaponupdamp; //提升武器物理攻击力百分比
			SDWORD weaponumdamp; //提升武器法术攻击力百分比
			SDWORD uppetdamage; //提升召唤兽的攻击力
			SDWORD uppetdefence; //提升召唤兽的防御力
			SDWORD pmaxhp;		//被动提升生命最大值
			SDWORD x1_pmaxhp;		//新增提升被动生命值1
			SDWORD x2_pmaxhp;		//新增提升被动生命值2
			SDWORD x3_pmaxhp;		//新增提升被动生命值3
			SDWORD x4_pmaxhp;		//新增提升被动生命值4
			SDWORD x5_pmaxhp;		//新增提升被动生命值5
			SDWORD x1_pupdam;		//物理攻击增加固定数值（被动）1
			SDWORD x1_pumdam;		//法术攻击增加固定数值（被动）1
			SDWORD x1_pupdef;		//物理防御增加固定数值（被动）1
			SDWORD x1_pumdef;		//法术防御增加固定数值（被动）1		
			SDWORD x2_pupdam;		//物理攻击增加固定数值（被动）2
			SDWORD x2_pumdam;		//法术攻击增加固定数值（被动）2
			SDWORD x2_pupdef;		//物理防御增加固定数值（被动）2
			SDWORD x2_pumdef;		//法术防御增加固定数值（被动）2	
			SDWORD x3_pupdam;		//物理攻击增加固定数值（被动）3
			SDWORD x3_pumdam;		//法术攻击增加固定数值（被动）3
			SDWORD x3_pupdef;		//物理防御增加固定数值（被动）3
			SDWORD x3_pumdef;		//法术防御增加固定数值（被动）3								
			SDWORD bang;			//增加人物施放出重击的几率
			SDWORD theurgy_updam; //仙术增加物理攻击力
			SDWORD theurgy_updamp; //仙术增加物理攻击力百分比
			SDWORD theurgy_updef; //仙术增加物理防御力
			SDWORD theurgy_umdefp; //仙术增加法术防御力百分比
			SDWORD theurgy_umdef; //仙术增加法术防御力
			SDWORD theurgy_umdam; //仙术增加法术攻击力
			SDWORD theurgy_umdamp; //仙术增加法术攻击力百分比
			SDWORD theurgy_updefp;//仙术增加物理防御力百分比
			SDWORD theurgy_dpdef; //仙术降低物理防御力
			SDWORD theurgy_dmdef; //仙术降低物理防御力
			SDWORD theurgy_dpdam;		//物理攻击减少固定数值
			SDWORD theurgy_dmdam;		//法术攻击减少固定数值
			SDWORD hpupbylevel; //根据人物等级提升最大血量                    //80
			SDWORD reflect_ardor; // 反弹灼热状态给攻击自己的敌人
			SDWORD reflect_poison; //反弹中毒状态给攻击自己的敌人
			SDWORD reflect_lull;  // 反弹麻痹状态给攻击自己的敌人
			SDWORD reflect_frost; // 反弹冰冻状态给攻击自己的敌人
			DWORD introject_maxmdam; // 召唤合体增加魔法攻击
			DWORD introject_maxpdam; // 召唤合体增加物理攻击
			DWORD introject_mdam; // 召唤合体增加魔法攻击
			DWORD introject_pdam; // 召唤合体增加物理攻击
			DWORD introject_mdef; // 召唤合体增加魔法防御
			DWORD introject_pdef; // 召唤合体增加物理防御
			DWORD introject_maxhp; // 召唤合体增加生命上限
			SDWORD summonrelive; // 召唤兽重生
			SDWORD pdamtodef;	// 物攻转物防
			SDWORD mdamtodef;	// 魔攻转魔防
			SDWORD pdeftodam;	// 物防转物攻
			SDWORD mdeftodam;	// 魔防转魔攻
			SDWORD sept_updamp;  // 提升物理攻击百分比
			SDWORD sept_umdamp;	// 提升魔法攻击百分比
			SDWORD sept_updefp;	// 提升物理防御百分比
			SDWORD sept_umdefp;	// 提升法术防御百分比
			SDWORD sept_maxhp;	// 提升生命最大值百分比
			SDWORD sept_maxmp;	// 提升法术最大值百分比
			SDWORD blazeappend; // 火焰系法术伤害提升比例千分之几
			SDWORD pblazeappend; // 火焰系法术伤害提升比例千分之几
			SDWORD levinappend; // 雷电系法术伤害提升比例千分之几
			SDWORD plevinappend; // 雷电系法术伤害提升比例千分之几(被动)
			SDWORD trapappend;  // 陷阱系法术伤害提升比例千分之几
			SDWORD iceappend;  // 陷阱系法术伤害提升比例千分之几
			SDWORD udef;			// 防御提升固定值（物理和法术都升）
			SDWORD magicattack;  // 两仪心法，攻击的时候附带法术伤害
			SDWORD array_udamp; // 阵法的双攻比率提升
			SDWORD array_ddefp; // 阵法的双防比率降低
			SDWORD array_dmvspeed; // 阵法的移动速度降低
			SDWORD upcon; //提升体质
			SDWORD upint; //提升智力
			SDWORD updex; //提升敏捷
			SDWORD sword_udam; //提升弓箭攻击的伤害点数   // 110
			SDWORD attackaddhpnum; // 攻击增加自己生命值数目
			SDWORD dodge;         // 有一定几率闪避敌人攻击
			SDWORD reflect_icelull; // 反弹一个麻痹状态 冰雷盾用
			SDWORD pumdamp;		//被动提升法术攻击力百分比
			SDWORD addmenp;   // 增加精神百分比
			SDWORD ice_umdefp; //冰甲用提升魔法防御百分比
			SDWORD protectdam; //保护状态之伤害
			SDWORD protectUpAtt; //保护状态之攻击翻倍
			SDWORD unitarybattle; //一元阵法投送小技能到队长正在攻击的敌人身上，此为投送的技能ID
			SDWORD teamappend; //组队攻击加成，变量里存放的是组队队长的ID
			SDWORD nsc_updamp; //逆手刺的专用技能提升状态。
			SDWORD tgyt_updefp; //铁骨御体专用技能提升状态
			SDWORD tgzh_updef; //铁骨之魂专用技能提升状态
			SDWORD lm_updamp;   // 鲁莽提升物理攻击力百分比
			SDWORD upallattrib; // 提升所有属性数值
			SDWORD dnallattrib; // 所有属性降低数值
			SDWORD upstr;      // 提升力量属性百分比
			SDWORD dpallattrib; // 所有属性下降百分比
			SDWORD upattribstr; // 提升力量属性百分比
			SDWORD upattribcon; // 提升体质属性百分比
			SDWORD upattribmen; // 提升精神属性百分比
			SDWORD upattribdex; // 提升敏捷属性百分比
			SDWORD upattribint; // 提升智力属性百分比
			SDWORD relive;    // 再生
			SDWORD nowrelive; // 立即复活时回复百分比
			SDWORD brappenddam; //猎手印记所提供的额外伤害清
			SDWORD dpintmen;   // 智力精神降低百分比
			SDWORD dpstrdex;   // 力量敏捷降低百分比
			SDWORD enervation; // 衰弱
			SDWORD erupt;      // 破釜沉舟状态 魔物攻击翻300%持续时间完后会获得复活虚弱状态。
			SDWORD physic_att; // 物理攻击力百分比
			SDWORD physic_prt; // 物理防御力百分比
			SDWORD magic_att; // 魔法攻击力百分比
			SDWORD magic_prt; // 魔法防御力百分比
			SDWORD x_upmen; // 增加精神固定属性技能
			SDWORD x_upstr; // 增加力量固定属性技能
			SDWORD x_upcon; //提升体质固定属性技能
			SDWORD x_upint; //提升智力固定属性技能
			SDWORD x_updex; //提升敏捷固定属性技能
			SDWORD physic_add; //  主动增加物理实际攻击力,用来解决增加攻击buf冲突
			SDWORD magic_add;  // 主动增加魔法实际攻击力,用来解决增加攻击buf冲突
			SDWORD cikewu; //  刺客物理百分比
			SDWORD cikemo;  // 刺客魔法百分比
			SDWORD weaponupdamp2; //提升武器物理攻击力百分比
			SDWORD weaponumdamp2; //提升武器法术攻击力百分比
			SDWORD updefp2; //被动增加物理防御百分比
			SDWORD ice_umdefp2; //被动增加魔法防御百分比
			SDWORD shuanggong; //双攻
			SDWORD shuangfang; //双防
			SDWORD xueliang; //血量
		};
		SDWORD swdValue[189]; //这里是加的全代码的数量必须写
	};
};

//在每次换装备前调用
struct PkPreValue
{
	PkPreValue()
	{
		fiveexpress = 0;
		bzero(wdValue , sizeof(wdValue));
	}
	void init()
	{
		fiveexpress = 0;
		bzero(wdValue , sizeof(wdValue));
	}
	float fiveexpress;
	union
	{
		struct 
		{//soke 突破攻击/防御65535限制
			DWORD fivedam;
			DWORD fivemaxdam;
			DWORD fivedef;
			DWORD nofivedam;
			DWORD nofivemaxdam;
			DWORD nofivedef;
			DWORD fivemdam;
			DWORD fivemaxmdam;
			DWORD fivemdef;
			DWORD nofivemdam;
			DWORD nofivemaxmdam;
			DWORD nofivemdef;
		    //新增由技能管理器调用进行取值
		    DWORD nofivewdstr;
			DWORD nofivewddex;
			DWORD nofivewdint;
			DWORD nofivewdmen;
			DWORD nofivewdcon;
		};
		DWORD wdValue[17]; //这里也要改
	};
};

struct PkValue
{
	PkValue()
	{
		bzero(dwValue , sizeof(dwValue));
	}
	void init()
	{
		bzero(dwValue , sizeof(dwValue));
	}
	union
	{
		struct 
		{
			DWORD	pdamage;					/// 物理攻击力 *
			DWORD	mdamage;					/// 法术攻击力 *
			DWORD	pdefence;					/// 物理防御力 *
			DWORD	mdefence;					/// 法术防御力 *
			DWORD	mcost;						/// 消耗法术值
			DWORD	hpcost;						/// 消耗生命值
			DWORD	spcost;						/// 消耗体力值
			DWORD	exp;						/// 经验消耗
			DWORD	dvalue;						/// 伤害值
			DWORD	dvaluep;					/// 伤害值增加百分比
			SWORD	damagebonus;				/// 技能伤害加成
		};
		DWORD dwValue[11];
	};
};

struct ScenePkState
{
	ScenePkState()
	{
		zRTime ctv;
		lastPTime = 0;
		lastMTime = 0;
		tGood = ctv;
		tGood.addDelay(ScenePkState::goodnessPeriod);
		tProtect = ctv;
		lastCheckGoodness = ctv;
		lastCheckGoodness.addDelay(ScenePkState::protectPeriod);
		protect_time=0;
	}


	//善恶度检查时间间隔
	static const int goodnessPeriod = 60 * 1000;
	zRTime tGood;

	//自卫时间间隔
	static const int protectPeriod = 10 * 1000;
	zRTime tProtect;

	//最后一次检查状态的时间
	zRTime lastCheckGoodness;

	//攻击人和自卫人记录
	struct attProtect
	{
		DWORD	id;
		BYTE	time;
		attProtect()
		{
			id = 0;
			time = 0;
		}
	};
	bool speedOutP(WORD speed, DWORD dwTime);
	bool speedOutM(WORD speed, DWORD dwTime);
	private:
	DWORD lastPTime;
	DWORD lastMTime;
	public:
	//好人杀死第一个坏人的时间
	zRTime tGoodNormal;
	void clearProtect()
	{
		protect_time=0; 
	}
	bool deathUserProtect(SceneUser *pThis , DWORD defid);
	bool cancelProtect(SceneUser *pThis,DWORD time=0);
	bool addProtect(SceneUser * pThis,DWORD time=0);
	bool hasProtected();
	DWORD leftProtectTime()
	{
		return protect_time;
	}
	private:
	DWORD protect_time;
};

struct zSkill;
using namespace SkillDef;
class ScenePk  
{
	public:
		static bool physicalMagicU2U(const Cmd::stAttackMagicUserCmd *rev , SceneUser *pAtt , SceneUser *pDef);
		static bool attackDeathUser(SceneUser *pAtt , SceneUser *pDef);
		static bool sendChangedUserData(SceneUser *pUser);
		static bool attackDeathNpc(SceneEntryPk *pAtt , SceneNpc *pDef);
		static bool attackFailToMe(const Cmd::stAttackMagicUserCmd *rev , SceneEntryPk * pAtt, bool failed=true, bool me=false);
		static void	checkProtect(SceneEntryPk *pAtt , SceneEntryPk *pDef);
		static bool attackUserCmdToNine(const Cmd::stAttackMagicUserCmd *rev , SceneEntryPk *pAtt);
		static void attackRTHpAndMp(SceneUser *pUser);
		static void attackRTExp(SceneUser *pUser , DWORD exp, DWORD dwTempID=0, BYTE byType=0);
		static bool attackRTCmdToNine(const Cmd::stAttackMagicUserCmd *rev , SceneEntryPk *pAtt , SceneEntryPk *pDef , const SDWORD sdwHP , BYTE byLuck);
		static bool checkAttackSpeed(SceneUser *pAtt , const Cmd::stAttackMagicUserCmd *rev);
		static void	calpdamU2U(const Cmd::stAttackMagicUserCmd *rev , SceneUser *pAtt , SceneUser *pDef);
		static void	calpdamU2N(const Cmd::stAttackMagicUserCmd *rev , SceneUser *pAtt , SceneNpc *pDef);
		static void	calmdamU2U(const Cmd::stAttackMagicUserCmd *rev , SceneUser *pAtt , SceneUser *pDef);
		static void calmdamU2N(const Cmd::stAttackMagicUserCmd *rev , SceneUser *pAtt , SceneNpc *pDef);
		static void calpdamN2U(const Cmd::stAttackMagicUserCmd *rev , SceneNpc *pAtt , SceneUser *pDef);
		static void calmdamN2U(const Cmd::stAttackMagicUserCmd *rev , SceneNpc *pAtt , SceneUser *pDef);
		static void calpdamN2N(const Cmd::stAttackMagicUserCmd *rev , SceneNpc *pAtt , SceneNpc *pDef);
		static void calmdamN2N(const Cmd::stAttackMagicUserCmd *rev , SceneNpc *pAtt , SceneNpc *pDef);
};

// */
#endif
