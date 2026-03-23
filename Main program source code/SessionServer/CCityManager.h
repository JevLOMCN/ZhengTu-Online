/**
 * \file
 * \version  $Id: CCityManager.h  $
 * \author  
 * \date 
 * \brief 城市管理器
 *
 * 
 */

#ifndef _CCITY_MANAGER_H
#define _CCITY_MANAGER_H
#include <vector>
#include <set>
#include "zType.h"
#include "zRWLock.h"
#include "SessionCommand.h"
#include "zSingleton.h"
const DWORD KING_CITY_ID = 139;

class Record;
class UserSession;

typedef std::set<DWORD> DareSet;

class CCity
{
	public:
		CCity()
		{
			dwCountry = 0;
			dwCityID = 0;
			dwUnionID = 0;
			isAward = 0;
			dwGold = 20000;
			bzero(catcherName, MAX_NAMESIZE);
			//dwDareUnionID = 0;
			vDareList.clear();
		}
		
		~CCity()
		{
		}
		
		void init(Record* rec);
		void writeDatabase();
		bool insertDatabase();
		bool isMe(DWORD country, DWORD cityid, DWORD unionid);
		bool changeUnion(DWORD unionid);
		bool changeCatcher(UserSession* pUser);
		bool cancelCatcher();
		bool abandonCity();
		bool addDareList(DWORD dwUnionID);
		bool isDare(DWORD dwUnionID);
		size_t dareSize()
		{
			return vDareList.size();
		}
		
		char* getName();

		void   beginDare();
		void   endDare();	

		DWORD dwCountry;
		DWORD dwCityID;
		DWORD dwUnionID;
		int   isAward;	// 当天的钱，是否已经刷新
		DWORD dwGold;
		char  name[MAX_NAMESIZE];
		char  catcherName[MAX_NAMESIZE];
		
		//DWORD dwDareUnionID;
		DareSet vDareList;
		
		zRWLock rwlock;
};

class CCityM : public Singleton<CCityM>
{
	friend class SingletonFactory<CCityM>;
	public:
		bool init();
		static void destroyMe();

		void timer();

		bool load();
		bool refreshDB();
		void refreshUnion(DWORD dwCountryID, DWORD dwCityID);
		bool addNewCity(Cmd::Session::t_UnionCity_Dare_SceneSession* pCmd);

		
		CCity* find(DWORD country, DWORD cityid, DWORD unionid);
		CCity* find(DWORD country, DWORD cityid);
		CCity* findByUnionID(DWORD unionid);
		CCity* findDareUnionByID(DWORD unionid);
		bool   isCastellan(UserSession* pUser);
		bool   isCatcher(UserSession* pUser);

		void   awardTaxGold(UserSession *pUser);

		struct cityCallback
		{
			virtual void exec(CCity *)=0;
			virtual ~cityCallback(){};
		};

		void execEveryCity(cityCallback &);//不包括无国籍国家	 
		
		void beginDare();
                void endDare();
	private:
		CCityM();
		std::vector<CCity*> citys;
		bool isBeging;
		zRWLock rwlock;
};

#endif

