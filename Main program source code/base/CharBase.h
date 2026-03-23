/**
 * \file
 * \version  $Id: CharBase.h  $
 * \author  
 * \date 
 * \brief 定义人物角色相关信息
 *
 * 
 */


#ifndef _CharBase_h_
#define _CharBase_h_

#include "zType.h"
#include "Object.h"
#pragma pack(1)

#define HAIRTYPE_MASK	0xff000000		/// 头发发型
#define HAIRRGB_MASK	0x00ffffff		/// 头发颜色
const int exploit_arg = 100;			// 功勋值同比放大系数

//---------------------------------
#define LEVELUP_HP_N			100
#define LEVELUP_MP_N			1
#define LEVELUP_SP_N			5

#define LEVELUP_RESUMEHP_N		0
#define	LEVELUP_RESUMEMP_N		0
#define LEVELUP_RESUMESP_N		0

#define LEVELUP_ATTACKRATING_N	1
#define LEVELUP_ATTACKDODGE_N	1

#define LEVELUP_MOVESPEED_N		0
#define LEVELUP_ATTACKSPEED_N	0

#define LEVELUP_PDAMAGE_N		0
#define LEVELUP_MDAMAGE_N		0
#define LEVELUP_PDEFENCE_N		0
#define LEVELUP_MDEFENCE_N		0

#define LEVELUP_BANG_N		0
//----------------------------------
#define BASEDATA_M_HP			1100 //soke 新建角色初始血量
#define BASEDATA_M_MP			61   //soke 新建角色初始魔法
#define BASEDATA_M_SP			100

#define BASEDATA_M_RESUMEHP		1
#define	BASEDATA_M_RESUMEMP		1
#define BASEDATA_M_RESUMESP		1

#define BASEDATA_M_ATTACKRATING	1
#define BASEDATA_M_ATTACKDODGE	1

#define BASEDATA_M_MOVESPEED	0
#define BASEDATA_M_ATTACKSPEED	0

#define BASEDATA_M_PDAMAGE		11
#define BASEDATA_M_MDAMAGE		11
#define BASEDATA_M_PDEFENCE		1
#define BASEDATA_M_MDEFENCE		1

#define BASEDATA_M_BANG		1
//----------------------------------
#define BASEDATA_F_HP			1100
#define BASEDATA_F_MP			61
#define BASEDATA_F_SP			90

#define BASEDATA_F_RESUMEHP		1
#define	BASEDATA_F_RESUMEMP		1
#define BASEDATA_F_RESUMESP		1

#define BASEDATA_F_ATTACKRATING	1
#define BASEDATA_F_ATTACKDODGE	1

#define BASEDATA_F_MOVESPEED	0
#define BASEDATA_F_ATTACKSPEED	0

#define BASEDATA_F_PDAMAGE		11
#define BASEDATA_F_MDAMAGE		11
#define BASEDATA_F_PDEFENCE		1
#define BASEDATA_F_MDEFENCE		1

#define BASEDATA_F_BANG		1
//----------------------------------

