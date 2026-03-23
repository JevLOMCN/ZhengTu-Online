/**
 * \file
 * \version  $Id: OfflineMessage.h  $
 * \author  
 * \date 
 * \brief 离线消息管理类
 *
 */

#ifndef __OFFLINE_MESSAGE__
#define __OFFLINE_MESSAGE__

#include <string>
#include "zType.h"
#include "Command.h"


class UserSession;


/**
 * \brief 离线消息管理器
 *
 * 提供了对离线消息的管理
 *
 */
class COfflineMessage
{
private:
	static std::string rootpath;
public:

	static bool init();

	static void writeOfflineMessage(const BYTE &type, const DWORD &id, const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen);

	static void getOfflineMessage(const UserSession *pUser);

	static void getOfflineMessageSetAndSend(const UserSession *pUser, std::string path);
};

#endif

