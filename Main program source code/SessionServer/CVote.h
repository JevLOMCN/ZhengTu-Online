/**
 * \file
 * \version  $Id: CVote.h  $
 * \author  
 * \date 
 * \brief 投票管理系统定义
 *
 * 
 */

#ifndef _CVOTE_MANAGER_H
#define _CVOTE_MANAGER_H
#include <vector>
#include "zType.h"
#include "zRWLock.h"
#include "SessionCommand.h"
#include "zSingleton.h"
#include "zEntry.h"
#include "zEntryManager.h"

class Record;
class RecordSet;
class UserSession;
class CVoteM;
class CTech;

class CVoteItem
{
	public:
		DWORD dwOption;
		char szOptionDesc[MAX_NAMESIZE];
		DWORD dwBallot;
		DWORD dwVoteID;
		void init(Record* rec);
		void writeDatabase();
		bool insertDatabase();
		bool deleteMeFromDB();
		CVoteItem();
		~CVoteItem();
};

class CVote
{
	friend class CVoteM;
	public:
		enum
		{
			VOTE_READY,             // 准备状态
			VOTE_ACTIVE,            // 正在投票状态
			VOTE_READY_OVER,        // 结束投票，修改对应国家科技状态
			VOTE_OVER               // 结束清除状态
		};
		
		void init(Record* rec);
		void writeDatabase();
		bool insertDatabase();
		bool deleteMeFromDB();
		bool loadItemFromDB();
		bool loadItemFromVec(std::vector<CTech*>& itemset);
		
		virtual void setReadyState();
		virtual void setActiveState();  
		virtual void setReadyOverState();
		virtual void setOverState();    
		virtual bool isActiveState();
		
		DWORD 	getState()
		{
			return this->dwStatus;
		}

		DWORD 	getType()
		{
			return this->dwType;
		}
		
		virtual void vote(UserSession* pUser, DWORD dwOption);
		bool addVoted(DWORD dwCharID);
		bool clearVoted();
		
		CVote();	
		virtual ~CVote();

	protected:
		DWORD dwID;
		DWORD dwCountryID;
		DWORD dwType;
		DWORD dwStatus;

		zRWLock rwlock;
		std::vector<CVoteItem*> items;
};

class CVoteM : 	public Singleton<CVoteM>
{
	friend class SingletonFactory<CVoteM>;
	public:
		bool init();
		static void destroyMe();
		void timer();
		bool load();

		bool processUserMessage(UserSession *pUser,const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen);
                bool processSceneMessage(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen);

		CVote* find(DWORD dwCountryID, DWORD dwType);
		CVote* findByID(DWORD dwVoteID);
		
		~CVoteM();
		
		bool createNewVote(DWORD dwCountry, DWORD dwType, std::vector<CTech*>& items);
		void removeVote(DWORD dwCountry, DWORD dwType);
		void removeVoteByID(DWORD dwID);
		void force_close_vote(DWORD dwCountryID, DWORD dwType);
		
	protected:
		CVoteM();
		std::vector<CVote*> votes;
		void addNewVote();
		zRWLock rwlock;
};

#endif

