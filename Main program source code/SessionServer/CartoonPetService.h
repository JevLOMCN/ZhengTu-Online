
#ifndef _CARTOONPETSERVICE_H_
#define _CARTOONPETSERVICE_H_
//#include "zType.h"
#include "zMisc.h"
#include "SessionCommand.h"
#include "Session.h"
#include <ext/hash_map>
#include <ext/hash_set>

#pragma pack(1)
struct cartoon_load_struct
{
	DWORD cartoonID;        
	Cmd::t_CartoonData data;
	cartoon_load_struct()
	{
		cartoonID = 0;
		bzero(&data, sizeof(data));
	}
};

struct cartoon_relation_struct
{
	DWORD relationID;        
	BYTE type;
};

#pragma pack()

class CartoonPetService
{
	private:
		struct key_hash : public std::unary_function<const std::string, size_t>
		{
			size_t operator()(const std::string &x) const
			{
				__gnu_cxx::hash<const char *> H;
				return H(x.c_str());
			}
		};
		__gnu_cxx::hash_map<DWORD, Cmd::t_CartoonData> cartoonPetList;
		__gnu_cxx::hash_set<DWORD> modifyList;
		__gnu_cxx::hash_set<DWORD> waitingList;
		__gnu_cxx::hash_map<DWORD, __gnu_cxx::hash_set<DWORD> > cartoonPetMap;
		__gnu_cxx::hash_map<std::string, __gnu_cxx::hash_set<DWORD>, key_hash> adoptedPetMap;

		static CartoonPetService *cs;
		CartoonPetService();
		~CartoonPetService();

		void sendCmdToItsFriendAndFamily(DWORD, const char *, const void *, DWORD, const char * = "");
		void repairData();

		bool loadAllFromDB();
		DWORD group;
	public:
		DWORD writeAllToDB(bool groupflag=true);

		static CartoonPetService& getMe();
		static void delMe();

		bool doCartoonCmd(const Cmd::Session::t_CartoonCmd *cmd, const DWORD cmdLen);
		void loadFromDB(DWORD);
		void checkAdoptable(DWORD);
		bool writeDB(DWORD, Cmd::t_CartoonData&);
		void userLevelUp(DWORD id, DWORD level);

		void delPetRecordByID(DWORD masterID);

		void userOnline(UserSession * pUser);
};

#endif
