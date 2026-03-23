/**
 * \file
 * \version  $Id: CDareRecordManager.h  $
 * \author 
 * \date 
 * \brief 对战记录管理器
 *
 * 
 */

#ifndef _CDARERECORD_MANAGER_H
#define _CDARERECORD_MANAGER_H
#include <vector>
#include "zType.h"
#include "zRWLock.h"
#include "SessionCommand.h"
#include "zSingleton.h"

class Record;
class UserSession;

class CDareRecord
{
	public:
		CDareRecord()
		{
			dwTime = 0;
			dwAttCountryID = 0;
			dwDefCountryID = 0;
			dwResult = 0;
			bzero(attKingName, sizeof(attKingName));
			bzero(defKingName, sizeof(defKingName));
		}
		
		~CDareRecord()
		{
		}
		
		void init(Record* rec);
		//void writeDatabase();
		bool insertDatabase();
		//bool isMe(DWORD country, DWORD cityid, DWORD unionid);
		//bool changeUnion(DWORD unionid);

		DWORD	dwTime;
		DWORD	dwAttCountryID;
		DWORD	dwDefCountryID;
		DWORD	dwResult;
		char    attKingName[MAX_NAMESIZE+1];
		char    defKingName[MAX_NAMESIZE+1];

		zRWLock rwlock;

};

class CDareRecordM : public Singleton<CDareRecordM>
{
	friend class SingletonFactory<CDareRecordM>;
	
	public:
		bool init();
		void timer();
		void destoryMe()
		{
			delMe();
		};

		bool load();
		bool addNewDareRecord(DWORD dwAttCountry, DWORD dwDefCountry, DWORD dwResult);
		bool processUserMessage(UserSession *pUser,const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen);
		CDareRecord* findLastRecord(DWORD dwAttCountry, DWORD dwDefCountry);

	private:
		CDareRecordM(){}
		std::vector<CDareRecord*> vDareRecord;
		zRWLock rwlock;
};

#endif

