#ifndef _SKILLMANAGER_H_
#define _SKILLMANAGER_H_

#include "zType.h"
#include "Skill.h"
#include "zEntryManager.h"


/**
 * \brief  技能管理器基类
 */
class SkillManager : public zEntryManager < zEntryID, zEntryTempID >
{
	protected:
		bool getUniqeID(DWORD &tempid);
		void putUniqeID(const DWORD &tempid);
};

/**
 * \brief  回调基类
 */
class UserSkillExec
{
	public:
		virtual bool exec(zSkill *s) = 0;
};


/**
 * \brief  技能管理器基类
 */
class UserSkillM : private SkillManager
{
	public:
		UserSkillM();
		~UserSkillM();
		zSkill *getSkillByTempID(DWORD id);
		void removeSkillByTempID(DWORD id);
		void removeSkill(zSkill *s);
		bool addSkill(zSkill *s);
		zSkill *findSkill(DWORD skillid);
		void execEvery(UserSkillExec &exec);
		void resetAllUseTime();
		void clearAllUseTime(DWORD skillID=0);
		void clear();
		DWORD getPointInTree(DWORD myKind, DWORD mySubkind);
		void refresh();
		int size() const;
		void clearskill(DWORD skillid);
};

#endif