struct CharBase
{
	DWORD accid;						/// 账号
	DWORD id;							/// 角色编号
	char  name[MAX_NAMESIZE + 1];		/// 角色名称
	WORD  type;							/// 角色类型
	WORD  level;						/// 角色等级
	WORD  face;							/// 性别
	DWORD hair;							/// 头发，发型和颜色
	DWORD bodyColor;							/// 光身颜色
	DWORD goodness;						/// 善恶度
	DWORD mapid;						/// 角色所在地图编号
	char  mapName[MAX_NAMESIZE + 1];	/// 角色所在地图名称
	DWORD x;							/// 角色所在坐标x
	DWORD y;							/// 角色所在坐标y
	DWORD unionid;						/// 帮会ID
	DWORD schoolid;						/// 门派ID
	DWORD septid;						/// 家族ID
	DWORD hp;							/// 当前生命值
//	DWORD maxhp;						/// 最大生命值 *
//	DWORD resumehp;						/// 生命值恢复 *
	DWORD mp;							/// 当前法术值
//	DWORD maxmp;						/// 最大法术值 *
//	DWORD resumemp;						/// 法术值恢复 *
	DWORD sp;							/// 当前体力值
//	DWORD maxsp;						/// 最大体力值 *
//	DWORD resumesp;						/// 体力值恢复 *
//	DWORD pdamage;						/// 最小物理攻击力 *
//	DWORD mdamage;						/// 最小法术攻击力 *
//	DWORD pdefence;						/// 最小物理防御力 *
//	DWORD mdefence;						/// 最小法术防御力 *
	uint64_t exp;						///sky 当前经验值
//	WORD  attackspeed;					/// 攻击速度 *
//	WORD  movespeed;					/// 移动速度 *
//	WORD  attackrating;					/// 攻击命中 *
//	WORD  attackdodge;					/// 攻击躲避 *
//	WORD  bang;							/// 重击 *
	WORD  lucky;						/// 幸运值 *
	WORD  skillpoint;					/// 技能点数
	WORD  points;						/// 未分配人物属性点数
	DWORD	country;						/// 国家
	DWORD	consort;							/// 配偶
	/*
	//系统设置
	union{
		struct{
			union{
				struct{
					BYTE pkMode;
					BYTE setting[19];
				};
				BYTE sysSetting[20];
			};
		};
		DWORD dwSeting;
	};
	*/
	QWORD forbidtalk;					///禁言
	DWORD bitmask;						/// 标志掩码
	DWORD onlinetime;					/// 在线时间统计
	union {
		struct {
			WORD wdCon;	//体质
			WORD wdStr;	//体力
			WORD wdDex;	//敏捷
			WORD wdInt;	//智力
			WORD wdMen;	//精神
		};
		WORD wdProperty[5];
	};
	WORD reliveWeakTime;			/// 复活虚弱剩余时间
	DWORD grace;				// 文采值
	DWORD exploit;				/// 功勋值
	char tiretime[36+1];			/// 疲劳时间标记
	DWORD offlinetime;				/// 上次下线时间
	DWORD fivetype;					/// 五行类型
	DWORD fivelevel;				/// 五行点数
	DWORD pkaddition;				/// 善恶追加值
	DWORD money;				/// 当前银子,只存储维护,不能使用
	DWORD answerCount;			/// 当天答题次数
	DWORD honor;				//荣誉值
	DWORD maxhonor;				//最大荣誉值
	DWORD gomaptype;			//跳地图类型
	DWORD msgTime;				//下次发送GM留言的时间
	DWORD accPriv;			//帐号权限
	DWORD gold;					/// 金子
	DWORD ticket; 				/// 积分点数
	DWORD salary; 				/// 工资
	DWORD createtime;			/// 角色创建时间
	DWORD goldgive;				/// 金币冲值赠品数量
	BYTE petPack;				/// 宠物包裹大小
	DWORD petPoint;				/// 奖励的宠物修炼时间
	DWORD levelsept;			/// 离开家族的时间
	DWORD punishTime;			/// 杀人被抓的时间，分钟为单位
	DWORD trainTime;			/// 在练级地图逗留的时间
	DWORD doubleatime;          /// 1.25倍物品剩余时间
	DWORD doublebtime;          /// 1.50倍物品剩余时间
	DWORD doublectime;          /// 1.75倍物品剩余时间
	DWORD flower1time;          /// 9朵花剩余时间
	DWORD flower2time;          /// 99朵花剩余时间
	DWORD flower3time;          /// 999朵花剩余时间
	DWORD flower4time;          /// 9朵蓝色妖姬花剩余时间
	DWORD flower5time;          /// 99朵蓝色妖姬花剩余时间
	DWORD flower6time;          /// 999朵蓝色妖姬花剩余时间
	DWORD luckytime;            /// 幸运丹剩余时间
	DWORD luckyCount;			/// 当天幸运丹次数
	DWORD loginCount;			/// 每日登陆领取次数
	DWORD svipCount;			/// 每日vip领取次数
	DWORD taimCount;			/// 每日太庙完成次数
	DWORD killerone; 		    /// 被杀次数（1v1专用）
	DWORD killertwo; 			/// 被杀次数（待定）
	DWORD killerthe; 			/// 被杀次数（待定）
	DWORD killerTime; 			/// 杀人时间（攻击者功勋专用）
	DWORD safeTime; 			//  被杀时间（用于10分钟击杀同一玩家不计功勋）
    DWORD changejob;            /// 角色职业
	DWORD charvip; 				/// VIP点数
	DWORD chongzhi; 			/// 充值点数
	DWORD fbCount; 			    /// 副本次数
	DWORD fubentime; 			/// 副本时间
	DWORD folwers; 				/// 鲜花
    DWORD moods; 				/// 人气
	DWORD flloves; 				/// 爱心
	DWORD round;                /// 转生次数
	DWORD angel; 				/// 翅膀
	DWORD hanbing; 				/// 寒冰
	DWORD zhanli;				///add 2023-04-10 战力
	DWORD xhbbcount;            //鲜花榜单特效判定
    DWORD herocount;            //英雄榜单特效判定 
	DWORD zhanlicount;          //战力榜单特效判定 
	DWORD jian1; 				/// 剑冢
	DWORD jian2; 				/// 剑冢
	DWORD jian3; 				/// 剑冢
	DWORD jian4; 				/// 剑冢
	DWORD jian5; 				/// 剑冢
	DWORD jian6; 				/// 剑冢
	DWORD zhuan1; 				/// 征途传	
	DWORD zhuan2; 				/// 征途传	
	DWORD zhuan3; 				/// 征途传
	DWORD zhuan4; 				/// 征途传
	DWORD zhuan5; 				/// 征途传
	DWORD zhuan6; 				/// 征途传
	DWORD zhuan7; 				/// 征途传	
	DWORD zhuan8; 				/// 征途传
	DWORD zhuan9; 				/// 征途传
	DWORD zhuan10; 				/// 征途传	
	DWORD shengq1; 			/// 圣器
	DWORD shengx1; 			/// 十二生肖
	DWORD shengx2; 			/// 十二生肖
	DWORD shengx3; 			/// 十二生肖
	DWORD shengx4; 			/// 十二生肖
	DWORD shengx5; 			/// 十二生肖
	DWORD shengx6; 			/// 十二生肖
	DWORD shengx7; 			/// 十二生肖
	DWORD shengx8; 			/// 十二生肖
	DWORD shengx9; 			/// 十二生肖
	DWORD shengx10; 			/// 十二生肖
	DWORD shengx11; 			/// 十二生肖
	DWORD shengx12; 			/// 十二生肖		
	DWORD jingm1; 			/// 经脉
	DWORD jingm2; 			/// 经脉
	DWORD jingm3; 			/// 经脉
	DWORD lianzhannum; 			/// 连斩人头
	DWORD lianzhanmaxnum; 		/// 连斩最高人头
	DWORD lianzhantime; 		/// 连斩时间  

