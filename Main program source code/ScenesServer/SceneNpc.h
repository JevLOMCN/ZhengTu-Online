/**
 * \file
 * \version  $Id: SceneNpc.h $
 * \author  
 * \date 
 * \brief 定义Npc
 *
 * 
 */

#ifndef _SceneNpc_h_
#define _SceneNpc_h_

#include "zSceneEntry.h"
#include "SceneDefine.h"
#include "zDatabaseManager.h"
#include "Command.h"
#include "zTime.h"
#include "zUniqueID.h"
//召唤宠物
#include <set>
#include <map>
#include "SceneEntryPk.h"
#include "zAStar.h"
#include "SceneNpcAI.h"

class SceneUser;
class NpcAIController;
class SceneNpc;

//需要进行AI动作的npc
#ifdef _POOL_ALLOC_
typedef std::set<SceneNpc *, std::less<SceneNpc *>, __gnu_cxx::__pool_alloc<SceneNpc *> > MonkeyNpcs;
#else
typedef std::set<SceneNpc *, std::less<SceneNpc *> > MonkeyNpcs;
#endif

/*
struct t_PetState
{
	DWORD id;
	Cmd::petType type;
	char name[MAX_NAMESIZE];
	DWORD hp;
	DWORD exp;
	WORD ai;
};
*/

struct t_expRec
{
	DWORD wdHP;
	zTime  attack_time;

	t_expRec()
	{
		attack_time.now();
		wdHP = 0;
	}
};

/* 一些特殊npc的ID */
const unsigned int COUNTRY_MAIN_FLAG = 58001; //soke 国旗
const unsigned int COUNTRY_SEC_FLAG  =  58002;//soke 副旗
const unsigned int COUNTRY_MAIN_GEN = 58200;  //soke 大将军王(一级)
const unsigned int COUNTRY_SEC_GEN  = 58201;  //soke 禁卫队长
const unsigned int COUNTRY_KING_MAIN_FLAG = 58005; //国旗
const unsigned int COUNTRY_KING_SEC_FLAG = 58006;  //副旗
const unsigned int COUNTRY_EMPEROR_MAIN_GEN = 58203; //大将军王
const unsigned int COUNTRY_EMPEROR_SEC_GEN = 58204; //禁卫队长

const unsigned int ALLY_GUARDNPC = 54100;//盟国镖车
const unsigned int EMPEROR_HORSE_ID = 3202;//皇帝的马ID
const unsigned int KING_HORSE_ID = 3204;//国王的马ID


/**
 * \brief 定义Npc
 *
 */
class SceneNpc : public SceneEntryPk, public zAStar<>, public zAStar<2>
{
	friend class NpcAIController;

	static const DWORD maxUniqueID = 100000;

	public:
	bool isMainGeneral();//是否大将军王

	zRTime BossDieTime;//死亡时间

	zRTime reliveTime;//复活时间

	int targetDistance;//与当前目标的最短距离
	int closeCount;//追逐计数，10步以内最短距离没有减少则认为目标不可到达

	std::list<ScenePet *> semipetList;//半宠物列表

	/**
	 * \brief 定义Npc跟踪状态
	 *
	 */
	enum SceneNpcChase
	{
		CHASE_NONE,			/// 没有跟踪状态
		CHASE_ATTACK,		/// 跟踪攻击状态
		CHASE_NOATTACK		/// 普通跟踪状态
	};

	/**
	 * \brief Npc类型
	 * 静态的还是动态分配的
	 */
	enum SceneNpcType
	{
		STATIC,				/// 静态的
		GANG				/// 动态的
	};

	/**
	 * \brief Npc基本数据
	 *
	 */
	zNpcB *npc;

	/**
	 * \brief 增强Npc基本数据
	 *
	 */
	zNpcB *anpc;

	/**
	 * \brief Npc定义数据
	 *
	 */
	const t_NpcDefine *define;

	/**
	 * \brief npc当前生命值
	 *
	 */
	DWORD hp;

	///上次发送时的hp
	DWORD lasthp;

	///回血标记
	bool needRecover;
	///下次回血的时间
	zRTime rcvTimePet;//宠物休息
	zRTime rcvTimeUnder30;//hp30以下
	zRTime rcvTimeRest;//脱离战斗
	//hp30以下回血
	bool recoverUnder30;
	//结束战斗回血
	//bool recoverLeaveBattle;

