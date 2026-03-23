/**
 * \file
 * \version  $Id: RecordCommand.h  $
 * \author  
 * \date 
 * \brief 定义档案服务器相关指令
 *
 */


#ifndef _RecordCommand_h_
#define _RecordCommand_h_

#include "zNullCmd.h"
#include "Command.h"
#include "CharBase.h"

#pragma pack(1)

namespace Cmd
{

	namespace Record
	{
		const BYTE CMD_LOGIN = 1;
		const BYTE CMD_GATE = 2;
		const BYTE CMD_SCENE = 3;
		const BYTE CMD_SESSION = 4;

		//////////////////////////////////////////////////////////////
		/// 登陆档案服务器指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_LOGIN = 1;
		struct t_LoginRecord : t_NullCmd
		{
			WORD wdServerID;
			WORD wdServerType;
			t_LoginRecord()
				: t_NullCmd(CMD_LOGIN, PARA_LOGIN) {};
		};
		//////////////////////////////////////////////////////////////
		/// 登陆档案服务器指令
		//////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////
		/// 档案服务器和网关交互的指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_GATE_GET_SELECTINFO = 1;
		struct t_Get_SelectInfo_GateRecord : t_NullCmd
		{
			DWORD accid;					/// 账号
			t_Get_SelectInfo_GateRecord()
				: t_NullCmd(CMD_GATE, PARA_GATE_GET_SELECTINFO) {};
		};

		const BYTE PARA_GATE_RET_SELECTINFO = 2;
		struct t_Ret_SelectInfo_GateRecord : t_NullCmd
		{
			DWORD accid;						/// 账号
			WORD num;							/// 有效角色信息计数
			SelectUserInfo info[MAX_CHARINFO];	/// 角色信息列表
			t_Ret_SelectInfo_GateRecord()
				: t_NullCmd(CMD_GATE, PARA_GATE_RET_SELECTINFO)
				{
					num = 0;
					bzero(info, sizeof(info));
				}
		};

		const BYTE PARA_GATE_CREATECHAR = 3;
		struct t_CreateChar_GateRecord : t_NullCmd
		{
			DWORD accid;						/// 账号
			char  name[MAX_NAMESIZE+1];		/// 角色名称
			WORD  type;						/// 角色类型
			DWORD hair;						/// 角色发型以及颜色
			WORD country;				/// 国家ID
			WORD five;					/// 五行主属性
			char mapName[MAX_NAMESIZE];	/**< 用户地图名字  */
			DWORD createip;				/// 创建角色时的ip
			t_CreateChar_GateRecord()
				: t_NullCmd(CMD_GATE, PARA_GATE_CREATECHAR) {};
		};

		const BYTE PARA_GATE_CREATECHAR_RETURN = 4;
		struct t_CreateChar_Return_GateRecord : t_NullCmd
		{
			DWORD accid;						/// 账号
			BYTE retcode;						/// 返回代码，0表示失败，角色名称重复，1表示创建角色成功
			SelectUserInfo charinfo;			/// 角色信息
			DWORD dwFace;					// 角色脸型
			t_CreateChar_Return_GateRecord()
				: t_NullCmd(CMD_GATE, PARA_GATE_CREATECHAR_RETURN) {};
		};

		const BYTE PARA_GATE_DELCHAR = 5;
		struct t_DelChar_GateRecord : t_NullCmd
		{
			DWORD accid;						/// 账号
			DWORD id;							/// 角色编号
			char name[MAX_NAMESIZE+1];
			t_DelChar_GateRecord()
				: t_NullCmd(CMD_GATE, PARA_GATE_DELCHAR) {};
		};

		const BYTE PARA_GATE_DELCHAR_RETURN = 6;
		struct t_DelChar_Return_GateRecord : t_NullCmd
		{
			DWORD accid;						/// 账号
			DWORD id;							/// 角色编号
			BYTE retcode;						/// 返回代码，0表示删除角色失败，1表示删除角色成功
			char name[MAX_NAMESIZE+1];
			t_DelChar_Return_GateRecord()
				: t_NullCmd(CMD_GATE, PARA_GATE_DELCHAR_RETURN) {};
		};

