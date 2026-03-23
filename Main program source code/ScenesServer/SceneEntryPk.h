/**
 * \file
 * \version  $Id: SceneEntryPk.h $
 * \author 
 * \date 
 * \brief PK相关变量和虚方法
 */

#ifndef _ZSCENEENTRYPK_H_
#define _ZSCENEENTRYPK_H_

#include <map>
#include <set>
#include <ext/pool_allocator.h>
#include "zSceneEntry.h"
#include "Zebra.h"
#include "ScenePk.h"
#include "SkillStatusManager.h"
#include "zSceneEntryIndex.h"

class Scene;
class zSkill;
class zSkillB;
struct SceneNpc;
class ScenePet;


/**
 * \brief 角色PK定义类,有待扩充
 */
struct SceneEntryPk: public zSceneEntry
{
	public:
		typedef std::set<DWORD> SelectedSet;
		typedef SelectedSet::value_type SelectedSet_value_type;
		typedef SelectedSet::iterator SelectedSet_iterator;
		///被选中列表
		SelectedSet selected;
		//zMutex selected_lock;
	public:
		/**
		 * \brief 构造函数，初始化
		 *
		 *
		 * \param type 类型
		 * \param state 初始状态
		 * \return 
		 */
		SceneEntryPk(SceneEntryType type, const SceneEntryState state = SceneEntry_Normal):zSceneEntry(type,state)
		{
			attackAction = true;
			moveAction = true;
			skillAction = true;
			ignoreDam = false;
			ignorePhysicDam = false; ///物理攻击免疫
			ignoreMagicDam = false;  ///魔法攻击免疫
			blind  = false;
			mplock = false;
			hplock = false;
			splock = false;
			maxattack=false;
			mdeftozero=false;   /// 魔法防御变成0
			pdeftozero=false;   /// 魔法防御变成0
			useRes=true;		// 可否使用物品
			isAutoRestitute=true;// 自动恢复HP,MP,SP
			frenzy = false; // 狂暴状态
			huntermark = false; //
			isPhysics =  false; // 走物理计算
			dread = false;// 恐吓
			dropweapon = false; //缴械
			tosh = false; //胡言乱语
			stone = false; // 石化
			watchTrap = false; //观察陷阱
			mhpd = false;
			x_mhpd = false;
			x1_mhpd = false;
			x2_mhpd = false;
			x3_mhpd = false;
			x4_mhpd = false;
			x5_mhpd = false;
			x6_mhpd = false;
			blazeflag = false; // 火焰系法术加成标志
			levinflag = false; // 雷电系法术加成标志
			trapflag = false; // 陷阱系法术加成标志
			iceflag = false; // 冰系法术加成标志
			swapdamcharid = 0; //  献身，施法者的临时ID
			swapdamcharid2 = 0; //关爱，伤害转移施法者的临时ID
			reSendData = false;
			changeface = false; // 易容术可以访问国外NPC
			hideme = false; //隐藏自己
			icebox = false; //冰灵柜状态
			liquidState = false; //液态（无阻挡）
			resist = 0;
			dmpbyhp = 0; //吸魔剑法
			mdmtomp = 0; //魔法伤害转换成魔法值
			mdmtompper = 0; //魔法伤害转换成魔法值几率
			icedam	= 0; //冰冻结界吸收的伤害值
			unitaryBattleKeepTime = 0;

			afreetMode = false; /// 恶魔杀无赦（砍人必死）
			angelMode = false;  /// 天使无敌（优于恶魔，砍不死）
			isPhysicBang = false;///是否暴击
			isMagicBang = false;///是否暴击
			isHPhysicBang = false;///是否暴击
			isHMagicBang = false;///是否暴击
			assault = false;
			curMagicManType =0;
			curMagicManID =0;
			lastPkTime=0;
			bombskillId = 0;
			dietodam = 0;
			attacklow = false; ///攻击力取下限
			killnow=0; ///一击必杀百分比
			diewithme=0; //血债血偿
			switchdie=0; //同屏内转移死亡状态
			summonsplit = 0; // 焰魔君主死亡后分裂标志
			damtohpper = 0;	/// 角色的伤害转化成生命值的几率;
			damtohpvalper = 0; /// 角色的伤害转化成生命值的百分比 
			reSendOther=false; 
		}
		virtual ~SceneEntryPk(){};
		///pk状态
		ScenePkState pkState;
		///PK过程中变化的数值
		PkValue pkValue;
		///技能附加的状态
		SkillState skillValue;
		///PK过程相对不便的数值
		PkPreValue pkpreValue;

