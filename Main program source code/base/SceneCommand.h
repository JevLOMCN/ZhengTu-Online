/**
 * \file
 * \version  $Id: SceneCommand.h  $
 * \author  
 * \date 
 * \brief 定义场景服务器相关指令
 *
 */


#ifndef _SceneCommand_h_
#define _SceneCommand_h_

#include "zNullCmd.h"

#pragma pack(1)

namespace Cmd
{

	namespace Scene
	{
		const BYTE CMD_LOGIN = 1;
		const BYTE CMD_FORWARD = 2;
		const BYTE CMD_SCENE = 3;
		const BYTE CMD_SCENE_GATE_BILL = 4;
		const BYTE CMD_STOCK = 5;


		//////////////////////////////////////////////////////////////
		/// 登陆场景服务器指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_LOGIN = 1;
		struct t_LoginScene : t_NullCmd
		{
			WORD wdServerID;
			WORD wdServerType;
			t_LoginScene()
				: t_NullCmd(CMD_LOGIN, PARA_LOGIN) {};
		};

		const BYTE PARA_LOGIN_REFRESH = 2;
		struct t_Refresh_LoginScene : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwSceneTempID;
			t_Refresh_LoginScene()
				: t_NullCmd(CMD_LOGIN, PARA_LOGIN_REFRESH) {};
		};

