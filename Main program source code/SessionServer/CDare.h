/**
 * \file CDare.h
 * \version  $Id: CDare.h  $
 * \author  
 * \date 
 * \brief 定义家族类，家族管理器
 *
 */


#ifndef _CLASS_CDARE_H_
#define _CLASS_CDARE_H_
#include <vector>
#include "zMutex.h"
#include "zMisc.h"
#include "zType.h"
#include "zEntry.h"
#include "zEntryManager.h"
#include "Command.h"
#include "SessionCommand.h"
#include "zUser.h"
#include "zUniqueID.h"
#include "zTime.h"
#include "zSingleton.h"

class UserSession;
class zRWLock;
class CDare;
class CSchool;
class CSept;
class CUnion;

extern unsigned int dare_active_time;
extern unsigned int dare_ready_time;
extern int dare_need_gold;
extern int dare_winner_gold;

extern int dare_need_gold_sept;
extern int dare_winner_gold_sept;

typedef zUniqueID<DWORD> zUniqueDWORDID;

/**
 * \brief 对战记录管理类
 *
 * 管理对战记录的容器,使用Singleton模式
 */
class CDareM:public zEntryManager<zEntryTempID>, 
	     public Singleton<CDareM>
{
	friend class SingletonFactory<CDareM>;

	private:
		zUniqueDWORDID *channelUniqeID;
		zRWLock rwlock;

		CDareM();

	public:
		~CDareM();

		bool getUniqeID(DWORD &tempid);
		void putUniqeID(const DWORD &tempid);

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

		bool init();
		static void destroyMe();

		/**
		 * \brief 对战管理器的定时器回调函数
		 *
		 * 定时遍历所有对战记录，删除无效记录，结束达到时间的对战等相关处理。
		 */
		void timer();


		/**
		 * \brief 处理用户的对战命令
		 *
		 * \param pUser 发送该命令的玩家相对应的UserSession对象
		 * \param ptNullCmd 收到的命令
		 * \param cmdLen 命令长度
		 * \return 是对战命令，并得到相应处理，返回为true, 否则为false
		 *
		 */
		bool processUserMessage(UserSession *pUser, const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen);

		
		/**
		 * \brief 处理用户的家族随意战对战命令
		 *
		 * \param pUser 发送该命令的玩家相对应的UserSession对象
		 * \param ptNullCmd 收到的命令
		 * \param cmdLen 命令长度
		 * \return 是对战命令，并得到相应处理，返回为true, 否则为false
		 *
		 */
		bool processSeptDare(UserSession *pUser, Cmd::stActiveDareCmd *pCmd, const unsigned int cmdLen);

		/**
		 * \brief 处理用户的帮会随意战对战命令
		 *
		 * \param pUser 发送该命令的玩家相对应的UserSession对象
		 * \param ptNullCmd 收到的命令
		 * \param cmdLen 命令长度
		 * \return 是对战命令，并得到相应处理，返回为true, 否则为false
		 *
		 */
		bool processUnionDare(UserSession *pUser, Cmd::stActiveDareCmd *pCmd, const unsigned int cmdLen);


		/**
		 * \brief 处理场景发送过来的服务器间消息
		 *
		 * \param cmd 场景服务器发送过来的命令指令
		 * \param cmdLen 命令长度
		 *
		 * \return 是已定义的服务器间命令，并得到相应处理，返回true, 否则为false.
		 *
		 */
		bool processSceneMessage(const Cmd::t_NullCmd *cmd, const unsigned int cmdLen);

		/**
		 * \brief 查找符合条件的对战记录
		 *
		 *  当ID2为0时，查找是否有ID1参战的记录。dwType参数必须输入。
		 *
		 * \param dwType 对战类型:Cmd:UNION_DARE, Cmd:SCHOOL_DARE, Cmd:SEPT_DARE
		 * \param dwID1  攻方ID
		 * \param dwID2  守方ID
		 *
		 * \return 找到则返回对应对战记录的指针
		 */
		CDare * findDareRecord(DWORD dwType, DWORD dwID1, DWORD dwID2);
		CDare*  findDareRecordByFull(DWORD dwType, DWORD dwDefID, std::vector<DWORD>& vDareList);

		/**
		 * \brief 查找该社会关系指定的的对战记录,不管该社会关系处于攻方还是守方
		 *
		 * \param dwType 对战类型:Cmd:UNION_DARE, Cmd:SCHOOL_DARE, Cmd:SEPT_DARE
		 * \param dwID  社会关系ID
		 *
		 * \return 找到则返回对应对战记录的指针
		 */
		CDare*  findDareRecordByID(DWORD dwType, DWORD dwID);
		
		/**
		 * \brief 查找符合条件的对战记录
		 *
		 *  r1,r2没有顺序限制，不区别哪一个r是挑战方的名称
		 *  当r2为NULL时，查找是否有r1参战的记录。dwType参数必须输入。
		 *
		 * \param dwType 对战类型:Cmd:UNION_DARE, Cmd:SCHOOL_DARE, Cmd:SEPT_DARE
		 * \param r1  社会关系名称1
		 * \param r2  社会关系名称2
		 *
		 * \return 找到则返回对应对战记录的指针
		 */

		CDare * findDareRecordByRelationName(DWORD dwType, const char* r1, const char* r2);

		/**
		 * \brief 查找符合条件的对战记录
		 *
		 *  ID1,ID2没有顺序限制，不区别哪一个ID是挑战方的ID。
		 *  当ID2为0时，查找是否有ID1参战的记录。dwType参数必须输入。
		 *
		 * \param dwType 对战类型:Cmd:UNION_DARE, Cmd:SCHOOL_DARE, Cmd:SEPT_DARE
		 * \param dwUserID1  玩家ID1
		 * \param dwUserID2  玩家ID2
		 *
		 * \return 找到则返回对应对战记录的指针
		 */
		CDare*  findDareRecordByUser(DWORD dwType, DWORD dwUserID1, DWORD dwUserID2);


		/**
		 * \brief 加入新的对战记录
		 *
		 * \param pCmd 对战请求命令
		 * \param dwType 对战类型
		 * \param dwID1 挑战者社会关系ID
		 * \param dwID2 应战者社会关系ID
		 * \param dwUserID 挑战者ID
		 *
		 * \return 没有重复记录，并加入成功，返回true,否则返回false
		 *
		 */
		bool addNewDareRecord(Cmd::stActiveDareCmd *pCmd, DWORD dwType, DWORD dwID1, DWORD dwID2, DWORD dwUserID);

		/**
		  * \brief 用户上线处理
		  *
		  * 用户上线时，同时判断该用户的对战状态，并做相应处理
		  *
		  * \param pUser 上线用户
		  *
		  */
		void userOnline(UserSession* pUser);

		void userOnlineCountry(UserSession* pUser);

		/**
		 * \brief 处理场景回复的t_activeDare命令
		 *
		 * \param cmd 对战记录
		 */
		void activeDare_sceneSession(Cmd::Session::t_activeDare_SceneSession* cmd);

		/**
		 * \brief 处理场景发过来的战斗时的PK战果命令
		 *
		 * \param cmd 命令指针
		 */
		void darePk_sceneSession(Cmd::Session::t_darePk_SceneSession* cmd);
		
		/**
		 * \brief 处理场景发过来的新建对战的命令
		 *
		 * \param pCmd 命令指针
		 */

		void createDare_sceneSession(Cmd::Session::t_createDare_SceneSession* pCmd, std::vector<DWORD>& dare_list);
		

		void createUnionCityDare_sceneSession(Cmd::Session::t_UnionCity_Dare_SceneSession* pCmd);
};