	bool checkRecoverTime(const zRTime& ct);
	void setRecoverTime(const zRTime& ct, int delay);

	virtual bool recover();
	/// npc的生命结束时间(如果没设置就是其创建时间)
	DWORD dwStandTime;

	///npc的生命持续时间
	DWORD dwStandTimeCount;

	///附加最小攻击力
	DWORD appendMinDamage;

	///附加最大攻击力
	DWORD appendMaxDamage;

	///是否已经锁定目标
	bool lockTarget;

	///攻城的npc，随攻城一起删除
	bool isRushNpc;

	///根据配置召唤其他npc
	int summonByNpcMap(std::map<DWORD, std::pair<DWORD, DWORD> > map);

	///是否召唤过
	bool summoned;

	bool setCurTarget(SceneEntryPk *, bool=false);
	bool setCurTarget(DWORD, DWORD, bool=false);
	void leaveBattle();

	void setClearState();
	bool needClear();

	bool isSpecialNpc();
	bool isFunctionNpc();
	bool isTaskNpc();

	SceneNpc(Scene *scene, zNpcB *npc, const t_NpcDefine *define, const SceneNpcType type, const SceneEntryType entrytype=SceneEntry_NPC, zNpcB *anpc=NULL);
	~SceneNpc();
	bool zPosOutofRange()const;
	SceneNpcChase getChaseMode()const;
	SceneEntryPk* getChaseSceneEntry() const;
	bool chaseSceneEntry(const DWORD type, const DWORD userid);
	virtual bool isRedNamed(bool allRedMode=true);
	void unChaseUser();
	bool checkLockUserOverdue(const zRTime &ct);
	void setLockUser(const DWORD dwID);
	bool checkMoveTime(const zRTime &ct);
	void setMoveTime(const zRTime &ct);
	void setMoveTime(const zRTime &ct, const int delay);
	void delayMoveTime(const int delay);
	bool checkAttackTime(const zRTime &ct) const;
	void setAttackTime(const zRTime &ct);
	void setAttackTime(const zRTime &ct, const int delay);
	//bool checkUnchaseTime(const zRTime &ct) const;
	//void setUnchaseTime(const zRTime &ct, const int delay);
	bool canLostObject(const zRTime &ct);
	bool canRelive(const zRTime &ct);
	bool canRush();
	const SceneNpcType &getSceneNpcType() const;
	void full_t_NpcData(Cmd::t_NpcData &data);
	void full_t_MapNpcDataState(Cmd::t_MapNpcDataState &data);
	void full_t_MapNpcDataAndPosState(Cmd::t_MapNpcDataPosState &data);
	void full_t_MapNpcData(Cmd::t_MapNpcData &data);
	void full_stRTMagicPosUserCmd(Cmd::stRTMagicPosUserCmd &ret) const;
	void full_t_MapNpcDataAndPos(Cmd::t_MapNpcDataPos &data);
	virtual void sendMeToNine();
	bool isBugbear();
	bool canBeAttack();
	void death(const zRTime &ct);
	void backoff(const int direct, const int step);
	void reduceHP(SceneUser *pAtt, DWORD wdHP);
	void distributeExp();
	void distributeMoney(DWORD money);
	void clearStateToNine(WORD state);
	void setStateToNine(WORD state);
	/**
	 * \brief 让宠物重生
	 * \author fqnewman
	 */
	virtual void relivePet(){};

	DWORD catchme; ///吸引怪物攻击自己
	int boostupPet; /// 增强比例
	DWORD boostupPetMDef; //增强宠物的法术防御
	DWORD boostupSummon; ///召唤兽攻击加强
	DWORD boostupHpMaxP;  ///增加生命值上限
	DWORD dwReduceDam;  /// 召唤兽伤害扣减
	DWORD giddy;   ///攻击的时候使对方眩晕的几率

	BYTE notifystep; //绿BOSS通知步骤

	//*
	static void AI(const zRTime& ctv, MonkeyNpcs &affectNpc,const DWORD group , const bool every);

