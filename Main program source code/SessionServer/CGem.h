/**
 * \file CDare.h
 * \version  $Id: CGem.h  $
 * \author  
 * \date 
 * \brief 定义护宝任务管理器
 *
 */


#ifndef _CLASS_CGEM_H_
#define _CLASS_CGEM_H_

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
class CGem;

typedef zUniqueID<DWORD> zUniqueDWORDID;

struct ArhatMapPoint
{
	DWORD dwMapID;
	DWORD x;
	DWORD y;
};
const int mappoint_num = 10;

/**
 * \brief 护宝任务记录管理类
 *
 * 管理对战记录的容器,使用Singleton模式
 */
class CGemM:public zEntryManager<zEntryTempID>, 
	    public Singleton<CGemM>
{
	friend class SingletonFactory<CGemM>;

	private:
	zUniqueDWORDID *channelUniqeID;
	zRWLock rwlock;

	CGemM();

	public:
	~CGemM();

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

	ArhatMapPoint wait_point[mappoint_num];
	bool init();
	static void destroyMe();

	/**
	 * \brief 护宝任务管理器的定时器回调函数
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
	 * \return 是护宝任务相关命令，并得到相应处理，返回为true, 否则为false
	 *
	 */
	bool processUserMessage(UserSession *pUser, const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen);

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
	 * \brief 查找符合条件的护宝任务
	 *
	 *  当ID2为0时，查找是否有ID1参战的记录。dwType参数必须输入。
	 *
	 * \param dwType 对战类型:Cmd:UNION_DARE, Cmd:SCHOOL_DARE, Cmd:SEPT_DARE
	 * \param dwID1  攻方ID
	 * \param dwID2  守方ID
	 *
	 * \return 找到则返回对应对战记录的指针
	 */
	CGem * findGem(DWORD dwCountryID);

	/**
	 * \brief 根据ID查找护宝任务
	 *
	 * \param dwID  GEM ID
	 *
	 * \return 找到则返回对应护宝任务记录的指针
	 */
	CGem*  findGemByID(DWORD dwID);

	/**
	 * \brief 加入新的对战记录
	 *
	 * \param dwCountryID 任务所属国家
	 *
	 * \return 没有重复记录，并加入成功，返回true,否则返回false
	 *
	 */
	bool addNewGem(DWORD dwCountryID);
				

	void forceEnd();

	/**
	 * \brief 用户上线处理
	 *
	 * 用户上线时，同时判断该用户的对战状态，并做相应处理
	 *
	 * \param pUser 上线用户
	 *
	 */
	void userOnline(UserSession* pUser);
	
	/**
	 * \brief 用户下线处理
	 *
	 * 用户下线时，同时状态他所处的护宝状态，并做相应处理
	 *
	 * \param pUser 下线用户
	 *
	 */
	void userOffline(UserSession* pUser);
};

/// 罗汉
class CArhat
{
	public:
	DWORD x;
	DWORD y;
	DWORD dwCountryID;
	DWORD dwMapRealID;
	BYTE  byState; // 状态，0为道具未被人获取，1为道具已被人获取
	DWORD dwHoldUserID; // 道具目前所属玩家ID
	DWORD dwID;	// 罗汉的ID

	CArhat()
	{
		x = 0;
		y = 0;
		dwCountryID = 0;
		dwMapRealID = 0;
		byState = 0;
		dwHoldUserID = 0;
		dwID = 0;
	}

	bool refreshNPC();
	bool clearNPC();
};

class CGem : public zEntry
{
	protected:
		zRWLock rwlock;                             // 读写锁
		time_t tmStart;				    // 开始时间

	public:
		enum{
			GEM_READY,		// 护宝任务准备状态
			GEM_ACTIVE,		// 护宝任务进行状态
			GEM_READY_OVER,        // 护宝任务准备结束状态
			GEM_OVER		// 结束清除状态
		};

		CGem();

		virtual ~CGem();

		/**
		 * \brief 时间事件处理回调函数，由TimeTick.cpp中函数回调
		 *
		 *  当对战处于DARE_READY状态五分钟，则取消对战，并进入DARE_RETURN_GOLD状态
		 *  当对战处于DARE_READY_QUESTION  当对战处于DARE_READY
		 *
		 *
		 */
		virtual void timer();	//时间事件处理回调


		// 状态处理方法,由timer做状态变迁的处理
		virtual void setReadyState();
		virtual void setActiveState();
		virtual void setReadyOverState();
		virtual void setOverState();

		/**
		 * \brief 发送对战进行状态给场景
		 *
		 *  场景对相应玩家添加对战记录，在sendNineToMe时，进行对战状态的计算,该
		 *  函数只在用户上线处理时被调用。
		 *
		 */
		virtual void sendActiveStateToScene(UserSession* pUser);

		/**
		 * \brief 判断是否仍在护宝任务时期
		 *
		 * \return 如果仍在护宝任务时期，返回TRUE,否则返回FALSE
		 */
		virtual bool isActivePeriod();

		void	printState();

		void    holdDragon(UserSession* pUser);
		void    holdTiger(UserSession* pUser);

		void    resetDragon();
		void    resetTiger();

		CArhat dragon; // 龙精
		CArhat tiger;   // 虎魄
		BYTE   state;       	// 所处状态
		DWORD  dwCountryID;	// 护宝任务所属国家
};
#endif

