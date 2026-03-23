/**
 * \file
 * \version  $Id: entrytest.cpp  $
 * \author  
 * \date 
 * \brief Entryº∞EntryManager≤‚ ‘
 *
 */

#include <vector>

#include "zEntryManager.h"

class User:public zEntry
{
public:
	User(const char * name,DWORD id)
	{
		this->id=id;
		strncpy(this->name, name, MAX_NAMESIZE);
	}

	User(const std::string &name,DWORD id)
	{
		this->id=id;
		strncpy(this->name, name.c_str(), MAX_NAMESIZE);
	}
};


class UserManager:protected zEntryManager< zEntryID, zEntryName>
{
	typedef zEntryID::hashmap::iterator iter_type;
	public:

		bool addUser(User * pUser)
		{
			if(pUser==NULL) return false;
			return addEntry((zEntry *) pUser);
		}

		User *getByName(const char *name)
		{
			return (User *)getEntryByName(name);
		}

		User *getByID(DWORD id)
		{
			return (User *)getEntryByID(id);
		}

		void printA()
		{
			struct pes:execEntry<User>
			{
				bool exec(User * p)
				{
					std::cout<< p->name<<std::endl;
					return true;
				}
			};

			pes p;
			//execEveryEntry<iter_type>(p);
		}

		void printE()
		{
			struct per:public removeEntry_Pred<User>
			{
				bool isIt( User * p)
				{
					//std::cout<< p.second->name<<std::endl;
					return true;
				}
			};

			per p;
			//removeEntry_if<iter_type>(p);
			//for(unsigned int i=0;i<p.removed.size();i++)
			//	std::cout<<p.removed[i]->id<< std::endl;
		}
};

int main(int argc , char *argv[])
{
	UserManager *um=new UserManager;
	User *pUser=new User("Œ“øø",100);
	um->addUser(pUser);
	um->printA();
	um->printE();
	um->printA();

	if(1)
	{
		User *pser=um->getByName("Œ“øø");
		//User *pser=um->getByID(100);
		if(pser!=NULL)
			std::cout<< pser->id << std::endl;
	}

	return 0;
}