	/**
	 * \brief 强制跟踪用户，如果怪已经在跟踪用户，那么有45%的几率将目标转换成目前的用户
	 * \param pAtt 要跟踪的对象
	 */
	bool forceChaseUser(SceneEntryPk *pAtt);
	/**
	 * \brief 将客户端消息转发到会话服务器
	 * \author fqnewman
	 */
	bool forwardSession(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen);
	//bool checkChaseUser(SceneUser *&sceneUser);
	bool checkChaseAttackTarget(SceneEntryPk *&entry);
	bool canChaseTarget(const SceneEntryPk *entry);
	bool canAttack(SceneEntryPk *entry);
	bool inRange(SceneEntryPk *entry);
	bool attackTarget(SceneEntryPk *entry = NULL);
	//bool chasedFindFrontUser( const int direct);
	//bool findAndChaseUser(const int radius, SceneEntryPk *&entry);
	//bool findAnyTarget(const int radius, SceneEntryPk *&entry);
	void action(const zRTime& ctv);
	virtual bool normalAction();
	bool deathAction();
	bool hideAction();
	bool moveable(const zPos &tempPos, const zPos &destPos, const int radius);
	bool move(const int direct, const int step);
	bool warp(const zPos &pos,bool ignore=false);//跳转
	void jumpTo(zPos &newPos);
	bool gotoFindPath(const zPos &srcPos, const zPos &destPos);
	bool goTo(const zPos &pos);
	bool shiftMove(const int direct);
	void set_quest_status(SceneUser* user);
	void setStandingTime(DWORD standTime);
	void refreshExpmapAttackTime(SceneUser* pAtt);

	/**
	 * \brief 改变角色的hp
	 * \param hp 变更的HP
	 * \author fqnewman
	 */
	void changeHP(const SDWORD &hp);

	/**
	 * \brief 造成直接伤害
	 * \param pAtt 攻击者
	 * \param dam 伤害值
	 * \param notify 通知伤害显示
	 * \author fqnewman
	 */
	//soke 属性突破 伤害越界（打怪掉血）
	SQWORD directDamage(SceneEntryPk *pAtt, const SDWORD &dam, bool notify=false);

	/**
	 * \brief 改变角色的sp
	 * \param sp 变更的SP
	 * \author fqnewman
	 */
	void changeSP(const SDWORD &sp);

	/**
	 * \brief 改变角色的mp
	 * \param mp 变更的MP
	 * \author fqnewman
	 */
	void changeMP(const SDWORD &mp);

	/**
	 * \brief 在被自己攻击之前的处理，包括，装备损耗处理，攻击有效几率判断等
	 * \param pUser 攻击者
	 * \param rev 本次攻击的触发指令
	 * \param physics 是否物理攻击
	 * \param good 为true则必中，为false需要判断回避率
	 * \author fqnewman
	 * \return true为这次攻击是有效的，false为一次无效的攻击
	 */
	bool preAttackMe(SceneEntryPk *pUser, const Cmd::stAttackMagicUserCmd *rev, bool physics=true, const bool good = false);

	/**
	 * \brief 角色被攻击
	 * \param pEntry 攻击者
	 * \param rev 本次攻击的触发消息
	 * \param physics 是否物理攻击
	 * \author fqnewman
	 * \return true为这次攻击是有效的，false为一次无效的攻击
	 */
	bool AttackMe(SceneEntryPk *pEntry, const Cmd::stAttackMagicUserCmd *rev, bool physics=true, SWORD rangDamageBonus=0);

	/**
	 * \brief 角色被击退N格
	 * \param dwAttTempID 攻击者的临时ID
	 * \param grids 后退几格
	 * \author fqnewman
	 */
	void standBack(const DWORD dwAttTempID, DWORD grids);

	/**
	 * \brief 将攻击目标换成dwTempID所指向的角色玩家
	 * \param dwTempID 目标角色的临时ID
	 * \author fqnewman
	 */
	void changeAttackTarget(const DWORD &dwTempID);

	/**
	 * \brief 让角色死亡
	 * \author fqnewman
	 */
	void toDie(const DWORD &dwTempID);

	/**
	 * \brief 通知客户端生命值的变化
	 * \author fqnewman
	 */
	void attackRTHpAndMp();

