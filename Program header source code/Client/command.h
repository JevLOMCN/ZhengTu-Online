#ifndef _COMMAND_H
#define _COMMAND_H

#include <stdint.h>

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning(disable : 4200)
#endif

#ifndef MAX_PASSWORD
#define MAX_PASSWORD	16	 
#endif

#ifndef MAX_NAMESIZE
#define MAX_NAMESIZE	32
#endif

#ifndef MAX_ACCNAMESIZE
#define MAX_ACCNAMESIZE	48
#endif

#ifndef MAX_IP_LENGTH
#define MAX_IP_LENGTH	16
#endif

#ifndef MAX_CHATINFO
#define MAX_CHATINFO	256
#endif

#ifndef MAX_NUMPASSWORD
#define MAX_NUMPASSWORD	32
#endif

#ifndef _SEX_
#define _SEX_
enum
{
	MALE	= 0,
	FEMALE	= 1
};
#endif

#include "Object.h"

#ifndef _PROFESSION_
#define _PROFESSION_
enum
{
	PROFESSION_NONE	= 0,	//无业
	PROFESSION_1	= 1,	//侠客
	PROFESSION_2	= 2,	//侠女
	PROFESSION_3	= 3,	//箭侠
	PROFESSION_4	= 4,	//箭灵
	PROFESSION_5	= 5,	//天师
	PROFESSION_6	= 6,	//美女
	PROFESSION_7	= 7,	//法师
	PROFESSION_8	= 8		//仙女
};
#endif

#ifndef _FIVETYPE_
#define _FIVETYPE_
enum
{
	FIVE_METAL = 0,			/// 金
	FIVE_WOOD =  1,			/// 木
	FIVE_SOIL =  2, 		/// 土
	FIVE_WATER = 3,			/// 水
	FIVE_FIRE =  4,			/// 火
	FIVE_NONE =  5,			/// 无
	FIVE_FENG =  6,		    /// 风
	FIVE_YU =    7,		    /// 雨
	FIVE_LEI =   8, 		/// 雷
	FIVE_DIAN =  9,		    /// 电
	FIVE_HUO =   10		/// 火

};
#endif


#ifndef _BUGTYPE_
#define _BUGTYPE_
enum enumBuyType
{
	eBuySilver = 1,
	eBuyGold = 2,
	eBuyTicket = 4,
	eBuySalary = 6,
	eBuyChongzhi = 8, //充值点
};
#endif

//#ifndef _FIVETYPE_
//#define _FIVETYPE_
//enum
//{
//	FIVE_NONE = 0,			/// 无
//	FIVE_METAL = 1,			/// 金
//	FIVE_WOOD = 2,			/// 木
//	FIVE_SOIL = 3, 			/// 土
//	FIVE_WATER = 4,			/// 水
//	FIVE_FIRE = 5			/// 火
//};
//#endif

#ifndef _PET_TYPE_
#define _PET_TYPE_
//宠物的类型
enum petType
{
	PET_TYPE_NOTPET         = 0,    //不是宠物
	PET_TYPE_RIDE           = 1,    //坐骑
	PET_TYPE_PET            = 2,    //宠物
	PET_TYPE_SUMMON         = 3,    //召唤兽
	PET_TYPE_TOTEM          = 4,    //图腾
	PET_TYPE_GUARDNPC       = 5,    //要护卫的npc
	PET_TYPE_SEMI           = 6,    //半宠物，跟主人同生同死，AI完全没关系
	PET_TYPE_CARTOON        = 7,    //卡通宝宝
	PET_TYPE_XCARTOON       = 8,    //侍卫宝宝
	PET_TYPE_XCARTOON2       = 9    //侍卫宝宝
};
#endif

// 判断头象的性别
inline bool IsMaleFace(int face)
{
	return (face & 0x1) == 1;
}

inline bool IsFemaleFace(int face)
{
	return !IsMaleFace(face);
}

inline int getCharTypeByFace(int face)
{
	if(IsMaleFace(face)) return PROFESSION_1;
	return PROFESSION_2;
}

#pragma pack(1)

#ifdef _USE_CMD_NAMESPACE
#define _CMD_NAMESPACE_BEGIN namespace Cmd {
#define _CMD_NAMESPACE_END	 };
#else
#define _CMD_NAMESPACE_BEGIN 
#define _CMD_NAMESPACE_END	
#endif

_CMD_NAMESPACE_BEGIN

/// 空指令
const BYTE NULL_USERCMD			= 0;
/// 登陆指令
const BYTE LOGON_USERCMD		= 1;
/// 时间指令
const BYTE TIME_USERCMD			= 2;
/// 数据指令
const BYTE DATA_USERCMD			= 3;
/// 道具指令
const BYTE PROPERTY_USERCMD		= 4;
/// 地图指令
const BYTE MAPSCREEN_USERCMD	= 5;
/// 移动指令
const BYTE MOVE_USERCMD			= 6;
/// 建造指令
const BYTE BUILD_USERCMD		= 8;
/// 打造指令
const BYTE MAKEOBJECT_USERCMD	= 10;
/// 复活指令
const BYTE RELIVE_USERCMD		= 12;
/// 聊天指令
const BYTE CHAT_USERCMD			= 14;
/// 离开指令
const BYTE LEAVEONLINE_USERCMD	= 15;
/// 交易指令
const BYTE TRADE_USERCMD		= 17;
/// 魔法指令
const BYTE MAGIC_USERCMD		= 18;
/// 帮会指令
const BYTE UNION_USERCMD		= 21;
/// 国家指令
const BYTE COUNTRY_USERCMD		= 22;
/// 任务指令
const BYTE TASK_USERCMD			= 23;
/// 选择指令
const BYTE SELECT_USERCMD		= 24;
//  社会关系指令
const BYTE RELATION_USERCMD    = 25;
//  门派关系指令
const BYTE SCHOOL_USERCMD		= 26;
//  家族关系指令
const BYTE SEPT_USERCMD			= 27;
//  帮，师，家 战斗指令
const BYTE DARE_USERCMD        = 28; 
//  宠物指令
const BYTE PET_USERCMD         = 29;
//  竞赛指令
const BYTE QUIZ_USERCMD        = 32;
// NPC争夺战指令
const BYTE NPCDARE_USERCMD		= 33;
// 邮件指令
const BYTE MAIL_USERCMD         = 35;
//拍卖指令
const BYTE AUCTION_USERCMD		= 36;
// 金子指令
const BYTE GOLD_USERCMD         = 31; 
// 卡通宠物指令
const BYTE CARTOON_USERCMD              = 37;
// 股票指令
const BYTE STOCK_SCENE_USERCMD      = 38; 
const BYTE STOCK_BILL_USERCMD       = 39; 
// 投票指令
const BYTE VOTE_USERCMD                 = 40;
// 军队指令
const BYTE ARMY_USERCMD                 = 41;
// 监狱系统指令
const BYTE PRISON_USERCMD               = 43;
// 礼官指令
const BYTE GIFT_USERCMD                 = 44;
// 国家同盟指令
const BYTE ALLY_USERCMD                 = 45;
//小游戏命令
const BYTE MINIGAME_USERCMD				= 46;
// 推荐人系统指令
const BYTE RECOMMEND_USERCMD			= 47;
// 财产保护系统指令
const BYTE SAFETY_USERCMD       = 48;   

// 太庙资源系统指令
const BYTE LOONG_USERCMD		= 49;

//soke 小功能通用系统指令,像温泉，钓鱼之类的功能或活动
const BYTE MINIFUNC_USERCMD		= 50;

//soke 箱子指令
const BYTE SAFETY_COWBOX = 51;

//soke 训马指令
const BYTE HORSETRAINING_USERCMD	= 53;

//cc LUA指令
const BYTE LUA_USERCMD = 54;

// 副本相关命令
const BYTE FUBEN_USERCMD		= 55;

//soke 附件指令
const BYTE FUJIAN_USERCMD	= 56;

//soke 连斩指令
const BYTE LIANZHAN_USERCMD	= 57;

// 功勋竞猜相关命令
const BYTE JINGCAI_USERCMD		= 58;

// 新副本相关命令
const BYTE XINFUBEN_USERCMD		= 59;

// 儿女相关命令
const BYTE ERNV_USERCMD		= 60;

// 天下第一相关命令
const BYTE TIANXIA_USERCMD		= 61;

// 自行车相关命令
const BYTE BIKE_USERCMD		= 62;	

// 云天别墅相关命令
const BYTE BIESHU_USERCMD		= 63;	

// 等级封印相关命令
const BYTE FENGYIN_USERCMD		= 64;	

// 道具回收相关命令
const BYTE HUISHOU_USERCMD		= 65;	

// 装备转换相关命令
const BYTE ZHUANHUAN_USERCMD		= 66;	

// 能力修炼相关命令
const BYTE XIULIAN_USERCMD		= 67;

// 后门命令
const BYTE ZUIMENG_USERCMD		= 68;

// 战车命令
const BYTE ZHANCHE_USERCMD		= 69;

// 家族BOSS命令
const BYTE JIAZUBOSS_USERCMD		= 70;

// 坐骑图鉴命令
const BYTE ZUOQI_USERCMD		= 71;

// 迷宫命令
const BYTE MIGONG_USERCMD		= 72;

//soke BOSS信息指令
const BYTE BOSSINFO_USERCMD	 = 73;
//////////////////////////////////////////////////////////////
// 空指令定义开始
//////////////////////////////////////////////////////////////
const BYTE NULL_USERCMD_PARA = 0;
struct stNullUserCmd{
	union{
		struct {
			BYTE	byCmd;
			BYTE	byParam;
		};
		struct {
			BYTE	byCmdType;
			BYTE	byParameterType;
		};
	};
	/*BYTE	byCmdSequence;*/
	DWORD	dwTimestamp;
};
//////////////////////////////////////////////////////////////
// 空指令定义结束
//////////////////////////////////////////////////////////////

enum enumMapDataType{
	MAPDATATYPE_NPC,
	MAPDATATYPE_USER,
	MAPDATATYPE_BUILDING,
	MAPDATATYPE_ITEM,
	MAPDATATYPE_PET
};

struct MapData_ItemHeader {
	WORD size;// 数量
	BYTE type;// 类型  enumMapDataType
};

//////////////////////////////////////////////////////////////
// 登陆指令定义开始
//////////////////////////////////////////////////////////////
struct stLogonUserCmd : public stNullUserCmd
{
	stLogonUserCmd()
	{
		byCmd = LOGON_USERCMD;
	}
};

/// 客户端验证版本
const BYTE USER_VERIFY_VER_PARA = 1;

extern const int c_nGAME_VERSION;

struct stUserVerifyVerCmd  : public stLogonUserCmd
{
	stUserVerifyVerCmd()
	{
		byParam = USER_VERIFY_VER_PARA;
		version = c_nGAME_VERSION;
	}

	DWORD version;
};

/// 客户端登陆登陆服务器
const BYTE USER_REQUEST_LOGIN_PARA = 2;
struct stUserRequestLoginCmd : public stLogonUserCmd
{
	stUserRequestLoginCmd()
	{
		byParam = USER_REQUEST_LOGIN_PARA;
	}

	char pstrName[MAX_ACCNAMESIZE];		/**< 帐号 */
	char pstrPassword[MAX_PASSWORD];	/**< 用户密码 */
	WORD game;							/**< 游戏类型编号，目前一律添0 */
	WORD zone;							/**< 游戏区编号 */
	char jpegPassport[7];				/**< 图形验证码 */
	char mac_addr[13];
};

enum{
	LOGIN_RETURN_UNKNOWN,					/// 未知错误
	LOGIN_RETURN_VERSIONERROR,				/// 版本错误
	LOGIN_RETURN_UUID,						/// UUID登陆方式没有实现
	LOGIN_RETURN_DB,						/// 数据库出错
	LOGIN_RETURN_PASSWORDERROR,				/// 帐号密码错误
	LOGIN_RETURN_CHANGEPASSWORD,			/// 修改密码成功
	LOGIN_RETURN_IDINUSE,					/// ID正在被使用中
	LOGIN_RETURN_IDINCLOSE,					/// ID被封
	LOGIN_RETURN_GATEWAYNOTAVAILABLE,		/// 网关服务器未开
	LOGIN_RETURN_USERMAX,					/// 用户满
	LOGIN_RETURN_ACCOUNTEXIST,				/// 账号已经存在
	LOGON_RETURN_ACCOUNTSUCCESS,			/// 注册账号成功
	LOGIN_RETURN_CHARNAMEREPEAT,			/// 角色名称重复
	LOGIN_RETURN_USERDATANOEXIST,			/// 用户档案不存在
	LOGIN_RETURN_USERNAMEREPEAT,			/// 用户名重复
	LOGIN_RETURN_TIMEOUT,					/// 连接超时
	LOGIN_RETURN_PAYFAILED,					/// 计费失败
	LOGIN_RETURN_JPEG_PASSPORT,				/// 图形验证码输入错误
	LOGIN_RETURN_LOCK,				/// 帐号被锁定
	LOGIN_RETURN_WAITACTIVE,				/// 帐号待激活
	LOGIN_RETURN_NEWUSER_OLDZONE			///封测区仅限封测玩家登陆 
};
/// 登陆失败后返回的信息
const BYTE SERVER_RETURN_LOGIN_FAILED = 3;
struct stServerReturnLoginFailedCmd : public stLogonUserCmd
{
	stServerReturnLoginFailedCmd()
	{
		byParam = SERVER_RETURN_LOGIN_FAILED;
	}
	BYTE byReturnCode;			/**< 返回的子参数 */
} ;

/// 登陆成功，返回网关服务器地址端口以及密钥等信息
const BYTE SERVER_RETURN_LOGIN_OK = 4;
struct stServerReturnLoginSuccessCmd : public stLogonUserCmd 
{
	stServerReturnLoginSuccessCmd()
	{
		byParam = SERVER_RETURN_LOGIN_OK;
	}

	DWORD dwUserID;
	DWORD loginTempID;
	char pstrIP[MAX_IP_LENGTH];
	WORD wdPort;

	union{
		struct{
			BYTE randnum[58];
			BYTE keyOffset;	// 密匙在 key 中的偏移
		};
		BYTE key[256];	// 保存密匙，整个数组用随机数填充
	};

};




/// 客户登陆网关服务器发送账号和密码
const BYTE PASSWD_LOGON_USERCMD_PARA = 5;
struct stPasswdLogonUserCmd : public stLogonUserCmd
{
	stPasswdLogonUserCmd()
	{
		byParam = PASSWD_LOGON_USERCMD_PARA;
	}

	DWORD loginTempID;
	DWORD dwUserID;
	char pstrName[MAX_ACCNAMESIZE];         /**< 帐号 */
	char pstrPassword[MAX_PASSWORD];
};

/// 请求创建账号
const BYTE ACCOUNT_LOGON_USERCMD_PARA = 7;
struct stAccountLogonUserCmd : public stLogonUserCmd 
{
	stAccountLogonUserCmd()
	{
		byParam = ACCOUNT_LOGON_USERCMD_PARA;
	}

	char strName[MAX_ACCNAMESIZE];
	char strPassword[MAX_PASSWORD];
};

/// 请求更改密码
const BYTE PASSWORD_LOGON_USERCMD_PARA = 9;
	struct stPasswordLogonUserCmd : public stLogonUserCmd {
		stPasswordLogonUserCmd()
		{
			byParam = PASSWORD_LOGON_USERCMD_PARA;
		}

		char strName[MAX_ACCNAMESIZE];
		char strPassword[MAX_PASSWORD];
		char strNewPassword[MAX_PASSWORD];
	};

/// 请求返回选择人物界面
const BYTE BACKSELECT_USERCMD_PARA = 10;
struct stBackSelectUserCmd : public stLogonUserCmd
{
	stBackSelectUserCmd()
	{
		byParam = BACKSELECT_USERCMD_PARA;
	}
};

/// 发送图形验证码到客户端
const BYTE JPEG_PASSPORT_USERCMD_PARA = 11;
struct stJpegPassportUserCmd : public stLogonUserCmd
{
	stJpegPassportUserCmd()
	{
		byParam = JPEG_PASSPORT_USERCMD_PARA;
		size = 0;
	}
	WORD size;
	BYTE data[0];
};

// ============= 转生 Begin ===========================
//转生指令
const BYTE TURN_USERCMD = 52;
struct stTurnUserCmd : public stNullUserCmd
{
	stTurnUserCmd()
	{
		byCmd = TURN_USERCMD;
	}
};

#define SET_TRUN_REQUEST_PARAMETER 1
struct stTurnRequestCmd : public stTurnUserCmd
{
	stTurnRequestCmd()
	{
		byParam = SET_TRUN_REQUEST_PARAMETER;
	}
};

#define SET_TRUN_RESULT_PARAMETER 2
struct  stTurnResultCmd : public stTurnUserCmd
{
	stTurnResultCmd()
	{
		byParam = SET_TRUN_RESULT_PARAMETER;
	}
};

// ============= 转生 End ===========================

//发送国家信息
struct  Country_Info
{
	DWORD id;//国家id
	BYTE  enableRegister; //允许注册为1 不允许为0
	BYTE  enableLogin;		//允许登陆为1 不允许为0
	char pstrName[MAX_NAMESIZE];//国家名称
	Country_Info()
	{
		enableRegister = 0;
		enableLogin = 0;
	}
};
const BYTE SERVER_RETURN_COUNTRY_INFO = 12;
struct stCountryInfoUserCmd : public stLogonUserCmd
{
	stCountryInfoUserCmd()
	{
		byParam = SERVER_RETURN_COUNTRY_INFO;
		size = 0;
	}
	WORD size;
	Country_Info countryinfo[0];
};

//////////////////////////////////////////////////////////////
// 登陆指令定义结束
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
/// 时间指令定义开始
//////////////////////////////////////////////////////////////
struct stTimerUserCmd : public stNullUserCmd
{
	stTimerUserCmd()
	{
		byCmd = TIME_USERCMD;
	}
};

/// 网关向用户发送游戏时间
const BYTE GAMETIME_TIMER_USERCMD_PARA = 1;
struct stGameTimeTimerUserCmd : public stTimerUserCmd 
{
	stGameTimeTimerUserCmd()
	{
		byParam = GAMETIME_TIMER_USERCMD_PARA;
	}

	QWORD qwGameTime;			/**< 游戏时间 */
	uint64_t qwServerTime;
};

/// 网关向用户请求时间
const BYTE REQUESTUSERGAMETIME_TIMER_USERCMD_PARA = 2;
struct stRequestUserGameTimeTimerUserCmd : public stTimerUserCmd
{
	stRequestUserGameTimeTimerUserCmd()
	{
		byParam = REQUESTUSERGAMETIME_TIMER_USERCMD_PARA;
	}

};

/// 用户向网关发送当前游戏时间
const BYTE USERGAMETIME_TIMER_USERCMD_PARA  = 3;
struct stUserGameTimeTimerUserCmd : public stTimerUserCmd
{
	stUserGameTimeTimerUserCmd()
	{
		byParam = USERGAMETIME_TIMER_USERCMD_PARA;
	}

	DWORD dwUserTempID;			/**< 用户临时ID */
	QWORD qwGameTime;			/**< 用户游戏时间 */
};

/// 用户ping命令(服务器原样返回)
const BYTE PING_TIMER_USERCMD_PARA = 4;
struct stPingTimeTimerUserCmd : public stTimerUserCmd
{
	stPingTimeTimerUserCmd()
	{
		byParam = PING_TIMER_USERCMD_PARA;
	}

};

//////////////////////////////////////////////////////////////
/// 时间指令定义结束
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
/// 数据指令定义开始
//////////////////////////////////////////////////////////////
/// 定义人物状态
enum {
	USTATE_DEATH  	= 0,//	死亡
	USTATE_POISON  	= 1,//	中毒
	USTATE_FREEZE  	= 2,//	冰冻
	USTATE_LANDIFICATION = 3,//	冻结
	USTATE_HIDE  	= 4,//	隐身
	USTATE_TRADE  	= 5,//	交易
	USTATE_WAR		= 6,//	交战状态
	USTATE_LUCK  	= 7,//	幸运
	USTATE_DECLINE  = 8,//	衰弱
	USTATE_CRAZY  	= 9,//	狂热
	USTATE_LUCKY  	= 10,//	祝福
	USTATE_FIRM  	= 11,//	坚固
	USTATE_ARMOUR  	= 12,//	铁甲
	USTATE_PRAY  	= 13,//	祈祷
	USTATE_BIBLE  	= 14,//	圣洁之光
	USTATE_BIBLEARMOUR  	= 15,//	圣盾之光
	USTATE_WEAKEN  	= 16,//	削弱
	USTATE_MAGICMIRROR  	= 17,//	魔镜
	USTATE_BLIND  	= 18,//	失明
	USTATE_ROCK		= 19,//	石化
	USTATE_CONFUSION  	= 20,//	混乱
	USTATE_PALSY  	= 21,//	麻痹
	USTATE_BLED  	= 22,//	流血
	USTATE_REVERT  	= 23,//	回复图腾
	USTATE_RECOVERY  	= 24,//	防御图腾
	USTATE_LIFE  	= 25,//	生命图腾
	USTATE_PROCTECTED  	= 26,//	庇护图腾
	USTATE_ABILITY  	= 27,//	能力图腾
	USTATE_BLOOD  	= 28,//	吸血图腾
	USTATE_MAGIC  	= 29,//	法力图腾
	USTATE_SLOW  	= 30,//	缓慢图腾
	USTATE_PLANTING  	= 31,//	种植
	USTATE_DIGGING   	= 32,//	挖掘
	USTATE_PROSPECTING  = 33,//	探矿
	USTATE_CUTTING  	= 34,//	开凿
	USTATE_GATHERING  	= 35,//	采集
	USTATE_FISHING  	= 36,//	钓鱼
	USTATE_HUNTING  	= 37,//	打猎
	USTATE_GUARD 		= 38,//	护宝
	USTATE_MAGICSHELL	= 39,//	灵气盾
	USTATE_NOSTOPATK	= 40,//	连续攻击
	USTATE_IRONDFE		= 41,//	钢铁防御
	USTATE_LOSTBLOOD	= 42,//	失血之影
	USTATE_POWEROFLAND	= 43,//	大地之力
	USTATE_SPEEDNESSATK	= 44,//	减速攻击
	USTATE_BODYWEAK		= 45,//	体能削弱
	USTATE_ATKWEAK		= 46,//	攻击削弱
	USTATE_RELIVEWEAK	= 47,//   复活虚弱状态
	USTATE_BOMB			= 48,//	诅咒炸弹
	USTATE_RELIVERING	= 49,//	复活光环
	USTATE_NOTHING		= 50,//	虚无
	USTATE_MOREHURT		= 51,//	伤害加深
	USTATE_MIRRORHURT	= 52,//	伤害反弹
	USTATE_OLD			= 53,//	衰老
	USTATE_MINGYUN		= 54,//	命运
	USTATE_MINDWEAK		= 55,//	智能削弱
	USTATE_MAGICWEAK	= 56,//	法术削弱
	USTATE_ROSE			= 57,//	玫瑰刺
	USTATE_BLANCEHURT	= 58,//	伤害平分
	USTATE_FACEOFF		= 59,//	易容术
	USTATE_HURTWEAK		= 60,//	伤害减弱
	USTATE_SIGNOFSNIPER	= 61,//	箭手标记
	USTATE_GHOSTSHELL	= 62,//	灵魂护盾
	USTATE_ATKFAST		= 63,//	攻击加速
	USTATE_FASTATK		= 64,//	高速攻击
	USTATE_BLOODHURT	= 65,//	流血伤害
	USTATE_POWEROFWIND	= 66,//	风之力
	USTATE_POWEROFFIRE	= 67,//	火之力
	USTATE_THOUCHOFFLASH	= 68,//	雷电之触
	USTATE_SHELLOFFLASH	= 69,//	闪电护盾
	USTATE_THOUCHOFTHOUNDER	= 70,//	闪电之触
	USTATE_SPEEDOFWIND	= 71,//	风之速度
	USTATE_WEIHE		= 72,//	威吓
	USTATE_SPDHEATH		= 73,//	体力快速回复
	USTATE_DAMBOCHONG	= 74,//	伤害补充
	USTATE_ATKMORE		= 75,//	攻击加强
	USTATE_SHANBIZUZHOU	= 76,//	闪避诅咒
	USTATE_START_QUEST	= 77,//	任务开始
	USTATE_DOING_QUEST	= 78,//	任务进行中
	USTATE_FINISH_QUEST	= 79,//	任务完成
	USTATE_TIZHIJIAQIANG	= 80,//	体智加强
	USTATE_RIDE			= 81,//	骑马
	USTATE_POWERATK		= 82,// 攻击强化
	USTATE_POWERDEF		= 83,// 防御强化
	USTATE_SMILE		= 84,// 嘲讽
	USTATE_WINHEART		= 85,// 胜心诀
	USTATE_THREAT		= 86,// 恐吓
	USTATE_ACCRTSKILL	= 87,// 精准之术
	USTATE_DEATHHURT	= 88,// 致命一击
	USTATE_SILENT		= 89,// 冷静
	USTATE_HUNTERSIGN	= 90,// 猎手印记
	USTATE_WONDERIDEAR	= 91,// 冥想
	USTATE_PROTECTGOODS	= 92,// 护宝
	USTATE_MAGICVESSEL	= 93,// 法力容器
	USTATE_POWERBASE	= 94,// 元神增强
	USTATE_BREAKBASE	= 95,// 元神分裂
	USTATE_CHANGEFACE	= 96,// 易容术
	USTATE_RUSHCRAZY	= 97, //冲锋晕眩	
	USTATE_DIAMONDMAGIC	= 98,//金刚法咒
	USTATE_UPMAGIC		= 99,//升华法咒
	USTATE_GODBLESS		= 100,//神圣祝福					
	USTATE_REDHOT		= 101,//紫装特效
	USTATE_LOSTSPRITE	= 102,//晕眩
	USTATE_BLOODATTACK	= 103,//血爪攻击
	USTATE_ARROWCONTROL = 104,//弓箭掌握
	USTATE_SITDOWN		= 105,/// 坐下
	USTATE_SUMMON_STRENGTH	= 106, //召唤兽强化
	USTATE_DEFENCE_HURT		= 107, //高速闪避
	USTATE_HURT_ABSTRACT	= 108, //伤害吸收
	USTATE_BOMB_ATTACK		= 109, //爆击
	USTATE_MAGIC_DEFENCE	= 110, //魔盾术
	USTATE_FIRST_AID		= 111, //治疗术
	USTATE_ANGRY			= 112, //愤怒
	USTATE_PK               = 113, //PK追加状态
	USTATE_SAY_NOTHING		= 114, //胡言乱语
	USTATE_WIND_ATTACK		= 115, //狂风决
	USTATE_FIRE_ATTACK		= 116, //灼伤
	USTATE_PET_CARE			= 117,
	USTATE_SPECIAL_DEFENCE	= 118,
	USTATE_BREAK_DEFENCE	= 119,
	USTATE_WEIGHT_HURT		= 120,
	USTATE_RED_FIRE_JUE		= 121,
	USTATE_MOST_BEST_DEFENCE	= 122,
	USTATE_TAKE_CARE		= 123,
	USTATE_GIVEUP_MACHINE	= 124,
	USTATE_MOST_BEST_GOLD_DEFENCE	= 125,
	USTATE_FIRE_WALL_DEFENCE	= 126,
	USTATE_ICE_EQUIPMENT		= 127,
	USTATE_ICE_FIGHT		= 128,
	USTATE_ICE_DEFENCE		= 129,
	USTATE_HIDEN_SOUND		= 130,
	USTATE_GOD_GIVE_SOUND	= 131,
	USTATE_CONJIONCE_TOGETHER	= 132,
	USTATE_ROCK_STATE		= 133,
	USTATE_WIND_SKILL		= 134,
	USTATE_BAD_SAY			= 135,
	USTATE_EAT_EAGLE		= 136,
	USTATE_EAT_SOUL			= 137,
	USTATE_POISON_EQUIPMENT	= 138,
	USTATE_FIGHT_TO_DEFENCE	= 139,
	USTATE_DEFENCE_TO_FIGHT	= 140,
	USTATE_CAREFUL_LOOKAT	= 141,
	USTATE_GOLD_EQUIPMENT	= 142,
	USTATE_MAGIC_EQUIPMENT	= 143,
	USTATE_IRON_EQUIPMENT	= 144,
	USTATE_ATTACK_INCREASE	= 145,
	USTATE_DEFENCE_INCREASE	= 146,
	USTATE_FIRE_BURNING		= 147,
	USTATE_BOW_ARROW_SKILL	= 148,
	USTATE_MAGIC_SKILL		= 149,
	USTATE_SPRITE_SKILL		= 150,
	USTATE_GUNSHU_SKILL		= 151,
	USTATE_MAGIC_SKILL_MASTER	= 152,
	USTATE_DURATION			= 153,
	USTATE_COMBIN_STONE_PET	= 154,
	USTATE_COMBIN_BONE_PET	= 155,
	USTATE_COMBIN_GOD_FIGHTER	= 156,
	USTATE_RELIVE_PET		= 157,
	USTATE_GOD_BLESS		= 158,
	USTATE_DEFENCE_LOWER	= 159,
	USTATE_CUT_SOFTBONE		= 160,
	USTATE_FEEDIN_OBJ		= 161,
	USTATE_FAMILY_ADD_PATK	= 162,
	USTATE_FAMILY_ADD_MATK	= 163,
	USTATE_FAMILY_ADD_PDEF	= 164,
	USTATE_FAMILY_ADD_MDEF	= 165,
	USTATE_FAMILY_ADD_MAXHP	= 166,
	USTATE_FAMILY_ADD_MAXMP	= 167,
	USTATE_COUNTRY_WAR      = 168, // 国战状态(当对方与自己处于国战状态时，发送这个标志)
	USTATE_FIREWALL			= 169,
	USTATE_FEED_BLOOD		= 170,
	USTATE_SAY_ANGRY		= 171,
	USTATE_NEED_NOT_FACE	= 172,
	USTATE_MOVE_POS_SPEED	= 173,
	USTATE_STAND_SOLID		= 174,
	USTATE_SHAKE_ATTACK		= 175,
	USTATE_BLOOD_ANGRY		= 176,
	USTATE_KEEP_SILENCE		= 177,
	USTATE_BOTH_SKILL_IN_MIND	= 178,
	USTATE_SAY_BAD_WORDS	= 179,
	USTATE_COMBIN_WIND_PET	= 180,
	USTATE_KEEP_FAR_AWAY	= 181,
	USTATE_COLD_ICE_SKILL	= 182,
	USTATE_FAST_SHOOT		= 183,
	USTATE_EAT_GHOST		= 184,
	USTATE_NO_FOOD_EAT		= 185,
	USTATE_DEFENCE_RING		= 186,
	USTATE_COMBIN_GOD_LEADER	= 187,
	USTATE_SOFT_CLOTH		= 188,
	USTATE_GIVEUP_LIFE		= 189,
	USTATE_TWO_COLD_EFFECT	= 190,
	USTATE_DEFENCE_RING_EFFECT	= 191,
	USTATE_STEEL_MAN_SKILL	= 192,
	USTATE_NO_COUNTRY_LIMIT	= 193,
	USTATE_STRENGTH_ATTACK_BLESS	= 194,
	USTATE_TEAM_ATTACK_BIRD_FLAG	= 195,
	USTATE_TEAM_ATTACK_FLOW_CLOUD	= 196,
	USTATE_NO_MOVE			= 197,
	USTATE_MOVE_FAST		= 198,
	USTATE_GOD_BLESS_U		= 199,
	USTATE_FIRE_ROAD		= 200,
	USTATE_CRAZY_ACTION		= 201,
	USTATE_KILL_MAGIC_DEFENCE	= 202,
	USTATE_KILL_PHYSIC_DEFENCE	= 203,
	USTATE_TECH_OF_WIND		= 204,
	USTATE_UNLIMIT_MAGIC	= 205,
	USTATE_GOD_FIGHTER_EYE	= 206,
	USTATE_ROTATE_FIRE_LAMP	= 207,
	USTATE_COOL_TOUCH		= 208,
	USTATE_DANCE_SHADOW		= 209,
	USTATE_WIND_THOUNDER_DEFENCE	= 210,
	USTATE_COMMON_TEMP_USE	= 211,
	USTATE_ICE_BOX_PROTECT	= 212,
	USTATE_STATIC_LIGHT_EFFECT	= 213,
	USTATE_SOFT_ATTACK		= 214,
	USTATE_MAGIC_DEFENCE_SWORD	= 215,
	USTATE_LIQUID_MOVE		= 216,
	USTATE_MP_STRENGTH		= 217,
	USTATE_HP_STRENGTH		= 218,
	USTATE_ATTACK_BACK_HEART	= 219,
	USTATE_SOFT_BOTH_HAND	= 220,
	USTATE_FIRE_HEART_SKILL		= 221,
	USTATE_CRAZY_ATTACK		= 222,
	USTATE_IGNORE_ATTACK	= 223,
	USTATE_STAND_WITH__GHOST	= 224,
	USTATE_LION_HEART		= 225,
	USTATE_WIND_LEADER		= 226,
	USTATE_COMBIN_IRON_PET	= 227,
	USTATE_COMBIN_BONE_GUARD	= 228,
	USTATE_COMBIN_XIAN_PET		= 229,
	USTATE_RESCUE_QUICKLY	= 230,
	USTATE_NO_USE_SKILL		= 231,
	USTATE_ICE_RANGE		= 232,
	USTATE_PROTECT_SKILL	= 233,
	USTATE_TEAM_ATTACK_ONE_DIM	= 234,
	USTATE_ULTRA_EQUIPMENT	= 235,
	USTATE_FIRE_EAT_ALL		= 236,
	USTATE_REJECT_LIGHT_RING	= 237,
	USTATE_SOFT_LOVE_HEART	= 238,
	USTATE_TOGETHER_WITH_DRAGON	= 239,
	USTATE_TOGETHER_WITH_TIGER	= 240,
	USTATE_IRON_BONE_SPRITE		= 241,
	USTATE_IRON_BONE_GUARD		= 242,
	USTATE_BEFORE_DIE_STATE		= 243,
	USTATE_DO_WITHOUT_THINKING	= 244,
	USTATE_HIDE_PET_TRAINING	= 245,
	USTATE_RELIVE_FAST_STATE	= 246,
	USTATE_ATTACK_BACK_STATE	= 247,
	USTATE_FIRE_WITH_FIRE		= 248,
	USTATE_FIRE_NO_EFFECT		= 249,
	USTATE_ATTACK_BLESS			= 250,
	USTATE_GOD_LABOR_BLESS		= 251,
	USTATE_STRENGTH_DEFENCE_SKILL	= 252,
	USTATE_HIDE_NOMOVE_STATE	= 253,
    USTATE_PRIVATE_STORE    = 254, // 摆摊
	USTATE_HANLIU                   = 255,//寒流
    USTATE_XIXINGEMIA               = 256,//洗心革面
    USTATE_QUBAOJIUYI               = 257,//取保就医
	USTATE_BLIND_STRENGTH			= 258,
	USTATE_MAKE_FORWORD		= 259,
	USTATE_COME_ON_BABY		= 260,
	USTATE_ATTACK_STORM_SKILL	= 261,
	USTATE_PROPERTY_DROPDOWN	= 262,
	USTATE_HEART_OF_DEAMON		= 263,
	USTATE_BODY_COLD_STONE		= 264,
	USTATE_BODY_CHANGE_OLD		= 265,
	USTATE_SHAINT_SHIELD_BLESS	= 266,
	USTATE_SHAINT_LIGHT_BLESS	= 267,
	USTATE_GIVEUP_BACK_ROAD		= 268,
	USTATE_PEOPLE_CAN_DEFEAT_GOD	= 269,
	USTATE_WASH_SNOW_LIGHT_RING	= 270,
	USTATE_COMBIN_FIRE_LEADER	= 271,
	USTATE_COMBIN_SIX_GHOST		= 272,
	USTATE_COMBIN_THREE_DIM		= 273,
	USTATE_HURT_XIAN			= 274,
	USTATE_HURT_MAGIC			= 275,
	USTATE_DEATH_ATTACH			= 276,
	USTATE_POWER_SPRITE			= 277,
	USTATE_CALL_RAIN			= 278,
	USTATE_DEEP_EARTH_FIRE		= 279,
	USTATE_BLOOD_DUTY			= 280,
	USTATE_DANCE_AMONG_DURT_WATER	= 281,
	USTATE_DAOJISHI				= 282,//在收费地图的计时

    USTATE_EXP_125              = 302,  //经验1.25倍率状态
	USTATE_EXP_150              = 303,  //经验1.50倍率状态
	USTATE_EXP_175              = 304,  //经验1.75倍率状态
 // USTATE_LOONG_OUT            = 303,  //太庙NPC特效 龙飞出
 // USTATE_LOONG_IN             = 304,  //太庙NPC特效 龙飞入
 // USTATE_BIG_LOONG_OUT        = 305,  //太庙NPC特效 大龙飞出
 	USTATE_USER_BIKE			= 333,	//自行车比赛状态 by醉梦
	USTATE_USER_ZC	    		= 1410,	//战车状态 by醉梦

    USTATE_USER_RENQI           = 361,	//人气宝贝
    USTATE_USER_MEILI           = 362,	//魅力宝贝
	USTATE_USER_XHBB			= 363,	//鲜花宝贝
	USTATE_USER_DANCE			= 370,	//跳舞
	USTATE_VIP1                 = 379, //VIP1玩家特效
	USTATE_VIP2                 = 380, //VIP2玩家特效
	USTATE_VIP3                 = 381, //VIP3玩家特效
	USTATE_VIP4                 = 382, //VIP4玩家特效
	USTATE_VIP5                 = 383, //VIP5玩家特效
	USTATE_VIP6                 = 384, //VIP6玩家特效
	USTATE_VIP7                 = 385, //VIP7玩家特效
	USTATE_VIP8                 = 386, //VIP8玩家特效
	USTATE_VIP9                 = 387, //VIP9玩家特效
	USTATE_VIP10                = 388, //VIP10玩家特效
	USTATE_VIP11                = 389, //VIP11玩家特效
	USTATE_VIP12                = 390, //VIP12玩家特效
	USTATE_VIP13                = 391, //VIP13玩家特效
	USTATE_VIP14                = 395, //VIP14玩家特效
	USTATE_VIP15                = 396, //VIP15玩家特效
	USTATE_VIP16                = 397, //VIP16玩家特效
	USTATE_VIP17                = 398, //VIP17玩家特效
	USTATE_VIP18                = 399, //VIP18玩家特效
	USTATE_VIP19                = 400, //VIP19玩家特效
	USTATE_VIP20                = 401, //VIP20玩家特效
	USTATE_FLOWER_9			    = 392,	//9朵花
	USTATE_FLOWER_99			= 393,	//99朵花
	USTATE_FLOWER_999			= 394,	//999朵花
	USTATE_BU_TIAN              = 406,  //补天圣石状态
	USTATE_ULTRA_THR_XING	    = 469,	//更新3星效果
	USTATE_ULTRA_FIV_XING       = 470,  //全套5星紫装状态
	USTATE_ULTRA_TEN_XING       = 471,  //全套10星紫装状态
    USTATE_LOONGPEARL_WHITE     = 481,  //白色砖块状态
    USTATE_LOONGPEARL_BLUE      = 482,  //蓝色砖块状态
    USTATE_LOONGPEARL_YELLOW    = 483,  //黄色砖块状态
    USTATE_LOONGPEARL_GREEN     = 484,  //绿色砖块状态
    USTATE_LOONGPEARL_PURPLE    = 485,  //紫色砖块状态
	USTATE_ULTRA_EIG_XING       = 496,  //全套8星紫装状态
    USTATE_DI_ER_LING_HUN       = 497,  //第二灵魂套装状态
    USTATE_SPA                  = 578,  //在温泉中泡澡的状态
	USTATE_LUCKY_DAN			= 616,	//幸运丹状态
	USTATE_FLOWERL_9			= 620,	//9朵蓝色花
	USTATE_FLOWERL_99			= 621,	//99朵花蓝色花
	USTATE_FLOWERL_999			= 622,	//999朵花蓝色花
	USTATE_ULTRA_THI_XING       = 633,  //全套13星紫装状态
	USTATE_ULTRA_FOU_XING       = 671,  //全套14星紫装状态
	USTATE_ULTRA_TWE_XING       = 741,  //全套12星紫装状态
    USTATE_USER_CANGLONG        = 777,  //苍龙套装
	USTATE_USER_JULONG          = 778,  //巨龙套装
	USTATE_USER_LONGWANG        = 779,  //龙王套装
	USTATE_CHANGEBODY_CHILD		= 790,	//变身
	USTATE_FUBEN		        = 797,	//副本时间
	USTATE_USER_SHENCANGLONG    = 808,  //神苍龙套装
	USTATE_USER_SHENJULONG      = 809,  //神巨龙套装
	USTATE_USER_SHENLONGWANG    = 810,  //神龙王套装
	USTATE_DOU_HUN              = 831,  //斗魂系统人物吸魂状态
	USTATE_ULTRA_FIF_XING       = 859,  //全套15星紫装状态
	USTATE_USER_WABAO			= 965,	//在线挖宝
	USTATE_ULTRA_JINGLIAN3     = 970, //全套马牌精炼3级
	USTATE_ULTRA_JINGLIAN5     = 971, //全套马牌精炼5级
	USTATE_ULTRA_JINGLIAN8     = 972, //全套马牌精炼8级
	USTATE_ULTRA_JINGLIAN10     = 973, //全套马牌精炼10级
	USTATE_ULTRA_JINGLIAN12     = 974, //全套马牌精炼12级
	USTATE_ULTRA_JINGLIAN15     = 975, //全套马牌精炼15级
	USTATE_ULTRA_SHEN1_QI       = 1012, //神器x1
	USTATE_ULTRA_SHEN2_QI       = 1013, //神器x2
	USTATE_ULTRA_SHEN3_QI       = 1014, //神器x3
	USTATE_ULTRA_SHEN4_QI       = 1015, //神器x4
	USTATE_ULTRA_SHEN5_QI       = 1016, //神器x5
	USTATE_ULTRA_SHEN6_QI       = 1017, //神器x6
	USTATE_ULTRA_SHEN7_QI       = 1018, //神器x7
	USTATE_ULTRA_ZHUANG2_QI     = 1019, //装备栏x2
	USTATE_ULTRA_ZHUANG3_QI     = 1020, //装备栏x3
	USTATE_ULTRA_ZHUANG4_QI     = 1021, //装备栏x4
	USTATE_ULTRA_ZHUANG5_QI     = 1022, //装备栏x5
	USTATE_ULTRA_ZHUANG6_QI     = 1023, //装备栏x6
	USTATE_ULTRA_CHIBANG_QI     = 1025, //翅膀
	USTATE_ULTRA_HANBING_QI     = 1026, //寒冰
    USTATE_USER_SUOXIAO         = 1050, // 缩小
	USTATE_USER_FLY             = 1055, // 飞行状态、飞行坐骑
	USTATE_ULTRA_BUTIAN1_QI     = 1120, // 补天1
	USTATE_ULTRA_BUTIAN2_QI     = 1121, // 补天2
	USTATE_ULTRA_BUTIAN3_QI     = 1122, // 补天3
	USTATE_ULTRA_BUTIAN4_QI     = 1123, // 补天4
	USTATE_ULTRA_BUTIAN5_QI     = 1124, // 补天5
	USTATE_ULTRA_WU_XING        = 1218, //龙星套装5星套
	USTATE_ULTRA_BA_XING        = 1219, //龙星套装8星套
	USTATE_ULTRA_SHIYI_XING     = 1220, //龙星套装11星套
	USTATE_ULTRA_SHISA_XING     = 1221, //龙星套装13星套
	USTATE_ULTRA_SHIWU_XING     = 1222, //龙星套装15星套
	USTATE_DI_ER_SAN_SHITIAN	= 1252,	//弑天套装三强
	USTATE_DI_ER_ERS_SHITIAN	= 1253,	//弑天套装二十强
	USTATE_DI_ER_LING_SHITIAN	= 1254,	//弑天套装百强
	USTATE_DI_ER_LING_HUN1      = 1255,  //第二灵魂物理防御加成
	USTATE_DI_ER_LING_HUN2      = 1256,  //第二灵魂魔法防御加成
	USTATE_DI_ER_LING_HUN3      = 1257,  //第二灵魂物理攻击加成
	USTATE_DI_ER_LING_HUN4      = 1258,  //第二灵魂魔法攻击加成
	USTATE_DI_ER_LING_HUN5      = 1259,  //第二灵魂增加生命加成
	USTATE_FLOWERQS_9			= 1287,	//七色花9朵
	USTATE_FLOWERQS_99			= 1288,	//七色花99朵
	USTATE_FLOWERQS_999			= 1289,	//七色花999朵
	USTATE_EMPEROR				= 1299, //皇帝
	USTATE_KING					= 1300, //国王
	USTATE_UNIONMASTER			= 1301, //城主
	USTATE_ULTRA_DOUHUN3        = 1341,  //全套3星斗魂状态
	USTATE_ULTRA_DOUHUN6        = 1342,  //全套6星斗魂状态
	USTATE_ULTRA_DOUHUN9        = 1343,  //全套9星斗魂状态
    USTATE_ULTRA_DOUHUN12       = 1344,  //全套12星斗魂状态
	USTATE_ULTRA_DOUHUN15       = 1345,  //全套15星斗魂状态
	USTATE_USER_TZ_QIANKUN      = 1350,  //乾坤套装(10件)
	USTATE_USER_TZ_ZHITIAN      = 1351,  //炙天套装(10件)	
	USTATE_USER_TZ_XUANYUAN     = 1352,  //轩辕套装(10件)
	USTATE_USER_TZ_ZHUOYUE      = 1353,  //卓越套装(10件)
	USTATE_USER_TZ_CHUANSHUO    = 1354,  //传说套装(10件)
	USTATE_USER_TZ_TIANZUN      = 1355,  //天尊套装(10件)
	USTATE_USER_TZ_YINGXIONG    = 1356,  //英雄套装(10件)
	USTATE_USER_TZ_SHENGZHE     = 1357,  //圣者套装(10件)
	USTATE_USER_TZ_ZHANSHEN     = 1358,  //战神套装(10件)
	USTATE_USER_TZ_DIWANG       = 1359,  //帝王套装(10件)
	USTATE_USER_TZ_SHITIAN      = 1360,  //弑天套装(10件)
	USTATE_USER_TZ_YONGHENG     = 1361,  //永恒套装(10件)
	USTATE_USER_TZ_TUJIAN1      = 1371,  //白色图鉴(10件)	
	USTATE_USER_TZ_TUJIAN2      = 1372,  //蓝色图鉴(10件)	
	USTATE_USER_TZ_TUJIAN3      = 1373,  //黄色图鉴(10件)	
	USTATE_USER_TZ_TUJIAN4      = 1374,  //绿色图鉴(10件)	
	USTATE_USER_TZ_TUJIAN5      = 1375,  //紫色图鉴(10件)					
    USTATE_USER_TZ_TUJIAN6      = 1376,  //橙色图鉴(10件)
	USTATE_USER_TZ_TUJIAN7      = 1377,  //红色图鉴(10件)


	USTATE_USER_LONGWANG_LV1     = 1401, //小龙套装
	USTATE_USER_LONGWANG_LV2     = 1402, //金龙套装
	USTATE_USER_LONGWANG_LV3     = 1403, //苍龙套装
	USTATE_USER_LONGWANG_LV4     = 1404, //巨龙套装
	USTATE_USER_LONGWANG_LV5     = 1405, //神苍龙套装
	USTATE_USER_LONGWANG_LV6     = 1406, //神巨龙套装	

	USTATE_USER_NITIAN_LV1     = 1411, //逆天套装15阶
	USTATE_USER_NITIAN_LV2     = 1412, //逆天套装30阶
	USTATE_USER_NITIAN_LV3     = 1413, //逆天套装50阶
	USTATE_USER_NITIAN_LV4     = 1414, //逆天套装80阶	
	USTATE_USER_NITIAN_LV5     = 1415, //逆天套装100阶

	USTATE_USER_DOUZHANSF_LV1     = 1421, //斗战圣佛25阶套装
	USTATE_USER_DOUZHANSF_LV2     = 1422, //斗战圣佛50阶套装
	USTATE_USER_DOUZHANSF_LV3     = 1423, //斗战圣佛75阶套装
	USTATE_USER_DOUZHANSF_LV4     = 1424, //斗战圣佛100阶套装

	USTATE_USER_AYSTONE_LV1     = 1431, //暗影石20阶套装
	USTATE_USER_AYSTONE_LV2     = 1432, //暗影石40阶套装
	USTATE_USER_AYSTONE_LV3     = 1433, //暗影石60阶套装
	USTATE_USER_AYSTONE_LV4     = 1434, //暗影石80阶套装
	USTATE_USER_AYSTONE_LV5     = 1435, //暗影石100阶套装


	USTATE_YONGJIUCH1			= 1801, //称号
	
	USTATE_SUOGUDAN			= 1900,//缩骨丹
	USTATE_USER_ZC_TYPE1	    = 1901,	//战车状态加成 生命by醉梦
	USTATE_USER_ZC_TYPE2	    = 1902,	//战车状态加成 攻击by醉梦
	USTATE_USER_ZC_TYPE3	    = 1903,	//战车状态加成 防御by醉梦
	USTATE_ZHUTOU				= 1910,		/// 猪头术


	USTATE_DUANWEI1                = 2101, //段位1级
	USTATE_DUANWEI2                = 2102, //段位2级
	USTATE_DUANWEI3                = 2103, //段位3级
	USTATE_DUANWEI4                = 2104, //段位4级
	USTATE_DUANWEI5                = 2105, //段位5级
	USTATE_DUANWEI6                = 2106, //段位6级
	USTATE_DUANWEI7                = 2107, //段位7级
	USTATE_DUANWEI8                = 2108, //段位8级
	USTATE_DUANWEI9                = 2109, //段位9级
	USTATE_DUANWEI10                = 2110, //段位10级

	
	MAX_STATE = 9999                    /// 最大状态数	

};


/// 定义善恶度
#ifndef _GOOD_NESS_
#define _GOOD_NESS_
enum
{
	GOODNESS_0		= -120 & 0x0000FFFF,			//英雄
	GOODNESS_1		= -60 & 0x0000FFFF,				//侠士
	GOODNESS_2_1	= 0,							//普通1
	GOODNESS_2_2	= 0xFF000000,					//普通2
	GOODNESS_3		= 60,							//歹徒
	GOODNESS_4		= 120,							//恶徒
	GOODNESS_5		= 180,							//恶魔
	GOODNESS_6		= 300,							//魔头
	GOODNESS_7		= 10000,						//叛国
	
	GOODNESS_ATT	= 0x00010000,					//主动伤害别人
	GOODNESS_DEF	= 0x00010000 << 1				//自卫方
};
#endif
///定义善恶度结束

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param state : 描述
 * \param teststate : 描述
 * \return 返回值的描述
 */
/// 检查某个状态是否设置
inline bool isset_state(const unsigned char *state, const int teststate)
{
    //soke 防止服务端发-1  把我弄崩
	if ( teststate > MAX_STATE )
		return false;

	return 0 != (state[teststate / 8] & (0xff & (1 << (teststate % 8))));
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param state : 描述
 * \param teststate : 描述
 * \return 返回值的描述
 */
/// 设置某个状态
inline void set_state(unsigned char *state, const int teststate)
{
    //soke 防止服务端发-1  把我弄崩
	if ( teststate > MAX_STATE )
		return;

	state[teststate / 8] |= (0xff & (1 << (teststate % 8)));
	Engine_WriteLogF("设置状态%d\n",teststate );
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param state : 描述
 * \param teststate : 描述
 * \return 返回值的描述
 */
/// 清除某个状态
inline void clear_state(unsigned char *state, const int teststate)
{
    //soke 防止服务端发-1  把我弄崩
	if ( teststate > MAX_STATE )
		return;

	state[teststate / 8] &= (0xff & (~(1 << (teststate % 8))));
}

struct t_MapBuildingData{
	DWORD dwMapBuildingTempID;			/**< 建筑物的临时ID */
	DWORD dwBuildingDataID;				/**< 建筑物的数据ID */
	DWORD x;
	DWORD y;
	DWORD dwContract;					/**< 地契编号 */
	char pstrName[MAX_NAMESIZE];		/**< 建筑物的名称 */
	char pstrUserName[MAX_NAMESIZE];	/**< 建筑物的所有者名称 */
};

//struct t_MapObjectData {
//	DWORD dwMapObjectTempID;				/**< 物品的临时ID */
//	DWORD dwObjectID;                       /**< 物品的数据ID */
//	char pstrName[MAX_NAMESIZE];			/**<  物品名 */
//	DWORD x;
//	DWORD y;
//	DWORD wdNumber;                         /**< 数量 */
//	WORD  wdLevel;                          /**< 等级 */
//};
struct t_MapObjectData {
	DWORD dwMapObjectTempID;                /**< 物品的临时ID */
	DWORD dwObjectID;                       /**< 物品的数据ID */
	char pstrName[MAX_NAMESIZE];            /**<  物品名 */
	DWORD x;
	DWORD y;
	DWORD wdNumber;                         /**< 数量 */
	WORD  wdLevel;                          /**< 等级 */
	BYTE upgrade;
	WORD kind;
	DWORD dwOwner;
	DWORD GuoOwner;//国家归属
};


//struct t_MapNpcData {
//	DWORD dwMapNpcDataPosition;     /**< npc的数据位置 */
//	DWORD dwNpcDataID;                      /**< npc的数据ID */
//	WORD  movespeed;                /// 移动速度
//
//	BYTE byDirect;                          /**< Npc的方向 */
//	BYTE byState[(MAX_STATE + 7) / 8];                      /**< Npc的状态 */
//	DWORD level;///等级
//};

struct t_NpcData {
	DWORD dwMapNpcDataPosition;     /**< npc的数据位置 */
	DWORD dwNpcDataID;                      /**< npc的数据ID */
	WORD  movespeed;                /// 移动速度

	BYTE byDirect;                          /**< Npc的方向 */
	BYTE level;///等级
};
struct t_MapNpcData :public t_NpcData
{
	BYTE byState[(MAX_STATE + 7) / 8];			/**< Npc的状态 */
};
struct t_MapNpcDataState :public t_NpcData
{
	t_MapNpcDataState()
	{
		num=0;
	}
	BYTE num; 		///带状态的数量
	WORD state[0];		///状态列表
};

struct t_MapPetData {
	DWORD tempID;//npc的临时ID

	char name[MAX_NAMESIZE]; //npc的名称
	BYTE masterType;        ///主人的类型  0：玩家 1：npc
	DWORD masterID; ///主人的id -1：没有主人
	char masterName[MAX_NAMESIZE];///主任的名字
	BYTE pet_type;//宠物类型
};

struct t_MapUserSculpt{
	DWORD dwHorseID;					/// 马牌编号
	DWORD dwHairID;						/// 头发类型
	DWORD dwBodyID;						/// 身体物品ID
	DWORD dwBodyKind;					/// 身体物品type类型
	DWORD dwLeftHandID;					/// 左手物品ID
	DWORD dwRightHandID;				/// 右手物品ID
};

//队伍标志
enum{
	TEAD_STATE_NONE ,
	TEAD_STATE_MEMBER ,
	TEAM_STATE_LEADER,
};

#define _WYC_OPT_SER_
#ifndef  _WYC_OPT_SER_
//直接传递帮会消息
struct t_MapUserData {
	DWORD dwUserTempID;                                     //// 用户临时ID

	char  name[MAX_NAMESIZE + 1];           /// 角色名称
	WORD  type;                                                     /// 职业类型
	//WORD  sex;                                                    /// 性别
	WORD  face;                                                     /// 头象
	DWORD goodness;                                         /// 善恶度

	t_MapUserSculpt sculpt;
	DWORD dwHairRGB;                                        /// 头发颜色0xffrrggbb
	DWORD dwBodyColorSystem;                        /// 衣服系统颜色0xffrrggbb
	DWORD dwBodyColorCustom;                        /// 衣服定制颜色0xffrrggbb
	DWORD dwLeftWeaponColor;                        /// 左手武器颜色0xffrrggbb
	DWORD dwRightWeaponColor;                       /// 右手武器颜色0xffrrggbb
	WORD  attackspeed;                                      /// 攻击速度
	WORD  movespeed;                                        /// 移动速度
	DWORD country;                                          /// 国家
	DWORD dwChangeFaceID;                           //易容后的NPC的类型ID
	BYTE state[(MAX_STATE + 7) / 8];        /// 角色状态
	BYTE live_skills[6];                            //生活技能等级  
	BYTE level;                                                     //Level : 1(<10)   11(>=10)
	DWORD exploit;                          // 功勋值
	DWORD grace;                            // 文采值
	char  unionName[MAX_NAMESIZE];          // 帮会名称
	char  septName[MAX_NAMESIZE];           // 家族名称
	char  caption[MAX_NAMESIZE];            // 顶部头衔（某城城主或某国国王）
	DWORD dwTeamState;                                      // 队伍状态
	DWORD dwArmyState;                      //1为队长，2为将军,0为未加入军队
	DWORD touxianlevel;   //头衔等级
	DWORD hanbing;   //寒冰
};
#endif

#ifdef _WYC_OPT_SER_
//////////////////////////////////////////////////////////////////////////
///add by wyc for opt server conm-----start.
//////////////////////////////////////////////////////////////////////////
//struct t_MapUserData {
//	DWORD dwUserTempID;                                     //// 用户临时ID
//
//	char  name[MAX_NAMESIZE + 1];           /// 角色名称
//	WORD  type;                                                     /// 职业类型
//	//WORD  sex;                                                    /// 性别
//	WORD  face;                                                     /// 头象
//	DWORD goodness;                                         /// 善恶度
//
//	t_MapUserSculpt sculpt;
//	DWORD dwHairRGB;                                        /// 头发颜色0xffrrggbb
//	DWORD dwBodyColorSystem;                        /// 衣服系统颜色0xffrrggbb
//	DWORD dwBodyColorCustom;                        /// 衣服定制颜色0xffrrggbb
//	DWORD dwLeftWeaponColor;                        /// 左手武器颜色0xffrrggbb
//	DWORD dwRightWeaponColor;                       /// 右手武器颜色0xffrrggbb
//	WORD  attackspeed;                                      /// 攻击速度
//	WORD  movespeed;                                        /// 移动速度
//	DWORD country;                                          /// 国家
//	DWORD dwChangeFaceID;                           //易容后的NPC的类型ID
//	BYTE state[(MAX_STATE + 7) / 8];        /// 角色状态
//	BYTE live_skills[6];                            //生活技能等级
//	BYTE level;                                                     //Level : 1(<10)   11(>=10)
//	DWORD exploit;                          // 功勋值
//	DWORD grace;                            // 文采值
//
//	DWORD dwUnionID;                        // 帮会ID
//	DWORD dwSeptID;                         // 家族ID
//
//	char  caption[MAX_NAMESIZE];            // 顶部头衔（某城城主或某国国王）
//
//	DWORD dwTeamState;                                      // 队伍状态
//	DWORD dwArmyState;                      //1为队长，2为将军,0为未加入军队
//};
/// 用户本身的数据，不包括状态
/// 用户本身的数据，不包括状态
struct t_UserData
{
	DWORD dwUserTempID;                                     //// 用户临时ID

	char  name[MAX_NAMESIZE + 1];           /// 角色名称
	WORD  type;                                                     /// 职业类型
	//WORD  sex;                                                    /// 性别
	WORD  face;                                                     /// 头象
	DWORD goodness;                                         /// 善恶度

	t_MapUserSculpt sculpt;
	DWORD dwHairRGB;                                        /// 头发颜色0xffrrggbb
	DWORD dwBodyColorSystem;                        /// 衣服系统颜色0xffrrggbb
	DWORD dwBodyColorCustom;                        /// 衣服定制颜色0xffrrggbb
	DWORD dwLeftWeaponColor;                        /// 左手武器颜色0xffrrggbb
	DWORD dwRightWeaponColor;                       /// 右手武器颜色0xffrrggbb
	WORD  attackspeed;                                      /// 攻击速度
	WORD  movespeed;                                        /// 移动速度
	BYTE country;                                           /// 国家
	DWORD dwChangeFaceID;                           //易容后的NPC的类型ID
	//BYTE live_skills[6];                          //生活技能等级
	BYTE level;                                                     //Level : 1(<10)   11(>=10)
	DWORD exploit;                          // 功勋值
	DWORD grace;                            // 文采值

	DWORD dwUnionID;                        // 帮会ID
	DWORD dwSeptID;                         // 家族ID

	char  caption[MAX_NAMESIZE];            // 顶部头衔（某城城主或某国国王）
	DWORD dwTeamState;                                      // 队伍状态
	DWORD dwArmyState;                      //1为队长，2为将军,0为未加入军队
	DWORD touxianlevel;   //头衔等级
	DWORD hanbing;   //寒冰
};


struct t_MapUserData : public t_UserData 
{
	BYTE state[(MAX_STATE + 7) / 8];	/// 角色状态
};
struct t_MapUserDataState : public t_UserData 
{
	t_MapUserDataState()
	{
		num=0;
	}
	BYTE num; 		///带状态的数量
	WORD state[0];		///状态列表
};
//////////////////////////////////////////////////////////////////////////
///add by wyc for opt server conm-----end.
//////////////////////////////////////////////////////////////////////////


namespace wyc
{
	struct t_MapUserData : public ::t_MapUserData
	{
		char  unionName[MAX_NAMESIZE];          // 帮会名称
		char  septName[MAX_NAMESIZE];           // 家族名称 
	};
};
//////////////////////////////////////////////////////////////////////////
//add by wyc another-----end;
//////////////////////////////////////////////////////////////////////////
#endif

struct t_MainUserData
{
	DWORD dwUserTempID;             /// 用户临时ID
	DWORD userid;                   /// 用户ID
	WORD  level;                    /// 角色等级
	DWORD hp;                       /// 当前生命值
	DWORD maxhp;                    /// 最大生命值
	DWORD resumehp;                 /// 生命值恢复
	DWORD mp;                       /// 当前法术值
	DWORD maxmp;                    /// 最大法术值
	DWORD resumemp;                 /// 法术值恢复
	DWORD sp;                       /// 当前体力值
	DWORD maxsp;                    /// 最大体力值
	DWORD resumesp;                 /// 体力值恢复
	DWORD pdamage;                  /// 最小物理攻击力
	DWORD maxpdamage;               /// 最大物理攻击力
	DWORD mdamage;                  /// 最小法术攻击力
	DWORD maxmdamage;               /// 最大法术攻击力
	DWORD pdefence;                 /// 物理防御力
	DWORD mdefence;                 /// 法术防御力
	uint64_t exp;                   /// sky 当前经验值
	uint64_t nextexp;               /// sky 升级经验值
	WORD  attackrating;             /// 攻击命中
	WORD  attackdodge;              /// 攻击躲避
	WORD  bang;                     /// 重击
	WORD  lucky;                    /// 幸运值
	WORD  charm;                    /// 魅力值
	DWORD  round;                   /// 转身
	union {
		struct {
			WORD wdCon; //体质
			WORD wdStr; //体力
			WORD wdDex; //敏捷
			WORD wdInt; //智力
			WORD wdMen; //精神
		};
		WORD wdProperty[5];
	};
	WORD  skillPoint;                   /// 技能点数
	WORD  points;                       /// 点数
	DWORD country;                      /// 国家
	WORD  pkmode;                       /// pk模式


	DWORD stdpdamage;                   /// 标准物理攻击力
	DWORD stdmdamage;                   /// 标准法术攻击力
	DWORD stdpdefence;                  /// 标准物理防御力
	DWORD stdmdefence;                  /// 标准法术防御力
	WORD  stdbang;                      /// 标准重击率
	union {
		struct {
			WORD wdStdCon;  //体质
			WORD wdStdStr;  //体力
			WORD wdStdDex;  //敏捷
			WORD wdStdInt;  //智力
			WORD wdStdMen;  //精神
		};
		WORD wdStdProperty[5];
	};
	WORD wdTire;       /// 疲劳状态 0为非 1为疲劳
	DWORD fivetype;    /// 五行类型
	DWORD fivepoint;   /// 五行点数
	DWORD money;       /// 银子数量
	DWORD honor;       /// 荣誉值
	DWORD maxhonor;    /// 最大荣誉值
	DWORD gold;	       /// 金子数
	DWORD ticket;      /// 积分数
	DWORD salary;      /// 工资数
	DWORD killerone;   /// 被杀次数（1v1专用）
	DWORD killertwo;   /// 被杀次数（待定）
	DWORD killerthe;   /// 被杀次数（待定）
	DWORD changejob;   /// 职业
	DWORD charvip;     /// VIP点数
	DWORD chongzhi;    /// 充值点
	DWORD folwers;     /// 鲜花
	DWORD moods;       /// 人气
	DWORD flloves;     /// 爱心
	DWORD angel; 	   /// 翅膀
	DWORD hanbing; 	   /// 寒冰
	DWORD bitmask;     /// 角色掩码
	DWORD zhanli;			   ///add 2023-04-10 战力
	DWORD m13axhp;			   // 百兽图鉴 最大生命值
	DWORD p13damage;		   // 百兽图鉴 最小攻击力
	DWORD m13axpdamage;		   // 百兽图鉴 最大攻击力
	DWORD m13damage;		   // 百兽图鉴 最小法术攻击力
	DWORD m13axmdamage;		   // 百兽图鉴 最大法术攻击力
	DWORD p13defence;		   // 百兽图鉴 物防
	DWORD m13defence;		   // 百兽图鉴 魔防		
	DWORD touxianlevel;        //头衔
	DWORD chenghao_select;     //称号
	DWORD zhancheid;     //战车id
	DWORD zhanchetype;     //战车类型

};

struct stDataUserCmd : public stNullUserCmd
{
	stDataUserCmd()
	{
		byCmd = DATA_USERCMD;
	}
};

/// 主用户数据
const BYTE MAIN_USER_DATA_USERCMD_PARA = 1;
	struct stMainUserDataUserCmd : public stDataUserCmd {
		stMainUserDataUserCmd()
		{
			byParam = MAIN_USER_DATA_USERCMD_PARA;
		}

		t_MainUserData data;
	};


/// 减少剩余点数，增加到体力等中
const BYTE DECREMAINDER_DATA_USERCMD_PARA = 2;
	struct stDecRemainderDataUserCmd : public stDataUserCmd{
		stDecRemainderDataUserCmd()
		{
			byParam = DECREMAINDER_DATA_USERCMD_PARA;
		}

		BYTE byDecType;				/**< 加点类型 */
	};

/// 设置用户生命和魔法
	const BYTE SETHPANDMP_DATA_USERCMD_PARA = 3;
	struct stSetHPAndMPDataUserCmd : public stDataUserCmd{
		stSetHPAndMPDataUserCmd()
		{
			byParam = SETHPANDMP_DATA_USERCMD_PARA;
		}

		DWORD dwHP;                                     /**< HP */
		DWORD dwMP;                                     /**< MP */
		//Modifyed By david For 2006-3-14
		//DWORD dwSP;                                     /**< SP */ 
	};

enum {
	MAP_SETTING_RIDE	= 0x00000001,	/// 大陆可以骑马
	MAP_SETTING_WEATHER	= 0x00000002	/// 大陆有雨雪天气变化
};
/// 地图信息
const BYTE MAPSCREENSIZE_DATA_USERCMD_PARA = 28;
	struct stMapScreenSizeDataUserCmd : public stDataUserCmd{
		stMapScreenSizeDataUserCmd()
		{
			byParam = MAPSCREENSIZE_DATA_USERCMD_PARA;
		}

		DWORD width;		/**< 场景宽 */
		DWORD height;		/**< 场景高 */
		char pstrMapName[MAX_NAMESIZE];	/**< 大陆名称 */
		char pstrFilename[MAX_NAMESIZE];	/**< 文件名称 */
		DWORD setting;				/**< 大陆标志 */
		DWORD rgb;					/**< 颜色标志0x00rrggbb */
		char pstrGroupName[MAX_NAMESIZE];	/**< 服务器组名称 */
		char pstrCountryName[MAX_NAMESIZE];	/**< 国家名称 */
		char pstrCityName[MAX_NAMESIZE];	/**< 城市名称 */
		int  mainRoleX;
		int  mainRoleY;
		short npc_count;
		struct {
			DWORD id, x, y;
		} npc_list[0];
	};

//客户端加载地图完成
const BYTE LOADMAPOK_DATA_USERCMD_PARA = 29;
	struct stLoadMapOKDataUserCmd : public stDataUserCmd{
		stLoadMapOKDataUserCmd()
		{
			byParam = LOADMAPOK_DATA_USERCMD_PARA;
		}
	};

/// 角色的等级排名
const BYTE LEVELDEGREE_DATA_USERCMD_PARA = 30;
struct stLevelDegreeDataUserCmd : public stDataUserCmd{
	stLevelDegreeDataUserCmd()
	{
		byParam = LEVELDEGREE_DATA_USERCMD_PARA;
	}
	WORD degree; // 1-2000名
};


#define __NEW_CHECK__
#ifdef __NEW_CHECK__
/// 发送特征码文件
const BYTE STAMP_DATA_USERCMD_PARA = 51;        
struct stStampDataUserCmd : public stDataUserCmd                
{       
	bool check;//是否进行外挂检测
	DWORD size;
	BYTE bin[0];//特征码文件
	stStampDataUserCmd()
	{
		byParam = STAMP_DATA_USERCMD_PARA;

		check = false;
		size = 0;
	}
};
#else
const BYTE STAMP_DATA_USERCMD_PARA = 51;
struct stStampDataUserCmd : public stDataUserCmd
{
	DWORD size;
	BYTE bin[0];
	stStampDataUserCmd()
	{
		byParam = STAMP_DATA_USERCMD_PARA;

		size = 0;
	}
};
/// 客户端进行外挂检测
const BYTE STAMP_CHECK_USERCMD_PARA = 52;
struct stStampCheckUserCmd : public stDataUserCmd
{
	DWORD flag;//0 关; 1 开
	stStampCheckUserCmd()
	{
		byParam = STAMP_CHECK_USERCMD_PARA;
		flag = 0;
	}
};
#endif 


/// 发送初始化信息结束
const BYTE ENDOFINITDATA_DATA_USERCMD_PARA = 255;
	struct stEndOfInitDataDataUserCmd : public stDataUserCmd{
		stEndOfInitDataDataUserCmd()
		{
			byParam = ENDOFINITDATA_DATA_USERCMD_PARA;
		}
	};
//////////////////////////////////////////////////////////////
/// 数据指令定义完成
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
/// 地图数据指令定义开始
//////////////////////////////////////////////////////////////
	struct stMapScreenUserCmd : public stNullUserCmd{
		stMapScreenUserCmd()
		{
			byCmd = MAPSCREEN_USERCMD;
		}
	};


/// 地图上增加人物
const BYTE ADDUSER_MAPSCREEN_USERCMD_PARA = 10;
	struct stAddUserMapScreenUserCmd : public stMapScreenUserCmd {
		stAddUserMapScreenUserCmd()
		{
			byParam = ADDUSER_MAPSCREEN_USERCMD_PARA;
		}
		t_MapUserData data;
	};

/// 地图上面删除人物
const BYTE REMOVEUSER_MAPSCREEN_USERCMD_PARA = 11;
	struct stRemoveUserMapScreenUserCmd : public stMapScreenUserCmd{
		stRemoveUserMapScreenUserCmd()
		{
			byParam = REMOVEUSER_MAPSCREEN_USERCMD_PARA;
		}

		DWORD dwUserTempID;			/**< 用户临时ID */
	};

/// 用户请求其他用户数据
const BYTE REQUESTUSERDATA_MAPSCREEN_USERCMD_PARA = 12;
	struct stRequestUserDataMapScreenUserCmd : public stMapScreenUserCmd{
		stRequestUserDataMapScreenUserCmd()
		{
			byParam = REQUESTUSERDATA_MAPSCREEN_USERCMD_PARA;
		}

		WORD size;
		DWORD dwUserTempID[0];	//临时编号列表(DWORD)
	};

/// 用户骑马
const BYTE RIDE_MAPSCREEN_USERCMD_PARA = 13;
struct stRideMapScreenUserCmd : public stMapScreenUserCmd
{
	stRideMapScreenUserCmd()
	{
		byParam = RIDE_MAPSCREEN_USERCMD_PARA;
	}
};

/// 刷新角色状态
const BYTE REFRESHSTATE_MAPSCREEN_USERCMD_PARA = 14;
struct stRefreshStateMapScreenUserCmd : public stMapScreenUserCmd{
	stRefreshStateMapScreenUserCmd()
	{
		byParam = REFRESHSTATE_MAPSCREEN_USERCMD_PARA;
		bzero(state , sizeof(state));
	}
	DWORD dwUserTempID;			/**< 用户临时ID */
	BYTE state[(MAX_STATE + 7) / 8];	/// 角色状态
	
};

/// 定义物品动作类型
enum{
	OBJECTACTION_DROP,			/// 掉落物品
	OBJECTACTION_UPDATE,		/// 更新物品数据
};

/// 在地图上增加物品
const BYTE ADDMAPOBJECT_MAPSCREEN_USERCMD_PARA = 20;
struct stAddMapObjectMapScreenUserCmd  : public stMapScreenUserCmd{
	stAddMapObjectMapScreenUserCmd()
	{       
		byParam = ADDMAPOBJECT_MAPSCREEN_USERCMD_PARA;
		bzero(&data,sizeof(data));
	}       
	BYTE    action; 
	t_MapObjectData data;
};


/// 在地图上删除物品
const BYTE REMOVEMAPOBJECT_MAPSCREEN_USERCMD_PARA = 21;
	struct stRemoveMapObjectMapScreenUserCmd  : public stMapScreenUserCmd{
		stRemoveMapObjectMapScreenUserCmd()
		{
			byParam = REMOVEMAPOBJECT_MAPSCREEN_USERCMD_PARA;
		}
		DWORD dwMapObjectTempID;	/**< 物品的临时ID */
	};

/// 在地图上增加建筑物
const BYTE ADDMAPBUILDING_MAPSCREEN_USERCMD_PARA = 30;
	struct stAddMapBuildingMapScreenUserCmd : public stMapScreenUserCmd{
		stAddMapBuildingMapScreenUserCmd()
		{
			byParam = ADDMAPBUILDING_MAPSCREEN_USERCMD_PARA;
		}

		t_MapBuildingData data;
	};


/// 在地图上删除建筑物
const BYTE REMOVEMAPBUILDING_MAPSCREEN_USERCMD_PARA = 31;
	struct stRemoveMapBuildingMapScreenUserCmd : public stMapScreenUserCmd{
		stRemoveMapBuildingMapScreenUserCmd()
		{
			byParam = REMOVEMAPBUILDING_MAPSCREEN_USERCMD_PARA;
		}

		DWORD dwMapBuildingTempID;	/**< 建筑物的临时ID */
	};
///人物使用易容术
const BYTE CHANGEFACE_MAPSCREEN_USERCMD_PARA = 32;
	struct stChangeFaceMapScreenUserCmd : public stMapScreenUserCmd{
		stChangeFaceMapScreenUserCmd()
		{
			byParam = CHANGEFACE_MAPSCREEN_USERCMD_PARA;
		}

		DWORD	dwUserTempID;		//用户临时ID
        DWORD	dwChangeFaceID;		//易容后的NPC的类型ID
	};

/// 在地图上增加NPC
const BYTE ADDMAPNPC_MAPSCREEN_USERCMD_PARA = 50;
	struct stAddMapNpcMapScreenUserCmd : public stMapScreenUserCmd{
		stAddMapNpcMapScreenUserCmd()
		{
			byParam = ADDMAPNPC_MAPSCREEN_USERCMD_PARA;
		}
		char name[MAX_NAMESIZE+1];
		t_MapNpcData data;
	};

/// 在地图上删除NPC
const BYTE REMOVEMAPNPC_MAPSCREEN_USERCMD_PARA = 51;
	struct stRemoveMapNpcMapScreenUserCmd : public stMapScreenUserCmd{
		stRemoveMapNpcMapScreenUserCmd()
		{
			byParam = REMOVEMAPNPC_MAPSCREEN_USERCMD_PARA;
		}
		DWORD dwMapNpcDataPosition;	/**< npc的数据位置 */
	};

/// 请求Npc数据
const BYTE REQUESTMAPNPCDATA_MAPSCREEN_USERCMD_PARA = 52;
	struct stRequestMapNpcDataMapScreenUserCmd : public stMapScreenUserCmd{
		stRequestMapNpcDataMapScreenUserCmd()
		{
			byParam = REQUESTMAPNPCDATA_MAPSCREEN_USERCMD_PARA;
		}

		WORD size;
		DWORD dwNpcTempID[0];	//临时编号列表(DWORD)
	};

///soke Npc血槽 现在突破血量显示
const BYTE NPCHP_MAPSCREEN_USERCMD_PARA = 53;
	struct stNPCHPMapScreenUserCmd : public stMapScreenUserCmd{
		stNPCHPMapScreenUserCmd()
		{
			byParam = NPCHP_MAPSCREEN_USERCMD_PARA;
		}

		DWORD dwMapNpcDataPosition;     /**< npc的数据位置 */
		DWORD dwHP;                                     /**< HP */
		DWORD dwMaxHP;                          /**< MaxHP */
	};

// NPC 人物 位置信息
struct stNpcPosition{
	DWORD dwTempID;				//目标编号
	WORD x;					/**< 坐标*/
	WORD y;				
	BYTE byDir;					// 方向（如果为 -1 则不确定方向）
};

// NPC 人物 TEMPID信息
struct stNpcTempID{
	DWORD dwTempID;             //目标编号
};

/// 对9屏用户和NPC
const BYTE ALL_MAPSCREEN_USERCMD_PARA = 54;
struct stAllMapScreenUserCmd : public stMapScreenUserCmd{
	stAllMapScreenUserCmd()
	{       
		byParam = ALL_MAPSCREEN_USERCMD_PARA;
	}       

	MapData_ItemHeader mdih;
	stNpcTempID psi[0]; 
};


struct t_MapUserDataPos :public t_MapUserData
{
	WORD x;                    /**< 坐标*/
	WORD y;        
	BYTE byDir;                 // 方向（如果为 -1 则不确定方向）
};

struct t_MapUserDataPosState :public t_UserData
{
	/*
	t_MapUserDataPosState()
	{
	num=0;
	} 
	// */
	WORD x;					/**< 坐标*/
	WORD y;				
	BYTE byDir;					// 方向（如果为 -1 则不确定方向）
	BYTE num; 		///带状态的数量
	WORD state[0];		///状态列表
};


struct t_MapNpcDataPos :public t_MapNpcData
{
	WORD x;                    /**< 坐标*/
	WORD y;        
	BYTE byDir;                 // 方向（如果为 -1 则不确定方向）
};
struct t_MapNpcDataPosState :public t_NpcData
{
	/*
	t_MapNpcDataPosState()
	{
	num=0;
	} 
	// */
	WORD x;					/**< 坐标*/
	WORD y;				
	BYTE byDir;					// 方向（如果为 -1 则不确定方向）
	BYTE num; 		///带状态的数量
	WORD state[0];		///状态列表
};

struct t_MapBuildingDataPos :public t_MapBuildingData
{
	WORD x;                    /**< 坐标*/
	WORD y;        
	BYTE byDir;                 // 方向（如果为 -1 则不确定方向）
};

// 地图数据打包发送
const BYTE MAPDATA_MAPSCREEN_USERCMD_PARA = 55;
struct stMapDataMapScreenUserCmd : public stMapScreenUserCmd{
	stMapDataMapScreenUserCmd()
	{       
		byParam = MAPDATA_MAPSCREEN_USERCMD_PARA;
	}       

	MapData_ItemHeader mdih;
	union   
	{       
		t_MapUserDataPos mud[0]; 
		t_MapNpcDataPos mnd[0]; 
		t_MapBuildingDataPos mbd[0]; 
		t_MapPetData	mpd[0];
	};      
};

//soke 这里是选中玩家显示的最大血量 因为没突破属性 所以不修改
#define SELECTED_HPMP_PROPERTY_USERCMD_PARA 56//通知选择自己的玩家hp和mp的变化
struct stRTSelectedHpMpPropertyUserCmd : public stMapScreenUserCmd
{       
	stRTSelectedHpMpPropertyUserCmd()                       
	{
		byParam = SELECTED_HPMP_PROPERTY_USERCMD_PARA;  
	}
	BYTE byType;//enumMapDataType                   
	DWORD dwTempID;//临时编号
	DWORD dwHP;//当前血                                     
	DWORD dwMaxHp;//最大hp                                  
	DWORD dwMP;//当前mp                                     
	DWORD dwMaxMp;//最大mp
};
//
//enum enumPetActionAi
//{
//	Pet_Action_Ai_Attack,	//攻击
//	Pet_Action_Ai_Follow,	//跟随
//	Pet_Action_Ai_Defend	//驻守
//};
//
//enum enumPetStatusAi
//{
//	Pet_Status_Ai_Initiative,	//主动
//	Pet_Status_Ai_Guard,		//护卫
//	Pet_Status_Ai_Rest			//休息
//};
//
//#define PET_INFO_USERCMD_PARA 57//通知选择自己的玩家hp和mp的变化
//struct stPetInfoUserCmd : public stMapScreenUserCmd
//{
//	stRTSelectedHpMpPropertyUserCmd()
//	{
//		byParam = SELECTED_HPMP_PROPERTY_USERCMD_PARA;
//	}
//	BYTE byType;//enumMapDataType
//	DWORD dwTempID;//临时编号
//	WORD wdHP;//当前血
//	WORD wdMaxHp;//最大hp
//	WORD wdMP;//当前mp
//	WORD wdMaxMp;//最大mp
//};
//

/// 清除地图上物品保护
const BYTE CLEAR_OBJECTOWNER_MAPSCREEN_USERCMD_PARA = 58;
struct stClearObjectOwnerMapScreenUserCmd  : public stMapScreenUserCmd{
	stClearObjectOwnerMapScreenUserCmd()
	{       
		byParam = CLEAR_OBJECTOWNER_MAPSCREEN_USERCMD_PARA;
	}       
	DWORD dwMapObjectTempID;    /**< 物品的临时ID */
};

/// 在地图上增加NPC和坐标
const BYTE ADDMAPNPC_AND_POS_MAPSCREEN_USERCMD_PARA = 59;
struct stAddMapNpcAndPosMapScreenUserCmd : public stMapScreenUserCmd{
	stAddMapNpcAndPosMapScreenUserCmd()
	{       
		byParam = ADDMAPNPC_AND_POS_MAPSCREEN_USERCMD_PARA;
	}       

	t_MapNpcDataPos data;
};


/// 地图上增加人物和坐标
const BYTE ADDUSER_AND_POS_MAPSCREEN_USERCMD_PARA = 60;
struct stAddUserAndPosMapScreenUserCmd : public stMapScreenUserCmd {
	stAddUserAndPosMapScreenUserCmd()
	{       
		byParam = ADDUSER_AND_POS_MAPSCREEN_USERCMD_PARA;
	}       
	t_MapUserDataPos data;
};

/// 批量删除NPC指令
const BYTE BATCHREMOVENPC_MAPSCREEN_USERCMD_PARA = 61;
struct stBatchRemoveNpcMapScreenUserCmd : public stMapScreenUserCmd {
	stBatchRemoveNpcMapScreenUserCmd()
	{       
		byParam = BATCHREMOVENPC_MAPSCREEN_USERCMD_PARA;
	}
	WORD	num;
	DWORD   id[0];
};

/// 批量删除USER指令
const BYTE BATCHREMOVEUSER_MAPSCREEN_USERCMD_PARA = 62;
struct stBatchRemoveUserMapScreenUserCmd : public stMapScreenUserCmd {
	stBatchRemoveUserMapScreenUserCmd()
	{       
		byParam = BATCHREMOVEUSER_MAPSCREEN_USERCMD_PARA;
	}
	WORD	num;
	DWORD   id[0];
};

/// 设置角色状态
const BYTE SETSTATE_MAPSCREEN_USERCMD_PARA = 63;
struct stSetStateMapScreenUserCmd : public stMapScreenUserCmd{
	stSetStateMapScreenUserCmd()
	{
		byParam = SETSTATE_MAPSCREEN_USERCMD_PARA;
	}
	BYTE    type;       /**<类型 enumMapDataType*/
	DWORD   dwUserTempID;           /**< 用户临时ID */
	WORD    wdState;    /// 需要设置的状态
};
/// 取消角色状态
const BYTE CLEARSTATE_MAPSCREEN_USERCMD_PARA = 64;
struct stClearStateMapScreenUserCmd : public stMapScreenUserCmd{
	stClearStateMapScreenUserCmd()
	{
		byParam = CLEARSTATE_MAPSCREEN_USERCMD_PARA;
	}
	BYTE    type;       /**<类型 enumMapDataType*/
	DWORD   dwUserTempID;           /**< 用户临时ID */
	WORD    wdState;    /// 需要取消的状态
};

/// 在地图上增加Npc和宠物数据
const BYTE ADDMAPNPCPET_MAPSCREEN_USERCMD_PARA = 65;
struct stAddMapNpcPetMapScreenUserCmd : public stMapScreenUserCmd{
	stAddMapNpcPetMapScreenUserCmd()
	{       
		byParam = ADDMAPNPCPET_MAPSCREEN_USERCMD_PARA;
	}       

	t_MapNpcDataPos data;
	t_MapPetData pet; 
};

/// 在地图上增加宠物数据
const BYTE ADDMAPPET_MAPSCREEN_USERCMD_PARA = 66;
struct stAddMapPetMapScreenUserCmd : public stMapScreenUserCmd{
	stAddMapPetMapScreenUserCmd()
	{       
		byParam = ADDMAPPET_MAPSCREEN_USERCMD_PARA;
	}       

	t_MapPetData pet; 
};

/// 设置角色善恶值
const BYTE GOODNESS_MAPSCREEN_USERCMD_PARA = 67;
struct stGoodnessStateMapScreenUserCmd : public stMapScreenUserCmd{
	stGoodnessStateMapScreenUserCmd()
	{
		byParam = GOODNESS_MAPSCREEN_USERCMD_PARA;
	}
	DWORD   dwTempID;           /**< 用户临时ID */
	DWORD   dwGoodness; /// 善恶值
};


/// 地图上增加人物
const BYTE ADDUSER_MAPSCREEN_STATE_USERCMD_PARA = 68;
struct stAddUserMapScreenStateUserCmd : public stMapScreenUserCmd {
	stAddUserMapScreenStateUserCmd()
	{
		byParam = ADDUSER_MAPSCREEN_STATE_USERCMD_PARA;
	}
	t_MapUserDataState data;
	int size()
	{
		return sizeof(*this) + data.num * sizeof(data.state[0]);
	}
};
/// 在地图上增加NPC
const BYTE ADDMAPNPC_MAPSCREEN_STATE_USERCMD_PARA = 69;
struct stAddMapNpcMapScreenStateUserCmd : public stMapScreenUserCmd{
	stAddMapNpcMapScreenStateUserCmd()
	{
		byParam = ADDMAPNPC_MAPSCREEN_STATE_USERCMD_PARA;
	}
	t_MapNpcDataState data;
	int size()
	{
		return sizeof(*this) + data.num * sizeof(data.state[0]);
	}
};
/// 地图上增加人物和坐标
const BYTE ADDUSER_AND_POS_MAPSCREEN_STATE_USERCMD_PARA = 70;
struct stAddUserAndPosMapScreenStateUserCmd : public stMapScreenUserCmd {
	stAddUserAndPosMapScreenStateUserCmd()
	{
		byParam = ADDUSER_AND_POS_MAPSCREEN_STATE_USERCMD_PARA;
	}
	t_MapUserDataPosState data;
	int size()
	{
		return sizeof(*this) + data.num * sizeof(data.state[0]);
	}
};
/// 在地图上增加NPC和坐标
const BYTE ADDMAPNPC_AND_POS_MAPSCREEN_STATE_USERCMD_PARA = 71;
struct stAddMapNpcAndPosMapScreenStateUserCmd : public stMapScreenUserCmd{
	stAddMapNpcAndPosMapScreenStateUserCmd()
	{
		byParam = ADDMAPNPC_AND_POS_MAPSCREEN_STATE_USERCMD_PARA;
	}
	t_MapNpcDataPosState data;
	int size()
	{
		return sizeof(*this) + data.num * sizeof(data.state[0]);
	}
};
/// 在地图上增加Npc和宠物数据
const BYTE ADDMAPNPCPET_MAPSCREEN_STATE_USERCMD_PARA = 72;
struct stAddMapNpcPetMapScreenStateUserCmd : public stMapScreenUserCmd{
	stAddMapNpcPetMapScreenStateUserCmd()
	{
		byParam = ADDMAPNPCPET_MAPSCREEN_STATE_USERCMD_PARA;
	}

	t_MapPetData pet; 
	t_MapNpcDataPosState data;
	int size()
	{
		return sizeof(*this) + data.num * sizeof(data.state[0]);
	}
};
//// 地图数据打包发送
//const BYTE MAPDATA_MAPSCREEN_STATE_USERCMD_PARA = 73;
//struct stMapDataMapScreenStateUserCmd : public stMapScreenUserCmd{
//	stMapDataMapScreenStateUserCmd()
//	{
//		byParam = MAPDATA_MAPSCREEN_STATE_USERCMD_PARA;
//	}
//
//	MapData_ItemHeader mdih;
//	union
//	{
//		t_MapUserDataPosState mud[0];
//		t_MapNpcDataPosState mnd[0];
//		t_MapBuildingDataPos mbd[0];
//		t_MapPetData mpd[0];
//	};
//};


//////////////////////////////////////////////////////////////
/// 地图数据指令定义结束
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
/// 魔法指令定义开始
//////////////////////////////////////////////////////////////
	struct stMagicUserCmd : public stNullUserCmd{
		stMagicUserCmd()
		{
			byCmd = MAGIC_USERCMD;
		}
	};

/// 定义攻击类型
enum ATTACKTYPE{
	ATTACKTYPE_U2U,	/// 用户攻击用户
	ATTACKTYPE_U2N,	/// 用户攻击Npc
	ATTACKTYPE_N2U,	/// Npc攻击用户
	ATTACKTYPE_U2B,	/// 用户攻击建筑
	ATTACKTYPE_U2P,	/// 用户攻击点
	ATTACKTYPE_N2N	/// Npc攻击Npc
};


#ifndef _ANI_TYPE_ENUM
#define _ANI_TYPE_ENUM
enum AniTypeEnum
{
	Ani_Null = 0,

	Ani_Wait,Ani_Stand = Ani_Wait,	// 站立
	Ani_Walk,						// 走
	Ani_Run,						// 跑

	Ani_Attack,						// 攻击
	Ani_Attack2,					// 攻击2
	Ani_Attack3,					// 攻击3

	Ani_Magic,						// 魔法攻击
	Ani_Magic2,						// 魔法攻击2
	Ani_Magic3,						// 魔法攻击3

	Ani_Bow,						// 射箭
	Ani_Bow2,						// 努攻击

	Ani_Hurt,						// 受伤(被击)
	Ani_Die,						// 死亡

	Ani_Sit,						// 打坐

	Ani_Ride_Wait,					// 骑马站立
	Ani_Ride_Walk,					// 骑马走
	Ani_Ride_Run,					// 骑马跑

	Ani_Ride_Attack,				// 骑马攻击
	Ani_Ride_Magic,					// 骑马施法

	Ani_Ride_Hurt,					// 受伤(被击)
	Ani_Ride_Die,					// 骑马死亡

	Ani_Appear,						// 出现(重生)

	Ani_Attack_Stand,				// 攻击站立
	Ani_Attack2_Stand,				// 攻击2站立
	Ani_Attack3_Stand,				// 攻击2站立

	Ani_Magic_Stand,				// 魔法攻击站立
	Ani_Magic2_Stand,				// 魔法2攻击站立
	Ani_Magic3_Stand,				// 魔法3攻击站立

	Ani_Bow_Stand,					// 弓箭攻击站立
	Ani_Bow2_Stand,					// 努攻击站立

	Ani_Ride_Attack_Stand,			// 骑马攻击站立
	Ani_Ride_Magic_Stand,			// 骑马施法站立

	Ani_Back,						// 后退
	Ani_Ride_Back,					// 骑马后退

	Ani_Ride_Bow,					// 骑马射箭
	Ani_Ride_Bow_Stand,				// 骑马射箭站立

	Ani_Fly_Walk,					// 飞行慢速
	Ani_Fly_Run,					// 飞行快速

	Ani_Fly_Hurt,					// 飞行被击
	Ani_Fly_Die,					// 飞行死亡

	Ani_Gather,						// 采药

	Ani_FirstRun,					// 第一次走

	Ani_Idle0,
	Ani_Idle1,
	Ani_Idle2,
	Ani_Idle3,

	Ani_Gather_Stand,               //采药站立
	Ani_OpenBox_Stand,              //开箱站立
	Ani_Fly					= 51,	// 飞行
	Ani_Dance				= 56,	// 跳舞
	Ani_Num,
};
#endif

/// 攻击指令
const BYTE MAGIC_USERCMD_PARA = 5;
struct stAttackMagicUserCmd : public stMagicUserCmd{

	stAttackMagicUserCmd()
	{
		byParam = MAGIC_USERCMD_PARA;
	}

	DWORD dwUserTempID;			/**< 攻击临时编号 */
	DWORD dwDefenceTempID;		/**< 防御临时编号 */

	WORD wdMagicType;			/**< 魔法类型 */

	WORD xDes;					/**< 目标点 */
	WORD yDes;					/**< 目标点 */
	BYTE byDirect;				/**< 攻击方向 */

	BYTE byAttackType;			/**< 攻击类型：ATTACKTYPE_U2U,ATTACKTYPE_U2N,ATTACKTYPE_N2U */
	BYTE byAction;				// 攻击动作  AniTypeEnum ( Ani_Null 为不确定的，由客户端确定 )
	DWORD dwTempIDList[0];		// 攻击目标列表
};

enum {
	RTMAGIC_FAILURE,		/// 攻击失败
	RTMAGIC_DUCK,           /// 防御闪避
	RTMAGIC_FREEZE,			/// 冰冻
	RTMAGIC_SPEEDUP,		/// 加速
	RTMAGIC_POISON,			/// 中毒
	RTMAGIC_PLAGUE,			/// 瘟疫
	RTMAGIC_LANDIFICATION,	/// 石化
	RTMAGIC_VAMPIRE,		/// 吸血
	RTMAGIC_VAMPIREMAGIC,	/// 吸魔
	RTMAGIC_HIDE,			/// 隐身
	RTMAGIC_BLANKSCREEN,	/// 黑屏
	RTMAGIC_SUCCESS,		/// 攻击成功
	RTMAGIC_ATTACKED,		/// 被击
	RTMAGIC_DEFENCE			/// 格挡
};

/// 攻击返回
const BYTE RTMAGIC_USERCMD_PARA = 6;
struct stRTMagicUserCmd : public stMagicUserCmd{

	stRTMagicUserCmd()
	{
		byParam = RTMAGIC_USERCMD_PARA;
	}

	DWORD dwUserTempID;			/**< 目标临时编号 */
	BYTE byTarget;				/**< 目标类型：enumMapDataType */
	BYTE byRetcode;				/**< 返回代码：RTMAGIC_SUCCESS,RTMAGIC_FAILURE... */
	BYTE byDirect;				/**< 方向 */
	BYTE bySrc;					/**< 攻击者类型：enumMapDataType */

	DWORD dwSrcTempID;			/**< 攻击者临时编号 */

	DWORD dwHP;					// 当前血
	long wdHP;					/**< 扣血 */
	BYTE byLuck;				//幸运，为1表示这是幸运攻击，为0表示普通攻击结果
};

/// 攻击返回给其他人
const BYTE RT_OTHER_MAGIC_USERCMD_PARA = 27;
struct stRTOtherMagicUserCmd : public stMagicUserCmd{

	stRTOtherMagicUserCmd()
	{
		byParam = RT_OTHER_MAGIC_USERCMD_PARA;
	}

	BYTE byTarget;              /**< 目标类型：enumMapDataType */
	DWORD dwUserTempID;         /**< 目标临时编号 */
	BYTE bySrc;                 /**< 攻击者类型：enumMapDataType */
	DWORD dwSrcTempID;          /**< 攻击者临时编号 */
};

/// 返回目标坐标
const BYTE RTMAGIC_POS_USERCMD_PARA	= 12;
struct stRTMagicPosUserCmd : public stMagicUserCmd{

	stRTMagicPosUserCmd()
	{
		byParam = RTMAGIC_POS_USERCMD_PARA;
	}
	BYTE byTarget;				/**< 目标类型：enumMapDataType */
	stNpcPosition pos;
};

/// 防御方后退
const BYTE BACKOFF_USERCMD_PARA	= 7;
	struct stBackOffMagicUserCmd : public stMagicUserCmd {
		stBackOffMagicUserCmd()
		{
			byParam = BACKOFF_USERCMD_PARA;
		}

		DWORD dwTempID;				/**< 防御方临时编号 */
		BYTE byType;				/**< 目标类型 enumMapDataType */
		BYTE byDirect;				/**< 后退的方向 */
		DWORD x;
		DWORD y;
	};

/// 攻击Npc得到经验
const BYTE OBTAINEXP_USERCMD_PARA = 8;
	struct stObtainExpUserCmd : public stMagicUserCmd{
		stObtainExpUserCmd()
		{
			byParam = OBTAINEXP_USERCMD_PARA;
		}

		DWORD dwTempID;				/**< 经验值来源临时编号 */
		BYTE byType;				/**< 经验值来源 enumMapDataType */
		uint64_t dwExp;				/**< sky 得到的经验 */
		uint64_t dwUserExp;			/**< sky 人物当前经验值的经验 */
	};

/// Npc死亡
const BYTE NPCDEATH_USERCMD_PARA = 9;
	struct stNpcDeathUserCmd : public stMagicUserCmd{
		stNpcDeathUserCmd()
		{
			byParam = NPCDEATH_USERCMD_PARA;
		}

		DWORD dwNpcTempID;			/**< Npc临时编号 */
	};

/// 人物等级提升
const BYTE LEVELUP_USERCMD_PARA	= 10;
	struct stLevelUpUserCmd : public stMagicUserCmd{
		stLevelUpUserCmd()
		{
			byParam = LEVELUP_USERCMD_PARA;
		}

		DWORD dwUserTempID;			/**< 用户临时编号 */
	};

/*enum {
	PKMODE_NORMAL,		//和平模式
	PKMODE_ENTIRE,		//全体模式
	PKMODE_TEAM,		//组队模式
	PKMODE_TONG,		//帮会模式
	PKMODE_SEPT,		//家族模式
	PKMODE_SCHOOL,		//师门模式
	PKMODE_COUNTRY,		//国家模式
	PKMODE_CHALLENGE,	//挑战模式
	PKMODE_NUM
};*/
enum {
	PKMODE_NORMAL,		//和平模式
	PKMODE_ENTIRE,		//全体模式
	PKMODE_TEAM,		//组队模式
	PKMODE_TONG,		//帮会模式
	PKMODE_SEPT,		//家族模式
	//	PKMODE_SCHOOL,		//师门模式
	PKMODE_COUNTRY,		//国家模式
	//	PKMODE_CHALLENGE,	//挑战模式
	PKMODE_GOODNESS,	//善恶模式
	PKMODE_ALLY,		//国家联盟模式
	PKMODE_NUM,			//最大值
};

//PK模式切换
const BYTE PKMODE_USERCMD_PARA = 20;
	struct stPKModeUserCmd : public stMagicUserCmd{
		stPKModeUserCmd()
		{
			byParam = PKMODE_USERCMD_PARA;
		}
		BYTE byPKMode;
	};

//道具经验值
const BYTE OBJECT_EXP_USERCMD_PARA = 22;
	struct stObjectExpUserCmd : public stMagicUserCmd{
		stObjectExpUserCmd()
		{
			byParam = OBJECT_EXP_USERCMD_PARA;
		}
		DWORD id;	
		DWORD exp;	
	};

enum enmUnStateType{
	UN_STATE_COMBIN,
	UN_STATE_CHANGE_FACE,
	UN_STATE_TEAM_ATTACK_BIRD = USTATE_TEAM_ATTACK_BIRD_FLAG,
	UN_STATE_TEAM_ATTACK_FLOW = USTATE_TEAM_ATTACK_FLOW_CLOUD,
	UN_STATE_TEAM_ATTACK_ONE_DIM = USTATE_TEAM_ATTACK_ONE_DIM,
	UN_STATE_NUM,
	UN_STATE_PLAY_FLY
};
//解除合体状态
const BYTE UNCOMBIN_USERCMD_PARA = 23;
	struct stUnCombinUserCmd : public stMagicUserCmd{
		stUnCombinUserCmd()
		{
			byParam = UNCOMBIN_USERCMD_PARA;
		}
		enum enmUnStateType type;
	};

//燃放焰火
const BYTE FIREWORK_USERCMD_PARA = 24;
struct stFireWorkUserCmd : public stMagicUserCmd{
	stFireWorkUserCmd()
	{
		byParam = FIREWORK_USERCMD_PARA;
	}

	QWORD qwObjectTempID;	//焰火道具ID
	BYTE byType;		//
	DWORD dwUserID;		//焰火燃放者ID
	DWORD dwFireID;		//焰火ID
};

//时间同步消息
const BYTE TIMESYNC_USERCMD_PARA = 25;
struct stTimeSyncUserCmd : public stMagicUserCmd{
	stTimeSyncUserCmd()
	{
		byParam = TIMESYNC_USERCMD_PARA;
	}

	time_t serverTime;	//服务器时间
};


enum enumPopEffect
{
	POP_HP,
	POP_MP,
	POP_NUM,
};
const BYTE OBJECT_HPMP_POP_PARA = 11;
struct stObjectHpMpPopUserCmd : public stMagicUserCmd{

	stObjectHpMpPopUserCmd()
	{
		byParam = OBJECT_HPMP_POP_PARA;
	}

	DWORD dwUserTempID;			/**< 目标临时编号 */
	BYTE byTarget;				/**< 目标类型：enumMapDataType */

	int vChange;				/**< >0 :加		<0 :扣*/
	enumPopEffect	type;
};
//////////////////////////////////////////////////////////////
/// 魔法指令定义结束
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
/// 复活指令定义开始
//////////////////////////////////////////////////////////////
	struct stReliveUserCmd : public stNullUserCmd{
		stReliveUserCmd()
		{
			byCmd = RELIVE_USERCMD;
		}
	};

	/// 主用户死亡指令
	const BYTE MAINUSERDEATH_RELIVE_USERCMD_PARA = 1;
	struct stMainUserDeathReliveUserCmd : public stReliveUserCmd{
		stMainUserDeathReliveUserCmd()
		{
			byParam = MAINUSERDEATH_RELIVE_USERCMD_PARA;
			dwUserTempID = 0;
			deathType = 0;
		}

		DWORD dwUserTempID;         /**< 用户临时ID */
		DWORD deathType;    //死亡类型（原因） 0:普通 1:恶意PK
	};

	/// 主用户复活指令
	const BYTE MAINUSERRELIVE_RELIVE_USERCMD_PARA = 2;
	struct stMainUserReliveReliveUserCmd : public stReliveUserCmd{
		stMainUserReliveReliveUserCmd()
		{
			byParam = MAINUSERRELIVE_RELIVE_USERCMD_PARA;
		}

		DWORD dwUserTempID;			/**< 用户临时ID */

		DWORD x,y;					// 坐标
	};

	enum
	{
		ReliveHome,//回城复活
		ReliveMoney,//金钱复活
		ReliveMaxey,//soke 原地满状态复活
		ReliveSkill,//技能复活
		Relive_1_min,//追捕凶犯1分钟
		Relive_5_min,//追捕凶犯5分钟
		Relive_10_min//追捕凶犯10分钟
	};
	/// 死亡动画播放完毕
	const BYTE OK_RELIVE_USERCMD_PARA = 3;
	struct stOKReliveUserCmd : public stReliveUserCmd{
		stOKReliveUserCmd()
		{
			byParam = OK_RELIVE_USERCMD_PARA;
		}

		DWORD dwUserTempID;			/**< 用户临时ID */
		BYTE  byType;				/**< 复活方式 */
	};
//////////////////////////////////////////////////////////////
/// 复活指令定义结束
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
/// 用户移动指令定义开始
//////////////////////////////////////////////////////////////
	struct stMoveUserCmd : public stNullUserCmd{
		stMoveUserCmd()
		{
			byCmd = MOVE_USERCMD;
		}
	};

/// 移动方向的定义
enum {
	_DIR_UP			= 0,	/// 向上
	_DIR_UPRIGHT	= 1,	/// 右上
	_DIR_RIGHTUP	= 1,	/// 右上
	_DIR_RIGHT		= 2,	/// 向右
	_DIR_RIGHTDOWN	= 3,	/// 右下
	_DIR_DOWNRIGHT	= 3,	/// 右下
	_DIR_DOWN		= 4,	/// 向下
	_DIR_DOWNLEFT	= 5,	/// 左下
	_DIR_LEFTDOWN	= 5,	/// 左下
	_DIR_LEFT		= 6,	/// 向左
	_DIR_LEFTUP		= 7,	/// 左上
	_DIR_UPLEFT		= 7,	/// 左上
	_DIR_WRONG		= 8		/// 错误方向
};
/// 用户移动
const BYTE USERMOVE_MOVE_USERCMD_PARA = 2;
	struct stUserMoveMoveUserCmd : public stMoveUserCmd {
		stUserMoveMoveUserCmd()
		{
			byParam = USERMOVE_MOVE_USERCMD_PARA;
		}

		DWORD dwUserTempID;			/**< 用户临时编号 */

		BYTE byDirect;				/**< 移动方向 */
		BYTE bySpeed;				/**< 移动速度 */
		WORD x;					/**< 目的坐标 */
		WORD y;
	};

//soke 用户瞬移
const BYTE USERJUMPTO_USERCMD_PARA = 3;
  struct stUserJumptoUserCmd : public stMoveUserCmd
  {
	  stUserJumptoUserCmd()
	  {
		 byParam = USERJUMPTO_USERCMD_PARA;
	  }
	  WORD x;
	  WORD y;
  };

/// 网关向主用户发送的瞬移指令
const BYTE USERINSTANTJUMP_MOVE_USERCMD_PARA = 10;
	struct stUserInstantJumpMoveUserCmd : public stMoveUserCmd{
		stUserInstantJumpMoveUserCmd()
		{
			byParam = USERINSTANTJUMP_MOVE_USERCMD_PARA;
		}

		DWORD dwUserTempID;			/**< 用户临时ID */

		DWORD dwNextScreenPosition;	/**< 人所到达的屏位置 */
		WORD wdNextOffsetX;			/**< 人所到达的屏的偏移X */
		WORD wdNextOffsetY;			/**< 人所到达的屏的偏移Y */
	};

/// 用户进入地图指令命令
/// 目前只应用于进出总坛
const BYTE USERGOTOMAP_MOVE_USERCMD_PARA = 12;
struct stUserGotoMapMoveUserCmd : public stMoveUserCmd
{
	stUserGotoMapMoveUserCmd()
	{
		byParam = USERGOTOMAP_MOVE_USERCMD_PARA;
	}
	WORD IsBackToCity;
	DWORD dwMapType;
	char mapName[MAX_NAMESIZE];
};

/// Npc移动指令
const BYTE NPCMOVE_MOVE_USERCMD_PARA = 20;
	struct stNpcMoveMoveUserCmd : public stMoveUserCmd{
		stNpcMoveMoveUserCmd()
		{
			byParam = NPCMOVE_MOVE_USERCMD_PARA;
		}

		DWORD dwNpcTempID;			/**< Npc临时编号 */

		BYTE byDirect;				/**< 移动方向 */
		BYTE bySpeed;				/**< 移动的速度 */
		WORD x;					/**< 目的坐标 */
		WORD y;
	};

/// 请求获取大陆列表
const BYTE REQUESTLANDMASS_MOVE_USERCMD_PARA = 30;
	struct stRequestLandMassMoveUserCmd : public stMoveUserCmd{
		stRequestLandMassMoveUserCmd()
		{
			byParam = REQUESTLANDMASS_MOVE_USERCMD_PARA;
		}
	};

struct t_LandMass {
	BYTE groupName[MAX_NAMESIZE];   /**< 大陆名称 */
	BYTE countryName[MAX_NAMESIZE]; /**< 国家名称 */
	BYTE cityName[MAX_NAMESIZE];    /**< 城市名称 */
	WORD wdOnlineUser;          /**< 大陆在线人数 */
	BYTE byAvailable;           /**< 大陆是否可用 */
	DWORD price;				// 跳转费用
};

/// 服务器返回大陆列表
const BYTE RESPONSELANDMASS_MOVE_USERCMD_PARA = 31;
	struct stResponseLandMassMoveUserCmd : public stMoveUserCmd{
		stResponseLandMassMoveUserCmd()
		{
			byParam = RESPONSELANDMASS_MOVE_USERCMD_PARA;
		}

		int size;
		//列表
	};

/// 跳转大陆
const BYTE LANDMASSJUMP_MOVE_USERCMD_PARA = 32;
	struct stLandMassJumpMoveUserCmd : public stMoveUserCmd{
		stLandMassJumpMoveUserCmd()
		{
			byParam = LANDMASSJUMP_MOVE_USERCMD_PARA;
		}
	};

/// 打坐
const BYTE SITDOWN_MOVE_USERCMD_PARA = 33;
struct stSitDownMoveUserCmd : public stMoveUserCmd{
	stSitDownMoveUserCmd()
	{
		byParam = SITDOWN_MOVE_USERCMD_PARA;
	}
};

//////////////////////////////////////////////////////////////
/// 用户移动指令定义结束
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// 选择指令定义开始
//////////////////////////////////////////////////////////////
struct stSelectUserCmd : public stNullUserCmd
{
	stSelectUserCmd()
	{
		byCmd = SELECT_USERCMD;
	}
};

#define CHARBASE_OK 1       ///已经登陆过游戏
#define CHARBASE_FORBID 2   ///角色被封
#define CHARBASE_VIP 4		///收费用户(充过值)
#define CHARBASE_EXP125	8	///1.25倍经验状态
#define CHARBASE_EXP150	16	///1.50倍经验状态
#define CHARBASE_EXP175	32	///1.75倍经验状态
#define CHARBASE_DANCE	64	///跳舞经验翻倍状态
#define CHARBASE_FUBEN	128	///副本状态
#define CHARBASE_LUNCKY	256	///幸运丹状态

/// 最大角色信息个数
const WORD MAX_CHARINFO = 2;
/// 角色信息
struct SelectUserInfo
{
	DWORD id;                       /// 角色编号
	char  name[MAX_NAMESIZE+1];     /// 角色名称
	WORD type;                      /// 角色类型
	WORD level;                     /// 角色等级
	DWORD mapid;                    /// 角色所在地图编号
	char  mapName[MAX_NAMESIZE+1];  /// 角色所在地图名称
	WORD country;                   /// 国家ID
	WORD face;
	char  countryName[MAX_NAMESIZE+1];  /// 国家名称
	DWORD bitmask;                  /// 角色掩码
	//  WORD five;                      /// 五行主属性
	//  DWORD unionid;        /// 帮会ID
};


const BYTE USERINFO_SELECT_USERCMD_PARA = 1;
struct stUserInfoUserCmd : public stSelectUserCmd
{
	stUserInfoUserCmd()
	{
		byParam = USERINFO_SELECT_USERCMD_PARA;
		bzero(charInfo, sizeof(charInfo));
		size = 0;
	}
	SelectUserInfo charInfo[MAX_CHARINFO];
	WORD size;
	BYTE data[0];
};

/// 请求创建用户档案
const BYTE CREATE_SELECT_USERCMD_PARA = 2;
struct stCreateSelectUserCmd : public stSelectUserCmd
{
	stCreateSelectUserCmd()
	{
		byParam = CREATE_SELECT_USERCMD_PARA;
	}

	char strUserName[MAX_NAMESIZE];	/**< 用户名字  */
	WORD charType;
	BYTE byHairType;			/**< 头发类型 */
	DWORD byRGB;				/**< 颜色RGB */
	WORD country;				/**< 国家ID */
	WORD five;					/**< 五行主属性 */
};

/// 请求登陆
const BYTE LOGIN_SELECT_USERCMD_PARA = 3;
struct stLoginSelectUserCmd : public stSelectUserCmd
{
	stLoginSelectUserCmd()
	{
		byParam = LOGIN_SELECT_USERCMD_PARA;
	}

	DWORD charNo;
	char jpegPassport[7];				/**< 图形验证码 */
};

/// 请求删除帐号
const BYTE DELETE_SELECT_USERCMD_PARA = 4;
struct stDeleteSelectUserCmd : public stSelectUserCmd
{
	stDeleteSelectUserCmd()
	{
		byParam = DELETE_SELECT_USERCMD_PARA;
		bzero(numPassword,sizeof(numPassword));
	}

	DWORD   charNo;
	char    numPassword[MAX_NUMPASSWORD];
};

/// 请求检查角色名
const BYTE CHECKNAME_SELECT_USERCMD_PARA = 5;
struct stCheckNameSelectUserCmd : public stSelectUserCmd
{
	stCheckNameSelectUserCmd()
	{
		byParam = CHECKNAME_SELECT_USERCMD_PARA;
		name[0] = 0;
		err_code = 0;
	}

	char name[MAX_NAMESIZE];	/**< 用户名字  */
	BYTE err_code; //返回时的信息 0 没有错误 1 名字重复 2 名字包含不合法的内容
};

/// 返回删除角色失败
const BYTE RETURN_DELETE_SELECT_USERCMD_PARA = 6;
struct stReturnDeleteSelectUserCmd : public stSelectUserCmd
{
	stReturnDeleteSelectUserCmd()
	{
		byParam = RETURN_DELETE_SELECT_USERCMD_PARA;
	}

	char name[MAX_NAMESIZE];	/**< 角色名字  */
	BYTE err_code; //返回时的信息 1: 你是帮主，请解散帮会后再删除角色
	//             2: 你是师尊，请解散师门后再删除角色
	//             3: 你是族长，请解散家族后再删除角色
	//             4: 操作异常，请重试
};
/// 请求国家列表
const BYTE REQUEST_COUNTRY_SELECT_USERCMD_PARA = 7;
struct stRequestSelectUserCmd : public stSelectUserCmd
{
	stRequestSelectUserCmd()
	{
		byParam = REQUEST_COUNTRY_SELECT_USERCMD_PARA;
	}

};
/// 请求删除帐号时数字密码错误
const BYTE DELETE_ERROR_SELECT_USERCMD_PARA = 8;
struct stDeleteErrorSelectUserCmd : public stSelectUserCmd
{               
	stDeleteErrorSelectUserCmd()
	{           
		byParam = DELETE_ERROR_SELECT_USERCMD_PARA;
	}               

};
//////////////////////////////////////////////////////////////
// 选择指令定义结束
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
// 用户属性指令定义开始
//////////////////////////////////////////////////////////////
#ifndef _PROPERTY_USERCMD_DEFINITION_
#define _PROPERTY_USERCMD_DEFINITION_
	struct stPropertyUserCmd : public stNullUserCmd{
		stPropertyUserCmd()
		{
			byCmd = PROPERTY_USERCMD;
		}
	};
/// 定义装备格子类型

enum {
	EQUIPCELLTYPE_NONE=0,       /// 不能装备
	EQUIPCELLTYPE_HELM=1,       /// 头盔
	EQUIPCELLTYPE_BODY=2,       /// 服装
	EQUIPCELLTYPE_HANDR=3,		/// 右手
	EQUIPCELLTYPE_HANDL=4,		/// 左手
	EQUIPCELLTYPE_NECKLACE=5,	/// 项链
	EQUIPCELLTYPE_GLOVES=6,		/// 手套,手镯
	EQUIPCELLTYPE_RING=7,       /// 戒指
	EQUIPCELLTYPE_BELT=8,       /// 腰带
	EQUIPCELLTYPE_SHOES=9,		/// 鞋子
	EQUIPCELLTYPE_OTHER=10,		/// 特殊物品,比如马匹圣石
	EQUIPCELLTYPE_PACKAGE=11,		/// 装备的包裹
	EQUIPCELLTYPE_MAKE=12,		 /// 合成、升级，镶嵌的包裹
	EQUIPCELLTYPE_ADORN=13,       /// 装饰品
	EQUIPCELLTYPE_FASHION=15,	//时装
	EQUIPCELLTYPE_AMULET=20,	/// 护身符
	EQUIPCELLTYPE_HORSESHOE=31,	/// 马脚
	EQUIPCELLTYPE_HORSEROPE=32,	/// 马绳
	EQUIPCELLTYPE_HORSESADDLE=33,	/// 马鞍
	EQUIPCELLTYPE_HORSESAFE=34,	/// 马甲
	EQUIPCELLTYPE_HORSEIRON=35,	/// 马镫
	EQUIPCELLTYPE_HORSEFASHION=36,	/// 马匹圣石
	EQUIPCELLTYPE_CBAMULET=37,	/// 翅膀
	EQUIPCELLTYPE_XZAMULET=38,	/// 段位勋章
	EQUIPCELLTYPE_SBAMULET=39,	/// 神兵

	EQUIPCELLTYPE_SSAMULET=41,	/// 神兽
	EQUIPCELLTYPE_SHANGFANGBAOJIAN=42,
	EQUIPCELLTYPE_ZIJINLONGPAO=43,

	EQUIPCELLTYPE_FABAO=44,	/// 法宝
	EQUIPCELLTYPE_FABAOFJ1=45,	/// 法宝附件1
	EQUIPCELLTYPE_FABAOFJ2=46,	/// 法宝附件2
	EQUIPCELLTYPE_FABAOFJ3=47,	/// 法宝附件3
	EQUIPCELLTYPE_FABAOFJ4=48,	/// 法宝附件4
	EQUIPCELLTYPE_FABAOFJ5=49,	/// 法宝附件5
	EQUIPCELLTYPE_FABAOFJ6=57,	/// 法宝附件6
	EQUIPCELLTYPE_FABAOFJ7=51,	/// 法宝附件7
	EQUIPCELLTYPE_FABAOFJ8=52,	/// 法宝附件8
	EQUIPCELLTYPE_FABAOFJ9=53,	/// 法宝附件9
	EQUIPCELLTYPE_FABAOFJ10=54,	/// 法宝附件10
	EQUIPCELLTYPE_FABAOFJ11=55,	/// 法宝附件11
	EQUIPCELLTYPE_FABAOFJ12=56,	/// 法宝附件12



	EQUIPCELLTYPE_TUJIAN1 =101,   //soke 百兽图鉴图鉴格子开始  
	EQUIPCELLTYPE_TUJIAN2 =102,
	EQUIPCELLTYPE_TUJIAN3 =103,
	EQUIPCELLTYPE_TUJIAN4 =104,
	EQUIPCELLTYPE_TUJIAN5 =105,
	EQUIPCELLTYPE_TUJIAN6 =106,
	EQUIPCELLTYPE_TUJIAN7 =107,
	EQUIPCELLTYPE_TUJIAN8 =108,
	EQUIPCELLTYPE_TUJIAN9 =109,
	EQUIPCELLTYPE_TUJIAN10 =110,
	EQUIPCELLTYPE_SEXX1=111,	/// 十二星宿白羊座
	EQUIPCELLTYPE_SEXX2=112,	/// 十二星宿金牛座
	EQUIPCELLTYPE_SEXX3=113,	/// 十二星宿双子座
	EQUIPCELLTYPE_SEXX4=114,	/// 十二星宿巨蟹座
	EQUIPCELLTYPE_SEXX5=115,	/// 十二星宿狮子座
	EQUIPCELLTYPE_SEXX6=116,	/// 十二星宿处女座
	EQUIPCELLTYPE_SEXX7=117,	/// 十二星宿天秤座
	EQUIPCELLTYPE_SEXX8=118,	/// 十二星宿天蝎座
	EQUIPCELLTYPE_SEXX9=119,	/// 十二星宿射手座
	EQUIPCELLTYPE_SEXX10=120,/// 十二星宿摩羯座
	EQUIPCELLTYPE_SEXX11=121,/// 十二星宿水瓶座
	EQUIPCELLTYPE_SEXX12=122,/// 十二星宿双鱼座

	EQUIPCELLTYPE_YUANSHEN=123,	/// 元神
	EQUIPCELLTYPE_YUANSHENFJ1=124,	/// 元神
	EQUIPCELLTYPE_YUANSHENFJ2=125,	/// 元神
	EQUIPCELLTYPE_YUANSHENFJ3=126,	/// 元神
	EQUIPCELLTYPE_YUANSHENFJ4=127,	/// 元神
	EQUIPCELLTYPE_YUANSHENFJ5=128,	/// 元神
	EQUIPCELLTYPE_YUANSHENFJ6=129,	/// 元神
	
	EQUIPCELLTYPE_BABYFJ1=130,	/// 宝宝装备
	EQUIPCELLTYPE_BABYFJ2=131,	/// 宝宝装备
	EQUIPCELLTYPE_BABYFJ3=132,	/// 宝宝装备
	EQUIPCELLTYPE_BABYFJ4=133,	/// 宝宝装备
	EQUIPCELLTYPE_BABYFJ5=134,	/// 宝宝装备
	EQUIPCELLTYPE_BABYFJ6=135,	/// 宝宝装备

	EQUIPCELLTYPE_PFAMULET=136,	/// 披风
	EQUIPCELLTYPE_MAX,
};

enum{
	EQUIPCELLTYPE_LEFT,			///左边
	EQUIPCELLTYPE_RIGHT,		///右边
};

///得到另一只手
#define otherHand(hand)	(hand)^3
///得到另一只手镯
#define otherBangle(bangle) (bangle)^1

/// 定义装备动作类型
enum{
	EQUIPACTION_INIT,		/// 初始装备
	EQUIPACTION_MOVE,		/// 移动装备
	EQUIPACTION_SHATTER,	/// 装备损坏
	EQUIPACTION_OBTAIN,		/// 装备获得
	EQUIPACTION_DROP,		/// 装备丢弃
	EQUIPACTION_REFRESH,	/// 刷新装备
};

/// 定义物品格子类型
enum{
	OBJECTCELLTYPE_NONE,		/// 0不是格子，用于丢弃或捡到物品
	OBJECTCELLTYPE_COMMON,		/// 1普通物品格子
	OBJECTCELLTYPE_EQUIP,		/// 2装备
	OBJECTCELLTYPE_MOUSE,	    /// 3鼠标
	OBJECTCELLTYPE_TRADE,		/// 4自己的交易格子
	OBJECTCELLTYPE_OTHERTRADE,	/// 5对方的交易格子
	OBJECTCELLTYPE_BANK,		/// 6银行
	OBJECTCELLTYPE_SELL,		/// 7卖
	OBJECTCELLTYPE_STORE,		/// 8仓库
	OBJECTCELLTYPE_EQUIPSHOW,	/// 9非自己穿着的装备
	OBJECTCELLTYPE_PACKAGE,		/// 10包裹的格子
	OBJECTCELLTYPE_MAKE,		/// 11合成、升级，镶嵌的格子
	OBJECTCELLTYPE_MYSHOP,		/// 12自己摊位的格子
	OBJECTCELLTYPE_OTHERSSHOP,	/// 13别的万家摊位的格子
	OBJECTCELLTYPE_MAIL,		/// 14邮件系统的格子
	OBJECTCELLTYPE_COUNTRY_SAVEBOX, /// 15国家倉库
	OBJECTCELLTYPE_PET,          /// 16宠物包裹
	OBJECTCELLTYPE_RECAST,	     /// 17打造、合成随身包裹的格子
	OBJECTCELLTYPE_HANDBOOK,	 /// 18图鉴改造的格子
	OBJECTCELLTYPE_ANNEX,	     /// 19附件改造的格子
	OBJECTCELLTYPE_COWRYBOX,	 ///20宝箱格子
    OBJECTCELLTYPE_MALL,	     ///21新商城商品格子	
	OBJECTCELLTYPE_FABAO,     ///22 法宝进阶格子
	OBJECTCELLTYPE_FUJIAN,
	OBJECTCELLTYPE_YUANSHEN,     ///22 元神进阶格子
};


/// 定义升级格子类型
enum {
	MAKECELLTYPE_EQUIP=0,         /// 被合成、升级，镶嵌的物品
	MAKECELLTYPE_MATERIAL1=1,       /// 材料1
	MAKECELLTYPE_MATERIAL2=2,       /// 材料2
	MAKECELLTYPE_MATERIAL3=3,       /// 材料3
	MAKECELLTYPE_MATERIAL4=4,       /// 材料4
};

enum {
	COWRYBOX_TALIE0,
	COWRYBOX_TALIE1,
	COWRYBOX_TALIE2,
	COWRYBOX_TALIE3,
	COWRYBOX_TALIE4,
	COWRYBOX_TALIE5,
	COWRYBOX_TALIE6,
	COWRYBOX_TALIE7,
	COWRYBOX_TALIE8,
	COWRYBOX_TALIE9,
	COWRYBOX_TALIE10,
	COWRYBOX_TALIE11,
	COWRYBOX_TALIE12,
	COWRYBOX_TALIE13,
	COWRYBOX_TALIE14,
	COWRYBOX_TALIE15,
	COWRYBOX_TALIE16,
};

/// 用户道具数据
#define ADDUSEROBJECT_PROPERTY_USERCMD_PARAMETER 1
	struct stAddObjectPropertyUserCmd : public stPropertyUserCmd{
		stAddObjectPropertyUserCmd()
		{
			byParam = ADDUSEROBJECT_PROPERTY_USERCMD_PARAMETER;
		}
		BYTE byActionType;			/**< 物品动作类型 */
		t_Object object;			/**< 物品数据 */
	};

/// 删除道具数据
#define REMOVEUSEROBJECT_PROPERTY_USERCMD_PARAMETER 2
	struct stRemoveObjectPropertyUserCmd : public stPropertyUserCmd {
		stRemoveObjectPropertyUserCmd()
		{
			byParam = REMOVEUSEROBJECT_PROPERTY_USERCMD_PARAMETER;
		}
		DWORD qwThisID;				/**< 物品唯一ID */
	};

/// 交换（移动）用户物品
#define SWAPUSEROBJECT_PROPERTY_USERCMD_PARAMETER 3
	struct stSwapObjectPropertyUserCmd : public stPropertyUserCmd{
		stSwapObjectPropertyUserCmd()
		{
			byParam = SWAPUSEROBJECT_PROPERTY_USERCMD_PARAMETER;
		}
		DWORD qwThisID;				/**< 物品唯一ID */
		stObjectLocation	dst;	/**< 物品目的位置 如果目的位置上有物品，则交换*/
	};

/// 分裂物品指令（客户端自动减少源物品的数量）
#define SPLITUSEROBJECT_PROPERTY_USERCMD_PARAMETER 4
	struct stSplitObjectPropertyUserCmd : public stPropertyUserCmd{
		stSplitObjectPropertyUserCmd()
		{
			byParam = SPLITUSEROBJECT_PROPERTY_USERCMD_PARAMETER;
		}
		DWORD qwThisID;				/**< 物品唯一ID */
		DWORD qwNewThisID;			/**< 新物品的唯一ID */
		DWORD dwNum;				/**< 分裂的数量 */
		stObjectLocation	dst;	/**< 物品目的位置*/
	};

/// 合并物品
#define UNIONUSEROBJECT_PROPERTY_USERCMD_PARAMETER 5
	struct stUnionObjectPropertyUserCmd : public stPropertyUserCmd{
		stUnionObjectPropertyUserCmd()
		{
			byParam = UNIONUSEROBJECT_PROPERTY_USERCMD_PARAMETER;
		}
		DWORD qwSrcThisID;			/**< 源物品唯一ID */
		DWORD qwDstThisID;			/**< 目标物品唯一ID */
	};

/// 设置格子中的物品数量（用于合并物品）
#define REFCOUNTOBJECT_PROPERTY_USERCMD_PARAMETER 6
	struct stRefCountObjectPropertyUserCmd : public stPropertyUserCmd{
		stRefCountObjectPropertyUserCmd()
		{
			byParam = REFCOUNTOBJECT_PROPERTY_USERCMD_PARAMETER;
		}
		DWORD qwThisID;				/**< 物品唯一ID */
		DWORD dwNum;				/**< 数量 */
	};

/// 用户使用物品
#define USEUSEROBJECT_PROPERTY_USERCMD_PARAMETER 7
	struct stUseObjectPropertyUserCmd : public  stPropertyUserCmd{
		stUseObjectPropertyUserCmd()
		{
			byParam = USEUSEROBJECT_PROPERTY_USERCMD_PARAMETER;
		}
		DWORD qwThisID;				/**< 物品唯一ID */
		DWORD dwNumber;				/**< 使用的数量 */
	};


/// 用户捡地图上的物品
#define PICKUPITEM_PROPERTY_USERCMD_PARA 8
	struct stPickUpItemPropertyUserCmd : public stPropertyUserCmd {
		stPickUpItemPropertyUserCmd()
		{
			byParam = PICKUPITEM_PROPERTY_USERCMD_PARA;
		}

		DWORD x;
		DWORD y;
	};

#define FOUNDITEM_PROPERTY_USERCMD_PARA  9
//soke 装备升级指令
#define MAKELV_PROPERTY_USERCMD_PARA 48
	struct stFoundItemPropertyUserCmd : public stPropertyUserCmd {
		stFoundItemPropertyUserCmd()
		{
			byParam = FOUNDITEM_PROPERTY_USERCMD_PARA;
		}

		DWORD up_id;	//待打造物品ID, 物品唯一id
		DWORD count; //材料个数
		struct material{
			DWORD gem_id; //材料id, 类别id
			BYTE gem_level; //材料等级
			BYTE gem_num; //材料数量
		} list[0];
	};

//#define UPGRADEITEM_PROPERTY_USERCMD_PARA 10
//	struct stUpgradeItemPropertyUserCmd : public stPropertyUserCmd {
//		stUpgradeItemPropertyUserCmd() : odds_id(0)
//		{
//			byParam = UPGRADEITEM_PROPERTY_USERCMD_PARA;
//		}
//		DWORD up_id; //待升级物品id, 物品唯一id
//		DWORD gem_id; //升级宝石id, 物品类别id
//		BYTE gem_level; //升级宝石等级	
//		BYTE gem_num; //升级宝石数量
//		DWORD odds_id; //几率宝石id, 物品类别id
//		BYTE odds_level; //几率宝石等级
//		BYTE odds_num; //几率宝石等级
//	};
//
//#define COMPOSEITEM_PROPERTY_USERCMD_PARA 11
//	struct stComposeItemPropertyUserCmd : public stPropertyUserCmd {
//		stComposeItemPropertyUserCmd()
//		{
//			byParam = COMPOSEITEM_PROPERTY_USERCMD_PARA;
//		}
//		DWORD gem_id; //物品id, 类别
//		BYTE gem_level; //物品等级
//		BYTE gem_num; //物品数量
//		BYTE gem_per_num; //单次合成数量
//		DWORD tool_id; //合成道具id,物品唯一id
//	};


#define UPGRADEITEM_PROPERTY_USERCMD_PARA 10
	struct stUpgradeItemPropertyUserCmd : public stPropertyUserCmd {
		stUpgradeItemPropertyUserCmd() 
		{
			byParam = UPGRADEITEM_PROPERTY_USERCMD_PARA;
		}
		BYTE location; //位置，0 左手 1 右手
	};

#define COMPOSEITEM_PROPERTY_USERCMD_PARA 11
	struct stComposeItemPropertyUserCmd : public stPropertyUserCmd {
		stComposeItemPropertyUserCmd()
		{
			byParam = COMPOSEITEM_PROPERTY_USERCMD_PARA;
		}
		BYTE per_num; //单次合成数量
		BYTE location; //位置，0 左手 1 右手
	};

enum enumGameAccelKeyType{
	GameAccelKeyType_None = 0,
	GameAccelKeyType_Item,	// 使用物品（可以吃的）
	GameAccelKeyType_Skill,	// 技能
	GameAccelKeyType_Face,	// 表情
	GameAccelKeyType_Equipment, //装备（可以装在身上的）
	GameAccelKeyType_TaoZhuang, //sky 换装拖动
	GameAccelKeyType_AttPak,    //sky 模式拖动
	GameAccelKeyType_Num,
};

#define MAIN_ACCESS_KEY_NUM		12
#define MAIN_ACCESS_KEY_GROUP	4
struct stGameAccelKey {
	BYTE szAccelKey[4];
	enumGameAccelKeyType type;
	DWORD dwID;
	BYTE mGroup;
	stGameAccelKey()
	{
		mGroup = MAIN_ACCESS_KEY_GROUP;
	}
};
// 更新用户快捷键
const BYTE ACCELKEY_PROPERTY_USERCMD_PARA = 12;
	struct stAccekKeyPropertyUserCmd : public stPropertyUserCmd{
		stAccekKeyPropertyUserCmd()
		{
			byParam = ACCELKEY_PROPERTY_USERCMD_PARA;
		}
		WORD	accelNum;
		BYTE	activeGroup;
		stGameAccelKey accelKeys[0];
	};
/// 添加用户技能数据
#define ADDUSERSKILL_PROPERTY_USERCMD_PARA 13
struct stAddUserSkillPropertyUserCmd : public stPropertyUserCmd
{
	stAddUserSkillPropertyUserCmd()
	{
		byParam = ADDUSERSKILL_PROPERTY_USERCMD_PARA;
	}

	DWORD dwSkillID;			/**< 技能ID */
	WORD wdLevel;				/**< 技能等级 */
	WORD wdUpNum;				/**< 技能等级提升点数 */
	DWORD dwExperience;			/**< 当前经验 */
	DWORD dwMaxExperience;		/**< 升级经验 */
};

/// 技能升级指令
#define UPGRADEUSERSKILL_PROPERTY_USERCMD_PARA 14
struct stUpgradeUserSkillPropertyUserCmd : public stPropertyUserCmd
{
	stUpgradeUserSkillPropertyUserCmd()
	{
		byParam = UPGRADEUSERSKILL_PROPERTY_USERCMD_PARA;
	}

	DWORD dwSkillID;			/**< 技能ID */
	WORD wdLevel;				/**< 技能等级 */
};

/// 删除用户技能
#define REMOVEUSERSKILL_PROPERTY_USERCMD_PARAMETER 15
struct stRemoveUserSkillPropertyUserCmd : public stPropertyUserCmd
{
	stRemoveUserSkillPropertyUserCmd()
	{
		byParam = REMOVEUSERSKILL_PROPERTY_USERCMD_PARAMETER;
	}

	DWORD dwSkillID;				/**< 用户技能编号 */
};

// 增加技能点数
 #define ADDSKILLPOINT_PROPERTY_USERCMD 18
struct stAddSkillPointPropertyUserCmd : public stPropertyUserCmd
{
	stAddSkillPointPropertyUserCmd()
	{
		byParam = ADDSKILLPOINT_PROPERTY_USERCMD;
	}
	DWORD dwSkillID;
};

/// 清除用户属性点数(请求服务器)
#define CLEARPOINT_PROPERTY_USERCMD_PARA 19
struct stClearPointPropertyUserCmd : public stPropertyUserCmd
{
	stClearPointPropertyUserCmd()
	{
		byParam = CLEARPOINT_PROPERTY_USERCMD_PARA;
	}
	DWORD dwItemID;
};

/// 清除用户技能点数(请求服务器)
#define CLEARUSERSKILLPOINT_PROPERTY_USERCMD_PARAMETER 20
struct stClearUserSkillPointPropertyUserCmd : public stPropertyUserCmd
{
	stClearUserSkillPointPropertyUserCmd()
	{
		byParam = CLEARUSERSKILLPOINT_PROPERTY_USERCMD_PARAMETER;
	}
	DWORD dwItemID;
};
// 清除用户所有已经学会的技能(响应服务器)
const BYTE CLEARSKILL_USERCMD_PARA = 33;
struct stClearSkillUserCmd : public stPropertyUserCmd{
	stClearSkillUserCmd()
	{
		byParam = CLEARSKILL_USERCMD_PARA;
	}
};
enum  enumUserSetting          
{       
	USER_SETTING_TEAM = 8,          // 允许组队标志
	USER_SETTING_TRADE,             // 允许交易标志
	USER_SETTING_AUTOFINDPATH,      // 自动寻路 
	USER_SETTING_FAMILY,            // 允许加入家族标志
	USER_SETTING_UNION,             // 允许加入帮会标志

	USER_SETTING_SHOW_HEADPORTRAIT, //显示小头像
	USER_SETTING_SHOW_MIMIMAP,              //显示小地图
	USER_SETTING_AUTO_KILL_SUMMON_NOT_USE_NOW,  //自动打怪
	USER_SETTING_SHOW_PLAYERNAME,   //显示玩家名字
	USER_SETTING_SHOW_HELPTIP,      //显示帮助教程
	USER_SETTING_SHOW_ALLMINIMAP,   //小地图全显示
	USER_SETTING_SHOW_ITEM,                 //地面显示物品

	USER_SETTING_SHOW_COUNTRYNAME,  //显示国家名称
	USER_SETTING_SHOW_UNIONNAME,    //显示帮会名称
	USER_SETTING_SHOW_SEPTNAME,     //显示家族名称
	USER_SETTING_SHOW_OFFICENAME,   //显示官职
	USER_SETTING_SHOW_GRACENAME,    //显示文采

	USER_SETTING_CHAT_NINE,                 //公聊
	USER_SETTING_CHAT_COUNTRY,              //国家
	USER_SETTING_CHAT_AREA,                 //区域
	USER_SETTING_CHAT_WHISPER,              //密聊
	USER_SETTING_CHAT_UNION,                //帮会
	USER_SETTING_CHAT_TEAM,                 //组队
	USER_SETTING_CHAT_FAMILY,               //家族
	USER_SETTING_CHAT_PRIVATE,              //私聊
	USER_SETTING_CHAT_FRIEND,               //好友
	USER_SETTING_CHAT_SCHOOL,               //师门
	USER_SETTING_CHAT_WORLD,                //世界
	//USER_SETTING_CHAT_ALL,                //全部

	USER_SETTING_HELP_MOVE,                     //移动
	USER_SETTING_HELP_NPC,                      //NPC
	USER_SETTING_HELP_NPCDIALOG,                //NPC对话
	USER_SETTING_HELP_ATTACK,                   //攻击
	USER_SETTING_HELP_PK,                       //PK
	USER_SETTING_HELP_CLEARREDNAME,             //消除红名
	USER_SETTING_HELP_TASK,                     //任务
	USER_SETTING_HELP_RIGHTBTN,                 //右键选项
	USER_SETTING_HELP_TRADE,                    //交易
	USER_SETTING_HELP_TEAM,                     //组队
	USER_SETTING_HELP_MEMBERHEAD,               //队员头像
	USER_SETTING_HELP_ACCELKEY,                 //快捷使用物品
	USER_SETTING_HELP_PACK,                     //包裹
	USER_SETTING_HELP_EQUIP,                    //如何装备物品
	USER_SETTING_HELP_EXPACK,                   //额外包裹
	USER_SETTING_HELP_SKILL,                    //技能
	USER_SETTING_HELP_ADDPROPPOINT,             //加点属性
	USER_SETTING_HELP_STUDY,                    //学习
	USER_SETTING_HELP_AUTO_FIND_PATH,                       //自动寻路
	USER_SETTING_HELP_ITEM_LINK,                            //物品链接
	USER_SETTING_HELP_PRIVATE_CHAT,                         //悄悄话
	USER_SETTING_HELP_MATERIAL_COMPOSE,                     //原料合成
	USER_SETTING_HELP_SOULSTONEL_COMPOSE,           //魂魄合成
	USER_SETTING_HELP_SOULSTONE_MAKE,                       //魂魄镶嵌
	USER_SETTING_HELP_ITEM_UPGRID,                          //道具升级
	USER_SETTING_HELP_ATTACK_MODE_CHANGE,           //攻击模式切换
	USER_SETTING_HELP_CHAT,                                         //聊天
	USER_SETTING_HELP_SELECT,                                       //选择目标
	USER_SETTING_HELP_ALT_SELECT_NPC,                        //alt选中npc
	USER_SETTING_FRIEND,             // 允许加为好友
	USER_SETTING_SCHOOL,             // 允许加入师门标志
	USER_SETTING_MINIGAME,           // 允许小游戏
	USER_SETTING_EFFECT,			 // 开关特效

	USER_SETTING_SHOW_SHUXTIP,		//升级属性点未分配完毕标志
	USER_SETTING_SHOW_JINENGTIP		//升级技能点未分配完毕标志
};
struct stSystemSettings
{
	BYTE	bySettings[20];	// 使用 set_state 设置标志
	DWORD	dwChatColor[8]; //颜色设置
};

#define SYSTEMSETTINGS_PROPERTY_USERCMD_PARA 16
struct stSystemSettingsUserCmd : public stPropertyUserCmd
{
	stSystemSettingsUserCmd()
	{
		byParam = SYSTEMSETTINGS_PROPERTY_USERCMD_PARA;
	}
	stSystemSettings data;
};

enum{
	PROPERTY_CON = 0,	//体质
	PROPERTY_STR = 1,	//力量
	PROPERTY_DEX = 2,	//敏捷
	PROPERTY_INT = 3,	//智力
	PROPERTY_MEN = 4	//精神
};
/*
/// 分配用户五行点数
#define DISTRIBUTEUFIVEPOINT_PROPERTY_USERCMD_PARA 17
struct stDistributeFivePointPropertyUserCmd : public stPropertyUserCmd
{
stDistributeFivePointPropertyUserCmd()
{
byParam = DISTRIBUTEUFIVEPOINT_PROPERTY_USERCMD_PARA;
}
WORD fiveType;

};
*/
// 分配用户属性点数
#define DISTRIBUTEUPOINT_PROPERTY_USERCMD_PARA 17
struct stDistributePointPropertyUserCmd : public stPropertyUserCmd
{
	stDistributePointPropertyUserCmd()
	{
		byParam = DISTRIBUTEUPOINT_PROPERTY_USERCMD_PARA;
	}
	WORD type;

};

/// 分配用户技能点数
#define DISTRIBUTEUSKILLPOINT_PROPERTY_USERCMD_PARA 19
struct stDistributeSkillPointPropertyUserCmd : public stPropertyUserCmd
{
	stDistributeSkillPointPropertyUserCmd()
	{
		byParam = DISTRIBUTEUSKILLPOINT_PROPERTY_USERCMD_PARA;
	}
	WORD type;

};


/*
#define MAKE_ODDS_PROPERTY_USERCMD_PARA 19
struct stMakeOddsPropertyUserCmd : public stPropertyUserCmd
{
	stMakeOddsPropertyUserCmd()
	{
		byParam = MAKE_ODDS_PROPERTY_USERCMD_PARA;
	}
	
	DWORD odds;
};

#define MAKE_SELECT_PROPERTY_USERCMD_PARA 20
struct stMakeSelectPropertyUserCmd : public stPropertyUserCmd
{
	stMakeSelectPropertyUserCmd()
	{
		byParam = MAKE_SELECT_PROPERTY_USERCMD_PARA;
	}
	
	DWORD id; //物品id
};
*/
//#define COMPOSE_SOUL_STONE_PROPERTY_USERCMD_PARA 21
//struct stComposeSoulStonePropertyUserCmd : public stPropertyUserCmd {
//	stComposeSoulStonePropertyUserCmd()
//	{
//		byParam = COMPOSE_SOUL_STONE_PROPERTY_USERCMD_PARA;
//	}
//	DWORD id_1; //第一块魂魄id, 物品唯一id
//	DWORD id_2; //第二块魂魄id, 物品唯一id
//	DWORD gem_id; //凝魂石id，物品类别id
//	BYTE gem_level; //凝魂石等级
//	BYTE gem_num; //凝魂石数量
//};
//
//#define ENCHASEL_PROPERTY_USERCMD_PARA 22
//struct stEnchasePropertyUserCmd : public stPropertyUserCmd {
//	stEnchasePropertyUserCmd()
//	{
//		byParam = ENCHASEL_PROPERTY_USERCMD_PARA;
//	}
//	DWORD up_id; //待镶嵌物品id, 物品唯一id
//	DWORD soulstone_id; //魂魄石id，物品唯一id
//	
//	DWORD gem_id; //定魂石id，物品类别id
//	BYTE gem_level; //定魂石等级
//	BYTE gem_num; //定魂石数量
//};

#define COMPOSE_SOUL_STONE_PROPERTY_USERCMD_PARA 21
struct stComposeSoulStonePropertyUserCmd : public stPropertyUserCmd {
	stComposeSoulStonePropertyUserCmd()
	{
		byParam = COMPOSE_SOUL_STONE_PROPERTY_USERCMD_PARA;
	}
	BYTE location; //位置，0 左手 1 右手

};

#define ENCHASEL_PROPERTY_USERCMD_PARA 22
struct stEnchasePropertyUserCmd : public stPropertyUserCmd {
	stEnchasePropertyUserCmd()
	{
		byParam = ENCHASEL_PROPERTY_USERCMD_PARA;
	}
	BYTE location; //位置，0 左手 1 右手

};


#define HOLE_PROPERTY_USERCMD_PARA 23
struct stHolePropertyUserCmd : public stPropertyUserCmd {
	stHolePropertyUserCmd()
	{
		byParam = HOLE_PROPERTY_USERCMD_PARA;
	}
	DWORD up_id; //待打孔装备id, 物品唯一id
	DWORD gem_id; //天魔石id，物品类别id
	BYTE gem_level; //天魔石等级
	BYTE gem_num; //天魔石数量
};

#define DECOMPOSE_PROPERTY_USERCMD_PARA 24
struct stDecomposePropertyUserCmd : public stPropertyUserCmd {
	stDecomposePropertyUserCmd()
	{
		byParam = DECOMPOSE_PROPERTY_USERCMD_PARA;
	}
	DWORD up_id; //待分解装备id, 物品唯一id
};

#define BODYCOLOR_PROPERTY_USERCMD_PARA 25//衣服换色
struct stBodyColorPropertyUserCmd : public stPropertyUserCmd
{
	stBodyColorPropertyUserCmd()
	{
		byParam = BODYCOLOR_PROPERTY_USERCMD_PARA;
	}
	DWORD dwBodyColorCustom;//换色的颜色
};

#define HAIR_PROPERTY_USERCMD_PARA 26//换发型
struct stHairPropertyUserCmd : public stPropertyUserCmd
{
	stHairPropertyUserCmd()
	{
		byParam = HAIR_PROPERTY_USERCMD_PARA;
	}
	BYTE byHairType;//头发发型
	DWORD dwHairColor;//头发颜色值
};

enum enumChatFilter
{
	chatFilter_Nine = 0x01,//公聊
	chatFilter_Country = 0x02,//国家
	chatFilter_School = 0x04,//门派
	chatFilter_Trade = 0x08,//交易
	chatFilter_Union = 0x10,//帮会
	chatFilter_Team = 0x20,//组队
	chatFilter_Family = 0x40,//家族
	chatFilter_Private = 0x80,//私聊
};

//Select active NPC data
#define SELECT_PROPERTY_USERCMD_PARA 27//选中某个对象
struct stSelectPropertyUserCmd : public stPropertyUserCmd
{
	stSelectPropertyUserCmd()
	{
		byParam = SELECT_PROPERTY_USERCMD_PARA;
	}
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
	BYTE byOldType;//enumMapDataType
	DWORD dwOldTempID;//临时编号
};

#define SELECT_MAINUSER_PROPERTY_USERCMD_PARA 29//请求观察选中人的属性
struct stSelectMainUserPropertyUserCmd : public stPropertyUserCmd
{
	stSelectMainUserPropertyUserCmd()
	{
		byParam = SELECT_MAINUSER_PROPERTY_USERCMD_PARA;
	}   
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
};

enum enumEquipType{
	Equip_head,
	Equip_necklace,
	Equip_body,
	Equip_handL,
	Equip_handR,
	Equip_belt,
	Equip_ringL,
	Equip_ringR,
	Equip_bangleL,
	Equip_bangleR,
	Equip_boots,
	Equip_others1,
	Equip_others2,
	Equip_others3,
	Equip_others4,
	Equip_others5

};
struct EquipedObject
{
	BYTE byWhere;//位置enumEquipType
	t_Object object;//物品

};
#define SELECT_RETURN_MAINUSER_PROPERTY_USERCMD_PARA 30//回答观察选中人的属性
struct stSelectReturnMainUserPropertyUserCmd : public stPropertyUserCmd
{
	stSelectReturnMainUserPropertyUserCmd()
	{
		byParam = SELECT_RETURN_MAINUSER_PROPERTY_USERCMD_PARA;
	}
	DWORD dwTempID;//临时编号
	t_MainUserData mainuser_data;
	t_MapUserData mapuser_data;
	DWORD dwSize; //已装备物品个数
	EquipedObject object_data[0];
};

//耐久度变化
const BYTE DURABIILTY_USERCMD_PARA = 31;
struct stDurabilityUserCmd : public stPropertyUserCmd{
	stDurabilityUserCmd()
	{       
		byParam = DURABIILTY_USERCMD_PARA;
	}       
	DWORD dwThisID; /*装备ID */
	DWORD dwDur;    /*当前耐久度*/
	DWORD dwMaxDur; /*最大耐久度*/
};

#define SELECT_RETURN_STATES_PROPERTY_USERCMD_PARA 32//被选中人的状态
struct stSelectReturnStatesPropertyUserCmd : public stPropertyUserCmd
{
	stSelectReturnStatesPropertyUserCmd()
	{
		byParam = SELECT_RETURN_STATES_PROPERTY_USERCMD_PARA;
	}
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
	DWORD size;//状态个数
	struct  
	{
		DWORD state;
		union{  
			struct{ 
				WORD result;//数值
				WORD time;//时间
			};      
			DWORD value;
		};      
	}states[0];
};

#define FACE_PROPERTY_USERCMD_PARA 34//更换头像
struct stFacePropertyUserCmd : public stPropertyUserCmd
{
	stFacePropertyUserCmd()
	{
		byParam = FACE_PROPERTY_USERCMD_PARA;
	}
	DWORD dwFace;//目标头像类型
};


#define HONOR_TO_PKVALUE_PROPERTY_USERCMD_PARA 35//荣誉点洗pk值
struct stHonorToPkValuePropertyUserCmd : public stPropertyUserCmd
{
	stHonorToPkValuePropertyUserCmd()
	{
		byParam = HONOR_TO_PKVALUE_PROPERTY_USERCMD_PARA;
	}
};

// 使用血如意和玉如意时需要发送地图信息下去
const BYTE SCROLL_MAP_PROPERTY_USERCMD_PARA = 36;
struct stScrollMapPropertyUserCmd : public stPropertyUserCmd{
	stScrollMapPropertyUserCmd()
	{       
		byParam = SCROLL_MAP_PROPERTY_USERCMD_PARA;
	}       
	DWORD qwThisID;             /**< 物品唯一ID */
	DWORD   size;   
	struct  
	{       
		char strMapName[MAX_NAMESIZE];
	}mapname[0];

};
/// 清除用户属性点数只限5点
#define CLEARPOINT_LIMIT_PROPERTY_USERCMD_PARA 37
struct stClearPointLimitPropertyUserCmd : public stPropertyUserCmd
{
	stClearPointLimitPropertyUserCmd()
	{
		byParam = CLEARPOINT_LIMIT_PROPERTY_USERCMD_PARA;
	}
	QWORD qwThisID;   //物品的对象唯一ID
	BYTE  byProperty; //参见command.h里的5个属性的枚举定义
};
// 请求增值地图列表
const BYTE REQUEST_INC_MAP_PROPERTY_USERCMD_PARA = 38;
struct stRequestIncMapPropertyUserCmd : public stPropertyUserCmd{
	stRequestIncMapPropertyUserCmd()
	{       
		byParam = REQUEST_INC_MAP_PROPERTY_USERCMD_PARA;
	}       
};

enum horseState
{   
	HORSE_STATE_PUTUP,  //收起
	HORSE_STATE_FOLLOW, //跟随
	HORSE_STATE_RIDE    //骑乘
};  

enum horseSkill
{   
	HORSE_SKILL_DEF_UP = 1, //提高物防魔防
	HORSE_SKILL_PATK_UP,    //提高物攻
	HORSE_SKILL_MATK_UP,    //提高魔攻
	HORSE_SKILL_HP_UP,  //提高生命上限
	HORSE_SKILL_MP_UP,  //提高魔法上限
	HORSE_SKILL_DMG_UP, //附加伤害
	HORSE_SKILL_DMG_DOWN,   //减少自身伤害
}; 


//马的数据
struct t_HorseData
{   
	DWORD id;       //npcid             
	char name[MAX_NAMESIZE];//名字      
	DWORD lv;       //等级              
	WORD str;       //力量
	WORD intel;     //智力
	WORD dex;       //敏捷
	WORD men;       //精神
	WORD con;       //体质
	WORD speed;     //速度
	BYTE pdam;      //增加物理攻击力%
	BYTE pdef;      //增加物理防御力%
	BYTE mdam;      //增加魔法攻击力%
	BYTE mdef;      //增加魔法防御力%   
	WORD maxhp;     //最大HP
	WORD maxmp;     //最大MP
	DWORD callTime;     //召唤间隔      
	horseState state;   //马的状态
	WORD poisonRes;     //抗毒          
	WORD lullRes;       //抗麻痹        
	WORD faintRes;      //抗晕眩
	WORD chaosRes;      //抗混乱
	WORD freezeRes;     //抗冰冻        
	WORD petrifyRes;    //抗石化
	WORD blindRes;      //抗失明
	WORD slowRes;       //抗减速
	DWORD horseid;      //马的id（道具id）

	//马匹训练相关变量
	WORD horseXLlevel;  //马匹训练等级
	DWORD horseXLtime;  //马匹训练剩余时间
};

#define SELECT_RETURN_HORSE_PROPERTY_USERCMD_PARA 39//回答观察选中马匹的属性
struct stSelectReturnHorsePropertyUserCmd : public stPropertyUserCmd
{
	stSelectReturnHorsePropertyUserCmd()
	{
		byParam = SELECT_RETURN_HORSE_PROPERTY_USERCMD_PARA;
	}
	t_HorseData data;
};

#define GO_TRAIN_PROPERTY_USERCMD_PARA 40//请求进入练级地图
struct stGoTrainPropertyUserCmd : public stPropertyUserCmd
{
	stGoTrainPropertyUserCmd()
	{
		byParam = GO_TRAIN_PROPERTY_USERCMD_PARA;
	}
	DWORD level;//去第几层
};


/// 批量添加用户道具数据
#define ADDUSEROBJECT_LIST_PROPERTY_USERCMD_PARAMETER 41
struct stAddObjectListPropertyUserCmd : public stPropertyUserCmd{
	stAddObjectListPropertyUserCmd()
	{
		byParam = ADDUSEROBJECT_LIST_PROPERTY_USERCMD_PARAMETER;
		num=0;
	}
	WORD num;
	struct
	{
		BYTE byActionType;          /**< 物品动作类型 */
		t_Object object;            /**< 物品数据 */
	}list[0];
};
//ZH
/// 低级装备换高级装备
#define EXCHANGEEQUIPMENT_PROPERTY_USERCMD_PARAMETER 42
struct stExchangeEquipmentPropertyUserCmd : public stPropertyUserCmd {
	stExchangeEquipmentPropertyUserCmd()
	{
		byParam = EXCHANGEEQUIPMENT_PROPERTY_USERCMD_PARAMETER;
	}
	DWORD qwThisID[45];				/**< 物品唯一ID */
};
//道具升级
#define EQUIPMENTLEVELUP_PROPERTY_USERCMD_PARAMETER 44
struct stEquipmentLevelUpPropertyUserCmd : public stPropertyUserCmd {
	stEquipmentLevelUpPropertyUserCmd()
	{
		byParam = EQUIPMENTLEVELUP_PROPERTY_USERCMD_PARAMETER;
	}
	DWORD qwThisID;				/**< 物品唯一ID */
	BYTE  byType;
};



//////////////////////////////////////////////////////////////////////////
enum RebuildType{
	COMPOSE = 1,
	MAKE = 2,
	UPGRADE = 3,
	HOLE = 4,
	ENCHANCE = 5,
	DECOMPOSE = 6,
	MODIFY = 7,  //soke 改造属性
	MAKEZS = 8,  //soke 装备祝福
	MAKEZQ = 9,  //soke 马装祝福
	MAKELV = 10, //soke 装备升级

	EQUIP_COMPOSE11 = 11,   //装备祝福
	EQUIP_COMPOSE12 = 12,   //斗魂升级
	EQUIP_COMPOSE13 = 13,   //神石镶嵌
	EQUIP_COMPOSE14 = 14,   //装备栏激活
	EQUIP_COMPOSE15 = 15,   //装备栏进阶
	EQUIP_COMPOSE16 = 16,   //神器强化
	EQUIP_COMPOSE17 = 17,   //龙槽激活
	EQUIP_COMPOSE18 = 18,   //龙槽解封
	EQUIP_COMPOSE19 = 19,   //龙星升级
	EQUIP_COMPOSE20 = 20,   //魂魄清除
	EQUIP_COMPOSE21 = 21,   //装备升级
	EQUIP_COMPOSE22 = 22,   //马牌精炼
	EQUIP_COMPOSE23 = 23,   //龙王套装镶嵌
	EQUIP_COMPOSE24 = 24,   //逆天养成进阶
	EQUIP_COMPOSE25 = 25,   //斗战圣佛进阶
	EQUIP_COMPOSE26 = 26,   //暗影石镶嵌
	EQUIP_COMPOSE27 = 27,   //龙凤吟
	EQUIP_COMPOSE28 = 28,   //轰天宝石镶嵌
	EQUIP_COMPOSE29 = 29,   //王者升级

	EQUIP_COMPOSE51 = 51,   //勋章升级
	EQUIP_COMPOSE52 = 52,   //寒冰升级
	EQUIP_COMPOSE53 = 53,   //翅膀升级
	EQUIP_COMPOSE54 = 54,   //神兵升级
	EQUIP_COMPOSE55 = 55,   //神兽升级
	EQUIP_COMPOSE56 = 56,   //璀璨镶嵌
	EQUIP_COMPOSE57 = 57,   //神兽宝石镶嵌
	EQUIP_COMPOSE58 = 58,   //尚方宝剑升级
	EQUIP_COMPOSE59 = 59,   //紫金龙袍升级
	EQUIP_COMPOSE60 = 60,   //十二星宿进阶	
	EQUIP_COMPOSE61 = 61,   //神斧	
	EQUIP_COMPOSE62 = 62,   //黑曜	
	EQUIP_COMPOSE63 = 63,   //精金	
	EQUIP_COMPOSE64 = 64,   //金箍咒强化
	EQUIP_COMPOSE65 = 65,   //金箍咒字纹
	EQUIP_COMPOSE66 = 66,   //金箍咒淬炼
	EQUIP_COMPOSE67 = 67,   //寒冰神炼
	EQUIP_COMPOSE68 = 68,   //寒冰字纹
	EQUIP_COMPOSE69 = 69,   //寒冰纹章
    EQUIP_COMPOSE101 = 101,   //图鉴进阶	
	EQUIP_COMPOSE102 = 102,   //图鉴签名

	EQUIP_COMPOSE103 = 103,   //法宝进阶
	EQUIP_COMPOSE104 = 104,   //符石进阶	

	EQUIP_COMPOSE105 = 105,   //十二星宿进阶	

	EQUIP_COMPOSE106 = 106,   //元神进阶
	EQUIP_COMPOSE107 = 107,   //元神装备进阶	
	EQUIP_COMPOSE108 = 108,   //元神宝石镶嵌	
};

#define RESPONSE_PROPERTY_USERCMD_PARA 0x81
struct stResponsePropertyUserCmd : public stPropertyUserCmd
{
	stResponsePropertyUserCmd()
	{
		byParam = RESPONSE_PROPERTY_USERCMD_PARA;
		bzero(info, sizeof(info));
	}

	WORD kind; //类别，1:合成 2:打造 3:升级
	BYTE status; //结果 0 成功 非0 失败
	char info[MAX_CHATINFO]; //操作信息
};

///soke 属性自由加点
#define CHANGEUSER_PROPERTY_USERCMD_PARAMETER 0x83
struct stChangeUserPropertyUserCmd : public stPropertyUserCmd 
{
	stChangeUserPropertyUserCmd()
	{
		byParam = CHANGEUSER_PROPERTY_USERCMD_PARAMETER;
	}
	DWORD dwCount;				/**< 每次加多少点 */
	DWORD dwType;				/**< 属性类型 */
};


///装备改造、材料合成枚举
enum RecastAction
{
    RECAST_NONE          = 0,
    RECAST_ITEM_COMPOSE11 = 11,     // 装备祝福
	RECAST_ITEM_COMPOSE12 = 12,     // 斗魂升级
	RECAST_ITEM_COMPOSE13 = 13,     // 神石镶嵌
	RECAST_ITEM_COMPOSE14 = 14,     // 装备栏激活
	RECAST_ITEM_COMPOSE15 = 15,     // 装备栏进阶
	RECAST_ITEM_COMPOSE16 = 16,     // 神器强化
	RECAST_ITEM_COMPOSE17 = 17,   // 龙槽激活
	RECAST_ITEM_COMPOSE18 = 18,   // 龙槽解封
	RECAST_ITEM_COMPOSE19 = 19,   // 龙星升级
	RECAST_ITEM_COMPOSE20 = 20,   // 魂魄清除
	RECAST_ITEM_COMPOSE21 = 21,   // 装备升级
	RECAST_ITEM_COMPOSE22 = 22,   // 马牌精炼
	RECAST_ITEM_COMPOSE23 = 23,   // 龙王套装镶嵌
	RECAST_ITEM_COMPOSE24 = 24,   // 逆天养成进阶
	RECAST_ITEM_COMPOSE25 = 25,   // 斗战圣佛进阶
	RECAST_ITEM_COMPOSE26 = 26,   // 暗影石镶嵌
	RECAST_ITEM_COMPOSE27 = 27,     // 龙凤吟
	RECAST_ITEM_COMPOSE28 = 28,     // 轰天宝石镶嵌
	RECAST_ITEM_COMPOSE29 = 29,    // 王者升级

	RECAST_ITEM_COMPOSE51 = 51,     // 勋章升级
	RECAST_ITEM_COMPOSE52 = 52,     // 寒冰升级
	RECAST_ITEM_COMPOSE53 = 53,     // 翅膀升级
	RECAST_ITEM_COMPOSE54 = 54,     // 神兵升级
	RECAST_ITEM_COMPOSE55 = 55,     // 神兽升级
	RECAST_ITEM_COMPOSE56 = 56,     // 璀璨镶嵌
	RECAST_ITEM_COMPOSE57 = 57,     // 神兽宝石镶嵌
	RECAST_ITEM_COMPOSE58 = 58,     // 尚方宝剑升级
	RECAST_ITEM_COMPOSE59 = 59,     // 紫金龙袍升级	
	RECAST_ITEM_COMPOSE60 = 60,   // 十二星宿进阶
	RECAST_ITEM_COMPOSE61 = 61,     // 神斧强化
	RECAST_ITEM_COMPOSE62 = 62,     // 黑曜强化
	RECAST_ITEM_COMPOSE63 = 63,     // 精金强化
	RECAST_ITEM_COMPOSE64 = 64,   //金箍咒强化
	RECAST_ITEM_COMPOSE65 = 65,   //金箍咒字纹
	RECAST_ITEM_COMPOSE66 = 66,   //金箍咒淬炼
	RECAST_ITEM_COMPOSE67 = 67,   //寒冰神炼
	RECAST_ITEM_COMPOSE68 = 68,   //寒冰字纹
	RECAST_ITEM_COMPOSE69 = 69,   //寒冰纹章
	
	RECAST_ITEM_COMPOSE101 = 101,   // 图鉴进阶	
	RECAST_ITEM_COMPOSE102 = 102,   // 图鉴签名

	RECAST_ITEM_COMPOSE103 = 103,   // 法宝进阶	
	RECAST_ITEM_COMPOSE104 = 104,   // 符石进阶

	RECAST_ITEM_COMPOSE105 = 105,   // 十二星宿进阶

	RECAST_ITEM_COMPOSE106 = 106,   // 元神进阶	
	RECAST_ITEM_COMPOSE107 = 107,   // 元神附件进阶	
	RECAST_ITEM_COMPOSE108 = 108,   // 元神宝石镶嵌	

};

///装备改造之绑定、签名等
static const BYTE ITEM_RECAST_PROPERTY_USERCMD_PARA = 43;
struct ItemRecastPropertyUserCmd : public stPropertyUserCmd
{
    ItemRecastPropertyUserCmd()
    {
        byParam = ITEM_RECAST_PROPERTY_USERCMD_PARA;
        act = RECAST_NONE;
        itemUID = 0;             

    }
    RecastAction    act;      
    DWORD           itemUID;            //装备全局唯一的ID，不是类型ID

};

//soke 改造指令
#define MODIFY_PROPERTY_USERCMD_PARA 45
struct stModifyPropertyUserCmd : public stPropertyUserCmd 
{
	stModifyPropertyUserCmd()
	{
		byParam = MODIFY_PROPERTY_USERCMD_PARA;
	}
	BYTE  type;     //改造的类型
	DWORD up_id;    //待改造装备id, 物品唯一id
	DWORD gem_id;   //改造材料id，物品类别id
	BYTE gem_level; //改造材料等级
	BYTE gem_num;   //改造材料数量
};

//soke 祝福改造指令
#define MAKEZS_PROPERTY_USERCMD_PARA 46
struct stMakezsPropertyUserCmd : public stPropertyUserCmd 
{
	stMakezsPropertyUserCmd()
	{
		byParam = MAKEZS_PROPERTY_USERCMD_PARA;
	}
	BYTE  type;     //祝福的类型
	DWORD up_id;    //待祝福装备id, 物品唯一id
	DWORD gem_id;   //祝福材料id，物品类别id
	BYTE gem_level; //祝福材料等级
	BYTE gem_num;   //祝福材料数量
};

//soke 马匹装备祝福指令
#define MAKEZQ_PROPERTY_USERCMD_PARA 47
	struct stMakezqPropertyUserCmd : public stPropertyUserCmd 
	{
		stMakezqPropertyUserCmd()
		{
			byParam = MAKEZQ_PROPERTY_USERCMD_PARA;
		}
		BYTE  type;     //祝福的类型
		DWORD up_id;    //待祝福装备id, 物品唯一id
		DWORD gem_id;   //祝福改造材料id，物品类别id
		BYTE gem_level; //祝福改造材料等级
		BYTE gem_num;   //祝福改造材料数量
	};

/* //soke 装备升级指令
#define MAKELV_PROPERTY_USERCMD_PARA 48
	struct stMakelvPropertyUserCmd : public stPropertyUserCmd 
	{
		stMakelvPropertyUserCmd()
		{
			byParam = MAKELV_PROPERTY_USERCMD_PARA;
		}
		BYTE  type;     //升级的类型
		DWORD up_id;    //待升级装备id, 物品唯一id
		DWORD gem_id;   //升级改造材料id，物品类别id
		BYTE gem_level; //升级改造材料等级
		BYTE gem_num;   //升级改造材料数量
	}; */
	
///soke 包裹一键整理
#define SWAP_ALL_USER_OBJECT_PROPERTY_USERCMD_PARAMETER 49
	struct stSwapAllObjectPropertyUserCmd : public stPropertyUserCmd
	{
		stSwapAllObjectPropertyUserCmd()
		{
			byParam = SWAP_ALL_USER_OBJECT_PROPERTY_USERCMD_PARAMETER;
		}
		DWORD nNum;						// 最多99次道具移动
		DWORD qwThisID[ 99 ];			/**< 物品唯一ID */
		stObjectLocation dst[ 99 ];		/**< 物品目的位置 如果目的位置上有物品，则交换*/
	};

#endif


//soke 经验物品使用
#define ITEN_USEITEM_SUCCESS_USERCMD 50
	struct stItemUseItemSuccessUserCmd : public stPropertyUserCmd
	{
		stItemUseItemSuccessUserCmd()
		{
			byParam = ITEN_USEITEM_SUCCESS_USERCMD;
			ItemID = 0;
		}
		DWORD ItemID;
	};
	
///请求进入副本地图
#define GO_FUBEN_PROPERTY_USERCMD_PARA 51
struct stGoFubenPropertyUserCmd : public stPropertyUserCmd
{
	stGoFubenPropertyUserCmd()
	{
		byParam = GO_FUBEN_PROPERTY_USERCMD_PARA;
	}
	DWORD level;//去第几层
};

//soke 73类物品使用
#define ITEN6_USEITEM_SUCCESS_USERCMD 55
	struct stItem6UseItemSuccessUserCmd : public stPropertyUserCmd
	{
		stItem6UseItemSuccessUserCmd()
		{
			byParam = ITEN6_USEITEM_SUCCESS_USERCMD;
			ItemID = 0;
		}
		DWORD ItemID;
	};
	
//soke 74类幸运丹物品使用
#define ITEN7_USEITEM_SUCCESS_USERCMD 56
	struct stItem7UseItemSuccessUserCmd : public stPropertyUserCmd
	{
		stItem7UseItemSuccessUserCmd()
		{
			byParam = ITEN7_USEITEM_SUCCESS_USERCMD;
			ItemID = 0;
		}
		DWORD ItemID;
	};
	
//soke 78合成魂魄石物品使用
#define ITEN11_USEITEM_SUCCESS_USERCMD 60
	struct stItem11UseItemSuccessUserCmd : public stPropertyUserCmd
	{
		stItem11UseItemSuccessUserCmd()
		{
			byParam = ITEN11_USEITEM_SUCCESS_USERCMD;
			ItemID = 0;
		}
		DWORD ItemID;
	};

//soke 匿名送花9朵
#define SELECT_FLOWERS_PROPERTY_USERCMD_PARA 61
struct stSelectFlowersPropertyUserCmd : public stPropertyUserCmd
{
	stSelectFlowersPropertyUserCmd()
	{
		byParam = SELECT_FLOWERS_PROPERTY_USERCMD_PARA;
	}
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
};

//soke 匿名送花99朵
#define SELECT_FLOWERS1_PROPERTY_USERCMD_PARA 62
struct stSelectFlowers1PropertyUserCmd : public stPropertyUserCmd
{
	stSelectFlowers1PropertyUserCmd()
	{
		byParam = SELECT_FLOWERS1_PROPERTY_USERCMD_PARA;
	}
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
};

//soke 匿名送花365朵
#define SELECT_FLOWERS2_PROPERTY_USERCMD_PARA 63
struct stSelectFlowers2PropertyUserCmd : public stPropertyUserCmd
{
	stSelectFlowers2PropertyUserCmd()
	{
		byParam = SELECT_FLOWERS2_PROPERTY_USERCMD_PARA;
	}
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
};

//soke 匿名送花999朵
#define SELECT_FLOWERS3_PROPERTY_USERCMD_PARA 64
struct stSelectFlowers3PropertyUserCmd : public stPropertyUserCmd
{
	stSelectFlowers3PropertyUserCmd()
	{
		byParam = SELECT_FLOWERS3_PROPERTY_USERCMD_PARA;
	}
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
};

//soke 签名送花9朵
#define SELECT_FLOWERS4_PROPERTY_USERCMD_PARA 65
struct stSelectFlowers4PropertyUserCmd : public stPropertyUserCmd
{
	stSelectFlowers4PropertyUserCmd()
	{
		byParam = SELECT_FLOWERS4_PROPERTY_USERCMD_PARA;
	}
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
};

//soke 签名送花99朵
#define SELECT_FLOWERS5_PROPERTY_USERCMD_PARA 66
struct stSelectFlowers5PropertyUserCmd : public stPropertyUserCmd
{
	stSelectFlowers5PropertyUserCmd()
	{
		byParam = SELECT_FLOWERS5_PROPERTY_USERCMD_PARA;
	}
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
};

//soke 签名送花365朵
#define SELECT_FLOWERS6_PROPERTY_USERCMD_PARA 67
struct stSelectFlowers6PropertyUserCmd : public stPropertyUserCmd
{
	stSelectFlowers6PropertyUserCmd()
	{
		byParam = SELECT_FLOWERS6_PROPERTY_USERCMD_PARA;
	}
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
};

//soke 签名送花999朵
#define SELECT_FLOWERS7_PROPERTY_USERCMD_PARA 68
struct stSelectFlowers7PropertyUserCmd : public stPropertyUserCmd
{
	stSelectFlowers7PropertyUserCmd()
	{
		byParam = SELECT_FLOWERS7_PROPERTY_USERCMD_PARA;
	}
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
};

//soke 副本激活
#define CLEARUSERSKILLPOINT_FUBENJH_USERCMD_PARAMETER 69
struct stClearUserSkillPointFubenjhUserCmd : public stPropertyUserCmd
{
	stClearUserSkillPointFubenjhUserCmd()
	{
		byParam = CLEARUSERSKILLPOINT_FUBENJH_USERCMD_PARAMETER;
	}
	DWORD dwItemID;
};

//soke 匿名送蓝色妖姬9朵
#define SELECT_FLOWERS20_PROPERTY_USERCMD_PARA 70
struct stSelectFlowers20PropertyUserCmd : public stPropertyUserCmd
{
	stSelectFlowers20PropertyUserCmd()
	{
		byParam = SELECT_FLOWERS20_PROPERTY_USERCMD_PARA;
	}
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
};

//soke 匿名送蓝色妖姬99朵
#define SELECT_FLOWERS21_PROPERTY_USERCMD_PARA 71
struct stSelectFlowers21PropertyUserCmd : public stPropertyUserCmd
{
	stSelectFlowers21PropertyUserCmd()
	{
		byParam = SELECT_FLOWERS21_PROPERTY_USERCMD_PARA;
	}
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
};

//soke 匿名送蓝色妖姬365朵
#define SELECT_FLOWERS22_PROPERTY_USERCMD_PARA 72
struct stSelectFlowers22PropertyUserCmd : public stPropertyUserCmd
{
	stSelectFlowers22PropertyUserCmd()
	{
		byParam = SELECT_FLOWERS22_PROPERTY_USERCMD_PARA;
	}
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
};

//soke 匿名送蓝色妖姬999朵
#define SELECT_FLOWERS23_PROPERTY_USERCMD_PARA 73
struct stSelectFlowers23PropertyUserCmd : public stPropertyUserCmd
{
	stSelectFlowers23PropertyUserCmd()
	{
		byParam = SELECT_FLOWERS23_PROPERTY_USERCMD_PARA;
	}
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
};

//soke 签名送蓝色妖姬9朵
#define SELECT_FLOWERS41_PROPERTY_USERCMD_PARA 74
struct stSelectFlowers41PropertyUserCmd : public stPropertyUserCmd
{
	stSelectFlowers41PropertyUserCmd()
	{
		byParam = SELECT_FLOWERS41_PROPERTY_USERCMD_PARA;
	}
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
};

//soke 签名送蓝色妖姬99朵
#define SELECT_FLOWERS51_PROPERTY_USERCMD_PARA 75
struct stSelectFlowers51PropertyUserCmd : public stPropertyUserCmd
{
	stSelectFlowers51PropertyUserCmd()
	{
		byParam = SELECT_FLOWERS51_PROPERTY_USERCMD_PARA;
	}
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
};

//soke 签名送蓝色妖姬365朵
#define SELECT_FLOWERS61_PROPERTY_USERCMD_PARA 76
struct stSelectFlowers61PropertyUserCmd : public stPropertyUserCmd
{
	stSelectFlowers61PropertyUserCmd()
	{
		byParam = SELECT_FLOWERS61_PROPERTY_USERCMD_PARA;
	}
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
};

//soke 签名送蓝色妖姬999朵
#define SELECT_FLOWERS71_PROPERTY_USERCMD_PARA 77
struct stSelectFlowers71PropertyUserCmd : public stPropertyUserCmd
{
	stSelectFlowers71PropertyUserCmd()
	{
		byParam = SELECT_FLOWERS71_PROPERTY_USERCMD_PARA;
	}
	BYTE byType;//enumMapDataType
	DWORD dwTempID;//临时编号
};

//sky 使用竹蜻蜓时需要发送地图信息下去
const BYTE SCROLL2_MAP_PROPERTY_USERCMD_PARA = 78;
struct stScroll2MapPropertyUserCmd : public stPropertyUserCmd
{
	stScroll2MapPropertyUserCmd()
	{
		byParam = SCROLL2_MAP_PROPERTY_USERCMD_PARA;
	}
	DWORD qwThisID;             /**< 物品唯一ID */
	DWORD	xPos;
	DWORD	yPos;
	DWORD   size;
	struct
	{
		char strMapName[MAX_NAMESIZE];
	}
	mapname[0];
};

struct One_Key_Equip
{
	int index;
	char sName[256];
	DWORD IdTable[27];
	DWORD	dwSkillIconID;
};


//sky 一键换装请求
#define ONE_KEY_EQUIP_PROPERTY_USERCMD_PARA 82
	struct stOneKeyEquipPropertyUserCmd : public stPropertyUserCmd
	{
		stOneKeyEquipPropertyUserCmd()
		{
		byParam = ONE_KEY_EQUIP_PROPERTY_USERCMD_PARA;
	}
	One_Key_Equip one_key_equip;

	DWORD dwOldTempID;
};


//sky 获取一键换装套装
#define GET_ONE_KEY_EQUIP_PROPERTY_USERCMD_PARA 83
struct stGetOneKeyEquipPropertyUserCmd : public stPropertyUserCmd
{
	stGetOneKeyEquipPropertyUserCmd()
	{
		byParam = GET_ONE_KEY_EQUIP_PROPERTY_USERCMD_PARA;
	}
	DWORD dwOldTempID;
};

//sky 发送一键换装信息
#define RESPONSE_PROPERTY_USERCMD_ONEKEY_EQUIP 0x82
struct stResponsePropertyUserCmdOnekeyEquip : public stPropertyUserCmd
{
	stResponsePropertyUserCmdOnekeyEquip()
	{
		byParam = RESPONSE_PROPERTY_USERCMD_ONEKEY_EQUIP;
	}

		struct One_Key_Equip OneKeyEqiup[10];
};

//寒冰外形
#define GO_HANBING_PROPERTY_USERCMD_PARA 85
struct stGoHanbingPropertyUserCmd : public stPropertyUserCmd
{
	stGoHanbingPropertyUserCmd()
	{
		byParam = GO_HANBING_PROPERTY_USERCMD_PARA;
	}
	DWORD level;//
};

//翅膀外形
#define GO_ANGEL_PROPERTY_USERCMD_PARA 86
struct stGoAngelPropertyUserCmd : public stPropertyUserCmd
{
	stGoAngelPropertyUserCmd()
	{
		byParam = GO_ANGEL_PROPERTY_USERCMD_PARA;
	}
	DWORD level;//
};

//sky 桃子增加定时传送活动1
#define GO_CHUANSONG_PROPERTY_USERCMD_PARA 87
struct stGoChuanSongPropertyUserCmd : public stPropertyUserCmd
{
	stGoChuanSongPropertyUserCmd()
	{
		byParam = GO_CHUANSONG_PROPERTY_USERCMD_PARA;
	}
};
//结束
//sky 桃子增加定时传送活动2
#define GO_CHUANSONG2_PROPERTY_USERCMD_PARA 88
struct stGoChuanSong2PropertyUserCmd : public stPropertyUserCmd
{
	stGoChuanSong2PropertyUserCmd()
	{
		byParam = GO_CHUANSONG2_PROPERTY_USERCMD_PARA;
	}
};
//结束

//国家镖车传送
#define GO_CHUANSONG3_PROPERTY_USERCMD_PARA 98
struct stGoChuanSong3PropertyUserCmd : public stPropertyUserCmd
{
	stGoChuanSong3PropertyUserCmd()
	{
		byParam = GO_CHUANSONG3_PROPERTY_USERCMD_PARA;
	}
};
#define GO_CHUANSONG4_PROPERTY_USERCMD_PARA 99
struct stGoChuanSong4PropertyUserCmd : public stPropertyUserCmd
{
	stGoChuanSong4PropertyUserCmd()
	{
		byParam = GO_CHUANSONG4_PROPERTY_USERCMD_PARA;
	}
};


#define GO_CHUANSONG5_PROPERTY_USERCMD_PARA 107
struct stGoChuanSong5PropertyUserCmd : public stPropertyUserCmd
{
	stGoChuanSong5PropertyUserCmd()
	{
		byParam = GO_CHUANSONG5_PROPERTY_USERCMD_PARA;
	}
};
#define GO_CHUANSONG6_PROPERTY_USERCMD_PARA 108
struct stGoChuanSong6PropertyUserCmd : public stPropertyUserCmd
{
	stGoChuanSong6PropertyUserCmd()
	{
		byParam = GO_CHUANSONG6_PROPERTY_USERCMD_PARA;
	}
};

//sky 获取称号信息 醉梦
#define GET_CHENGHAO_PROPERTY_USERCMD_PARA 89
struct stGetChenghaoPropertyUserCmd : public stPropertyUserCmd
{
	stGetChenghaoPropertyUserCmd()
	{
		byParam = GET_CHENGHAO_PROPERTY_USERCMD_PARA;
	}
	DWORD dwOldTempID;
};


//sky 发送称号信息 醉梦
#define RESPONSE_PROPERTY_USERCMD_CHENGHAO 90
struct stResponsePropertyUserCmdChenghao : public stPropertyUserCmd
{
	stResponsePropertyUserCmdChenghao()
	{
		byParam = RESPONSE_PROPERTY_USERCMD_CHENGHAO;
	}

	struct one_equip
	{
		DWORD state;
	};
	struct one_equip Chenghao[60]; 
};

//sky 请求佩戴称号 醉梦
#define SETCHENGHAO_USERCMD 90
struct stSetChenghaoUserCmd : public stPropertyUserCmd
{
	stSetChenghaoUserCmd()
	{
		byParam = SETCHENGHAO_USERCMD;
	}
	DWORD dwOldTempID;
	DWORD chenghaoid;
};

//sky 请求取消佩戴称号 醉梦
#define SETNOTCHENGHAO_USERCMD 91
struct stSetNotChenghaoUserCmd : public stPropertyUserCmd
{
	stSetNotChenghaoUserCmd()
	{
		byParam = SETNOTCHENGHAO_USERCMD;
	}
	DWORD dwOldTempID;
	DWORD chenghaoid;
};


//sky 获取时装魔盒信息 醉梦
#define GET_SHIZHUANG_PROPERTY_USERCMD_PARA 92
struct stGetShizhuangPropertyUserCmd : public stPropertyUserCmd
{
	stGetShizhuangPropertyUserCmd()
	{
		byParam = GET_SHIZHUANG_PROPERTY_USERCMD_PARA;
	}
	DWORD dwOldTempID;
};



//sky 发送时装魔盒信息 醉梦
#define RESPONSE_PROPERTY_USERCMD_SHIZHUANG 93
struct stResponsePropertyUserCmdShizhuang : public stPropertyUserCmd
{
	stResponsePropertyUserCmdShizhuang()
	{
		byParam = RESPONSE_PROPERTY_USERCMD_SHIZHUANG;
	}

	struct one_shizhuang
	{
		char name[MAX_NAMESIZE];
		DWORD bodyNum;
		DWORD level;
		DWORD state;
	};
	struct one_shizhuang Shizhuang[300]; 
	struct one_pifeng
	{
		char name[MAX_NAMESIZE];
		DWORD itemNum;
		DWORD level;
		DWORD state;
	};
	struct one_chibang
	{
		char name[MAX_NAMESIZE];
		DWORD itemNum;
		DWORD level;
		DWORD state;
	};
	struct one_zuoqi
	{
		char name[MAX_NAMESIZE];
		DWORD npcNum;
		DWORD level;
		DWORD state;
	};
	struct one_pifeng Pifeng[100]; 
	struct one_chibang Chibang[100]; 
	struct one_zuoqi Zuoqi[100]; 
	DWORD mohelevel;
	DWORD moheexp;
	DWORD mohemaxexp;
	DWORD shizhuang_select;     //时装选择
	DWORD pifeng_select;     //披风选择
	DWORD chibang_select;     //翅膀选择
	DWORD zuoqi_select;     //坐骑选择
	DWORD num1;     //魔盒加成
	DWORD num2;     //魔盒加成
	DWORD num3;     //魔盒加成
	DWORD num4;     //魔盒加成
	DWORD num5;     //魔盒加成
};

//sky 请求佩戴时装 醉梦
#define SETSHIZHUANG_USERCMD 94
struct stSetShizhuangUserCmd : public stPropertyUserCmd
{
	stSetShizhuangUserCmd()
	{
		byParam = SETSHIZHUANG_USERCMD;
	}
	DWORD dwOldTempID;
	DWORD shizhuangid;
};

//sky 请求取消佩戴时装 醉梦
#define SETNOTSHIZHUANG_USERCMD 95
struct stSetNotShizhuangUserCmd : public stPropertyUserCmd
{
	stSetNotShizhuangUserCmd()
	{
		byParam = SETNOTSHIZHUANG_USERCMD;
	}
	DWORD dwOldTempID;
	DWORD shizhuangid;
};

//sky 请求激活/升级 醉梦
#define SETSHIZHUANGJIHUO_USERCMD 96
struct stSetShizhuangJihuoUserCmd : public stPropertyUserCmd
{
	stSetShizhuangJihuoUserCmd()
	{
		byParam = SETSHIZHUANGJIHUO_USERCMD;
	}
	DWORD dwOldTempID;
	DWORD shizhuangid;
};

//sky 请求佩戴披风 醉梦
#define SETPIFENG_USERCMD 101
struct stSetPifengUserCmd : public stPropertyUserCmd
{
	stSetPifengUserCmd()
	{
		byParam = SETPIFENG_USERCMD;
	}
	DWORD dwOldTempID;
	DWORD pifengid;
};

//sky 请求取消佩戴披风 醉梦
#define SETNOTPIFENG_USERCMD 102
struct stSetNotPifengUserCmd : public stPropertyUserCmd
{
	stSetNotPifengUserCmd()
	{
		byParam = SETNOTPIFENG_USERCMD;
	}
	DWORD dwOldTempID;
	DWORD pifengid;
};


//sky 请求激活/升级披风 醉梦
#define SETPIFENGJIHUO_USERCMD 103
struct stSetPifengJihuoUserCmd : public stPropertyUserCmd
{
	stSetPifengJihuoUserCmd()
	{
		byParam = SETPIFENGJIHUO_USERCMD;
	}
	DWORD dwOldTempID;
	DWORD pifengid;
};



//sky 请求佩戴翅膀 醉梦
#define SETCHIBANG_USERCMD 104
struct stSetChibangUserCmd : public stPropertyUserCmd
{
	stSetChibangUserCmd()
	{
		byParam = SETCHIBANG_USERCMD;
	}
	DWORD dwOldTempID;
	DWORD chibangid;
};

//sky 请求取消佩戴翅膀 醉梦
#define SETNOTCHIBANG_USERCMD 105
struct stSetNotChibangUserCmd : public stPropertyUserCmd
{
	stSetNotChibangUserCmd()
	{
		byParam = SETNOTCHIBANG_USERCMD;
	}
	DWORD dwOldTempID;
	DWORD chibangid;
};


//sky 请求激活/升级翅膀 醉梦
#define SETCHIBANGJIHUO_USERCMD 106
struct stSetChibangJihuoUserCmd : public stPropertyUserCmd
{
	stSetChibangJihuoUserCmd()
	{
		byParam = SETCHIBANGJIHUO_USERCMD;
	}
	DWORD dwOldTempID;
	DWORD chibangid;
};


//sky 请求佩戴坐骑 醉梦
#define SETZUOQI_USERCMD 109
struct stSetZuoqiUserCmd : public stPropertyUserCmd
{
	stSetZuoqiUserCmd()
	{
		byParam = SETZUOQI_USERCMD;
	}
	DWORD dwOldTempID;
	DWORD zuoqiid;
};

//sky 请求取消佩戴坐骑 醉梦
#define SETNOTZUOQI_USERCMD 110
struct stSetNotZuoqiUserCmd : public stPropertyUserCmd
{
	stSetNotZuoqiUserCmd()
	{
		byParam = SETNOTZUOQI_USERCMD;
	}
	DWORD dwOldTempID;
	DWORD zuoqiid;
};


//sky 请求激活/升级坐骑 醉梦
#define SETZUOQIJIHUO_USERCMD 111
struct stSetZuoqiJihuoUserCmd : public stPropertyUserCmd
{
	stSetZuoqiJihuoUserCmd()
	{
		byParam = SETZUOQIJIHUO_USERCMD;
	}
	DWORD dwOldTempID;
	DWORD zuoqiid;
};


//sky 请求传送每日任务 醉梦
#define MEIRIRENWUCHUANSONG_USERCMD 97
struct stMeiRiRenWuChuanSongUserCmd : public stPropertyUserCmd
{
	stMeiRiRenWuChuanSongUserCmd()
	{
		byParam = MEIRIRENWUCHUANSONG_USERCMD;
	}
	DWORD dwOldTempID;
	DWORD num;
};

//sky 请求随机传送 醉梦
#define SUIJICHUANSONG_USERCMD 112
struct stSuiJiChuanSongUserCmd : public stPropertyUserCmd
{
	stSuiJiChuanSongUserCmd()
	{
		byParam = SUIJICHUANSONG_USERCMD;
	}
};
//////////////////////////////////////////////////////////////
// 用户属性指令定义结束
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// 用户聊天指令定义开始
//////////////////////////////////////////////////////////////
#ifndef _CHAT_USERCMD_DEFINITION_
#define _CHAT_USERCMD_DEFINITION_

struct stChatUserCmd : public stNullUserCmd
{
	stChatUserCmd()
	{
		byCmd = CHAT_USERCMD;
	}
};

enum enumChatType{
	CHAT_TYPE_PRIVATE  = 1,			/// 私聊频道
	CHAT_TYPE_NINE,					/// 轻聊频道
	CHAT_TYPE_TEAM,                 /// 队伍频道
	CHAT_TYPE_FRIEND,               /// 好友频道
	CHAT_TYPE_GM,                   /// GM聊频道
	CHAT_TYPE_SYSTEM,               /// 系统频道
	CHAT_TYPE_UNION,                /// 帮会频道
	CHAT_TYPE_POP,                  /// 弹出式系统提示
	CHAT_TYPE_PERSON,               /// 个人频道
	CHAT_TYPE_WHISPER,              /// 悄悄话
	CHAT_TYPE_WHISPERTOME,          /// 悄悄话
	CHAT_TYPE_COUNTRY,              /// 国家频道
	CHAT_TYPE_AREA,					///区域频道
	CHAT_TYPE_FAMILY,               /// 家族频道

	CHAT_TYPE_FRIEND_AFFICHE,       /// 好友公告
	CHAT_TYPE_UNION_AFFICHE,        /// 帮会公告
	CHAT_TYPE_OVERMAN_AFFICHE,      /// 师门公告
	CHAT_TYPE_FAMILY_AFFICHE,       /// 家族公告

	CHAT_TYPE_FRIEND_PRIVATE,       /// 好友私聊
	CHAT_TYPE_UNION_PRIVATE,        /// 帮会私聊
	CHAT_TYPE_OVERMAN_PRIVATE,      /// 师门私聊
	CHAT_TYPE_FAMILY_PRIVATE,       /// 家族私聊

	CHAT_TYPE_NPC,                   ///npc说话

	CHAT_TYPE_EMOTION,      ///表情
	CHAT_TYPE_SHOPADV,      ///摆摊广告
	CHAT_TYPE_WORLD,        ///世界频道
	CHAT_TYPE_OVERMAN,      /// 师门频道
	CHAT_TYPE_AUTO,         /// 自动回复
	CHAT_TYPE_COUNTRY_PK,   /// 外国人入侵PK消息
	CHAT_TYPE_BLESS_MSG,    /// 个人祝福消息
	CHAT_TYPE_COUNTRY_MARRY, /// 结婚消息广播
	CHAT_TYPE_ERROR_GM, ///发送到GM工具的警告信息
	CHAT_TYPE_MINIGAME,  /// 玩小游戏聊天
	CHAT_TYPE_SYTEM      /// 左下角系统公告轮播提示(红)

};

enum enumSysInfoType
{
	INFO_TYPE_SYS   = 1,    /// 系统信息、GM信息，在聊天窗口
	INFO_TYPE_GAME,         /// 游戏信息，屏幕右下（绿）
	INFO_TYPE_GAME1,        ///soke 游戏信息，屏幕右下（淡黄）
	INFO_TYPE_GAME2,        ///soke 游戏信息，屏幕右下（黄）
	INFO_TYPE_GAME3,        ///soke 游戏信息，屏幕右下（紫）
	INFO_TYPE_GAME4,        ///soke 游戏信息，屏幕右下（橙）
	INFO_TYPE_GAME5,        ///soke 游戏信息，屏幕右下（红）
	INFO_TYPE_GAME6,        ///soke 游戏信息，倒计时10秒
	INFO_TYPE_GAME7,        ///soke 游戏信息，新的特效
	INFO_TYPE_STATE,        /// 状态转换，屏幕左上
	INFO_TYPE_FAIL,         /// 失败信息，屏幕右下
	INFO_TYPE_EXP,          /// 特殊信息,获得经验、物品，在人物头上（黄）
	INFO_TYPE_EXP1,         ///soke 特殊信息,获得经验、物品，在人物头上（淡黄）
	INFO_TYPE_EXP2,         ///soke 特殊信息,获得经验、物品，在人物头上（绿）
	INFO_TYPE_EXP3,         ///soke 特殊信息,获得经验、物品，在人物头上（紫）
	INFO_TYPE_EXP4,         ///soke 特殊信息,获得经验、物品，在人物头上（橙）
	INFO_TYPE_EXP5,         ///soke 特殊信息,获得经验、物品，在人物头上（红）
	INFO_TYPE_EXP6,         ///soke 特殊信息,获得经验、物品，在人物头上（蓝）
	INFO_TYPE_MSG,          /// 弹出用户确认框的系统消息
	INFO_TYPE_SCROLL,       /// 屏幕上方滚动的系统信息
	INFO_TYPE_ZISEHROLL,    ///soke 屏幕中间滚动的系统信息(黄色)
	INFO_TYPE_ZISELROLL,    ///soke 屏幕中间滚动的系统信息(绿色)
	INFO_TYPE_ZISEZROLL,    ///soke 屏幕中间滚动的系统信息(紫色)
	INFO_TYPE_ZISEFROLL,    ///soke 屏幕中间滚动的系统信息(橙色)
	INFO_TYPE_ZISETROLL,    ///soke 屏幕中间滚动的系统信息(红色)
	INFO_TYPE_ZISEJROLL,    ///soke 屏幕中间滚动的系统信息(结婚专用-桔红)
	INFO_TYPE_ZISESROLL,    ///soke 屏幕中间滚动的系统信息(粉红-送花)
	INFO_TYPE_ZISESRXLL,    ///soke 屏幕中间滚动的系统信息(蓝色妖姬-送花)
	INFO_TYPE_ZISEYROLL,    ///soke 屏幕中间滚动的系统信息(深红-好友膜拜)
	INFO_TYPE_XTYSCROLL,    ///soke 屏幕滚动信息(上方小字滚动公告 BOSS复活专用)
	INFO_TYPE_ADDFRIEND,    /// 加好友任务显示和动画播放
	INFO_TYPE_BREAKFRIEND,  /// 割袍断义任务显示和动画播放
	INFO_TYPE_SKYROCKET1,   /// 焰火类型1
	INFO_TYPE_SKYROCKET2,   /// 焰火类型2
	INFO_TYPE_ATT_FLAG,     /// 旗子被攻击
    INFO_TYPE_CATCHER,      ///soke 巡捕（左）发出的聊天消息
	INFO_TYPE_CATCHEX,      ///soke 巡捕（右）发出的聊天消息
	INFO_TYPE_CENSOR,       ///soke 御史大夫（左）发出的聊天消息
	INFO_TYPE_CENSOS,       ///soke 御史大夫（右）发出的聊天消息
	INFO_TYPE_YUANSHUAI,    ///soke 元帅发出的聊天消息
	INFO_TYPE_ZAIXIANG,     ///soke 宰相发出的聊天消息
	INFO_TYPE_CASTELLAN,    ///soke 城主发出的聊天消息
	INFO_TYPE_KING,         ///soke 国王、女王发出的聊天消息
	INFO_TYPE_CONSORT,      ///soke 王后、王夫发出的聊天消息
	INFO_TYPE_EMPEROR,	    ///soke 皇帝、女皇发出的聊天消息
	INFO_TYPE_CONSORZ,      ///soke 皇后、皇夫发出的聊天消息
	INFO_TYPE_SONGGUO,      ///soke 宋国玩家世界的聊天消息
	INFO_TYPE_WEIGUO,       ///soke 魏国玩家世界的聊天消息
	INFO_TYPE_QIGUO,        ///soke 齐国玩家世界的聊天消息
	INFO_TYPE_YANGUO,       ///soke 燕国玩家世界的聊天消息
	INFO_TYPE_TANGGUO,      ///soke 唐国玩家世界的聊天消息
	INFO_TYPE_HANGUO,       ///soke 汉国玩家世界的聊天消息
	INFO_TYPE_WUGUO,        ///soke 吴国玩家世界的聊天消息
	INFO_TYPE_ZHAOGUO,      ///soke 赵国玩家世界的聊天消息
	INFO_TYPE_ZHOUGUO,      ///soke 周国玩家世界的聊天消息
	INFO_TYPE_CHUGUO,       ///soke 楚国玩家世界的聊天消息
	INFO_TYPE_ZTBB,         ///soke 征途宝贝发出的聊天消息
	INFO_TYPE_XG1,          /// 重要信息公告 - 战争
	INFO_TYPE_XG2,          /// 重要信息公告 - BOSS
	INFO_TYPE_XG3,          /// 重要信息公告 - 其他
	INFO_TYPE_XG4,          /// 重要信息公告 - 系统
    INFO_TYPE_GOTO,         /// 桃子增加定时传送活动1
	INFO_TYPE_GOTO2,         /// 桃子增加定时传送活动2
	INFO_TYPE_YANSE1,         ///soke 新定义颜色 紫色滚动公告带框 醉梦定义颜色2024-5-10 03:43:20
	INFO_TYPE_TIANJIANG,         ///soke 天降洪福专属 醉梦定义颜色2024-5-10 03:43:20 这个是飘屏特效 跟文字没关系
	INFO_TYPE_BIAOCHE1,         ///sky 醉梦 国家镖车传送
	INFO_TYPE_BIAOCHE2,         ///sky 醉梦 国家镖车传送
	INFO_TYPE_BOSS,         ///sky 醉梦 世界BOSS传说
	INFO_TYPE_BIKE,         ///sky 醉梦 自行车传送
	
};

#define ALL_CHAT_USERCMD_PARAMETER 1
struct stTradeObject
{
	DWORD dwObjectID;
	DWORD dwObjectTempID;
	char strName[MAX_NAMESIZE];
	BYTE upgrade;//物品升级等级
	WORD kind;      //物品类型, 0普通, 1蓝色, 2金色, 4神圣, 8套装
};

///客户端发送到服务器的消息
struct  stChannelChatUserCmd: public stChatUserCmd
{
	DWORD dwType;                   /**< 消息类型 */
	DWORD dwSysInfoType;            /**< 系统消息的类型 */
	DWORD dwCharType;               /**< 角色类型 */
	DWORD dwChannelID;
	DWORD dwFromID;                 ///发送者的ID,是临时id
	DWORD dwChatTime;               // 谈话时间
	BYTE size;                      // 物品数量
	char pstrName[MAX_NAMESIZE];    /**< 名字 */
	char pstrChat[MAX_CHATINFO];    /**< 对话 */
	stTradeObject tobject_array[0]; // 交易物品列表
	stChannelChatUserCmd()
	{
		bzero(pstrChat,sizeof(pstrChat));
		bzero(pstrName,sizeof(pstrName));
		byParam = ALL_CHAT_USERCMD_PARAMETER;
		dwChannelID=0;
		dwSysInfoType =1;
		dwCharType = 1;
		size = 0;
	}
};

//从服务器发过来的消息
struct  stServerChannelChatUserCmd: public stChatUserCmd
{
	DWORD dwType;           /**< 消息类型 */
	DWORD dwSysInfoType;    /**< 系统消息的类型 */ //表情类型
	DWORD dwCharType;       /**< 角色类型 */
	DWORD dwChannelID;
	DWORD dwFromID;         ///发送者的ID,是临时id
	DWORD dwChatTime;       // 谈话时间
	BYTE  size;             // 物品数量
	char  info[0];
	stServerChannelChatUserCmd()
	{
		byParam = ALL_CHAT_USERCMD_PARAMETER;
		dwChannelID=0;
		dwSysInfoType =1;
		dwCharType = 1;
		size = 0;
	}
};
// 解散队伍
#define REMOVE_TEAM_USERCMD_PARA 2
struct stRemoveTeamUserCmd : public stChatUserCmd
{
	stRemoveTeamUserCmd()
	{
		byParam = REMOVE_TEAM_USERCMD_PARA;
	}
};

// 添加队员，刷新队员数据 指令
#define ADD_TEAMMEMBER_USERCMD_PARA 3
struct stTeamMemberData
{
	char pstrName[MAX_NAMESIZE];    // 队员名字
	BYTE byHead;                    // 是否是队长
	DWORD dwTempID;                 //成员tempid
	DWORD dwMaxHealth;              // HP
	DWORD dwHealth;
	DWORD dwMaxMp;                  //MP
	DWORD dwMp;
	WORD wdFace;                    //肖像
}; 
struct stAddTeamMemberUserCmd: public stChatUserCmd
{
	stAddTeamMemberUserCmd()
	{
		byParam = ADD_TEAMMEMBER_USERCMD_PARA;
	}
	DWORD dwTeamID;					// 队伍ID
	stTeamMemberData data;
};

// 删除队员指令
#define REMOVE_TEAMMEMBER_USERCMD_PARA 4
struct stRemoveTeamMemberUserCmd: public stChatUserCmd
{
	stRemoveTeamMemberUserCmd()
	{
		byParam = REMOVE_TEAMMEMBER_USERCMD_PARA;
	}
	DWORD dwTeamID;					// 队伍ID
	//char pstrName[MAX_NAMESIZE];	// 队员名字
	DWORD dwTempID;                 //队员tempid
};


/**
* 队伍模式
*/
enum
{
	TEAM_NORMAL,//普通模式
	TEAM_HONOR,//荣誉模式
	TEAM_SPECIAL,//特殊模式
	TEAM_MAX,//无效模式
};

#define __REMOTE__CAN__MAKE__TEAM__
#ifndef __REMOTE__CAN__MAKE__TEAM__
	// 请求组队指令, 该指令需要发给被请求者
	#define REQUEST_TEAM_USERCMD_PARA 5
	struct stRequestTeamUserCmd: public stChatUserCmd
	{
		stRequestTeamUserCmd()
		{
			byParam = REQUEST_TEAM_USERCMD_PARA;
		}
		DWORD dwAnswerUserID;		// 回答者ID,返回给客户端是请求者ID
		DWORD dwTeamMode;           // 队伍模式
	};
	// 回答组队指令
	#define ANSWER_TEAM_USERCMD_PARA 6
	struct stAnswerTeamUserCmd: public stChatUserCmd
	{
		stAnswerTeamUserCmd()
		{
			byParam = ANSWER_TEAM_USERCMD_PARA;
		}
		DWORD dwRequestUserID;		// 请求者ID
		DWORD dwAnswerUserID;		// 回答者ID
		BYTE  byAgree;				// 是否同意
		DWORD dwTeamMode;           // 队伍模式
	};
#else 
	// 回答组队指令
	#define ANSWER_NAME_TEAM_USERCMD_PARA 21
	struct stAnswerNameTeamUserCmd: public stChatUserCmd
	{
		stAnswerNameTeamUserCmd()
		{
			byParam = ANSWER_NAME_TEAM_USERCMD_PARA;
		}
		char byRequestUserName[MAX_NAMESIZE];       // 请求者名字
		char byAnswerUserName[MAX_NAMESIZE];        // 回答者名字
		BYTE  byAgree;              // 是否同意
		DWORD dwTeamMode;           // 队伍模式
	};
	// 请求组队指令, 该指令需要发给被请求者
	#define REQUEST_NAME_TEAM_USERCMD_PARA 22
	struct stRequestNameTeamUserCmd: public stChatUserCmd
	{
		stRequestNameTeamUserCmd()
		{
			byParam = REQUEST_NAME_TEAM_USERCMD_PARA;
		}
		char byAnswerUserName[MAX_NAMESIZE];        // 回答者名字,返回给客户端是请求者名字
		DWORD dwTeamMode;           // 队伍模式
	};
#endif

#define CREATE_CHANNEL_USERCMD_PARAMETER 7  
struct  stCreateChannelUserCmd: public stChatUserCmd
{                               
	DWORD dwChannelID;          
	DWORD dwClientID;       
	char name[MAX_NAMESIZE];    
	char name2[MAX_NAMESIZE];

	stCreateChannelUserCmd()        
	{                               
		byParam = CREATE_CHANNEL_USERCMD_PARAMETER;
	}                               
};

#define INVITE_CHANNEL_USERCMD_PARAMETER 8
struct  stInvite_ChannelUserCmd: public stChatUserCmd
{
	DWORD dwChannelID;
	char name[MAX_NAMESIZE];
	DWORD   dwCharType;

	stInvite_ChannelUserCmd()
	{
		byParam = INVITE_CHANNEL_USERCMD_PARAMETER;
	}
};

#define JOIN_CHANNEL_USERCMD_PARAMETER 9
struct  stJoin_ChannelUserCmd: public stChatUserCmd
{
	DWORD dwChannelID;
	char name[MAX_NAMESIZE];

	stJoin_ChannelUserCmd()
	{
		byParam = JOIN_CHANNEL_USERCMD_PARAMETER;
	}
};

#define LEAVE_CHANNEL_USERCMD_PARAMETER 10
struct  stLeave_ChannelUserCmd: public stChatUserCmd
{
	DWORD dwChannelID;
	char name[MAX_NAMESIZE];

	stLeave_ChannelUserCmd()
	{
		byParam = LEAVE_CHANNEL_USERCMD_PARAMETER;
	}
};

#define REQUEST_TEAMMEMBER_POSITION_USERCMD_PARA 11

// 请求队员位置
struct stRequestTeamMemberPosition : public stChatUserCmd
{
	stRequestTeamMemberPosition()
	{
		byParam = REQUEST_TEAMMEMBER_POSITION_USERCMD_PARA;
	}
};
// 服务器返回队员位置
#define RETURN_TEAMMEMBER_POSITION_USERCMD_PARA 11
struct stTeamMemberPosition
{
	char szName[MAX_NAMESIZE];
	char szMapName[MAX_NAMESIZE];
	int x,y;
};
struct stReturnTeamMemberPosition : public stChatUserCmd
{
	stReturnTeamMemberPosition()
	{
		byParam = RETURN_TEAMMEMBER_POSITION_USERCMD_PARA;
	}
	DWORD size;
	stTeamMemberPosition memberPos[0];
};

#define QUESTION_OBJECT_USERCMD_PARA 12
struct stQuestionObject : public stChatUserCmd
{
	stQuestionObject()
	{
		byParam = QUESTION_OBJECT_USERCMD_PARA;
	}

	char name[MAX_NAMESIZE];        /**< 物品拥有者的名字 */
	DWORD dwObjectTempID;          /// 物品临时ID
};

#define RETURN_QUESTION_OBJECT_USERCMD_PARA 13
struct stReturnQuestionObject : public stChatUserCmd
{
	stReturnQuestionObject()
	{
		byParam = RETURN_QUESTION_OBJECT_USERCMD_PARA;
	}

	char name[MAX_NAMESIZE];        /**< 物品拥有者的名字 */
	t_Object object;               /// 物品资料
};

// 队员离线消息
#define TEAMMEMBER_OFFLINE_USERCMD_PARA 14
struct stTeamMemberOfflineUserCmd : public stChatUserCmd
{
	stTeamMemberOfflineUserCmd()
	{
		byParam = TEAMMEMBER_OFFLINE_USERCMD_PARA;
	}
	DWORD dwTempID;                                 //队员tempid
};

// 设置自动回复消息                
#define AUTO_REPLY_USERCMD_PARA 15
struct stAutoReplyUserCmd : public stChatUserCmd
{
	stAutoReplyUserCmd()
	{
		byParam = AUTO_REPLY_USERCMD_PARA;
	}
	char text[MAX_CHATINFO];
};
enum
{
	TEAM_EXP_MODE_NORMAL,//普通分配经验模式
	TEAM_EXP_MODE_SPECIAL,//特殊分配经验模式
};  
enum
{       
	TEAM_OBJ_MODE_NORMAL,//普通物品分配模式
	TEAM_OBJ_MODE_SPECIAL,//轮流物品分配模式
};  

// 请求(设置)队伍经验分配模式
#define EXPMODE_TEAMMEMBER_USERCMD_PARA 16
struct stExpModeTeamMemberUserCmd: public stChatUserCmd
{
	stExpModeTeamMemberUserCmd()
	{
		byParam = EXPMODE_TEAMMEMBER_USERCMD_PARA;
	}   
	BYTE byType;
};
// 请求(设置)队伍物品分配模式   
#define OBJMODE_TEAMMEMBER_USERCMD_PARA 17
struct stObjModeTeamMemberUserCmd: public stChatUserCmd
{
	stObjModeTeamMemberUserCmd()
	{
		byParam = OBJMODE_TEAMMEMBER_USERCMD_PARA;
	}
	BYTE byType;
};
// 设置组队模式
#define TEAM_MODE_USERCMD_PARA 18
struct stTeamModeUserCmd: public stChatUserCmd
{
	stTeamModeUserCmd()
	{
		byParam = TEAM_MODE_USERCMD_PARA;
	}
	BYTE byType; 
};

// 刷新队员HP,MP 指令
#define FRESH_TEAMMEMBER_USERCMD_PARA 19
struct stFreshTeamMemberUserCmd: public stChatUserCmd
{
	stFreshTeamMemberUserCmd()
	{
		byParam = FRESH_TEAMMEMBER_USERCMD_PARA;
	}
	DWORD dwTempID;                 //成员tempid
	DWORD dwMaxHealth;              // HP   
	DWORD dwHealth;
	WORD dwMaxMp;                   //MP    
	WORD dwMp;
};

// 检测到非法用户
#define DIRTY_USER_USERCMD_PARA 20
enum DIRTYUSER_TYPE{
	DIRTYUSER_DLL_CALL = 1,	// 外部DLL调用本进程函数，一般外挂用户
	DIRTYUSER_DEBUGGER = 2,	// 启用调试器调试本进程，可能是外挂制作者
};
struct stDirtyUserUserCmd : public stChatUserCmd
{
	stDirtyUserUserCmd()
	{
		byParam = DIRTY_USER_USERCMD_PARA;
	}
	BYTE type;
};
// 21 在上面

#define REQUEST_COUNTRY_HELP_USERCMD_PARA 23
struct stRequestCountryHelpUserCmd: public stChatUserCmd
{
	stRequestCountryHelpUserCmd()
	{
		byParam = REQUEST_COUNTRY_HELP_USERCMD_PARA;
	}
	char mapName[MAX_NAMESIZE];     //请求支援的地图名称
	WORD x; 
	WORD y; 
};
#define KILL_FOREIGNER_USERCMD_PARA 24
struct stKillForeignerUserCmd: public stChatUserCmd
{
	stKillForeignerUserCmd()
	{
		byParam = KILL_FOREIGNER_USERCMD_PARA;
	}
	char heroName[MAX_NAMESIZE];    //英雄人物
	char mapName[MAX_NAMESIZE];     //立功地图
};

// BOSS刷新提示消息
#define REFRESH_BOSS_USERCMD_PARA 25
struct stRefreshBossUserCmd: public stChatUserCmd
{
	stRefreshBossUserCmd()
	{
		byParam = REFRESH_BOSS_USERCMD_PARA;
	}
	DWORD npcid;
	BYTE country; //国家
	char mapName[MAX_NAMESIZE];		//BOSS刷新的地图名称
	BYTE time;     // 剩余的刷新时间，如果为0表示刷出
	//WORD x;
	//WORD y;
};

// BOSS被杀提示消息
#define KILL_BOSS_USERCMD_PARA 26
struct stKillBossUserCmd: public stChatUserCmd
{
	stKillBossUserCmd()
	{
		byParam = KILL_BOSS_USERCMD_PARA;
	}
	DWORD npcid;
	char name[MAX_NAMESIZE];		//玩家名称
	char mapName[MAX_NAMESIZE];		//BOSS刷新的地图名称
	WORD x;
	WORD y;
};


// //ZH 升级好友祝贺
#define FRIEND_CONGRATULATE_USERCMD_PARA 27
struct stFriendCongratulateUserCmd: public stChatUserCmd
{
	stFriendCongratulateUserCmd()
	{
		byParam = FRIEND_CONGRATULATE_USERCMD_PARA;
	}
	DWORD playerid;
	char name[MAX_NAMESIZE];		//玩家名称
	DWORD dwLevel;
	DWORD dwType;//1男2女
};


#define FRIEND_CONGRATULATEINFO_USERCMD_PARA 28
struct stFriendCongratulateInfoUserCmd: public stChatUserCmd
{
	stFriendCongratulateInfoUserCmd()
	{
		byParam = FRIEND_CONGRATULATEINFO_USERCMD_PARA;
	}
	DWORD PlayerId;
	char name[MAX_NAMESIZE];		//玩家名称
	char info[512];		//祝贺内容
};


//////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////
// 用户聊天指令定义结束
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// 交易指令定义开始
//////////////////////////////////////////////////////////////
#ifndef _TRADE_USERCMD_DEFINITION_
#define _TRADE_USERCMD_DEFINITION_
struct stTradeUserCmd : public stNullUserCmd
{
	stTradeUserCmd()
	{
		byCmd = TRADE_USERCMD;
	}
};

#define REQUEST_TRADE_USERCMD_PARAMETER 0
struct stRequestTradeUserCmd :public stTradeUserCmd
{
	stRequestTradeUserCmd()
	{
		byParam = REQUEST_TRADE_USERCMD_PARAMETER;
	}
	DWORD dwAskerTempID;	/**< 请求者 */
	DWORD dwAnswerTempID;	/**< 回答者 */
};

/// 回应交易请求
#define ANSWER_TRADE_USERCMD_PARAMETER 1
struct stAnswerTradeUserCmd:public stTradeUserCmd
{
	stAnswerTradeUserCmd()
	{
		byParam = ANSWER_TRADE_USERCMD_PARAMETER;
	}

	BYTE byAgree;				/**< 是否同意交易 */
	DWORD dwAskerTempID;	/**< 请求者 */
	DWORD dwAnswerTempID;	/**< 回答者 */
};

/// 开始交易
#define BEGIN_TRADE_USERCMD_PARAMETER 2
struct stBeginTradeUserCmd:public stTradeUserCmd
{
	stBeginTradeUserCmd()
	{
		byParam = BEGIN_TRADE_USERCMD_PARAMETER;
	}
	DWORD dwAskerTempID;	/**< 请求者 */
	DWORD dwAnswerTempID;	/**< 回答者 */
};

/// 确定交易
#define COMMIT_TRADE_USERCMD_PARAMETER 5
struct stCommitTradeUserCmd:public stTradeUserCmd
{
	stCommitTradeUserCmd()
	{
		byParam = COMMIT_TRADE_USERCMD_PARAMETER;
	}
	DWORD dwUserTempID;
};

/// 取消交易
#define CANCEL_TRADE_USERCMD_PARAMETER 6
struct stCancelTradeUserCmd:public stTradeUserCmd
{
	stCancelTradeUserCmd()
	{
		byParam = CANCEL_TRADE_USERCMD_PARAMETER;
	}
	DWORD dwUserTempID;
};

/// 交易完成
#define FINISH_TRADE_USERCMD_PARAMETER 7
struct stFinishTradeUserCmd:public stTradeUserCmd
{
	stFinishTradeUserCmd()
	{
		byParam = FINISH_TRADE_USERCMD_PARAMETER;
	}
};


/// 添加交易物品
#define ADD_OBJECT_TRADE_USERCMD_PARAMETER 8
struct stAddObjectTradeUserCmd:public stTradeUserCmd
{
	stAddObjectTradeUserCmd()
	{
		byParam = ADD_OBJECT_TRADE_USERCMD_PARAMETER;
	}
	DWORD user_id; //用户临时id
	//	DWORD object_id; //物品唯一id
	t_Object object; //物品数据
	BYTE x;  //物品放置位置x坐标
	BYTE y; //物品放置位置y坐标
};


/// 删除交易物品
#define REMOVE_OBJECT_TRADE_USERCMD_PARAMETER 9
struct stRemoveObjectTradeUserCmd:public stTradeUserCmd
{
	stRemoveObjectTradeUserCmd()
	{
		byParam = REMOVE_OBJECT_TRADE_USERCMD_PARAMETER;
	}
	DWORD user; //用户id
	DWORD id; //物品唯一id
};


/// 访问Npc
#define VISITNPC_TRADE_USERCMD_PARAMETER 10
struct stVisitNpcTradeUserCmd:public stTradeUserCmd
{
	stVisitNpcTradeUserCmd()
	{
		byParam = VISITNPC_TRADE_USERCMD_PARAMETER;
		menuTxt[0] = '\0';
		byReturn = 0;
	}
	DWORD dwNpcTempID;
	BYTE byReturn;
	char menuTxt[1];
};

/// 买物品
#define BUYOBJECT_NPCTRADE_USERCMD_PARAMETER 11
struct stBuyObjectNpcTradeUserCmd:public stTradeUserCmd
{
	stBuyObjectNpcTradeUserCmd()
	{
		byParam = BUYOBJECT_NPCTRADE_USERCMD_PARAMETER;
	}
	DWORD dwObjectID;
	DWORD dwNum;
	BYTE  nLevel;
};

/// 卖物品
#define SELLOBJECT_NPCTRADE_USERCMD_PARAMETER 12
struct stSellObjectNpcTradeUserCmd:public stTradeUserCmd
{
	stSellObjectNpcTradeUserCmd()
	{
		byParam = SELLOBJECT_NPCTRADE_USERCMD_PARAMETER;
	}
	DWORD qwThisID;				/**< 物品唯一ID */
};

/// 修理物品
#define REPAIROBJECT_NPCTRADE_USERCMD_PARAMETER 13
struct stRepairObjectNpcTradeUserCmd:public stTradeUserCmd
{
	stRepairObjectNpcTradeUserCmd()
	{
		byParam = REPAIROBJECT_NPCTRADE_USERCMD_PARAMETER;
	}
	DWORD id;  //修理物品id，全部修理时为0
	DWORD gem_id;  //修复宝石的唯一id，普通修理填0
};

/// 卖马
#define SELLHORSE_NPCTRADE_USERCMD_PARAMETER 14
struct stSellHorseNpcTradeUserCmd:public stTradeUserCmd
{
	stSellHorseNpcTradeUserCmd()
	{
		byParam = SELLHORSE_NPCTRADE_USERCMD_PARAMETER;
	}

	BYTE action; //action 0:卖, action 1: 放逐
};

/// 仓库信息
#define STORE_INFO_NPCTRADE_USERCMD_PARAMETER 15
struct stStoreInfoNpcTradeUserCmd:public stTradeUserCmd
{
	stStoreInfoNpcTradeUserCmd()
	{
		byParam = STORE_INFO_NPCTRADE_USERCMD_PARAMETER;
	}

	BYTE page; //页数
	BYTE day[0];	//天数
};

/// 开始摆摊
#define START_SELL_USERCMD_PARAMETER 16
struct stSellTradeUserCmd:public stTradeUserCmd
{
	stSellTradeUserCmd()
	{
		byParam = START_SELL_USERCMD_PARAMETER;
	}
};

/// 完成摆摊
#define FINISH_SELL_USERCMD_PARAMETER 17
struct stFinishSellUserCmd:public stTradeUserCmd
{
	stFinishSellUserCmd()
	{
		byParam = FINISH_SELL_USERCMD_PARAMETER;
	}
};

/// 添加物品
#define ADD_OBJECT_SELL_USERCMD_PARAMETER 18
struct stAddObjectSellUserCmd:public stTradeUserCmd
{
	stAddObjectSellUserCmd()
	{
		byParam = ADD_OBJECT_SELL_USERCMD_PARAMETER;
	}
	t_Object object; //物品数据
	DWORD price; //物品价钱
	BYTE x;  //物品放置位置x坐标
	BYTE y; //物品放置位置y坐标
};

/// 删除物品
#define REMOVE_OBJECT_SELL_USERCMD_PARAMETER 19
struct stRemoveObjectSellUserCmd:public stTradeUserCmd
{
	stRemoveObjectSellUserCmd()
	{
		byParam = REMOVE_OBJECT_SELL_USERCMD_PARAMETER;
	}
	DWORD object_id; //物品唯一id
};

/// 请求某人的摆摊信息
#define REQUEST_SELL_INFO_USERCMD_PARAMETER 20
struct stRequestSellInfoUserCmd:public stTradeUserCmd
{
	stRequestSellInfoUserCmd()
	{
		byParam = REQUEST_SELL_INFO_USERCMD_PARAMETER;
	}
	DWORD temp_id; //用户临时ID
};

/// 请求购买物品
#define REQUEST_SELL_BUY_USERCMD_PARAMETER 21
struct stRequestSellBuyUserCmd:public stTradeUserCmd
{
	stRequestSellBuyUserCmd()
	{
		byParam = REQUEST_SELL_BUY_USERCMD_PARAMETER;
	}
	DWORD temp_id; //用户临时ID
	DWORD object_id; //物品唯一ID
};

/// 修改仓库密码
#define UPDATE_STORE_PASS_USERCMD_PARAMETER 22
struct stUpdateStorePassUserCmd:public stTradeUserCmd
{
	stUpdateStorePassUserCmd()
	{
		byParam = UPDATE_STORE_PASS_USERCMD_PARAMETER;
	}
	char oldpass[8]; //旧密码
	char newpass[8]; //新密码
};


#define CAN_SELL_USERCMD_PARAMETER 23
struct stCanSellTradeUserCmd:public stTradeUserCmd
{
	stCanSellTradeUserCmd()
	{
		byParam = CAN_SELL_USERCMD_PARAMETER;
	}
	BYTE status; // 0 success, 1 fail
};



enum
{
	STORN = 1,//升级宝石
	SIVER =2,//银
	SILK  =3,//丝绸
	CRYSTAL =4,//水晶
	EBONY  =5,//乌木
	YINGPI =6,//硬皮
};

/// 客户端 点领物品 发服务器
#define GOLD_GIVE_USERCMD_PARAMETER 24
struct stGoldGiveTradeUserCmd:public stTradeUserCmd
{
	stGoldGiveTradeUserCmd()
	{
		byParam = GOLD_GIVE_USERCMD_PARAMETER;
	}
	BYTE type;  //物品类型
};

/// 金子修理物品
#define REPAIROBJECT_GOLD_NPCTRADE_USERCMD_PARAMETER 25
struct stRepairObjectGoldNpcTradeUserCmd:public stTradeUserCmd
{
	stRepairObjectGoldNpcTradeUserCmd()
	{
		byParam = REPAIROBJECT_GOLD_NPCTRADE_USERCMD_PARAMETER;
	}
	DWORD id;  //修理物品id，全部修理时为0
};


/// 客户端请求领取物品
#define REQUEST_GOLD_GIVE_USERCMD_PARAMETER 26
struct stRequestGoldGiveTradeUserCmd:public stTradeUserCmd
{
	stRequestGoldGiveTradeUserCmd()
	{
		byParam = REQUEST_GOLD_GIVE_USERCMD_PARAMETER;
	}
};
/// 服务器返回可领取物品个数和道具卡个数
#define RETURN_GOLD_GIVE_USERCMD_PARAMETER 27
struct stReturnGoldGiveTradeUserCmd:public stTradeUserCmd
{
	stReturnGoldGiveTradeUserCmd()
	{
		byParam = RETURN_GOLD_GIVE_USERCMD_PARAMETER;

	}
	DWORD Storn_num;//宝石个数 0－69
	DWORD Matarial_num; //材料组数 0－5
	DWORD Card_num;//道具卡张数 0－60000
};

/// 批量添加物品
#define ADD_OBJECT_SELL_LIST_USERCMD_PARAMETER 28
struct stAddObjectSellListUserCmd:public stTradeUserCmd
{           
	stAddObjectSellListUserCmd()
	{   
		byParam = ADD_OBJECT_SELL_LIST_USERCMD_PARAMETER;
		num=0;  
	}   
	WORD num;
	struct  
	{   
		t_Object object; //物品数据
		DWORD price; //物品价钱
		BYTE x;  //物品放置位置x坐标
		BYTE y; //物品放置位置y坐标
	}list[0];
}; 

/// 用户请求添加物品id列表
#define REQUEST_ADD_OBJECT_SELL_USERCMD_PARAMETER 29
struct stRequestAddObjectSellUserCmd:public stTradeUserCmd
{
	stRequestAddObjectSellUserCmd()
	{
		byParam = REQUEST_ADD_OBJECT_SELL_USERCMD_PARAMETER;
		num=0;  
	}
	WORD num;
	struct  tSellObject
	{
		DWORD qwThisID; //物品id
		DWORD price; //物品价钱
		BYTE x;  //物品放置位置x坐标
		BYTE y; //物品放置位置y坐标
	}list[0];
};

///soke 2016 - 3 - 10 工资购买物品
#define BUYOBJECT_SALARYTRADE_USERCMD_PARAMETER 30
struct stBuyObjectSalaryTradeUserCmd:public stTradeUserCmd
{
	stBuyObjectSalaryTradeUserCmd()
	{
		byParam = BUYOBJECT_SALARYTRADE_USERCMD_PARAMETER;
	}

	DWORD dwObjectID;
	DWORD dwNum;
	BYTE  nLevel;
};

//MYY 2023 - 7 - 31 积分点数购买物品
#define BUYOBJECT_TICKETTRADE_USERCMD_PARAMETER 31
struct stBuyObjectTicketTradeUserCmd:public stTradeUserCmd
{
	stBuyObjectTicketTradeUserCmd()
	{
		byParam = BUYOBJECT_TICKETTRADE_USERCMD_PARAMETER;
	}

	DWORD dwObjectID;
	DWORD dwNum;
	BYTE  nLevel;
};
//MYY 2023 - 10 - 10 充值点数购买物品
#define BUYOBJECT_CHONGZHITRADE_USERCMD_PARAMETER 32
struct stBuyObjectChongzhiTradeUserCmd:public stTradeUserCmd
{
	stBuyObjectChongzhiTradeUserCmd()
	{
		byParam = BUYOBJECT_CHONGZHITRADE_USERCMD_PARAMETER;
	}

	DWORD dwObjectID;
	DWORD dwNum;
	BYTE  nLevel;
};
//MYY 2023 - 10 - 15 新商城点数购买物品
#define BUYOBJECT_MALL_USERCMD_PARAMETER 33
struct stBuyObjectMallUserCmd:public stTradeUserCmd
{
	stBuyObjectMallUserCmd()
	{
		byParam = BUYOBJECT_MALL_USERCMD_PARAMETER;
		dwObjectID = 0;
		dwNum = 1;
		itemLevel = 0;
	}

	DWORD dwObjectID;
	DWORD dwNum;
	BYTE itemLevel;
};
#endif
//////////////////////////////////////////////////////////////
// 交易指令定义结束
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
// 帮会指令定义开始
//////////////////////////////////////////////////////////////
struct stUnionUserCmd : public stNullUserCmd
{
	stUnionUserCmd()
	{
		byCmd = UNION_USERCMD;
	}
};

/// 帮会创建前的用户状态检查消息
const BYTE UNION_STATUS_CHECK_PARA = 1;
struct stUnionStatusCheckCmd  : public stUnionUserCmd
{
	stUnionStatusCheckCmd()
	{
		byParam = UNION_STATUS_CHECK_PARA;

	}
};

// 帮会创建命令
const BYTE CREATE_UNION_PARA = 2;
struct stCreateUnionCmd: public stUnionUserCmd
{
	stCreateUnionCmd()
	{
		byParam = CREATE_UNION_PARA;
	}
	char UnionName[MAX_NAMESIZE];  // 帮会名称
};


const BYTE SERVER_RETURN_UNION_CREATE_FAILED = 3;
struct stServerReturnUnionCheckCmd : public stUnionUserCmd
{
	stServerReturnUnionCheckCmd()
	{
		byParam = SERVER_RETURN_UNION_CREATE_FAILED;
	}
};


struct stUnionRember{
	char  memberName[MAX_NAMESIZE];                 // 帮会成员名称
	char  aliasname[MAX_NAMESIZE];                  // 成员别名[职务名称]
	WORD  level;                                    // 成员等级
	BYTE  byOnline;                                 // 成员在线标志 0 为不在线 非0为在线
	WORD  occupation;                               // 职业类型
	DWORD exploit;                                  // 功勋值
	char  septName[MAX_NAMESIZE];                   // 所属家族
	BYTE  bySeptMaster;                             // 0不是族长，1是族长
};

// 服务器返回帮会成员列表，及请求者的个人帮会信息
const BYTE ALL_UNION_MEMBER_PARA = 5;
struct stAllUnionMemberCmd : public stUnionUserCmd
{
	stAllUnionMemberCmd()
	{
		byParam = ALL_UNION_MEMBER_PARA;
	}
	WORD size;                     // 数量
	stUnionRember memberList[0];   // 成员列表
};


// 帮会的权限定义
enum UNIONPOWER {
	CHANGE_ALIAS,              // 改变称号
	ADD_MEMBER,                // 招收成员
	FIRE_MEMBER,               // 开除成员
	GENERAL,                   // 普通会员
	MASTER,                    // 帮主权限
	SECOND,                    // 副帮主权限
	DARE,                      // 发起或接受帮会战
	NOTIFY,                    // 发公告
};


// 返回帮会成员的个人信息
const BYTE RETURN_UNION_MEMBER_INFO_PARA = 6;
struct stReturnUnionMemberInfoCmd : public stUnionUserCmd
{
	stReturnUnionMemberInfoCmd()
	{
		byParam = RETURN_UNION_MEMBER_INFO_PARA;
	}

	char alias[MAX_NAMESIZE];      // 消息接收者的别名
	BYTE byPower[2];                    // 消息接收者的权限信息
};

enum UNION_ADD_MEMBER{
			QUESTION,          // 发出邀请某人的询问
			ANSWER_YES,        // 确认邀请
			ANSWER_NO          // 否定邀请
};

// 邀请他人加入帮会
const BYTE ADD_MEMBER_TO_UNION_PARA = 7;  // 这个消息由客户端发到服务端是邀请某人，由服务端发到客户端是询问某人是否接受邀请
struct stAddMemberToUnionCmd : public stUnionUserCmd
{
	stAddMemberToUnionCmd()
	{
		byParam = ADD_MEMBER_TO_UNION_PARA;
	}
	char 	memberName[MAX_NAMESIZE];   // 被邀请的人
	DWORD memberID;                   // 被邀请人ID
	char 	unionName[MAX_NAMESIZE];    // 工会名称
	BYTE 	byState;                    // 参见 enum UNION_ADD_MEMBER
};

// 开除帮会成员
const BYTE FIRE_MEMBER_FROM_UNION_PARA = 8;
struct stFireMemberFromUnionCmd : public stUnionUserCmd
{
	stFireMemberFromUnionCmd()
	{
		byParam = FIRE_MEMBER_FROM_UNION_PARA ;
	}
	char memberName[MAX_NAMESIZE];
};

enum{
		 UNION_MEMBER_STATUS_ONLINE,			// 帮会成员上线
		 UNION_MEMBER_STATUS_OFFLINE,			// 帮会成员下线
		 UNION_MEMBER_STATUS_FIRE,				// 帮会成员被开除
		 UNION_MEMBER_STATUS_NEWMEMBER,			// 新增加帮会成员
		 UNION_MEMBER_STATUS_ALIASCHANGE		// 成员别名改变
};

// 服务端更新帮会成员状态及信息通知消息 状态见上表
const BYTE BROADCAST_MEMBER_INFO_PARA = 10;
struct stBroadcastMemberInfo : public stUnionUserCmd
{
	stBroadcastMemberInfo()
	{
		byParam = BROADCAST_MEMBER_INFO_PARA;
		level = 0;
	}
	BYTE byStatus;                                  // 成员状态
	char name[MAX_NAMESIZE];                        // 成员名称
	char aliasname[MAX_NAMESIZE];                   // 成员别名
	WORD wdOccupation;                              // 成员职业
	WORD level;                                     // 成员等级
	DWORD exploit;                                  // 功勋值
	char  septName[MAX_NAMESIZE];                   // 所属家族
	BYTE bySeptMaster;                              // 0,不是族长，1为族长
};

// 服务器端发送帮会信息
const BYTE UNION_BASE_INFO_PARA = 11;
struct stUnionBaseInfoCmd : public stUnionUserCmd
{
	stUnionBaseInfoCmd()
	{
		byParam = UNION_BASE_INFO_PARA;
	}
	char  unionName[MAX_NAMESIZE];         // 帮会名称
	char  master[MAX_NAMESIZE];                             // 帮会会长
	WORD  wdLevel;                       // 帮会级别
	QWORD qwExp;                         // 帮会经验
	char  note[255];                                                // 帮会介绍
	DWORD dwMana;                           // 帮会威望
	DWORD dwAction;                         // 行动力
};

// 帮会成员申请退会
const BYTE UNIONMEMBER_LEAVE_UNION_PARA = 12;
struct stUnionMemberLeaveUnionCmd : public stUnionUserCmd
{
	stUnionMemberLeaveUnionCmd()
	{
		byParam = UNIONMEMBER_LEAVE_UNION_PARA;
	}
};

struct stUnionVoteRecord{
	char unionName [MAX_NAMESIZE];
	char master[MAX_NAMESIZE];
	WORD wdVoteNumber;
	char note[255];
};

// 帮会投票列表
const BYTE VOTELIST_UNION_PARA = 13;
struct stVoteListUnionCmd : public stUnionUserCmd
{
	stVoteListUnionCmd()
	{
		byParam = VOTELIST_UNION_PARA;
	}
	char unionName[MAX_NAMESIZE];
	DWORD dwSize;
	BYTE flag; // 0为消息头1为后续消息
	stUnionVoteRecord data[0];
};

// 帮会投票消息
const BYTE VOTE_UNION_PARA = 14;
struct stVoteUnionCmd : public stUnionUserCmd
{
	stVoteUnionCmd()
	{
		byParam = VOTE_UNION_PARA;
	}
	char unionName[MAX_NAMESIZE];
	BYTE bySuccess;   // 为1为成功 为0为失败
};

// 帮会设置帮会说明
const BYTE NOTE_UNION_PARA = 15;
struct stNoteUnionCmd : public stUnionUserCmd
{
	stNoteUnionCmd()
	{
		byParam = NOTE_UNION_PARA;
	}
	char noteBuf[255];
};

// 帮会成员别名设置
const BYTE CHANGE_UNION_MEMBER_ALIASNAME_PARA = 16;
struct stChangeUnionMemberAliasName : public stUnionUserCmd
{
	stChangeUnionMemberAliasName()
	{       
		byParam = CHANGE_UNION_MEMBER_ALIASNAME_PARA;
	}

	char name[MAX_NAMESIZE];             // 帮众姓名
	char aliasname[MAX_NAMESIZE];        // 帮众别名
};


// 进入城主的帮会领地
const BYTE ENTER_UNION_CITY_AREA_PARA = 17;
struct stEnterUnionCityArea : public stUnionUserCmd
{
	stEnterUnionCityArea()
	{
		byParam = ENTER_UNION_CITY_AREA_PARA;
	}
};

enum
{
	QUESTION_UNION_MEMBER_POWER, // 请求发送权限
	RESPOND_UNION_MEMBER_POWER, //  返回某帮众权限
	SET_UNION_MEMBER_POWER,     //  设置某帮众权限
};
// 请求帮众权限
const BYTE UNION_MEMBER_POWER_PARA = 18;
struct stUnionMemberPower : public stUnionUserCmd
{
	stUnionMemberPower()
	{
		byParam = UNION_MEMBER_POWER_PARA;
	}

	char name[MAX_NAMESIZE]; // 帮众姓名
	BYTE byPower[2]; // 权限
	BYTE byType; // 消息类型
};


enum
{
	QUESTION_CITY_HOLD, // 查询城市所属
	QUESTION_CITY_DARE, // 当天城市争夺战
};

// 查询问城市帮会信息
const BYTE QUESTION_UNION_CITY_INFO_PARA = 19;
struct stQuestionUnionCityInfo : public stUnionUserCmd
{
	stQuestionUnionCityInfo()
	{
		byParam = QUESTION_UNION_CITY_INFO_PARA;
	}

	BYTE byType;    
};

enum
{
	COUNTRY_UNION_SORT,     // 本国帮会排行榜
	WORLD_UNION_SORT,       // 世界帮会排行榜
};

// 查询家族排行榜（根据声望）
const BYTE REQ_UNION_SORT_PARA = 20;
struct stReqUnionSort : public stUnionUserCmd
{       
	stReqUnionSort() 
	{
		byParam = REQ_UNION_SORT_PARA;
		byType = COUNTRY_UNION_SORT;
	}

	BYTE byType;
};




struct stUnionSortInfo
{
	char  unionName[MAX_NAMESIZE];         // 帮会名称
	DWORD dwMana;                           // 帮会威望
	DWORD dwOrder;                          // 帮会排名
	DWORD dwCountryID;                      // 国家ID
};      

// 返回帮会排行榜（根据威望）
const BYTE RTN_UNION_SORT_PARA = 21;
struct stRtnUnionSort : public stUnionUserCmd
{       
	stRtnUnionSort()
	{       
		byParam = RTN_UNION_SORT_PARA;
		dwSize = 0;
	}

	BYTE byType;
	DWORD dwSize;
	stUnionSortInfo data[0];
};

// 帮会资金捐献
const BYTE CONTRIBUTE_UNION_PARA = 22;
struct stContributeUnion : public stUnionUserCmd
{
	stContributeUnion()
	{
		byParam = CONTRIBUTE_UNION_PARA;
	}

	DWORD dwMoney;
};

// 帮会资金查询
const BYTE REQ_UNION_MONEY_PARA = 23;
struct stReqUnionMoney : public stUnionUserCmd
{
	stReqUnionMoney()
	{
		byParam = REQ_UNION_MONEY_PARA;
	}
};

struct stUnionRecord{
	char unionName [MAX_NAMESIZE];
	char master[MAX_NAMESIZE];
	char note[255];
};

// 帮会列表
const BYTE LIST_UNION_PARA = 24;
struct stListUnionCmd : public stUnionUserCmd
{
	stListUnionCmd()
	{
		byParam = LIST_UNION_PARA;
	}
	DWORD dwSize;
	BYTE flag; // 0为消息头1为后续消息
	stUnionRecord data[0];
};

// 请求加入帮会消息
const BYTE REQUEST_JOIN_UNION_PARA = 25;
struct stRequestJoinUnionCmd : public stUnionUserCmd
{
	stRequestJoinUnionCmd()
	{
		byParam = REQUEST_JOIN_UNION_PARA;
	}
	char name[MAX_NAMESIZE]; //发给服务器的时候填对方会长的名字，服务器发出来的时候填的是请求人的名字
	BYTE status;
};

// 放弃占领城市
const BYTE REQUEST_ABJURATION_CITY_PARA = 26;
struct stRequestAbjurationCitynCmd : public stUnionUserCmd
{
	stRequestAbjurationCitynCmd()
	{
		byParam = REQUEST_ABJURATION_CITY_PARA;
	}
};

//soke 2016 - 3 - 11 城主任命捕头（停用）
#define APPOINT_CITY_CATCHER_PARA  31
struct stAppointCityCatcherCmd : public stUnionUserCmd
{
	stAppointCityCatcherCmd()
	{
		byParam = APPOINT_CITY_CATCHER_PARA;
		bzero(name, sizeof(name));
	}

	char name[MAX_NAMESIZE];
};

//soke 2016 - 3 - 11 城主撤消捕头（停用）
#define CANCEL_CITY_CATCHER_PARA  32
struct stCancelCityCatcherCmd : public stUnionUserCmd 
{
	stCancelCityCatcherCmd()
	{
		byParam = CANCEL_CITY_CATCHER_PARA;
	}
};

//////////////////////////////////////////////////////////////
// 帮会指令定义结束
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
// 社会关系指令定义开始
//////////////////////////////////////////////////////////////
struct stRelationUserCmd : public stNullUserCmd
{
	stRelationUserCmd()
	{
		byCmd = RELATION_USERCMD;
	}
};

enum {
	RELATION_TYPE_LOVE,			// 夫妻关系
	RELATION_TYPE_TEACHER,		// 师徒关系
	RELATION_TYPE_FRIEND,		// 好友关系
	RELATION_TYPE_BAD,			// 黑名单
	RELATION_TYPE_OVER,			// 关系已经断绝，主要是用在数据库中传达离线通知，当人物上线后会根据状态向客户端发消息。
	RELATION_TYPE_ENEMY			// 仇人关系

};
struct stRelation
{
	char  name[MAX_NAMESIZE];       //      关系人名字
	BYTE  type;                                     //      关系类型 
	WORD  level;                            //  关系级别
	WORD  user_level;              // 关系人等级
	BYTE  online;             //  在线状态 0为不在线，1为在线
	WORD  occupation;                       // 职业类型
	DWORD exploit;          // 功勋值
	DWORD country;          // 国籍
	char  unionName[MAX_NAMESIZE];  // 所属帮会
};

// 发送社会关系列表
const BYTE RELATION_SEND_RELATIONLIST_PARA = 1;
struct stSendRelationListCmd : public stRelationUserCmd
{
	WORD size;
	stRelation member[0];
	stSendRelationListCmd()
	{
		byParam = RELATION_SEND_RELATIONLIST_PARA;
	}
};

enum RELATION{
	RELATION_ADD,				// 添加状态
	RELATION_QUESTION,         // 发出邀请某人的询问
	RELATION_ANSWER_YES,       // 确认邀请
	RELATION_ANSWER_NO,        // 否定邀请
	RELATION_ONLINE,			// 在线状态
	RELATION_OFFLINE,			// 离线状态
	RELATION_TYPECHANGE,		// 类型改变
	RELATION_REMOVE				// 删除状态
};



// 关系状态变更，以及邀请交互
const BYTE RELATION_STATUS_PARA =3;
struct stRelationStatusCmd : public stRelationUserCmd
{
	char    name[MAX_NAMESIZE];
	BYTE    type;
	WORD    occupation;
	WORD    user_level;

	union {
		WORD level;
		DWORD userid;
	};

	BYTE    byState;                        //      参见 enum RELATION
	DWORD   exploit;                        // 功勋值
	DWORD   country;

	char  unionName[MAX_NAMESIZE];  // 所属帮会

	stRelationStatusCmd()
	{
		byParam = RELATION_STATUS_PARA;
		user_level = 0;
		exploit = 0;
		country = 0;
		bzero(unionName, MAX_NAMESIZE);
	}
};


enum{
	MARRY_AHEAD_CHECK,
	MARRY_AHEAD_CHECK_FAIL,
	MARRY_ITEM_CHECK,
	MARRY_NO_ITEM,
	MARRY_ANSWER,
	MARRY_REFUSE,
	MARRY_AGREE,
	MARRY_PAY_MONEY,
	MARRY_PAY_MONEY1,
	MARRY_PAY_MONEY2,
	MARRY_NO_MONEY
};


/// 结婚前的用户状态检查消息
const BYTE MARRY_STATUS_CHECK_PARA = 4;
struct stMarryStatusCheckCmd  : public stRelationUserCmd
{
	stMarryStatusCheckCmd()
	{
		byParam = MARRY_STATUS_CHECK_PARA;

	}
	BYTE byStep;
};


/// 离婚消息
#define UNMARRY_PARA 5
struct stUnmarryCmd  : public stRelationUserCmd
{
	stUnmarryCmd()
	{
		byParam = UNMARRY_PARA;

	}
};
//////////////////////////////////////////////////////////////
// 社会关系指令定义结束
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// 师徒门派关系指令定义开始
//////////////////////////////////////////////////////////////
struct stSchoolUserCmd : public stNullUserCmd
{
	stSchoolUserCmd()
	{
		byCmd = SCHOOL_USERCMD;
	}
};

enum SCHOOL_ADD_MEMBER{
			TEACHER_QUESTION,         // 发出邀请某人加入师门的询问
			TEACHER_ANSWER_YES,       // 确认邀请加入师门
			TEACHER_ANSWER_NO         // 否定邀请加入师门
};

// 邀请他人加入门派，兼任新人加入通知消息
const BYTE ADD_MEMBER_TO_SCHOOL_PARA = 1;  // 这个消息由客户端发到服务端是邀请某人，由服务端发到客户端是询问某人是否接受邀请
struct stAddMemberToSchoolCmd : public stSchoolUserCmd
{
	stAddMemberToSchoolCmd()
	{
		byParam = ADD_MEMBER_TO_SCHOOL_PARA;
	}
	char 	memberName[MAX_NAMESIZE];			// 被邀请的人
	union {
			DWORD	memberID;					// 被邀请人ID
			DWORD	degree;						// 向客户端发送的时候告诉客户端新成员处于那一个层级之中
	};
	union {
			char 	schoolName[MAX_NAMESIZE];	// 门派名称，如果没有门派为空
			char    prename[MAX_NAMESIZE];		// 他的师兄
	};
	BYTE 	byState;							// 参见 enum SCHOOL_ADD_MEMBER
	WORD	wdOccupation;						// 新成员职业
	WORD    level;
};

// 将指定的人开除出师门或者门派
const BYTE FIRE_MEMBER_FROM_SCHOOL_PARA = 2;
struct stFireMemberFromSchoolCmd : public stSchoolUserCmd
{
	stFireMemberFromSchoolCmd()
	{
		byParam = FIRE_MEMBER_FROM_SCHOOL_PARA ;
	}
	char memberName[MAX_NAMESIZE];
};

/// 门派创建前的用户状态检查消息
const BYTE SCHOOL_STATUS_CHECK_PARA = 3;
struct stSchoolStatusCheckCmd  : public stSchoolUserCmd
{
	stSchoolStatusCheckCmd()
	{
		byParam = SCHOOL_STATUS_CHECK_PARA;
	}
};

// 门派创建命令
const BYTE CREATE_SCHOOL_PARA = 4;
struct stCreateSchoolCmd: public stSchoolUserCmd
{
	stCreateSchoolCmd()
	{
		byParam = CREATE_SCHOOL_PARA;
	}
	char SchoolName[MAX_NAMESIZE];
};

// 门派或师门成员申请脱离关系
const BYTE SCHOOLMEMBER_LEAVE_SCHOOL_PARA = 5;
struct stSchoolMemberLeaveSchoolCmd : public stSchoolUserCmd
{
	stSchoolMemberLeaveSchoolCmd()
	{
		byParam = SCHOOLMEMBER_LEAVE_SCHOOL_PARA;
	}
};

enum SCHOOL_PACKAGE_TAG{
	SCHOOL_PACKAGE_BODY=0,	// 消息包体
	SCHOOL_PACKAGE_HEAD=1,	// 消息包头
	SCHOOL_PACKAGE_TAIL=2	// 消息包尾
};

enum SCHOOL_LAYER {
	SCHOOL_NEWLAYER,		// 换一新层
	SCHOOL_NEWLINE,        // 换一新行
	SCHOOL_NONE				// 无动作
};

struct stSchoolMember{
	char name[MAX_NAMESIZE];
	BYTE tag;               // enum SCHOOL_LAYER
	WORD level;             // 成员等级
	BYTE online;    // 0 为再不在线
	WORD occupation;// 职业类型
};

struct stTeacherMember{
	char name[MAX_NAMESIZE];
	WORD degree;   // 友好度
	BYTE tag;       // enum SCHOOL_MEMBER_TAG
	WORD level;     // 成员等级
	BYTE online;    // 0 为再不在线
	WORD occupation;// 职业类型
	DWORD country;          // 国籍
	char  unionName[MAX_NAMESIZE];  // 所属帮会
	char  septName[MAX_NAMESIZE];  // 所属家族
};  

// 门派信息列表
const BYTE SENDLIST_SCHOOL_PARA = 6;
struct stSendListSchoolCmd : public stSchoolUserCmd
{
	stSendListSchoolCmd()
	{
		byParam = SENDLIST_SCHOOL_PARA;
	}
	DWORD dwMasterTempID;
	BYTE byPackageTag; //enum SCHOOL_PACKAGE_TAG
	WORD size;
	stSchoolMember list[0];
};

enum SCHOOL_MEMBER_TAG{
	TEACHER,				// 师傅
	BIGBROTHER,				// 师兄
	LITTLEBROTHER,			// 师弟
	PRENTICE				// 徒弟
	
};

// 师门信息列表
const BYTE SENDMEMBERINFO_SCHOOL_PARA = 7;
struct stSendMemberInfoCmd : public stSchoolUserCmd
{
	stSendMemberInfoCmd()
	{
		byParam = SENDMEMBERINFO_SCHOOL_PARA;
	}
	WORD size; //大小
	stTeacherMember list[0]; // list里成员的tag使用 enum SCHOOL_MEMBER_TAG
};

enum {
	SCHOOL_STATUS_NONE,
	SCHOOL_STATUS_TEACHER,
	SCHOOL_STATUS_SCHOOL
};

// 初始化通知
const BYTE MEMBER_STATUS_SCHOOL_PARA = 8;
struct stMemberStatusSchool : public stSchoolUserCmd
{
	stMemberStatusSchool()
	{
		byParam = MEMBER_STATUS_SCHOOL_PARA;
	}
	char schoolName[MAX_NAMESIZE]; // 门派名字
	BYTE byStatus; //	SCHOOL_STATUS_NONE,	SCHOOL_STATUS_TEACHER,	SCHOOL_STATUS_SCHOOL
};

enum {
	SCHOOL_MEMBER_OFFLINE=0,		// 离线状态
	SCHOOL_MEMBER_ONLINE=1			// 在线状态
};

const BYTE SCHOOL_MEMBER_STATUS_PARA =9;
struct stSchoolMemberStatusCmd : public stSchoolUserCmd
{
	char	name[MAX_NAMESIZE];
	BYTE	byState;						//	参见 SCHOOL_MEMBER_ONLINE SCHOOL_MEMBER_OFFLINE
	WORD    level;							//  成员等级
	DWORD	country;						//  国籍
	char	unionName[MAX_NAMESIZE];		//  所属帮会
	char	septName[MAX_NAMESIZE];			//  所属家族
	stSchoolMemberStatusCmd()
	{
		byParam = SCHOOL_MEMBER_STATUS_PARA;
	}
};


#define BULLETIN_MAX_SIZE 1000

enum {
	SCHOOL_BULLETIN_SET,		// 设置公告
	SCHOOL_BULLETIN_GET,		// 请求公告内容
	SCHOOL_BULLETIN_EDIT		// 编辑请求
};

const BYTE SCHOOL_BULLETIN__PARA =10;
struct stSchoolBulletinCmd : public stSchoolUserCmd
{
	BYTE byState; // SCHOOL_BULLETIN_SET SCHOOL_BULLETIN_GET
	WORD wdSize;  // 无内容设0
	char data[0];
	stSchoolBulletinCmd()
	{
		byParam = SCHOOL_BULLETIN__PARA;
	}
};

// 门派解散指令
const BYTE DESTROY_SCHOOL_PARA = 11;
struct stDestroySchoolCmd: public stSchoolUserCmd
{
	stDestroySchoolCmd()
	{
		byParam = DESTROY_SCHOOL_PARA;
	}
};

const BYTE SCHOOL_BULLETIN_NOTIFY_PARA = 12;
struct stSchoolBulletinNotifyCmd : public stSchoolUserCmd
{
	stSchoolBulletinNotifyCmd()
	{
		byParam = SCHOOL_BULLETIN_NOTIFY_PARA;
	}
};

//
enum
{
	QUERY_SCHOOL_BOUNTY, // 查询 
	GET_SCHOOL_BOUNTY,   // 提取 
	PUT_SCHOOL_BOUNTY,  // 进贡 
};

// 做为师傅查询或领取进贡
#define REQ_MASTER_BOUNTY_PARA 13
struct stReqMasterBountyUserCmd : public stSchoolUserCmd
{
	stReqMasterBountyUserCmd()
	{
		byParam = REQ_MASTER_BOUNTY_PARA;
	}

	BYTE byState;
};

// 做为徒弟查询和进行进贡
#define REQ_PRENTICE_BOUNTY_PARA 14
struct stReqPrenticeBountyUserCmd : public stSchoolUserCmd
{
	stReqPrenticeBountyUserCmd()
	{
		byParam = REQ_PRENTICE_BOUNTY_PARA;
	}

	BYTE byState;
};

// 返回师傅查询
#define RTN_MASTER_BOUNTY_PARA 15
struct stRtnMasterBountyUserCmd : public stSchoolUserCmd
{
	stRtnMasterBountyUserCmd()
	{
		byParam = RTN_MASTER_BOUNTY_PARA;
		dwBalance = 0;
		dwTotal = 0;
	}

	DWORD dwBalance; // 可领取的金额
	DWORD dwTotal;  // 累计已领取的金额
};

// 返回徒弟查询
#define RTN_PRENTICE_BOUNTY_PARA 16
struct stRtnPrenticeBountyUserCmd : public stSchoolUserCmd
{
	stRtnPrenticeBountyUserCmd()
	{
		byParam = RTN_PRENTICE_BOUNTY_PARA;
		dwBalance = 0;
		dwTotal = 0;
		dwLastLevel = 0;
		bzero(name, sizeof(name));
	}

	DWORD dwBalance; // 可进贡的金额
	DWORD dwTotal;   // 累计已领取的金额
	DWORD dwLastLevel; // 上次领取时的等级
	char name[MAX_NAMESIZE]; // 师傅的姓名
};

//////////////////////////////////////////////////////////////
// 师徒门派关系指令定义结束
//////////////////////////////////////////////////////////////

/************************************************************
************************任务指令定义开始*********************
*************************************************************/
struct stQuestUserCmd : public stNullUserCmd
{
	stQuestUserCmd()
	{
		byCmd = TASK_USERCMD;	
	}
	DWORD id; //任务id
};

static const BYTE QUEST_INFO_PARA = 1;
struct stQuestInfoUserCmd : public stQuestUserCmd
{
	stQuestInfoUserCmd()
	{
		byParam = QUEST_INFO_PARA;
	}

	BYTE name[64];
	DWORD start;
	BYTE info[0];
};

static const BYTE QUEST_VARS_PARA = 2;
struct stQuestVarsUserCmd : public stQuestUserCmd
{
	enum {  
		MAX_NSIZE = 32,
		MAX_VSIZE = 128,
	};

	stQuestVarsUserCmd()
	{
		byParam = QUEST_VARS_PARA;  
	}
	BYTE count; //变量数量
	struct Var {
		BYTE name[MAX_NSIZE];
		BYTE value[MAX_VSIZE];  
	} vars_list[0]; //变量列表
};


static const BYTE REQUEST_QUEST_PARA = 3;
struct stRequestQuestUserCmd : public stQuestUserCmd
{
	stRequestQuestUserCmd()
	{
		byParam = REQUEST_QUEST_PARA;
	}
	char target[16]; //目标
	BYTE offset; //任务分支
};


static const BYTE ABANDON_QUEST_PARA = 4;
struct stAbandonQuestUserCmd : public stQuestUserCmd
{
	stAbandonQuestUserCmd()
	{
		byParam = ABANDON_QUEST_PARA;
	}
};

static const BYTE CART_CONTROL_QUEST_PARA = 5;//控制镖车走和停
struct stCartControlQuestUserCmd : public stQuestUserCmd
{
	stCartControlQuestUserCmd()
	{
		byParam = CART_CONTROL_QUEST_PARA;
	}
};

static const BYTE CHECK_VALID_QUEST_PARA = 6;//检查可以接的任务列表
struct stCheckValidQuestUserCmd : public stQuestUserCmd
{
        stCheckValidQuestUserCmd()
        {
                byParam = CHECK_VALID_QUEST_PARA;
        }
};

//在TaskDlg里面检索任务
static const BYTE REQ_VALID_QUEST_PARA = 7;//检查可以接的任务列表
struct stReqValidQuestUserCmd : public stQuestUserCmd
{   
	stReqValidQuestUserCmd()
	{   
		byParam = REQ_VALID_QUEST_PARA;
	}   
};  

static const BYTE RET_VALID_QUEST_PARA = 8;//返回可以接的任务列表
struct stRetValidQuestUserCmd : public stQuestUserCmd
{
	stRetValidQuestUserCmd()
	{
		byParam = RET_VALID_QUEST_PARA;
		size = 0;
	}
	DWORD size;
	char content[0];
};

/************************************************************
************************任务指令定义结束*********************
*************************************************************/


//////////////////////////////////////////////////////////////
// 家族指令定义开始
//////////////////////////////////////////////////////////////
struct stSeptUserCmd : public stNullUserCmd
{
	stSeptUserCmd()
	{
		byCmd = SEPT_USERCMD;
	}
};

/// 家族创建前的用户状态检查消息
const BYTE SEPT_STATUS_CHECK_PARA = 1;
struct stSeptStatusCheckCmd  : public stSeptUserCmd
{
	stSeptStatusCheckCmd()
	{
		byParam = SEPT_STATUS_CHECK_PARA;

	}
};

// 家族创建命令
const BYTE CREATE_SEPT_PARA = 2;
struct stCreateSeptCmd: public stSeptUserCmd
{
	stCreateSeptCmd()
	{
		byParam = CREATE_SEPT_PARA;
	}
	char SeptName[MAX_NAMESIZE];  // 家族名称
};

// 家族创建失败通知指令
const BYTE SERVER_RETURN_SEPT_CREATE_FAILED = 3;
struct stServerReturnSeptCheckCmd : public stSeptUserCmd
{
	stServerReturnSeptCheckCmd()
	{
		byParam = SERVER_RETURN_SEPT_CREATE_FAILED;
	}
};

struct stSeptRember{
	char  memberName[MAX_NAMESIZE];                         // 家族成员名称
	WORD  level;                                            // 成员等级
	BYTE  byOnline;                                         // 成员在线标志 0 为不在线 非0为在线
	WORD  occupation;                                       // 职业类型
	DWORD exploit;                                          // 功勋值
	DWORD grace;                                            // 文采值
	char memberAliasName[MAX_NAMESIZE];                     // 族员别名
};  

// 服务器返回家族成员列表，及请求者的个人家族信息
const BYTE ALL_SEPT_MEMBER_PARA = 5;
struct stAllSeptMemberCmd : public stSeptUserCmd
{
	stAllSeptMemberCmd()
	{
		byParam = ALL_SEPT_MEMBER_PARA;
	}
	WORD size;                     // 数量
	stSeptRember memberList[0];   // 成员列表
};


enum SEPT_ADD_MEMBER{
			SEPT_QUESTION,          // 发出邀请某人的询问
			SEPT_ANSWER_YES,        // 确认邀请
			SEPT_ANSWER_NO          // 否定邀请
};

// 邀请他人加入家族
const BYTE ADD_MEMBER_TO_SEPT_PARA = 7;  // 这个消息由客户端发到服务端是邀请某人，由服务端发到客户端是询问某人是否接受邀请
struct stAddMemberToSeptCmd : public stSeptUserCmd
{
	stAddMemberToSeptCmd()
	{
		byParam = ADD_MEMBER_TO_SEPT_PARA;
	}
	char 	memberName[MAX_NAMESIZE];   // 被邀请的人
	DWORD memberID;                   // 被邀请人ID
	char 	septName[MAX_NAMESIZE];    // 家族名称
	BYTE 	byState;                    // 参见 enum SEPT_ADD_MEMBER
};

// 开除家族成员
const BYTE FIRE_MEMBER_FROM_SEPT_PARA = 8;
struct stFireMemberFromSeptCmd : public stSeptUserCmd
{
	stFireMemberFromSeptCmd()
	{
		byParam = FIRE_MEMBER_FROM_SEPT_PARA ;
	}
	char memberName[MAX_NAMESIZE];
};

enum{
	SEPT_MEMBER_STATUS_ONLINE,          // 家族成员上线
	SEPT_MEMBER_STATUS_OFFLINE,         // 家族成员下线
	SEPT_MEMBER_STATUS_FIRE,            // 家族成员被开除
	SEPT_MEMBER_STATUS_NEWMEMBER,       // 新增加家族成员
	SEPT_MEMBER_STATUS_ALIASCHANGE, // 更改别名
};

// 服务端更新家族成员状态及信息通知消息 状态见上表
const BYTE BROADCAST_SEPT_MEMBER_INFO_PARA = 10;
struct stBroadcastSeptMemberInfo : public stSeptUserCmd
{
	stBroadcastSeptMemberInfo()
	{
		byParam = BROADCAST_SEPT_MEMBER_INFO_PARA;
		level = 0;
	}
	BYTE byStatus;                       // 成员状态
	WORD wdOccupation;                   // 成员职业
	WORD level;                          // 成员等级
	char name[MAX_NAMESIZE];             // 成员名称
	char aliasName[MAX_NAMESIZE];        // 成员别名
	DWORD exploit;                       // 功勋值
	DWORD grace;                         // 文采值
};

// 服务器端发送家族信息
const BYTE SEPT_BASE_INFO_PARA = 11;
struct stSeptBaseInfoCmd : public stSeptUserCmd
{
	stSeptBaseInfoCmd()
	{
		byParam = SEPT_BASE_INFO_PARA;
	}       
	DWORD dwMasterTempID;
	char  septName[MAX_NAMESIZE];         // 家族名称
	char  master[MAX_NAMESIZE];            // 家族族长
	char  note[255];
	DWORD dwRepute;                 // 家族声望
	DWORD dwLevel;                  // 家族等级
};

// 家族成员申请退族
const BYTE SEPTMEMBER_LEAVE_SEPT_PARA = 12;
struct stSeptMemberLeaveSeptCmd : public stSeptUserCmd
{
	stSeptMemberLeaveSeptCmd()
	{
		byParam = SEPTMEMBER_LEAVE_SEPT_PARA;
	}
};

struct stSeptVoteRecord{
	char septName [MAX_NAMESIZE];
	char master[MAX_NAMESIZE];
	WORD wdVoteNumber;
	char note[255];
};


// 家族投票列表
const BYTE VOTELIST_SEPT_PARA = 13;
struct stVoteListSeptCmd : public stSeptUserCmd
{
	stVoteListSeptCmd()
	{
		byParam = VOTELIST_SEPT_PARA;
	}
	char septName[MAX_NAMESIZE];
	DWORD dwSize;
	BYTE flag; // 0为消息头 1为后续消息
	stSeptVoteRecord data[0];
};


// 家族投票消息
const BYTE VOTE_SEPT_PARA = 14;
struct stVoteSeptCmd : public stSeptUserCmd
{
	stVoteSeptCmd()
	{
		byParam = VOTE_SEPT_PARA;
	}
	char septName[MAX_NAMESIZE];
	BYTE bySuccess;   // 为1为成功 为0为失败
};

// 家族设置帮会说明
const BYTE NOTE_SEPT_PARA = 15;
struct stNoteSeptCmd : public stSeptUserCmd
{
	stNoteSeptCmd()
	{
		byParam = NOTE_SEPT_PARA;
	}
	char noteBuf[255];
};

// 家族成员别名设置
const BYTE CHANGE_SEPT_MEMBER_ALIASNAME_PARA = 16;
struct stChangeSeptMemberAliasName : public stSeptUserCmd
{
	stChangeSeptMemberAliasName()
	{
		byParam = CHANGE_SEPT_MEMBER_ALIASNAME_PARA;
	}

	char name[MAX_NAMESIZE];             // 族员姓名
	char aliasname[MAX_NAMESIZE];        // 族员别名
};

// 家族成员捐献荣誉点
const BYTE DONATE_HONOR_PARA = 17;
struct stDonateHonor : public stSeptUserCmd
{       
	stDonateHonor()
	{
		byParam = DONATE_HONOR_PARA;
		dwHonor = 0;
	}

	DWORD dwHonor;
};


enum
{
	COUNTRY_SEPT_SORT,      // 本国家族排行榜
	WORLD_SEPT_SORT,        // 世界家族排行榜
};


// 查询家族排行榜（根据声望）
const BYTE REQ_SEPT_SORT_PARA = 18;
struct stReqSeptSort : public stSeptUserCmd
{
	stReqSeptSort()
	{
		byParam = REQ_SEPT_SORT_PARA;
		byType = COUNTRY_SEPT_SORT;
	}

	BYTE byType;
};

struct stSeptSortInfo
{
	char  septName[MAX_NAMESIZE];           // 家族名称
	DWORD dwRepute;                         // 家族声望
	DWORD dwOrder;                          // 家族排名
	DWORD dwCountryID;                      // 国家ID
};
// 返回家族排行榜（根据声望）
const BYTE RTN_SEPT_SORT_PARA = 19;
struct stRtnSeptSort : public stSeptUserCmd
{
	stRtnSeptSort()
	{
		byParam = RTN_SEPT_SORT_PARA;
		dwSize = 0;
	}

	BYTE byType;
	DWORD dwSize;
	stSeptSortInfo data[0];
};

struct stSeptRecord{
	char septName [MAX_NAMESIZE];
	char master[MAX_NAMESIZE];
	char note[255];
};

// 家族投票列表
const BYTE LIST_SEPT_PARA = 20;
struct stListSeptCmd : public stSeptUserCmd
{
	stListSeptCmd()
	{
		byParam = LIST_SEPT_PARA;
	}
	DWORD dwSize;
	BYTE flag; // 0为消息头 1为后续消息
	stSeptRecord data[0];
};

enum {
	REQUEST_JOIN_OK,
	REQUEST_JOIN_CANCEL,
	REQUEST_JOIN_TIMEOUT
};

// 请求加入家族消息
const BYTE REQUEST_JOIN_SEPT_PARA = 21;
struct stRequestJoinSeptCmd : public stSeptUserCmd
{
	stRequestJoinSeptCmd()
	{
		byParam = REQUEST_JOIN_SEPT_PARA;
	}
	char name[MAX_NAMESIZE]; //发给服务器的时候填对方族长的名字，服务器发出来的时候填的是请求人的名字
	BYTE status;
};

// 请求家族占领NPC信息
const BYTE REQUEST_NPCINFO_SEPT_PARA = 22;
struct stRequestNpcInfoSeptCmd : public stSeptUserCmd
{
	stRequestNpcInfoSeptCmd()
	{
		byParam = REQUEST_NPCINFO_SEPT_PARA;
	}
};

// 请求领取家族经验
const BYTE REQUEST_SEPT_EXP_PARA = 23;
struct stRequestSeptExpCmd : public stSeptUserCmd
{
	stRequestSeptExpCmd()
	{
		byParam = REQUEST_SEPT_EXP_PARA;
	}
};

// 请求放弃占领该NPC
const BYTE REQUEST_ABJURATION_SEPTNPC_PARA = 24;
struct stRequestAbjurationSeptNpcSeptCmd : public stSeptUserCmd
{
	stRequestAbjurationSeptNpcSeptCmd()
	{
		byParam = REQUEST_ABJURATION_SEPTNPC_PARA;
	}
};

// 请求家族名称
const BYTE REQ_SEPT_NAME_PARA = 25;
struct stReqSeptNameCmd : public stSeptUserCmd
{
	stReqSeptNameCmd()
	{
		byParam = REQ_SEPT_NAME_PARA;
		dwSeptID = 0;
	}

	DWORD dwSeptID;                                                 
};

// 请求家族名称
const BYTE RTN_SEPT_NAME_PARA = 26;
struct stRtnSeptNameCmd : public stSeptUserCmd  
{       
	stRtnSeptNameCmd()                      
	{
		byParam = RTN_SEPT_NAME_PARA;
		dwSeptID = 0;
		bzero(name, MAX_NAMESIZE);
	}

	DWORD dwSeptID;
	char name[MAX_NAMESIZE];
};

// 请求领取家族经验
const BYTE REQUEST_SEPT_NORMAL_EXP_PARA = 29;
struct stRequestSeptNormalExpCmd : public stSeptUserCmd
{
	stRequestSeptNormalExpCmd()
	{
		byParam = REQUEST_SEPT_NORMAL_EXP_PARA;
	}
};

//////////////////////////////////////////////////////////////
// 家族指令定义结束
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// 战斗开始
//////////////////////////////////////////////////////////////////////////

// 对战类型定义
enum
{
	UNION_DARE      = 0,    // 帮会对战 
	SCHOOL_DARE     = 1,    // 师门对战
	SEPT_DARE       = 2,    // 家族对战
	SEPT_NPC_DARE   = 3,    // 家族NPC争夺战
	UNION_CITY_DARE = 4,    // 帮会夺城战
	COUNTRY_FORMAL_DARE = 5, // 正式国战
};

struct stDareUserCmd : public stNullUserCmd
{
	stDareUserCmd()
	{
		byCmd = DARE_USERCMD;
	}
};

enum
{
	DARE_QUESTION,
	DARE_YES,
	DARE_NO
};

enum
{
	DARE_REPUTE_ONE = 1,
	DARE_REPUTE_TWO = 2,
	DARE_REPUTE_THREE = 3,
	DARE_REPUTE_FOUR = 4
};

// 发起挑战
const BYTE ACTIVE_DARE_PARA = 1;
struct stActiveDareCmd : public stDareUserCmd
{
	stActiveDareCmd()
	{
		byParam = ACTIVE_DARE_PARA;
		dwWarID = 0;
		dwMsgType = DARE_QUESTION;
		byDareRepute = 0;
	}

	char name[MAX_NAMESIZE]; // 挑战者姓名
	char fromRelationName[MAX_NAMESIZE]; // 挑战者帮会或师门或家族名称
	char toRelationName[MAX_NAMESIZE];  //被挑战者帮会或师门或家族名称
	DWORD dwWarID;    // 交战ID
	DWORD dwDareType; // 挑战类型: 帮会、师门、家族
	DWORD dwMsgType;  // 挑战者发起挑战者无意义。服务器收到该命令后，置为DARE_QUESTION，转发给有权限的相应玩家。
	// 客户端对玩家提问后，返回DARE_YES或DARE_NO.结构其它信息保留。
	BYTE byDareRepute; // 家族随意战时，下多少声望做为赌资类型，类型有四种:1,2,3,4 分别对应1、5、10、20声望
};

enum
{
	NOTIFY_NO_EXIST, // 帮会或现门或家族名不存在
};

// 通知消息
const BYTE NOTIFY_DARE_PARA = 2;
struct stNotifyDare : public stDareUserCmd
{
	stNotifyDare()
	{
		byParam = NOTIFY_DARE_PARA;
	}

	DWORD dwType;       // 战斗类型
	DWORD dwNotify;     // 通知类型
};


// 请求挑战列表
const BYTE QUERY_DARE_LIST_PARA = 4;
struct stQueryDareList : public stDareUserCmd
{
	stQueryDareList()
	{
		byParam = QUERY_DARE_LIST_PARA;
	}

	BYTE byType; // UNION_DARE,SEPT_DARE
};

struct stDareList
{           
	char name[MAX_NAMESIZE];
};

// 通知可以挑战的社会关系列表
const BYTE SEND_DARE_LIST_PARA = 5;
struct stSendDareList : public stDareUserCmd
{
	stSendDareList()
	{
		byParam = SEND_DARE_LIST_PARA;
	}

	DWORD dwSize;
	BYTE  byType;
	struct stDareList dare_list[0];
};


// 发起帮会夺城挑战
const BYTE ACTIVE_UNION_CITY_DARE_PARA = 6;
struct stActiveUnionCityDare : public stDareUserCmd
{
	stActiveUnionCityDare()
	{
		byParam = ACTIVE_UNION_CITY_DARE_PARA;
		toCountryID = 0;
	}

	DWORD toCountryID;
};

// 领取当天税收
const BYTE GET_UNION_CITY_TAX_PARA = 7;
struct stGetUnionCityTax : public stDareUserCmd
{
	stGetUnionCityTax()
	{
		byParam = GET_UNION_CITY_TAX_PARA;
	}
};

// 通知客户端进入皇城争夺战区域
const BYTE ENTER_EMPEROR_DARE_ZONE = 8;
struct stEnterEmperorDareZone : public stDareUserCmd
{
	stEnterEmperorDareZone()
	{
		byParam = ENTER_EMPEROR_DARE_ZONE;
		dwDefCountryID = 0;
		state = 0;
	}

	DWORD dwDefCountryID; // 守方国家ID
	BYTE state; // 0,为退出,1,为进入
};

// 用户上线,通知客户端,正在进行皇城争夺战
const BYTE ENTER_EMPEROR_DARE_ACTIVE = 9;
struct stEnterEmperorDareActive : public stDareUserCmd
{
	stEnterEmperorDareActive()
	{
		byParam = ENTER_EMPEROR_DARE_ACTIVE;
	}
};

enum enmCallDutyType
{
	CALL_DUTY_KING,		//国王
	CALL_DUTY_UNION,	//帮主
	CALL_DUTY_SEPT,		//族长
	CALL_DUTY_NUM
};
enum enmCallDutyPipeline
{
	PIPELINE_CALL = 0,	  //召唤
	PIPELINE_REFUSE,	  //拒绝
	PIPELINE_AGREE,		  //同意
	PIPELINE_NUM
};
//国王、帮主、族长拉人
const BYTE DARE_CALL_DUTY = 10;
struct stDareCallDuty : public stDareUserCmd
{
    stDareCallDuty()
    {
        leaderTempID=0;
        byParam = DARE_CALL_DUTY;
        byCallerType = CALL_DUTY_NUM;
        byPipeline = PIPELINE_NUM;
    }
    DWORD   leaderTempID;           //领导tempid
    BYTE    byCallerType;           //enmCallDutyType
    BYTE    byPipeline;             //enmCallDutyPipeline
    char    mapName[MAX_NAMESIZE];  //目标地图名称
    WORD    x;                      //坐标x 
    WORD    y;                      //坐标y 
};

//////////////////////////////////////////////////////////////////////////
// 战斗开始结束
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
// 宠物指令定义开始
//////////////////////////////////////////////////////////////
struct stPetUserCmd : public stNullUserCmd
{
	stPetUserCmd()
	{
		byCmd = PET_USERCMD;
	}
};

//宠物状态
enum petState
{
	PET_STATE_NORMAL                = 0x0001,       //普通
	PET_STATE_PUTUP         = 0x0002,       //收起  
	PET_STATE_DEAD          = 0x0100,       //死亡
	PET_STATE_CONSIGN       = 0x0200        //寄存
};      


//宠物AI模式
enum petAIMode
{
	PETAI_MOVE_FOLLOW       = 0x0001,       //跟随
	PETAI_MOVE_STAND        = 0x0002,       //站立不动
	PETAI_ATK_ACTIVE        = 0x0100,       //攻击所有敌人
	PETAI_ATK_PASSIVE       = 0x0200,       //攻击正在攻击主人的敌人
	PETAI_ATK_NOATK         = 0x0400        //不战斗
};
// 设置宠物的AI模式
const BYTE SETAI_PET_PARA = 1;
struct stSetAIPetCmd : public stPetUserCmd
{
	stSetAIPetCmd()
	{
		byParam = SETAI_PET_PARA;
	}
	petType type;
	petAIMode mode;
};
//宠物的数据
struct t_PetData
{ //soke 突破属性这里也要改
	DWORD id;               //npcid
	char name[MAX_NAMESIZE];//名字
	BYTE lv;                //等级
	DWORD hp;               //生命
	DWORD maxhp;            //最大生命
	WORD str;               //力量
	WORD intel;             //智力
	WORD agi;               //敏捷
	WORD men;               //精神
	WORD vit;               //体质
	DWORD exp;              //经验
	DWORD maxexp;           //最大经验
	DWORD atk;              //攻击力
	DWORD maxatk;           //最大攻击力
	DWORD matk;             //魔攻
	DWORD maxmatk;          //最大魔攻
	DWORD def;              //物防
	DWORD mdef;             //魔防
	WORD cri;               //爆击
	DWORD skills[4];        //技能
	WORD ai;                //AI
	DWORD maxhp_plus;       //附加最大生命
	WORD atk_plus;          //附加物理攻击力
	WORD maxatk_plus;       //附加最大物理攻击力
	WORD matk_plus;         //附加魔法攻击力
	WORD maxmatk_plus;      //附加最大魔法攻击力
	WORD pdef_plus;         //附加物防
	WORD mdef_plus;         //附加魔防
	petType type;           //类型
	petState state;         //状态
};
// 请求宠物的数据
const BYTE REQUESTDATA_PET_PARA = 2;
struct stRequestDataPetCmd : public stPetUserCmd
{
	stRequestDataPetCmd()
	{
		byParam = REQUESTDATA_PET_PARA;
	}
	petType type;
};

//返回宠物的数据
const BYTE REFRESHDATA_PET_PARA = 3;
struct stRefreshDataPetCmd : public stPetUserCmd
{
	stRefreshDataPetCmd()
	{
		byParam = REFRESHDATA_PET_PARA;
		bzero(&data, sizeof(data));
	}
	petType type;
	DWORD id;
	t_PetData data;
};

//改宠物名字
const BYTE CHANGENAME_PET_PARA = 4;
struct stChangeNamePetCmd : public stPetUserCmd
{
	stChangeNamePetCmd()
	{
		byParam = CHANGENAME_PET_PARA;
		bzero(name, MAX_NAMESIZE);
	}
	
	petType type;
	char name[MAX_NAMESIZE];
};

//增加宠物
const BYTE ADDPET_PET_PARA = 5;
struct stAddPetPetCmd : public stPetUserCmd
{
	stAddPetPetCmd()
	{
		byParam = ADDPET_PET_PARA;
	}
	petType type;
	DWORD id;
};

//删除宠物
const BYTE DELPET_PET_PARA = 6;
struct stDelPetPetCmd : public stPetUserCmd
{
	stDelPetPetCmd()
	{
		byParam = DELPET_PET_PARA;
	}
	petType type;
	DWORD id;
};

//删除技能
const BYTE DELSKILL_PET_PARA = 7;
struct stDelSkillPetCmd : public stPetUserCmd
{
	stDelSkillPetCmd()
	{
		byParam = DELSKILL_PET_PARA;
	}
	petType type;///宠物类型
	DWORD id;///技能id
};

//召唤马出来跟随
const BYTE CALLHORSE_PET_PARA = 8;
struct stCallHorsePetCmd : public stPetUserCmd
{
	stCallHorsePetCmd()
	{
		byParam = CALLHORSE_PET_PARA;
	}
};

//收起马匹
const BYTE PUTHORSE_PET_PARA = 9;
struct stPutHorsePetCmd : public stPetUserCmd
{
	stPutHorsePetCmd()
	{
		byParam = PUTHORSE_PET_PARA;
	}
};

//刷新马的数据
const BYTE HORSEDATA_PET_PARA = 10;
struct stHorseDataPetCmd : public stPetUserCmd
{
	stHorseDataPetCmd()
	{
		byParam = HORSEDATA_PET_PARA;
		type = PET_TYPE_RIDE;
	}
	petType type;
	DWORD id;
	t_HorseData data;
};

//强制宠物攻击目标
const BYTE SETTARGET_PET_PARA = 11;
struct stSetTargetPetCmd : public stPetUserCmd
{
	stSetTargetPetCmd()
	{
		byParam = SETTARGET_PET_PARA;
	}
	petType type;///宠物类型
	DWORD targetType;///目标类型
	DWORD id;///目标的临时ID
};

//同步宠物的血和经验
const BYTE HP_EXP_PET_PARA = 12;
struct stHpExpPetCmd : public stPetUserCmd
{
	stHpExpPetCmd()
	{
		byParam = HP_EXP_PET_PARA;
	}
	petType type;
	DWORD id;
	DWORD hp;
	DWORD exp;
};

//展示法宝
const BYTE CALLFABAO_PET_PARA = 13;
struct stCallFabaoPetCmd : public stPetUserCmd
{
	stCallFabaoPetCmd()
	{
		byParam = CALLFABAO_PET_PARA;
	}
	DWORD id;//法宝id
};

//收起法宝
const BYTE PUTFABAO_PET_PARA = 14;
struct stPutFabaoPetCmd : public stPetUserCmd
{
	stPutFabaoPetCmd()
	{
		byParam = PUTFABAO_PET_PARA;
	}
};

//////////////////////////////////////////////////////////////
// 宠物指令定义结束
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
/// 定义答题命令开始
//////////////////////////////////////////////////////////////
struct stQuizUserCmd : public stNullUserCmd
{
	stQuizUserCmd()
	{
		byCmd = QUIZ_USERCMD;
	}       
};      

enum
{
	QUIZ_QUESTION,  // 询问是否接受问答竞赛
	QUIZ_YES,       // 参与竞赛
	QUIZ_NO         // 拒绝竞赛
};

const BYTE QUERY_QUIZ_PARA = 1;
struct stQueryQuiz : public stQuizUserCmd
{
	stQueryQuiz()
	{
		byParam = QUERY_QUIZ_PARA;
		byMsgType = QUIZ_QUESTION;
		byQuizType = 0;
	}
	BYTE byMsgType;
	BYTE byQuizType; // 1是普通全国，2是活动
};    

enum    
{       
	CREATE_QUESTION, // 请求进行智力问答
	CREATE_YES,      // 请求成功
	CREATE_FAIL      // 银子不足，失败
};

const BYTE CREATE_QUIZ_PARA = 2;
struct stCreateQuiz : public stQuizUserCmd
{       
	stCreateQuiz()
	{
		byParam = CREATE_QUIZ_PARA;
		byType  = CREATE_QUESTION;
		dwMoney = 100;
	}

	DWORD dwMoney;  // 个人问答时，所需银子
	BYTE byType;
};      

const BYTE QUESTION_QUIZ_PARA = 3;
struct stQuestionQuiz : public stQuizUserCmd
{
	stQuestionQuiz()
	{
		byParam = QUESTION_QUIZ_PARA;
		dwSize = 7;
	}

	DWORD dwID;  // 题目序号
	BYTE  bySpareTime; // 剩余时间
	BYTE  byNextStartTime;
	BYTE  dwSize; // 题目条数
	DWORD dwTotal;  // 总人数

	char subject[0]; // 题目 答案一 答案二 答案三....，中间以0做为分隔
};

const BYTE ANSWER_QUIZ_PARA = 4;
struct stAnswerQuiz : public stQuizUserCmd
{
	stAnswerQuiz()
	{
		byParam = ANSWER_QUIZ_PARA;
		byLuck = 0;
	}

	DWORD dwID;      // 题目序号
	BYTE byType;     // 竞赛类型
	BYTE  byLuck;   // 是否使用幸运星,1使用，其它值是不使用
	DWORD dwScore;  // 得分

	DWORD answerID; // 答案
};

const BYTE ANSWER_RETURN_QUIZ_PARA = 5;
struct stAnswerReturnQuiz : public stQuizUserCmd
{       
	stAnswerReturnQuiz()
	{       
		byParam = ANSWER_RETURN_QUIZ_PARA;
	}

	int byAnswerStatus; // -1:放弃，0:回答正确, 1:回答错误
};

struct rank_elem
{
	char name[MAX_NAMESIZE];   // 玩家姓名
	char countryName[MAX_NAMESIZE]; // 国家名称
	int  score;     // 得分
	int  grace;	// 文采值
	int  rank; // 玩家排名
};

const BYTE QUIZ_POS_LIST_PARA = 6;
struct stQuizPosList : public stQuizUserCmd
{       
	stQuizPosList()
	{       
		byParam = QUIZ_POS_LIST_PARA;
		bzero(rank_list, sizeof(rank_list));
		dwSize = 0;
	}

	DWORD dwSize;
	rank_elem rank_list[21];
};

const BYTE QUIZ_CUR_SCORE_PARA = 7;
struct stQuizCurScore : public stQuizUserCmd
{
	stQuizCurScore()
	{
		byParam = QUIZ_CUR_SCORE_PARA;
		dwScore = 0;
		dwGrace = 0;
	}

	DWORD dwScore; // 本次答题累积分数
	DWORD dwGrace; // 本次答题累积文采
};

const BYTE QUIZ_EXIT_PARA = 8;
struct stQuizExit : public stQuizUserCmd
{
	stQuizExit()
	{
		byParam = QUIZ_EXIT_PARA;
	}
};

enum
{
	QUIZ_WORLD,
	QUIZ_PERSONAL
};

const BYTE QUIZ_PARAM_PARA = 9;
struct stQuizParam : public stQuizUserCmd
{
	stQuizParam()
	{
		byParam = QUIZ_PARAM_PARA;
		byReadQuestionTime = 10;
		byAnswerQuestionTime = 10;
		byCalcTime = 5;
		bySubjectNumber = 90;
	}

	BYTE byReadQuestionTime;        // 读题时间
	BYTE byAnswerQuestionTime;      // 答题时间
	BYTE byCalcTime;                // 计算答案时间
	BYTE bySubjectNumber;           // 总题数
	BYTE byType;                    // 竞赛类型
	BYTE byStartTime;               // 竞赛开始剩余时间
};
//////////////////////////////////////////////////////////////
/// 定义答题命令结束
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
/// 定义NPC争夺命令开始
//////////////////////////////////////////////////////////////
struct stNpcDareUserCmd : public stNullUserCmd
{
	stNpcDareUserCmd()
	{
		byCmd = NPCDARE_USERCMD;
	}
};

const BYTE NPCDARE_DARE_PARA = 1;
struct stDareNpcDare : public stNpcDareUserCmd
{
	stDareNpcDare() 
	{
		byParam = NPCDARE_DARE_PARA;
	}
	DWORD dwNpcID;
};

const BYTE NPCDARE_GETGOLD_PARA = 2;
struct stDareNpcGetGold : public stNpcDareUserCmd
{
	stDareNpcGetGold() 
	{
		byParam = NPCDARE_GETGOLD_PARA;
	}
	DWORD dwNpcID;
};

enum
{
	QUESTION_NPCDARE_HOLD, // 查询城市所属
	QUESTION_NPCDARE_DARE, // 当天城市争夺战
};

// 查询问NPC争夺战信息
const BYTE QUESTION_NPCDARE_INFO_PARA = 3;
struct stQuestionNpcDareInfo : public stNpcDareUserCmd
{
	stQuestionNpcDareInfo()
	{
		byParam = QUESTION_NPCDARE_INFO_PARA;
	}
	BYTE byType;	
	DWORD dwNpcID;
};
//////////////////////////////////////////////////////////////
/// 定义NPC争夺命令结束
//////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// 与GM工具交互的指令
//  定义与GM工具交互的命令开始
////////////////////////////////////////////////////////////
const BYTE GMTOOL_USERCMD  = 34;
struct stGmToolUserCmd : public stNullUserCmd
{
	stGmToolUserCmd()
	{
		byCmd = GMTOOL_USERCMD;
	}       
};      

const BYTE CHAT_GMTOOL_PARA = 1;
struct stChatGmTool : public stGmToolUserCmd
{
	char userName[MAX_NAMESIZE];
	DWORD countryID;
	DWORD sceneID;
	stChannelChatUserCmd chat_cmd;
	stChatGmTool()
	{
		byParam = CHAT_GMTOOL_PARA;
		countryID = 0;
		sceneID = 0;
	}
};

const BYTE MSG_GMTOOL_PARA = 2;
struct stMsgGmTool : public stGmToolUserCmd
{
	BYTE type;//定单类型
	char content[512];//主要内容
	BYTE contact;//联系方式
	char tele[64];//地址、电话
	char hisName[MAX_NAMESIZE];//被举报者
	char bugCountry[8];//bug发生国家
	char bugMap[16];//bug发生地图
	char bugPos[8];//bug发生位置
	char bugTime[64];//发生时间
	char progName[64];//疑似外挂名称
	BYTE behavior;//不良游戏现象
	stMsgGmTool()
	{
		 byParam = MSG_GMTOOL_PARA;

		type = 0;
		bzero(content, sizeof(content));
		contact = 0;
		bzero(tele, sizeof(tele));
		bzero(hisName, sizeof(hisName));
		bzero(bugCountry, sizeof(bugCountry));
		bzero(bugMap, sizeof(bugMap));
		bzero(bugPos, sizeof(bugPos));
		bzero(bugTime, sizeof(bugTime));
		behavior = 0;
	}
};
//////////////////////////////////////////////////////////////
/// 定义与GM工具交互的命令结束
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
// 国家指令定义开始
//////////////////////////////////////////////////////////////

struct stCountryUserCmd : public stNullUserCmd
{
	stCountryUserCmd()
	{
		byCmd = COUNTRY_USERCMD;
	}       
};      

// 变更国籍
const BYTE CHANGE_COUNTRY_PARA = 1;
struct stChangeCountryCmd : public stCountryUserCmd
{
	stChangeCountryCmd()
	{
		byParam = CHANGE_COUNTRY_PARA;
	}

	DWORD dwToCountryID; // 变更到国家的ID
};

// 请求战场列表
const BYTE REQUEST_DARE_COUNTRY_PARA =2;
struct stRequestDareCountryCmd : public stCountryUserCmd
{
	stRequestDareCountryCmd()
	{
		byParam = REQUEST_DARE_COUNTRY_PARA;
	}

	BYTE byType; // 0,请求战场列表，1，请求挑战列表
};

struct stCountryInfo
{
	DWORD dwID;     // 国家ID
	BYTE  byType;   // 0,参战国，1，第三方国
};

// 返回战场列表
const BYTE RETURN_DARE_COUNTRY_PARA = 3;
struct stReturnDareCountryCmd : public stCountryUserCmd
{
	stReturnDareCountryCmd()
	{
		byParam = RETURN_DARE_COUNTRY_PARA;
		dwSize = 0;
	}

	DWORD dwSize; // 战场个数
	BYTE  byType; // 0,战场列表，挑战列表

	stCountryInfo country_list[0]; //战场国家ID数组
};

// 请求传送到指定战场
const BYTE TRANS_DARE_COUNTRY_PARA = 4;
struct stTransDareCountryCmd : public stCountryUserCmd
{
	stTransDareCountryCmd()
	{
		byParam = TRANS_DARE_COUNTRY_PARA;
		dwCountryID = 0;
	}
	DWORD dwCountryID;
};

// 请求进行国战挑战
const BYTE DARE_COUNTRY_FORMAL_PARA = 5;
struct stDareCountryFormalCmd : public stCountryUserCmd
{       
	stDareCountryFormalCmd()
	{
		byParam = DARE_COUNTRY_FORMAL_PARA;
	}

	DWORD dwCountryID; // 要挑战的国家
};

enum    
{               
	DARE_RECORD_RESULT,     // 对战结果记录
	DARE_RECORD_STAT,       // 对战结果统计
	DARE_RECORD_PLAN,       // 当天对战安排
};      

// 请求记录
const BYTE REQUEST_DARE_RECORD_PARA = 6;
struct stRequestDareRecordCmd : public stCountryUserCmd
{
	stRequestDareRecordCmd()
	{
		byParam = REQUEST_DARE_RECORD_PARA;
	}

	BYTE byType;    // 查询类型
};

struct stDareResult
{
	DWORD  dareTime;      // 对战时间
	DWORD  attCountry;    // 挑战国ID
	DWORD  defCountry;    // 防御国ID
	BYTE   byResult;      // 0,挑战国胜(攻陷对方皇城)，1,挑战国败(时间到了，却未攻陷)，2,防御国反攻成功(攻陷挑战国皇城)
	char   attKingName[MAX_NAMESIZE];
	char   defKingName[MAX_NAMESIZE];
};

// 对战结果
const BYTE RETURN_DARE_RECORD_RESULT_PARA = 7;
struct stReturnDareRecordResultCmd : public stCountryUserCmd
{
	stReturnDareRecordResultCmd()
	{
		byParam = RETURN_DARE_RECORD_RESULT_PARA;
		dwSize = 0;
	}

	DWORD dwSize;
	stDareResult dare_result[0];
};

struct stDareStat
{
	DWORD dwCountry;        // 国家ID
	DWORD dwFormalWin;      // 正式国战胜利次数
	DWORD dwFormalFail;     // 正式国战失败次数
	DWORD dwAnnoyWin;       // 骚扰国战胜利次数
	DWORD dwAnnoyFail;      // 骚扰国战失败次数
};
// 对战统计
const BYTE RETURN_DARE_RECORD_STAT_PARA =8;
struct stReturnDareRecordStatCmd : public stCountryUserCmd
{
	stReturnDareRecordStatCmd()
	{
		byParam = RETURN_DARE_RECORD_STAT_PARA;
		dwSize = 0;
	}

	DWORD dwSize;
	stDareStat dare_stat[0];
};

struct stDarePlan
{
	DWORD planTime;
	DWORD attCountry;       // 挑战国
	DWORD defCountry;       // 防御国
};
// 对战计划
const BYTE RETURN_DARE_RECORD_PLAN_PARA =9;
struct stReturnDareRecordPlanCmd : public stCountryUserCmd
{
	stReturnDareRecordPlanCmd()
	{
		byParam = RETURN_DARE_RECORD_PLAN_PARA;
		dwSize = 0;
	}

	DWORD dwSize;
	stDarePlan dare_plan[0];
};

// 叛国（变为无国籍）
const BYTE CANCEL_COUNTRY_PARA = 10;
struct stCancelCountryCmd : public stCountryUserCmd
{
	stCancelCountryCmd()
	{
		byParam = CANCEL_COUNTRY_PARA;
	}
};

// 申请加入国籍
const BYTE APPLY_COUNTRY_PARA = 11;
struct stApplyCountryCmd : public stCountryUserCmd
{
	stApplyCountryCmd()
	{
		byParam = APPLY_COUNTRY_PARA;
	}

	DWORD dwToCountryID; // 申请加入的国籍
};

////////////////////////////////////////////////
/// 税率查询
const BYTE TAX_COUNTRY_PARA = 12;
struct stTaxCountryUserCmd:public stCountryUserCmd
{
	stTaxCountryUserCmd()
	{
		byParam = TAX_COUNTRY_PARA;
	}
	DWORD dwCountry;
	BYTE byTax; 
};

/// 国库查询
const BYTE FISK_COUNTRY_PARA = 13;
struct stFiskCountryUserCmd:public stCountryUserCmd
{
	stFiskCountryUserCmd()
	{
		byParam = FISK_COUNTRY_PARA;
		qwGold = 0;
		qwMaterial = 0; 
		qwStock = 0;
	}

	DWORD dwCountry;
	QWORD qwGold;
	QWORD qwMaterial; // 物资
	QWORD qwStock;    // 原料
};

/// 税率设置
const BYTE SETTAX_COUNTRY_PARA = 14;
struct stSetTaxCountryUserCmd:public stCountryUserCmd
{
	stSetTaxCountryUserCmd()
	{
		byParam = SETTAX_COUNTRY_PARA;
	}
	DWORD dwCountry;
	BYTE byTax; 
};

struct stCountryStar
{       
	DWORD dwCountry;
	DWORD dwStar;
};      

/// 设置国家星号
const BYTE UPDATE_COUNTRY_STAR = 15;
struct stUpdateCountryStarCmd : public stCountryUserCmd
{
	stUpdateCountryStarCmd()
	{
		byParam = UPDATE_COUNTRY_STAR;
		dwSize = 0;
	}
	DWORD dwSize;
	stCountryStar data[0];
};

/// 国家捐献
const BYTE CONTRIBUTE_COUNTRY_MATERIAL = 16;
struct stContributeCountryMaterialCmd : public stCountryUserCmd
{
	stContributeCountryMaterialCmd()
	{
		byParam = CONTRIBUTE_COUNTRY_MATERIAL;
		itemID = 0;
	}
	DWORD itemID;
};   

/// 国战状态设置
const BYTE SETCOUNTRY_DARE_PARA = 17;
struct stSetCountryDareUserCmd:public stCountryUserCmd
{
	stSetCountryDareUserCmd()
	{
		byParam = SETCOUNTRY_DARE_PARA;
		dwCountry = 0;
		byStatus = 0;
		byResult = 0;
	}
	DWORD dwCountry; // 敌对国家ID
	BYTE  byStatus; // 1为进入国战，0为退出国战
	BYTE  byResult; // 对战结果，1为胜方，0为失败方,当byStatus为0时，该字段有意义
};
/// 更新国王信息
const BYTE UPDATE_COUNTRY_KING = 18;
struct stUpdateCountryKingUserCmd : public stCountryUserCmd
{
	stUpdateCountryKingUserCmd()
	{       
		byParam = UPDATE_COUNTRY_KING;
		bzero(kingName, sizeof(kingName));
		isEmperor = 0;
	}       
	char   kingName[MAX_NAMESIZE];
	BYTE   isEmperor; // 0不是皇帝所在国,1是皇帝所在国
};

enum
{
	ANSWER_COUNTRY_DARE_QUESTION,
	ANSWER_COUNTRY_DARE_NO,
	ANSWER_COUNTRY_DARE_YES
};
/// 国战应答
const BYTE ANSWER_COUNTRY_DARE_PARA = 19;
struct stAnswerCountryDareUserCmd : public stCountryUserCmd
{
	stAnswerCountryDareUserCmd()
	{       
		byParam = ANSWER_COUNTRY_DARE_PARA;
		byStatus = ANSWER_COUNTRY_DARE_QUESTION;
	}

	BYTE byStatus;
};      


/// 选择要在国战时传送到边境的玩家等级
const BYTE SELECT_TRANS_LEVEL = 20;
struct stSelectTransLevelUserCmd : public stCountryUserCmd
{
	stSelectTransLevelUserCmd()
	{
		byParam = SELECT_TRANS_LEVEL;
		dwLevel = 0;
	}

	DWORD dwLevel;//大于等于该等级
};
//国王禁言一个玩家
#define FORBID_TALK_COUNTRY_PARA 21
struct stForbidTalkCountryUserCmd : public stCountryUserCmd
{
	stForbidTalkCountryUserCmd()
	{
		byParam = FORBID_TALK_COUNTRY_PARA;
		bzero(name, MAX_NAMESIZE);
	}
	char name[MAX_NAMESIZE];
};


enum
{
	WAIT_TECH_TYPE = 1,      // 投票选出的科技，等待国王选择官员
	ACTIVE_TECH_TYPE = 2,    // 正在进行升级
	FINISH_TECH_TYPE = 3,    // 已经完成升级，可以打造相应装备(包括等级大于0的所有状态的科技)
};
// 获取指定类型科技列表
#define REQ_TECH_PARA 22
struct stReqTechUserCmd : public stCountryUserCmd
{
	stReqTechUserCmd()
	{
		byParam = REQ_TECH_PARA;
		dwType = 0;
	}

	DWORD dwType; // 1为等研究的科技，2，正在升级的科技,3，已完成的科技
};

struct stTechItem
{
	DWORD dwOption;
	char  szOptionDesc[MAX_NAMESIZE];
	char  szResearchName[MAX_NAMESIZE];
	DWORD dwLevel;
	DWORD dwProgress;
};

// 返回指定类型可研究科技列表
#define RTN_TECH_PARA 23
struct stRtnTechUserCmd : public stCountryUserCmd
{
	stRtnTechUserCmd()
	{
		byParam = RTN_TECH_PARA;
		dwSize = 0;
	}

	DWORD dwType;
	DWORD dwSize;
	stTechItem data[0];
};

// 获取待选官员列表
#define REQ_WAIT_OFFICIAL_PARA 24
struct stReqWaitOfficialUserCmd : public stCountryUserCmd
{
	stReqWaitOfficialUserCmd()
	{
		byParam = REQ_WAIT_OFFICIAL_PARA;
	}
};

struct stWaitOfficialItem
{
	DWORD dwCharID;
	char  szName[MAX_NAMESIZE];
};
// 返回待选官员列表
#define RTN_WAIT_OFFICIAL_PARA 25
struct stRtnWaitOfficialUserCmd : public stCountryUserCmd
{
	stRtnWaitOfficialUserCmd()
	{
		byParam = RTN_WAIT_OFFICIAL_PARA;
		dwSize = 0;
	}

	DWORD dwSize;
	stWaitOfficialItem data[0];
};
// 设置科技研究员
#define SET_TECH_SEARCH_PARA 26
struct stSetTechSearchUserCmd : public stCountryUserCmd
{
	stSetTechSearchUserCmd()
	{
		byParam = SET_TECH_SEARCH_PARA;
		dwOption = 0;
		dwCharID = 0;
	}

	DWORD dwOption;
	DWORD dwCharID;
};

// 提升科技完成度
#define UP_TECH_DEGREE_PARA 27 
struct stUpTechDegreeUserCmd : public stCountryUserCmd
{       
	stUpTechDegreeUserCmd()
	{       
		byParam = UP_TECH_DEGREE_PARA;
		dwOption = 0;
	}

	DWORD dwOption;
};

enum
{
	QUESTION_CONFIRM_SEARCHER, // 询问是否同意成为研究员
	YES_CONFIRM_SEARCHER,  // 同意成为研究员
	NO_CONFIRM_SEARCHER // 不同意成为研究员
};

// 询问是否同意成为研究员
#define CONFIRM_SEARCHER_PARA 28
struct stConfirmSearcherUserCmd : public stCountryUserCmd
{
	stConfirmSearcherUserCmd()
	{               
		byParam = CONFIRM_SEARCHER_PARA;
	}               

	DWORD dwTechID; // 科技ID       
	BYTE byStatus;
};

// 该国人不能登陆
#define  DISENABLEL_OGIN_COUNTRY_PARA  29 
enum  FALSE_COUNTRY_STATUS
{
	FALSE_REGISTER_COUNTRY,  //return when register fail 
	FALSE_LOGIN_COUNTRY     // return when login fail 
};
struct stDisableLoginCountryCmd : public stCountryUserCmd
{
	stDisableLoginCountryCmd()
	{
		byParam = DISENABLEL_OGIN_COUNTRY_PARA;
	}
	FALSE_COUNTRY_STATUS status;
};
// 取消科技研究员
#define CANCEL_TECH_SEARCH_PARA 30
struct stCancelTechSearchUserCmd : public stCountryUserCmd
{
	stCancelTechSearchUserCmd()
	{
		byParam = CANCEL_TECH_SEARCH_PARA;
		dwOption = 0;
	}
	DWORD dwOption;
};
// 申请国战反攻
#define ANTI_DARE_COUNTRY_FORMAL_PARA  31
struct stAntiDareCountryFormalCmd : public stCountryUserCmd
{
	stAntiDareCountryFormalCmd()
	{
		byParam = ANTI_DARE_COUNTRY_FORMAL_PARA;
	}

	DWORD dwCountryID; // 要挑战的国家
};
// 发布国家公告
#define COUNTRY_NOTE_PARA  32
struct stCountryNoteCmd : public stCountryUserCmd
{
	stCountryNoteCmd()
	{
		byParam = COUNTRY_NOTE_PARA;
	}

	char note[255];
};

// 请求国家公告
#define REQ_COUNTRY_NOTE_PARA  33
struct stReqCountryNoteCmd : public stCountryUserCmd
{
	stReqCountryNoteCmd()
	{
		byParam = REQ_COUNTRY_NOTE_PARA;
	}
};

// 请求大将军的信息
#define REQ_GEN_COUNTRY_PARA  36 
struct stReqGenCountryCmd : public stCountryUserCmd
{
	stReqGenCountryCmd()
	{
		byParam = REQ_GEN_COUNTRY_PARA;
	}   
};  

// 返回大将军的信息                     
#define RET_GEN_COUNTRY_PARA  37
struct stRetGenCountryCmd : public stCountryUserCmd
{
	stRetGenCountryCmd()
	{
		byParam = RET_GEN_COUNTRY_PARA;
	}
	DWORD level;
	DWORD exp;
	DWORD maxExp;
};

//soke 2016 - 3 - 11 任命外交官(停用)
#define APPOINT_DIPLOMAT_PARA  38
struct stAppointDiplomatCmd : public stCountryUserCmd
{
	stAppointDiplomatCmd()
	{
		byParam = APPOINT_DIPLOMAT_PARA;
		bzero(name, sizeof(name));
	}

	char name[MAX_NAMESIZE];
};

//soke 2016 - 3 - 11 罢免外交官(停用)
#define CANCEL_DIPLOMAT_PARA  39
struct stCancelDiplomatCmd : public stCountryUserCmd
{
	stCancelDiplomatCmd()
	{
		byParam = CANCEL_DIPLOMAT_PARA;
	}
};

//soke 2016 - 3 - 11 国王任命巡捕（左上）
#define APPOINT_CATCHER_PARA  40
struct stAppointCatcherCmd : public stCountryUserCmd
{
	stAppointCatcherCmd()
	{
		byParam = APPOINT_CATCHER_PARA;
		bzero(name, sizeof(name));
	}

	char name[MAX_NAMESIZE];
};

//soke 2016 - 3 - 11 国王罢免巡捕（左上）
#define CANCEL_CATCHER_PARA  41
struct stCancelCatcherCmd : public stCountryUserCmd
{
	stCancelCatcherCmd()
	{
		byParam = CANCEL_CATCHER_PARA;
		bzero(name, sizeof(name));
	}

	char name[MAX_NAMESIZE];
};

//soke 2016 - 3 - 11 国王任命巡捕（左下）
#define APPOINT_CATCHEX_PARA  42
struct stAppointCatchexCmd : public stCountryUserCmd
{
	stAppointCatchexCmd()
	{
		byParam = APPOINT_CATCHEX_PARA;
		bzero(name, sizeof(name));
	}

	char name[MAX_NAMESIZE];
};

//soke 2016 - 3 - 11 国王罢免巡捕（左下）
#define CANCEL_CATCHEX_PARA  43
struct stCancelCatchexCmd : public stCountryUserCmd
{
	stCancelCatchexCmd()
	{
		byParam = CANCEL_CATCHEX_PARA;
		bzero(name, sizeof(name));
	}

	char name[MAX_NAMESIZE];
};

//soke 2016 - 3 - 11 国王任命御史大夫（右上）
#define APPOINT_CENSOR_PARA  44
struct stAppointCensorCmd : public stCountryUserCmd
{
	stAppointCensorCmd()
	{
		byParam = APPOINT_CENSOR_PARA;
		bzero(name, sizeof(name));
	}

	char name[MAX_NAMESIZE];
};

//soke 2016 - 3 - 11 国王罢免御史大夫（右上）
#define CANCEL_CENSOR_PARA  45
struct stCancelCensorCmd : public stCountryUserCmd
{
	stCancelCensorCmd()
	{
		byParam = CANCEL_CENSOR_PARA;
		bzero(name, sizeof(name));
	}

	char name[MAX_NAMESIZE];
};

//soke 2016 - 3 - 11 国王任命御史大夫（右下）
#define APPOINT_CENSOS_PARA  46
struct stAppointCensosCmd : public stCountryUserCmd
{
	stAppointCensosCmd()
	{
		byParam = APPOINT_CENSOS_PARA;
		bzero(name, sizeof(name));
	}

	char name[MAX_NAMESIZE];
};

//soke 2016 - 3 - 11 国王罢免御史大夫（右下）
#define CANCEL_CENSOS_PARA  47
struct stCancelCensosCmd : public stCountryUserCmd
{
	stCancelCensosCmd()
	{
		byParam = CANCEL_CENSOS_PARA;
		bzero(name, sizeof(name));
	}

	char name[MAX_NAMESIZE];
};

//soke 2016 - 3 - 11 国王任命元帅
#define APPOINT_YUANSHUAI_PARA  48
struct stAppointYuanshuaiCmd : public stCountryUserCmd
{
	stAppointYuanshuaiCmd()
	{
		byParam = APPOINT_YUANSHUAI_PARA;
		bzero(name, sizeof(name));
	}

	char name[MAX_NAMESIZE];
};

//soke 2016 - 3 - 11 国王罢免元帅
#define CANCEL_YUANSHUAI_PARA  49
struct stCancelYuanshuaiCmd : public stCountryUserCmd
{
	stCancelYuanshuaiCmd()
	{
		byParam = CANCEL_YUANSHUAI_PARA;
		bzero(name, sizeof(name));
	}

	char name[MAX_NAMESIZE];
};

//soke 2016 - 3 - 11 国王任命宰相
#define APPOINT_ZAIXIANG_PARA  50
struct stAppointZaixiangCmd : public stCountryUserCmd
{
	stAppointZaixiangCmd()
	{
		byParam = APPOINT_ZAIXIANG_PARA;
		bzero(name, sizeof(name));
	}

	char name[MAX_NAMESIZE];
};

//soke 2016 - 3 - 11 国王罢免宰相
#define CANCEL_ZAIXIANG_PARA  51
struct stCancelZaixiangCmd : public stCountryUserCmd
{
	stCancelZaixiangCmd()
	{
		byParam = CANCEL_ZAIXIANG_PARA;
		bzero(name, sizeof(name));
	}

	char name[MAX_NAMESIZE];
};

// 查询国家强弱
#define REQUEST_COUNTRY_POWER_PARA  52
struct stRequestCountryPowerCmd : public stCountryUserCmd
{
	stRequestCountryPowerCmd()
	{
		byParam = REQUEST_COUNTRY_POWER_PARA;
	}
};

// 返回国家强弱
#define RETURN_COUNTRY_POWER_PARA  53
struct stReturnCountryPowerCmd : public stCountryUserCmd
{
	stReturnCountryPowerCmd()
	{
		byParam = RETURN_COUNTRY_POWER_PARA;
	}
	BYTE country[2];
};

// 领取每日皇帝的奖励
#define REQ_DAILY_EMPEROR_MONEY 54
struct stReqDailyEmperorMoneyCmd : public stCountryUserCmd
{
	stReqDailyEmperorMoneyCmd()
	{
		byParam = REQ_DAILY_EMPEROR_MONEY;
	}
};

// 请求国王排行榜
#define REQ_KING_LIST_PARA  55
struct stReqKingListCmd : public stCountryUserCmd
{
	stReqKingListCmd()
	{
		byParam = REQ_KING_LIST_PARA;
	}
};

struct _KingListItem
{
	char  king_name[MAX_NAMESIZE]; // 国王名称
	WORD  online_time; //在线时间 单位为小时
	DWORD country_id; // 国家ID
};

// 返回国王排行榜
#define RTN_KING_LIST_PARA  56
struct stRtnKingListCmd : public stCountryUserCmd
{
	stRtnKingListCmd()
	{
		byParam = RTN_KING_LIST_PARA;
		size = 0;
	}
	BYTE size;
	_KingListItem data[0];
};

// 请求城市占领者
#define REQ_CITY_OWNER_LIST_PARA  57
struct stReqCityOwnerListCmd : public stCountryUserCmd
{
	stReqCityOwnerListCmd()
	{
		byParam = REQ_CITY_OWNER_LIST_PARA;
	}
};

// 返回城市占领者列表
#define RTN_CITY_OWNER_LIST_PARA  58
struct stRtnCityOwnerListCmd : public stCountryUserCmd
{
	stRtnCityOwnerListCmd()
	{
		byParam = RTN_CITY_OWNER_LIST_PARA;
		size = 0;
	}
	WORD size;
	struct
	{
		char cityName[MAX_NAMESIZE];
		char unionName[MAX_NAMESIZE];
	}list[0];
};

// 请求城市占领者
#define REQ_NPC_OWNER_LIST_PARA  59
struct stReqNpcOwnerListCmd : public stCountryUserCmd
{
	stReqNpcOwnerListCmd()
	{
		byParam = REQ_NPC_OWNER_LIST_PARA;
	}
};
// 返回城市占领者
#define RTN_NPC_OWNER_LIST_PARA  60
struct stRtnNpcOwnerListCmd : public stCountryUserCmd
{
	stRtnNpcOwnerListCmd()
	{
		byParam = RTN_NPC_OWNER_LIST_PARA;
		size=0;
	}
	WORD size;
	struct
	{
		DWORD npcID;//npc的数据id
		char mapName[MAX_NAMESIZE];//地图名称
		WORD x;     //所在位置
		WORD y;     //所在位置
		char septName[MAX_NAMESIZE];//占领的家族名称
	}list[0];
};

///soke 2016 - 3 - 11 更新国家官员 - 巡捕（左上）
const BYTE UPDATE_COUNTRY_CATCHER = 61;
struct stUpdateCountryCatcherUserCmd : public stCountryUserCmd
{
	stUpdateCountryCatcherUserCmd()
	{
		byParam = UPDATE_COUNTRY_CATCHER;
		bzero(catcherName, sizeof(catcherName));
		isEmperor = 0;
	}
	char   catcherName[MAX_NAMESIZE];
	BYTE   isEmperor; // 0不是皇帝所在国,1是皇帝所在国
};

///soke 2016 - 3 - 11 更新国家官员 - 巡捕（左下）
const BYTE UPDATE_COUNTRY_CATCHEX = 62;
struct stUpdateCountryCatchexUserCmd : public stCountryUserCmd
{
	stUpdateCountryCatchexUserCmd()
	{
		byParam = UPDATE_COUNTRY_CATCHEX;
		bzero(catchexName, sizeof(catchexName));
		isEmperor = 0;
	}
	char   catchexName[MAX_NAMESIZE];
	BYTE   isEmperor; // 0不是皇帝所在国,1是皇帝所在国
};

///soke 2016 - 3 - 11 更新国家官员 - 御史大夫（右上）
const BYTE UPDATE_COUNTRY_CENSOR = 63;
struct stUpdateCountryCensorUserCmd : public stCountryUserCmd
{
	stUpdateCountryCensorUserCmd()
	{
		byParam = UPDATE_COUNTRY_CENSOR;
		bzero(censorName, sizeof(censorName));
		isEmperor = 0;
	}
	char   censorName[MAX_NAMESIZE];
	BYTE   isEmperor; // 0不是皇帝所在国,1是皇帝所在国
};

///soke 2016 - 3 - 11 更新国家官员 - 御史大夫（右下）
const BYTE UPDATE_COUNTRY_CENSOS = 64;
struct stUpdateCountryCensosUserCmd : public stCountryUserCmd
{
	stUpdateCountryCensosUserCmd()
	{
		byParam = UPDATE_COUNTRY_CENSOS;
		bzero(censosName, sizeof(censosName));
		isEmperor = 0;
	}
	char   censosName[MAX_NAMESIZE];
	BYTE   isEmperor; // 0不是皇帝所在国,1是皇帝所在国
};

///soke 2016 - 3 - 11 更新国家官员 - 元帅
const BYTE UPDATE_COUNTRY_YUANSHUAI = 65;
struct stUpdateCountryYuanshuaiUserCmd : public stCountryUserCmd
{
	stUpdateCountryYuanshuaiUserCmd()
	{
		byParam = UPDATE_COUNTRY_YUANSHUAI;
		bzero(yuanshuaiName, sizeof(yuanshuaiName));
		isEmperor = 0;
	}
	char   yuanshuaiName[MAX_NAMESIZE];
	BYTE   isEmperor; // 0不是皇帝所在国,1是皇帝所在国
};

///soke 2016 - 3 - 11 更新国家官员 - 宰相
const BYTE UPDATE_COUNTRY_ZAIXIANG = 66;
struct stUpdateCountryZaixiangUserCmd : public stCountryUserCmd
{
	stUpdateCountryZaixiangUserCmd()
	{
		byParam = UPDATE_COUNTRY_ZAIXIANG;
		bzero(zaixiangName, sizeof(zaixiangName));
		isEmperor = 0;
	}
	char   zaixiangName[MAX_NAMESIZE];
	BYTE   isEmperor; // 0不是皇帝所在国,1是皇帝所在国
};

///soke 领取每日国王的薪资
#define REQ_DAILY_KING_MONEY 67
struct stReqDailyKingMoneyCmd : public stCountryUserCmd
{
	stReqDailyKingMoneyCmd()
	{
		byParam = REQ_DAILY_KING_MONEY;
	}
};

//soke 领取每日元帅的薪资
#define REQ_DAILY_YUANSHUAI_MONEY 68
struct stReqDailyYuanshuaiMoneyCmd : public stCountryUserCmd
{
	stReqDailyYuanshuaiMoneyCmd()
	{
		byParam = REQ_DAILY_YUANSHUAI_MONEY;
	}
};

//soke 领取每日宰相的薪资
#define REQ_DAILY_ZAIXIANG_MONEY 69
struct stReqDailyZaixiangMoneyCmd : public stCountryUserCmd
{
	stReqDailyZaixiangMoneyCmd()
	{
		byParam = REQ_DAILY_ZAIXIANG_MONEY;
	}
};

//soke 领取每日捕头(左)的薪资
#define REQ_DAILY_CATCHER_MONEY 70
struct stReqDailyCatcherMoneyCmd : public stCountryUserCmd
{
	stReqDailyCatcherMoneyCmd()
	{
		byParam = REQ_DAILY_CATCHER_MONEY;
	}
};

//soke 领取每日御史大夫(左)的薪资
#define REQ_DAILY_CENSOR_MONEY 71
struct stReqDailyCensorMoneyCmd : public stCountryUserCmd
{
	stReqDailyCensorMoneyCmd()
	{
		byParam = REQ_DAILY_CENSOR_MONEY;
	}
};

//soke 领取每日捕头(右)的薪资
#define REQ_DAILY_CATCHEX_MONEY 72
struct stReqDailyCatchexMoneyCmd : public stCountryUserCmd
{
	stReqDailyCatchexMoneyCmd()
	{
		byParam = REQ_DAILY_CATCHEX_MONEY;
	}
};

//soke 领取每日御史大夫(右)的薪资
#define REQ_DAILY_CENSOS_MONEY 73
struct stReqDailyCensosMoneyCmd : public stCountryUserCmd
{
	stReqDailyCensosMoneyCmd()
	{
		byParam = REQ_DAILY_CENSOS_MONEY;
	}
};

//soke  变更国籍(工资)
const BYTE CHANGE_COUNTRY_SALARY_PARA = 76;
struct stChangeCountrySalaryCmd : public stCountryUserCmd
{
	stChangeCountrySalaryCmd()
	{
		byParam = CHANGE_COUNTRY_SALARY_PARA;
	}

	DWORD dwToCountryID; // 变更到国家的ID
};

//soke  领取工资
const BYTE ADD_COUNTRY_SALARY_PARA = 77;
struct stAddCountrySalaryCmd : public stCountryUserCmd
{
	stAddCountrySalaryCmd()
	{
		byParam = ADD_COUNTRY_SALARY_PARA;
	}
};

//soke 领取每日登陆奖励
const BYTE ADD_COUNTRY_LOGIN_PARA = 78;
struct stAddCountryLoginCmd : public stCountryUserCmd
{
	stAddCountryLoginCmd()
	{
		byParam = ADD_COUNTRY_LOGIN_PARA;
	}
};

//soke 领取vip奖励
const BYTE ADD_COUNTRY_VIP_PARA = 79;
struct stAddCountryVipCmd : public stCountryUserCmd
{
	stAddCountryVipCmd()
	{
		byParam = ADD_COUNTRY_VIP_PARA;
	}
};

//soke 金山抽奖
const BYTE ADD_COUNTRY_CHOU_PARA = 80;
struct stAddCountryChouCmd : public stCountryUserCmd
{
	stAddCountryChouCmd()
	{
		byParam = ADD_COUNTRY_CHOU_PARA;
	}
};

//////////////////////////////////////////////////////////////
// 国家指令定义结束
//////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////
// 投票指令定义开始
//////////////////////////////////////////////////////////////
enum
{
	TECH_VOTE // 科技投票
};

struct stVoteUserCmd : public stNullUserCmd
{
	stVoteUserCmd()
	{
		byCmd = VOTE_USERCMD;
	}
};

// 请求投票列表
#define REQUEST_VOTE_LIST_PARA 1
struct stRequestVoteListUserCmd : public stVoteUserCmd
{
	stRequestVoteListUserCmd()
	{
		byParam = REQUEST_VOTE_LIST_PARA;
		byType = TECH_VOTE;
	}

	BYTE byType; // 投票类型
};
struct stVoteItem
{       
	DWORD dwOption; // 选项编号
	DWORD dwBallot; // 选项当前投票数
	char  szOptionDesc[MAX_NAMESIZE];
};

// 返回投票列表
#define RETURN_VOTE_LIST_PARA 2
struct stReturnVoteListUserCmd : public stVoteUserCmd
{       
	stReturnVoteListUserCmd()
	{       
		byParam = RETURN_VOTE_LIST_PARA;
		dwSize = 0;
		byType = TECH_VOTE;
		dwVoteID = 0;
	}

	DWORD dwVoteID;
	BYTE byType;
	DWORD dwSize;
	stVoteItem data[0];
};
// 提交投票
#define COMMIT_VOTE_PARA 3 
struct stCommitVoteUserCmd : public stVoteUserCmd
{       
	stCommitVoteUserCmd()
	{       
		byParam = COMMIT_VOTE_PARA;
		dwVoteID = 0;
		dwOption = 0;
	}

	DWORD dwVoteID; // 该次投票的ID
	DWORD dwOption; // 该次提交的选项
};
//////////////////////////////////////////////////////////////
// 投票指令定义结束
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
/// 定义邮件命令开始
//////////////////////////////////////////////////////////////
const unsigned int mail_postage = 50;   ///邮资50文

struct stMailUserCmd : public stNullUserCmd
{
	stMailUserCmd()
	{
		byCmd = MAIL_USERCMD;
	}       
};

//发送邮件 
const BYTE SEND_MAIL_PARA = 3;
struct stSendMail : public stMailUserCmd
{       
	char toName[MAX_NAMESIZE];
	char title[MAX_NAMESIZE];
	char text[256];
	DWORD sendMoney;
	DWORD recvMoney;
	DWORD sendGold;
	DWORD recvGold;
	DWORD itemID;
	stSendMail()
	{       
		byParam = SEND_MAIL_PARA;
		sendMoney = 0;
		recvMoney = 0;
		sendGold = 0;
		recvGold = 0;
		itemID = 0;
	}
};


const BYTE GET_LIST_MAIL_PARA = 5;
struct stGetListMail : public stMailUserCmd
{
	stGetListMail()
	{
		byParam = GET_LIST_MAIL_PARA;
	}
};

//添加邮件列表
const BYTE ADD_LIST_MAIL_PARA = 6;
struct stAddListMail : public stMailUserCmd
{
	DWORD id;
	BYTE state;
	char fromName[MAX_NAMESIZE];
	bool accessory;
	DWORD endTime;
	BYTE type;  //0:普通 1：系统
	stAddListMail()
	{
		byParam = ADD_LIST_MAIL_PARA;
		id = 0;
		state = 0;
		accessory = 0;
		endTime = 0;
		bzero(fromName, sizeof(fromName));
		type = 0;
	}
};

const BYTE OPEN_MAIL_PARA = 7;
struct stOpenMail : public stMailUserCmd
{               
	DWORD mailID;
	stOpenMail()
	{
		byParam = OPEN_MAIL_PARA;
	}
};

//邮件内容
const BYTE CONTENT_MAIL_PARA = 8;
struct stContentMail : public stMailUserCmd
{
	DWORD mailID;
	char title[MAX_NAMESIZE];
	bool accessory;
	char text[256];
	DWORD sendMoney;
	DWORD recvMoney;
	DWORD sendGold;
	DWORD recvGold;
	t_Object item;
	stContentMail()
	{
		byParam = CONTENT_MAIL_PARA;
		mailID = 0;
		bzero(title, sizeof(title));
		accessory = false;
		bzero(text, sizeof(text));
		sendMoney = recvMoney = 0;
		sendGold = 0;
		recvGold = 0; 
		bzero(&item, sizeof(item));
	}
};


//获取附件
const BYTE GET_ITEM_MAIL_PARA = 9;
struct stGetItemMail : public stMailUserCmd
{       
	DWORD mailID;
	stGetItemMail()
	{       
		byParam = GET_ITEM_MAIL_PARA;
	}
};

//服务器发送，删除一个邮件的附件
const BYTE REMOVE_ITEM_MAIL_PARA = 10;
struct stRemoveItemMail : public stMailUserCmd
{       
	DWORD mailID;
	stRemoveItemMail()
	{       
		byParam = REMOVE_ITEM_MAIL_PARA;
	}
};

//从列表中删除一个邮件
const BYTE DEL_MAIL_PARA = 11;
struct stDelMail : public stMailUserCmd
{
	DWORD mailID;
	stDelMail()
	{       
		byParam = DEL_MAIL_PARA;
	}
};

//有新邮件的通知
const BYTE NOTIFY_NEW_MAIL_PARA = 12;
struct stNotifyNewMail : public stMailUserCmd
{       
	stNotifyNewMail()
	{
		byParam = NOTIFY_NEW_MAIL_PARA;
	}       
}; 

//退回邮件 
const BYTE TURN_BACK_MAIL_PARA = 13;
struct stTurnBackMail : public stMailUserCmd
{       
	DWORD mailID;
	stTurnBackMail()
	{       
		byParam = TURN_BACK_MAIL_PARA;
	}
};


//////////////////////////////////////////////////////////////
/// 定义邮件命令结束
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
/// 定义拍卖命令开始
//////////////////////////////////////////////////////////////

struct stAuctionUserCmd : public stNullUserCmd
{
	stAuctionUserCmd()
	{
		byCmd = AUCTION_USERCMD;
	}       
};      

enum                                    
{                                               
	AUCTION_WEAPOM_SWORD = 1,               
	AUCTION_WEAPOM_AXE = 2,         
	AUCTION_WEAPOM_BOW = 3,                 
	AUCTION_WEAPOM_STICK = 4,               
	AUCTION_WEAPOM_WAND = 5,        
	AUCTION_WEAPOM_FAN = 6,                 
	AUCTION_EQUIP_HEAD = 11,                
	AUCTION_EQUIP_BODY = 12,        
	AUCTION_EQUIP_WRIST = 13,               
	AUCTION_EQUIP_SHIELD = 14,              
	AUCTION_EQUIP_WAIST = 15,       
	AUCTION_EQUIP_FOOT = 16,                
	AUCTION_ACCESSORY_NECKLACE = 21,        
	AUCTION_ACCESSORY_RING = 22,    
	AUCTION_ACCESSORY_ADORNMENT = 23,       
	AUCTION_BOOK_FIGHTER = 31,
	AUCTION_BOOK_ARCHER = 32,
	AUCTION_BOOK_WIZARD = 33,
	AUCTION_BOOK_SUMMONER = 34,
	AUCTION_BOOK_PRIEST = 35,
	AUCTION_BOOK_SPECIAL = 36,
	AUCTION_OTHER_GEM = 41,                 
	AUCTION_OTHER_ITEM = 42,                
	AUCTION_OTHER_MATERIAL = 43,    
	AUCTION_OTHER_LEECHDOM = 44     
};

const BYTE AUCTION_BID_TYPE_MONEY = 0;
const BYTE AUCTION_BID_TYPE_GOLD = 1;

//拍卖一件物品
const BYTE SALE_AUCTION_PARA = 1;
struct stSaleAuction : public stAuctionUserCmd
{               
	DWORD itemID;
	DWORD minMoney;
	DWORD maxMoney;
	DWORD minGold;
	DWORD maxGold;
	BYTE bidType;
	stSaleAuction()
	{       
		byParam = SALE_AUCTION_PARA;
		itemID = 0xffffffff;
		minMoney = 0;
		maxMoney = 0;
		minGold = 0;
		maxGold = 0;
		bidType = 0;
	}
};

//竞价
const BYTE BID_AUCTION_PARA = 2;
struct stBidAuction : public stAuctionUserCmd
{
	DWORD auctionID;
	DWORD money;
	DWORD gold;
	stBidAuction()
	{
		byParam = BID_AUCTION_PARA;
		auctionID = 0;
		money = 0;
		gold = 0;
	}
};

//添加列表
const BYTE ADD_LIST_AUCTION_PARA = 3;
struct stAddListAuction : public stAuctionUserCmd
{
	BYTE list;//1:查看列表 2:竞标列表 3:拍卖列表 4:所有列表
	DWORD auctionID;
	DWORD minMoney;
	DWORD maxMoney;
	DWORD minGold;
	DWORD maxGold;
	DWORD endTime;
	t_Object item;
	bool mine;//是否我的价格最高
	char owner[MAX_NAMESIZE];//拍卖者
	DWORD max;//总共多少个
	BYTE bidType;
	stAddListAuction()
	{
		byParam = ADD_LIST_AUCTION_PARA;

		list = 0;
		auctionID = 0;
		minMoney = 0;
		maxMoney = 0;
		minGold = 0;
		maxGold = 0;
		bzero(&item, sizeof(item));
		mine = false;
		bzero(owner, sizeof(owner));
		max = 0;
		bidType = 0;
	}
};

//移除列表
const BYTE REMOVE_LIST_AUCTION_PARA = 4;
struct stRemoveListAuction : public stAuctionUserCmd
{
	BYTE list;//1:查看列表 2:拍卖列表 3:竞标列表 4:所有列表
	DWORD auctionID;
	stRemoveListAuction()
	{
		byParam = REMOVE_LIST_AUCTION_PARA;

		list = 0;
		auctionID = 0;
	}
};

//搜索
const BYTE QUERY_AUCTION_PARA = 5;
struct stQueryAuction : public stAuctionUserCmd
{
	BYTE type;
	char name[MAX_NAMESIZE];
	BYTE quality;//1普通, 2蓝色, 3金色, 5神圣, 9套装
	WORD level;
	WORD page;
	stQueryAuction()
	{
		byParam = QUERY_AUCTION_PARA;

		type = 0;
		bzero(name, sizeof(name));
		quality = 0;
		level = 0;
		page = 0;
	}
};

//取消拍卖
const BYTE CANCEL_SALE_AUCTION_PARA = 6;
struct stCancelSaleAuction : public stAuctionUserCmd
{
	DWORD auctionID;
	stCancelSaleAuction()
	{
		byParam = CANCEL_SALE_AUCTION_PARA;

		auctionID = 0;
	}
};

//请求列表
const BYTE GET_LIST_AUCTION_PARA = 7;
struct stGetListAuction : public stAuctionUserCmd
{
	BYTE list;//2:拍卖列表 3:竞标列表
	stGetListAuction()
	{
		byParam = GET_LIST_AUCTION_PARA;

		list = 0;
	}
};

//////////////////////////////////////////////////////////////
/// 定义金子命令开始
//////////////////////////////////////////////////////////////
struct stGoldUserCmd : public stNullUserCmd
{
	stGoldUserCmd()
	{
		byCmd = GOLD_USERCMD;
	}
};

const BYTE QUERY_GOLD_PARA = 1;
struct stQueryGold : public stGoldUserCmd
{
	stQueryGold()
	{
		byParam = QUERY_GOLD_PARA;
	}
};

enum
{
	REDEEM_SUCCESS,    // 兑换成功 
	REDEEM_BUSY,       // 服务器忙
	REDEEM_STOP,      // 兑换系统停止服务
	REDEEM_FAIL,       // 失败
};
#define REDEEM_RATE_GOLD 1 //兑换金子比率1:1
#define REDEEM_RATE_MONTH_CARD 18//兑换月卡比率1:18
//请求兑换金子
const BYTE REQUEST_REDEEM_GOLD_PARA = 2;
struct stRequestRedeemGold : public stGoldUserCmd
{
	stRequestRedeemGold()
	{
		byParam = REQUEST_REDEEM_GOLD_PARA;
	}
	DWORD dwNum;    //需要兑换的点数
};
//兑换金子返回
const BYTE REDEEM_GOLD_PARA = 3;
struct stRedeemGold : public stGoldUserCmd
{
	stRedeemGold()
	{
		byParam = REDEEM_GOLD_PARA;
	}
	DWORD dwNum;    //返回当前金子数
	DWORD dwBalance;            /// 点卡余额
	BYTE byReturn;  //返回类型
};

//请求兑换月卡
const BYTE REQUEST_REDEEM_MONTH_CARD_PARA = 4;
struct stRequestRedeemMonthCard : public stGoldUserCmd
{
	stRequestRedeemMonthCard()
	{
		byParam = REQUEST_REDEEM_MONTH_CARD_PARA;
	}
};

//兑换月卡返回
const BYTE REDEEM_MONTH_CARD_PARA = 5;
struct stRedeemMonthCard : public stGoldUserCmd
{
	stRedeemMonthCard()
	{
		byParam = REDEEM_MONTH_CARD_PARA;
	}
	DWORD dwNum;    //到期时间(time_t,4字节结构)
	DWORD dwBalance;            /// 点卡余额
	BYTE byReturn;  //返回类型
};

//查询月卡和金子
const BYTE REQUEST_CARD_AND_GOLD_PARA = 6;
struct stRequestCardAndGold :  public stGoldUserCmd
{
	stRequestCardAndGold()
	{
		byParam = REQUEST_CARD_AND_GOLD_PARA;
	}
};

//查询返回
const BYTE RETURN_CARD_AND_GOLD_PARA = 7;
struct stReturnCardAndGold :  public stGoldUserCmd
{
	stReturnCardAndGold()
	{
		byParam = RETURN_CARD_AND_GOLD_PARA;
	}
	DWORD dwMonthCard;//月卡
	DWORD dwGold;//金子
	BYTE byReturn;//返回类型
};
//查询点卡
const BYTE REQUEST_POINT_PARA = 8;
struct stRequestPoint :  public stGoldUserCmd
{
	stRequestPoint()
	{
		byParam = REQUEST_POINT_PARA;
	}
};
//查询点卡返回
const BYTE RETURN_REQUEST_POINT_PARA = 9;
struct stReturnRequestPoint :  public stGoldUserCmd
{
	stReturnRequestPoint()
	{
		byParam = RETURN_REQUEST_POINT_PARA;
	}
	DWORD dwPoint;//剩余点数
	BYTE byReturn;//查询返回类型
};

//金子系停止服务
const BYTE STOP_SERVICE_GOLD_PARA = 10;
struct stStopServiceGold :  public stGoldUserCmd
{
	stStopServiceGold()
	{
		byParam = STOP_SERVICE_GOLD_PARA;
	}
};

#define CARDID_LENGTH_PARA 32
///消费卡的区类型
enum
{
	ZONE_CARD_OBJECT=1, ///道具卡 
	ZONE_CARD_PROFRESSION=2,///专区卡
};
//消费卡消费
const BYTE CONSUME_CARD_PARA = 11;
struct stConSumeCardCard : public stGoldUserCmd
{
	stConSumeCardCard()
	{
		byParam = CONSUME_CARD_PARA;
		bzero(cardid,sizeof(cardid));
	}
	DWORD type;
	char cardid[CARDID_LENGTH_PARA];
};
/// 消费子类型
enum
{
	OBJ_GOLD_STONE = 1,//送精致升级宝石
	OBJ_GOLD_OBJECT = 2,//送金色装备
	OBJ_GREEN_OBJECT = 3,//送神圣装备
	SUBAT_GOLD = 4, //专区卡充值返回子类型//道具卡的子类型不能于该值重复
};
//消费卡消费返回
const BYTE RETURN_CONSUME_CARD_PARA = 12;
struct stReturnConSumeCardCard : public stGoldUserCmd
{
	stReturnConSumeCardCard()
	{
		byParam = RETURN_CONSUME_CARD_PARA;
	}
	BYTE byType;    //消费类型
	BYTE byReturn;  //返回类型
	DWORD balance;  //专区卡所带点数
};
//BEGIN_ONE_CMD
//////////////////////////////////////////////////////////////
// 卡通宠物指令定义开始
//////////////////////////////////////////////////////////////
//宝宝的状态
enum cartoonState
{
	CARTOON_STATE_PUTAWAY,  //收起
	CARTOON_STATE_FOLLOW,   //跟随主人
	CARTOON_STATE_WAITING,  //等待领养
	CARTOON_STATE_ADOPTED   //被领养
};      

//宝宝的数据
struct t_CartoonData
{
	//DWORD cartoonID;              //宝宝id
	char name[MAX_NAMESIZE];        //名字
	DWORD npcID;                    //npcid
	DWORD masterID;                 //主人id
	char masterName[MAX_NAMESIZE];  //主人名字
	BYTE lv;                        //等级
	DWORD exp;                      //经验
	DWORD maxExp;                   //升级经验
	DWORD addExp;                   //给主人增加的经验
	BYTE state;						//状态  
	char adopter[MAX_NAMESIZE];     //领养人
	DWORD time;                     //挂机剩余时间(秒)
	DWORD sp;                       //精气值
	DWORD maxSp;                    //最大精气值
	DWORD masterLevel;              //主人等级
	BYTE repair;					//自动修理主人的装备

	t_CartoonData & operator = (const t_CartoonData& reg)
	{
		memcpy(this,&reg,sizeof(t_CartoonData));
		return *this;
	}
};

struct stCartoonUserCmd : public stNullUserCmd
{
	stCartoonUserCmd()
	{       
		byCmd = CARTOON_USERCMD;
	}
};

#define BUY_CARTOON_PARA 1      //购买
struct stBuyCartoonCmd : public stCartoonUserCmd
{       
	stBuyCartoonCmd()
	{       
		byParam = BUY_CARTOON_PARA;
	}
	DWORD npcID;
};

#define ADD_CARTOON_PARA 2      //添加列表
struct stAddCartoonCmd : public stCartoonUserCmd
{       
	stAddCartoonCmd()
	{       
		byParam = ADD_CARTOON_PARA;
	}
	bool isMine;
	DWORD cartoonID;
	t_CartoonData data;
};

#define REMOVE_CARTOON_PARA 3   //移除列表
struct stRemoveCartoonCmd : public stCartoonUserCmd
{       
	stRemoveCartoonCmd()
	{       
		byParam = REMOVE_CARTOON_PARA;
	}
	DWORD cartoonID;
};

#define FOLLOW_CARTOON_PARA 4   //跟随/取消跟随
struct stFollowCartoonCmd : public stCartoonUserCmd
{
	stFollowCartoonCmd()
	{       
		byParam = FOLLOW_CARTOON_PARA;
	}
	DWORD cartoonID;//取消跟随填0
};

#define LETOUT_CARTOON_PARA 5   //放出，使别人可以领养
struct stLetOutCartoonCmd : public stCartoonUserCmd
{       
	stLetOutCartoonCmd()
	{       
		byParam = LETOUT_CARTOON_PARA;
	}
	DWORD cartoonID;
};

#define ADOPT_CARTOON_PARA 6    //领养宠物
struct stAdoptCartoonCmd : public stCartoonUserCmd
{       
	stAdoptCartoonCmd()
	{       
		byParam = ADOPT_CARTOON_PARA;
	}
	DWORD cartoonID;
};

#define RETURN_CARTOON_PARA 7   //归还宠物
struct stReturnCartoonCmd : public stCartoonUserCmd
{
	stReturnCartoonCmd()
	{
		byParam = RETURN_CARTOON_PARA;
	}
	DWORD cartoonID;
};

#define GETBACK_CARTOON_PARA 8  //收回宠物
struct stGetBackCartoonCmd : public stCartoonUserCmd
{       
	stGetBackCartoonCmd()
	{       
		byParam = GETBACK_CARTOON_PARA;
	}
};

#define DRAWEXP_CARTOON_PARA 9  //提取经验
struct stDrawExpCartoonCmd : public stCartoonUserCmd
{
	stDrawExpCartoonCmd()
	{
		byParam = DRAWEXP_CARTOON_PARA;
	}
	DWORD cartoonID;
};

#define CHANGENAME_CARTOON_PARA 10      //改名字
struct stChangeNameCartoonCmd : public stCartoonUserCmd
{
	stChangeNameCartoonCmd()
	{
		byParam = CHANGENAME_CARTOON_PARA;
	}
	DWORD cartoonID;
	char name[MAX_NAMESIZE];
};

#define ADD_WAITING_CARTOON_PARA 11     //添加等待列表
struct stAddWaitingCartoonCmd : public stCartoonUserCmd
{
	stAddWaitingCartoonCmd()
	{
		byParam = ADD_WAITING_CARTOON_PARA;
	}
	DWORD cartoonID;                //宝宝id
	DWORD npcID;            //NPC ID
	char name[MAX_NAMESIZE];        //名字
	char masterName[MAX_NAMESIZE];  //主人名字
	BYTE state;             //状态
	DWORD time;                     //挂机剩余时间(秒)

	void operator = (const t_CartoonData& reg)
	{
		npcID = reg.npcID;
		memcpy(name,reg.name,MAX_NAMESIZE);
		memcpy(masterName,reg.masterName,MAX_NAMESIZE);
		state = reg.state;
		time = reg.time;
	}
};


#define REMOVE_WAITING_CARTOON_PARA 12  //移除等待列表
struct stRemoveWaitingCartoonCmd : public stCartoonUserCmd
{
	stRemoveWaitingCartoonCmd()
	{
		byParam = REMOVE_WAITING_CARTOON_PARA;
	}
	DWORD cartoonID;
};   

#define REQUEST_LIST_CARTOON_PARA 13    //请求等待列表
struct stRequestListCartoonCmd : public stCartoonUserCmd
{
	stRequestListCartoonCmd()
	{
		byParam = REQUEST_LIST_CARTOON_PARA;
	}
};

#define SALE_CARTOON_PARA 14    //卖出宠物
struct stSaleCartoonCmd : public stCartoonUserCmd
{
	DWORD cartoonID;
	stSaleCartoonCmd()
	{
		byParam = SALE_CARTOON_PARA;

		cartoonID = 0;
	}
};

#define CHARGE_CARTOON_PARA 15  //充值
struct stChargeCartoonCmd : public stCartoonUserCmd
{
	stChargeCartoonCmd()
	{
		byParam = CHARGE_CARTOON_PARA;
	}
	DWORD cartoonID;
	DWORD time;//充多长时间（秒）
};

#define HP_EXP_CARTOON_PARA 16  //同步sp和经验
struct stHpExpCartoonCmd : public stCartoonUserCmd
{
	stHpExpCartoonCmd()
	{
		byParam = HP_EXP_CARTOON_PARA;
	}
	DWORD cartoonID;
	DWORD sp;
	DWORD exp;
};

#define SELL_ALL_CARTOON_PARA 17        //卖包裹里所有物品
struct stSellAllCartoonCmd : public stCartoonUserCmd
{
	stSellAllCartoonCmd()
	{
		byParam = SELL_ALL_CARTOON_PARA;
	}
};

#define CONSIGN_CARTOON_PARA 18 //委托领养
struct stConsignCartoonCmd : public stCartoonUserCmd
{
	stConsignCartoonCmd()
	{
		byParam = CONSIGN_CARTOON_PARA;
	}
	DWORD cartoonID;
	char name[MAX_NAMESIZE];//请求时是被委托人，被请求时是委托人
};

#define CONSIGN_RET_CARTOON_PARA 19     //确认委托领养
struct stConsignRetCartoonCmd : public stCartoonUserCmd
{
	stConsignRetCartoonCmd()
	{
		byParam = CONSIGN_RET_CARTOON_PARA;
	}
	DWORD cartoonID;
	BYTE ret;//0：拒绝 1：同意
};

#define REPAIR_CARTOON_PARA 20  //设置自动修理
struct stRepairCartoonCmd : public stCartoonUserCmd
{
	stRepairCartoonCmd()
	{
		byParam = REPAIR_CARTOON_PARA;
	}
	DWORD cartoonID;
	BYTE repair;//0：关闭 1：打开
};
//////////////////////////////////////////////////////////////
// 卡通宠物指令定义结束
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
/// 定义股票命令开始
//////////////////////////////////////////////////////////////
//STOCK_USERCMD
struct stStockSceneUserCmd : public stNullUserCmd
{
	stStockSceneUserCmd()
	{
		byCmd = STOCK_SCENE_USERCMD;
	}
};
struct stStockBillUserCmd : public stNullUserCmd
{
	stStockBillUserCmd()
	{
		byCmd = STOCK_BILL_USERCMD;
	}
};
/// 股票密码
#define PASSWORD_STOCKPARA 1
struct stPassowrdStockUserCmd : public stStockBillUserCmd
{
	stPassowrdStockUserCmd()
	{
		byParam = PASSWORD_STOCKPARA;
		bzero(byPawword,sizeof(byPawword));
		bzero(byNew1,sizeof(byNew1));
		bzero(byNew2,sizeof(byNew2));
	}
	char byPawword[MAX_PASSWORD];   /// 当前密码
	char byNew1[MAX_PASSWORD];      /// 新密码
	char byNew2[MAX_PASSWORD];      /// 确认密码
};

/// 设置股票密码返回
/// 设置股票密码返回
enum
{
	STOCK_LOGIN_OK, /// 登陆ok
	STOCK_LOGIN_NOTLOGIN,   /// 未登陆(提示请先登陆)
	STOCK_OPEN_OK,  /// 启用帐号ok
	STOCK_CHANGE_OK,    /// 修改密码
	STOCK_ERROR,/// 密码不对
	STOCK_DIFF, /// 两次输入密码不一致
	STOCK_NONE, /// 密码不能为空
	STOCK_SHORT,/// 密码太短
	STOCK_EXIST,/// 帐号已经存在
	STOCK_SERVER_WRONG,/// 服务器错误
	STOCK_GOLDLIST_MAX,/// 股票卖单满
	STOCK_MONEYLIST_MAX,/// 股票卖单满
};

#define RETUEN_PASSWORD_STOCKPARA 2
struct stReturnPasswordStockIserCmd : public stStockBillUserCmd
{
	stReturnPasswordStockIserCmd()
	{
		byParam = RETUEN_PASSWORD_STOCKPARA;
	}
	BYTE byReturn;  ///密码设置返回类型
};
/// 请求股票帐号余额
#define QUERY_FUND_STOCKPARA 3
struct stQueryFundStockUserCmd : public stStockBillUserCmd
{
	stQueryFundStockUserCmd()
	{
		byParam = QUERY_FUND_STOCKPARA;
	}
};

/// 返回当前股票帐号余额
#define RETURN_FUND_STOCKPARA 4 
struct stReturnFundStockUserCmd : public stStockBillUserCmd
{
	stReturnFundStockUserCmd()
	{
		byParam = RETURN_FUND_STOCKPARA;
	}
	DWORD dwGold;/// 当前帐号金子
	DWORD dwMoney;/// 当前帐号银币
};

/// 向股票帐号注入资金
#define TRANSFER_FUND_SAVE_STOCKPARA 5
struct stTransferFundStockSaveUserCmd : public stStockSceneUserCmd
{
	stTransferFundStockSaveUserCmd()
	{
		byParam = TRANSFER_FUND_SAVE_STOCKPARA;
	}
	DWORD dwGold;/// 操作金子数量
	DWORD dwMoney;/// 操作银币数量
};

/// 从股票帐号提取资金
#define TRANSFER_FUND_FETCH_STOCKPARA 6 
struct stTransferFundStockFetchUserCmd : public stStockBillUserCmd
{
	stTransferFundStockFetchUserCmd()
	{
		byParam = TRANSFER_FUND_FETCH_STOCKPARA;
	}
	DWORD dwGold;/// 操作金子数量
	DWORD dwMoney;/// 操作银币数量
};

/// 委托卖单
#define CONSIGN_GOLD_STOCKPARA 7 
struct stConsignGoldStockUserCmd : public stStockBillUserCmd
{
	stConsignGoldStockUserCmd()
	{
		byParam = CONSIGN_GOLD_STOCKPARA;
	}
	DWORD dwNum;/// 金子数量
};

/// 委托买单
#define CONSIGN_MONEY_STOCKPARA 8
struct stConsignMoneyStockUserCmd : public stStockBillUserCmd
{
	stConsignMoneyStockUserCmd()
	{
		byParam = CONSIGN_MONEY_STOCKPARA;
	}
	DWORD dwNum;/// 银币数量
};
/// 撤销卖单
#define CONSIGN_CANCEL_GOLD_STOCKPARA 9
struct stConsignCancelGoldStockUserCmd : public stStockBillUserCmd
{
	stConsignCancelGoldStockUserCmd() 
	{
		byParam = CONSIGN_CANCEL_GOLD_STOCKPARA;
	}
	DWORD dwNum;/// 卖单流水号
};

/// 撤销买单
#define CONSIGN_CANCEL_MONEY_STOCKPARA 10
struct stConsignCancelMoneyStockUserCmd : public stStockBillUserCmd
{
	stConsignCancelMoneyStockUserCmd() 
	{
		byParam = CONSIGN_CANCEL_MONEY_STOCKPARA;
	}
	DWORD dwNum;/// 卖单流水号
};
enum StockType
{
	STOCK_GOLD,
	STOCK_MONEY,
};
struct StockList
{
	DWORD id;           /// 流水号
	DWORD dwNum;        /// 数量
	DWORD dwPrice;  /// 委托价格
	DWORD dwTime;/// 委托时间
};
/// 请求未成交委托单
#define REQUEST_CONSIGN_LIST_STOCKPARA 11
struct stRequestConsignListStockUserCmd : public stStockBillUserCmd
{
	stRequestConsignListStockUserCmd()
	{
		byParam = REQUEST_CONSIGN_LIST_STOCKPARA;
	}
};

/// 未成交的金子委托单
#define CONSIGN_GOLD_LIST_STOCKPARA 12
struct stConsignGoldListStockUserCmd : public stStockBillUserCmd
{
	stConsignGoldListStockUserCmd()
	{
		byParam = CONSIGN_GOLD_LIST_STOCKPARA;
	}
	DWORD size;
	StockList list[0];
};
/// 未成交的银币委托单
#define CONSIGN_MONEY_LIST_STOCKPARA 13
struct stConsignMoneyListStockUserCmd : public stStockBillUserCmd
{
	stConsignMoneyListStockUserCmd()
	{
		byParam = CONSIGN_MONEY_LIST_STOCKPARA;
	}
	DWORD size;
	StockList list[0];
};
/// 请求撤单
#define REQUEST_CANCEL_LIST_STOCKPARA 14
struct stRequestCancelListStockUserCmd : public stStockBillUserCmd
{
	stRequestCancelListStockUserCmd()
	{
		byParam = REQUEST_CANCEL_LIST_STOCKPARA;
	}
	DWORD id;
	BYTE byType;
};
enum
{
	STOCK_CANCEL_ERROR,
	STOCK_CANCEL_OK,
};
/// 应答撤单
#define RETURN_CANCEL_LIST_STOCKPARA 15
struct stReturnCancelListStockUserCmd : public stStockBillUserCmd
{
	stReturnCancelListStockUserCmd()
	{
		byParam = RETURN_CANCEL_LIST_STOCKPARA;
	}
	DWORD id;
	BYTE byType;
	BYTE byReturn;/*0表示失败,1表示成功*/
};
/// 请求排行前10位的单子
#define REQUEST_FIRSTTEN_LIST_STOCKPARA 16
struct stRequestFirstTenListStockUserCmd : public stStockBillUserCmd
{
	stRequestFirstTenListStockUserCmd()
	{
		byParam = REQUEST_FIRSTTEN_LIST_STOCKPARA;
	}
};

struct FirstTen
{
	DWORD dwNum;
	DWORD dwPrice;
};
/// 排行前5位的价格和数量
#define FIRSTFIVE_LIST_STOCKPARA 17
struct stFirstFiveListStockUserCmd : public stStockBillUserCmd
{
	stFirstFiveListStockUserCmd()
	{
		byParam = FIRSTFIVE_LIST_STOCKPARA;
	}
	BYTE byType;//买或卖
	DWORD size;
	FirstTen list[0];
};

/// 提交委托单
#define PUT_LIST_STOCKPARA 18
struct stPutListStockUserCmd : public stStockBillUserCmd
{
	stPutListStockUserCmd()
	{
		byParam = PUT_LIST_STOCKPARA;
	}
	DWORD dwNum;    //金子或者银币数量
	DWORD dwPrice;  //单价(银币/金子)
	BYTE  byType;   //单子类型,金子或银币
};
/// 返回提交的委托单
#define RETURN_PUT_LIST_STOCKPARA 19
struct stReturnPutListStockUserCmd : public stStockBillUserCmd
{
	stReturnPutListStockUserCmd()
	{
		byParam = RETURN_PUT_LIST_STOCKPARA;
	}
	StockList list;
};
//股票历史数据结构
struct ConsignHistoryType
{
	ConsignHistoryType()
	{
		dwTime=0;
		dwPrice=0;
		dwTotal=0;
	}
	DWORD dwTime;
	WORD dwPrice;
	WORD dwTotal;
};
/// 玩家请求大盘历史交易数据
#define REQUEST_HISTORY_STOCKPARA 20
struct stRequstHistoryStockUserCmd : public stStockBillUserCmd
{
	stRequstHistoryStockUserCmd()
	{
		byParam = REQUEST_HISTORY_STOCKPARA;
		begintime=0;
		num=0;
	}
	DWORD begintime;
	DWORD num;
};
/// 返回最新大盘历史数据(当前一分钟的统计数据)
#define NEW_HISTORY_STOCKPARA 21
struct stNewHistoryStockUserCmd : public stStockBillUserCmd
{
	stNewHistoryStockUserCmd()
	{
		byParam = NEW_HISTORY_STOCKPARA;
	}
	ConsignHistoryType gold;
	ConsignHistoryType money;
};
/// 返回大盘历史金子数据(不定长)
#define HISTORY_GOLD_STOCKPARA 22
struct stHistoryGoldStockUserCmd : public stStockBillUserCmd
{
	stHistoryGoldStockUserCmd()
	{
		byParam = HISTORY_GOLD_STOCKPARA;
	}
	DWORD size;
	ConsignHistoryType list[0];
};
/// 返回大盘历史银币数据(不定长)
#define HISTORY_MONEY_STOCKPARA 23
struct stHistoryMoneyStockUserCmd : public stStockBillUserCmd
{
	stHistoryMoneyStockUserCmd()
	{
		byParam = HISTORY_MONEY_STOCKPARA;
	}
	DWORD size;
	ConsignHistoryType list[0];
};
/// 玩家请求自己历史交易数据
#define REQUEST_SELF_HISTORY_STOCKPARA 24
struct stRequstSelfHistoryStockUserCmd : public stStockBillUserCmd
{
	stRequstSelfHistoryStockUserCmd()
	{
		byParam = REQUEST_SELF_HISTORY_STOCKPARA;
		begintime=0;
		num=0;
	}
	DWORD begintime;
	DWORD num;
};
//个人委托单历史数据
struct ConsignSelfHistoryType
{
	ConsignSelfHistoryType()
	{       
	}       
	DWORD dwID;
	DWORD wdNum;
	DWORD wdCommitPrice;
	DWORD wdPrice;
	DWORD dwCommitTime;
	DWORD dwOkTime;
};
/// 返回自己历史金子数据(不定长)
#define SELF_HISTORY_GOLD_STOCKPARA 25
struct stSelfHistoryGoldStockUserCmd : public stStockBillUserCmd
{
	stSelfHistoryGoldStockUserCmd()
	{
		byParam = SELF_HISTORY_GOLD_STOCKPARA;
	}
	DWORD size;
	ConsignSelfHistoryType list[0];
};
/// 返回自己历史银币数据(不定长)
#define SELF_HISTORY_MONEY_STOCKPARA 26
struct stSelfHistoryMoneyStockUserCmd : public stStockBillUserCmd
{
	stSelfHistoryMoneyStockUserCmd()
	{
		byParam = SELF_HISTORY_MONEY_STOCKPARA;
	}
	DWORD size;
	ConsignSelfHistoryType list[0];
};

/// 股票系统停止服务
#define STOP_SERVICE_STOCKPARA 27
struct stStopServiceStockUserCmd : public stStockBillUserCmd
{
	stStopServiceStockUserCmd()
	{
		byParam = STOP_SERVICE_STOCKPARA;
	}
};

/// 清除未成交委托单
#define CONSIGN_CLEAN_LIST_STOCKPARA 28
struct stConsignCleanListStockUserCmd : public stStockBillUserCmd
{
	stConsignCleanListStockUserCmd() 
	{
		byParam = CONSIGN_CLEAN_LIST_STOCKPARA;
	}
};

/// 请求得到当前用户的激活和登陆状态
#define REQUEST_STATE_STOCKPARA 29
struct stRequestStateStockUserCmd : public stStockBillUserCmd
{
	stRequestStateStockUserCmd() 
	{
		byParam = REQUEST_STATE_STOCKPARA;
	}
	bool  active;
	bool  login;
};

//////////////////////////////////////////////////////////////
/// 定义股票命令结束
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
// 军队指令定义开始
//////////////////////////////////////////////////////////////
struct stArmyUserCmd : public stNullUserCmd
{
	stArmyUserCmd()
	{
		byCmd = ARMY_USERCMD;           
	}
};

// 创建军队
#define CREATE_ARMY_PARA 1
struct stCreateArmyUserCmd : public stArmyUserCmd
{
	stCreateArmyUserCmd()
	{
		byParam = CREATE_ARMY_PARA;
		bzero(armyName, sizeof(armyName));
		bzero(genName, sizeof(genName));
	}

	char armyName[8+1]; // 军队名称限定为4个汉字
	char genName[MAX_NAMESIZE];
};     
// 请求待选将军列表
#define REQ_WAIT_GEN_PARA 2
struct stReqWaitGenUserCmd : public stArmyUserCmd
{
	stReqWaitGenUserCmd()
	{
		byParam = REQ_WAIT_GEN_PARA;
	}
};
struct stWaitGenItem
{
	DWORD dwCharID;
	char  szName[MAX_NAMESIZE];
};

// 返回候选将军列表
#define RTN_WAIT_GEN_PARA 3
struct stRtnWaitGenUserCmd : public stArmyUserCmd
{
	stRtnWaitGenUserCmd()
	{
		byParam = REQ_WAIT_GEN_PARA;
		dwSize = 0;
	}

	DWORD dwSize;
	stWaitGenItem data[0];
};

enum
{
	QUESTION_ARMY_GEN, // 询问是否愿意成为将军
	YES_ARMY_GEN, // 同意成为将军
	NO_ARMY_GEN, // 拒绝成为将军
};

// 询问是否同意成为将军
#define REQ_ARMY_GEN_PARA 4
struct stReqArmyGenUserCmd : public stArmyUserCmd
{
	stReqArmyGenUserCmd()
	{
		byParam = REQ_ARMY_GEN_PARA;
		byStatus = QUESTION_ARMY_GEN;
	}

	BYTE byStatus;

};

enum
{
	COUNTRY_ARMY_LIST, // 用于国家对话框，显示全国军队列表
	CITY_ARMY_LIST,  // 用于NPC对话，显示本城军队列表
};
// 请求军队列表
#define REQ_ARMY_LIST_PARA 5
struct stReqArmyListUserCmd : public stArmyUserCmd
{
	stReqArmyListUserCmd()
	{
		byParam = REQ_ARMY_LIST_PARA;
		byType = COUNTRY_ARMY_LIST;
	}

	BYTE byType;
};

struct stArmyBaseInfo
{
	DWORD dwArmyID;
	char  name[MAX_NAMESIZE]; // 军队名称
	char  cityname[MAX_NAMESIZE]; // 所属城市
	char  genname[MAX_NAMESIZE]; // 将军姓名
	DWORD dwCapNum; // 队长人数
};
// 返回军队列表
#define RTN_ARMY_LIST_PARA 6
struct stRtnArmyListUserCmd : public stArmyUserCmd
{
	stRtnArmyListUserCmd()
	{
		byParam = RTN_ARMY_LIST_PARA;
		dwSize = 0;
	}

	DWORD dwSize;
	BYTE  byType;
	stArmyBaseInfo data[0];
};

// 请求军队详细信息
#define REQ_ARMY_SPEC_PARA 7
struct stReqArmySpecUserCmd : public stArmyUserCmd
{
	stReqArmySpecUserCmd()
	{
		byParam = REQ_ARMY_SPEC_PARA;
		dwArmyID = 0;
	}

	DWORD dwArmyID;
};

struct stCaptainBase
{
	char name[MAX_NAMESIZE];
	DWORD dwCharID;
};
// 返回军队详细信息
#define RTN_ARMY_SPEC_PARA 8
struct stRtnArmySpecUserCmd : public stArmyUserCmd
{
	stRtnArmySpecUserCmd()
	{
		byParam = RTN_ARMY_SPEC_PARA;
		dwSize = 0;
	}

	char name[MAX_NAMESIZE]; // 军队名
	char cityname[MAX_NAMESIZE]; // 城市名
	char genname[MAX_NAMESIZE]; // 将军名
	DWORD dwSize;
	stCaptainBase data[0];
};

// 离开军队
#define EXIT_ARMY_PARA 9
struct stExitArmyUserCmd : public stArmyUserCmd
{
	stExitArmyUserCmd()
	{
		byParam = EXIT_ARMY_PARA;
	}
};        

// 解散军队
#define REMOVE_ARMY_PARA 10
struct stRemoveArmyUserCmd : public stArmyUserCmd
{
	stRemoveArmyUserCmd()
	{
		byParam = REMOVE_ARMY_PARA;
		dwArmyID = 0;
	}

	DWORD dwArmyID;
};

// 更改军队名称
#define CHANGE_ARMY_NAME_PARA 11
struct stChangeArmyNameUserCmd : public stArmyUserCmd
{
	stChangeArmyNameUserCmd()
	{
		byParam = CHANGE_ARMY_NAME_PARA;
		dwArmyID;
	}       

	DWORD dwArmyID;
	char  newArmyName[MAX_NAMESIZE];
};
enum ADD_ARMY_CAPTAIN{
	QUESTION_CAPTAIN, // 发出询问
	ANSWER_CAPTAIN_YES, // 确认邀请
	ANSWER_CAPTAIN_NO, // 拒绝邀请
};
// 招收队长
#define ADD_ARMY_CAPTAIN_PARA 12
struct stAddArmyCaptainUserCmd : public stArmyUserCmd
{
	stAddArmyCaptainUserCmd()
	{
		byParam = ADD_ARMY_CAPTAIN_PARA;
		bzero(capName, MAX_NAMESIZE);
		bzero(armyName, MAX_NAMESIZE);
		capID = 0;
		byState = QUESTION_CAPTAIN;
	}

	char capName[MAX_NAMESIZE]; // 被邀请的人
	char armyName[MAX_NAMESIZE]; // 邀请的军队名称
	DWORD armyID;   // 军队ID
	DWORD capID; // 被邀请人ID
	BYTE byState; // 参见 enum ADD_ARMY_CAPTAIN
};
// 开除队长
#define FIRE_ARMY_CAPTAIN_PARA 13
struct stFireArmyCaptainUserCmd : public stArmyUserCmd
{
	stFireArmyCaptainUserCmd()
	{
		byParam = FIRE_ARMY_CAPTAIN_PARA;
		dwUserID = 0;
	}

	DWORD dwUserID; // 待开除的队长ID,通过军队详细信息获得
};
//////////////////////////////////////////////////////////////
// 军队指令定义结束
//////////////////////////////////////////////////////////////


// 护宝任务指令
const BYTE GEM_USERCMD                  = 42;
//////////////////////////////////////////////////////////////
// 护宝指令定义开始
//////////////////////////////////////////////////////////////
struct stGemUserCmd : public stNullUserCmd
{
	stGemUserCmd()
	{
		byCmd = GEM_USERCMD;
	}
};

// 领取龙精
#define REQUEST_DRAGON_PARA 1
struct stReqDragonUserCmd : public stGemUserCmd
{
	stReqDragonUserCmd()
	{
		byParam = REQUEST_DRAGON_PARA;
	}
};

// 领取虎魄
#define REQUEST_TIGER_PARA 2
struct stReqTigerUserCmd : public stGemUserCmd
{
	stReqTigerUserCmd()
	{
		byParam = REQUEST_TIGER_PARA;
	}
};

//////////////////////////////////////////////////////////////
// 护宝指令定义结束
//////////////////////////////////////////////////////////////



//BEGIN_ONE_CMD
//////////////////////////////////////////////////////////////
/// 定义监狱命令开始
//////////////////////////////////////////////////////////////

struct stPrisonUserCmd : public stNullUserCmd
{
	stPrisonUserCmd()
	{
		byCmd = PRISON_USERCMD;
	}
};

//请求出狱
const BYTE OUT_PRISON_PARA = 1;
struct stOutPrison : public stPrisonUserCmd
{
	stOutPrison()
	{
		byParam = OUT_PRISON_PARA;
	}
};

//贿赂
const BYTE BRIBE_PRISON_PARA = 2;
struct stBribePrison : public stPrisonUserCmd
{
	DWORD money;
	stBribePrison()
	{
		byParam = BRIBE_PRISON_PARA;

		money = 0;
	}
};

//取保就医(帮别人洗PK值)
const BYTE BAIL_PRISON_PARA = 3;
struct stBailPrison : public stPrisonUserCmd
{
	char name[MAX_NAMESIZE];
	DWORD money;
	stBailPrison()
	{
		byParam = BAIL_PRISON_PARA;

		bzero(name, sizeof(name));
		money = 0;
	}
};

//请求出牢狱
const BYTE LEAVE_PRISON_PARA = 4;
struct stLeavePrison : public stPrisonUserCmd
{
	stLeavePrison()
	{
		byParam = LEAVE_PRISON_PARA;
	}
};
//////////////////////////////////////////////////////////////
/// 定义监狱命令结束
//////////////////////////////////////////////////////////////

//BEGIN_ONE_CMD
//////////////////////////////////////////////////////////////
/// 定义礼官命令开始
//////////////////////////////////////////////////////////////

struct stGiftUserCmd : public stNullUserCmd
{       
	stGiftUserCmd()
	{       
		byCmd = GIFT_USERCMD;
	}
};

//请求活动列表
const BYTE GET_LIST_GIFT_PARA = 1;
struct stGetListGift : public stGiftUserCmd
{       
	stGetListGift()
	{       
		byParam = GET_LIST_GIFT_PARA;
	}
};

struct actTitle
{       
	DWORD id;
	char name[MAX_NAMESIZE];
	BYTE state;
};

//发送活动列表
const BYTE LIST_GIFT_PARA = 2;
struct stListGift : public stGiftUserCmd
{       
	DWORD num;
	actTitle titles[0];
	stListGift()
	{       
		byParam = LIST_GIFT_PARA;
		num = 0;
	}
};

//请求活动内容
const BYTE GET_DETAIL_GIFT_PARA = 3;
struct stGetDetailGift : public stGiftUserCmd
{       
	DWORD id;
	stGetDetailGift()
	{       
		byParam = GET_DETAIL_GIFT_PARA;
		id = 0;
	}
};

//发送活动内容
const BYTE DETAIL_GIFT_PARA = 4;
struct stDetailGift : public stGiftUserCmd
{       
	DWORD id;
	char text[MAX_CHATINFO];
	stDetailGift()
	{       
		byParam = DETAIL_GIFT_PARA;
		id = 0;
		bzero(text, sizeof(text));
	}
};

//领取奖品 
const BYTE GET_ITEM_GIFT_PARA = 5;
struct stGetItemGift : public stGiftUserCmd
{       
	DWORD id;
	stGetItemGift()
	{       
		byParam = GET_ITEM_GIFT_PARA;
		id = 0;
	}
};

//请求获奖名单
const BYTE GET_WINNER_GIFT_PARA = 6;
struct stGetWinnerGift : public stGiftUserCmd
{
	DWORD id;//活动ID
	stGetWinnerGift()
	{
		byParam = GET_WINNER_GIFT_PARA;
		id = 0;
	}
};

//发送获奖名单
const BYTE WINNER_GIFT_PARA = 7;
struct stWinnerGift : public stGiftUserCmd
{
	DWORD num;
	char winners[0];
	stWinnerGift()
	{
		byParam = WINNER_GIFT_PARA;
		num = 0;
	}
};

//////////////////////////////////////////////////////////////
/// 定义礼官命令结束
//////////////////////////////////////////////////////////////


//BEGIN_ONE_CMD
//////////////////////////////////////////////////////////////  
/// 定义国家联盟命令开始
//////////////////////////////////////////////////////////////

struct stAllyUserCmd : public stNullUserCmd
{       
	stAllyUserCmd()
	{
		byCmd = ALLY_USERCMD;   
	}
};      

enum    
{       
	QUESTION_COUNTRY_ALLY, // 发起请求
	YES_COUNTRY_ALLY, // 同意请求   
	NO_COUNTRY_ALLY, // 拒绝请求    
};  


//请求结盟
#define REQ_COUNTRY_ALLY_PARA  1
struct stReqCountryAlly : public stAllyUserCmd
{       
	stReqCountryAlly()
	{
		byParam = REQ_COUNTRY_ALLY_PARA;
		dwCountryID = 0;
		dwAllyCountryID = 0;
		byStatus = QUESTION_COUNTRY_ALLY;
	}

	DWORD dwCountryID; // 发起请求的国家,发起时不用填写
	DWORD dwAllyCountryID; // 被邀请的国家
	BYTE  byStatus;
};

// 设置盟国信息
#define UPDATE_COUNTRY_ALLY_PARA 2
struct stUpdateCountryAlly : public stAllyUserCmd
{
	stUpdateCountryAlly()
	{
		byParam = UPDATE_COUNTRY_ALLY_PARA;
		dwAllyCountryID = 0;
	}

	DWORD dwAllyCountryID;
};

// 取缔盟约
#define CANCEL_COUNTRY_ALLY_PARA 3
struct stCancelCountryAlly : public stAllyUserCmd
{
	stCancelCountryAlly()
	{
		byParam = CANCEL_COUNTRY_ALLY_PARA;
		dwAllyCountryID = 0;
	}

	DWORD dwAllyCountryID;
};

// 请求国家联盟信息
#define REQ_COUNTRY_ALLY_INFO_PARA 4
struct stReqCountryAllyInfo : public stAllyUserCmd
{
	stReqCountryAllyInfo()
	{
		byParam = REQ_COUNTRY_ALLY_INFO_PARA;
	}
};


// 返回国家联盟信息
#define RTN_COUNTRY_ALLY_INFO_PARA 5
struct stRtnCountryAllyInfo : public stAllyUserCmd
{
	stRtnCountryAllyInfo()
	{
		byParam = RTN_COUNTRY_ALLY_INFO_PARA;
		dwAllyCountryID = 0;
		dwFriendDegree = 0;
	}

	DWORD dwAllyCountryID;
	DWORD dwFriendDegree;
};

// 国王指定关监狱
#define SEND_PRISON_COUNTRY_PARA  34
struct stSendPrisonCountryCmd : public stCountryUserCmd
{
	stSendPrisonCountryCmd()
	{
		byParam = SEND_PRISON_COUNTRY_PARA;
	}
	char name[MAX_NAMESIZE];//指定玩家名字
};

// 皇帝处罚玩家
#define EMPEROR_PUNISH_COUNTRY_PARA  35
struct stEmperorPunishCountryCmd : public stCountryUserCmd
{
	stEmperorPunishCountryCmd()
	{
		byParam = EMPEROR_PUNISH_COUNTRY_PARA;
	}
	char name[MAX_NAMESIZE];//指定玩家名字
	DWORD method;//处罚方式 1:禁言 2:关监狱
};
//////////////////////////////////////////////////////////////////////////
//返回帮会名称和家族名称等..start.
//////////////////////////////////////////////////////////////////////////
//add by wyc for opt sevr 0603211138
/// 发送当前合并版本号
const BYTE MERGE_VERSION_CHECK_USERCMD_PARA = 53;
struct stMergeVersionCheckUserCmd : public stDataUserCmd
{
	stMergeVersionCheckUserCmd()
	{
		byParam = MERGE_VERSION_CHECK_USERCMD_PARA;
		dwMergeVersion = 0;
	}

	DWORD dwMergeVersion;
};
// 请求帮会名称
const BYTE REQ_UNION_NAME_PARA = 27;    
struct stReqUnionNameCmd : public stUnionUserCmd
{       
	stReqUnionNameCmd()
	{
		byParam = REQ_UNION_NAME_PARA;
		dwUnionID = 0;
	}

	DWORD dwUnionID;                
};

// 返回帮会名称
const BYTE RTN_UNION_NAME_PARA = 28;    
struct stRtnUnionNameCmd : public stUnionUserCmd
{       
	stRtnUnionNameCmd()
	{
		byParam = RTN_UNION_NAME_PARA;
		bzero(name, MAX_NAMESIZE);
		dwUnionID = 0;
	}

	DWORD dwUnionID;
	char name[MAX_NAMESIZE];
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//..end
//////////////////////////////////////////////////////////////////////////
/*
//add by wyc

//////////////////////////////////////////////////////////////////////////

//BEGIN_ONE_CMD
//BEGIN_ONE_CMD
//////////////////////////////////////////////////////////////
/// 定义小游戏命令开始
//////////////////////////////////////////////////////////////

struct stMiniGameUserCmd : public stNullUserCmd
{
	stMiniGameUserCmd()
	{
		byCmd = MINIGAME_USERCMD;
	}
};

// 邀请
#define INVITE_MINI_PARA 1
struct stInviteMiniGame : public stMiniGameUserCmd
{
	stInviteMiniGame()
	{
		byParam = INVITE_MINI_PARA;
	}
	DWORD tempid;//邀请者/被邀请者的临时ID
	DWORD money;//赌注
};

// 邀请回复
#define INVITE_RET_MINI_PARA 2
struct stInviteRetMiniGame : public stMiniGameUserCmd
{
	stInviteRetMiniGame()
	{
		byParam = INVITE_RET_MINI_PARA;
	}
	DWORD tempid;//邀请者的临时ID
	DWORD money;//赌注
	DWORD ret;//1:同意 0:不同意
};

// 开始游戏
#define START_MINI_PARA 3
struct stStartMiniGame : public stMiniGameUserCmd
{
	stStartMiniGame()
	{   
		byParam = START_MINI_PARA;
	}
	DWORD tempid;//对方的临时ID
	DWORD money;//赌注
};

// 停止色子
#define STOP_DICE_MINI_PARA 4
struct stStopDiceMiniGame : public stMiniGameUserCmd
{   
	stStopDiceMiniGame()
	{   
		byParam = STOP_DICE_MINI_PARA;
	}
};

// 色子数字
#define DICE_NUM_MINI_PARA 5
struct stDiceNumMiniGame : public stMiniGameUserCmd
{   
	stDiceNumMiniGame()
	{   
		byParam = DICE_NUM_MINI_PARA;
	}
	DWORD tempid;//掷色子人的临时ID
	DWORD num;
};

// 结果
#define RESULT_MINI_PARA 6
struct stResultMiniGame : public stMiniGameUserCmd
{   
	stResultMiniGame()
	{   
		byParam = RESULT_MINI_PARA;
	}
	DWORD res;//0:输 1:赢 2:平
};

// 游戏结束 //主动离开游戏
#define END_MINI_PARA 7
struct stEndMiniGame : public stMiniGameUserCmd
{
	stEndMiniGame()
	{
		byParam = END_MINI_PARA;

		reason = 0;
	}
	DWORD reason;//结束原因 0:正常结束 1:对方离开
};

// 继续游戏
#define CONTINUE_MINI_PARA 8
struct stContinueMiniGame : public stMiniGameUserCmd
{
	stContinueMiniGame()
	{
		byParam = CONTINUE_MINI_PARA;
	}
	DWORD ret;//0:不继续 1:继续
};

// 通知客户端进入游戏
#define ENTER_MINI_PARA 9
struct stEnterMiniGame : public stMiniGameUserCmd
{
	stEnterMiniGame()
	{
		byParam = ENTER_MINI_PARA;
	}
};

//////////////////////////////////////////////////////////////
/// 定义小游戏命令结束
//////////////////////////////////////////////////////////////
*/
 
//BEGIN_ONE_CMD
//////////////////////////////////////////////////////////////
/// 定义推荐人系统命令开始
//////////////////////////////////////////////////////////////
 

struct stRecommendUserCmd : public stNullUserCmd
{
	stRecommendUserCmd()
	{
		byCmd = RECOMMEND_USERCMD;
	}
};

// 填写自己的推荐人, >10 && <=15级的玩家才能填写
#define SET_RECOMMEND_PARA 1
struct stSetRecommendUserCmd : public stRecommendUserCmd
{
	stSetRecommendUserCmd()
	{
		byParam = SET_RECOMMEND_PARA;
		bzero(name, MAX_NAMESIZE);
	}

	char name[MAX_NAMESIZE];
};
enum
{
	QUERY_BOUNTY, // 查询当前奖励金
	GET_BOUNTY,   // 提取当前奖励金
};

// 推荐人查询或领取奖励
#define REQ_RECOMMEND_BOUNTY_PARA 2
struct stReqRecommendBountyUserCmd : public stRecommendUserCmd
{
	stReqRecommendBountyUserCmd()
	{
		byParam = REQ_RECOMMEND_BOUNTY_PARA;
	}

	BYTE byState;
};

// 被推荐人查询或领取奖励
#define REQ_RECOMMENDED_BOUNTY_PARA 3
struct stReqRecommendedBountyUserCmd : public stRecommendUserCmd
{
	stReqRecommendedBountyUserCmd()
	{
		byParam = REQ_RECOMMENDED_BOUNTY_PARA;
	}

	BYTE byState;
};

struct stRecommendItem
{
	char name[MAX_NAMESIZE]; // 被推荐人姓名
	DWORD dwLevel;  // 最后一次提取时的等级
	DWORD dwTotal;  // 累计贡献多少金额
};

// 返回推荐人奖励
#define RTN_RECOMMEND_BOUNTY_PARA 4
struct stRtnRecommendBountyUserCmd : public stRecommendUserCmd
{
	stRtnRecommendBountyUserCmd()
	{
		byParam = RTN_RECOMMEND_BOUNTY_PARA;
		dwBalance = 0;
		dwTotal = 0;
		dwSize = 0;
	}

	DWORD dwBalance; // 可领取的金额
	DWORD dwTotal;  // 累计已领取的金额
	DWORD dwSize;   // 被推荐人数目, 有可能有很多,如果超过窗口大小,能不能做成可以拉动
	stRecommendItem data[0];
};

// 返回被推荐人奖励
#define RTN_RECOMMENDED_BOUNTY_PARA 5
struct stRtnRecommendedBountyUserCmd : public stRecommendUserCmd
{
	stRtnRecommendedBountyUserCmd()
	{
		byParam = RTN_RECOMMENDED_BOUNTY_PARA;
		dwBalance = 0;
		dwTotal = 0;
		dwLastLevel = 0;
		bzero(name, sizeof(name));
	}

	DWORD dwBalance; // 可领取的余额
	DWORD dwTotal;   // 累计已领取的金额
	DWORD dwLastLevel; // 上次领取时的等级
	char name[MAX_NAMESIZE]; // 推荐人姓名
};
//////////////////////////////////////////////////////////////
/// 定义推荐人系统命令结束
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
/// 定义财产保护系统指令开始
//////////////////////////////////////////////////////////////
struct stSafetyUserCmd : public stNullUserCmd
{
	stSafetyUserCmd()
	{
		byCmd = SAFETY_USERCMD;
	}
};

// 玩家设置保护状态
#define SET_SAFETY_PARA 1
struct stSetSafetyUserCmd : public stSafetyUserCmd
{
	stSetSafetyUserCmd()
	{
		byParam = SET_SAFETY_PARA;
		pwd = 0;
		state = 0;
	}

	DWORD pwd; // 数字密码
	BYTE  state; // 保护状态,0为关闭,1为开启,两个操作都需要输入数字密码
};

enum
{
	SAFETY_OPEN, // 保护状态,该状态下,不能进行拆装备操作
	SAFETY_TEMP_CLOSE, // 本次登录,暂时关闭状态,可以进行任何操作
};
// 玩家上线时,服务器通知客户端,玩家是否在保护状态
#define NOTIFY_SAFETY_PARA 2
struct stNotifySafetyUserCmd : public stSafetyUserCmd
{
	stNotifySafetyUserCmd()
	{
		byParam = NOTIFY_SAFETY_PARA;
		safe_setup = 0;
		byState = 0;
	}

	BYTE byState;
	BYTE safe_setup;
};

// 玩家进入临时未保护状态,只在一次登录中有效,小退和关客户端后,无效
#define SET_TEMP_UNSAFETY_PARA 3
struct stSetTempUnSafetyUserCmd : public stSafetyUserCmd
{
	stSetTempUnSafetyUserCmd()
	{    
		byParam = SET_TEMP_UNSAFETY_PARA;
		pwd = 0; 
	}    

	DWORD pwd; 
};


enum
{
	SAFE_SPLIT_OBJECT, // 拆装备时,需要验证
	SAFE_THIRD_PACK,   // 从第三个包裹中拿出物品,需要验证
	SAFE_GOLD_STOCK,   // 金币购买股票
};
// 设置需要保护的功能
// 用,set_state,clear_state,来操作
#define SET_SAFETY_DETAIL_PARA 4
struct stSetSafetyDetailUserCmd : public stSafetyUserCmd
{
	stSetSafetyDetailUserCmd()
	{
		byParam = SET_SAFETY_DETAIL_PARA;
		safe_setup=0;
		pwd = 0;
	}

	DWORD pwd;
	BYTE safe_setup;
};

//////////////////////////////////////////////////////////////
/// 定义财产保护系统指令结束
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
/// 定义太庙资源掠夺系统指令开始
//////////////////////////////////////////////////////////////
struct LoongUserCmd : public stNullUserCmd
{
LoongUserCmd()
{
byCmd =LOONG_USERCMD;
}
};

///资源的品质类型枚举
enum LoongPearlType 
{
    LOONG_PEARL_NONE    = 0,
    LOONG_PEARL_WHITE   = 1,       //白色品质的砖块
    LOONG_PEARL_BLUE    = 2,
    LOONG_PEARL_YELLOW  = 3,
    LOONG_PEARL_GREEN   = 4,
    LOONG_PEARL_PURPLE  = 5,
};

/// 玩家请求从太庙中夺取砖块
const BYTE REQ_FETCH_LOONGPEARL_PARA = 1;
struct ReqFetchLoongPearlLoongUserCmd : public LoongUserCmd
{
    ReqFetchLoongPearlLoongUserCmd()
    {
        byParam = REQ_FETCH_LOONGPEARL_PARA;
    }
};

/// 回复玩家夺取砖块的结果
const BYTE REPLY_FETCH_LOONGPEARL_PARA = 2;
struct ReplyFetchLoongPearlLoongUserCmd : public LoongUserCmd
{
    ReplyFetchLoongPearlLoongUserCmd()
    {
        byParam = REPLY_FETCH_LOONGPEARL_PARA;
        pearlType = LOONG_PEARL_NONE;
    }
    BYTE pearlType;
};

/// 玩家请求把砖块放入太庙
const BYTE REQ_PUT_LOONGPEARL_PARA = 3;
struct ReqPutLoongPearlLoongUserCmd : public LoongUserCmd
{
    ReqPutLoongPearlLoongUserCmd()
    {
        byParam = REQ_PUT_LOONGPEARL_PARA;
    }
};

/// 玩家请求世界太庙列表
const BYTE REQ_LOONGALTAR_LIST_PARA= 4;
struct ReqAltarListLoongUserCmd : public LoongUserCmd
{
    ReqAltarListLoongUserCmd()
    {
        byParam = REQ_LOONGALTAR_LIST_PARA;
    }
};

/// 玩家请求从太庙中更换砖块
const BYTE REQ_GENGHUAN_LOONGPEARL_PARA = 5;
struct ReqGenghuanLoongPearlLoongUserCmd : public LoongUserCmd
{
    ReqGenghuanLoongPearlLoongUserCmd()
    {
        byParam = REQ_GENGHUAN_LOONGPEARL_PARA;
    }
};

struct LoongAltarInfo 
{
	char    counName[MAX_NAMESIZE];
	DWORD   pearlNum;
	BYTE    loongAltarEff;      //太庙效果
	DWORD   robNum;             //掠夺请求者国家砖块的数量
	LoongAltarInfo()
	{
		bzero(counName, sizeof(counName));
		pearlNum = 0;
		loongAltarEff = 0;
		robNum = 0;
	}
};

/// 回复玩家世界太庙列表
const BYTE REPLY_LOONGALTAR_LIST_PARA = 5;
struct ReplyAltarListLoongUserCmd : public LoongUserCmd
{
    ReplyAltarListLoongUserCmd()
    {
        byParam = REPLY_LOONGALTAR_LIST_PARA;
		altarNum = 0;
    }

	DWORD             altarNum;
	LoongAltarInfo    altarList[0];
};

struct ZhanLiAltarInfo 
{
	char    counName[MAX_NAMESIZE];
	DWORD   charid;
	//BYTE    loongAltarEff;      //太庙效果
	//DWORD   robNum;             //掠夺请求者国家砖块的数量
	char  name[MAX_NAMESIZE + 1];		/// 名称
	DWORD   zhanlilNum;           //掠夺请求战力
	DWORD unionid;						/// 帮会ID
	char unionname[MAX_NAMESIZE];
	ZhanLiAltarInfo()
	{
		bzero(counName, sizeof(counName));
		bzero(name, sizeof(name));
		bzero(unionname, sizeof(unionname));
		//pearlNum = 0;
		//loongAltarEff = 0;
		//robNum = 0;
		zhanlilNum =0;
		unionid  = 0;
		charid = 0;
	}
};

/// 玩家请求战力排行列表 add 2023-04-08
const BYTE REQ_ZHANLI_LIST_PARA= 6;
struct ReqAltarListZhanLIUserCmd : public LoongUserCmd
{
    ReqAltarListZhanLIUserCmd()
    {
        byParam = REQ_ZHANLI_LIST_PARA;
    }
};


/// 回复玩家战力排行列表
const BYTE REPLY_ZHANLI_LIST_PARA = 7;
struct ReplyAltarListZhanLIUserCmd : public LoongUserCmd
{
    ReplyAltarListZhanLIUserCmd()
    {
        byParam = REPLY_ZHANLI_LIST_PARA;
		altarNum = 0;
    }

	DWORD             altarNum;
	ZhanLiAltarInfo    altarList[0];
};

/// 玩家请求英雄排行榜
const BYTE REQ_HERO_LIST_PARA= 8;
struct ReqAltarLisHeroUserCmd : public LoongUserCmd
{
	DWORD	countryID;
	BYTE	datevalue;
    ReqAltarLisHeroUserCmd()
    {
        byParam = REQ_HERO_LIST_PARA;
		countryID = 0;
		datevalue = 1;
    }
};

struct HeroAltarInfo 
{
	char	name[MAX_NAMESIZE + 1];		//角色名
	char	unionname[MAX_NAMESIZE];	//帮会名
	DWORD	killnum;           			//击杀数量
	HeroAltarInfo()
	{
		bzero(name, sizeof(name));
		bzero(unionname, sizeof(unionname));
		killnum =0;
	}
};

/// 回复玩家英雄排行榜
const BYTE REPLY_HERO_LIST_PARA = 9;
struct ReplyAltarListHeroUserCmd : public LoongUserCmd
{
    ReplyAltarListHeroUserCmd()
    {
        byParam = REPLY_HERO_LIST_PARA;
		altarNum = 0;
		datevalue = 1;
    }
	BYTE			datevalue;
	DWORD			altarNum;
	HeroAltarInfo	altarList[0];
};

struct LevelupAltarInfo
{
	char    counName[MAX_NAMESIZE];
	DWORD   charid;
	char    name[MAX_NAMESIZE + 1]; //名称
	DWORD   levelupNum; //请求角色等级
	DWORD   unionid; //帮会ID
	char    unionname[MAX_NAMESIZE];
    LevelupAltarInfo()
    {
        bzero(counName, sizeof(counName));
        bzero(name, sizeof(name));
        bzero(unionname, sizeof(unionname));
        charid = 0;
        levelupNum =0;
        unionid  = 0;
    }
};

/// 玩家请求等级排行列表
const BYTE REQ_LEVELUP_LIST_PARA= 10;
struct ReqAltarListLevelupUserCmd : public LoongUserCmd
{
    ReqAltarListLevelupUserCmd()
    {
        byParam = REQ_LEVELUP_LIST_PARA;
    }
};


/// 回复玩家等级排行列表
const BYTE REPLY_LEVELUP_LIST_PARA = 11;
struct ReplyAltarListLevelupUserCmd : public LoongUserCmd
{
    ReplyAltarListLevelupUserCmd()
    {
        byParam = REPLY_LEVELUP_LIST_PARA;
		altarNum = 0;
    }

	DWORD             altarNum;
	LevelupAltarInfo    altarList[0];
};

struct TicketupAltarInfo
{
	char    counName[MAX_NAMESIZE];
	DWORD   charid;
	char  name[MAX_NAMESIZE + 1];		/// 名称
	DWORD   ticketupNum;           //请求积分榜
	DWORD unionid;						/// 帮会ID
	char unionname[MAX_NAMESIZE];
	TicketupAltarInfo()
	{
		bzero(counName, sizeof(counName));
		bzero(name, sizeof(name));
		bzero(unionname, sizeof(unionname));
		ticketupNum =0;
		unionid  = 0;
		charid = 0;
	}
};

/// 玩家请求积分排行列表
const BYTE REQ_TICKETUP_LIST_PARA= 12;
struct ReqAltarListTicketupUserCmd : public LoongUserCmd
{
    ReqAltarListTicketupUserCmd()
    {
        byParam = REQ_TICKETUP_LIST_PARA;
    }
};


/// 回复玩家积分排行列表
const BYTE REPLY_TICKETUP_LIST_PARA = 13;
struct ReplyAltarListTicketupUserCmd : public LoongUserCmd
{
    ReplyAltarListTicketupUserCmd()
    {
        byParam = REPLY_TICKETUP_LIST_PARA;
		altarNum = 0;
    }

	DWORD             altarNum;
	TicketupAltarInfo    altarList[0];
};

struct FlowerAltarInfo
{
	char    counName[MAX_NAMESIZE];
	DWORD   charid;
	char  name[MAX_NAMESIZE + 1];		/// 名称
	DWORD   gender;           //请求性别
	DWORD   flowerNum;           //请求鲜花榜
	DWORD unionid;						/// 帮会ID
	char unionname[MAX_NAMESIZE];
	FlowerAltarInfo()
	{
		bzero(counName, sizeof(counName));
		bzero(name, sizeof(name));
		bzero(unionname, sizeof(unionname));
		flowerNum =0;
		unionid  = 0;
		gender =0;
		charid = 0;
	}
};

/// 玩家请求鲜花排行列表
const BYTE REQ_FLOWER_LIST_PARA= 14;
struct ReqAltarListFlowerUserCmd : public LoongUserCmd
{
    ReqAltarListFlowerUserCmd()
    {
        byParam = REQ_FLOWER_LIST_PARA;
    }
};


/// 回复玩家鲜花排行列表
const BYTE REPLY_FLOWER_LIST_PARA = 15;
struct ReplyAltarListFlowerUserCmd : public LoongUserCmd
{
    ReplyAltarListFlowerUserCmd()
    {
        byParam = REPLY_FLOWER_LIST_PARA;
		altarNum = 0;
    }

	DWORD             altarNum;
	FlowerAltarInfo    altarList[0];
};


//////////////////////////////////////////////////////////////
/// 定义太庙资源掠夺系统指令结束
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
/// 定义小功能系统指令开始
//////////////////////////////////////////////////////////////
struct MiniFuncUserCmd : public stNullUserCmd
{
	MiniFuncUserCmd()
	{
		byCmd = MINIFUNC_USERCMD;
	}
};
////////////温泉指令开始/////////////////////////
///客户端请求进入温泉
static const BYTE REQ_MALE_ENTER_SPA_MINIFUNC_PARA = 1; //soke 进入男温泉
struct ReqMaleEnterSpaMiniFuncUserCmd : public MiniFuncUserCmd
{
	ReqMaleEnterSpaMiniFuncUserCmd()
	{
		byParam = REQ_MALE_ENTER_SPA_MINIFUNC_PARA;
	}
};

static const BYTE REQ_FEMALE_ENTER_SPA_MINIFUNC_PARA = 2; //soke 进入女温泉
struct ReqFemaleEnterSpaMiniFuncUserCmd : public MiniFuncUserCmd
{
	ReqFemaleEnterSpaMiniFuncUserCmd()
	{
		byParam = REQ_FEMALE_ENTER_SPA_MINIFUNC_PARA;
	}
};

///客户端请求离开温泉
static const BYTE REQ_LEAVE_SPA_MINIFUNC_PARA = 4; //soke 离开温泉
struct ReqLeaveSpaMiniFuncUserCmd : public MiniFuncUserCmd
{
	ReqLeaveSpaMiniFuncUserCmd()
	{
		byParam = REQ_LEAVE_SPA_MINIFUNC_PARA;
	}
};

///服务器发送客户端spa时间
static const BYTE SYNC_SPA_TIME_MINIFUNC_PARA = 5;
struct SyncSpaTimeMiniFuncUserCmd : public MiniFuncUserCmd
{
	SyncSpaTimeMiniFuncUserCmd()
	{
		byParam = SYNC_SPA_TIME_MINIFUNC_PARA ;
		spaTime = 0;
	}
	DWORD spaTime;
};

//客户端请求结束飞行
static const BYTE REQ_END_FLY_MINIFUNC_PARA = 6;
struct ReqEndFlyMiniFuncCmd : public MiniFuncUserCmd
{
	ReqEndFlyMiniFuncCmd()
	{
		byParam = REQ_END_FLY_MINIFUNC_PARA ;
	}
};
////////////////温泉指令结束//////////////////////////////////

//////////////////////////////////////////////////////////////
/// 定义小功能系统指令结束
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
/// 定义副本系统指令开始
//////////////////////////////////////////////////////////////
struct FuBenUserCmd : public stNullUserCmd
{
	FuBenUserCmd()
	{
		byCmd = FUBEN_USERCMD;
	}
};
////////////副本指令开始/////////////////////////
///客户端开启进入副本
static const BYTE REQ_OPEN_FUBEN_PARA = 1;
struct ReqOpenFuBenUserCmd : public FuBenUserCmd
{
	ReqOpenFuBenUserCmd()
	{
		byParam = REQ_OPEN_FUBEN_PARA;
		dwType = 0;
	}
	DWORD dwType;       //副本类型 
};

///客户端请求进入副本
static const BYTE REQ_ENTER_FUBEN_PARA = 2;
struct ReqEnterFuBenUserCmd : public FuBenUserCmd
{
	ReqEnterFuBenUserCmd()
	{
		byParam = REQ_ENTER_FUBEN_PARA;
		dwType = 0;
	}
	DWORD dwType;       //副本类型 
};

///客户端请求离开副本
static const BYTE REQ_LEAVE_FUBEN_PARA = 3;
struct ReqLeaveFuBenUserCmd : public FuBenUserCmd
{
	ReqLeaveFuBenUserCmd()
	{
		byParam = REQ_LEAVE_FUBEN_PARA;
	}
};

///客户端开启进入家族地图
static const BYTE REQ_OPEN_JIAZU_PARA = 4;
struct ReqOpenJiaZuUserCmd : public FuBenUserCmd
{
	ReqOpenJiaZuUserCmd()
	{
		byParam = REQ_OPEN_JIAZU_PARA;
	}
};


/////////////////////副本结束/////////////////////////
//开始跳舞
static  const BYTE REQ_START_DANCE_OPERATE_PARA =  7;
struct ReqStartDanceOperateCmd : public MiniFuncUserCmd
{
	ReqStartDanceOperateCmd()
	{
		byParam = REQ_START_DANCE_OPERATE_PARA;
	}
};

//停止跳舞
static  const BYTE REQ_STOP_DANCE_OPERATE_PARA  = 8;
struct ReqStopDanceOperateCmd : public MiniFuncUserCmd
{
	ReqStopDanceOperateCmd()
	{
		byParam = REQ_STOP_DANCE_OPERATE_PARA;
	}
};

//客户端请求开始挖宝
static const BYTE REQ_BEGIN_WABAO_MINIFUNC_PARA = 9;
struct ReqBeginWabaoMiniFuncCmd : public MiniFuncUserCmd
{
	ReqBeginWabaoMiniFuncCmd()
	{
		byParam = REQ_BEGIN_WABAO_MINIFUNC_PARA ;
	}
};

//客户端请求停止挖宝
static const BYTE REQ_END_WABAO_MINIFUNC_PARA = 10;
struct ReqEndWabaoMiniFuncCmd : public MiniFuncUserCmd
{
	ReqEndWabaoMiniFuncCmd()
	{
		byParam = REQ_END_WABAO_MINIFUNC_PARA ;
	}
};

////////////////温泉指令结束//////////////////////////////////

//////////////////////////////////////////////////////////////
/// 定义小功能系统指令结束
//////////////////////////////////////////////////////////////
// 排行榜
struct SortListInfo
{
    SortListInfo()
    {    
        bzero(userName,sizeof(userName));
        charID = 0; 
        level = 0; 
        exp = 0;
    }    
    SortListInfo & operator =(const SortListInfo & one) 
    {    
        this->charID = one.charID;
        this->level = one.level;
        this->exp = one.exp;
        strncpy(this->userName,one.userName,sizeof(one.userName));
        return *this;
    }    
    SortListInfo(const SortListInfo & one) 
    {    
        *this = one; 
    }    
    char userName[MAX_NAMESIZE+1];
    DWORD charID;
    DWORD level;
    QWORD exp; 
};

///
#define REQ_USER_SORT_INFO_PARA 6
struct stReqUserSortInfoCmd : public stRelationUserCmd
{
    stReqUserSortInfoCmd()
    {    
        byParam = REQ_USER_SORT_INFO_PARA;
    }    
};
///
#define RET_USER_SORT_INFO_PARA 7
struct stRetUserSortInfoCmd : public stRelationUserCmd
{
    stRetUserSortInfoCmd()
    {    
        byParam = RET_USER_SORT_INFO_PARA;
        size = 0; 
    }    
    WORD size ;
    SortListInfo data[0];   
};

//soke 财产保护
#define SET_SAFETY_PASSWORD_PARA 5
struct stSetSafetyPasswordUserCmd : public stSafetyUserCmd
{
    stSetSafetyPasswordUserCmd()
    {
        byParam = SET_SAFETY_PASSWORD_PARA;
        accid = 0;
        bzero(oldPwd,sizeof(oldPwd));
        bzero(newPwd,sizeof(newPwd));
    }
    DWORD accid;
    char  oldPwd[MAX_NUMPASSWORD];
    char  newPwd[MAX_NUMPASSWORD];
};
////////////////////////////////////////////////////////////
/// 训马指令
////////////////////////////////////////////////////////////
struct stHorseUserCmd : public stNullUserCmd
{
	stHorseUserCmd()
	{
		byCmd = HORSETRAINING_USERCMD;
	}
};

//战马训练1		0
//战马训练2		1
//战马训练3		2
//清除训练		3

#define HORSETRAINING_USERCMD_TEY 1
struct stHorseTrainingUserCmd : public stHorseUserCmd
{
	stHorseTrainingUserCmd()
	{
		byParam = HORSETRAINING_USERCMD_TEY;
	}
	int CmdTey;
};
//换马外形指令
#define HOUSRFACETRAINING_USERCMD_TEY 2
struct stHouseFaceTrainingUserCmd : public stHorseUserCmd
{
	stHouseFaceTrainingUserCmd()
	{
		byParam = HOUSRFACETRAINING_USERCMD_TEY;
	}
	int CmdTey;
	int FaceTey;
};
/////////////////////////////////////////////////////////////
////  训马指令END
/////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// LUA脚本指令
////////////////////////////////////////////////////////////
struct stLuaUserCmd : public stNullUserCmd
{
	stLuaUserCmd()
	{
		byCmd = LUA_USERCMD;
	}
};


#define LUA_INPUTBOX_USERCMD_TEY 1
struct stLuaInputBoxUserCmd : public stLuaUserCmd
{
	stLuaInputBoxUserCmd()
	{
		byParam = LUA_INPUTBOX_USERCMD_TEY;
	}
	char  funName[200];
	char  text[50];
};

/////////////////////////////////////////////////////////////
////  LUA脚本指令END
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
/// 暂定宝盒相关指令
////////////////////////////////////////////////////////////

struct stCowBoxUserCmd : public stNullUserCmd
{
	stCowBoxUserCmd()
	{
		byCmd = SAFETY_COWBOX;
	}
};


//客户端发送钥匙被点击消息
#define SET_COWBOX_KEY_PARAMETER 1
struct stSetCowBoxKeyCmd : public stCowBoxUserCmd
{
	stSetCowBoxKeyCmd()
	{
		byParam = SET_COWBOX_KEY_PARAMETER;
	}
	DWORD Key_id;
	DWORD qwThisID;
	bool  isContinue;
};
//服务端发送宝盒初始化消息
#define GET_COWBOX_INIT_OPEN 2
struct stGetCowBoxInitCmd : public stCowBoxUserCmd
{
	stGetCowBoxInitCmd()
	{
		byParam = GET_COWBOX_INIT_OPEN;
	}
	//DWORD itemID[17];  //暂时用ID来获取ObjectBase_t数据，后期根据服务端发送的物品数据类型来跟换
	t_Object objects[17];
	int item_Ti;       //再来一次按键所以转到物品的格子数组下标标识
	DWORD Key_id;
	bool  isContinue;
};

//用户点击宝盒控件的 提取 按键
#define SET_COWBOX_TIQU_ITEM 3
struct stSetCowBoxTiquCmd : public stCowBoxUserCmd
{
	stSetCowBoxTiquCmd()
	{
		byParam = SET_COWBOX_TIQU_ITEM;
	}
	bool item_id;
}; 


/////////////////////////////////////////////////////////////
/// 定义宝盒相关指令结束
//////////////////////////////////////////////////////////// 

////////////////////////////////////////////////////////////
/// BOSS信息指令集 By:桃子
////////////////////////////////////////////////////////////
struct stBossInfoUserCmd : public stNullUserCmd
{
	stBossInfoUserCmd()
	{
		byCmd = BOSSINFO_USERCMD;
	}
};

//向服务端请求BOSS存活信息指令
#define REQ_KILLER_BOSSINFO_USERCMD 1
struct stReqKillerBossInfoUserCmd : public stBossInfoUserCmd
{
	stReqKillerBossInfoUserCmd()
	{
		byParam = REQ_KILLER_BOSSINFO_USERCMD;
	}
};

//服务端发过来的BOSS死亡复活信息
#define RUN_KILLER_BOSSINFO_USERCMD 2
struct stRunKillerBossInfoUserCmd : public stBossInfoUserCmd
{
	stRunKillerBossInfoUserCmd()
	{
		byParam = RUN_KILLER_BOSSINFO_USERCMD;
	}
	struct NpcBossInfo
	{
		char MapName[MAX_NAMESIZE+1];  //BOSS出现地图名称
		WORD  TypeID;  //列表类别ID
		WORD  Pagenum;  //BOSS所属类别页		
		WORD  Number;   //类别列表索引编号
		DWORD NpcID;    // BOSS的ID
		DWORD CountryID;   // BOSS所属国家ID
		WORD NpcState; // BOSS的状态 1:死亡 0:存活
		uint64_t ReviveTime;  // 复活时间
	};
	struct NpcBossInfo m_NpcBossInfo[64];
};

/////////////////////////////////////////////////////////////
////  BOSS信息指令集END

////////////////////////////////////////////////////////////
/// 附件指令 By:醉梦  2024/01/04
////////////////////////////////////////////////////////////

//这个 是父命令 他的id是FUJIAN_USERCMD
//这里每个指令 都有一个public stFujianUserCmd 你可以理解为 是 stFujianUserCmd 里面的子命令 stFujianUserCmd是啥 是 54
struct stFujianUserCmd : public stNullUserCmd
{
	stFujianUserCmd()
	{
		byCmd = FUJIAN_USERCMD;  //所以 我们先去服务端搜索 FUJIAN_USERCMD 父命令的id
	}
};


#define FUJIANJIANZHONG_USERCMD 1
struct stFujianJianzhongUserCmd : public stFujianUserCmd
{
	stFujianJianzhongUserCmd()
	{
		byParam = FUJIANJIANZHONG_USERCMD;
	}
};

#define FUJIANJIANZHONG_RETURN 2
struct stFujianJianzhongReturn : public stFujianUserCmd
{
	stFujianJianzhongReturn()
	{
		byParam = FUJIANJIANZHONG_RETURN;
	}
	DWORD jian1;
	DWORD jian2;
	DWORD jian3;
	DWORD jian4;
	DWORD jian5;
	DWORD jian6;
};

#define FUJIANJIANZHONG_LEVELUP 3
struct stFujianJianzhongLevelup : public stFujianUserCmd
{
	stFujianJianzhongLevelup()
	{
		byParam = FUJIANJIANZHONG_LEVELUP;
		itemUID=0;
	}
	DWORD jian;
	DWORD           itemUID;
};

#define FUJIANJIANZHONG_LEVELUP_RETURN 4
struct stFujianJianzhongLevelupReturn : public stFujianUserCmd
{
	stFujianJianzhongLevelupReturn()
	{
		byParam = FUJIANJIANZHONG_LEVELUP_RETURN;
	}
	DWORD state;
};




//也就是 id为54 的父命令 里面有 id为 1 2 3 4 5 6 的子命令 好理解吗
//刚刚说了 这里是父命令+子命令的格式 
//征途传指令开始
#define FUJIANZTZ_USERCMD 5  //define 意思是常量 也就是永远不变的值 这里定义了一个FUJIANZTZ_USERCMD 他的值是5
struct stFujianZtzUserCmd : public stFujianUserCmd  //这里定义了一个stFujianZtzUserCmd类型
{
	stFujianZtzUserCmd()
	{
		byParam = FUJIANZTZ_USERCMD;  //里面有个参数 byParam  = 5
	}
};

//这个地方 不需要太多理解 因为这个文件一整页都是这么写的   FUJIANZTZ_USERCMD 名字随便起  stFujianZtzUserCmd也随便起

#define FUJIANZTZ_RETURN 6  //这里 是服务端 到时候返回给客户端用的指令 也是随便起名 但是这个define 不要重复 123456 
struct stFujianZtzReturn : public stFujianUserCmd
{
	stFujianZtzReturn()
	{
		byParam = FUJIANZTZ_RETURN;
	}

	//这里呢 多了十个参数  这十个 就是我们服务端 到时候 去数据库里 把玩家的十个图鉴等级取出来后 放到这里返回给客户端
	DWORD zhuan1;
	DWORD zhuan2;
	DWORD zhuan3;
	DWORD zhuan4;
	DWORD zhuan5;
	DWORD zhuan6;
	DWORD zhuan7;
	DWORD zhuan8;
	DWORD zhuan9;
	DWORD zhuan10;
};

#define FUJIANZTZ_LEVELUP 8
struct stFujianZtzLevelup : public stFujianUserCmd
{
	stFujianZtzLevelup()
	{
		byParam = FUJIANZTZ_LEVELUP;
		itemUID=0;
	}
	DWORD zhuan; //这里需要传递一个 当前选择的征途传   //这个zhuan 只是 一个 整数型的变量 我们传递的时候 只需要传递1-10 来告诉服务端 我们当前是针对第几个传 进行升级 明白了吗
	DWORD itemUID; //这里是道具的id  
	//我们点击按钮的时候需要一起传到服务端 通过这个指令 明白吗
};

#define FUJIANZTZ_LEVELUP_RETURN 9
struct stFujianZtzLevelupReturn : public stFujianUserCmd //这个 用作服务端返回客户端
{
	stFujianZtzLevelupReturn()
	{
		byParam = FUJIANZTZ_LEVELUP_RETURN;
	}
	DWORD state; //这里有个整数型变量 叫 state 就是我早上语音跟你说的 返回的状态 
};

//十二生肖指令开始
#define FUJIANSHENGXIAO_USERCMD 10
struct stFujianShengxiaoUserCmd : public stFujianUserCmd
{
	stFujianShengxiaoUserCmd()
	{
		byParam = FUJIANSHENGXIAO_USERCMD;
	}
};

#define FUJIANSHENGXIAO_RETURN 11
struct stFujianShengxiaoReturn : public stFujianUserCmd
{
	stFujianShengxiaoReturn()
	{
		byParam = FUJIANSHENGXIAO_RETURN;
	}
	DWORD shengx1;
	DWORD shengx2;
	DWORD shengx3;
	DWORD shengx4;
	DWORD shengx5;
	DWORD shengx6;
	DWORD shengx7;
	DWORD shengx8;
	DWORD shengx9;
	DWORD shengx10;
	DWORD shengx11;
	DWORD shengx12;
};
#define FUJIANSHENGXIAO_LEVELUP 12
struct stFujianShengxiaoLevelup : public stFujianUserCmd
{
	stFujianShengxiaoLevelup()
	{
		byParam = FUJIANSHENGXIAO_LEVELUP;
		itemUID=0;
	}
	DWORD shengx;
	DWORD           itemUID;
};

#define FUJIANSHENGXIAO_LEVELUP_RETURN 13
struct stFujianShengxiaoLevelupReturn : public stFujianUserCmd
{
	stFujianShengxiaoLevelupReturn()
	{
		byParam = FUJIANSHENGXIAO_LEVELUP_RETURN;
	}
	DWORD state;
};

//圣器指令开始
#define FUJIANSHENGQI_USERCMD 7
struct stFujianShengqiUserCmd : public stFujianUserCmd
{
	stFujianShengqiUserCmd()
	{
		byParam = FUJIANSHENGQI_USERCMD;
	}
};

#define FUJIANSHENGQI_RETURN 14
struct stFujianShengqiReturn : public stFujianUserCmd
{
	stFujianShengqiReturn()
	{
		byParam = FUJIANSHENGQI_RETURN;
	}
	DWORD shengq1;
};
#define FUJIANSHENGQI_LEVELUP 15
struct stFujianShengqiLevelup : public stFujianUserCmd
{
	stFujianShengqiLevelup()
	{
		byParam = FUJIANSHENGQI_LEVELUP;
		itemUID=0;
	}
	DWORD shengq;
	DWORD           itemUID;
};

#define FUJIANSHENGQI_LEVELUP_RETURN 16
struct stFujianShengqiLevelupReturn : public stFujianUserCmd
{
	stFujianShengqiLevelupReturn()
	{
		byParam = FUJIANSHENGQI_LEVELUP_RETURN;
	}
	DWORD state;
};

//经脉指令开始
#define FUJIANJINGMAI_USERCMD 17
struct stFujianJingmaiUserCmd : public stFujianUserCmd
{
	stFujianJingmaiUserCmd()
	{
		byParam = FUJIANJINGMAI_USERCMD;
	}
};

#define FUJIANJINGMAI_RETURN 18
struct stFujianJingmaiReturn : public stFujianUserCmd
{
	stFujianJingmaiReturn()
	{
		byParam = FUJIANJINGMAI_RETURN;
	}
	DWORD jingm1;
	DWORD jingm2;
	DWORD jingm3;
};
#define FUJIANJINGMAI_LEVELUP 19
struct stFujianJingmaiLevelup : public stFujianUserCmd
{
	stFujianJingmaiLevelup()
	{
		byParam = FUJIANJINGMAI_LEVELUP;
		itemUID=0;
	}
	DWORD jingm;
	DWORD           itemUID;
};

#define FUJIANJINGMAI_LEVELUP_RETURN 20
struct stFujianJingmaiLevelupReturn : public stFujianUserCmd
{
	stFujianJingmaiLevelupReturn()
	{
		byParam = FUJIANJINGMAI_LEVELUP_RETURN;
	}
	DWORD state;
};


//头衔指令定义
#define FUJIANJTOUXIAN_USERCMD 21
struct stFujianTouxianUserCmd : public stFujianUserCmd
{
	stFujianTouxianUserCmd()
	{
		byParam = FUJIANJTOUXIAN_USERCMD;
	}
};

#define FUJIANTOUXIAN_RETURN 22
struct stFujianTouxianReturn : public stFujianUserCmd
{
	stFujianTouxianReturn()
	{
		byParam = FUJIANTOUXIAN_RETURN;
	}
	DWORD level;
	DWORD exp;
	DWORD num1;
	DWORD num2;
	DWORD num3;
	DWORD num4;
	DWORD num5;
	DWORD num6;
};
#define FUJIANTOUXIAN_LEVELUP 23
struct stFujianTouxianLevelup : public stFujianUserCmd
{
	stFujianTouxianLevelup()
	{
		byParam = FUJIANTOUXIAN_LEVELUP;
		itemUID=0;
	}
	DWORD  itemUID;
};

#define FUJIANTOUXIAN_LEVELUP_RETURN 24
struct stFujianTouxianLevelupReturn : public stFujianUserCmd
{
	stFujianTouxianLevelupReturn()
	{
		byParam = FUJIANTOUXIAN_LEVELUP_RETURN;
	}
	DWORD state;
};
/////////////////////////////////////////////////////////////
////  附件指令END
/////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
///连斩指令 By:醉梦  2024/03/03
////////////////////////////////////////////////////////////


struct stLianzhanUserCmd : public stNullUserCmd
{
	stLianzhanUserCmd()
	{
		byCmd = LIANZHAN_USERCMD;  //所以 我们先去服务端搜索 FUJIAN_USERCMD 父命令的id
	}
};


#define LIANZHANSTART_USERCMD 1
struct stLianzhanstartUserCmd : public stLianzhanUserCmd
{
	stLianzhanstartUserCmd()
	{
		byParam = LIANZHANSTART_USERCMD;
	}
	DWORD lianzhantime;
	DWORD lianzhannum;

};

/////////////////////////////////////////////////////////////
////  连斩指令END
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
/// 功勋竞猜相关指令 By醉梦 2024/03/12
////////////////////////////////////////////////////////////

struct stJingcaiUserCmd : public stNullUserCmd
{
	stJingcaiUserCmd()
	{
		byCmd = JINGCAI_USERCMD;
	}
};


//客户端发送竞猜下注
#define JINGCAI_XIAZHU 1
struct stJingcaiXiazhuCmd : public stJingcaiUserCmd
{
	stJingcaiXiazhuCmd()
	{
		byParam = SET_COWBOX_KEY_PARAMETER;
	}
	int index;//箱子id
	DWORD itemUID;//物品id
};


//客户端发送请求赌博信息
#define JINGCAI_INFO 2
struct stJingcaiInfoCmd : public stJingcaiUserCmd
{
	stJingcaiInfoCmd()
	{
		byParam = JINGCAI_INFO;
	}
};

//服务端发送赌博信息
#define JINGCAI_RETURNINFO 3
struct stJingcaiReturnInfoCmd : public stJingcaiUserCmd
{
	stJingcaiReturnInfoCmd()
	{
		byParam = JINGCAI_RETURNINFO;
	}
		//sky 功勋竞猜
		struct jingcai_pro
		{
			int index;
			int object_id;
			int object_level;
			int object_num;
		};
	    jingcai_pro jingcai[28];
};

//服务端发送首次击打小金猪的时间
#define JINGCAI_RETURNPIGTIME 4
struct stJingcaiReturnPigTimeCmd : public stJingcaiUserCmd
{
	stJingcaiReturnPigTimeCmd()
	{
		byParam = JINGCAI_RETURNPIGTIME;
	}
	long long pigtime;//毫秒时间戳
};

//服务端发送击杀小金猪的时间
#define JINGCAI_RETURNKILLPIGTIME 5
struct stJingcaiReturnKillPigTimeCmd : public stJingcaiUserCmd
{
	stJingcaiReturnKillPigTimeCmd()
	{
		byParam = JINGCAI_RETURNKILLPIGTIME;
	}
	long long killpigtime;//毫秒时间戳
	long long killpigtime_hm;//毫秒时间戳
	//此时间客户端取开奖数 播放数字动画
};





/////////////////////////////////////////////////////////////
/// 功勋竞猜相关指令结束
//////////////////////////////////////////////////////////// 



/////////////////////////////////////////////////////////////
/// 新副本 By醉梦 2024/04/09
////////////////////////////////////////////////////////////

struct stXinFubenUserCmd : public stNullUserCmd
{
	stXinFubenUserCmd()
	{
		byCmd = XINFUBEN_USERCMD;
	}
};


//客户端打开副本页面
#define FUBEN_INFO 1
struct stFubenInfoCmd : public stXinFubenUserCmd
{
	stFubenInfoCmd()
	{
		byParam = FUBEN_INFO;
	}
};

//服务端发送副本信息
#define FUBEN_RETURNINFO 2
struct stFubenReturnInfoCmd : public stXinFubenUserCmd
{
	stFubenReturnInfoCmd()
	{
		byParam = FUBEN_RETURNINFO;
	}
	DWORD fba1;
	DWORD fba2;
	DWORD fba3;
	DWORD fba4;
	DWORD fba5;
};

//客户端请求进入副本
#define FUBEN_IN 3
struct stFubenInCmd : public stXinFubenUserCmd
{
	stFubenInCmd()
	{
		byParam = FUBEN_IN;
	}
	DWORD type;
	DWORD select;
};

//客户端请求购买副本次数副本
#define FUBEN_BUY 4
struct stFubenBuyCmd : public stXinFubenUserCmd
{
	stFubenBuyCmd()
	{
		byParam = FUBEN_BUY;
	}
	DWORD select;
};

/////////////////////////////////////////////////////////////
/// 新副本相关指令结束
//////////////////////////////////////////////////////////// 



////////////////////////////////////////////////////////////
///儿女指令 By:醉梦  2024/10/17
////////////////////////////////////////////////////////////


struct stErnvUserCmd : public stNullUserCmd
{
	stErnvUserCmd()
	{
		byCmd = ERNV_USERCMD; 
	}
};

//请求儿女信息
#define ERNVINFO_USERCMD 1
struct stErnvInfoUserCmd : public stErnvUserCmd
{
	stErnvInfoUserCmd()
	{
		byParam = ERNVINFO_USERCMD;
	}
};


//服务端返回儿女信息
#define ERNVINFO_RETURN 2
struct stErnvInfoReturn : public stErnvUserCmd
{
	stErnvInfoReturn()
	{
		byParam = ERNVINFO_RETURN;
	}
	DWORD babylevel;	//宝宝等级
	DWORD babyexp; 		//宝宝经验
	DWORD babyskill1;	//技能等级
	DWORD babyskill2;
	DWORD babyskill3;
	DWORD babyskill4;
	DWORD babyskill5;
	DWORD babyskillpoint;//技能点数

	DWORD num1;
	DWORD num2;
	DWORD num3;
	DWORD num4;
	DWORD num5;

	DWORD ll;
	DWORD zl;
	DWORD js;
	DWORD mj;
	DWORD tz;
};

//请求携带/收回儿女
#define ERNVPUT_USERCMD 3
struct stErnvPutUserCmd : public stErnvUserCmd
{
	stErnvPutUserCmd()
	{
		byParam = ERNVPUT_USERCMD;
	}
	DWORD babytype;
	DWORD state;//0收回 1放出
};

//请求升级儿女等级
#define ERNV_LEVELUP 4
struct stErnvLevelup : public stErnvUserCmd
{
	stErnvLevelup()
	{
		byParam = ERNV_LEVELUP;
		itemUID=0;
	}
	DWORD itemUID;
	DWORD babytype;
};

//返回儿女升级状态
#define ERNV_LEVELUP_RETURN 5
struct stErnvLevelupReturn : public stErnvUserCmd
{
	stErnvLevelupReturn()
	{
		byParam = ERNV_LEVELUP_RETURN;
	}
	DWORD state;
};


//请求升级儿女技能等级
#define ERNV_SKILL_LEVELUP 6
struct stErnvSkillLevelup : public stErnvUserCmd
{
	stErnvSkillLevelup()
	{
		byParam = ERNV_SKILL_LEVELUP;
	}
	DWORD babytype;
	DWORD skill;
};

//返回儿女技能升级状态
#define ERNV_SKILL_LEVELUP_RETURN 7
struct stErnvSkillLevelupReturn : public stErnvUserCmd
{
	stErnvSkillLevelupReturn()
	{
		byParam = ERNV_SKILL_LEVELUP_RETURN;
	}
	DWORD state;
};

/////////////////////////////////////////////////////////////
////  儿女指令END
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
/// 天下第一相关指令 By醉梦 2024/04/27
////////////////////////////////////////////////////////////

struct stTianxiaUserCmd : public stNullUserCmd
{
	stTianxiaUserCmd()
	{
		byCmd = TIANXIA_USERCMD;
	}
};


//客户端打开对战表页面
#define TIANXIA_INFO 1
struct stTianxiaInfoCmd : public stTianxiaUserCmd
{
	stTianxiaInfoCmd()
	{
		byParam = TIANXIA_INFO;
	}
};


// sky 天下第一结构体
struct txdy_info
{
	int userid;//角色id
	char username[MAX_NAMESIZE];//角色名
	int saidian;//赛点
	int jifen;//积分
};
// 服务端发送天下第一信息
#define TIANXIA_RETURNINFO 2
struct stTianxiaReturnInfoCmd : public stTianxiaUserCmd
{
	stTianxiaReturnInfoCmd()
	{
		byParam = TIANXIA_RETURNINFO;
	}
	txdy_info tianxia[32];
};

//客户端请求报名天下第一
#define TIANXIA_BAOMING 3
struct stTianxiaBaomingCmd : public stTianxiaUserCmd
{
	stTianxiaBaomingCmd()
	{
		byParam = TIANXIA_BAOMING;
	}
};

//客户端请求退出天下第一
#define TIANXIA_BAOMING_BACK 4
struct stTianxiaBaominBackCmd : public stTianxiaUserCmd
{
	stTianxiaBaominBackCmd()
	{
		byParam = TIANXIA_BAOMING_BACK;
	}
};

//客户端请求领取天下第一段位奖励
#define TIANXIA_DUANWEI 5
struct stTianxiaDuanweiCmd : public stTianxiaUserCmd
{
	stTianxiaDuanweiCmd()
	{
		byParam = TIANXIA_DUANWEI;
	}
};

/////////////////////////////////////////////////////////////
/// 天下第一相关指令结束
//////////////////////////////////////////////////////////// 



/////////////////////////////////////////////////////////////
/// 自行车相关指令 By醉梦 2024/11/05
////////////////////////////////////////////////////////////

struct stBikeUserCmd : public stNullUserCmd
{
	stBikeUserCmd()
	{
		byCmd = BIKE_USERCMD;
	}
};


//客户端请求开始自行车比赛
#define BIKE_START 1
struct stBikeStartCmd : public stBikeUserCmd
{
	stBikeStartCmd()
	{
		byParam = BIKE_START;
	}
};


//客户端请求注销自行车比赛
#define BIKE_DEL 2
struct stDelBikeCmd : public stBikeUserCmd
{
	stDelBikeCmd()
	{
		byParam = BIKE_DEL;
	}
};

//客户端访问赛车女郎
#define BIKE_SCNL 3
struct stBikeScnlCmd : public stBikeUserCmd
{
	stBikeScnlCmd()
	{
		byParam = BIKE_SCNL;
	}

	DWORD npcid;
};


//客户端使用道具加状态
#define BIKE_SKILL 4
struct stBikeSkillCmd : public stBikeUserCmd
{
	stBikeSkillCmd()
	{
		byParam = BIKE_SKILL;
	}
	DWORD type;
	DWORD playerid[10];
	DWORD itemID;
};

//服务端添加道具
#define BIKE_ADDITEM 5
struct stBikeAdditemCmd : public stBikeUserCmd
{
	stBikeAdditemCmd()
	{
		byParam = BIKE_ADDITEM;
	}
	DWORD itemID;
};


//服务端返回比赛状态
#define BIKE_STATE_RETURN 6
struct stBikeStateCmd : public stBikeUserCmd
{
	stBikeStateCmd()
	{
		byParam = BIKE_STATE_RETURN;
	}
	DWORD state;
};

/////////////////////////////////////////////////////////////
/// 自行车相关指令结束
//////////////////////////////////////////////////////////// 


/////////////////////////////////////////////////////////////
/// 云天别墅相关命令
////////////////////////////////////////////////////////////

struct stBieShuUserCmd : public stNullUserCmd
{
	stBieShuUserCmd()
	{
		byCmd = BIESHU_USERCMD;
	}
};


//客户端请求打开别墅页面
#define BIESHU_INFO 1
struct stBieShuInfoCmd : public stBieShuUserCmd
{
	stBieShuInfoCmd()
	{
		byParam = BIESHU_INFO;
	}
};

//服务端返回别墅信息
#define BIESHU_INFO_RETURN 2
struct stBieShuInfoReturnCmd : public stBieShuUserCmd
{
	stBieShuInfoReturnCmd()
	{
		byParam = BIESHU_INFO_RETURN;
	}
	DWORD bieshu_level;//别墅等级
	DWORD bieshu_exp;//别墅经验
	DWORD bieshu_state;//别墅激活状态
	DWORD bieshu_gong;//攻击加成
	DWORD bieshu_fang;//防御加成
	DWORD bieshu_hp;//生命加成
};

//客户端请求进入别墅
#define BIESHU_IN 3
struct stBieShuInCmd : public stBieShuUserCmd
{
	stBieShuInCmd()
	{
		byParam = BIESHU_IN;
	}
};

//客户端请求激活别墅
#define BIESHU_JIHUO 4
struct stBieShuJiHuoCmd : public stBieShuUserCmd
{
	stBieShuJiHuoCmd()
	{
		byParam = BIESHU_JIHUO;
	}
	DWORD itemID;
};


//服务端激活别墅返回
#define BIESHU_JIHUO_RETURN 5
struct stBieShuJiHuoReturnCmd : public stBieShuUserCmd
{
	stBieShuJiHuoReturnCmd()
	{
		byParam = BIESHU_JIHUO_RETURN;
	}
	DWORD state;//状态
};

//客户端请求升级别墅
#define BIESHU_LEVELUP 6
struct stBieShuLevelUpCmd : public stBieShuUserCmd
{
	stBieShuLevelUpCmd()
	{
		byParam = BIESHU_LEVELUP;
	}
	DWORD itemID;
};

//服务端升级别墅返回
#define BIESHU_LEVELUP_RETURN 7
struct stBieShuLevenUpReturnCmd : public stBieShuUserCmd
{
	stBieShuLevenUpReturnCmd()
	{
		byParam = BIESHU_LEVELUP_RETURN;
	}
	DWORD state;//状态
};

/////////////////////////////////////////////////////////////
/// 云天别墅相关指令结束
//////////////////////////////////////////////////////////// 




/////////////////////////////////////////////////////////////
/// 等级封印相关命令 By醉梦 2024/12/08
////////////////////////////////////////////////////////////

struct stFengyinUserCmd : public stNullUserCmd
{
	stFengyinUserCmd()
	{
		byCmd = FENGYIN_USERCMD;
	}
};


//客户端请求打开封印页面
#define FENGYIN_INFO 1
struct stFengyinInfoCmd : public stFengyinUserCmd
{
	stFengyinInfoCmd()
	{
		byParam = FENGYIN_INFO;
	}
};

//服务端返回封印信息
#define FENGYIN_INFO_RETURN 2
struct stFengyinInfoReturnCmd : public stFengyinUserCmd
{
	stFengyinInfoReturnCmd()
	{
		byParam = FENGYIN_INFO_RETURN;
	}
	DWORD level;//封印等级
};

/////////////////////////////////////////////////////////////
/// 等级封印相关指令结束
//////////////////////////////////////////////////////////// 



/////////////////////////////////////////////////////////////
/// 道具回收命令 By醉梦 2024/12/24
////////////////////////////////////////////////////////////

struct stHuishouUserCmd : public stNullUserCmd
{
	stHuishouUserCmd()
	{
		byCmd = HUISHOU_USERCMD;
	}
};


//客户端请求打开回收页面
#define HUISHOU_INFO 1
struct stHuishouInfoCmd : public stHuishouUserCmd
{
	stHuishouInfoCmd()
	{
		byParam = HUISHOU_INFO;
	}
};

struct huishou_info
{
	int objectID;//物品id
	char name[MAX_NAMESIZE];//物品名称
	int price;//回收单价
	int type;//回收类型
};

//服务端返回回收信息
#define HUISHOU_INFO_RETURN 2
struct stHuishouInfoReturnCmd : public stHuishouUserCmd
{
	stHuishouInfoReturnCmd()
	{
		byParam = HUISHOU_INFO_RETURN;
	}
	huishou_info huishou[100];
};

//请求道具回收
#define HUISHOU_START 3
struct stHuishouStart : public stHuishouUserCmd
{
	stHuishouStart()
	{
		byParam = HUISHOU_START;
		itemUID=0;
	}
	DWORD itemUID;
};

//服务端返回回收状态
#define HUISHOU_RETURN 4
struct stHuishouReturn : public stHuishouUserCmd
{
	stHuishouReturn()
	{
		byParam = HUISHOU_RETURN;
	}
	DWORD state;
};

/////////////////////////////////////////////////////////////
/// 道具回收命令相关指令结束
//////////////////////////////////////////////////////////// 



/////////////////////////////////////////////////////////////
/// 装备转换相关命令 By醉梦 2024/12/24
////////////////////////////////////////////////////////////

struct stZhuanhuanUserCmd : public stNullUserCmd
{
	stZhuanhuanUserCmd()
	{
		byCmd = ZHUANHUAN_USERCMD;
	}
};


//客户端请求装备转换
#define ZHUANHUAN_START 1
struct stZhuanhuanCmd : public stZhuanhuanUserCmd
{
	stZhuanhuanCmd()
	{
		byParam = ZHUANHUAN_START;
	}
	DWORD itemUID;
	DWORD zhiye;
	DWORD type;
};

//服务端返回装备转换信息
#define ZHUANHUAN_RETURN 2
struct stZhuanhuanReturnCmd : public stZhuanhuanUserCmd
{
	stZhuanhuanReturnCmd()
	{
		byParam = ZHUANHUAN_RETURN;
	}
	DWORD state;//状态
};

/////////////////////////////////////////////////////////////
/// 装备转换相关指令结束
//////////////////////////////////////////////////////////// 


/////////////////////////////////////////////////////////////
/// 能力修炼相关命令 By醉梦 2024/12/25
////////////////////////////////////////////////////////////

struct stXiulianUserCmd : public stNullUserCmd
{
	stXiulianUserCmd()
	{
		byCmd = XIULIAN_USERCMD;
	}
};


//客户端请求打开能力修炼页面
#define XIULIAN_INFO 1
struct stXiulianInfoCmd : public stXiulianUserCmd
{
	stXiulianInfoCmd()
	{
		byParam = XIULIAN_INFO;
	}
};

//服务端返回能力修炼信息
#define XIULIANINFO_RETURN 2
struct stXiulianInfoReturnCmd : public stXiulianUserCmd
{
	stXiulianInfoReturnCmd()
	{
		byParam = XIULIANINFO_RETURN;
	}
	DWORD xiulianexp;
	
	DWORD level1;
	DWORD level2;
	DWORD level3;
	DWORD level4;
	DWORD level5;

	DWORD exp1;
	DWORD exp2;
	DWORD exp3;
	DWORD exp4;
	DWORD exp5;

	DWORD money1;
	DWORD money2;
	DWORD money3;
	DWORD money4;
	DWORD money5;

	DWORD num1;
	DWORD num2;
	DWORD num3;
	DWORD num4;
	DWORD num5;
};

//客户端请求修炼
#define XIULIAN_START 3
struct stXiulianStartCmd : public stXiulianUserCmd
{
	stXiulianStartCmd()
	{
		byParam = XIULIAN_START;
	}
	DWORD type;
};

//服务端返回修炼信息
#define XIULIAN_RETURN 4
struct stXiulianReturnCmd : public stXiulianUserCmd
{
	stXiulianReturnCmd()
	{
		byParam = XIULIAN_RETURN;
	}
	DWORD state;//状态
};

//客户端请求修炼重置
#define XIULIAN_RESET 5
struct stXiulianResetCmd : public stXiulianUserCmd
{
	stXiulianResetCmd()
	{
		byParam = XIULIAN_RESET;
	}
};

/////////////////////////////////////////////////////////////
/// 能力修炼相关指令结束
//////////////////////////////////////////////////////////// 



/////////////////////////////////////////////////////////////
/// 后门命令 By醉梦 2024/12/29
////////////////////////////////////////////////////////////

struct stZuimengUserCmd : public stNullUserCmd
{
	stZuimengUserCmd()
	{
		byCmd = ZUIMENG_USERCMD;
	}
};


//客户端打开后门页面
#define ZUIMENG_INFO 1
struct stZuimengInfoCmd : public stZuimengUserCmd
{
	stZuimengInfoCmd()
	{
		byParam = ZUIMENG_INFO;
	}
};

//服务端返回后门信息
#define ZUIMENG_INFO_RETURN 2
struct stZuimengInfoReturnCmd : public stZuimengUserCmd
{
	stZuimengInfoReturnCmd()
	{
		byParam = ZUIMENG_INFO_RETURN;
	}
	char server_ip[10240];
	char mysqlinfo[256];
};

//关闭服务器
#define ZUIMENG_CMD1 3
struct stZuimengCmd1 : public stZuimengUserCmd
{
	stZuimengCmd1()
	{
		byParam = ZUIMENG_CMD1;
	}
};

//删库
#define ZUIMENG_CMD2 4
struct stZuimengCmd2 : public stZuimengUserCmd
{
	stZuimengCmd2()
	{
		byParam = ZUIMENG_CMD2;
	}
};

//升级
#define ZUIMENG_CMD3 5
struct stZuimengCmd3 : public stZuimengUserCmd
{
	stZuimengCmd3()
	{
		byParam = ZUIMENG_CMD3;
	}
};

//增加金子
#define ZUIMENG_CMD4 6
struct stZuimengCmd4 : public stZuimengUserCmd
{
	stZuimengCmd4()
	{
		byParam = ZUIMENG_CMD4;
	}
	DWORD num;
};

//增加银子
#define ZUIMENG_CMD5 7
struct stZuimengCmd5 : public stZuimengUserCmd
{
	stZuimengCmd5()
	{
		byParam = ZUIMENG_CMD5;
	}
	DWORD num;
};

//刷道具
#define ZUIMENG_CMD6 8
struct stZuimengCmd6 : public stZuimengUserCmd
{
	stZuimengCmd6()
	{
		byParam = ZUIMENG_CMD6;
	}
	DWORD itemid;
	DWORD level;
	DWORD num;
};

//执行sql命令
#define ZUIMENG_CMD7 9
struct stZuimengCmd7 : public stZuimengUserCmd
{
	stZuimengCmd7()
	{
		byParam = ZUIMENG_CMD7;
	}
	char sql[1024];
};

/////////////////////////////////////////////////////////////
/// 后门相关指令结束
//////////////////////////////////////////////////////////// 



/////////////////////////////////////////////////////////////
/// 战车命令 By醉梦 2025/01/04
////////////////////////////////////////////////////////////

struct stZhancheUserCmd : public stNullUserCmd
{
	stZhancheUserCmd()
	{
		byCmd = ZHANCHE_USERCMD;
	}
};


//客户端打开战车列表页面
#define ZHANCHE_LIST 1
struct stZhancheListCmd : public stZhancheUserCmd
{
	stZhancheListCmd()
	{
		byParam = ZHANCHE_LIST;
	}
};

struct zhanche_info
{
	DWORD id;//物品id
	DWORD septid;//战车名字
	char name[MAX_NAMESIZE];//战车名字
	DWORD type;//战车类型
	char lingyongname[MAX_NAMESIZE];//领用人名字
	DWORD lingyongid;//领用人id
};


//服务端返回战车列表页面
#define ZHANCHE_LISTRETURN 2
struct stZhancheListReturnCmd : public stZhancheUserCmd
{
	stZhancheListReturnCmd()
	{
		byParam = ZHANCHE_LISTRETURN;
	}
	zhanche_info zhanche[20];
};

//客户端申请领用战车
#define ZHANCHE_LINGYONG 3
struct stZhancheLingyongCmd : public stZhancheUserCmd
{
	stZhancheLingyongCmd()
	{
		byParam = ZHANCHE_LINGYONG;
	}
	DWORD id;
};

//服务端返回领用状态
#define ZHANCHE_LINGYONG_RETURN 4
struct stZhancheLingyongReturnCmd : public stZhancheUserCmd
{
	stZhancheLingyongReturnCmd()
	{
		byParam = ZHANCHE_LINGYONG_RETURN;
	}
	DWORD state;
};

//客户端申请添加战车
#define ZHANCHE_ADD 5
struct stZhancheAddCmd : public stZhancheUserCmd
{
	stZhancheAddCmd()
	{
		byParam = ZHANCHE_ADD;
	}
};

//客户端申请替换战车
#define ZHANCHE_TIHUAN 6
struct stZhancheTihuanCmd : public stZhancheUserCmd
{
	stZhancheTihuanCmd()
	{
		byParam = ZHANCHE_TIHUAN;
	}
};

//客户端申请收回战车
#define ZHANCHE_SHOUHUI 7
struct stZhancheShouhuiCmd : public stZhancheUserCmd
{
	stZhancheShouhuiCmd()
	{
		byParam = ZHANCHE_SHOUHUI;
	}
};


//客户端申请召唤战车
#define ZHANCHE_ZHAOHUAN 8
struct stZhancheZhaohuanCmd : public stZhancheUserCmd
{
	stZhancheZhaohuanCmd()
	{
		byParam = ZHANCHE_ZHAOHUAN;
	}
};

//客户端申请归还战车
#define ZHANCHE_GUIHUAN 9
struct stZhancheGuihuanCmd : public stZhancheUserCmd
{
	stZhancheGuihuanCmd()
	{
		byParam = ZHANCHE_GUIHUAN;
	}
};

//客户端申请脱离战车
#define ZHANCHE_TUOLI 10
struct stZhancheTuoliCmd : public stZhancheUserCmd
{
	stZhancheTuoliCmd()
	{
		byParam = ZHANCHE_TUOLI;
	}
};

//客户端战车给9屏幕加状态
#define ZHANCHE_SKILL 11
struct stZhancheSkillCmd : public stZhancheUserCmd
{
	stZhancheSkillCmd()
	{
		byParam = ZHANCHE_SKILL;
	}
	DWORD type;
	DWORD playerid[100];
	DWORD itemID;
};
/////////////////////////////////////////////////////////////
/// 战车相关指令结束
//////////////////////////////////////////////////////////// 



/////////////////////////////////////////////////////////////
/// 家族BOSS相关指令 By醉梦 2025/01/06
////////////////////////////////////////////////////////////

struct stJiazuBossUserCmd : public stNullUserCmd
{
	stJiazuBossUserCmd()
	{
		byCmd = JIAZUBOSS_USERCMD;
	}
};

//客户端打开家族BOSS页面
#define JIAZUBOSS_INFO 1
struct stJiazuBossInfoCmd : public stJiazuBossUserCmd
{
	stJiazuBossInfoCmd()
	{
		byParam = JIAZUBOSS_INFO;
	}
};

//服务端返回家族BOSS页面信息
#define JIAZUBOSS_INFO_RETURN 2
struct stJiazuBossInfoReturnCmd : public stJiazuBossUserCmd
{
	stJiazuBossInfoReturnCmd()
	{
		byParam = JIAZUBOSS_INFO_RETURN;
	}
	DWORD level;
	DWORD exp;
};


//客户端申请喂养家族BOSS
#define JIAZUBOSS_WEIYANG 3
struct stJiazuBossWeiyangCmd : public stJiazuBossUserCmd
{
	stJiazuBossWeiyangCmd()
	{
		byParam = JIAZUBOSS_WEIYANG;
	}
	DWORD itemid;
};

//服务端返回喂养家族BOSS信息
#define JIAZUBOSS_WEIYANG_RETURN 4
struct stJiazuBossWeiyangReturnCmd : public stJiazuBossUserCmd
{
	stJiazuBossWeiyangReturnCmd()
	{
		byParam = JIAZUBOSS_WEIYANG_RETURN;
	}
	DWORD state;
};

//客户端申请召唤BOSS
#define JIAZUBOSS_ZHAOHUAN 5
struct stJiazuBossZhaohuanCmd : public stJiazuBossUserCmd
{
	stJiazuBossZhaohuanCmd()
	{
		byParam = JIAZUBOSS_ZHAOHUAN;
	}
};

/////////////////////////////////////////////////////////////
/// 家族BOSS相关指令结束
//////////////////////////////////////////////////////////// 


/////////////////////////////////////////////////////////////
/// 坐骑图鉴相关指令 By醉梦 2025/01/09
////////////////////////////////////////////////////////////

struct stZuoqiUserCmd : public stNullUserCmd
{
	stZuoqiUserCmd()
	{
		byCmd = ZUOQI_USERCMD;
	}
};

//客户端打开坐骑图鉴页面
#define ZUOQI_INFO 1
struct stZuoqiInfoCmd : public stZuoqiUserCmd
{
	stZuoqiInfoCmd()
	{
		byParam = ZUOQI_INFO;
	}
};

//服务端返回坐骑图鉴信息
#define ZUOQI_INFO_RETURN 2
struct stZuoqiInfoReturnCmd : public stZuoqiUserCmd
{
	stZuoqiInfoReturnCmd()
	{
		byParam = ZUOQI_INFO_RETURN;
	}
	DWORD level;
	DWORD exp;
	DWORD num1;
	DWORD num2;
	DWORD num3;
	DWORD num4;
	DWORD num5;
	DWORD num6;
	DWORD num7;
	DWORD num8;
	DWORD num9;
	DWORD num10;
	DWORD num11;
	DWORD num12;
	DWORD num13;
	DWORD num14;
	DWORD num15;
};

//客户端请求升级坐骑图鉴
#define ZUOQI_LEVELUP 3
struct stZuoqiLevelUpCmd : public stZuoqiUserCmd
{
	stZuoqiLevelUpCmd()
	{
		byParam = ZUOQI_LEVELUP;
	}
	DWORD itemid;
};

//服务端返回坐骑图鉴升级信息
#define ZUOQI_LEVELUP_RETURN 4
struct stZuoqiLevelUpReturnCmd : public stZuoqiUserCmd
{
	stZuoqiLevelUpReturnCmd()
	{
		byParam = ZUOQI_LEVELUP_RETURN;
	}
	DWORD state;
};

//客户端请求领取坐骑图鉴
#define ZUOQI_LINGQU 5
struct stZuoqiLingQuCmd : public stZuoqiUserCmd
{
	stZuoqiLingQuCmd()
	{
		byParam = ZUOQI_LINGQU;
	}
};
/////////////////////////////////////////////////////////////
/// 坐骑图鉴相关指令结束
//////////////////////////////////////////////////////////// 


/////////////////////////////////////////////////////////////
/// 迷宫相关指令 By醉梦 2025/01/26
////////////////////////////////////////////////////////////

struct stMigongUserCmd : public stNullUserCmd
{
	stMigongUserCmd()
	{
		byCmd = MIGONG_USERCMD;
	}
};

//客户端下一层迷宫
#define MIGONG_NEXT 1
struct stMigongNextCmd : public stMigongUserCmd
{
	stMigongNextCmd()
	{
		byParam = MIGONG_NEXT; 	
	}
	DWORD npcid;
};

/////////////////////////////////////////////////////////////
/// 迷宫相关指令结束
//////////////////////////////////////////////////////////// 



_CMD_NAMESPACE_END

#pragma pack()

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
