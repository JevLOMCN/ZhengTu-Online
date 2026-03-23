#include <stdarg.h>
#include "SessionChat.h"
#include "SessionManager.h"
#include "SessionTaskManager.h"

SessionChannel::SessionChannel(UserSession * creator):zEntry()
{
	if (!creator)
	{
		Zebra::logger->debug("创建聊天频道失败");
		return;
	}
	strncpy(name,creator->name,MAX_NAMESIZE);
	userList.push_back(creator->tempid);
}

bool SessionChannel::add(UserSession *pUser)
{
	if (!pUser) return false;
	if (!has(pUser->tempid))
	{
		userList.push_back(pUser->tempid);

		Cmd::stJoin_ChannelUserCmd send;
		send.dwChannelID=tempid;
		strncpy(send.name,pUser->name,MAX_NAMESIZE);
		sendCmdToAll(&send,sizeof(send));

		UserSession * user = 0;
		for(std::list<DWORD>::iterator it=userList.begin(); it!=userList.end(); it++)
		{
			user = UserSessionManager::getInstance()->getUserByTempID(*it);
			if (user)
			{
				strncpy(send.name, user->name, MAX_NAMESIZE);
				pUser->sendCmdToMe(&send,sizeof(send));
			}
		}
		return true;
	}
	return false;
}

bool SessionChannel::remove(UserSession *pUser)
{
	if (!pUser) return false;
	if (has(pUser->tempid))
	{
		userList.remove(pUser->tempid);

		Cmd::stLeave_ChannelUserCmd send;
		send.dwChannelID=tempid;
		strncpy(send.name, pUser->name, MAX_NAMESIZE);
		sendCmdToAll(&send,sizeof(send));
	}

	if(userList.size()<1 || 0==strncmp(name, pUser->name, MAX_NAMESIZE))
		return false;
	else
		return true;
}

bool SessionChannel::remove(DWORD id)
{
	UserSession * user = UserSessionManager::getInstance()->getUserByTempID(id);
	return remove(user);
}

bool SessionChannel::removeAllUser()
{
		std::list<DWORD> list = userList;
		for (std::list<DWORD>::iterator it=list.begin(); it!=list.end(); it++)
				remove(*it);
		return true;
}

bool SessionChannel::has(DWORD id)
{
	for (std::list<DWORD>::iterator it=userList.begin(); it!=userList.end(); it++)
	{
		if (*it==id)
			return true;
	}
	return false;
}

DWORD SessionChannel::count()
{
		return userList.size();
}

bool SessionChannel::sendCmdToAll(const void *cmd, int len)
{
	UserSession * user = 0;
	for (std::list<DWORD>::iterator it=userList.begin(); it!=userList.end(); it++)
	{
		user = UserSessionManager::getInstance()->getUserByTempID(*it);
		if (user)
		{
			user->sendCmdToMe(cmd, len);
		}
	}
	return true;
}

bool SessionChannel::sendToOthers(UserSession *pUser, const Cmd::stChannelChatUserCmd *cmd,unsigned int len)
{
	UserSession * user = 0;
	for (std::list<DWORD>::iterator it=userList.begin(); it!=userList.end(); it++)
	{
		user = UserSessionManager::getInstance()->getUserByTempID(*it);
		if (user && user!=pUser)
		{
			user->sendCmdToMe(cmd, len);
		}
	}
	return true;
}

bool SessionChannel::sendCountry(DWORD countryID, const void *cmd,unsigned int len)
{
	if (!cmd) return false;

	SessionTaskManager::getInstance().sendCmdToCountry(countryID,cmd,len);
	return true;
}
/**
 * \brief 处理格式解析的宏
 */

#define getMessage(msg,msglen,pat)      \
	do      \
{       \
	va_list ap;     \
	bzero(msg, msglen);     \
	va_start(ap, pat);              \
	vsnprintf(msg, msglen - 1, pat, ap);    \
	va_end(ap);     \
}while(false)

bool SessionChannel::sendCountryInfo(int type, DWORD countryID, const char* mess, ...)
{
	if (mess == NULL)
	{               
		return false;         
	}                       

	char buf[1024+1];
	bzero(buf, sizeof(buf));
	getMessage(buf, 1024, mess);

	Cmd::stChannelChatUserCmd send;
	send.dwType = Cmd::CHAT_TYPE_SYSTEM;
	send.dwSysInfoType = type;
	bzero(send.pstrName, sizeof(send.pstrName));
	bzero(send.pstrChat, sizeof(send.pstrChat));

	sprintf((char*)send.pstrChat, "%s", buf);

	SessionTaskManager::getInstance().sendCmdToCountry(countryID,&send,sizeof(send));
	return true;
}

bool SessionChannel::sendAllCmd(const void *cmd,const unsigned int len)
{
	if (!cmd) return false;
	return SessionTaskManager::getInstance().sendCmdToWorld(cmd, len);
}

