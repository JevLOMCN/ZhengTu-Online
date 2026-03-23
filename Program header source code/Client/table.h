#ifndef _GAME_TABLE_H
#define _GAME_TABLE_H

enum enumSex{
	eSex_Male,
	eSex_Female,
};


struct stCharTypeInfo
{
	int type;		// 人物类型
	int background;	// 背景图片
	int actionpic;	// 动作图片
	int equippic;	// 纸娃娃图片
	char name[32];	// 名字
	enumSex sex;	// 性别
	int hair[16];
	int equipHairPic;// 纸娃娃图片目录编号
};

const stCharTypeInfo & GetCharTypeInfo(size_t type);

#pragma pack(1)
// ===============================================================================
// ObjectBase_t [物品基本表]
// ===============================================================================
typedef struct _ObjectBase  //征途原版
//struct ObjectBase_t   //暂时修改成新的

{
	DWORD	dwID;				// 编号
	char	strName[32];		// 名称
	DWORD	dwType;				// 类型
	DWORD	dwMaxCount;			// 最大数量
	DWORD	dwCharType;			// 需要性别
	DWORD	needLevel;			// 需要等级
	DWORD	level;				// 道具等级
	DWORD	dwField6;			// 配合物品
	DWORD	maxhp;				// 最大生命值
	DWORD	maxmp;				// 最大法术值
	DWORD	maxsp;				// 最大体力值
	DWORD	pdam;				// 最小物攻
	DWORD	maxpdam;			// 最大物攻	
	DWORD	mdam;				// 最小魔攻
	DWORD	maxmdam;			// 最大魔攻
	DWORD	pDef;				// 物防
	DWORD	mDef;				// 魔防
	DWORD	atspeed;			// 攻击速度
	DWORD	mvspeed;			// 移动速度
	DWORD	atrating;			// 命中率
	DWORD	akdodge;			// 躲避率
	DWORD	dwUp;				// 改造
	DWORD	dwChange;				// 合成等级
	DWORD	dwMake;					// 打造
	DWORD   maketype;				// 打造类别
	char	strField35[32];			// 需要技能
	char	strField36[256];		// 需要原料
	DWORD	dwEquipType;			// 装备位置
	DWORD	dwMaxDur;				// 耐久度
	DWORD	price;					// 价格
	DWORD	dwActionPic;			// 对应动作图片
	DWORD	dwGroundPic;			// 对应地上图片(如果为-1 则使用物品栏上图片)
	DWORD	dwItemPic;				// 对应道具栏图片
	DWORD	dwEquipPic;				// 对应纸娃娃图片(如果为-1 则使用物品栏上图片)
	DWORD	dwColor;				// 颜色
	DWORD	cx;						// 格子宽
	DWORD	cy;						// 格子高
	char	strDesc[128];			// 说明
	DWORD	attackSound;			// 攻击(挥动武器)的声音
	DWORD	hitSound;				// 打击的声音
	DWORD	itmeSound;				// 道具栏声音
	DWORD	dwChangePrice;		    // 合成单价
	DWORD	bluerating;				// 蓝色装备掉落几率
	DWORD	goldrating;				// 金色装备掉落几率
	DWORD   dwIntervalTime;         // 冷却时间 单位 秒
	DWORD   dwTradeType;			// 交易类型
	DWORD	dwChannelNum;			// 通道编号
	DWORD	dwChannelBlend;			// 通道混合方式
	char    xin_jia[256];
//	};   //暂时修改成新的
} ObjectBase_t;  //征途原版

// ===============================================================================
// NpcBase_t [NPC基本表]
// ===============================================================================
typedef struct _NpcBase  //征途原版
//struct NpcBase_t  //暂时修改成新的
{ 
	DWORD	dwID;		// 编号
	char	strName[32];	// 名称
	DWORD	dwType;			// 类型
	DWORD	dwLevel;		// 等级
	DWORD	dwColor;		// 颜色
	DWORD	dwFive;			// 五行属性
	DWORD	dwPic;			// 图片
	DWORD	dwImage;		// 宠物图片
	DWORD	dwSummonType;	// 类别
//	}; //暂时修改成新的
}NpcBase_t;  //征途原版

