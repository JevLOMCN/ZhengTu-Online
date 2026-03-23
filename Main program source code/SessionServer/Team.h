/**
 * \file
 * \version  $Id: Team.h  $
 * \author  
 * \date 
 * \brief Session保存的队伍信息
 *
 * 
 */

#ifndef _TEAM_H_
#define _TEAM_H_

#include <vector>
#include <set>
#include <map>
#include "zMutex.h"
#include "zType.h"
//#include "Session.h"
using namespace std;
/**
 * \brief Session的队伍信息
 *
 */
struct Team
{
	Team():leaderid(0){};
	typedef set<DWORD> MemberSet;
	typedef MemberSet::iterator MemberSet_iter;
	typedef MemberSet::const_iterator MemberSet_const_iter;
	private:
	DWORD leaderid;
	MemberSet member;

	public:
	bool addMember(const DWORD userid);
	bool delMember(const DWORD userid);
	bool setLeader(const DWORD leader);
	bool delTeam();
};

/**
 * \brief Session队伍管理器
 *
 */
class GlobalTeamIndex
{
	private:
		GlobalTeamIndex(){};
		~GlobalTeamIndex(){};
		static GlobalTeamIndex *instance;

		typedef map<DWORD , Team> TeamMap;
		typedef TeamMap::value_type TeamMap_value_type;
		typedef TeamMap::iterator TeamMap_iterator;
		typedef TeamMap::const_iterator TeamMap_const_iterator;
		TeamMap team;
		zMutex mlock;

	public:
		static GlobalTeamIndex *getInstance();
		static void delInstance();
		bool addMember(const DWORD leaderid , const DWORD userid);
		bool delMember(const DWORD leaderid , const DWORD userid);
};
#endif
