#ifndef _GIFT_H_
#define _GIFT_H_

#include "zType.h"
#include "zMisc.h"
#include "SessionCommand.h"
#include "Session.h"
#include "zSingleton.h"

#pragma pack(1)
struct actInfo
{
	DWORD id;
	char name[MAX_NAMESIZE];
	BYTE state;
	char text[MAX_CHATINFO];
};

/*
struct giftInfo
{
	DWORD actID;
	DWORD charID;
	BYTE itemGot;
	char mailText[MAX_CHATINFO];
	DWORD money;
	DWORD itemID;
	BYTE itemType;
	DWORD itemNum;
};
*/
#pragma pack()

class Gift : public Singleton<Gift>
{
	friend class SingletonFactory<Gift>;
	private:
		Gift();

		static std::vector<actInfo> actList;
		static std::multimap<DWORD, Cmd::Session::giftInfo> giftList;
		static std::multimap<DWORD, std::string> winnerList;
		static DWORD loadActList();
		static DWORD loadGiftList();
	public:
		~Gift();
		bool init();

		bool doGiftCmd(UserSession * pUser, const Cmd::stNullUserCmd *cmd, const unsigned int cmdLen);
};

#endif