// ===============================================================================
// enumSkillType [技能类型]
// ===============================================================================
enum enumSkillType{
	SkillType_Attack,		// 攻击技能为0，
	SkillType_StateAttack,	// 状态技能为1（施放时不可移动），
	SkillType_State,		// 光环技能为2（可以移动中使用），
	SkillType_Passive,		// 被动技能为3
	SkillType_Active,		// 持续使用技能能 4
	SkillType_Max = 0xffffffff,

};

// ===============================================================================
// enumSkillUseMethod [技能使用方法]
// ===============================================================================
enum enumSkillUseMethod{
	SkillUseMethod_Self				= 1,
	SkillUseMethod_BlueHand			= 2,
	SkillUseMethod_SelectFirst		= 4,
	SkillUseMethod_Lock				= 8,
};
// ===============================================================================
// enumSkillTargetType [技能目标类型]
// ===============================================================================
enum enumSkillTargetType{
	SkillTarget_Self		= 1,		// 自己
	SkillTarget_Single		= 2,		// 单体
	SkillTarget_Place		= 4,		// 群体（范围攻击）
	SkillTarget_Npc			= 8,		// NPC
	SkillTarget_Friend	= 16,		// 友方人物
	SkillTarget_Enemy	= 32,		// 敌方人物
	SkillTarget_Pet		= 64,		// 宠物
	SkillTarget_Death	= 128,		// Death
	SkillTarget_Point	= 256,		// point
	SkillTarget_Summon	= 512,		// my summon
	SkillTarget_Max = 0xffffffff,
};
// ===============================================================================
// enumSkillSerialType [技能系别类型]
// ===============================================================================
enum enumSkillSerialType{
	SkillSerial_WuShu		= 1,			//武术
	SkillSerial_Shoot		= 2,			//射术
	SkillSerial_Magic		= 3,			//法术
	SkillSerial_Call		= 4,			//召唤
	SkillSerial_Xian		= 5,			//仙术
	SkillSerial_Cike		= 6,			//刺客
	SkillSerial_Weishi		= 7,			//卫士
	SkillSerial_Qiang		= 8,			//枪系
	SkillSerial_Profesional	= 9,			//专业
	SkillSerial_Num,						//
};

// ===============================================================================
// enumSkillTreeType [技能树类型]
// ===============================================================================
enum enumSkillTreeType{
	SkillTreeType_BaseFighter,				//基础树
	SkillTreeType_CrazyFighter,		//狂暴战士树
	SkillTreeType_MagicFighter,		//魔战士树
	SkillTreeType_ProtectFighter,	//守护战士树
	SkillTreeType_BaseShooter,
	SkillTreeType_ArrowShooter,		//弓箭猎手树
	SkillTreeType_PetShooter,		//宠物猎手树
	SkillTreeType_LayShooter,		//陷阱猎手树
	SkillTreeType_BaseMagic,
	SkillTreeType_Fire,				//火法术树
	SkillTreeType_Freeze,			//冰法术树
	SkillTreeType_Electric,			//电法术树
	SkillTreeType_BaseCall,
	SkillTreeType_CallDeath,		//亡灵召唤树
	SkillTreeType_CallGod,			//天神召唤树
	SkillTreeType_CallNatural,		//自然召唤树
	SkillTreeType_BaseSprite,
	SkillTreeType_Sky,				//天树
	SkillTreeType_Earth,			//地树
	SkillTreeType_Person,			//人树
	SkillTreeType_Cike = 24,
	SkillTreeType_Weishi = 28,
	SkillTreeType_Qiang = 32,
	SkillTreeType_BaseProfession,
	SkillTreeType_ProfessionOne,
	SkillTreeType_ProfessionTwo,
	SkillTreeType_ProfessionThree,
	SkillTreeType_Num,				//

};