		SkillStatusManager skillStatusM;	/// 技能状态管理器
		SkillStatusCarrier carrier;			/// 技能操作投送器
		//std::map<BYTE,BYTE>	_effectState;	/// 技能状态特效计数器
		SceneEntryPk *attackTarget;			/// 武器附带攻击状态攻击目标

		bool attackAction;	/// 攻击许可
		bool moveAction;	/// 移动许可
		bool skillAction;   /// 技能攻击许可
		bool ignoreDam;		/// 忽略伤害
		bool ignorePhysicDam; /// 忽略物理伤害
		bool ignoreMagicDam; /// 忽略魔法伤害
		bool blind;        /// 失明状态
		bool notifyHMS;		/// 通知HP,MP,SP改变
		bool maxattack;    /// 攻击取上限
		bool mdeftozero;   /// 魔法防御变成0
		bool pdeftozero;   /// 魔法防御变成0
		bool useRes;		/// 可否使用物品
		bool isAutoRestitute; // 是否自动恢复
		bool frenzy;       /// 狂暴状态
		bool huntermark;	/// 猎手标记
		bool isPhysics;		/// 计算公式走物理
		bool dread;			/// 恐惧
		bool dropweapon;   /// 缴械
		bool tosh;        /// 胡言乱语
		bool stone;			/// 石化
		bool watchTrap;    /// 观察陷阱
		bool mhpd;  /// 魔法伤害减半，物理伤害加倍标志。
		bool x_mhpd;  /// 物理魔法伤害减少 新
		bool x1_mhpd;  /// 物理魔法伤害减少 新
		bool x2_mhpd;  /// 物理魔法伤害减少 新
		bool x3_mhpd;  /// 物理魔法伤害减少 新
		bool x4_mhpd;  /// 物理魔法伤害减少 新
		bool x5_mhpd;  /// 物理魔法伤害减少 新
		bool x6_mhpd;  /// 物理魔法伤害减少 新
		bool blazeflag; ///火焰系法术加成标志
		bool levinflag;  ///雷电系法术加成标志
		bool trapflag;  ///陷阱系法术加成标志
		bool iceflag;  ///冰系法术加成标志
		DWORD swapdamcharid; ///献身施法者的临时ID
		DWORD swapdamcharid2; ///关爱施法着的临时ID
		bool changeface;  ///易容术，可以访问国外NPC
		bool hideme;     /// 隐藏自己
		bool icebox;	 /// 冰灵柜状态
		bool liquidState; ///液态（无阻挡）
		BYTE resist;     ///格档，有一次抵御伤害的机会
		BYTE dmpbyhp;  ///吸魔剑法
		BYTE mdmtomp;  ///将法术伤害转成魔法值百分比
		BYTE mdmtompper; ///将法术伤害转成魔法值百分比几率
		SDWORD icedam;  ///冰冻结界吸收的伤害数值
		unsigned long long unitaryBattleKeepTime; //一元阵法持续时间
		SDWORD dietodam; ///死亡攻击数值
		bool attacklow; ///攻击力取下限
		BYTE killnow; //一击必杀
		BYTE diewithme; //血债血偿
		BYTE switchdie; //同屏转移死亡状态的几率
		BYTE summonsplit; //焰魔君主死亡后分裂标志


		bool reSendData;	/// 重新发送NPC或角色数据给自己
		bool reSendOther;	/// 重新发送NPC或角色数据给其它人

		bool mplock;	///对mp进行锁定
		bool hplock;	///对hp进行锁定
		bool splock;	///对sp进行锁定

