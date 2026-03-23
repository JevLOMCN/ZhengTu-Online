/**
 * \file
 * \version  $Id: CUnion.h  $
 * \author  
 * \date 
 * \brief 定义帮会类，帮会管理器
 *
 */

#ifndef _CLASS_UNION_H_
#define _CLASS_UNION_H_

#include <vector>
#include "zMutex.h"
#include "zMisc.h"
#include "zType.h"
#include "zEntry.h"
#include "zEntryManager.h"
#include "Union.h"
#include "Command.h"
#include "SessionCommand.h"
#include "zUser.h"
#include "zSceneManager.h"
#include "zSingleton.h"

using namespace UnionDef;

class UserSession;
class zRWLock;
class CUnionMember;
class CUnion;

class CUnionM:public zEntryManager<zEntryID,zEntryName>,
	      public Singleton<CUnionM>
{
	friend class SingletonFactory<CUnionM>;
	private:
		/// 成员索引
		std::map<std::string, CUnionMember*> memberIndex;

		/// 类型定义
		typedef std::map<std::string, CUnionMember*>::value_type memberIndexValueType;

		/// 读写锁
		zRWLock rwlock;

		CUnion *	createUnionAndAddMaster(const stUnionInfo&);
		bool 		initAddUnionMember(const DWORD &, const stUnionMemberInfo &, bool notify = false);
		bool		createUnionDBRecord(stUnionInfo &info);
		void		removeEntryByName(const char * name);
		CUnion*		createUnionByDBRecord(const stUnionInfo & info);
		inline std::map<std::string, CUnionMember *>::iterator  findMemberIndex(const char *pName);
		CUnionM();

	public:
		~CUnionM();
		template <class YourEntry>
		bool execEveryOne(execEntry<YourEntry> &exec)
		{
			rwlock.rdlock();
			bool ret=execEveryEntry<>(exec);
			rwlock.unlock();
			return ret;
		}

		template <class YourEntry>
		void removeOne_if(removeEntry_Pred<YourEntry> &pred)
		{
			rwlock.wrlock();
			removeEntry_if<>(pred);
			rwlock.unlock();
		}

		CUnion * getUnionByName( const char * pName);
		CUnion* getUnionByID(DWORD dwUnionID);

		bool init();
		static void destroyMe();

		void fireUnionMember(UserSession*, const char *);

		/**
		  * \brief 删除帮会成员
		  *
		  * 如果该角色不是帮会会长，则踢除他，如果是，则保留，不做处理
		  *
		  * \param dwUserID 角色ID
		  * \param find     是否为查找
		  *
		  * \return 如果该角色不在任何帮会中，则返回2
		  *         如果该角色是帮主，则返回0
		  *         如果该角色是帮众，并删除成功，则返回1
		  *
		  */
		int  fireUnionMember(DWORD dwUserID, bool find);

		bool addNewMemberToUnion(const DWORD dwUnionID, const stUnionMemberInfo&);
		void addNewSeptToUnion(const DWORD dwUnionID, const DWORD dwSeptID);
		
		void userOnline(UserSession *);
		void userOffline(const UserSession *);
		void createNewUnion(Cmd::Session::t_addUnion_SceneSession *data);
		bool addMemberIndex(const char *, CUnionMember *);
		bool removeMemberIndex(const char *);
		bool processUserMessage(UserSession *pUser,const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen);
		void processUnionSortMessage(UserSession* pUser, const Cmd::stReqUnionSort* ptCmd);
		void processAppointCityCatcherMessage(UserSession* pUser, const Cmd::stAppointCityCatcherCmd* ptCmd);
		void processCancelCityCatcherMessage(UserSession* pUser, const Cmd::stCancelCityCatcherCmd* ptCmd);

		bool processSceneMessage(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen);
		bool processSceneUnionMessage(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen);
		
		bool processGateMessage(const Cmd::t_NullCmd *cmd, const unsigned int cmdLen);
		void delUnion(const DWORD dwUnionID);
		void processMemberLeave(const DWORD dwUnionID, const DWORD dwCharID);
		void processMemberPower(UserSession* pUser, const Cmd::stUnionMemberPower* ptCmd);
		bool havePowerByName(const char* name, const int power);

		void sendUnionNotify(const DWORD unionID, const char* message, ...);

		void sendUnionChatMessages(const UserSession *pUser, const Cmd::stChannelChatUserCmd *, const unsigned int cmdLen);
		void sendUnionPrivateChatMessages(const UserSession *pUser, const Cmd::stChannelChatUserCmd *, const unsigned int cmdLen);
		void sendVoteListToUser(const UserSession *pUser);
		void sendListToUser(const UserSession *pUser);
		void sendDareListToUser(const UserSession* pUser);
		
		void userVote(const UserSession *pUser, const char *pName);
		void userAboutVote(const UserSession *pUser, const char *pName);
		void addVoteMemberToUnion(const UserSession *pUser, const char *pName);
		void setUnionNote(UserSession *pUser, Cmd::stNoteUnionCmd *pCmd);


		/**
		 * \brief 更改别名
		 *
		 * \param pUser 请求更名者(目前只能是帮主)
		 *
		 * \param pCmd 更名命令
		 *
		 * \return 
		 */
		void change_aliasname(UserSession *pUser, Cmd::stChangeUnionMemberAliasName* pCmd);
		void delAllUnion(DWORD id);


		/**
		 * \brief 时间回调函数
		 */
		void timer();
		char * getUnionNameByUserName(char *Name);
		DWORD getCountryIDByUserName(char *Name);
};

