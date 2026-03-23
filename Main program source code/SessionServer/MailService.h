#ifndef _MAILSERVICE_H_
#define _MAILSERVICE_H_
#include "zType.h"
#include "zMisc.h"
#include "SessionCommand.h"
#include "zSingleton.h"
#include <ext/hash_map>
#include <ext/hash_set>

#pragma pack(1)
struct mailHeadInfo
{
	DWORD id;
	BYTE state;
	char fromName[MAX_NAMESIZE+1];
	DWORD delTime;
	BYTE accessory;
	BYTE itemGot;
	BYTE type;
};

struct mailContentInfo
{
	DWORD id;
	BYTE state;
	char toName[MAX_NAMESIZE+1];
	char title[MAX_NAMESIZE+1];
	BYTE accessory;
	BYTE itemGot;
	char text[256];
	DWORD sendMoney;
	DWORD recvMoney;
	DWORD sendGold;
	DWORD recvGold;
	DWORD toID;
	Cmd::Session::SessionObject item;
};

struct mailStateInfo
{
	BYTE state;
};

struct mailNewInfo
{
	DWORD id;
	DWORD toID;
};

struct mailCheckInfo
{
	DWORD id;
	char toName[MAX_NAMESIZE+1];
	DWORD toID;
};

struct mailTurnBackInfo
{
	BYTE state;
	char fromName[MAX_NAMESIZE+1];
	char toName[MAX_NAMESIZE+1];
	char title[MAX_NAMESIZE+1];
	BYTE type;
	DWORD createTime;
	DWORD delTime;
	BYTE accessory;
	BYTE itemGot;
	char text[256];
	DWORD recvMoney;
	DWORD recvGold;
	DWORD fromID;
	DWORD toID;
};

struct mailForwardInfo
{
	BYTE state;
	char fromName[MAX_NAMESIZE+1];
	char toName[MAX_NAMESIZE+1];
	BYTE type;
	DWORD delTime;
	char text[256];
	DWORD recvMoney;
	DWORD recvGold;
	DWORD toID;
};

#pragma pack()

class MailService : public Singleton<MailService>
{
	friend class SingletonFactory<MailService>;
	private:
		__gnu_cxx::hash_map<DWORD, __gnu_cxx::hash_set<DWORD> > newMailMap;
		MailService();
	public:
		~MailService();

		void loadNewMail();
		bool doMailCmd(const Cmd::t_NullCmd *cmd, const unsigned int cmdLen);
		bool sendTextMail(char *, DWORD, char *, DWORD, char *, DWORD=(DWORD)-1, BYTE=1);
		bool sendMoneyMail(char *,DWORD, char *, DWORD, DWORD, char *, DWORD=(DWORD)-1, BYTE=1, DWORD=0);
		bool sendMail(Cmd::Session::t_sendMail_SceneSession &);
		bool sendMail(DWORD, Cmd::Session::t_sendMail_SceneSession &);
		bool turnBackMail(DWORD);
		void checkDB();
		//sendMail(UserSession *, Cmd::stSendMail *, DWORD);

		void delMailByNameAndID(char *, DWORD);
};

#endif