		bool afreetMode; /// 恶魔杀无赦（砍人必死）
		bool angelMode;  /// 天使无敌（优于恶魔，砍不死）

		bool isPhysicBang; /// 是否物理暴击
		bool isMagicBang; /// 是否魔法暴击
		bool isHPhysicBang; /// 是否物理暴击
		bool isHMagicBang; /// 是否魔法暴击

		bool assault; ///冲锋

		zPos keepPos;  /// 保存技能的攻击目标点，传递给需要使用的功能
		BYTE keepDir;  /// 保存技能的攻击方向，传递给需要使用的功能
		BYTE  lastPkTime; //最后PK标志
		DWORD bombskillId; /// 人物死亡后投送技能ID

		BYTE damtohpper;	/// 角色的伤害转化成生命值的几率;
		BYTE damtohpvalper; /// 角色的伤害转化成生命值的百分比 


		///所在的场景指针
		Scene *scene;

		//技能
		UserSkillM usm;

		virtual void sendtoSelectedHpAndMp() = 0;
		virtual void sendtoSelectedState(DWORD state , WORD value , WORD time) = 0;
		virtual void sendSevenStateToMe(DWORD state , WORD value , WORD time){}
		void sendCmdToSelected(const void *pstrCmd, const unsigned int nCmdLen);

		/**
		 * \brief 造成直接伤害
		 * \param pAtt 攻击者
		 * \param dam 伤害
		 * \param notify 是否通知客户端
		 * \author fqnewman
		 * \return 伤害值
		 */
		//soke 属性突破 伤害越界（打怪掉血）
		virtual SQWORD directDamage(SceneEntryPk *pAtt, const SDWORD &dam, bool notify=false);

		/**
		 * \brief 改变角色的hp
		 * \param hp 变更的HP
		 * \author fqnewman
		 */
		virtual void changeHP(const SDWORD &hp)=0;

		/**
		 * \brief 改变角色的sp
		 * \param sp 变更的SP
		 * \author fqnewman
		 */
		virtual void changeSP(const SDWORD &sp)=0;

		/**
		 * \brief 改变角色的mp
		 * \param mp 变更的MP
		 * \author fqnewman
		 */
		virtual void changeMP(const SDWORD &mp)=0;

		/**
		 * \brief 获得最大的hp
		 * \author fqnewman
		 * \return 返回最大值
		 */
		virtual DWORD getMaxHP(){return 0;}

		/**
		 * \brief 获得最大的hp
		 * \author fqnewman
		 * \return 返回最大值
		 */
		virtual DWORD getBaseMaxHP(){return 0;}

		/**
		 * \brief 获得最大的mp
		 * \author fqnewman
		 * \return 返回最大值
		 */
		virtual DWORD getMaxMP(){return 0;}

		/**
		 * \brief 获得最大的mp
		 * \author fqnewman
		 * \return 返回最大值
		 */
		virtual DWORD getBaseMaxMP(){return 0;}

		/**
		 * \brief 获得最大的sp
		 * \author fqnewman
		 * \return 返回最大值
		 */
		virtual DWORD getMaxSP(){return 0;}

		/**
		 * \brief 获得当前魔法攻击力
		 * \author fqnewman
		 * \return 魔法攻击力
		 */
		virtual DWORD getMaxMDamage(){return 0;}

		/**
		 * \brief 获得当前物理攻击力
		 * \author fqnewman
		 * \return 物理攻击力
		 */
		virtual DWORD getMaxPDamage(){return 0;}

		/**
		 * \brief 获得当前物理防御力
		 * \author fqnewman
		 * \return 物理防御力
		 */
		virtual DWORD getPDefence(){return 0;}

		/**
		 * \brief 获得当前魔法防御力
		 * \author fqnewman
		 * \return 魔法防御力
		 */
		virtual DWORD getMDefence(){return 0;}

		/**
		 * \brief 重置最大的hp
		 * \author fqnewman
		 */
		virtual void changeAndRefreshHMS(bool lock=true, bool sendData=true){}