class CDare:public zEntry
{
	protected:
		zRWLock rwlock;                             // 读写锁

		/**
		 * \brief 计算并发送奖金
		 *
		 *  该函数目前只被setWaitBountyState函数调用
		 *
		 * \return 发送奖金成功，返回TRUE, 否则返回失败。
		 */
		virtual bool computeResult();

		virtual char* getFirstName(){return NULL;}    	// 挑战者社会关系的名字
		virtual DWORD getSecondUserID(){return 0;}  	// 应战者玩家的ID

		/// 对战进行时间
		DWORD active_time;

		/// 对战前的等待时间
		DWORD ready_time;

		// 对战开始时间
		time_t	start_time;

	public:
		enum{
			DARE_READY,		// 邀请等待状态
			DARE_READY_QUESTION,    // 等待被挑战者回应状态      
			DARE_READY_ACTIVE,      // 等待扣除应战者金币，如果失败，则进入DARE_RETURN_GOLD
			DARE_RETURN_GOLD,       // 如果金币不够，则把预先扣除的挑战方的金钱返还给他。并进入DARE_OVER状态。
			DARE_ACTIVE,		// 交战状态
			DARE_DATAPROCESS,	// 数据处理状态,暂时未用
			DARE_READY_OVER,        // 结束对战，通知场景取消所有人的对战状态，并计算输赢,
			DARE_WAIT_BOUNTY,       // 如果赢方帮主不在线，处于这个状态，该玩家上线后，把奖励发放给他
			DARE_OVER		// 结束清除状态
		};

