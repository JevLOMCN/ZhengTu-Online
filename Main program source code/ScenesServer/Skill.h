#ifndef _SKILL_H_
#define _SKILL_H_

#include "zDatabaseManager.h"
#include "Command.h"
#include "zSceneEntry.h"
#include "zUniqueID.h"
#include "SkillBase.h"
#include "zTime.h"
#include "zType.h"

/**
 * \brief 技能压缩结构
 */
struct ZlibSkill
{
	DWORD count;
	unsigned char data[0];
	ZlibSkill()
	{
		count = 0;
	}
};

/**
 * \brief 技能存档单元
 */
struct SaveSkill
{
	DWORD type;
	DWORD level;
};

/**
 * \brief 技能分类
 *
 */
enum SkillType
{
	SKILL_TYPE_DAMAGE = 1,	///直接伤害
	SKILL_TYPE_RECOVER,	///恢复系
	SKILL_TYPE_BUFF,	///增益效果
	SKILL_TYPE_DEBUFF,	///负面效果
	SKILL_TYPE_SUMMON,	///召唤
	SKILL_TYPE_RELIVE	///复活
};

class SceneUser;
class SceneEntryPk;
class SceneNpc;

/**
 * \brief 技能类，定义技能的基本特性及攻击执行方法
 */
struct zSkill : zEntry
{
	static const DWORD maxUniqueID = 100000;
	public:
		t_Skill data;
		const zSkillB *base;
		const zSkillB *actionbase;

		static zSkill *create(SceneEntryPk *pEntry , DWORD id , DWORD level);
		static zSkill *createTempSkill(SceneEntryPk *pEntry , DWORD id , DWORD level);
		static zSkill *load(SceneEntryPk *pEntry , const SaveSkill *s);
		bool canUpgrade(SceneEntryPk *pEntry);
		bool setupSkillBase(SceneEntryPk *pEntry);
		bool getSaveData(SaveSkill *save);
		bool canUse();
		bool checkSkillBook(bool nextbase=false);
		bool checkSkillStudy(bool nextbase=false);
		void resetUseTime();
		void clearUseTime();
		void refresh(bool ignoredirty=false);

		~zSkill();
		unsigned long long lastUseTime;
	private:
		bool dirty;
		bool istemp;
		bool needSave;
		bool inserted;
		SceneEntryPk *_entry;
		const Cmd::stAttackMagicUserCmd *curRevCmd;
		unsigned int curRevCmdLen;
		zSkill();

		//static DWORD uniqueID;

// 新增加的技能方法
	public:
		bool action(const Cmd::stAttackMagicUserCmd *rev, const unsigned int cmdLen);
		inline void doOperation(const SkillStatus *pSkillStatus);
		inline void doPassivenessSkill();
		inline bool showMagicToAll();
		inline bool findAttackTarget(const SkillStatus *pSkillStatus, zPos &pd, DWORD &count, SWORD rangDamageBonus=0);
		inline const zSkillB *getNewBase();
};

#endif