		/**
		 * \brief 在被自己攻击之前的处理，包括，装备损耗处理，攻击有效几率判断等
		 * \param pUser 攻击者
		 * \param rev 本次攻击的触发指令
		 * \param physics 是否物理攻击
		 * \param good 为true则必中，false要判断回避率
		 * \author fqnewman
		 * \return true为这次攻击是有效的，false为一次无效的攻击
		 */
		virtual bool preAttackMe(SceneEntryPk *pUser, const Cmd::stAttackMagicUserCmd *rev, bool physics=true, const bool good = false)=0;

		/**
		 * \brief 角色被攻击
		 * \param pUser 攻击者
		 * \param rev 本次攻击的触发消息
		 * \param physics 是否物理攻击
		 * \author fqnewman
		 * \return true为这次攻击是有效的，false为一次无效的攻击
		 */
		virtual bool AttackMe(SceneEntryPk *pUser, const Cmd::stAttackMagicUserCmd *rev, bool physics=true, SWORD rangDamageBonus=0);

		/**
		 * \brief 设置角色身上的特效状态，对应状态的访问计数将累加
		 * \param state 状态ID 根据enum SceneEntryState取值
		 * \author fqnewman
		 */
		//void setEffectStatus(const WORD &state)
		//{
			//_effectState[state]=_effectState[state]+1;
			//if (1 == _effectState[state])
			//{
				//showCurrentEffect(state, true);
			//}
		//}


		/**
		 * \brief 清除角色身上的特效状态，对应状态的访问计数将减，减为0的时候清除该状态
		 * \param state 状态ID 根据enum SceneEntryState取值
		 * \author fqnewman
		 */
		//void clearEffectStatus(const WORD &state)
		//{
			//_effectState[state]=_effectState[state]-1;
			//if (0 == _effectState[state])
			//{
				//showCurrentEffect(state, false);
			//}
		//}

		/**
		 * \brief 让宠物重生
		 * \author fqnewman
		 */
		virtual void relivePet(){};

		/**
		 * \brief 让角色重生
		 * \author fqnewman
		 */
		virtual void relive(){};

		/**
		 * \brief 得到玩家的权限等级
		 * \return 玩家的权限等级
		 */
		virtual BYTE getPriv(){return 0;}

		/**
		 * \brief 角色被击退N格
		 * \author fqnewman
		 */
		virtual void standBack(const DWORD dwAttTempID, DWORD grids)=0;

		/**
		 * \brief 将攻击目标换成dwTempID所指向的角色玩家
		 * \param dwTempID 目标角色的临时ID
		 * \author fqnewman
		 */
		virtual void changeAttackTarget(const DWORD &dwTempID){};

		/**
		 * \brief 让角色死亡
		 * \param dwTempID 攻击者的临时ID
		 * \author fqnewman
		 */
		virtual void toDie(const DWORD &dwTempID)=0;

		/**
		 * \brief 在屏幕内随机移动
		 * \author fqnewman
		 */
		virtual void goToRandomScreen(){};

		/**
		 * \brief 在屏幕内定点移动
		 * \author fqnewman
		 */
		virtual void jumpTo(zPos &newPos){};

		/**
		 * \brief 通知客户端生命值的变化
		 * \author fqnewman
		 */
		virtual void attackRTHpAndMp(){};

		/**
		 * \brief 判断角色是否死亡
		 * \author fqnewman
		 */
		virtual bool isDie() = 0;

		/**
		 * \brief 获取角色的级别
		 * \author fqnewman
		 */
		virtual DWORD getLevel() const =0;

		/**
		 * \brief 获取五行点数
		 * \author whj
		 * \return 返回该类型的五行点数
		 */
		virtual DWORD getFiveType() {return 0;}

		/**
		 * \brief 需要的职业类型，决定可以使用的技能类型
		 * \author fqnewman
		 */
		virtual bool needType(const DWORD &needtype)=0;

		/**
		 * \brief 需要的职业类型，决定可以使用的技能类型
		 * \author fqnewman
		 */
		virtual bool addSkillToMe(zSkill *skill)=0;