		CDare(DWORD active_time, DWORD ready_time); 
				
		virtual ~CDare();

		/**
		 * \brief 时间事件处理回调函数，由TimeTick.cpp中函数回调
		 *
		 *  当对战处于DARE_READY状态五分钟，则取消对战，并进入DARE_RETURN_GOLD状态
		 *  当对战处于DARE_READY_QUESTION  当对战处于DARE_READY
		 *
		 *
		 */
		virtual void timer();	//时间事件处理回调


		// 以下为设置对战状态的处理方法。对战状态具体查看对战系统状态转换图
		// 状态转换函数默认情况，皆不需要重载。只需重载指定的几个方法。
		// 如果你想实现一个与默认行为不同的状态转换过程。可能需要重载所有状态转换方法
		
		virtual void setReadyState();
		virtual void setReadyQuestionState();
		virtual void setReadyActiveState(UserSession* pUser);
		virtual void setReturnGoldState();
		virtual void setActiveState();
		virtual void setReadyOverState();
		virtual void setWaitBountyState();
		virtual void setOverState();
		virtual void sendAntiAtt(){};

		virtual void setSecondID(DWORD dwID)=0;
		virtual void addFirstID(DWORD dwID)=0;
		virtual bool isInvalid() {return false; }
		
		// 该社会关系是否属于该对战记录的攻方
		virtual bool isAtt(DWORD dwID);
		
		virtual void notifyWarResult(int winner_type) = 0;

		/**
		 * \brief 发送对战进行状态给场景
		 *
		 *  场景对相应玩家添加对战记录，在sendNineToMe时，进行对战状态的计算,该
		 *  函数只在用户上线处理时被调用。
		 *
		 */
		virtual void sendActiveStateToScene(UserSession* pUser);


		/**
		  * \brief 发送命令给所有参与该对战的玩家
		  *
		  * 发送对战命令给场景。由setActiveState和setReadyOverState函数调用
		  *
		  */
		virtual void 
		sendCmdToAllDarePlayer(Cmd::Session::t_enterWar_SceneSession* cmd, 
				unsigned int cmdLen, unsigned int relationID) = 0;

		/**
		  * \brief 判断是否仍在对战时期
		  *
		  * \return 如果仍在对战时期，返回TRUE,否则返回FALSE
		  */
		virtual bool isActivePeriod()
		{
			time_t cur_time = time(NULL);
			////////////////////////////////////////////////
			return (cur_time-start_time)<(time_t)this->active_time ? true : false;
	        ////////////////////////////////////////////////
		}

		/**
		  * \brief 判断是否仍在准备期
		  *
		  * \return 如果仍在准备期时期，返回TRUE,否则返回FALSE
		  */
		virtual bool isReadyPeriod()
		{
			return this->count<this->ready_time ? true : false;
		}

		/**
		  * \brief 更新计数器
		  *
		  */
		virtual void updateTime()
		{
			rwlock.wrlock();
			this->count += 5;
			rwlock.unlock();
		}

		/**
		 * \brief 战斗加分
		 *
		 *  调用一次，给相应的社会团队加上PK人数一人。得分公式如下：
		 *  grade = grade + user.level/10;
		 *
		 * \param pAtt 攻击方
		 * \param pDef 防守方
		 */
		virtual void addGrade(UserSession* pAtt, UserSession* pDef) = 0;

		void	printState();


		DWORD type;		// 战斗类型:Cmd::UNION_DARE, Cmd::SCHOOL_DARE, Cmd::SEPT_DARE
		
		//DWORD firstID;		// 挑战方社会关系ID1
		std::vector<DWORD> attList;	// 挑战方列表
		DWORD secondID;		// 防守方社会关系ID2

		DWORD userid1;     	// 挑战者ID
		DWORD userid2;     	// 应战者ID
		DWORD grade1;           // 对战团体1的所得分数
		DWORD grade2;		// 对战团体2的所得分数
		DWORD pk1;              // 社会关系1PK人数
		DWORD pk2;              // 社会关系2PK人数
		DWORD count;		// 记数单位为秒
		BYTE  state;       	// 所处状态
		DWORD dwWinnerID;	// 最终的赢家ID，用于多个攻方时使用
		bool  isAntiAtt;	// 设置反攻标志
};


class CDareUnion : public CDare
{
	public:
		CDareUnion();
		CDareUnion(DWORD active_time, DWORD ready_time);
		virtual ~CDareUnion();

	public:		
		// 以下为设置对战状态的处理方法。对战状态具体查看对战系统状态转换图
		virtual void setSecondID(DWORD dwID);
		virtual void addFirstID(DWORD dwID);