//------------------------------------
// SkillBase
//------------------------------------
//typedef struct SkillBase{
//	DWORD	dwID;		// 技能ID
//	char	pstrName[32];	// 技能名称
//	DWORD	dwLevel;		// 技能等级
//	DWORD	dwMaxLevel;		// 最高等级
//	DWORD	dwInitLevel;	// 最初角色等级
//	DWORD	dwHP;		// 消耗生命值
//	DWORD	dwMP;		// 消耗法术值
//	DWORD	dwSP;		// 消耗体力值
//	DWORD	dwInitFivePoint;// 最初五行点数
//	DWORD	dwIntervalTime;	// 间隔时间
//	DWORD	dwSkillPoint;		// 技能点数
//	DWORD	dwUpLevel;		// 需要角色等级
//	DWORD	dwInitLevel1;		// 辅助1初始等级
//	DWORD	dwUpLevel1;		// 辅助1升级等级
//	DWORD	dwInitLevel2;		// 辅助2初始等级
//	DWORD	dwUpLevel2;		// 辅助2升级等级
//	DWORD	dwUpItemID;		// 升级物品
//	char	strDesc[512];	// 说明
//	DWORD	dwFive;			// 五行属性
//	enumSkillTargetType	dwTarget;	// 目标
//	DWORD	dwDis;		// 距离范围
//	enumSkillType	dwType;	// 攻击方式
//	char	pstrConsume[32];// 适用武器类型
//	DWORD	dwCharType;		// 需要职业
//	DWORD	dwPic;			// 特效图片号码
//	DWORD	dwIcon;			// 技能ico
//	DWORD	dwAllowRide;	// 允许骑马
//
//}SkillBase_t;//导出 SkillBase 成功，共 320 条记录

//------------------------------------
// SkillBase
//------------------------------------
typedef struct SkillBase{
	DWORD	dwID;		// 技能ID
	char	pstrName[32];	// 技能名称
	DWORD	dwLevel;		// 技能等级
	enumSkillSerialType	dwSerialType;		//技能系别
	enumSkillTreeType	dwTreeType;		//技能树别
	DWORD	dwSkillPoint;		//需要本树技能点数
	DWORD	dwNeedSkill1;		//前提技能1
	DWORD	dwNeedSkill1Level;
	DWORD	dwNeedSkill2;		//前提技能2
	DWORD	dwNeedSkill2Level;
	DWORD	dwNeedSkill3;		//前提技能3
	DWORD	dwNeedSkill3Level;
	DWORD	dwIntervalTime;	// 间隔时间
	DWORD	dwPic;			// 特效图片号码
	DWORD	dwIcon;			// 技能ico
	enumSkillTargetType	dwTarget;	// 目标
	DWORD	dwDis;		// 距离范围
	enumSkillType	dwType;	// 攻击方式
	DWORD	dwDir;
	DWORD	dwAllowRide;	// 允许骑马
	DWORD	dwCenter;
	DWORD	dwUpItemID;		// 升级物品
	char	pstrConsume[32];// 适用武器类型
	DWORD	dwSP;		// 消耗体力值
	DWORD	dwMP;		// 消耗法术值
	DWORD	dwHP;		// 消耗生命值
	DWORD	dwHurtAdd;	// 伤害加成
	char	strDesc[512];	// 说明
	DWORD	dwCostID;	// 消耗物品
	DWORD	dwCostNum;	// 消耗数量
	enumSkillUseMethod	dwUseMethod;	// 施放方式
	
	//DWORD	dwMaxLevel;
	//DWORD	dwInitLevel;	// 最初角色等级
	//DWORD	dwInitFivePoint;// 最初五行点数
	//DWORD	dwUpLevel;		// 需要角色等级
	//DWORD	dwInitLevel1;		// 辅助1初始等级
	//DWORD	dwUpLevel1;		// 辅助1升级等级
	//DWORD	dwInitLevel2;		// 辅助2初始等级
	//DWORD	dwUpLevel2;		// 辅助2升级等级
	//DWORD	dwFive;			// 五行属性
	//DWORD	dwCharType;		// 需要职业
}SkillBase_t;//导出 SkillBase 成功，共 320 条记录

