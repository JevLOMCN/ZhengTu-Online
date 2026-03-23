#ifndef _TEAMMANAGER_H_
#define _TEAMMANAGER_H_

#include "Team.h"
#include "SessionCommand.h"
#include "zRWLock.h"

struct SceneUser;

/// 友好度成员
struct stDegreeMember{
	DWORD dwUserID;
	WORD  wdDegree;
	WORD  wdTime;
};

/// 友好度临时结构
struct stTempDegreeMember{
	DWORD dwUserID;
	WORD  wdDegree;
	WORD  wdTime;
	BYTE  byType;
};

/// 友好度范围 
const DWORD FRIENDDEGREE_RANGE_BTM = 3*60;

/// 友好度范围
const DWORD FRIENDDEGREE_RANGE_MED = 15*60;
// 友好度范围
const DWORD FRIENDDEGREE_RANGE_TOP = 55*60;

/// 夫妻防御增益百分比1
const DWORD FRIENDDEGREE_CONSORT_DEFPLUS_RATE_TOP	= 8;	
/// 夫妻防御增益百分比2
const DWORD FRIENDDEGREE_CONSORT_DEFPLUS_RATE_MED	= 5;	
/// 夫妻防御增益百分比3
const DWORD FRIENDDEGREE_CONSORT_DEFPLUS_RATE_BTM	= 3;	

/// 徒弟防御增益百分比1
const DWORD FRIENDDEGREE_PRENTICE_DEFPLUS_RATE_TOP	= 2;	
/// 徒弟防御增益百分比2
const DWORD FRIENDDEGREE_PRENTICE_DEFPLUS_RATE_MED	= 2;	
/// 徒弟防御增益百分比3
const DWORD FRIENDDEGREE_PRENTICE_DEFPLUS_RATE_BTM	= 2;	
/// 徒弟防御增益百分比100
const DWORD FRIENDDEGREE_PRENTICE_DEFPLUS_RATE_FULL    = 100;

/// 师傅攻击增益百分比1
const DWORD FRIENDDEGREE_TEACHER_ATTPLUS_RATE_TOP	= 4;	
/// 师傅攻击增益百分比2
const DWORD FRIENDDEGREE_TEACHER_ATTPLUS_RATE_MED	= 4;	
/// 师傅攻击增益百分比3
const DWORD FRIENDDEGREE_TEACHER_ATTPLUS_RATE_BTM	= 2;	

/// 好友攻击增益百分比1
const DWORD FRIENDDEGREE_FRIEND_ATTPLUS_RATE_TOP	= 8;	
/// 好友攻击增益百分比2
const DWORD FRIENDDEGREE_FRIEND_ATTPLUS_RATE_MED	= 5;	
/// 好友攻击增益百分比3
const DWORD FRIENDDEGREE_FRIEND_ATTPLUS_RATE_BTM	= 3;	

/// 经验增益百分比
const DWORD FRIENDDEGREE_EXPPLUS_RATE	= 5;	
/// 金钱增益百分比
const DWORD FRIENDDEGREE_MONEYPLUS_RATE = 20;  

/// 朋友关系增益概率1
const DWORD FRIENDDEGREE_FRIEND_RATE_TOP	= 100;	
/// 朋友关系增益概率2
const DWORD FRIENDDEGREE_FRIEND_RATE_MED	= 100;	
/// 朋友关系增益概率3
const DWORD FRIENDDEGREE_FRIEND_RATE_BTM	= 100;	

/// 夫妻关系增益概率1
const DWORD FRIENDDEGREE_CONSORT_RATE_TOP	= 100;	
/// 夫妻关系增益概率2
const DWORD FRIENDDEGREE_CONSORT_RATE_MED	= 100;	
/// 夫妻关系增益概率3
const DWORD FRIENDDEGREE_CONSORT_RATE_BTM	= 100;	

/// 师徒关系增益概率1
const DWORD FRIENDDEGREE_TEACHER_RATE_TOP	= 40;	
/// 师徒关系增益概率2
const DWORD FRIENDDEGREE_TEACHER_RATE_MED	= 20;	
/// 师徒关系增益概率3
const DWORD FRIENDDEGREE_TEACHER_RATE_BTM	= 20;	

/// 友好度增益有效距离
const DWORD FRIENDDEGREE_VALIDATION_DISTANCE_WIDE = SCREEN_WIDTH; 
/// 友好度增益有效距离
const DWORD FRIENDDEGREE_VALIDATION_DISTANCE_HEIGHT = SCREEN_HEIGHT; 


/**
 * \brief  队伍管理器
 */
class TeamManager
{
	private:
		//物品分配模式
		BYTE obj_mode;
		//经验分配模式
		BYTE exp_mode;
		// /队伍对象
		Team team;

		/// 排序方法
		struct ltword
		{
			bool operator()(const WORD s1, const WORD s2) const
			{
				return s1>s2;
			}
		};

		/// 朋友列表
		std::map<WORD, struct stDegreeMember,ltword> friendList;

		/// 夫妻列表
		std::map<WORD, struct stDegreeMember,ltword> consortList;

		/// 师傅列表
		std::map<WORD, struct stDegreeMember,ltword> teacherList;

		/// 徒弟列表
		std::map<WORD, struct stDegreeMember,ltword> prenticeList;

		/// 社会关系读写锁
		zRWLock relationlock;

		/// 类型定义
		typedef std::map<WORD, struct stDegreeMember, ltword>::value_type insValueType;

		/// 管理器拥有者
		SceneUser * me;

		bool giveupstatus; //阵法放弃标志
		zRTime giveuptime; //阵法放弃时间 

