/**
 * \file
 * \version  $Id: InfoCommand.h  $
 * \author  
 * \date 
 * \brief 定义服务器信息采集指令
 */

#ifndef _InfoCommand_h
#define _InfoCommand_h

#include "zType.h"
#include "zNullCmd.h"
#include "Command.h"

#pragma pack(1)
namespace Cmd
{
	namespace Info
	{
		const BYTE CMD_LOGIN = 1;
		const BYTE CMD_INFO = 2;
		const BYTE CMD_GMTOOL = 3;

		///////////////////////////////////////
		///登入服务器指令
		//////////////////////////////////////
		const BYTE PARA_LOGIN = 1;
		struct t_LoginCmd : t_NullCmd
		{
			char strIP[MAX_IP_LENGTH];
			unsigned short port;

			t_LoginCmd() : t_NullCmd(CMD_LOGIN, PARA_LOGIN) { }
		};
		const BYTE PARA_LOGIN_OK = 2;
		struct t_LoginCmd_OK : t_NullCmd
		{
			GameZone_t gameZone;
			char name[MAX_NAMESIZE];
			BYTE netType;

			t_LoginCmd_OK()
				: t_NullCmd(CMD_LOGIN, PARA_LOGIN_OK) {};
		};
		///////////////////////////////////////
		///登入服务器指令
		///////////////////////////////////////

		//请求在线人数信息
		const BYTE PARA_REQUEST_ONLINENUM = 3;
		struct t_Request_OnlineNum : t_NullCmd
		{
			QWORD			rTimestamp;
			
			t_Request_OnlineNum() : t_NullCmd(CMD_INFO, PARA_REQUEST_ONLINENUM) { }
		};
		//区的总人数
		const BYTE PARA_ONLINENUM =4;
		struct t_OnlineNum : t_NullCmd
		{
			QWORD			rTimestamp;
			WORD			ServerID;
			WORD			ServerType;
			GameZone_t		GameZone;
			char			ZoneName[MAX_NAMESIZE];
			DWORD			OnlineNum;
			
			t_OnlineNum() : t_NullCmd(CMD_INFO, PARA_ONLINENUM) { }	
		};
		//区中每个国家的人数
		const BYTE PARA_COUNTRY_ONLINENUM = 5;
		struct t_Country_OnlineNum : t_NullCmd
		{
			QWORD			rTimestamp;
			GameZone_t		GameZone;
			DWORD			OnlineNum;
			struct Online
			{
				DWORD country;
				DWORD num;
			}
			CountryOnline[0];

			t_Country_OnlineNum() : t_NullCmd(CMD_INFO, PARA_COUNTRY_ONLINENUM)
			{
				OnlineNum = 0;
			}
		};
	};
};
#pragma pack()

#endif