		/**
		 * \brief 是否有该技能需要的武器
		 * \author fqnewman
		 * \return true 有 false 没有
		 */
		virtual bool needWeapon(DWORD skillid)=0;

		/**
		 * \brief 是否Pk区域
		 * \param other PK相关人
		 * \author fqnewman
		 * \return true 是 false 否
		 */
		virtual bool isPkZone(SceneEntryPk *other=NULL)=0;

		/**
		 * \brief 依赖物品消耗型法术
		 * \param object 消耗物品的类型
		 * \param num 消耗物品的数量
		 * \author fqnewman
		 * \return true 消耗成功 false 失败
		 */
		virtual bool reduce(const DWORD &object, const BYTE num)=0;

		/**
		 * \brief 检查可消耗物品是否足够
		 * \param object 消耗物品的类型
		 * \param num 消耗物品的数量
		 * \author fqnewman
		 * \return true 足够 false 不够
		 */
		virtual bool checkReduce(const DWORD &object, const BYTE num)=0;
		/**
		 * \brief 获取装备伤害加成
		 * \author fqnewman
		 * \return 伤害加成
		 */
		virtual WORD getDamageBonus() {return 0;}

		/**
		 * \brief 施放技能所导致的消耗MP,HP,SP
		 * \param base 技能基本属性对象
		 * \author fqnewman
		 * \return true 消耗成功 false 失败
		 */
		virtual bool doSkillCost(const zSkillB *base) = 0;

		/**
		 * \brief 检查是否骑马
		 * \author fqnewman
		 * \return true 骑马 false 没骑
		 */
		virtual bool checkMountHorse() {return false;}

		/**
		 * \brief 检查施放技能所导致的消耗MP,HP,SP是否足够
		 * \param base 技能基本属性对象
		 * \author fqnewman
		 * \return true 消耗成功 false 失败
		 */
		virtual bool checkSkillCost(const zSkillB *base) = 0;

		/**
		 * \brief 检查自身的施放成功几率，决定这次技能是否可以施放
		 * \author fqnewman
		 * \return true 成功 false 失败
		 */
		virtual bool checkPercent()=0;

		/**
		 * \brief 判断是否是敌人
		 * \author fqnewman
		 * \return true 是 false 不是
		 */
		virtual int isEnemy(SceneEntryPk *entry, bool notify = false, bool good = false)=0;

		/**
		 * \brief 判断是否可攻击
		 * \author fqnewman
		 * \return true 是 false 不是
		 */
		virtual bool isBugbear(){return true;}

		/**
		 * \brief 获取自己的主人，一般针对NPC而言，Player的主人是自己
		 * \author fqnewman
		 * \return NULL或者主人的对象指针
		 */
		virtual SceneEntryPk *getMaster()=0 ;

		/**
		 * \brief 得到最上层的主人
		 *
		 * \return 主人
		 */
		virtual SceneEntryPk *getTopMaster() =0;

		/**
		 * \brief 检查飞行路径
		 * \param pTarget 被攻击者
		 * \param aType 飞行方式
		 * \author fqnewman
		 * \return 是否顺利攻击
		 */
		virtual bool checkMagicFlyRoute(zSceneEntry *pTarget, BYTE aType);

		/**
		 * \brief 角色死亡检查
		 * \param pAtt 攻击者
		 * \author fqnewman
		 */
		virtual bool processDeath(SceneEntryPk *pAtt);
		virtual void leaveBattle();

		/**
		 * \brief 通过杀死比自己等级高的npc来减少善恶度
		 *
		 * \param pNpc 杀死的npc
		 */
		virtual void reduceGoodness(SceneNpc *pNpc){};

		virtual void setupCharBase(bool lock=true) {};
		/**
		 * \brief 重新发送本角色的地图数据
		 */
		virtual void reSendMyMapData()=0;

		/**
		 * \brief 攻击者冲向我
		 * \param attacktype 攻击者的类型
		 * \param tempid 攻击者的临时id
		 * \author fqnewman
		 */
		virtual void assaultMe(BYTE attacktype, DWORD tempid) {}

