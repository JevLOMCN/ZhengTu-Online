/**
 * \file
 * \version  $Id: SceneNpcAI.h  $
 * \author  
 * \date 
 * \brief 处理NPCAI的类和相关的数据结构定义
 * 
 * 
 */

#ifndef _SCENE_NPC_AI_H_
#define _SCENE_NPC_AI_H_

#include "zSceneEntry.h"
#include "SceneEntryPk.h"
//#include "SceneNpc.h"
#include "zTime.h"

/**
 * \brief AI类型，npc会做的基本动作
 * 走、攻击、巡逻、跳转等
 *
 */
enum SceneNpcAIType
{
	NPC_AI_NORMAL,///普通AI
	NPC_AI_SAY,///说话
	NPC_AI_MOVETO,///移动到某位置
	NPC_AI_ATTACK,///在某范围内攻击
	NPC_AI_FLEE,///逃离玩家
	NPC_AI_RETURN_TO_REGION,///回到活动范围
	NPC_AI_GO_ON_PATH,///按照一定路线移动
	NPC_AI_CHANGE_MAP,///切换地图（同一服务器内）
	NPC_AI_WARP,///同一地图内瞬间移动
	NPC_AI_PATROL,///巡逻
	NPC_AI_CLEAR,///清除该npc
	NPC_AI_WAIT,///等待，什么也不做
	NPC_AI_GUARD_DOOR,///门卫
	NPC_AI_GUARD_ARCHER,///弓卫
	NPC_AI_GUARD_GUARD,///侍卫
	NPC_AI_GUARD_PATROL,///巡逻卫士
	NPC_AI_DROP_ITEM,///丢东西
	NPC_AI_CALL_PET,///招宠物
	NPC_AI_RANDOM_CHAT///随机说话
};

///npcAI标志定义
enum NpcAIFlag
{
	AIF_ATK_PDEF            = 0x00000001,   ///优先攻击物防最低的敌人
	AIF_ATK_MDEF            = 0x00000002,   ///优先攻击魔防最低的敌人
	AIF_ATK_HP              = 0x00000004,   ///优先攻击生命值最低的敌人
	AIF_GIVEUP_10_SEC       = 0x00000008,   ///追逐10秒放弃目标
	AIF_GIVEUP_6_SEC        = 0x00000010,   ///6秒未受到伤害放弃目标
	AIF_GIVEUP_3_SEC        = 0x00000020,   ///3秒未受到伤害放弃目标
	AIF_FLEE_30_HP		= 0x00000040,   ///HP30%以下逃跑4格
	AIF_FLEE_3_ENEMY_4	= 0x00000080,   ///被3个以上敌人围攻逃跑4格
	AIF_NO_BATTLE		= 0x00000100,	///非战斗npc
	AIF_NO_MOVE		= 0x00000200,   ///不移动（弓卫、买卖、路标等）
	AIF_WARP_MOVE		= 0x00000400,   ///瞬移方式移动
	AIF_CALL_FELLOW_7       = 0x00000800,   ///召唤7*7范围的未激活npc(几率50%)
	AIF_CALL_FELLOW_9       = 0x00001000,   ///召唤9*9范围的未激活npc（几率50%）
	AIF_CALL_BY_ATYPE       = 0x00002000,   ///召唤同种攻击类型的同伴（与上两个标志合作）
	AIF_HEAL_FELLOW_5       = 0x00004000,   ///治愈5*5范围内的HP50%以下的同伴
	AIF_BUFF_FELLOW_5	= 0x00008000,   ///给5*5范围内的同伴buff
	AIF_HELP_FELLOW_5	= 0x00010000,	///帮助5*5范围内的同伴攻击（用于被动npc）
	AIF_ATK_MASTER		= 0x00020000,	///直接攻击宠物的主人
	AIF_ATK_REDNAME		= 0x00040000,	///攻击红名的玩家
	AIF_DOUBLE_REGION	= 0x00080000,	///搜索范围加倍
	AIF_SPD_UP_HP20		= 0x00100000,	///hp20%以下移动速度加倍
	AIF_ASPD_UP_HP50	= 0x00200000,	///hp50%以下攻击速度加倍
	AIF_ACTIVE_MODE		= 0x00400000,	///主动攻击
	AIF_RUN_AWAY		= 0x00800000,	///逃离敌人
	AIF_LOCK_TARGET		= 0x01000000,	///不切换攻击目标直到死
	AIF_RCV_UNDER_30	= 0x02000000,	///hp30%以下持续回血1%
	AIF_RCV_REST		= 0x04000000,	///脱离战斗30秒后回血一次5%
	AIF_SUMMON_UNDER_60	= 0x08000000,	///hp60%以下召唤宠物
	AIF_LIMIT_REGION	= 0x10000000	///限制活动范围
};

///npc说话的类型
enum NpcChatType
{
	NPC_CHAT_ON_FIND_ENEMY = 1,	///发现敌人
	NPC_CHAT_ON_ATTACK,	///攻击时说的话
	NPC_CHAT_ON_RETURN,	///追逐返回时说的话
	NPC_CHAT_ON_DIE,	///死的时候说的话
	NPC_CHAT_ON_FLEE,	///逃跑时说的话
	NPC_CHAT_ON_HIT,	///被打时说的话
	NPC_CHAT_ON_HELP,	///帮助同伴时说的话
	NPC_CHAT_ON_BE_HELP,	///同伴来帮助时说的话
	NPC_CHAT_RANDOM		///随机说话
};

