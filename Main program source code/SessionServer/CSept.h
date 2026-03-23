/**
 * \file
 * \version  $Id: CSept.h  $
 * \author  
 * \date
 * \brief 定义家族类，家族管理器
 *
 */


#ifndef _CLASS_SEPT_H_
#define _CLASS_SEPT_H_

#include <vector>
#include "zMutex.h"
#include "zMisc.h"
#include "zType.h"
#include "zEntry.h"
#include "zEntryManager.h"
#include "Sept.h"
#include "Command.h"
#include "SessionCommand.h"
#include "zUser.h"
#include "zSceneManager.h"

using namespace SeptDef;

class UserSession;
class zRWLock;
class CSeptMember;
class CSept;

class CSeptM:public zEntryManager<zEntryID,zEntryName>
{
	private:
		/// 成员索引
		std::map<std::string, CSeptMember*> memberIndex;

		/// 类型定义
		typedef std::map<std::string, CSeptMember*>::value_type memberIndexValueType;

		/// 唯一实例指针
		static CSeptM * um;

		/// 读写锁
		zRWLock rwlock;

		CSept *	createSeptAndAddMaster(const stSeptInfo&);
		bool 		initAddSeptMember(const DWORD &, const stSeptMemberInfo &, bool notify = false);
		bool		createSeptDBRecord(stSeptInfo &info);
		void		removeEntryByName(const char * name);
		CSept*		createSeptByDBRecord(const stSeptInfo & info);
		inline std::map<std::string, CSeptMember *>::iterator  findMemberIndex(const char *pName);
		CSeptM();
		~CSeptM();

	public:
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

		CSept * getSeptByName( const char * name);
		
		CSept* getSeptByID(DWORD dwSeptID);
		CSeptMember * getMemberByName(const char * name);

		bool init();
		static CSeptM &getMe();
		static void destroyMe();

		void fireSeptMember(UserSession*, const char *);
		void notifyNpcHoldData(DWORD septid);

		/**
		  * \brief 删除家族成员
		  *
		  * 如果该角色不是家族族长，则踢除他，如果是，则保留，不做处理
		  *
		  * \param dwUserID 角色ID
		  * \param find     为TRUE表示查找，为FALSE为删除
		  *
		  * \return 如果该角色不在任何家族中，则返回2
		  *         如果该角色是族长，则返回0
		  *         如果该角色是族员，并删除成功，则返回1
		  *
		  */
		int  fireSeptMember(DWORD dwUserID, bool find);

		bool addNewMemberToSept(const DWORD dwSeptID, const stSeptMemberInfo&);
		void userOnline(UserSession *);
		void userOffline(const UserSession *);
		void createNewSept(Cmd::Session::t_addSept_SceneSession *data);
		bool addMemberIndex(const char *, CSeptMember *);
		bool removeMemberIndex(const char *);
		bool processUserMessage(UserSession *pUser,const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen);
		
		void processSeptSortMessage(UserSession* pUser, 
				const Cmd::stReqSeptSort* ptCmd);
		
		void processRequestSeptExpMessage(UserSession* pUser, const Cmd::stRequestSeptExpCmd* ptCmd);
		void processRequestSeptNormalExpMessage(UserSession* pUser, const Cmd::stRequestSeptNormalExpCmd* ptCmd);
		
		bool processSceneMessage(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen);
		bool processSceneSeptMessage(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen);
		bool processGateMessage(const Cmd::t_NullCmd *cmd, const unsigned int cmdLen);
		void delSept(const DWORD dwSeptID);
		void processMemberLeave(const DWORD dwSeptID, const DWORD dwCharID);
		void sendSeptNotify(const DWORD septID, const char* message, ...);
		void sendNpcDareCmdToScene(const DWORD septID, Cmd::Session::t_NpcDare_NotifyScene_SceneSession* ptCmd, unsigned int nCmdLen);
		void sendSeptChatMessages(const UserSession *pUser, const Cmd::stChannelChatUserCmd *rev, const unsigned int cmdLen);
		void sendSeptPrivateChatMessages(const UserSession *pUser, const Cmd::stChannelChatUserCmd *rev, const unsigned int cmdLen);
		void sendVoteListToUser(const UserSession *pUser);
		void sendListToUser(const UserSession *pUser);
		void sendDareListToUser(const UserSession *pUser);
		void userVote(const UserSession *pUser, const char *pName);
		void userAboutVote(const UserSession *pUser, const char *pName);
		void addVoteMemberToSept(const UserSession *pUser, const char *pName);
		void disributeExp(Cmd::Session::t_distributeSeptExp_SceneSession *cmd);
		void setSeptNote(UserSession *pUser, Cmd::stNoteSeptCmd *pCmd);
		DWORD findUserSept(DWORD dwUserID);
		void delSeptAllMember();

		/**
		 * \brief 更改别名
		 *
		 * \param pUser 请求更名者(目前只能是帮主)
		 *
		 * \param pCmd 更名命令
		 *
		 * \return 
		 */
		void change_aliasname(UserSession *pUser, Cmd::stChangeSeptMemberAliasName* pCmd);
		void changeAllRepute(DWORD countryid, int repute);
		DWORD getRepute(DWORD dwSeptID);//得到声威值
		void changeRepute(DWORD dwSeptID, int repute);//改变声威值
		void changeLevel(DWORD dwSeptID, int level);
		char * getSeptNameByUserName(char *Name);
		DWORD getSeptIDByUserName(char *Name);		
};

class CSept:public zEntryManager<zEntryID, zEntryName>, public zEntry
{
	private:
		/// 毁灭状态不再执行写库等操作
		bool					destroy;
		