		/**
		 * \brief 召唤宠物
		 */
		//virtual ScenePet* summonPet(DWORD id, Cmd::petType type) {return NULL;}
		/**
		 * \brief 召唤图腾
		 */
		virtual ScenePet* summonPet(DWORD id, Cmd::petType type, DWORD standTime = 0, DWORD sid=0, const char * petName = "", DWORD anpcid=0, zPos pos=zPos(0,0), BYTE dir=4) = 0;

		/**
		 * \brief 设置战斗类宠物的跟踪攻击目标
		 */
		virtual void setPetsChaseTarget(SceneEntryPk *entry){};

		/**
		 * \brief 设置战斗类宠物的跟踪攻击目标
		 * \param weaponType 武器类型
		 * \return true武器类型符合，false武器类型不符合
		 */
		virtual bool checkWeapon(BYTE weaponType){return true;}


		/**
		 * \brief 获取抗毒增加	
		 */
		virtual SWORD getPoisondef(){return 0;}
		/**
		 * \brief 获取抗麻痹增加	
		 */
		virtual SWORD getLulldef(){return 0;}
		/**
		 * \brief 获取抗眩晕增加	
		 */
		virtual SWORD getReeldef(){return 0;}
		/**
		 * \brief 获取抗噬魔增加	
		 */
		virtual SWORD getEvildef(){return 0;}
		/**
		 * \brief 获取抗噬力增加	
		 */
		virtual SWORD getBitedef(){return 0;}
		/**
		 * \brief 获取抗混乱增加	
		 */
		virtual SWORD getChaosdef(){return 0;}
		/**
		 * \brief 获取抗冰冻增加	
		 */
		virtual SWORD getColddef(){return 0;}
		/**
		 * \brief 获取抗石化增加	
		 */
		virtual SWORD getPetrifydef(){return 0;}
		/**
		 * \brief 获取抗失明增加	
		 */
		virtual SWORD getBlinddef(){return 0;}
		/**
		 * \brief 获取抗定身增加	
		 */
		virtual SWORD getStabledef(){return 0;}
		/**
		 * \brief 获取抗减速增加	
		 */
		virtual SWORD getSlowdef(){return 0;}
		/**
		 * \brief 获取抗诱惑增加
		 */
		virtual SWORD getLuredef(){return 0;}


		/**
		 * \brief 获取毒增加	
		 */
		virtual SWORD getPoison(){return 0;}
		/**
		 * \brief 获取麻痹增加	
		 */
		virtual SWORD getLull(){return 0;}
		/**
		 * \brief 获取眩晕增加	
		 */
		virtual SWORD getReel(){return 0;}
		/**
		 * \brief 获取噬魔增加	
		 */
		virtual SWORD getEvil(){return 0;}
		/**
		 * \brief 获取噬力增加	
		 */
		virtual SWORD getBite(){return 0;}
		/**
		 * \brief 获取混乱增加	
		 */
		virtual SWORD getChaos(){return 0;}
		/**
		 * \brief 获取冰冻增加	
		 */
		virtual SWORD getCold(){return 0;}
		/**
		 * \brief 获取石化增加	
		 */
		virtual SWORD getPetrify(){return 0;}
		/**
		 * \brief 获取失明增加	
		 */
		virtual SWORD getBlind(){return 0;}
		/**
		 * \brief 获取定身增加	
		 */
		virtual SWORD getStable(){return 0;}
		/**
		 * \brief 获取减速增加	
		 */
		virtual SWORD getSlow(){return 0;}
		/**
		 * \brief 获取诱惑增加
		 */
		virtual SWORD getLure(){return 0;}


		virtual void appendAttack(const Cmd::stAttackMagicUserCmd *rev) {};


		///宠物
		ScenePet * pet;
		///召唤兽
		ScenePet * summon;
		///图腾
		std::list<ScenePet *> totems;

		/**
		 * \brief 取消召唤的宠物
		 */
		virtual bool killOnePet(ScenePet *)=0;
		virtual void killAllPets()=0;
		virtual void killSummon() {};

		virtual void sendPetDataToNine();
		virtual void addPetExp(DWORD,bool,bool) = 0;
		void petLevelUp(ScenePet *);