/**
 * \brief 一个AI的定义
 *
 */
struct t_NpcAIDefine
{
	///类型，NPC在该阶段的主要动作
	SceneNpcAIType type;
	///位置 根据不同动作位置的意义也略不相同
	///移动时表示目的地，其他表示活动范围中心
	zPos pos;
	///范围 
	///移动时表示到达目的地的判定范围，其他表示活动范围
	int regionX, regionY;
	//zRTime endTime;
	///该AI的持续时间
	int lasttime;

	///是否正在逃跑
	bool flee;
	///逃跑的方向
	int fleeDir;
	///逃跑计数
	int fleeCount;

	///切换地图时，要去的地图
	///说话时，要说的话
	char str[MAX_CHATINFO];


	/**
	 * \brief 默认构造函数
	 *
	 */
	t_NpcAIDefine()
		:type(NPC_AI_NORMAL), pos(zPos(0,0)), regionX(2), regionY(2), lasttime(0), flee(false), fleeDir(-1), fleeCount(0)
		{
			bzero(str, sizeof(str));
		}
	
	/**
	 * \brief 构造函数
	 *
	 *
	 * \param type AI类型
	 * \param pos 位置
	 * \param regionX 范围宽
	 * \param regionY 范围高
	 * \param lasttime 持续时间
	 * \return 
	 */
	t_NpcAIDefine(SceneNpcAIType type, const zPos &pos, int regionX, int regionY, int lasttime)
		:type(type), pos(pos), regionX(regionX), regionY(regionY), lasttime(lasttime)
		{
			bzero(str, sizeof(str));
		}


	/**
	 * \brief 拷贝构造函数
	 *
	 * \param ad 要复制的对象
	 */
	t_NpcAIDefine(const t_NpcAIDefine &ad)
	{
		type = ad.type;
		pos = ad.pos;
		regionX = ad.regionX;
		regionY = ad.regionY;
		lasttime = ad.lasttime;
		flee = ad.flee;
		fleeDir = ad.fleeDir;
		fleeCount = ad.fleeCount;
		strncpy(str, ad.str, sizeof(str)-1);
	}

	/**
	 * \brief 赋值
	 *
	 * \param ad 要拷贝的对象
	 * \return 返回自身地址
	 */
	t_NpcAIDefine & operator = (const t_NpcAIDefine &ad)
	{
		type = ad.type;
		pos = ad.pos;
		regionX = ad.regionX;
		regionY = ad.regionY;
		lasttime = ad.lasttime;
		flee = ad.flee;
		fleeDir = ad.fleeDir;
		fleeCount = ad.fleeCount;
		strncpy(str, ad.str, sizeof(str)-1);

		return *this;
	}
};

class SceneNpc;

/**
 * \brief AI控制器
 * 控制器可以读取NPC脚本，使NPC按照脚本来动作
 * 
 * 控制器处理各种事件，时间、被攻击、死亡等
 * 根据不同的条件为NPC设置不同的AI
 *
 */
class NpcAIController
{
	static const int npc_call_fellow_rate;///NPC召唤同伴的几率
	static const int npc_one_checkpoint_time;///NPC按照路线移动时，走一个路点的最长时间
	static const int npc_checkpoint_region;///NPC移动，到达一个路点的判定范围
	static const int npc_onhit_stop_time;///任务NPC移动中被攻击时，停止的时间
	static const int npc_flee_distance;///NPC逃离攻击者的距离
	static const int npc_min_act_region;///NPC最小活动范围
	
	///AI容器，AI按照在容器中的顺序依次执行
	std::vector<t_NpcAIDefine> phaseVector;
	///当前的AI索引
	unsigned int curPhase;
	///当前AI的结束时间
	zRTime phaseEndTime;

	///脚本重复次数
	///-1：无限循环  0：停止  >0：循环次数
	int repeat;

	///是否起用了脚本
	bool active;
	void nextPhase(int index);
	void on_phaseEnd();
	SceneNpcAIType parseAction(char *);
	
	///本控制器控制的npc
	SceneNpc * npc;

	///当前的AI和保存的前一个AI
	t_NpcAIDefine curAI,oldAI;
	///活动范围的中心
	zPos actPos;
	///活动范围的宽和高
	int actRegionX,actRegionY;
	bool outOfRegion() const;
	void returnToRegion();

	bool arrived(zPos pos = zPos(0,0), int x = -1, int y = -1);
	bool dstReached();
	///是否到达目的地
	bool reached;

	///目的地图
	char dstMap[32];
	///目的位置
	zPos dstPos;

public:
	void setAI(const t_NpcAIDefine ai, const bool setTime = true);
	void switchAI(const bool setTime = true);
	void setNormalAI();
	void setActRegion(zPos pos = zPos(0,0), int x = -1, int y = -1);
	void getActRegion(zPos &, int &, int  &);
	NpcAIController(SceneNpc *);
	bool loadScript(unsigned int id);
	void unloadScript();
	void processPhase();
	void setRepeat(int repeat);
	int getRepeat();
	bool isActive();
	void setPhaseTime(const int delay);
	void delayPhaseTime(const int delay);
	bool phaseTimeOver();

	void on_relive();
	void on_find_enemy(const SceneEntryPk *);
	void on_hit(SceneEntryPk *pAtk);
	void on_die();
};
#endif