	/**
	 * \brief 发送npc血槽
	 * \author whj
	 */
	void showHP(SceneUser *pUser, QWORD npchp);

	/**
	 * \brief 检查玩家是否攻击过该npc
	 * \author whj
	 */
	bool isAttackMe(SceneEntryPk *);
	/**
	 * \brief 判断角色是否死亡
	 * \author fqnewman
	 * \return true为死亡
	 */
	bool isDie();
	/**
	 * \brief 获取角色的级别
	 * \author fqnewman
	 */
	virtual DWORD getLevel() const;

	/**
	 * \brief 需要的职业类型，决定可以使用的技能类型
	 * \author fqnewman
	 */
	bool needType(const DWORD &needtype);

	/**
	 * \brief 需要的职业类型，决定可以使用的技能类型
	 * \author fqnewman
	 */
	bool addSkillToMe(zSkill *skill);

	/**
	 * \brief 是否有该技能需要的武器
	 * \author fqnewman
	 * \return true 有 false 没有
	 */
	bool needWeapon(DWORD skillid);

	/**
	 * \brief 是否Pk区域
	 * \other PK相关人
	 * \author fqnewman
	 * \return true 是 false 否
	 */
	virtual bool isPkZone(SceneEntryPk *other=NULL);

	/**
	 * \brief 依赖物品消耗型法术
	 * \param object 消耗物品的类型
	 * \param num 消耗物品的数量
	 * \author fqnewman
	 * \return true 消耗成功 false 失败
	 */
	bool reduce(const DWORD &object, const BYTE num);

	/**
	 * \brief 检查可消耗物品是否足够
	 * \param object 消耗物品的类型
	 * \param num 消耗物品的数量
	 * \author fqnewman
	 * \return true 足够 false 不够
	 */
	bool checkReduce(const DWORD &object, const BYTE num);

	/**
	 * \brief 施放技能所导致的消耗MP,HP,SP
	 * \param base 技能基本属性对象
	 * \author fqnewman
	 * \return true 消耗成功 false 失败
	 */
	bool doSkillCost(const zSkillB *base);

	/**
	 * \brief 检查施放技能所导致的消耗MP,HP,SP是否足够
	 * \param base 技能基本属性对象
	 * \author fqnewman
	 * \return true 消耗成功 false 失败
	 */
	bool checkSkillCost(const zSkillB *base);

	/**
	 * \brief 检查自身的施放成功几率，决定这次技能是否可以施放
	 * \author fqnewman
	 * \return true 成功 false 失败
	 */
	bool checkPercent();

	/**
	 * \brief 判断是否是敌人
	 * \author fqnewman
	 * \return true 是 false 不是
	 */
	//bool isEnemy(SceneUser *pUser);

	/**
	 * \brief 死亡后给特定角色增加额外经验,包括护保和装备的额外经验
	 * \param wdExp 分配到的经验
	 * \param pUser 分配对象
	 * \return 重新计算后的经验值
	 */
	DWORD addOtherExp(DWORD wdExp , SceneUser *pUser);

	/**
	 * \brief 重置最大的hp
	 * \author fqnewman
	 */
	virtual void changeAndRefreshHMS(bool lock=true, bool sendData=true);

	/**
	 * \brief 根据等级差别重新计算经验
	 * \param wdExp 分配到的经验值
	 * \param char_level 角色等级
	 * \return 重新计算后的经验值
	 */
	DWORD levelExp(DWORD wdExp , DWORD char_level);

	/**
	 * \brief 重新发送本NPC的地图数据
	 */
	void reSendMyMapData();

	//设置宠物的主人
	virtual void setMaster(SceneEntryPk *){}

	//获取宠物的主人
	virtual SceneEntryPk *getMaster() {return 0;}
	/**
	 * \brief 得到最上层的主人
	 *
	 * \return 主人
	 */
	virtual SceneEntryPk *getTopMaster(){return this;}

	//切换场景
	bool changeMap(Scene *newScene,const zPos &pos);
	void setAspeedRate(float rate);
	void resetAspeedRate();

	/**
	 * \brief 通知选中自己的用户的hp和mp发生变化
	 */
	void sendtoSelectedHpAndMp();
	void sendtoSelectedState(DWORD state , WORD value , WORD time);