	DWORD jingcai0_objectid; 		/// 竞猜1 
	DWORD jingcai0_objectlevel; 	/// 竞猜1
	DWORD jingcai0_objectnum; 		/// 竞猜1 
	
	DWORD jingcai1_objectid; 		/// 竞猜1 
	DWORD jingcai1_objectlevel; 	/// 竞猜1
	DWORD jingcai1_objectnum; 		/// 竞猜1 
	
	DWORD jingcai2_objectid; 		/// 竞猜1 
	DWORD jingcai2_objectlevel; 	/// 竞猜1
	DWORD jingcai2_objectnum; 		/// 竞猜1 

	DWORD jingcai3_objectid; 		/// 竞猜1 
	DWORD jingcai3_objectlevel; 	/// 竞猜1
	DWORD jingcai3_objectnum; 		/// 竞猜1 

	DWORD jingcai4_objectid; 		/// 竞猜1 
	DWORD jingcai4_objectlevel; 	/// 竞猜1
	DWORD jingcai4_objectnum; 		/// 竞猜1 

	DWORD jingcai5_objectid; 		/// 竞猜1 
	DWORD jingcai5_objectlevel; 	/// 竞猜1
	DWORD jingcai5_objectnum; 		/// 竞猜1 

	DWORD jingcai6_objectid; 		/// 竞猜1 
	DWORD jingcai6_objectlevel; 	/// 竞猜1
	DWORD jingcai6_objectnum; 		/// 竞猜1 
	