		const BYTE PARA_GATE_CHECKNAME = 7;
		struct t_CheckName_GateRecord : t_NullCmd
		{
			DWORD accid;						/// 账号
			char name[MAX_NAMESIZE+1];
			t_CheckName_GateRecord()
				: t_NullCmd(CMD_GATE, PARA_GATE_CHECKNAME) {};
		};

		const BYTE PARA_GATE_CHECKNAME_RETURN = 8;
		struct t_CheckName_Return_GateRecord : t_NullCmd
		{
			DWORD accid;						/// 账号
			BYTE err_code;						/// 返回代码 0: 可以使用 1:角色名重复
			char name[MAX_NAMESIZE+1];
			t_CheckName_Return_GateRecord()
				: t_NullCmd(CMD_GATE, PARA_GATE_CHECKNAME_RETURN) {};
		};
		/// 请求国家档案人数排序
		const BYTE REQUEST_GATE_COUNTRY_ORDER = 9;
		struct t_request_Country_GateRecord : t_NullCmd
		{
			t_request_Country_GateRecord()
				: t_NullCmd(CMD_GATE, REQUEST_GATE_COUNTRY_ORDER) {};
		};
		struct CountrOrder
		{
			DWORD size;					//数量
			struct {
			DWORD country;				//国家	
			DWORD count;		//已经排序好的国家id
			} order[0];
		};
		/// 国家档案人数排序
		const BYTE PARA_GATE_COUNTRY_ORDER = 10;
		struct t_order_Country_GateRecord : t_NullCmd
		{
			DWORD dwID;					//用户id
			CountrOrder order;			//排序的国家 
			t_order_Country_GateRecord()
				: t_NullCmd(CMD_GATE, PARA_GATE_COUNTRY_ORDER) {};
		};
		//////////////////////////////////////////////////////////////
		/// 档案服务器和网关交互的指令
		//////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////
		/// 档案服务器和场景交互的指令
		//////////////////////////////////////////////////////////////
#define PARA_SCENE_USER_READ_ERROR -1
		const BYTE PARA_SCENE_USER_READ = 1;
		struct t_ReadUser_SceneRecord : t_NullCmd
		{
			DWORD accid;
			DWORD id;						/// 角色
			DWORD dwMapTempID;				/// 地图临时ID
			t_ReadUser_SceneRecord()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_USER_READ) {};
		};

		enum WriteBack_Type
		{
			TIMETICK_WRITEBACK,		/// 定时回写档案
			LOGOUT_WRITEBACK,		/// 退出回写档案
			CHANGE_SCENE_WRITEBACK, //切换场景时回写档案
			OPERATION_WRITEBACK,		/// 操作回写档案
		};
		const BYTE PARA_SCENE_USER_WRITE = 2;
		struct t_WriteUser_SceneRecord : t_NullCmd
		{
			DWORD accid;
			DWORD id;							/// 角色
			DWORD dwMapTempID;					/// 地图临时ID
			DWORD writeback_type;				/// 回写档案类型
			CharBase    charbase;				///存档的基本信息
			DWORD      dataSize;				///存档二进制的大小
			char        data[0];				///存档的二进制数据
			t_WriteUser_SceneRecord()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_USER_WRITE)
				{
					bzero(&charbase , sizeof(charbase));
					dataSize = 0;
				}
		};

		const BYTE PARA_SCENE_USERINFO = 3;
		struct t_UserInfo_SceneRecord : t_NullCmd
		{
			DWORD id;							/// 角色
			DWORD dwMapTempID;					/// 地图临时ID
			CharBase    charbase;				///存档的基本信息
			DWORD      dataSize;				///存档二进制的大小
			char        data[0];				///存档的二进制数据
			t_UserInfo_SceneRecord()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_USERINFO)
				{
					bzero(&charbase , sizeof(charbase));
					dataSize = 0;
				}
		};

		//删除档案服务器用户数据,如果新用户注册后档案服务器未返回而收到退出指令时需要使用
		const BYTE PARA_SCENE_USER_REMOVE = 4;
		struct t_RemoveUser_SceneRecord : t_NullCmd
		{
			DWORD accid;
			DWORD id;
			t_RemoveUser_SceneRecord()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_USER_REMOVE)
				{
				}
		};

		//检查用户是否存在
		const BYTE PARA_SCENE_USER_EXIST = 5;
		struct t_userExist_SceneRecord : t_NullCmd
		{
			DWORD fromID;//临时ID
			DWORD toID;
			Cmd::stSendMail sm;
			t_userExist_SceneRecord()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_USER_EXIST)
				{
					fromID = 0;
					toID = 0;
					bzero(&sm, sizeof(sm));
				}
		};
		const BYTE PARA_SCENE_USER_WRITE_OK = 6;
		struct t_WriteUser_SceneRecord_Ok : t_NullCmd
		{
			DWORD type;						/// 卸载类型
			DWORD accid;
			DWORD id;						/// 角色
			t_WriteUser_SceneRecord_Ok()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_USER_WRITE_OK) {};
		};