		const BYTE PARA_LOGIN_UNREG = 3;
		const BYTE UNREGUSER_RET_LOGOUT=0;
		const BYTE UNREGUSER_RET_ERROR=1;
		const BYTE UNREGUSER_RET_CHANGE_SCENE=2;
		const BYTE UNREGUSER_RET_KICKOUT=3;
		const BYTE UNREGUSER_RET_UNLOAD_SCENE=4;
		struct t_Unreg_LoginScene : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwSceneTempID;
			BYTE retcode;
			BYTE map[MAX_NAMESIZE];
			t_Unreg_LoginScene()
				: t_NullCmd(CMD_LOGIN, PARA_LOGIN_UNREG) {};
		};
		const BYTE PARA_LOGIN_UNREG_OK = 4;
		struct t_Unreg_LoginScene_Ok : t_NullCmd
		{
			DWORD type;						/// 卸载类型
			DWORD accid;
			DWORD id;						/// 角色
			t_Unreg_LoginScene_Ok()
				: t_NullCmd(CMD_LOGIN, PARA_LOGIN_UNREG_OK) {};
		};
		enum ChangeCountryOper {
			ENABLE_REGISTER,
			DISABLE_REGISTER,
			ENABLE_LOGIN,
			DISABLE_LOGIN,
			LIST_COUNTRY_INFO
		};
		const BYTE PARA_CHANGE_COUNTRY_STATE = 5;
		struct t_ChangeCountryStatus : t_NullCmd
		{
			BYTE  oper;							/// 操作类型
			WORD  country;						/// 卸载类型
			DWORD dwUserID;						/// 操作者的角色ID
			t_ChangeCountryStatus()
				: t_NullCmd(CMD_LOGIN, PARA_CHANGE_COUNTRY_STATE) {};
		};

		enum ChangeVerifyVersionOper {
			CHANGECODE,
			SHOWCODE
		};
		const BYTE PARA_CHANGE_VERIFY_VERSION = 6;
		struct t_ChangeVerifyVersion : t_NullCmd
		{
			BYTE  oper;							/// 操作类型
			DWORD versionCode;					/// 版本校验码
			DWORD dwUserID;						/// 操作者的角色ID
			t_ChangeVerifyVersion()
				: t_NullCmd(CMD_LOGIN, PARA_CHANGE_VERIFY_VERSION) {};
		};
		//////////////////////////////////////////////////////////////
		/// 登陆场景服务器指令
		//////////////////////////////////////////////////////////////
		
		//////////////////////////////////////////////////////////////
		/// 场景服务器指令
		//////////////////////////////////////////////////////////////
		
		//////////////////////////////////////////////////////////////
		/// 场景服务器指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_QUERY_ACCOUNT = 1;
		struct t_Query_AccountScene : t_NullCmd
		{
			char  dwDestID;      // 被查询者ID
			DWORD dwUserID;                // 查询者ID
			t_Query_AccountScene()
				: t_NullCmd(CMD_SCENE, PARA_QUERY_ACCOUNT) {};
		};

		const BYTE PARA_COUNTRY_AND_SCENE = 2;
		struct t_countryAndScene_GateScene : t_NullCmd
		{
			DWORD userID;//玩家ID
			DWORD countryID;      //国家ID
			DWORD sceneID;                // 场景ID
			t_countryAndScene_GateScene()
				: t_NullCmd(CMD_SCENE, PARA_COUNTRY_AND_SCENE) {};
		};
		/// 用户维护网关屏索引指令
		const BYTE PARA_FRESH_SCREENINDEX = 3;
		struct t_fresh_ScreenIndex : t_NullCmd
		{
			DWORD dwMapTempID;
			DWORD dwScreen;
			DWORD dwUserTempID;                // userID
			t_fresh_ScreenIndex()
				: t_NullCmd(CMD_SCENE, PARA_FRESH_SCREENINDEX) 
				{
					dwMapTempID = 0;
					dwScreen = 0;
					dwUserTempID = 0;
				};
		};
		struct MapIndexXY
		{
			DWORD maptempid;
			DWORD mapx;	// 宽多少屏
			DWORD mapy; // 高多少屏
		};
		/// 添加网关地图索引
		const BYTE PARA_FRESH_MAPINDEX = 4;
		struct t_fresh_MapIndex : t_NullCmd
		{
			DWORD dwSize;
			MapIndexXY mps[0]; 
			t_fresh_MapIndex()
				: t_NullCmd(CMD_SCENE, PARA_FRESH_MAPINDEX) 
				{
				};
		};
		/// 删除网关地图索引
		const BYTE PARA_REMOVE_MAPINDEX = 5;
		struct t_Remove_MapIndex : t_NullCmd
		{
			DWORD dwSize;
			DWORD dwMapTempID[0]; 
			t_Remove_MapIndex()
				: t_NullCmd(CMD_SCENE, PARA_REMOVE_MAPINDEX) 
				{
				};
		};

		///踢掉网关上的玩家
		const BYTE PARA_KICK_USER = 6;
		struct t_kickUser_GateScene : t_NullCmd
		{
			DWORD userID;
			DWORD accid;
			t_kickUser_GateScene()
				: t_NullCmd(CMD_SCENE, PARA_KICK_USER)
				{
					userID = 0;
					accid = 0;
				}
		};
		const BYTE PARA_SCENE_SYS_SETTING = 7;  //系统设置
		struct t_sysSetting_GateScene : t_NullCmd
		{
			BYTE name[MAX_NAMESIZE];
			DWORD id;
			BYTE sysSetting[20];
			DWORD face;
			t_sysSetting_GateScene()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_SYS_SETTING) {};
		};


		///服务系统控制指令
		enum
		{
			SERVICE_STOP,//停止服务
			SERVICE_START,//启动服务
		};
		//金币系统控制
		const BYTE PARA_SERVICE_GOLD = 8;
		struct t_ServiceGold_GateScene : t_NullCmd
		{
			t_ServiceGold_GateScene()
				: t_NullCmd(CMD_SCENE, PARA_SERVICE_GOLD)
				{
				}
			BYTE byType;
		};
		//股票系统控制指令
		const BYTE PARA_SERVICE_STOCK = 9;
		struct t_ServiceStock_GateScene : t_NullCmd
		{
			t_ServiceStock_GateScene()
				: t_NullCmd(CMD_SCENE, PARA_SERVICE_STOCK)
				{
				}
			BYTE byType;
		};
		//调整网关sleep时间
		const BYTE PARA_USLEEP_TIME = 10;
		struct t_Usleep_GateScene : t_NullCmd
		{
			t_Usleep_GateScene()
				: t_NullCmd(CMD_SCENE, PARA_USLEEP_TIME)
				{
					utask=0;
					uclient=0;
				}
			DWORD utask;
			DWORD uclient;
		};
		//////////////////////////////////////////////////////////////
		/// 场景服务器转发指令
		//////////////////////////////////////////////////////////////
		// 网关到9屏用户
		const BYTE PARA_FORWARD_NINE = 1;
		struct t_Nine_ForwardScene : t_NullCmd
		{
			DWORD maptempid;
			DWORD screen;
			DWORD  size;
			BYTE  data[0];
			t_Nine_ForwardScene()
				: t_NullCmd(CMD_FORWARD, PARA_FORWARD_NINE) {};
		};
		
		// 网关到正向5屏或者3屏用户
		const BYTE PARA_FORWARD_NINE_DIR = 2;
		struct t_Nine_dir_ForwardScene : t_NullCmd
		{
			DWORD maptempid;
			DWORD screen;
			WORD dir;
			DWORD  size;
			BYTE  data[0];
			t_Nine_dir_ForwardScene()
				: t_NullCmd(CMD_FORWARD, PARA_FORWARD_NINE_DIR) {};
		};
		
		// 网关到反向5屏或者3屏用户
		const BYTE PARA_FORWARD_NINE_RDIR = 3;
		struct t_Nine_rdir_ForwardScene : t_NullCmd
		{
			DWORD maptempid;
			DWORD screen;
			WORD dir;
			DWORD  size;
			BYTE  data[0];
			t_Nine_rdir_ForwardScene()
				: t_NullCmd(CMD_FORWARD, PARA_FORWARD_NINE_RDIR) {};
		};
		
		//网关到用户
		const BYTE PARA_FORWARD_USER = 4;
		struct t_User_ForwardScene : t_NullCmd
		{
			DWORD dwID;
			DWORD  size;
			BYTE  data[0];
			t_User_ForwardScene()
				: t_NullCmd(CMD_FORWARD, PARA_FORWARD_USER) {};
			//void init() { cmd=CMD_FORWARD;para=PARA_FORWARD_USER;};
		};

		const BYTE PARA_FORWARD_SCENE = 5;
		struct t_Scene_ForwardScene : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwSceneTempID;
			DWORD  size;
			BYTE  data[0];
			t_Scene_ForwardScene()
				: t_NullCmd(CMD_FORWARD, PARA_FORWARD_SCENE) {};
			//void init() { cmd=CMD_FORWARD;para=PARA_FORWARD_SCENE;};
		};

		//网关到用户
		const BYTE PARA_FORWARD_MAP = 6;
		struct t_User_ForwardMap : t_NullCmd
		{
			DWORD maptempid;
			DWORD  size;
			BYTE  data[0];
			t_User_ForwardMap()
				: t_NullCmd(CMD_FORWARD, PARA_FORWARD_MAP) {};
			//void init() { cmd=CMD_FORWARD;para=PARA_FORWARD_USER;};
		};
		const BYTE PARA_BILL_FORWARD_SCENE = 7;
		struct t_Bill_ForwardScene : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwSceneTempID;
			DWORD  size;
			BYTE  data[0];
			t_Bill_ForwardScene()
				: t_NullCmd(CMD_FORWARD, PARA_BILL_FORWARD_SCENE) {};
			//void init() { cmd=CMD_FORWARD;para=PARA_FORWARD_SCENE;};
		};

		///场景模拟用户发送消息到Bill
		const BYTE PARA_FORWARD_SCENE_TO_BILL = 8;
		struct t_Scene_ForwardSceneToBill : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwSceneTempID;
			DWORD  size;
			BYTE  data[0];
			t_Scene_ForwardSceneToBill()
				: t_NullCmd(CMD_FORWARD, PARA_FORWARD_SCENE_TO_BILL) {};
			//void init() { cmd=CMD_FORWARD;para=PARA_FORWARD_SCENE;};
		};
		/// 发送场景用户的消息到Bill
		const BYTE PARA_FORWARD_SCENEUSER_TO_BILL = 9;
		struct t_Scene_ForwardSceneUserToBill : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwSceneTempID;
			DWORD  size;
			BYTE  data[0];
			t_Scene_ForwardSceneUserToBill()
				: t_NullCmd(CMD_FORWARD, PARA_FORWARD_SCENEUSER_TO_BILL) {};
			//void init() { cmd=CMD_FORWARD;para=PARA_FORWARD_SCENE;};
		};
		///发送到除XX之外的屏用户
		const BYTE PARA_FORWARD_NINE_EXCEPTME = 10;
		struct t_Nine_ExceptMe_ForwardScene : t_NullCmd
		{
			DWORD maptempid;
			DWORD screen;
			DWORD exceptme_id;
			DWORD  size;
			BYTE  data[0];
			t_Nine_ExceptMe_ForwardScene()
				: t_NullCmd(CMD_FORWARD, PARA_FORWARD_NINE_EXCEPTME) {};
		};
		//////////////////////////////////////////////////////////////
		/// 场景服务器转发指令
		//////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////
		/// 场景服务器转发到Bill的指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_REQUEST_BILL = 1;
		struct t_Request_Bill : t_NullCmd
		{
			DWORD dwUserID;
			t_Request_Bill()
				: t_NullCmd(CMD_SCENE_GATE_BILL, PARA_REQUEST_BILL) {};
		};
		const BYTE PARA_REQUEST_POINT = 2;
		struct t_Request_Point : t_NullCmd
		{
			DWORD dwUserID;
			t_Request_Point()
				: t_NullCmd(CMD_SCENE_GATE_BILL, PARA_REQUEST_POINT) {};
		};
		const BYTE PARA_REQUEST_REDEEM_GOLD = 3;
		struct t_Request_RedeemGold : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwNum;	//需要兑换的点数
			t_Request_RedeemGold()
				: t_NullCmd(CMD_SCENE_GATE_BILL, PARA_REQUEST_REDEEM_GOLD) {};
		};
		const BYTE PARA_REQUEST_REDEEM_CARD = 4;
		struct t_Request_RedeemCard : t_NullCmd
		{
			DWORD dwUserID;
			t_Request_RedeemCard()
				: t_NullCmd(CMD_SCENE_GATE_BILL, PARA_REQUEST_REDEEM_CARD) {};
		};


		/*
		///股票套现
		const BYTE PARA_STOCK_FETCH = 5;
		struct t_Stock_Fetch : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwGold;
			DWORD dwMoney;
			t_Stock_Fetch()
				: t_NullCmd(CMD_SOTCK, PARA_STOCK_FETCH) {};
		};
		///股票冲值
		const BYTE PARA_STOCK_SAVE = 6;
		struct t_Stock_Save : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwGold;
			DWORD dwMoney;
			t_Stock_Save()
				: t_NullCmd(CMD_SOTCK, PARA_STOCK_FETCH) {};
		};
		// */
		//////////////////////////////////////////////////////////////
		/// 场景服务器转发到Bill的指令
		//////////////////////////////////////////////////////////////
	};

};

#pragma pack()

#endif