bool SessionChannel::sendAllInfo(int type, const char* mess, ...)
{
	if (mess == NULL)
	{               
		return false;         
	}                       

	char buf[1024+1];
	bzero(buf, sizeof(buf));
	getMessage(buf, 1024, mess);

	Cmd::stChannelChatUserCmd send;
	send.dwType = Cmd::CHAT_TYPE_SYSTEM;
	send.dwSysInfoType = type;
	bzero(send.pstrName, sizeof(send.pstrName));
	bzero(send.pstrChat, sizeof(send.pstrChat));

	sprintf((char*)send.pstrChat, "%s", buf);

	SessionTaskManager::getInstance().sendCmdToWorld(&send,sizeof(send));
	return true;

}

bool SessionChannel::sendPrivate(UserSession * pUser, const char * fromName, const char* mess, ...)
{
	if (!mess || !pUser || !fromName) return false;         

	char buf[MAX_CHATINFO];
	bzero(buf, sizeof(buf));
	getMessage(buf, MAX_CHATINFO, mess);

	Cmd::stChannelChatUserCmd send;
	send.dwType = Cmd::CHAT_TYPE_PRIVATE;
	bzero(send.pstrName, sizeof(send.pstrName));
	strncpy(send.pstrName, fromName, MAX_NAMESIZE);

	bzero(send.pstrChat, sizeof(send.pstrChat));
	strncpy(send.pstrChat, buf, MAX_CHATINFO);

	pUser->sendCmdToMe(&send, sizeof(send));

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
SessionChannelManager * SessionChannelManager::scm(0);

SessionChannelManager::SessionChannelManager()
{
	channelUniqeID =new zUniqueDWORDID(1000);
}
SessionChannelManager::~SessionChannelManager()
{
	SAFE_DELETE(channelUniqeID);
}

SessionChannelManager & SessionChannelManager::getMe()
{
	if (!scm)
		scm = new SessionChannelManager();
	return * scm;
}

void SessionChannelManager::destroyMe()
{
	SAFE_DELETE(scm);
}

bool SessionChannelManager::add(SessionChannel *ch)
{
	bool ret = false;
	ret = addEntry(ch);
	return ret;
	/*
	for (std::map<DWORD, SessionChannel*>::iterator it=channelList.begin(); it!=channelList.end();it++)
	{
		if (!strncmp(ch->name, it->second->name, MAX_NAMESIZE))
			return false;
	}
	if (channelList.end()==channelList.find(ch->tempid))
	{
		channelList[ch->tempid] = ch;
		return true;
	}
	return false;
	*/
}

void SessionChannelManager::remove(DWORD id)
{
	SessionChannel *ret=NULL;
	ret=(SessionChannel *)getEntryByTempID(id);
	ret->removeAllUser();
	removeEntry(ret);
	SAFE_DELETE(ret);
	/*
	if (channelList.end()!=channelList.find(id))
	{
		SAFE_DELETE(hannelList[id]);
		channelList.erase(id);
	}
	*/
}

SessionChannel * SessionChannelManager::get(DWORD id)
{
	SessionChannel *ret=NULL;
	ret=(SessionChannel *)getEntryByTempID(id);
	return ret;
	/*
	if (channelList.end()!=channelList.find(id))
	{
		return channelList[id];
	}
	return 0;
	*/
}

/*
void SessionChannelManager::removeUser(DWORD id)
{
	std::vector<DWORD> rmList;
	for(zEntryTempID::hashmap::iterator it=zEntryTempID::ets.begin();it!=zEntryTempID::ets.end();it++)
	{
		SessionChannel *temp=(SessionChannel *)it->second;
		if (!temp->remove(id) || temp->tempid==id)//创建人退出删除频道
				rmList.push_back(temp->tempid);
	}

	for (std::vector<DWORD>::iterator it=rmList.begin(); it!=rmList.end(); it++)
			remove(*it);
}
*/

void SessionChannelManager::removeUser(UserSession * user)
{
	if (!user) return;

	std::vector<DWORD> rmList;
	for(zEntryTempID::hashmap::iterator it=zEntryTempID::ets.begin();it!=zEntryTempID::ets.end();it++)
	{
		SessionChannel *temp=(SessionChannel *)it->second;
		if (!temp->remove(user) || 0==strncmp(temp->name, user->name, MAX_NAMESIZE))//创建人退出删除频道
				rmList.push_back(temp->tempid);
				//remove(temp->tempid);
	}

	for (std::vector<DWORD>::iterator it=rmList.begin(); it!=rmList.end(); it++)
			remove(*it);
}

bool SessionChannelManager::getUniqeID(DWORD &tempid)
{
	tempid=channelUniqeID->get();
	return (tempid!=channelUniqeID->invalid());
}

void SessionChannelManager::putUniqeID(const DWORD &tempid)
{       
	channelUniqeID->put(tempid);
}  
