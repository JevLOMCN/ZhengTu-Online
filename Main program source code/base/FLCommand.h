/**
 * \file
 * \version  $Id: FLCommand.h  $
 * \author  
 * \date 
 * \brief 定义统一用户平台登陆服务器指令
 *
 * 
 */

#ifndef _FLCommand_h_
#define _FLCommand_h_

#include "zType.h"
#include "zNullCmd.h"

#pragma pack(1)

namespace Cmd
{
	namespace FL
	{
		const BYTE CMD_LOGIN = 1;
		const BYTE CMD_GYLIST = 2;
		const BYTE CMD_SESSION = 3;


		//////////////////////////////////////////////////////////////
		/// 登陆FL服务器指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_LOGIN = 1;
		struct t_LoginFL : t_NullCmd
		{
			char strIP[MAX_IP_LENGTH];
			unsigned short port;

			t_LoginFL()
				: t_NullCmd(CMD_LOGIN, PARA_LOGIN) {};
		};

		const BYTE PARA_LOGIN_OK = 2;
		struct t_LoginFL_OK : t_NullCmd
		{
			GameZone_t gameZone;
			char name[MAX_NAMESIZE];
			BYTE netType;

			t_LoginFL_OK()
				: t_NullCmd(CMD_LOGIN, PARA_LOGIN_OK) {};
		};
		//////////////////////////////////////////////////////////////
		/// 登陆FL服务器指令
		//////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////
		/// 定义网关信息相关指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_FL_GYLIST = 1;
		struct t_GYList_FL : t_NullCmd
		{
			WORD wdServerID;			/**< 服务器编号 */
			BYTE pstrIP[MAX_IP_LENGTH];	/**< 服务器地址 */
			WORD wdPort;				/**< 服务器端口 */
			WORD wdNumOnline;			/**< 网关在线人数 */
			int  state;					/**< 服务器状态 */
			DWORD zoneGameVersion;
			t_GYList_FL()
				: t_NullCmd(CMD_GYLIST, PARA_FL_GYLIST) {};
		};

		const BYTE PARA_FL_RQGYLIST = 2;
		struct t_RQGYList_FL : t_NullCmd
		{
			t_RQGYList_FL()
				: t_NullCmd(CMD_GYLIST, PARA_FL_RQGYLIST) {};
		};
		//////////////////////////////////////////////////////////////
		/// 定义网关信息相关指令
		//////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////
		/// 定义与登陆session相关指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_SESSION_NEWSESSION = 1;
		struct t_NewSession_Session : t_NullCmd
		{
			t_NewLoginSession session;

			t_NewSession_Session()
				: t_NullCmd(CMD_SESSION, PARA_SESSION_NEWSESSION) {};
		};

		const BYTE PARA_SESSION_IDINUSE = 2;
		struct t_idinuse_Session : t_NullCmd
		{
			DWORD accid;
			DWORD loginTempID;
			WORD wdLoginID;
			char name[48];

			t_idinuse_Session()
				: t_NullCmd(CMD_SESSION, PARA_SESSION_IDINUSE) { bzero(name, sizeof(name)); };
		};
		//////////////////////////////////////////////////////////////
		/// 定义与登陆session相关指令
		//////////////////////////////////////////////////////////////
	};
};

#pragma pack()

#endif