	bool createRush();
	virtual bool moveToMaster();

	//AI相关的方法
	void setSpeedRate(float rate);
	float getSpeedRate();
	void resetSpeedRate();

	//const t_NpcAIDefine& getAI();
	void setAI(const t_NpcAIDefine &ad);
	//void setActRegion(zPos pos, unsigned int regionX, unsigned int regionY);
	//void setNormalAI();
	bool doNormalAI();
	bool doSayAI();
	bool doRandomChatAI();
	bool doMovetoAI();
	bool doAttackAI();
	bool doGoOnPathAI();
	bool doPatrolAI();
	bool doFleeAI();
	bool doChangeMapAI();
	bool doWarpAI();
	bool doReturnToRegionAI();
	bool doClearAI();
	bool doDropItemAI();
	//void returnToRegion();

	virtual bool isRedNamed(bool allRedMode=true) const;
	bool useSkill(SceneEntryPk * target, DWORD id);
	virtual SceneEntryPk * chooseEnemy(SceneEntryPk_vec &);
	bool debuffEnemy(SceneEntryPk *);
	bool attackEnemy(SceneEntryPk *);
	bool moveToEnemy(SceneEntryPk *);
	virtual bool runOffEnemy(SceneEntryPk_vec &);
	virtual bool isActive();
	virtual bool canMove();
	virtual bool canFight();
	//virtual bool checkMasterTarget(SceneEntryPk *&entry){entry = 0;return false;}
	virtual void petDeath(){}
	bool healSelf();
	bool buffSelf();

	bool canReach(SceneEntryPk *);
	bool isSurrounded();
	int isEnemy(SceneEntryPk *, bool notify = false, bool good = false);
	bool getEntries(int, SceneEntryPk_vec &, int);
	bool healFellow(SceneEntryPk_vec &);
	bool buffFellow(SceneEntryPk_vec &);
	bool helpFellow(SceneEntryPk_vec &);

	virtual bool randomMove();
	void randomChat(NpcChatType type);

	//回调点
	virtual void on_reached() { }
	virtual void on_death(SceneEntryPk* att){}
	virtual void check() { }
	bool dreadProcess();
	bool blindProcess();
	int IsOppose(DWORD five);

	virtual void clearMaster();

	//zPos actPos;//活动位置
	//unsigned int actRegionX,actRegionY;//活动范围
	//t_NpcAIDefine oldAI;
	///npc控制器的指针
	NpcAIController * AIC;
	bool setScript(int id);
	void clearScript();
	void assaultMe(BYTE attacktype, DWORD tempid);

	BYTE getAType();

	///npcAI标志
	DWORD aif;
	///宠物的AI模式
	//WORD petAI;
	ScenePet * summonPet(DWORD, Cmd::petType, DWORD, DWORD, const char *, DWORD, zPos pos=zPos(0,0), BYTE dir=4);
	bool killOnePet(ScenePet *);
	void killAllPets();

	virtual void addPetExp(DWORD num, bool addPet, bool addCartoon){}
	
	virtual void full_PetDataStruct(Cmd::t_PetData &);
	virtual void setPetAI(Cmd::petAIMode){}
	virtual Cmd::petType getPetType();
	virtual void setPetType(Cmd::petType){}
	virtual void setAppendDamage(DWORD mindamage, DWORD maxdamage);
	virtual DWORD getMinMDamage();
	virtual	DWORD getMaxMDamage(); 
	virtual DWORD getMinPDamage();
	virtual	DWORD getMaxPDamage();
	virtual DWORD getMasterMana(){return 0;}
	virtual DWORD getMinMDefence();
	virtual DWORD getMaxMDefence(); 
	virtual DWORD getMinPDefence();
	virtual DWORD getMaxPDefence();
	virtual DWORD getMaxHP();
	virtual DWORD getBaseMaxHP();

	///次攻击目标
	DWORD secondTargetType;
	DWORD secondTargetID;
	bool setSecondTarget(SceneEntryPk *);
	bool setSecondTarget(DWORD, DWORD);
	bool chaseSecondTarget();
	bool chaseItsMaster();
	SceneEntryPk * getSecondTarget();
	void hideMe(int showDelay);
	void goToRandomScreen();