	DWORD jingcai7_objectid; 		/// 竞猜1 
	DWORD jingcai7_objectlevel; 	/// 竞猜1
	DWORD jingcai7_objectnum; 		/// 竞猜1 
	
	DWORD jingcai8_objectid; 		/// 竞猜1 
	DWORD jingcai8_objectlevel; 	/// 竞猜1
	DWORD jingcai8_objectnum; 		/// 竞猜1 

	DWORD jingcai9_objectid; 		/// 竞猜1 
	DWORD jingcai9_objectlevel; 	/// 竞猜1
	DWORD jingcai9_objectnum; 		/// 竞猜1 

	DWORD jingcai10_objectid; 		/// 竞猜1 
	DWORD jingcai10_objectlevel; 	/// 竞猜1
	DWORD jingcai10_objectnum; 		/// 竞猜1 

	DWORD jingcai11_objectid; 		/// 竞猜1 
	DWORD jingcai11_objectlevel; 	/// 竞猜1
	DWORD jingcai11_objectnum; 		/// 竞猜1 

	DWORD touxianlevel; 	/// 头衔等级 
	DWORD touxianexp; 		/// 头衔经验
	DWORD chenghao_select;  //当前选择称号

	DWORD fba1; 	/// 副本次数
	DWORD fba2; 	/// 副本次数
	DWORD fba3; 	/// 副本次数
	DWORD fba4; 	/// 副本次数
	DWORD fba5; 	/// 副本次数
	DWORD vipfb;    ///vip副本次数

	DWORD babytype; 	/// 宝宝类型
	DWORD babylevel; 	/// 宝宝等级
	DWORD babyexp; 	/// 宝宝经验
	DWORD babyskill1; 	/// 宝宝技能1
	DWORD babyskill2; 	/// 宝宝技能2
	DWORD babyskill3; 	/// 宝宝技能3
	DWORD babyskill4; 	/// 宝宝技能4
	DWORD babyskill5; 	/// 宝宝技能5
	DWORD babyskillpoint; 	/// 宝宝技能点

	DWORD bikecs; 	/// 自行车次数
	DWORD bikestate; 	/// 自行车状态
	DWORD bikenum; 	/// 自行车节点
	
	//云天别墅
	DWORD bieshulevel; 	/// 别墅等级
	DWORD bieshuexp; 	/// 别墅经验
	DWORD bieshustate; 	/// 别墅激活状态

	DWORD xiulianexp; 	/// 能力修炼经验
	DWORD xiulianlevel1; 	/// 能力修炼等级
	DWORD xiulianlevel2; 	/// 能力修炼等级
	DWORD xiulianlevel3; 	/// 能力修炼等级
	DWORD xiulianlevel4; 	/// 能力修炼等级
	DWORD xiulianlevel5; 	/// 能力修炼等级

	DWORD zhancheid; 	/// 领用战车id
	DWORD zhanchetype; 	/// 领用战车类型

	DWORD zhanchetime1;          /// 战车time1时间
	DWORD zhanchetime2;          /// 战车time2时间
	DWORD zhanchetime3;          /// 战车time3时间

	DWORD zuoqitujianlevel;          /// 坐骑图鉴等级
	DWORD zuoqitujianexp;          /// 坐骑图鉴经验

	DWORD mohelevel;          /// 魔盒等级
	DWORD moheexp;          /// 魔盒经验
	
	DWORD shizhuang_select;  //当前选择时装
	DWORD pifeng_select;     //当前选择披风
	DWORD chibang_select;     //当前选择披风
	DWORD zuoqi_select;     //当前选择坐骑



	DWORD tianjiangtime;  //天降鸿福时间

	DWORD migongcs; 	/// 迷宫次数



	DWORD tianxiacount;          //天下第一称号判定 


	//这里是角色的一些常用变量  也和数据库charbase表是对应的 

	DWORD duanweiexp;//这里 加一个变量 叫 段位经验 用来存玩家 吃了多少个经验丹 后面根据这个值 计算等级 数据库也要加
	DWORD duanweilingqu;//这里 加一个变量 叫 段位领取 用来存玩家 领取了多少次段位奖励 后面根据这个值 计算等级 数据库也要加