// ===============================================================================
// SkillLevelBase [技能等级信息]
// ===============================================================================
typedef struct SkillLevelBase{
	DWORD	dwID;		// 技能ID
	char	pstrName[32];	// 技能名称
	DWORD	dwLevel;		// 技能等级
	enumSkillSerialType	dwSerialType;		//技能系别
	enumSkillTreeType	dwTreeType;		//技能树别
	DWORD	dwSkillPoint;		//需要本树技能点数
	DWORD	dwNeedSkill1;		//前提技能1
	DWORD	dwNeedSkill1Level;	//前提技能1级别
	DWORD	dwNeedSkill2;		//前提技能2
	DWORD	dwNeedSkill2Level;	//前提技能2级别
	DWORD	dwNeedSkill3;		//前提技能3
	DWORD	dwNeedSkill3Level;	//前提技能3级别
	DWORD	dwIntervalTime;		// 间隔时间
	DWORD	dwPic;			// 特效图片号码
	DWORD	dwIcon;			// 技能ico
	enumSkillTargetType	dwTarget;	// 目标
	DWORD	dwDis;		// 距离范围
	enumSkillType	dwType;	// 攻击方式
	DWORD	dwDir;
	DWORD	dwAllowRide;	// 允许骑马
	DWORD	dwCenter;
	DWORD	dwUpItemID;		// 升级物品
	char	pstrConsume[32];// 适用武器类型
	DWORD	dwSP;		// 消耗体力值
	DWORD	dwMP;		// 消耗法术值
	DWORD	dwHP;		// 消耗生命值
	DWORD	dwHurtAdd;	// 伤害加成
	char	strDesc[512];	// 说明
	DWORD	dwCostID;	// 消耗物品
	DWORD	dwCostNum;	// 消耗数量
	enumSkillUseMethod	dwUseMethod;	// 施放方式

}SkillLevelBase_t;//导出 SkillLevelBase 成功，共 320 条记录

// ===============================================================================
// CharacterStateBase [角色基本状态]
// ===============================================================================
typedef struct CharacterStateBase{
	DWORD	dwID;			// 编号
	char	strName[32];	// 名字
	DWORD	dwPicID1;		// 图片编号
	DWORD	dwPicID2;		// 图片编号
	DWORD	dwPicID3;		// 图片编号
	DWORD	dwPicID4;		// 图片编号
	DWORD	dwStateID;		// State icon id
	char	strDesc[512];	// 说明
	DWORD	dwType;			// 1:数值  0:百分比
}CharacterStateBase_t;//导出 CharacterStateBase 成功，共 41 条记录

//------------------------------------
// ObjectUp
//------------------------------------
typedef struct ObjectUp{
	DWORD	dwID;		        // 编号
	DWORD	dwMoney;	    	// 需要银子
}ObjectUp_t;//导出 ObjectUp 成功，共 2106 条记录

//--------------------------------------
// ===============================================================================
// ItemSetBase
// ===============================================================================
typedef struct ItemSetBase{
	DWORD	dwID;				//套装id
	char	pstrName[32];		//套装名称
	DWORD	charType;			//职业
	DWORD	itemList[7];		//道具id
	DWORD	effectOneID[3];		//effectOneID[0] = num	套装属性id
	DWORD	effectTwoID[5];		//effectTwoID[0] = num	套装属性id
	DWORD	idList[7];			//套装属性id
}ItemSetBase_t;		
//--------------------------------------

//--------------------------------------
//temp struct to save source table date
// ===============================================================================
// ItemSetTable
// ===============================================================================
typedef struct ItemSetTable
{
	DWORD	dwID;
	char	pstrName[32];
	DWORD	charType;
	DWORD	rate;
	char	pstrListID[32];
	char	pstrEffectOneID[32];
	char	pstrEffectTwoID[32];
	char	pstrAllID[32];
}ItemSetTable_t;
//---------------------------------------

// ===============================================================================
// HairStyle [发型]
// ===============================================================================
typedef struct HairStyle{
	DWORD	dwID;			// 编号
	char	strName[32];	// 名称
	DWORD	dwSex;			// 性别
	DWORD	dwActionPicID;	// 动作图片
	DWORD	dwEquipPicID;	// 纸娃娃图片
	DWORD	dwMoney;		// 费用
	DWORD	dwRegister;		//注册类型
}HairStyle_t;//导出 HairStyle 成功

//------------------------------------
// HairColor [头发颜色]
//------------------------------------
typedef struct HairColor{
	DWORD	dwID;				// 编号
	char	strName[32];		// 名称
	DWORD	dwColor;			// 颜色
	DWORD	dwMoney;			// 费用
}HairColor_t;//导出 HairColor 成功


//------------------------------------
// Knight [国王]
//------------------------------------
typedef struct Knight{
	DWORD	dwID;				// 编号
	char	strName[32];		// 官衔名称
	DWORD	dwValue;		// 所需功勋值
}Knight_t;//导出 Knight 成功，共 69 条记录

//------------------------------------
// Civilian [平民]
//------------------------------------
typedef struct Civilian{
	DWORD	dwID;				// 编号
	char	strName[32];		// 官衔名称
	DWORD	dwValue;		// 所需文采值
}Civilian_t;//导出 Civilian 成功，共 6 条记录