		virtual void sendCmdToAllDarePlayer(Cmd::Session::t_enterWar_SceneSession* cmd, unsigned int cmdLen, unsigned int relationID);
		virtual void sendActiveStateToScene(UserSession* pUser);
		virtual void notifyWarResult(int winner_type);

		/**
		 * \brief 战斗加分
		 *
		 *  调用一次，给相应的社会团队加上PK人数一人。得分公式如下：
		 *  grade = grade + user.level/10;
		 *
		 * \param pAtt 攻击方
		 * \param pDef 防守方
		 */
		virtual void addGrade(UserSession* pAtt, UserSession* pDef);

		virtual void timer();
		virtual bool isInvalid();

	protected:		
		virtual char* getFirstName();
		virtual DWORD getSecondUserID();
};

class CDareSchool : public CDare
{
	public:
		CDareSchool();
		virtual ~CDareSchool();

	public:		
		// 以下为设置对战状态的处理方法。对战状态具体查看对战系统状态转换图
		virtual void setSecondID(DWORD dwID);
		virtual void addFirstID(DWORD dwID);
		
		virtual void sendCmdToAllDarePlayer(Cmd::Session::t_enterWar_SceneSession* cmd, unsigned int cmdLen, unsigned int relationID);
		virtual void sendActiveStateToScene(UserSession* pUser);
		virtual void notifyWarResult(int winner_type);

		/**
		 * \brief 战斗加分
		 *
		 *  调用一次，给相应的社会团队加上PK人数一人。得分公式如下：
		 *  grade = grade + user.level/10;
		 *
		 * \param pAtt 攻击方
		 * \param pDef 防守方
		 */
		virtual void addGrade(UserSession* pAtt, UserSession* pDef);

	protected:		
		virtual char* getFirstName();
		virtual DWORD getSecondUserID();

//		CSchool* pFirst;
//		CSchool* pSecond;
};

class CDareUnionCity : public CDareUnion
{
	public:
		CDareUnionCity(DWORD active_time, DWORD ready_time);
		virtual ~CDareUnionCity();

	public:		
		virtual void setSecondID(DWORD dwID){CDareUnion::setSecondID(dwID);}
		virtual void addFirstID(DWORD dwID){CDareUnion::addFirstID(dwID);}
		
		virtual void sendCmdToAllDarePlayer(Cmd::Session::t_enterWar_SceneSession* cmd, 
				unsigned int cmdLen, unsigned int relationID)
		{
			CDareUnion::sendCmdToAllDarePlayer(cmd, cmdLen, relationID);
		}
		
		virtual void sendActiveStateToScene(UserSession* pUser)
		{
			CDareUnion::sendActiveStateToScene(pUser);
		}
		
		virtual void setReadyState();
		virtual void setReadyOverState();

		/**
  		  * \brief 对战结果通知
		  *
		  * \param winner_type 结果类型:0 挑战者胜， 1 应战者胜， 2 战平
		  *		  
		  */
 		virtual void notifyWarResult(int winner_type){};

		virtual void timer();	//时间事件处理回调

		/**
		 * \brief 战斗加分
		 *
		 *  调用一次，给相应的社会团队加上PK人数一人。得分公式如下：
		 *  grade = grade + user.level/10;
		 *
		 * \param pAtt 攻击方
		 * \param pDef 防守方
		 */
		virtual void addGrade(UserSession* pAtt, UserSession* pDef){};
		int last_fulltime;
};

class CDareCountry : public CDare
{
	public:
		CDareCountry();
		CDareCountry(DWORD active_time, DWORD ready_time);
		virtual ~CDareCountry();

	public:		
		// 以下为设置对战状态的处理方法。对战状态具体查看对战系统状态转换图
		virtual void setSecondID(DWORD dwID);
		virtual void addFirstID(DWORD dwID);
		virtual void sendCmdToAllDarePlayer(Cmd::Session::t_enterWar_SceneSession* cmd, unsigned int cmdLen, unsigned int relationID);

		virtual bool isInvalid();

		virtual void setActiveState();
		virtual void setReadyOverState();
		virtual void notifyWarResult(int winner_type) {};
		virtual void addGrade(UserSession* pAtt, UserSession* pDef){};

		virtual void sendAntiAtt();
		virtual void sendActiveStateToScene(UserSession* pUser);
		
		virtual void timer();
};

class CDareAntiCountry : public CDareCountry
{
	public:
		CDareAntiCountry();
		CDareAntiCountry(DWORD active_time, DWORD ready_time);
		virtual ~CDareAntiCountry();

	public:		
		// 只重载结束处理.
		virtual void setReadyOverState();
};

#endif

