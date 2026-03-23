/**
 * \file
 * \version  $Id: CSort.h $
 * \author  
 * \date 
 * \brief 定义等级排序对象
 *
 */


#ifndef _CLASS_CSORT_H_
#define _CLASS_CSORT_H_

#include "zMisc.h"
#include "zType.h"
#include "zUser.h"
#include "Session.h"
#include "zSceneManager.h"
#include "CharBase.h"



class CSortM
{
	private:

		CSortM();

		static CSortM *csm;
		//WORD leveltable[MAX_LEVEL+10];

		/**
		 * \brief 排序
		 * \author fqnewman
		 */
		struct ltqword
		{
			bool operator()(const QWORD s1, const QWORD s2) const
			{
				return s1>s2;
			}
		};
		std::multimap<QWORD, DWORD, ltqword> _sortKey;
		std::map<DWORD, QWORD> _sortMap;

		typedef std::multimap<QWORD, DWORD, ltqword>::value_type keyValueType;
		typedef std::map<DWORD, QWORD>::value_type mapValueType;

	public:
		~CSortM();
		static CSortM &getMe();
		bool init();
		static void destroyMe();
		void onlineCount(UserSession *pUser);
		void onlineCount(DWORD dwCharID, WORD wdLevel, QWORD qwExp);
		void offlineCount(UserSession *pUser);
		WORD getLevelDegree(UserSession *pUser);
		void upLevel(UserSession *pUser);
		bool createDBRecord();
		bool clearDBTable();

};

#endif