//------------------------------------
// HeadList头像
//------------------------------------
typedef struct HeadList{
	DWORD	dwID;		// 编号
	char	strName[16];		// 头像名
	DWORD	dwSex;		// 性别
	DWORD	dwHeadID;		// 头像编号
	DWORD	dwMoney;		// 费用
}HeadList_t;//导出 HeadList 成功

// ===============================================================================
// 国家地质表
// ===============================================================================
typedef struct CountryMaterial  //征途原版
//typedef struct  //暂时修改成新的
{
	DWORD	dwID;		// 编号
	DWORD	dwObjectID;		// 物品ID
	DWORD	dwMaterialType;		// 材料类型
	DWORD	dwObjectType;		// 物品类别
}CountryMaterial_t;//导出 CountryMaterial 成功，共 1066 条记录



// ===============================================================================
//  神剑配置表 by醉梦
// ===============================================================================
typedef struct shenjianpeizhi  //征途原版
//typedef struct  //暂时修改成新的
{
	DWORD	level;		// 编号
	DWORD	value;		// 物品ID
	DWORD	costID;		// 材料类型
	DWORD	costNum;		// 物品类别
	DWORD	salary;		// 物品类别
	DWORD	pDam;		// 物品类别
	DWORD	mDam;		// 物品类别
	DWORD	pDef;		// 物品类别
	DWORD	mDef;		// 物品类别
	DWORD	hp;		// 物品类别
}shenjianpeizhi_t;//导出 CountryMaterial 成功，共 1066 条记录


// ===============================================================================
//  征途转配置表 by醉梦
// ===============================================================================
//首先 我们先来到table.h这个文件内 定义一下 我们要从xml里读取什么值
//这里我们定义了一个类型 叫 ztzpeizhi  这个类型里有一堆 整数型的值 这些值就对应着  xml里每一个等级的属性  这里可以理解不
typedef struct ztzpeizhi  //征途原版
//typedef struct  //暂时修改成新的
{
	DWORD	level;		// 编号
	DWORD	value;		// 物品ID
	DWORD	costID;		// 材料类型
	DWORD	costNum;		// 物品类别
	DWORD	salary;		// 物品类别
	DWORD	pDam;		// 物品类别
	DWORD	mDam;		// 物品类别
	DWORD	pDef;		// 物品类别
	DWORD	mDef;		// 物品类别
	DWORD	hp;		// 物品类别
}ztzpeizhi_t;//导出 CountryMaterial 成功，共 1066 条记录

// ===============================================================================
//  十二生肖配置表 by醉梦
// ===============================================================================
typedef struct shengxiaopeizhi  //征途原版
//typedef struct  //暂时修改成新的
{
	DWORD	level;		// 编号
	DWORD	value;		// 物品ID
	DWORD	costID;		// 材料类型
	DWORD	costNum;		// 物品类别
	DWORD	salary;		// 物品类别
	DWORD	pDam;		// 物品类别
	DWORD	mDam;		// 物品类别
	DWORD	pDef;		// 物品类别
	DWORD	mDef;		// 物品类别
	DWORD	hp;		// 物品类别
}shengxiaopeizhi_t;//导出 CountryMaterial 成功，共 1066 条记录

// ===============================================================================
//  圣器配置表 by醉梦
// ===============================================================================
typedef struct shengqipeizhi  //征途原版
//typedef struct  //暂时修改成新的
{
	DWORD	level;		// 编号
	DWORD	value;		// 物品ID
	DWORD	costID;		// 材料类型
	DWORD	costNum;		// 物品类别
	DWORD	salary;		// 物品类别
	DWORD	pDam;		// 物品类别
	DWORD	mDam;		// 物品类别
	DWORD	pDef;		// 物品类别
	DWORD	mDef;		// 物品类别
	DWORD	hp;		// 物品类别
}shengqipeizhi_t;

// ===============================================================================
//  经脉配置表 by醉梦
// ===============================================================================
typedef struct jingmaipeizhi  //征途原版
//typedef struct  //暂时修改成新的
{
	DWORD	level;		// 编号
	DWORD	value;		// 物品ID
	DWORD	costID;		// 材料类型
	DWORD	costNum;		// 物品类别
	DWORD	salary;		// 物品类别
	DWORD	pDam;		// 物品类别
	DWORD	mDam;		// 物品类别
	DWORD	pDef;		// 物品类别
	DWORD	mDef;		// 物品类别
	DWORD	hp;		// 物品类别
}jingmaipeizhi_t;

