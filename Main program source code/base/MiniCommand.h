/**
 * \file
 * \version  $Id: MiniCommand.h  $
 * \author  
 * \date 
 * \brief 定义小游戏服务器相关指令
 *
 */


#ifndef _MINICOMMAND_H_
#define _MINICOMMAND_H_

#include "zNullCmd.h"
#include "Command.h"
#pragma pack(1)

namespace Cmd
{
	namespace Mini
	{
		const BYTE CMD_LOGIN = 1;
		const BYTE CMD_GATE = 2;
		const BYTE CMD_FORWARD = 3;
		const BYTE CMD_SCENE = 4;

		//////////////////////////////////////////////////////////////
		/// 登陆小游戏服务器指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_LOGIN = 1;
		struct t_LoginMini : t_NullCmd
		{
			WORD wdServerID;
			WORD wdServerType;
			t_LoginMini()
				: t_NullCmd(CMD_LOGIN, PARA_LOGIN) {};
		};
		//////////////////////////////////////////////////////////////
		/// 登陆小游戏服务器指令
		//////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////
		/// 场景服务器和Mini交互的指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_SCENE_SET_SCENE = 1;
		struct t_Scene_SetScene : t_NullCmd
		{
			DWORD userID;
			WORD serverID;
			t_Scene_SetScene()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_SET_SCENE) {};
		};

		const BYTE PARA_SCENE_DEPOSIT = 2;
		struct t_Scene_Deposit : t_NullCmd
		{
			DWORD userID;
			DWORD num;
			t_Scene_Deposit()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_DEPOSIT) {};
		};

		const BYTE PARA_SCENE_DEPOSIT_RET = 3;
		struct t_Scene_Deposit_Ret : t_NullCmd
		{
			DWORD userID;
			BYTE ret;
			t_Scene_Deposit_Ret()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_DEPOSIT_RET) {};
		};

		const BYTE PARA_SCENE_CHECK_DRAW = 4;
		struct t_Scene_Check_Draw : t_NullCmd
		{
			DWORD userID;
			DWORD num;
			t_Scene_Check_Draw()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_CHECK_DRAW) {};
		};

		const BYTE PARA_SCENE_DRAW_RET = 5;
		struct t_Scene_Draw_Ret : t_NullCmd
		{
			DWORD userID;
			DWORD num;
			DWORD ret;//0失败 1成功 2玩家不在线
			t_Scene_Draw_Ret()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_DRAW_RET) {};
		};

		//////////////////////////////////////////////////////////////
		/// 场景服务器和Mini交互的指令
		//////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////
		/// 小游戏服务器与网关服务器交互的指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_GATE_USER_LOGIN = 1;
		struct t_UserLogin_Gateway : t_NullCmd
		{
			t_UserLogin_Gateway()
				: t_NullCmd(CMD_GATE, PARA_GATE_USER_LOGIN) {};
			DWORD userID;
			char name[MAX_NAMESIZE];
			WORD countryID;
			WORD face;
			WORD gateServerID;
			WORD sceneServerID;
		};

		const BYTE PARA_GATE_USER_LOGOUT = 2;
		struct t_UserLogout_Gateway : t_NullCmd
		{
			t_UserLogout_Gateway()
				: t_NullCmd(CMD_GATE, PARA_GATE_USER_LOGOUT) {};
			DWORD userID;
		};

		const BYTE PARA_GATE_USER_DELETE = 3;
		struct t_UserDelete_Gateway : t_NullCmd
		{
			t_UserDelete_Gateway()
				: t_NullCmd(CMD_GATE, PARA_GATE_USER_DELETE) {};
			DWORD userID;
		};
		//////////////////////////////////////////////////////////////
		/// 小游戏服务器与网关服务器交互的指令
		//////////////////////////////////////////////////////////////
		
		//////////////////////////////////////////////////////////////
		/// 服务器转发指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_MINI_FORWARD_USER = 1;
		struct t_Mini_ForwardUser : t_NullCmd
		{
			DWORD id;
			WORD  size;
			BYTE  data[0];
			t_Mini_ForwardUser()
				: t_NullCmd(CMD_FORWARD, PARA_MINI_FORWARD_USER) {};
		};

		const BYTE PARA_USER_FORWARD_MINI = 2;
		struct t_Mini_UserForwardMini : t_NullCmd
		{
			DWORD id;
			WORD  size;
			BYTE  data[0];
			t_Mini_UserForwardMini()
				: t_NullCmd(CMD_FORWARD, PARA_USER_FORWARD_MINI) {};
		};

		const BYTE PARA_FORWARD_MINI_TO_SCENE = 3;
		struct t_Mini_ForwardMiniToScene : t_NullCmd
		{
			DWORD id;
			WORD  size;
			BYTE  data[0];
			t_Mini_ForwardMiniToScene()
				: t_NullCmd(CMD_FORWARD, PARA_FORWARD_MINI_TO_SCENE) {};
		};

		const BYTE PARA_FORWARD_SCENE_TO_MINI = 4;
		struct t_Scene_ForwardMini : t_NullCmd
		{
			DWORD id;
			WORD  size;
			BYTE  data[0];
			t_Scene_ForwardMini()
				: t_NullCmd(CMD_FORWARD, PARA_FORWARD_SCENE_TO_MINI) {};
		};
	};

};

#pragma pack()

#endif