		/// 读写锁
		zRWLock rwlock;

	public:
		/// 是否还处于投票期间，1为是0为否
		BYTE					byVote;

		/// 家族会长
		CSeptMember          *master;

		/// 创立时间
		DWORD					dwCreateTime;

		/// 介绍
		char					note[255];

		/// 家族所属国家
		DWORD dwCountryID;

		/// 家族所属帮会
		DWORD dwUnionID;

		/// 
		void clearUnion()
		{
			this->rwlock.wrlock();
			this->dwUnionID = 0;
			this->rwlock.unlock();
		}
		

		/// 家族等级
		DWORD dwLevel;

		/// 家族声望
		DWORD dwRepute;

		/// 家族消费金币
		DWORD dwSpendGold;
		
		/// 家族是否领取经验
		DWORD dwIsExp;

		/// 令牌使用次数
		DWORD calltimes;
		/// 令牌使用时间
		DWORD calldaytime;
		/// 上次领取普通家族经验的时间
		DWORD normal_exp_time;

		/// 家族BOSS经验
		DWORD bossexp;

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

		CSept();
		~CSept();
		void			removeEntryByName(const char * name);
		DWORD			getID() {return id;}
		void			init(const stSeptInfo & info);
		CSeptMember *  		addSeptMember(const stSeptMemberInfo&);
		CSeptMember *  		addSeptMaster(const stSeptInfo& info);
		CSeptMember *		getMemberByName(const char* pName);
		void			sendUserSeptData(const char *pName);
		void			fireSeptMember(const char *, const char *);
		int			fireSeptMemberDirect(const DWORD dwCharID, const bool checkunion=true);

		void			sendCmdToAllMember(const Cmd::stNullUserCmd * ptCmd, const unsigned int nCmdLen );
		void			sendCmdToAllMemberExcept(const Cmd::stNullUserCmd * ptCmd, const unsigned int nCmdLen , const char * except);
		bool			isMember(DWORD dwUserID);
		void                    sendSeptNotify(const char* message, ...);

		void			sendCmdToAllMemberScene(Cmd::Session::t_enterWar_SceneSession* ptEnterWarCmd, const unsigned int cmdLen);
		void			sendCallCmdToAllMemberScene(Cmd::Session::t_GoTo_Leader_Check_SceneSession* ptEnterWarCmd, const unsigned int cmdLen,DWORD expect);
		void			sendNpcDareCmdToScene(Cmd::Session::t_NpcDare_NotifyScene_SceneSession* pCmd, unsigned int cmdLen);

		void			sendDistributeSeptExpToScene(const DWORD dwUserID, const Cmd::Session::t_distributeSeptExp_SceneSession * ptCmd);
		void			notifyMemberFire(const char *);
		void			writeDatabase();
		void			sendSeptMemberList(UserSession *pUser);
		void			full_SeptMember_Data(Cmd::stUnionRember*& tempPoint, unsigned int& count);
		void			sendSeptInfoToUser(UserSession *pUser);
		bool			deleteMeFromDB();
		void			disbandSept();
		void			delSept(const DWORD dwSeptID);
		bool			loadSeptMemberFromDB();
		DWORD			size();
		bool			isVote();
		void			letVoteOver();
		
		void	setExp()
		{
			rwlock.wrlock();
			this->dwIsExp = 1;
			rwlock.unlock();
			this->writeDatabase();
		}
		
		void	clearExp()
		{
			rwlock.wrlock();
			this->dwIsExp = 0;
			rwlock.unlock();
			this->writeDatabase();
		}

		DWORD getExp()
		{
			return this->dwIsExp;
		}

		void			setNote(Cmd::stNoteSeptCmd *pCmd);
		void                    notifyWarResult(const char* msg, ...);
		void 			delSeptAllMember();
		DWORD 			getRepute();//得到声威值
		void                    changeRepute(int repute);//改变声威值
		void			changeLevel(int level);
		void			sendSeptInfoToAll(); // 重新通知家族信息
		void			sendSeptReputeToAll(); // 通知所有家族成员声望的改变
		void			notifyNpcHoldData();
		void			sendGoldToMember(DWORD userID, DWORD num);
};

struct CSeptMember:public zEntry
{
public:
		/// 族员状态
		BYTE  byStatus;
		
		/// 族员别名
		char  aliasname[MAX_NAMESIZE+1];

		/// 族员职业
		WORD  wdOccupation;

		/// 用户的会话对象
		//UserSession * user;

		/// 家族管理器对象指针
		CSept *mySept;

		/// 毁灭状态不再执行写库等操作
		bool  destroy;
		
		/// 互斥锁
		zRWLock rwlock;

		/// 再线状态枚举
		enum {
					Offline,
					Online
		};


		void init(const stSeptMemberInfo&);
		void getMemberBaseInfo(struct Cmd::stSeptRember& info);
		void sendCmdToMe(const Cmd::stNullUserCmd *pstrCmd, const unsigned int nCmdLen);
		void writeDatabase();
		void insertDatabase();
		void setSept(CSept * pSept);
		void online(const unsigned int status = Cmd::SEPT_MEMBER_STATUS_ONLINE);
		void offline();
		bool isOnline();
		void sendMessageToMe(const char *);
		void sendMessageToMe(int type, const char *);
		void fireMe(const bool notify = true, const bool checkunion=true);
		bool deleteMeFromDB();
		void sendUserSeptData();
		void change_aliasname(const char* aliasname);
		void update_data();
		void update_normal_data();
		void notifyNpcHoldData();
		void sendGoldToMember(DWORD userID, DWORD num);
		CSeptMember();
		~CSeptMember();
		
};

#endif

