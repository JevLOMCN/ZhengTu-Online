/**
 * \file
 * \version	$Id: QuestCondition.cpp  $
 * \author	
 * \date	
 * \brief	任务系统
 * 
 */

#include "QuestCondition.h"
#include "SceneUserManager.h"
#include "CountryAlly.h"
#include "ScenesServer.h"

/**     
 * \brief  参数检查
 *
 * 提供了一个默认的参数需求检测,继承类需要根据自己的要求重载此函数
 *      
 * \param user: 触发条件判定的用户
 * \param vars: 用户所带的该任务相关变量
 * \return true表示参数合法,false表示参数非法
 */   
bool Condition::check_args(const SceneUser* user, const Vars* vars) const
{
	if (user && vars) return true;
	
	return false;
}

/**     
 * \brief  脚本条件判定
 *
 * template method模式,进行参数检查，并执行脚本定义的条件判定
 *      
 * \param user: 触发条件的用户
 * \param vars: 用户所带的该任务相关变量
 * \return true表示满足条件,false表示不满足条件
 */    
bool Condition::is_valid(const SceneUser* user, const Vars* vars) const
{
	if (!check_args(user, vars) ) return false;
	
	return check_valid(user, vars);
}

/**     
 * \brief  脚本条件判定
 *
 * template method模式,进行参数检查，并对每一个用户执行脚本定义的条件判定
 *      
 * \param user: 触发条件的用户
 * \param vars: 用户所带的该任务相关变量
 * \return true表示所有用户满足条件,否则返回false
 */    
bool TeamCondition::is_valid(const SceneUser* user, const Vars* vars) const
{
	if (!check_args(user, vars) ) return false;
	
	if (_team) 
	{
		DWORD leader_id = user->team.getLeader();
		SceneUser* leader = SceneUserManager::getMe().getUserByTempID(leader_id);
		bool result = false;
		if (leader) {
			Team& team = const_cast<Team&>(leader->team.getTeam());

			team.rwlock.rdlock();
			std::vector<TeamMember>::iterator it = team.member.begin();
			for(; it!=team.member.end(); ++it) {
				SceneUser* member = SceneUserManager::getMe().getUserByTempID(it->tempid);
				if (member) {
					Vars* v = member->quest_list.vars(vars->quest_id());
					result = check_valid(member, v);
					if (!result) break;
				}
			}
			team.rwlock.unlock();
		}else {
			result = check_valid(user, vars);		
		}
		
		return result;
	}
	
	return check_valid(user, vars);
}

/**     
 * \brief  组队条件判定
 *
 * 重载了check_valid函数,判定用户的队伍是否满足脚本要求
 *      
 * \param user: 触发条件的用户
 * \param vars: 用户所带的该任务相关变量
 * \return true表示满足条件,false表示不满足条件
 */    
bool TeamedCondition::check_valid(const SceneUser* user, const Vars* vars) const
{
	if (!check_args(user, vars) ) return false;
	
	DWORD leader_id = user->team.getLeader();
	SceneUser* leader = SceneUserManager::getMe().getUserByTempID(leader_id);
	
	if (leader) {
		if (_number && leader->team.getSize() != _number) return false;
		
		int male = 0, female = 0;
		Team& team = const_cast<Team&>(leader->team.getTeam());
		team.rwlock.rdlock();
		std::vector<TeamMember>::iterator it = team.member.begin();
		for(; it!=team.member.end(); ++it) {
			SceneUser* member = SceneUserManager::getMe().getUserByTempID(it->tempid);
			if (member) {
				if (member->charbase.type == PROFESSION_1) ++male;
				if (member->charbase.type == PROFESSION_2) ++female;
			}
		}
		team.rwlock.unlock();
		
		if (_male && _male != male) return false;
		if (_female && _female != female) return false;
		
		return true;
	}
		
	return false;
}

#include "Scene.h"

bool MapCountryCondition::check_valid(const SceneUser* user, const Vars* vars) const
{
	Vars* vs = const_cast<Vars *>(vars);
	if (_id) vs = user->quest_list.vars(_id);

	if (!vs) {
		return false;
	}

	int country;
	if (!vs->get_value(_name, country)) {
		return false;
	}
	
	return (int)user->scene->getCountryID() == country;
}

bool HonorCondition::check_valid(const SceneUser* user, const Vars* vars) const
{
	return user->charbase.honor >= (DWORD)_value;
}

bool MaxHonorCondition::check_valid(const SceneUser* user, const Vars* vars) const
{
	return user->charbase.maxhonor >= (DWORD)_value;
}

bool SelfCountryCondition::check_valid(const SceneUser* user, const Vars* vars) const
{
	bool flag = ( user->charbase.country == user->scene->getCountryID() 
		|| (CountryAllyM::getMe().getFriendLevel(user->charbase.country, user->scene->getCountryID())>0));
	if (!_value) flag = !flag;

	return flag;
}

bool CountryPowerCondition::check_valid(const SceneUser* user, const Vars* vars) const
{
#ifdef _DEBUGLOG
	Zebra::logger->debug("我[%s countryid=%d]所在的国家是%s", user->name, user->charbase.country, (ScenesService::getInstance().countryPower[user->charbase.country] == 0)?"弱国":"强国");
#endif
	return (ScenesService::getInstance().countryPower[user->charbase.country] == 0);
}

bool WeekCondition::check_valid(const SceneUser* user, const Vars* vars) const
{
	struct tm tm_1;
	time_t timValue = time(NULL);
	zRTime::getLocalTime(tm_1, timValue);
	unsigned char tempvalue = (unsigned char)_value;
	return (Cmd::isset_state(&tempvalue, tm_1.tm_wday));
}

bool CaptionCondition::check_valid(const SceneUser* user, const Vars* vars) const
{
	return strstr(user->caption , "城主") != NULL;
}