	char jingcai[1564+1];   	/// sky 一键换装
	char chenghao[1564+1];   	/// sky 称号
	char shizhuang[5120+1];   	/// sky 时装魔盒
	char pifeng[5120+1];   	/// sky 时装魔盒
	char chibang[5120+1];   	/// sky 时装魔盒
	char zuoqi[5120+1];   	/// sky 时装魔盒
	char KeyEquip[5120+1];   	/// sky 一键换装
	DWORD m13axhp;			   //MYY 百兽图鉴 最大生命值
	DWORD p13damage;		   //MYY 百兽图鉴 最小攻击力
	DWORD m13axpdamage;		   //MYY 百兽图鉴 最大攻击力
	DWORD m13damage;		   //MYY 百兽图鉴 最小法术攻击力
	DWORD m13axmdamage;		   //MYY 百兽图鉴 最大法术攻击力
	DWORD p13defence;		   //MYY 百兽图鉴 物防
	DWORD m13defence;		   //MYY 百兽图鉴 魔防
};

struct CharState
{
	DWORD maxhp;						/// 最大生命值 *
	DWORD resumehp;						/// 生命值恢复 *
	DWORD maxmp;						/// 最大法术值 *
	DWORD resumemp;						/// 法术值恢复 *
	DWORD maxsp;                     /// 最大体力值 *
	DWORD resumesp;                  /// 体力恢复值 *

	DWORD pdamage;                   /// 最小物理攻击力 *
	DWORD maxpdamage;					/// 最大物理攻击力 *
	DWORD mdamage;						/// 最小法术攻击力 *
	DWORD maxmdamage;					/// 最大法术攻击力 *
	DWORD pdefence;						/// 物理防御力 *
	DWORD mdefence;						/// 法术防御力 *
	uint64_t nextexp;				    ///sky 升级经验值 *
	WORD  attackspeed;					/// 攻击速度 *
	WORD  movespeed;					/// 移动速度 *
	SWORD  attackrating;				/// 攻击命中 *
	SWORD  attackdodge;					/// 攻击躲避 *
	WORD  bang;							/// 重击 *
	WORD  lucky;						/// 幸运值 *
	WORD  charm;                     /// 魅力值 *
	BYTE  attackfive;					/// 攻击五行 *
	BYTE  defencefive;					/// 防御五行 *
	union {
		struct {
			WORD wdCon;	//体质
			WORD wdStr;	//体力
			WORD wdDex;	//敏捷
			WORD wdInt;	//智力
			WORD wdMen;	//精神
		};
		WORD wdProperty[5];
	};

	DWORD stdpdamage;					/// 标准物理攻击力
	DWORD stdmdamage;					/// 标准法术攻击力
	DWORD stdpdefence;					/// 标准物理防御力
	DWORD stdmdefence;					/// 标准法术防御力
	WORD  stdbang;						/// 标准重击率
};
#ifdef _TEST_DATA_LOG
struct CharTest
{
	DWORD upgrade_time;//升级时间
	DWORD upgrade_usetime;//本级使用时间
	DWORD death_times;//死亡次数
	DWORD hp_leechdom;//使用生命值药品个数
	DWORD mp_leechdom;//使用法术值药品个数
	DWORD sp_leechdom;//使用体力值药品个数
	DWORD get_money;//得到银子数
	DWORD get_heigh;//得到高级装备个数
	DWORD get_socket;//得到带孔装备个数
	DWORD get_material;//得到原料个数
	DWORD get_stone;//得到宝石个数
	DWORD get_scroll;//得到卷轴个数
	DWORD money;//当前银子数
};
#endif


struct CharSave
{
	CharBase 	charbase;
	DWORD		dataSize;
	char	 	data[0];
};

