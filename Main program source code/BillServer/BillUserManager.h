#ifndef _BILLUSERMANAGER_H_
#define _BILLUSERMANAGER_H_
#include "BillUser.h"
#include "zEntryManager.h"


class BillUserManager :public zEntryManager< zEntryID>
{
	private:
		zRWLock rwlock;

	private:
		BillUserManager();
		~BillUserManager();
		static BillUserManager *instance;
	public:
		static BillUserManager *getInstance();
		static void delInstance();
		bool getUniqeID(DWORD &tempid);
		void putUniqeID(const DWORD &tempid);
		BillUser * getUserByID( DWORD id)
		{
			rwlock.rdlock();
			BillUser *ret =(BillUser *)getEntryByID(id);
			rwlock.unlock();
			return ret;
		}
		/*
		BillUser * getUserByTempID( DWORD tempid)
		{
			rwlock.rdlock();
			BillUser *ret =(BillUser *)getEntryByTempID(tempid);
			rwlock.unlock();
			return ret;
		}
		// */
		bool addUser(BillUser *user)
		{
			rwlock.wrlock();
			bool ret =addEntry((zEntry *)user);
			rwlock.unlock();
			return ret;
		}
		void removeUser(BillUser *user)
		{
			rwlock.wrlock();
			removeEntry((zEntry *)user);
			rwlock.unlock();
		}
		template <class YourUserEntry>
			bool execEveryUser(execEntry<YourUserEntry> &exec)
			{
				rwlock.rdlock();
				bool ret=execEveryEntry<>(exec);
				rwlock.unlock();
				return ret;
			}



	public:
		/**
		 * \brief 会话超时时间
		 * 单位，秒
		 */
		void update();
		void removeUserByGatewayID(BillTask *task);
		//bool updateGold(DWORD acc , double gold);
		//bool updateVipTime(DWORD acc , DWORD vip);
		//DWORD getVipTime(DWORD acc);
		//double getGold(DWORD acc);
		//BillInfo get(const DWORD accid);


};





#endif