/**
 * \brief 简短描述

 * 
 * 详细描述
 * 
 * \param pSkill : 描述
 * \return 返回值的描述
 */
inline bool IsActiveSkill(SkillBase_t * pSkill)
{
	return pSkill->dwType == SkillType_Active;
}


#pragma pack()

typedef std::vector<ObjectBase_t> tTableObjectBase;
typedef std::vector<NpcBase_t> tTableNpcBase;
typedef std::vector<SkillBase_t> tTableSkillBase;
typedef std::vector<SkillLevelBase_t> tTableSkillLevelBase;
typedef std::vector<ObjectUp_t> tTableObjectUp;
typedef std::vector<ItemSetBase_t> tTableItemSetBase;
typedef std::vector<CharacterStateBase_t> tTableCharStateBase;
typedef std::vector<HairStyle_t> tTableHairStyle;
typedef std::vector<HairColor_t> tTableHairColor;
typedef std::vector<Knight_t> tTableKnight;
typedef std::vector<Civilian_t> tTableCivilian;
typedef std::vector<HeadList_t>  tTableHeadList;
typedef std::map<int,std::string>	tLevelUpTipMap;
typedef std::vector<HeadList_t>  tTableHeadList;
typedef std::vector<CountryMaterial_t>  tTableCountryMaterialList;
typedef std::vector<shenjianpeizhi>  tShenjianList;
typedef std::vector<ztzpeizhi>  tZtzList;
typedef std::vector<shengxiaopeizhi>  tShengxiaoList;
typedef std::vector<shengqipeizhi>  tShengqiList;
typedef std::vector<jingmaipeizhi>  tJingmaiList;

extern tTableObjectBase g_tableObjectBase;
extern tTableNpcBase	 g_tableNpcBase;
extern tTableSkillBase	 g_tableSkillBase;
extern tTableSkillLevelBase	 g_tableSkillLevelBase;
extern tTableObjectUp	 g_tableObjectUp;
extern tTableItemSetBase	g_tableItemSetBase;
extern tTableCharStateBase	g_tableCharStateBase;
extern tTableHairStyle		g_tableHairStyle;
extern tTableHairColor		g_tableHairColor;
extern tTableKnight        g_tableKnight;
extern tTableCivilian      g_tableCivilian;
extern tTableHeadList		g_tableHeadList;
extern tLevelUpTipMap		g_mapLevelup;
extern tTableCountryMaterialList  g_CountryMatrialList;
extern tShenjianList		g_jianzhong;
extern tZtzList		g_ztz;
extern tShengxiaoList		g_shengxiao;
extern tShengqiList		g_shengqi;
extern tJingmaiList		g_jingmai;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param p1 : 描述
 * \param p2 : 描述
 * \return 返回值的描述
 */
