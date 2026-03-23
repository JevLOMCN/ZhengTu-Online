#ifndef _MINIUSERMANAGER_H_
#define _MINIUSERMANAGER_H_
#include "MiniUser.h"
#include "zEntryManager.h"
#include "MiniUserCommand.h"


class MiniUserManager :public zEntryManager< zEntryID>
{
	private:
		zRWLock rwlock;
		BYTE saveGroup;

	private:
		MiniUserManager();
		~MiniUserManager();
		static MiniUserManager *instance;

	public:
		static MiniUserManager *getInstance();
		static MiniUserManager &getMe();
		static void delInstance();

		MiniUser * newUser(DWORD id, char *name, WORD country, WORD face, MiniTask *task);
		MiniUser * newUser(Cmd::Mini::t_UserLogin_Gateway *info);

		MiniUser * getUserByID( DWORD id)
		{
			rwlock.rdlock();
			MiniUser *ret =(MiniUser *)getEntryByID(id);
			rwlock.unlock();
			return ret;
		}

		template <class YourUserEntry>
			bool execEveryUser(execEntry<YourUserEntry> &exec)
			{
				rwlock.rdlock();
				bool ret=execEveryEntry<>(exec);
				rwlock.unlock();
				return ret;
			}

		void update();
		void removeUserByGatewayID(MiniTask *task);

		void removeUser(MiniUser *user)
		{
			rwlock.wrlock();
			removeEntry((zEntry *)user);
			delete user;
			rwlock.unlock();
		}

	private:
		bool addUser(MiniUser *user)
		{
			rwlock.wrlock();
			bool ret =addEntry((zEntry *)user);
			rwlock.unlock();
			return ret;
		}
};
#endif