		virtual bool isRedNamed(bool allRedMode=true) const = 0;
		DWORD getHp();
		DWORD getMaxHp();
		bool isFighting();
		///脱离战斗状态的时间
		zRTime endBattleTime;
		///施法者
		DWORD curMagicManID;
		///对施法者类型
		DWORD curMagicManType;
		///对手
		DWORD curTargetID;
		///对手类型
		DWORD curTargetType;
		virtual bool setCurTarget(SceneEntryPk *, bool=false);
		virtual bool setCurTarget(DWORD, DWORD, bool=false);
		SceneEntryPk * getCurTarget();

		///防御的对手
		DWORD defTargetID;
		///防御的对手类型
		DWORD defTargetType;
		virtual bool setDefTarget(SceneEntryPk *, bool=false);
		virtual bool setDefTarget(DWORD, DWORD, bool=false);
		SceneEntryPk * getDefTarget();
		void clearDefTarget();


		void setEndBattleTime(const zRTime &, int);
		bool checkEndBattleTime(const zRTime &);

		virtual WORD skillUpLevel(WORD skilltype, WORD skillkind){return 0;}

		virtual void getSummonAppendDamage(DWORD &minDamage, DWORD &maxDamage){};
		virtual void exchangeMeAndSummonPet(){};
		virtual DWORD getMasterMana(){return 0;}
		virtual DWORD getMP(){return 0;}
		virtual void clearMana(){}
		virtual void leech(DWORD dwDam) {}
		virtual void sendMessageToMe(const char *pattern) {};
		virtual void setSpeedRate(float rate){};
		virtual void resetSpeedRate(){};
		/**
		 * \brief 设置角色的当前状态，并根据当前状态呈现角色的特效或者关闭特效
		 * \param state 状态ID 根据enum SceneEntryState取值
		 * \param isShow 是否显示特效
		 * \author fqnewman
		 */
		virtual void showCurrentEffect(const WORD &state, bool isShow,bool notify=true) = 0;
	private:
		virtual void processMaskOnAttack(SceneEntryPk *){};
		virtual void processMaskOnDefence(){};
		virtual void processAddDam(int &dwDam, int &dwDamDef, bool physics){};
		virtual void reduceDam(int &dwDam, int &dwDamDef, bool physics){};
		virtual void reflectDam(int &dwDamDef, int &dwDamSelf, bool physics){};
		virtual void hp2mp(int &dwDamDef){};
		virtual void reflectSkill(SceneEntryPk *pAtt, const Cmd::stAttackMagicUserCmd *rev){};
};


/**
 * \brief 伤害转移回调函数，随机将伤害转移到一屏内的某一个可伤害对象
 * \author fqnewman
  */
struct DamagCallback: public zSceneEntryCallBack
{
	zSceneEntry *me;
	SceneEntryPk *target;
	DamagCallback(SceneEntryPk *entry)
	{
		me = entry;
		target = NULL;
	}
	bool exec(zSceneEntry *entry)
	{
		zSceneEntry::SceneEntryType myType = entry->getType();
		if (entry == me ) return false;
		if (myType == zSceneEntry::SceneEntry_NPC)
		{
			if (((SceneEntryPk *)entry)->isBugbear())
			{
				target = (SceneEntryPk *)entry;
				if (zMisc::selectByPercent(40)) return false;
			}
			else
			{
				return true;
			}
		}
		else if (myType == zSceneEntry::SceneEntry_Player)
		{
			target = (SceneEntryPk *)entry;
			if (zMisc::selectByPercent(40)) return false;
		}
		return true;
	}
	SDWORD action(SDWORD dam)
	{
		if (target)
		{
			return target->directDamage((SceneEntryPk *)me, dam);
		}
		return 0;
	}
};

#ifdef _POOL_ALLOC_
typedef std::vector<SceneEntryPk *, __gnu_cxx::__pool_alloc<SceneEntryPk *> > SceneEntryPk_vec;
#else
typedef std::vector<SceneEntryPk *> SceneEntryPk_vec;
#endif

#endif