#define MAX_TEMPARCHIVE_SIZE 0x10000
struct TempArchiveMember
{
	DWORD type;
	DWORD size;
	char data[0];
};
enum TempArchiveType
{
	TEAM,	//队伍
	ENTRY_STATE,	//队伍
	PET,	//宠物
	SAFETY_STATE,	// 临时关闭密码保护
};
struct BinaryArchiveMember
{
	DWORD type;
	DWORD size;
	char data[0];
};
enum BinaryArchiveType
{
	BINARY_DOUBLE_EXP_OBJ,	///双倍经验道具
	BINARY_TONG_OBJ,		///帮会令牌类道具
	BINARY_KING_OBJ,		///国王令牌类道具
	BINARY_FAMILY_OBJ,		///家族令牌类道具
	BINARY_CHANGE_COUNTRY_TIME, /// 最后一次叛国的时间
	BINARY_ADD_COUNTRY_TIME, /// 最后一次领取工资的时间
	BINARY_SAFETY,			/// 是否进行财产保护
	BINARY_GIVE_MATARIAL_NUM,   //道具卡兑换材料的组数
	BINARY_CARD_NUM,            //道具卡张数
	BINARY_SAFETY_SETUP,	/// 保护设置
    BINARY_COUNTER_MANAGER,  ///soke 计数器管理器
	BINARY_MAX,				///最大数值(占位用)
};

#ifndef _SEX_
#define _SEX_
enum
{
	MALE	= 0,
	FEMALE	= 1
};
#endif

#ifndef _PROFESSION_
#define _PROFESSION_
enum
{
	PROFESSION_NONE	= 0,	//无业
	PROFESSION_1	= 1,	//侠客
	PROFESSION_2	= 2,	//侠女
	PROFESSION_3	= 4,	//箭侠
	PROFESSION_4	= 8,	//箭灵
	PROFESSION_5	= 16,	//天师
	PROFESSION_6	= 32,	//美女
	PROFESSION_7	= 64,	//法师
	PROFESSION_8	= 128	//仙女
};
#endif


const WORD  MAX_LEVEL			= 160;					/// 最大角色等级
const DWORD MAX_GOODNESS		= 9999;					/// 最大善恶度
const DWORD MAX_HP			= 999999;				/// 最大生命值
const DWORD MAX_RESUMEHP		= 1000;					/// 生命值恢复
const DWORD MAX_MP			= 999999;				/// 最大法术值
const DWORD MAX_RESUMEMP		= 1000;					/// 法术值恢复
const DWORD MAX_SP			= 999999;				/// 最大体力值
const DWORD MAX_RESUMESP		= 1000;					/// 体力值恢复
const DWORD MAX_PDAMAGE			= 999999;				/// 最大物理攻击力
const DWORD MAX_RDAMAGE			= 999999;				/// 最大远程攻击力
const DWORD MAX_TDAMAGE 		= 999999;				/// 最大仙术攻击力
const DWORD MAX_MDAMAGE			= 999999;				/// 最大法术攻击力
const DWORD MAX_PDEFENCE		= 999999;				/// 最大物理防御力
const DWORD MAX_MDEFENCE		= 999999;				/// 最大法术防御力
const QWORD MAX_EXP			= 9999999999LL;			/// 最大经验值
const WORD  MAX_ATTACKSPEED		= 100;					/// 攻击速度
const WORD  MAX_MAGICSPEED		= 100;					/// 施法速度
const WORD  MAX_MOVESPEED		= 100;					/// 移动速度
const WORD  MAX_ATTACKRATING	= 999;				    /// 攻击命中
const WORD  MAX_ATTACKDODGE		= 999;					/// 攻击躲避
const WORD  MAX_MAGICRATING		= 999;					/// 法术命中
const WORD  MAX_MAGICDODGE		= 999;					/// 法术躲避
const WORD  MAX_LUCKY			= 999;					/// 幸运值
const WORD  MAX_METAL			= 999;					/// 金
const WORD  MAX_WOOD			= 999;					/// 木
const WORD  MAX_WATER			= 999;					/// 水
const WORD  MAX_FIRE			= 999;					/// 火
const WORD  MAX_SOIL			= 999;					/// 土
const WORD  MAX_METALRESISTANCE		= 10000;				/// 金系抵抗
const WORD  MAX_WOODRESISTANCE		= 10000;				/// 木系抵抗
const WORD  MAX_WATERRESISTANCE		= 10000;				/// 水系抵抗
const WORD  MAX_FIRERESISTANCE		= 10000;				/// 火系抵抗
const WORD  MAX_SOILRESISTANCE		= 10000;				/// 土系抵抗
const WORD  MAX_POINTS			= 1500;					/// 点数

#pragma pack()

#endif