	public:

		void setNextObjOwnerID(DWORD id)
		{
			team.setNextObjOwnerID(id);
		}
		DWORD getNextObjOwnerID()
		{
			return team.getNextObjOwnerID();
		}
		bool isSpecialObj()
		{
			return obj_mode == Cmd::TEAM_OBJ_MODE_SPECIAL;
		}
		bool isNormalObj()
		{
			return obj_mode == Cmd::TEAM_OBJ_MODE_NORMAL;
		}
		BYTE getObjMode()
		{
			return obj_mode;
		}
		void setObjMode(BYTE mode)
		{
			obj_mode = mode;
		}
		bool isSpecialExp()
		{
			return exp_mode == Cmd::TEAM_EXP_MODE_SPECIAL;
		}
		bool isNormalExp()
		{
			return exp_mode == Cmd::TEAM_EXP_MODE_NORMAL;
		}
		BYTE  getExpMode()
		{
			return exp_mode;
		}
		void setExpMode(BYTE mode)
		{
			exp_mode = mode;
		}
		/**
		 * \brief  构造初始化
		 * \param  pUser 队伍管理器拥有者
		 */
		TeamManager(SceneUser *pUser)
		{
			me = pUser;
			giveupstatus = false;
			obj_mode=0;
			exp_mode=0;
		}

		/**
		 * \brief  判断队伍是否已经达到最大人数
		 * \return true 队伍已经达到最大人数， false 还没有
		 */
		bool IsFull()
		{
			return team.getSize() == Team::MaxMember;
		}

		/**
		 * \brief  获取队伍成员人数
		 * \return 人数
		 */
		int getSize();
		
		int getExpSize(zPosI pos , DWORD sceneid);

		/**
		 * \brief  是否允许组队
		 * \return true 允许组队 false 不允许组队
		 */
		bool IsOpen()
		{
			return team.isOpen;
		}

		/**
		 * \brief  设置允许组队标志
		 * \param  is 允许组队标志
		 */
		void setOpen(bool is)
		{
			team.isOpen = is;
		}

		/**
		 * \brief  是否组队
		 * \return true 组队， false 没有组队
		 */
		bool IsTeamed()
		{
			return team.leader != 0;
		}

		/**
		 * \brief  获取队长的临时id
		 * \return 队长临时id
		 */
		DWORD getLeader() const
		{
			return team.leader;
		}

		/**
		 * \brief  记录队长的临时id
		 * \param  tempid 回调方法
		 */
		bool setLeader(DWORD tempid)
		{
			team.leader = tempid;
			return true;
		}

		/**
		 * \brief  角色升级数据变更处理
		 * \param  pUser 升级的角色
		 */
		void userupgrade(SceneUser *pUser)
		{
			team.userupgrade(pUser);
		}

		/**
		 * \brief  获得队伍平均分配经验
		 * \return 平均经验
		 */
		DWORD getAverageExp()
		{
			return team.averageExp;
		}

		/**
		 * \brief  获得队伍对象
		 * \return 队伍对象
		 */
		const Team& getTeam() const
		{
			return team;
		}
		bool delMemberToSession(DWORD leaderid , DWORD userid);
		bool loadTeam(SceneUser *leader , TempArchiveMember *data);
		bool addNewLeader(SceneUser *pUser);
		bool IsOurTeam(SceneUser *pUser);
		bool IsOurTeam(DWORD dwID);
		bool addNewMember(SceneUser *pUser , Cmd::stAnswerTeamUserCmd *rev);
		bool addNewMember(SceneUser *leader , SceneUser *pUser);
		void removeTeam(SceneUser *pUser , DWORD tempid);
		void deleteTeam(SceneUser *leader);
		void removeMember(SceneUser *pUser , Cmd::stRemoveTeamMemberUserCmd *rev);
		void kickoutMember(SceneUser *pUser , Cmd::stRemoveTeamMemberUserCmd *rev);
		void removeMember(SceneUser *pUser , SceneUser *mem);
		void checkOffline();
		void sendtoTeamCharData(SceneUser * leader , SceneUser *user);
		void sendtoTeamCharData(SceneUser *user);
		void sendCmdToTeam(SceneUser *user , void *cmd , DWORD cmdLen);
		void checkOffline(SceneUser *pUser);
		bool changeLeader(SceneUser *pUser);
		void execEveryOne(TeamMemExec &callback);
		void execEveryOneExceptMe(TeamMemExec &callback , DWORD tempid);
		void decreaseAverageExp(SceneUser *leader , SceneUser *pUser);
		void decreaseAverageExp(DWORD leaderid , SceneUser *pUser);
		void calAverageExp(zPosI pos,DWORD sceneid);

		// 设置该用户的友好度相关社会关系
		void setFriendDegree(Cmd::Session::t_ReturnFriendDegree_SceneSession * = NULL);
		DWORD getExpPlus(WORD wdExp);
		DWORD getAttPlus();
		DWORD getDefPlus();
		void putMoneyPlus(WORD wdMoney);
		void countFriendDegree();
		void requestFriendDegree();
		bool canPutSkill();
	private:
		void sendFriendDegreeToSession();

		SceneUser *findNewLeader(SceneUser *pUser);
		bool addMemberByID(SceneUser *pUser , DWORD id);
		bool addMemberByTempID(SceneUser *pUser , DWORD tempid);
		void removeMemberByID(DWORD id);
		void removeMemberByTempID(DWORD tempid);
		bool addMemberToSession(DWORD leaderid , DWORD userid);


};



#endif



