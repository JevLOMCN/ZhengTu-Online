#ifndef _AUCTIONSERVICE_H_
#define _AUCTIONSERVICE_H_
#include "zType.h"
#include "zMisc.h"
#include "SessionCommand.h"

#pragma pack(1)
/*
struct auctionInfo
{
	char owner[MAX_NAMESIZE+1];
	BYTE state;
	DWORD charge;
	char itemName[MAX_NAMESIZE+1];
	BYTE type;
	BYTE quality;
	WORD needLevel;
	DWORD minMoney;
	DWORD maxMoney;
	DWORD minGold;
	DWORD maxGold;
	char bidder[MAX_NAMESIZE+1];
	char bidder2[MAX_NAMESIZE+1];
	DWORD startTime;
	DWORD endTime;
};
*/
#pragma pack(1)
struct auctionBidInfo
{
	DWORD auctionID;
	DWORD ownerID;
	char owner[MAX_NAMESIZE+1];
	BYTE state;
	DWORD charge;
	DWORD minMoney;
	DWORD maxMoney;
	DWORD minGold;
	DWORD maxGold;
	DWORD bidderID;
	DWORD bidder2ID;
	char bidder[MAX_NAMESIZE+1];
	char bidder2[MAX_NAMESIZE+1];
	DWORD endTime;
	BYTE bidType;
	DWORD itemID;
	Cmd::Session::SessionObject item;
};
#pragma pack()

class AuctionService
{
	private:
		static AuctionService *as;
		AuctionService();
		~AuctionService();
	public:
		static AuctionService& getMe();
		static void delMe();

		bool doAuctionCmd(const Cmd::Session::t_AuctionCmd *cmd, const DWORD cmdLen);
		void checkDB();

		bool sendAuctionItem(DWORD, DWORD, BYTE, bool);
		void delAuctionRecordByName(char *);

		static bool error(const char * msg, ...);
};

#endif