class CUnion:public zEntryManager<zEntryID, zEntryName>, public zEntry
{
	private:
		/// 帮会级别
		WORD                  level;

		/// 帮会经验
		DWORD                 exp;

		/// 毁灭状态不再执行写库等操作
		bool					destroy;

		/// 是否还处于投票期间，1为是0为否
		BYTE					byVote;

	
       /// 读写锁
		zRWLock rwlock;

	public:
		/// 帮会会长
		CUnionMember          *master;
		/// 创立时间
		DWORD					dwCreateTime;
		/// 介绍
		char		note[255];

		/// 帮会所属国家ID
		DWORD dwCountryID;

		/// 帮会威望
		DWORD dwMana;

		// 当前行动力
		DWORD dwActionPoint;

		// 帮会资金（银两）
		DWORD dwMoney;
		DWORD calltimes;

		DWORD getActionPoint();//得到行动力
		bool  changeActionPoint(int repute);//改变行动力
		
		DWORD getMoney();//得到帮会资金
		bool  changeMoney(int money);//改变帮会资金

		void sendUnionDare(UserSession* pUser, const char* fromName, DWORD dwWarID);

		template <class YourEntry>
		bool execEveryOne(execEntry<YourEntry> &exec)
		{
			rwlock.rdlock();
			bool ret=execEveryEntry<>(exec);
			rwlock.unlock();
			return ret;
		}

		template <class YourEntry>
		void removeOne_if(removeEntry_Pred<YourEntry> &pred)
		{
			rwlock.wrlock();
			removeEntry_if<>(pred);
			rwlock.unlock();
		}

		CUnion();
		~CUnion();
		void			removeEntryByName(const char * name);
		DWORD			getID() {return id;}
		void			init(const stUnionInfo & info);
		CUnionMember *  addUnionMember(const stUnionMemberInfo&);
		CUnionMember *  addUnionMaster(const stUnionInfo& info);
		void			changeMaster(CUnionMember* pMember);
		void			sendUserUnionData(const char *pName);
		void			fireUnionMember(const char *, const char *);
		int			fireUnionMemberDirect(const DWORD dwCharID, const bool checksept=true);
		void 			fireSeptFromUnion(const DWORD dwSeptID);

		void			sendCmdToAllMember(const Cmd::stNullUserCmd * ptCmd, const unsigned int nCmdLen );
		void 			sendCmdToAllMemberScene(Cmd::Session::t_enterWar_SceneSession* ptEnterWarCmd, const unsigned int cmdLen);
		void			sendCallCmdToAllMemberScene(Cmd::Session::t_GoTo_Leader_Check_SceneSession* ptEnterWarCmd, const unsigned int cmdLen,DWORD expect);
		void			notifyMemberFire(const char *,const char *);
		void      	        notifyWarResult(const char* msg,...);
		void			writeDatabase();
		void			sendUnionMemberList(UserSession *pUser);
		void			sendUnionInfoToUser(UserSession *pUser);
		bool			deleteMeFromDB();
		void 			sendUnionNotify(const char* message, ...);

		bool			isMember(DWORD dwUserID);

		void			disbandUnion();
		void			delUnion(const DWORD dwUnionID);
		bool			loadUnionMemberFromDB();
		DWORD			size();
		bool			isVote();
		void			letVoteOver();		
		void			setNote(Cmd::stNoteUnionCmd *pCmd);
	
		void			sendUnionInfoToAll();
		void			sendUnionManaToAll();
		// 判断能否解散帮会
		bool			isDel();

		// 统计本帮有几个家族
		DWORD 			septSize();

		void 			changeAllSeptRepute(int repute);
	
		// 刷新该帮会所有家族经验领取标志	
		void 			refreshSeptExp();


		// 获得帮会威望
		DWORD			getMana();
		
		// 更新所有成员数据到场景	
		void 			update_all_data();

		// 处理一个成员的直接离开
		void		fireUnionMemberLeave(DWORD dwUserID);	
};

struct CUnionMember:public zEntry
{
public:

		/// 会员别名
		char  aliasname[MAX_NAMESIZE+1];
		/// 会员权限
		BYTE  byPower[2];

		/// 会员状态
		BYTE  byStatus;

		/// 会员职业
		WORD  wdOccupation;

		/// 会员所属家族
		DWORD septid;

		/// 用户的会话对象
		//UserSession * user;

		/// 帮会管理器对象指针
		CUnion *myUnion;

		/// 毁灭状态不再执行写库等操作
		bool  destroy;
		
		/// 互斥锁
		zRWLock rwlock;

		/// 在线标志枚举
		enum {
					Offline,
					Online
		};


		void init(const stUnionMemberInfo&);
		void getMemberBaseInfo(struct Cmd::stUnionRember& info);
		void sendUserUnionPrivateData();
		void sendUnionDare(UserSession* pDareUser, const char* fromName, DWORD dwWarID);
		void sendCmdToMe(const Cmd::stNullUserCmd *pstrCmd, const unsigned int nCmdLen);
		void writeDatabase();
		void insertDatabase();
		void setUnion(CUnion * pUnion);
		void online(const unsigned int status = Cmd::UNION_MEMBER_STATUS_ONLINE);
		bool isOnline();
		void offline();
		bool havePower(const int power);
		void setPower(const int power);
		void sendMessageToMe(const char *);
		void fireMe(const bool notify = true, const bool checksept = true);
		bool deleteMeFromDB();
		void sendUserUnionData();
		void change_aliasname(const char* aliasname);
		void update_data();
			
		CUnionMember();
		~CUnionMember();
		
};

#endif

