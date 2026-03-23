#include "SkillManager.h"
#include "Zebra.h"
#include "zMisc.h"


/**
 * \brief  获取临时id
 * \param  tempid 临时id
 * \return 获取成功
 */
bool SkillManager::getUniqeID(DWORD &tempid)
{
	return true;
}

/**
 * \brief  将使用完毕的临时id释放
 * \param  tempid 使用完毕的临时id
  */
void SkillManager::putUniqeID(const DWORD &tempid)
{
}

/**
 * \brief  构造函数
 */
UserSkillM::UserSkillM()
{
}

/**
 * \brief  析构处理，清除所有的技能对象
   */
UserSkillM::~UserSkillM()
{
	for(zEntryTempID::hashmap::iterator it=zEntryTempID::ets.begin();it!=zEntryTempID::ets.end();it++)
	{
		zSkill *skill = (zSkill *)it->second;
		SAFE_DELETE(skill);
	}
}

/**
 * \brief  根据临时id获取技能对象
 * \param  id 临时id
 * \return 技能对象
 */
zSkill *UserSkillM::getSkillByTempID(DWORD id)
{
	return (zSkill *)getEntryByTempID(id);
}

/**
 * \brief  根据临时id 删除技能对象
 * \param  id 临时id
  */
void UserSkillM::removeSkillByTempID(DWORD id)
{
	zEntry *e=getEntryByTempID(id);
	if(e)
		removeEntry(e);
}

/**
 * \brief  从管理器中删除指定的技能对象
 * \param  s 被删除的技能对象
 */
void UserSkillM::removeSkill(zSkill *s)
{
	removeEntry(s);
}

/**
 * \brief  增加一个技能对象到管理器中
 * \param  s 技能对象
 * \return true 增加成功 false 增加失败
 */
bool UserSkillM::addSkill(zSkill *s)
{
	bool bret = false;
	if(s)
	{
		zSkill *ret = (zSkill *)getEntryByTempID(s->id);
		if(ret)
		{
			Zebra::logger->debug("技能ID重复(%ld)" , s->id);
		}

		bret = addEntry((zSkill *)s);
		if(!bret)
		{
			Zebra::logger->fatal("添加技能表失败");
		}
	}

	return bret;
}

/**
 * \brief  根据技能id查找对应的技能对象
  * \return 技能对象
 */
zSkill *UserSkillM::findSkill(DWORD skillid)
{
	//zSkill *s;
	//for(zEntryTempID::hashmap::iterator it=zEntryTempID::ets.begin();it!=zEntryTempID::ets.end();it++)
	//{
	//	s = (zSkill *)it->second;
	//	if(s->data.skillid == skillid)
	//	{
	//		return (zSkill *)it->second;
	//	}
	//}
	//return NULL;
	return (zSkill *)getEntryByTempID(skillid);
}

/**
 * \brief  根据技能id查找对应的技能对象
 * \param myKind 技能系别
 * \param mySubkind 技能树别
 * \return 本树投入的技能点数
 */
DWORD UserSkillM::getPointInTree(DWORD myKind, DWORD mySubkind)
{
	zSkill *s;
	DWORD num=0;
	for(zEntryTempID::hashmap::iterator it=zEntryTempID::ets.begin();it!=zEntryTempID::ets.end();it++)
	{
		s = (zSkill *)it->second;
		if ((s->base->subkind == 1)&&(s->base->kind == myKind)) num+=s->base->level; //增加基础系点数
		if ((s->base->subkind !=1)&&(s->base->subkind == mySubkind)&&(s->base->kind == myKind)) num+=s->base->level;
	}
#ifdef _DEBUGLOG
	Zebra::logger->info("[技能点数]%u", num);
#endif 
	return num;
}

/**
 * \brief  回调遍历该用户所有的技能对象
 * \param  exec 回调函数
 */
void UserSkillM::execEvery(UserSkillExec &exec)
{
	for(zEntryTempID::hashmap::iterator it=zEntryTempID::ets.begin();it!=zEntryTempID::ets.end();it++)
	{
		if(!exec.exec((zSkill *)it->second))
			return;
	}
}

/**
 * \brief  重设该用户所有的技能冷却时间
 */
void UserSkillM::resetAllUseTime()
{
	zSkill *s;
	for(zEntryTempID::hashmap::iterator it=zEntryTempID::ets.begin();it!=zEntryTempID::ets.end();it++)
	{
		s = (zSkill *)it->second;
		s->resetUseTime();
	}
}

/**
 * \brief  清除该用户所有的技能冷却时间
 */
void UserSkillM::clearAllUseTime(DWORD skillID)
{
  zSkill *s;
  for(zEntryTempID::hashmap::iterator it=zEntryTempID::ets.begin();it!=zEntryTempID::ets.end();it++)
  {

	  s = (zSkill *)it->second;
	  if(s && s->base->skillid != skillID)
	  {
		  s->clearUseTime();
		  s->refresh(true);
	  }
  }
  }
/**
 * \brief  刷新技能(武器提升技能等级。。。。。)
 */
void UserSkillM::refresh()
{
	zSkill *s;
	for(zEntryTempID::hashmap::iterator it=zEntryTempID::ets.begin();it!=zEntryTempID::ets.end();it++)
	{
		s = (zSkill *)it->second;
		s->refresh();
	}
}

void UserSkillM::clear()
{
	SkillManager::clear();
}

int UserSkillM::size() const
{
	return SkillManager::size();
}

/**
 * \brief  根据技能id查找对应的技能对象并删除
  * \return 技能对象
 */
void UserSkillM::clearskill(DWORD skillid)
{
	SkillManager::removeEntry(getEntryByTempID(skillid));
}
