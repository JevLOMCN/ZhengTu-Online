/**
 * \file
 * \version  $Id: SkillStatusManager.h $
 * \author  
 * \date 
 * \brief 定义技能状态管理器头文件
 *
 */

#ifndef __SKILLSTATUSMANAGER__
#define __SKILLSTATUSMANAGER__

#include "zType.h"
#include "zDatabase.h"
#include "zSceneEntry.h"

struct SceneEntryPk;
struct PkValue;
struct SkillState;

/// 技能善恶类型枚举
enum {
	SKILL_GOOD=0,
	SKILL_BAD=1
};

/// 技能状态类型枚举
enum {
	SKILL_TYPE_INITIAVITE=1,	// 攻击技能
	SKILL_TYPE_RECOVERY=2,		// 临时被动技能
	SKILL_TYPE_PASSIVENESS=3	// 永久被动技能
};

/// 技能状态处理返回值枚举
enum {
	SKILL_ACTIVE	=	1,	//	加到活动MAP中
	SKILL_RECOVERY,			//  加到临时被动MAP中
	SKILL_PASSIVENESS,		//	加到永久被动MAP中
	SKILL_RETURN,			//	不加到任何MAP中
	SKILL_BREAK,			//	不继续投放操作
};

/// 技能状态执行步骤枚举
enum {
	ACTION_STEP_DOPASS=	0,	// 一个技能状态的开始步骤
	ACTION_STEP_START=	1,	// 一个技能状态的开始步骤
	ACTION_STEP_TIMER,		// 一个技能状态的定时步骤
	ACTION_STEP_STOP,		// 一个技能状态的停止步骤
	ACTION_STEP_CLEAR,		// 一个技能状态被外部清除
	ACTION_STEP_RELOAD		// 一个技能状态被重新载入
};

/// 技能状态最大数目定义
#define SKILLSTATENUMBER 400   /// 技能状态最大编号

/**
 * \brief  技能状态元素载体
 * \author fqnewman
 */
struct SkillStatusCarrier
{
		/// 技能操作
		const SkillStatus *status;
		/// 技能字典
		const zSkillB *skillbase;
		/// 收到的攻击消息
		Cmd::stAttackMagicUserCmd revCmd;
		/// 攻击者的指针
		SceneEntryPk *attacker;
		/**
		 * \brief  构造函数，初始化所有属性
		 * \author fqnewman
		 */
		SkillStatusCarrier()
		{
			status = NULL;
			skillbase = NULL;
			attacker = NULL;
		}
};

/**
 * \brief  技能状态元素
 * \author fqnewman
 */
struct SkillStatusElement
{
	/// 状态的 id
	WORD  id;

	/// 状态发生几率
	WORD percent;

	/// 突破技能属性
	SDWORD value;

	/// 状态的持续时间
	QWORD qwTime;

	/// 状态执行的步骤标志
	WORD state;


	///攻击者的临时ID;
	DWORD dwTempID;

	///攻击者的ID
	DWORD dwAttackerID;

	///技能ID
	DWORD dwSkillID;

	///执行时长
	DWORD dwTime;

	///执行步骤
	BYTE  byStep;
	
	///善恶类型
	BYTE  byGoodnessType;
	
	///技能的互斥大类
	BYTE  byMutexType;

	///是否刷新人物属性1为刷新0为否
	BYTE  refresh;

	///攻击者的类型
	zSceneEntry::SceneEntryType attacktype;

	/**
	 * \brief  技能状态元素构造函数初始化所有属性
	 * \author fqnewman
	 */
	SkillStatusElement()
	{
		id			= 0;					//状态的id;
		percent		= 0;
		value		= 0;
		qwTime		= 0;
		state		= 0;
		dwTempID	= 0;
		dwSkillID	= 0;
		dwTime		= 0;
		byStep		= 0;
		refresh    = 0;
	}
};

/**
 * \brief  技能状态管理器
 * \author fqnewman
 */
class SkillStatusManager
{
private:
	/// 主动技能状态列表
	std::map<DWORD, SkillStatusElement> _activeElement;

	/// 临时被动技能状态列表
	std::map<DWORD, SkillStatusElement> _recoveryElement;

	/// 永久被动技能状态列表
	std::map<DWORD, SkillStatusElement> _passivenessElement;

	/// 类型定义
	typedef std::map<DWORD, SkillStatusElement>::value_type VALUE_TYPE;

	/**
	 * \brief  技能状态函数列表
	 * \author fqnewman
	 */
	struct{
		BYTE (* func)(SceneEntryPk *, SkillStatusElement &);
	}funlist[SKILLSTATENUMBER+1];

	/// 技能状态管理器属主
	SceneEntryPk *entry;
	bool bclearActiveSkillStatus;

public:
		SkillStatusManager();
		~SkillStatusManager();
		void initMe(SceneEntryPk *pEntry);
		void loadSkillStatus(char *buf, DWORD length);
		void getSelectStates(Cmd::stSelectReturnStatesPropertyUserCmd *buf , unsigned long maxSize);
		void sendSelectStates(SceneEntryPk *pThis , DWORD state , WORD value , WORD time);
		void saveSkillStatus(char *buf, DWORD &size);
		bool putOperationToMe(const SkillStatusCarrier &carrier, const bool good = false, SWORD rangDamageBonus=0);
		void putPassivenessOperationToMe(const DWORD skillid, const SkillStatus *pSkillStatus);
		void processPassiveness();
		inline BYTE runStatusElement(SkillStatusElement &element);
		void timer();
		void clearActiveSkillStatus();
		void clearActiveSkillStatusOnlyUseToStatus48();
		void clearBadActiveSkillStatus();
		void addBadSkillStatusPersistTime(const DWORD &value);
		void addBadSkillStatusPersistTimePercent(const DWORD &value);
		void clearMapElement(const BYTE &byMutexType, std::map<DWORD, SkillStatusElement> &myMap, DWORD dwID,bool notify=true);
		WORD getSaveStatusSize();
		void clearRecoveryElement(DWORD value);
		void clearActiveElement(DWORD value);
		void processDeath();
		void clearSkill(DWORD dwSkillID);

//[test]-----------------------------------------------------------------
		void showValue();
		void showValueToLog();
		void showActive();
		void showRecovery();
		void showPassiveness();
		void SetStatuToRecovery(DWORD id, DWORD time, DWORD percent, DWORD value=0, DWORD state=0);
//-----------------------------------------------------------------------
};

#endif