template <class tRecord>
int compareRecord(const void* p1,const void* p2)
{
	DWORD key = *(DWORD*)p1;
	tRecord* pRecord = (tRecord*)p2;
	return (long)key - (long)pRecord->dwID;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param table : 描述
 * \param dwID : 描述
 * \return 返回值的描述
 */
template <class tRecord>
tRecord* FindRecordFromTable(std::vector<tRecord> & table,DWORD dwID)
{
	// 二分法查找
	return (tRecord*)bsearch(&dwID,&table[0],table.size(),sizeof(tRecord),compareRecord<tRecord>);
}

struct stFindSkillLevelKey{
	DWORD dwID;
	DWORD dwLevel;
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param p1 : 描述
 * \param p2 : 描述
 * \return 返回值的描述
 */
inline int compareSkillLevel(const void* p1,const void * p2)
{
	stFindSkillLevelKey * pKey = (stFindSkillLevelKey*)p1;
	SkillLevelBase_t* pSkill = (SkillLevelBase_t*)p2;

	if( pKey->dwID == pSkill->dwID)
		return (long)pKey->dwLevel - (long)pSkill->dwLevel;
	return (long)pKey->dwID - (long)pSkill->dwID;
	
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param table : 描述
 * \param dwID : 描述
 * \param dwLevel : 描述
 * \return 返回值的描述
 */
inline SkillLevelBase_t* FindSkillLevelFromTable(std::vector<SkillLevelBase_t> & table,DWORD dwID,DWORD dwLevel)
{
	stFindSkillLevelKey key;
	key.dwID = dwID;
	key.dwLevel = dwLevel;
	return (SkillLevelBase_t*)bsearch(&key,&table[0],table.size(),sizeof(SkillLevelBase_t),compareSkillLevel);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwID : 描述
 * \return 返回值的描述
 */
inline ObjectBase_t* GetObjectBase(DWORD dwID)
{
	return FindRecordFromTable(g_tableObjectBase,dwID);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwID : 描述
 * \return 返回值的描述
 */
inline NpcBase_t * GetNpcBase(DWORD dwID)
{
	return FindRecordFromTable(g_tableNpcBase,dwID);
}

inline HairStyle_t* GetHairStyle(DWORD id)
{
	return FindRecordFromTable(g_tableHairStyle,id);
}

inline DWORD GetHairActionPicID(DWORD id)
{
	HairStyle_t* p = GetHairStyle(id);
	if(p) return p->dwActionPicID;
	return 0;
}

inline HairColor_t* GetHairClor(DWORD id)
{
	return FindRecordFromTable(g_tableHairColor,id);
}

inline DWORD GetHairColorMoney(DWORD id)
{
	HairColor_t* p = GetHairClor(id);
	if(p) return p->dwMoney;
	return -1;
}

inline HeadList_t* GetHeadInfo(DWORD id)
{
	return FindRecordFromTable(g_tableHeadList,id);
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param dwID : 描述
* \return 返回值的描述
*/
inline DWORD GetNpcImageID(DWORD dwID)
{
	NpcBase_t* pNpcBase = GetNpcBase( dwID );
	DWORD imageID = 1;
	if ( pNpcBase )	imageID = pNpcBase->dwPic;

	return imageID;
}

inline const char* GetLevelUpTip(int level)
{
	tLevelUpTipMap::iterator it = g_mapLevelup.find(level);
	if( it != g_mapLevelup.end() )
	{
		return it->second.c_str();
	}
	return NULL;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param dwID : 描述
* \return 返回值的描述
*/
inline DWORD GetPetImageID(DWORD dwID)
{
	NpcBase_t* pNpcBase = GetNpcBase( dwID );
	DWORD imageID = 1;
	if ( pNpcBase )	imageID = pNpcBase->dwImage;

	return imageID;
}
//dwImage

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwID : 描述
 * \return 返回值的描述
 */
inline SkillBase_t * GetSkillBase(DWORD dwID)
{
	return FindRecordFromTable(g_tableSkillBase,dwID);
}

inline DWORD GetSkillIdFromBook(DWORD dwID)
{
	for(tTableSkillLevelBase::iterator it = g_tableSkillLevelBase.begin(); it != g_tableSkillLevelBase.end(); ++it)
	{
		if ( (*it).dwUpItemID == dwID )
			return (*it).dwID;
	}

	return -1;
}

inline DWORD GetSkillLevelFromBook(DWORD dwID)
{
	for(tTableSkillLevelBase::iterator it = g_tableSkillLevelBase.begin(); it != g_tableSkillLevelBase.end(); ++it)
	{
		if ( (*it).dwUpItemID == dwID )
			return (*it).dwLevel;
	}

	return -1;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwID : 描述
 * \param dwLevel : 描述
 * \return 返回值的描述
 */
inline SkillLevelBase_t* GetSkillLevelBase(DWORD dwID,DWORD dwLevel)
{
	return FindSkillLevelFromTable(g_tableSkillLevelBase,dwID,dwLevel);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwID : 描述
 * \return 返回值的描述
 */
inline ObjectUp_t* GetObjectUp(DWORD dwID)
{
	return FindRecordFromTable(g_tableObjectUp,dwID);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param dwID : 描述
 * \return 返回值的描述
 */
inline CharacterStateBase_t * GetCharacterStateBase(DWORD dwID)
{
	return FindRecordFromTable(g_tableCharStateBase,dwID);
}


extern bool LoadTables();
extern void UnloadTables();
extern bool ReLoadSkillTable();
extern bool ReSetSkillData(SkillBase_t* pSkill,DWORD dwLevel);
extern bool IsCanSelectInHandSkill(SkillBase_t* pSelectSkill);
extern bool IsSelectFirstSkill(SkillBase* pSelectSkill);
extern char*  GetKnightName(DWORD value);
extern char*  GetCivilianName(DWORD value);

#endif