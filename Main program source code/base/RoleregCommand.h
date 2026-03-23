/**
 * \file
 * \version  $Id: RoleregCommand.h  $
 * \author  
 * \date 
 * \brief 定义角色创建指令
 *
 */
 
 #ifndef _RoleregCommand_h_
 #define _RoleregCommand_h_
 
 #include "zType.h"
 #include "zNullCmd.h"
 
 #pragma pack(1)
 
 namespace Cmd
 {
	namespace	RoleReg
	{
		const BYTE CMD_LOGIN = 1; 
		const BYTE CMD_ROLEREG = 2;

		//////////////////////////////////////////////////////////////
		/// 登陆服务器指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_LOGIN = 1;
		struct t_LoginRoleReg : t_NullCmd   //登入服务器命令
		{
			char strIP[MAX_IP_LENGTH];
			unsigned short port;

			t_LoginRoleReg()
				:t_NullCmd(CMD_LOGIN, PARA_LOGIN) { }
		};
		const BYTE PARA_LOGIN_OK = 2;
		struct t_LoginRoleReg_OK : t_NullCmd
		{
			GameZone_t gameZone;
			char name[MAX_NAMESIZE];
			BYTE netType;

			t_LoginRoleReg_OK()
				: t_NullCmd(CMD_LOGIN, PARA_LOGIN_OK) {};
		};
		//////////////////////////////////////////////////////////////
		/// 登陆服务器指令
		//////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////
		/// 角色名称唯一性验证相关指令
		//////////////////////////////////////////////////////////////
		const WORD ROLEREG_STATE_TEST		= 1;	//测试
		const WORD ROLEREG_STATE_WRITE		= 2;	//回写
		const WORD ROLEREG_STATE_CLEAN		= 4;	//清除
		const WORD ROLEREG_STATE_HAS		= 8;	//测试有
		const WORD ROLEREG_STATE_OK			= 16;	//清除或回写成功
		const BYTE PARA_CHARNAME_ROLEREG = 1;
		struct t_Charname_Rolereg : t_NullCmd
		{
			WORD wdServerID;			/**< 服务器编号 */
			DWORD accid;				/**< 账号编号 */
			GameZone_t gameZone;		/**< 游戏区编号 */
			char name[MAX_NAMESIZE];	/**< 角色名称 */
			WORD state;					/**< 上面各种状态的位组合 */

			t_Charname_Rolereg()
				:t_NullCmd(CMD_ROLEREG, PARA_CHARNAME_ROLEREG) { }
		};
		//////////////////////////////////////////////////////////////
		/// 角色名称唯一性验证相关指令
		//////////////////////////////////////////////////////////////
	};
};

#pragma pack()

#endif
