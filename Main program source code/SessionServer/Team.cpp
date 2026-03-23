#include "Team.h"
#include "Session.h"
#include "SessionManager.h"


/**
 * \brief 添加成员
 * \param userid 角色id
 * \return true 成功 false 失败
 */
bool Team::addMember(const DWORD userid)
{
	UserSession *pUser=UserSessionManager::getInstance()->getUserByID(userid);
	if(!pUser)
	{
		return false;
	}
	bool bret = false;
	bret = member.insert(userid).second;
	if(bret)
	{
		pUser->teamid = leaderid;
		//Zebra::logger->debug("添加队员(%u , %u)" , pUser->teamid , pUser->id); 
	}
	return bret;
}

/**
 * \brief 删除成员
 * \param userid 角色id
 * \return true 成功
 */
bool Team::delMember(const DWORD userid)
{
	member.erase(userid);
	UserSession *pUser=UserSessionManager::getInstance()->getUserByID(userid);
	if(pUser)
	{
		//Zebra::logger->debug("删除队员(%u , %u)" , pUser->teamid , pUser->id); 
		pUser->teamid = 0;
	}
	return true;
}

/**
 * \brief 设置队长
 * \param leader 队长id
 * \return true 成功 false 失败
 */
bool Team::setLeader(const DWORD leader)
{
	leaderid = leader;
	return addMember(leaderid);
}

/**
 * \brief 删除队伍
 * \return true 成功 false 失败
 */
bool Team::delTeam()
{
	MemberSet_iter iter;
	for(iter = member.begin() ; iter != member.end() ; iter ++)
	{
		UserSession *pUser=UserSessionManager::getInstance()->getUserByID(*iter);
		if(pUser)
		{
			//Zebra::logger->debug("队伍解散删除队员(%u , %u)" , pUser->teamid , pUser->id); 
			pUser->teamid = 0;
		}
	}
	leaderid = 0;
	return true;
}

/// 全局队伍索引
GlobalTeamIndex *GlobalTeamIndex::instance = NULL;

/**
 * \brief 获取全局队伍索引唯一实例
 * \return 全局队伍索引对象
 */
GlobalTeamIndex *GlobalTeamIndex::getInstance()
{
	if(!instance)
	{
		instance = new GlobalTeamIndex;
	}
	return instance;
}

/**
 * \brief 删除实例
 */
void GlobalTeamIndex::delInstance()
{
	SAFE_DELETE(instance);
}

/**
 * \brief 增加成员
 * \param leaderid 队长id
 * \param userid 队员ID
 * \return true 成功 false 失败
 */
bool GlobalTeamIndex::addMember(const DWORD leaderid , const DWORD userid)
{
	bool bret = false;
	mlock.lock();
	if(leaderid == userid)
	{
		Team t;
		t.setLeader(leaderid);
		bret = team.insert(TeamMap_value_type(leaderid , t)).second;
	}
	else
	{
		TeamMap_iterator iter = team.find(leaderid);
		if(iter != team.end())
		{
			bret = iter->second.addMember(userid);
		}
		else
		{
			bret = false;
		}
	}
	mlock.unlock();
	return bret;
}

/**
 * \brief 删除成员
 * \param leaderid 队长id
 * \param userid 队员ID
 * \return true 成功 false 失败
 */
bool GlobalTeamIndex::delMember(const DWORD leaderid , const DWORD userid)
{
	bool bret = false;
	mlock.lock();
	TeamMap_iterator iter = team.find(leaderid);
	if(iter != team.end())
	{
		if(leaderid == userid)
		{
			iter->second.delTeam();
			team.erase(leaderid);
			bret = true;
		}
		else
		{
			bret = iter->second.delMember(userid);
		}
	}
	else
	{
		bret = false;
	}
	mlock.unlock();
	return bret;
}