	/**
	 * \brief 物品保护
	 *
	 */
	DWORD dwNpcLockedUser;
protected:
	/// NPC搜索敌人的范围
	static const int npc_search_region = 5;
	/// NPC远离目标放弃追踪的范围
	static const int npc_lost_target_region = 12;
	/// NPC远离活动范围放弃追踪的距离
	static const int npc_out_of_range_region = 20;
	/// NPC主动搜索目标的范围
	static const int npc_chase_region = 6;
	/// 宠物保持在主人身边的范围
	static const int npc_pet_chase_region = 2;
	/// 宠物离主人超过此距离则加速
	static const int npc_pet_run_region = 4;
	/// 宠物离主人超过此距离则跳转
	static const int npc_pet_warp_region = 6;
	/// 远程npc攻击范围
	static const int npc_far_attack_range = 6;
private:
	/**
	 * \brief NPC可清除
	 *
	 */
	bool clearMe;

	///npc当前的AI
	t_NpcAIDefine AIDefine;

	///npc的主人
	//SceneUser * master;

	///移动速度倍率
	float speedRate;
	///攻击速度倍率
	float aspeedRate;

	///是否已经因为hp<20%提升了移动速度
	bool speedUpUnder20;

	///是否已经因为hp<50%提升了攻击速度
	bool aspeedUpUnder50;


	bool processDeath(SceneEntryPk *pAtt);

	///是否正在后退中(ms)
	int backOffing;

	/**
	 * \brief 第一次被攻击时间(测试用
	 *
	 */
	/// 第一次被攻击的时间
	zRTime first_time;

	///半秒定时器
	Timer _half_sec;

	///1秒定时器
	Timer _one_sec;

	///3秒定时器
	Timer _3_sec;

	/**
	 * \brief 跟踪方式
	 *
	 */
	///npc的跟踪方式
	SceneNpcChase chaseMode;

	//DWORD	dwNpcChasedEntryType;
	/**
	 * \brief 所跟踪目标的的编号
	 *
	 */
	//DWORD curTarget;
	//DWORD	dwNpcChasedEntryID;

	/**
	 * \brief 物品保护时间
	 *
	 */
	zRTime lockedUserTime;
	/**
	 * \brief 下一次移动时间
	 *
	 */
	zRTime nextMoveTime;
	/**
	 * \brief 下一次攻击时间
	 *
	 */
	zRTime nextAttackTime;

	///结束隐身状态的时间
	zRTime showTime;

	/**
	 * \brief 是否可以掉落物品
	 *
	 */
	bool lostObject;

	//是否进行怪物攻城的判断
	bool mayRush;

	/**
	 * \brief Npc类型
	 * 静态的还是动态分配的
	 */
	const SceneNpcType type;

	/**
	 * \brief 临时编号的线性分配器
	 * 主要在创建静态Npc的时候需要使用
	 */
	static DWORD serialID;
	/**
	 * \brief 临时编号的唯一分配器
	 * 主要在创建动态Npc的时候需要使用
	 */
	static zUniqueDWORDID uniqueID;

	//typedef __gnu_cxx::hash_map<DWORD, t_expRec> NpcHpHashmap;
	typedef std::map<DWORD, t_expRec> NpcHpHashmap;
	typedef NpcHpHashmap::iterator NpcHpHashmap_iterator;
	typedef NpcHpHashmap::const_iterator NpcHpHashmap_const_iterator;
	typedef NpcHpHashmap::value_type NpcHpHashmap_pair;
	///经验值列表
	///可以分到该npc经验的玩家列表
	NpcHpHashmap expmap;


public:
	/**
	 * \brief 设置角色的当前状态，并根据当前状态呈现角色的特效或者关闭特效
	 * \param state 状态ID 根据enum SceneEntryState取值
	 * \param isShow 是否显示效果
	 * \author fqnewman
	 * \return true为这次攻击是有效的，false为一次无效的攻击
	 */
	void showCurrentEffect(const WORD &state, bool isShow,bool notify=true);
	/**
	 * \brief NPC尸体的使用状态
	 * true为已经被使用
	 */
	bool isUse;
};

#endif