#ifdef _TEST_DATA_LOG
		enum enumWriteBackTest_Type
		{
			NEWCHAR_WRITEBACK,//新角色插入
			LEVELUP_WRITEBACK,//升级
			DEATH_WRITEBACK,//死亡
			HP_WRITEBACK,//HP
			MP_WRITEBACK,//MP
			SP_WRITEBACK,//SP
		};
		const BYTE PARA_SCENE_INSERT_CHARTEST = 4;
		struct t_Insert_CharTest_SceneRecord : t_NullCmd
		{
			char name[MAX_NAMESIZE];				/// 角色名称
			DWORD level;							/// 角色等级
			CharTest    chartest;				///存档数据
			t_Insert_CharTest_SceneRecord()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_INSERT_CHARTEST)
				{
					bzero(&chartest , sizeof(chartest));
				}
		};
		const BYTE PARA_SCENE_UPDATE_CHARTEST = 5;
		struct t_Update_CharTest_SceneRecord : t_NullCmd
		{
			char name[MAX_NAMESIZE];				/// 角色名称
			DWORD level;							/// 角色等级
			CharTest    chartest;				///存档数据
			t_Update_CharTest_SceneRecord()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_UPDATE_CHARTEST)
				{
					bzero(&chartest , sizeof(chartest));
				}
		};
		const BYTE PARA_SCENE_DELETE_CHARTEST = 6;
		struct t_Delete_CharTest_SceneRecord : t_NullCmd
		{
			char name[MAX_NAMESIZE];				/// 角色名称
			DWORD level;							/// 角色等级
			t_Delete_CharTest_SceneRecord()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_DELETE_CHARTEST)
				{
				}
		};
		const BYTE PARA_SCENE_READ_CHARTEST = 7;
		struct t_Read_CharTest_SceneRecord : t_NullCmd
		{
			char name[MAX_NAMESIZE];				/// 角色名称
			DWORD level;							/// 角色等级
			CharTest    chartest;				///存档数据
			t_Read_CharTest_SceneRecord()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_READ_CHARTEST)
				{
					bzero(&chartest , sizeof(chartest));
				}
		};
#endif

		//////////////////////////////////////////////////////////////
		/// 档案服务器和场景交互的指令
		//////////////////////////////////////////////////////////////
		
		//////////////////////////////////////////////////////////////
		/// 档案服务器和会话交互的指令开始
		//////////////////////////////////////////////////////////////
		//检查用户是否存在
		const BYTE PARA_CHK_USER_EXIST = 1;
		struct t_chkUserExist_SessionRecord : t_NullCmd
		{
			char  name[MAX_NAMESIZE+1];		/// 角色名称
			DWORD user_id;					/// 推荐人ID
			DWORD  from_id;					/// 被推荐人ID
			DWORD user_level;				/// 推荐人等级

			t_chkUserExist_SessionRecord()
				: t_NullCmd(CMD_SESSION, PARA_CHK_USER_EXIST)
				{
					bzero(name, sizeof(name));
					user_id = 0;
					user_level = 0;
					from_id = 0;
				}
		};

		//////////////////////////////////////////////////////////////
		/// 档案服务器和会话交互的指令结束
		//////////////////////////////////////////////////////////////

	};

};

#pragma pack()

#endif

