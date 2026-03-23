#ifndef _SESSION_CHAT_H_
#define _SESSION_CHAT_H_

#include "zEntry.h"
#include "Session.h"
#include "zUniqueID.h"
#include <map>

class SessionChannel : public zEntry
{               
	private:
		std::list<DWORD> userList;
		//char creater[MAX_NAMESIZE];
	public:
		bool sendToOthers(UserSession *pUser, const Cmd::stChannelChatUserCmd *rev,unsigned int cmdLen);

		//bool sendToAll(UserSession *, const char *pattern, ...);
		bool sendCmdToAll(const void *cmd,int len);
		SessionChannel(UserSession *);
		bool remove(DWORD);
		bool remove(UserSession *pUser);
		bool removeAllUser();
		//bool add(const char *name);
		bool add(UserSession *pUser);
		bool has(DWORD);
		DWORD count();

		static bool sendCountry(DWORD, const void *rev,unsigned int cmdLen);
		static bool sendCountryInfo(int type, DWORD countryID, const char* mess, ...);
		static bool sendAllInfo(int type, const char* mess, ...);
		static bool sendAllCmd(const void *cmd, const unsigned int len);
		static bool sendPrivate(UserSession * pUser, const char * fromName, const char* mess, ...);
};

typedef zUniqueID<DWORD> zUniqueDWORDID;
class SessionChannelManager : public zEntryManager< zEntryTempID, zEntryName >
{
	private:
		//std::map<DWORD, SessionChannel *> channelList;
		zUniqueDWORDID *channelUniqeID;
		bool getUniqeID(DWORD &tempid);
		void putUniqeID(const DWORD &tempid);
		static SessionChannelManager * scm;

		SessionChannelManager();
		~SessionChannelManager();
	public:
		static SessionChannelManager & getMe();
		static void destroyMe();
		bool add(SessionChannel *);
		void remove(DWORD);
		SessionChannel * get(DWORD);
		//void removeUser(DWORD);
		void removeUser(UserSession *);
};

#endif
