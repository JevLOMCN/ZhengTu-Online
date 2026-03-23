#ifndef _SCENETEAM_H_
#define _SCENETEAM_H_


#include <vector>
#include "zType.h"
#include "zEntry.h"
#include "zRWLock.h"
#include "zSceneEntry.h"

//荣誉模式需要的等级限制
#define TEAM_HONOR_MEMBER_LEVEL 50
#define TEAM_HONOR_LEADER_LEVEL 31

/**
 * \brief 组队成员结构
 */
struct TeamMember 
{
	DWORD id;
	DWORD tempid;
	char name[MAX_NAMESIZE + 1];
	DWORD offtime;
	DWORD begintime;

	/**
	 * \brief 构造初始化结构
	 */
	TeamMember()
	{
		id = 0;
		tempid = 0;
		bzero(name , sizeof(name));
		offtime = 0;
	}
};

/**
 * \brief 队伍遍历方法
 */
struct TeamMemExec
{
	virtual ~TeamMemExec(){};
	virtual bool exec(TeamMember &member) = 0;
};

struct SceneUser;

struct Team
{
	/// 定义最大成员数
	static const  int MaxMember = 6;

	/**
	 * \brief 构造函数初始化相关属性
	 */
	Team()
	{
		isOpen = true;
		leader = 0;
		averageExp = 0;
		current_obj_owner=0; 
	}

	/// 允许组队标志
	bool isOpen;

	/// 这个是队长的tempid 如果要改变请通知服务端全体成员
	DWORD leader; 
	//轮流物品掉落时当前掉落的序号
	DWORD current_obj_owner;

	/// 平均经验参数
	DWORD averageExp;
	private:
	friend class TeamCondition;
	friend class TeamedCondition;
	friend class TeamAction;
	friend class QuestList;
	friend class SceneNpc;

	/// 队伍成员表
	std::vector<TeamMember> member;

	/// 离线成员临时登记表
	std::vector<TeamMember> offline_member;

	/// 队伍读写锁
	zRWLock rwlock;
	
	public:
	/**
	 * \brief 获取队伍人数
	 */
	int getSize()
	{
		return member.size();
	}

	/**
	 * \brief 清除所有的队员
	 */
	void Clear()
	{
		//rwlock.wrlock();
		member.clear();
		//rwlock.unlock();
	}
	DWORD getNextObjOwnerID();
	void setNextObjOwnerID(DWORD id)
	{
		current_obj_owner=id;
	}
	void userupgrade(SceneUser *pUser);
	void decreaseAverageExp(SceneUser *pUser);
	void increaseAverageExp(SceneUser *pUser);
	void calAverageExp(zPosI pos,DWORD sceneid);
	bool addMember(DWORD id , DWORD tempid , const char *pname);
	void removeMember(DWORD id , DWORD tempid);
	void removeMemberByID(DWORD id);
	DWORD removeMemberByName(const char *pname);
	void removeMemberByTempID(DWORD tempid);
	void execEvery(TeamMemExec &callback);
	void execEveryExceptMe(TeamMemExec &callback , DWORD tempid);
	void requestFriendDegree();
};



#endif
