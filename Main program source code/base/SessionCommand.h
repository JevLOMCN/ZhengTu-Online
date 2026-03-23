/**
 * \file
 * \version  $Id: SessionCommand.h  $
 * \author  
 * \date 
 * \brief 定义Session服务器相关指令
 *
 */


#ifndef _SessionCommand_h_
#define _SessionCommand_h_

#include "zNullCmd.h"
#include "Union.h"
#include "Sept.h"
#include "Object.h"
#include "Command.h"
#include "zSceneEntry.h"


#pragma pack(1)
#ifndef _USE_CMD_NAMESPACE
#define _USE_CMD_NAMESPACE
#endif
namespace Cmd
{

	namespace Session
	{
		const BYTE CMD_LOGIN = 1;
		const BYTE CMD_SCENE = 2;
		const BYTE CMD_GATE = 3;
		const BYTE CMD_FORWARD = 4;
		const BYTE CMD_SESSION = 5;
		const BYTE CMD_GMTOOL = 6;
		const BYTE CMD_SCENE_SHUTDOWN = 7;
		const BYTE CMD_SCENE_TMP = 8;	// 临时指令
		const BYTE CMD_SCENE_SEPT = 9;	// 家族指令
		const BYTE CMD_SCENE_COUNTRY = 10; // 国家指令
		const BYTE CMD_SCENE_DARE = 11; // 对战指令
		const BYTE CMD_SCENE_UNION = 12; // 帮会指令
		const BYTE CMD_SCENE_ARMY = 13; // 军队指令
		const BYTE CMD_SCENE_GEM = 14;  // 护宝指令
		const BYTE CMD_SCENE_RECOMMEND = 15; // 推荐系统指令
        const BYTE CMD_SCENE_FUBEN     = 16; // 副本系统指令
		const BYTE CMD_SCENE_BOSSINFO = 19;		

		//////////////////////////////////////////////////////////////
		/// 登陆Session服务器指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_LOGIN = 1;
		struct t_LoginSession : t_NullCmd
		{
			WORD wdServerID;
			WORD wdServerType;
			t_LoginSession()
				: t_NullCmd(CMD_LOGIN, PARA_LOGIN) {};
		};
		//////////////////////////////////////////////////////////////
		/// 登陆Session服务器指令
		//////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////
		/// 场景服务器指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_SCENE_REGSCENE = 1;
		struct t_regScene_SceneSession : t_NullCmd
		{
			DWORD dwID;
			DWORD dwTempID;
			char byName[MAX_NAMESIZE+1];
			char fileName[MAX_NAMESIZE+1];
			DWORD dwCountryID;
			BYTE byLevel;

			t_regScene_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_REGSCENE) {};
		};

		const BYTE PARA_SCENE_UNREGSCENE = 2;
		struct t_unregScene_SceneSession : t_NullCmd
		{
			DWORD dwTempID;
			t_unregScene_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_UNREGSCENE) {};
		};

		const BYTE PARA_SCENE_REGSCENE_RET = 3;
		const BYTE REGSCENE_RET_REGOK=	0;
		const BYTE REGSCENE_RET_REGERR=	2;
		struct t_regScene_ret_SceneSession : t_NullCmd
		{
			DWORD dwTempID;
			BYTE byValue;
			t_regScene_ret_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_REGSCENE_RET) {};
		};

		const BYTE PARA_SCENE_REGUSER = 4;
		struct t_regUser_SceneSession : t_NullCmd
		{
			DWORD accid;
			DWORD dwID;
			DWORD dwTempID;
			DWORD dwMapID;
			BYTE byName[MAX_NAMESIZE+1];
			BYTE byMapName[MAX_NAMESIZE+1];
			DWORD dwGatewayServerID;
			t_regUser_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_REGUSER) {};
		};

		const BYTE PARA_SCENE_UNREGUSER = 5;
		const BYTE UNREGUSER_RET_LOGOUT=0;
		const BYTE UNREGUSER_RET_ERROR=1;
		struct t_unregUser_SceneSession : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwSceneTempID;
			BYTE retcode;
			t_unregUser_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_UNREGUSER) {};
		};

		const BYTE PARA_SCENE_REGUSERSUCCESS = 6;  // 注册成功通知会话服务器
		struct t_regUserSuccess_SceneSession : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwUnionID;
			DWORD dwSchoolID;
			DWORD dwSeptID;
			DWORD dwCountryID;
			DWORD dwGrace;
			DWORD dwExploit;
			QWORD qwExp;
			t_regUserSuccess_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_REGUSERSUCCESS) {};
		};

		const BYTE PARA_SCENE_LEVELUPNOTIFY = 7;  // 玩家升级通知会话服务器
		struct t_levelupNotify_SceneSession : t_NullCmd
		{
			DWORD dwUserID;
			WORD level;
			QWORD qwExp;
			t_levelupNotify_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_LEVELUPNOTIFY) {};
		};


		const BYTE PARA_SCENE_CHANEG_SCENE = 8;  //切换场景
		struct t_changeScene_SceneSession : t_NullCmd
		{
			DWORD id;
			DWORD temp_id;
			DWORD x;
			DWORD y;
			DWORD map_id;
			BYTE map_file[MAX_NAMESIZE];
			BYTE map_name[MAX_NAMESIZE];			
			t_changeScene_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_CHANEG_SCENE)
				{
					bzero(map_file, sizeof(map_file));
					bzero(map_name, sizeof(map_name));
				}
		};

		const BYTE PARA_SCENE_GM_COMMAND = 9;  //GM指令
		const BYTE GM_COMMAND_FINDUSER = 1;  //指令finduser
		const BYTE GM_COMMAND_GOTOUSER = 2;  //指令gotouser
		const BYTE GM_COMMAND_CATCHUSER = 3;  //指令catchuser
		const BYTE GM_COMMAND_DONTTALK = 4;  //指令donttalk
		const BYTE GM_COMMAND_TALK = 5;  //指令talk
		const BYTE GM_COMMAND_KICK = 6;  //指令kick
		const BYTE GM_COMMAND_SETPRIV = 7;  //指令setpriv
		const BYTE GM_COMMAND_LOCKVALUE = 8;  //指令lockvalue
		const BYTE GM_COMMAND_LEVELUP = 9;  //指令levelup
		const BYTE GM_COMMAND_LOAD_GIFT = 10;  //指令loadgift
		const BYTE GM_COMMAND_LOAD_PROCESS = 11;  //指令loadprocess
		const BYTE GM_COMMAND_EMBAR = 12;  //指令embar
		const BYTE GM_COMMAND_NEWZONE = 13;  //配置重生点
		const BYTE GM_COMMAND_REFRESH_GENERAL = 14;  //刷新大将军
		const BYTE GM_COMMAND_LOADMESSAGE = 15;  //soke 刷新Message
		const BYTE GM_COMMAND_LOADQUEST = 16;	 //soke 重新加载任务系统
		const BYTE GM_COMMAND_LOADNPCTRADE = 17; //soke 初始化NPC交易配置
		const BYTE GM_COMMAND_LOADTBL = 18;		 //soke 重新加载TBL文件
		const BYTE GM_COMMAND_LOADSYSTEM = 19;  //soke 刷新System公告
		const BYTE GM_COMMAND_LOAD_GAME_CONFIG = 20; //soke加载场景和会话服务器上的温泉配置
		const BYTE GM_COMMAND_LOADNEWQUEST = 21; // 刷新lua脚本
	    const BYTE GM_COMMAND_LOADGIFTBOX = 22; // 刷新箱子配置
        const BYTE GM_COMMAND_LOAD_FUBEN_CONFIG = 23; // 刷新副本配置
		const BYTE GM_COMMAND_LOADTBL2 = 24;		 //soke 重新加载TBL文件2

		const BYTE GM_COMMAND_STATE_REQ = 1;  //指令状态req表示请求
		const BYTE GM_COMMAND_STATE_RET = 2;  //指令状态ret表示返回结果

		const BYTE GM_COMMAND_ERR_NOERR = 0;  //没有错误
		const BYTE GM_COMMAND_ERR_NOUSER = 1;  //玩家不在线
		const BYTE GM_COMMAND_ERR_PRIV = 2;  //权限不足
		const BYTE GM_COMMAND_ERR_PARAM = 3;  //参数错误
		const BYTE GM_COMMAND_ERR_FAIL = 4;  //指令执行失败
		struct t_gmCommand_SceneSession : t_NullCmd
		{
			BYTE gm_cmd;//GM指令
			BYTE cmd_state;//指令执行状态
			BYTE err_code;//错误信息
			DWORD id;//用户ID
			BYTE src_priv;//使用指令者权限
			DWORD x;//xy用来传递附加的参数
			DWORD y;
			BYTE src_name[MAX_NAMESIZE];
			BYTE dst_name[MAX_NAMESIZE];
			BYTE map_file[MAX_NAMESIZE];
			BYTE map_name[MAX_NAMESIZE];			
			t_gmCommand_SceneSession() 
				: t_NullCmd(CMD_SCENE, PARA_SCENE_GM_COMMAND),
			gm_cmd(0),cmd_state(1),err_code(0),id(0),src_priv(1),x(0),y(0)
			{
				src_name[0] = 0;
				dst_name[0] = 0;
				map_file[0] = 0;
				map_name[0] = 0;
			};
		};

		const BYTE PARA_SCENE_PRIVATE_CHAT = 10;  //跨场景私聊
		const BYTE PRIVATE_CHAT_ERR_NOUSER = 1;  //玩家不在线
		const BYTE PRIVATE_CHAT_ERR_FILTER = 2;  //对方没开启私聊
		const BYTE PRIVATE_CHAT_ACT_INVITE = 1;  //邀请
		const BYTE PRIVATE_CHAT_ACT_JOIN = 2;  //加入
		const BYTE PRIVATE_CHAT_ACT_LEAVE = 3;  //离开
		struct t_privateChat_SceneSession : t_NullCmd
		{
			BYTE act;//动作
			BYTE err_code;//错误信息
			BYTE src_name[MAX_NAMESIZE];
			BYTE dst_name[MAX_NAMESIZE];
			DWORD cmd_size;
			BYTE chat_cmd[0];
			t_privateChat_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_PRIVATE_CHAT) {};
		};

		const BYTE PARA_SCENE_UNLOAD_SCENE = 11;  //停止注册
		struct t_unloadScene_SceneSession : t_NullCmd
		{
			DWORD map_id;
			t_unloadScene_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_UNLOAD_SCENE) {};
		};

		const BYTE PARA_SCENE_SYS_SETTING = 12;  //系统设置
		struct t_sysSetting_SceneSession : t_NullCmd
		{
			BYTE name[MAX_NAMESIZE];
			BYTE sysSetting[20];
			DWORD face;
			t_sysSetting_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_SYS_SETTING) {};
		};

		const BYTE PARA_SCENE_CITY_RUSH = 13;  //怪物攻城的通知
		struct t_cityRush_SceneSession : t_NullCmd
		{
			char bossName[MAX_NAMESIZE];
			char rushName[MAX_NAMESIZE];
			char mapName[MAX_NAMESIZE];
			DWORD delay;
			DWORD countryID;
			t_cityRush_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_CITY_RUSH) {};
		};

		const BYTE PARA_SCENE_CREATE_SCHOOL = 16;  //GM指令创建门派
		struct t_createSchool_SceneSession : t_NullCmd
		{
			char masterName[MAX_NAMESIZE];
			char schoolName[MAX_NAMESIZE];
			t_createSchool_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_CREATE_SCHOOL) {};
		};
		
		

		const BYTE PARA_SCENE_CREATE_DARE = 17;  //创建对战指令
		struct t_createDare_SceneSession : t_NullCmd
		{
			//char relation1[MAX_NAMESIZE];	// 挑战方社会关系名称
			//char relation2[MAX_NAMESIZE];	// 应战方社会关系名称
			//DWORD relationID1;		// 挑战方社会关系ID(挑战方将用一个向量代替)
			DWORD relationID2;		// 应战方社会关系ID

			DWORD ready_time;		// 对战前的等待时间
			DWORD active_time;		// 对战进行时间
			BYTE type;  			//对战类型
			
			t_createDare_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_CREATE_DARE) 
				{
					ready_time = 0;
					active_time = 0;
				};
		};

		const BYTE PARA_SCENE_CREATE_QUIZ = 19;
		struct t_createQuiz_SceneSession : t_NullCmd
		{
			DWORD ready_time;
			DWORD active_time;
			DWORD dwUserID;	  
			DWORD dwSubjects; // 总题目数
			BYTE  type;	  // 竞赛类型
			BYTE  subject_type; // 题库类型
			
			t_createQuiz_SceneSession() : t_NullCmd(CMD_SCENE, PARA_SCENE_CREATE_QUIZ)
			{
				ready_time = 0;
				active_time = 0;
				dwUserID = 0;
				type = 0;
				subject_type = 0;
			}
		};

		//////////////////////////////////////////////////////////////
		///  场景服务器之对战指令
		//////////////////////////////////////////////////////////////

		const BYTE PARA_SCENE_REMOVE_SCENE = 14;  //卸载地图
		struct t_removeScene_SceneSession : t_NullCmd
		{
			DWORD map_id;
			t_removeScene_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_REMOVE_SCENE) {};
		};

		const BYTE PARA_SCENE_REQ_ADD_SCENE = 15;
		struct t_reqAddScene_SceneSession : t_NullCmd
		{
			DWORD dwServerID;
			DWORD dwCountryID;
			DWORD dwMapID;
			t_reqAddScene_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_REQ_ADD_SCENE) {};
		};

		///直接转发到用户的指令
		const BYTE PARA_SCENE_FORWARD_USER = 18;
		struct t_forwardUser_SceneSession : t_NullCmd
		{
			DWORD id;
			DWORD tempid;
			char name[MAX_NAMESIZE];
			DWORD cmd_len;
			BYTE cmd[0];
			t_forwardUser_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_FORWARD_USER)
				{
					id = 0;
					tempid = 0;
					bzero(name, MAX_NAMESIZE);
				}
		};

		///通知场景处罚一个玩家
		const BYTE PARA_SCENE_FORBID_TALK = 20;
		struct t_forbidTalk_SceneSession : t_NullCmd
		{
			char name[MAX_NAMESIZE];
			char reason[128];
			WORD operation;
			int delay;
			char gm[MAX_NAMESIZE];
			t_forbidTalk_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_FORBID_TALK)
				{
					bzero(name, sizeof(name));
					bzero(reason, sizeof(reason));
					operation = 0;
					delay = 0;
					bzero(gm, sizeof(gm));
				}
		};

		///通知玩家运镖失败
		const BYTE PARA_SCENE_GUARD_FAIL = 21;
		struct t_guardFail_SceneSession : t_NullCmd
		{
			DWORD userID;
			BYTE type;//0: 被抢劫 1:时间结束 
			t_guardFail_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_GUARD_FAIL)
				{
					userID = 0;
				}
		};

		// 场景通知session玩家消费金币
		const BYTE PARA_SPEND_GOLD = 22;
		struct t_SpendGold_SceneSession : t_NullCmd
		{
			DWORD userID;
			DWORD gold;
			t_SpendGold_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_SPEND_GOLD) {}
		};

		// 场景通知session功勋竞猜开奖
		const BYTE PARA_GXJCKJ = 300;
		struct t_Gxjckj_SceneSession : t_NullCmd
		{
			DWORD index;
			t_Gxjckj_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_GXJCKJ) {}
		};

		// 场景通知session战力榜离线发放
		const BYTE PARA_ZLFF = 301;
		struct t_Zhanlifafang_SceneSession : t_NullCmd
		{
			DWORD charid;
			DWORD index;
			t_Zhanlifafang_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_ZLFF) {}
		};

		// 场景通知session护国榜离线发放
		const BYTE PARA_HGFF = 302;
		struct t_Huguofafang_SceneSession : t_NullCmd
		{
			DWORD charid;
			DWORD index;
			t_Huguofafang_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_HGFF) {}
		};

		// 场景通知session鲜花榜离线发放
		const BYTE PARA_XHFF = 303;
		struct t_Xianhuafafang_SceneSession : t_NullCmd
		{
			DWORD charid;
			DWORD index;
			t_Xianhuafafang_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_XHFF) {}
		};

		// 场景通知session护花榜离线发放
		const BYTE PARA_SENDXHFF = 304;
		struct t_SendXianhuafafang_SceneSession : t_NullCmd
		{
			DWORD charid;
			DWORD index;
			t_SendXianhuafafang_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_SENDXHFF) {}
		};

		// 场景通知session查询家族战车列表
		const BYTE PARA_ZHANCHELIST = 305;
		struct t_ZhancheList_SceneSession : t_NullCmd
		{
			DWORD dwUserID;
			DWORD septid;
			t_ZhancheList_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_ZHANCHELIST) {}
		};

		// 场景通知session战车领用
		const BYTE PARA_ZHANCHELINGYONG = 306;
		struct t_ZhancheLingYong_SceneSession : t_NullCmd
		{
			DWORD dwUserID;
			DWORD zhancheid;
			t_ZhancheLingYong_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_ZHANCHELINGYONG) {}
		};

		// 场景通知session战车归还
		const BYTE PARA_ZHANCHEGUIHUAN = 307;
		struct t_ZhancheGuiHuan_SceneSession : t_NullCmd
		{
			DWORD dwUserID;
			DWORD zhancheid;
			t_ZhancheGuiHuan_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_ZHANCHEGUIHUAN) {}
		};
		
		// 场景通知session家族BOSS
		const BYTE PARA_JIAZUBOSS = 250;
		struct t_JiazuBoss_SceneSession : t_NullCmd
		{
			DWORD userID;
			t_JiazuBoss_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_JIAZUBOSS) {}
		};
		
		// 场景通知session喂养家族BOSS
		const BYTE PARA_WEIYANGJIAZUBOSS = 251;
		struct t_WeiyangJiazuBoss_SceneSession : t_NullCmd
		{
			DWORD userID;
			t_WeiyangJiazuBoss_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_WEIYANGJIAZUBOSS) {}
		};

		// 场景通知session召唤BOSS
		const BYTE PARA_ZHAOHUANJIAZUBOSS = 252;
		struct t_ZhaoHuanJiazuBoss_SceneSession : t_NullCmd
		{
			DWORD userID;
			t_ZhaoHuanJiazuBoss_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_ZHAOHUANJIAZUBOSS) {}
		};



		// 场景通知session开关各种服务
		const DWORD SERVICE_MAIL = 1;
		const DWORD SERVICE_AUCTION = 2;
		const DWORD SERVICE_PROCESS = 4;
		const DWORD SERVICE_HORSE = 8;
		const BYTE PARA_SET_SERVICE = 23;
		struct t_SetService_SceneSession : t_NullCmd
		{
			DWORD flag;
			t_SetService_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_SET_SERVICE)
				{
					flag = 0;
				}
		};

		const BYTE PARA_SCENE_CITY_RUSH_CUST = 24;  //自定义怪物攻城的通知
		struct t_cityRushCust_SceneSession : t_NullCmd
		{
			char text[128];
			DWORD countryID;
			t_cityRushCust_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_CITY_RUSH_CUST) {};
		};

		const BYTE PARA_SCENE_BAIL_CHECK = 25;  //取保就医的检查
		struct t_bailCheck_SceneSession : t_NullCmd
		{
			char name[MAX_NAMESIZE];
			DWORD money;
			t_bailCheck_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_BAIL_CHECK) {};
		};

		/*
		const BYTE PARA_SCENE_LOAD_PROCESS = 26;  //加载特征码文件
		struct t_loadProcess_SceneSession : t_NullCmd
		{
			t_loadProcess_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_LOAD_PROCESS) {};
		};
		*/

		struct giftInfo
		{
			DWORD actID;
			DWORD charID;
			char name[MAX_NAMESIZE];
			BYTE itemGot;
			char mailText[MAX_CHATINFO];
			DWORD money;
			DWORD itemID;
			BYTE itemType;
			DWORD itemNum;
			BYTE bind;
		};
		const BYTE PARA_SCENE_SEND_GIFT = 27;  //发送物品邮件，session到场景获得物品
		struct t_sendGift_SceneSession : t_NullCmd
		{
			giftInfo info;
			t_sendGift_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_SEND_GIFT) {};
		};

		// 发送全服通知
		const BYTE PARA_SERVER_NOTIFY = 28;
		struct t_serverNotify_SceneSession : t_NullCmd
		{
			int infoType;
			char info[MAX_CHATINFO];

			t_serverNotify_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_SERVER_NOTIFY)
				{
					infoType = Cmd::INFO_TYPE_GAME;
					bzero(info, sizeof(info));
				}
		};

		// 发送一个场景的GM公告
		const BYTE PARA_BROADCAST_SCENE = 29;
		struct t_broadcastScene_SceneSession : t_NullCmd
		{
			int infoType;
			DWORD mapID;
			char GM[MAX_NAMESIZE];
			char info[MAX_CHATINFO];

			t_broadcastScene_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_BROADCAST_SCENE)
				{
					infoType = Cmd::INFO_TYPE_SCROLL;
					mapID = 0;
					bzero(info, sizeof(info));
					bzero(GM, sizeof(GM));
				}
		};

		const BYTE PARA_SCENE_SEND_CMD = 30;
		struct t_sendCmd_SceneSession : t_NullCmd
		{
			DWORD mapID;
			DWORD len;
			BYTE cmd[0];
			
			t_sendCmd_SceneSession() : t_NullCmd(CMD_SCENE, PARA_SCENE_SEND_CMD)
			{
				mapID = 0;
				len = 0;
			}
		};
		
		//soke 升级好友祝贺
		const BYTE PARA_SCENE_LEVELUP = 31;
		struct t_levelup_SceneSession : t_NullCmd
		{
			DWORD dwID;
			DWORD dwLevel;
			DWORD dwType;//1男2女
			char byName[MAX_NAMESIZE+1];

			t_levelup_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_LEVELUP) {};
		};

		//soke 场景通知session封印等级（未转生）服务
		const DWORD SERVICE2_PROCESS = 32;
		const BYTE PARA2_SET_SERVICE = 33;
		struct t_SetFylevel_SceneSession : t_NullCmd
		{
			DWORD flag;
			t_SetFylevel_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA2_SET_SERVICE)
				{
					flag = 0;
				}
		};

		//soke 场景通知session满级等级（未转生）服务
		const DWORD SERVICE3_PROCESS = 34;
		const BYTE PARA3_SET_SERVICE = 35;
		struct t_SetMaxlevel_SceneSession : t_NullCmd
		{
			DWORD flag;
			t_SetMaxlevel_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA3_SET_SERVICE)
				{
					flag = 0;
				}
		};
		//soke 场景通知session封印等级（转生）服务
		const DWORD SERVICE4_PROCESS = 36;
		const BYTE PARA4_SET_SERVICE = 37;
		struct t_SetTrunFylevel_SceneSession : t_NullCmd
		{
			DWORD flag;
			t_SetTrunFylevel_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA4_SET_SERVICE)
				{
					flag = 0;
				}
		};

		//soke 场景通知session满级等级（转生）服务
		const DWORD SERVICE5_PROCESS = 38;
		const BYTE PARA5_SET_SERVICE = 39;
		struct t_SetTrunMaxlevel_SceneSession : t_NullCmd
		{
			DWORD flag;
			t_SetTrunMaxlevel_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA5_SET_SERVICE)
				{
					flag = 0;
				}
		};
		
		//////////////////////////////////////////////////////////////
		/// 场景服务器之社会关系公共指令
		//////////////////////////////////////////////////////////////
		enum{
			RELATION_TYPE_COUNTRY,
			RELATION_TYPE_SEPT,
			RELATION_TYPE_SCHOOL,
			RELATION_TYPE_UNION,
			RELATION_TYPE_NOTIFY
		};
		/// 角色成功加入或退出团体通知场景更新其对应的社会关系ID的值
		const BYTE PARA_SEND_USER_RELATION_ID = 50;
		struct t_sendUserRelationID : t_NullCmd
		{
			DWORD dwUserID;			/// 角色ID
			BYTE type;			/// 社会关系类型
			DWORD dwID;			/// 社会关系ID
			char name[MAX_NAMESIZE];	/// 社会关系名称
			DWORD caption;			/// 头衔，帮会所在城MAPID,当社会关系类型师徒的时候这个字段放师傅的CHARID
			bool  unionmaster;		/// 是否为帮主
			bool  septmaster;		/// 是否为族长
			bool  king;			/// 是否为国王
			DWORD dwActionPoint;		/// 行动力
			DWORD dwRepute;			/// 家族声望
			DWORD dwSeptLevel;		/// 家族等级
			bool  emperor;			/// 是否为皇帝

			t_sendUserRelationID()
				: t_NullCmd(CMD_SCENE, PARA_SEND_USER_RELATION_ID) 
			{
					dwActionPoint = 0;
					dwRepute = 0;
					king = false;
					emperor = false;
					septmaster = false;
					unionmaster = false;
					bzero(name, MAX_NAMESIZE);
				};
		};
		
		/// 用户上线时，更新其配偶关系
		const BYTE PARA_UPDATE_CONSORT = 51;
		struct t_updateConsort : t_NullCmd
		{
			DWORD dwConsort;
			DWORD dwUserID;
			BYTE  byKingConsort;
			t_updateConsort() : t_NullCmd(CMD_SCENE, PARA_UPDATE_CONSORT) 
			{
				byKingConsort = 0;
			};
		};
		
		/// 用户上线时，更新其国家星号
		const BYTE PARA_UPDATE_COUNTRY_STAR = 52;
		struct t_updateCountryStar : t_NullCmd
		{
			DWORD dwCountryStar;
			DWORD dwUserID;
			
			t_updateCountryStar() : t_NullCmd(CMD_SCENE, PARA_UPDATE_COUNTRY_STAR) {};
		};

		/// 增加仇人
		const BYTE PARA_ADD_RELATION_ENEMY = 53;
		struct t_addRelationEnemy : t_NullCmd
		{
			DWORD dwUserID;
			char name[MAX_NAMESIZE];
			
			t_addRelationEnemy() : t_NullCmd(CMD_SCENE, PARA_ADD_RELATION_ENEMY) {};
		};

		/// 通知上线角色所占领的NPC情况
		const BYTE PARA_NOTIFY_NPC_HOLD_DATA = 54;
		struct t_notifyNpcHoldData : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwMapID;
			DWORD dwPosX;
			DWORD dwPosY;
			t_notifyNpcHoldData() : t_NullCmd(CMD_SCENE, PARA_NOTIFY_NPC_HOLD_DATA) {};
		};

		/// 通知上线角色所占领的NPC情况
		const BYTE PARA_NOTIFY_ADD_INTEGRAL = 55;
		struct t_notifyAddIntegral : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwGoldUser;
			DWORD dwNum;
			t_notifyAddIntegral() : t_NullCmd(CMD_SCENE, PARA_NOTIFY_ADD_INTEGRAL) {};
		};

		//////////////////////////////////////////////////////////////
		/// 场景服务器之社会关系公共指令
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		/// 场景服务器之帮会指令
		//////////////////////////////////////////////////////////////
		// 增加帮会
		const BYTE PARA_UNION_ADDUNION = 101;
		struct t_addUnion_SceneSession : t_NullCmd
		{
			DWORD dwItemID;             /// 道具物品的对象id 成功以后删除
			DWORD byRetcode;            /// 创建返回时：0 表示创建失败名称重复， 1表示成功
			DWORD dwMapTempID;          /// 地图临时编号
			UnionDef::stUnionInfo info;
			t_addUnion_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_UNION_ADDUNION) {};
		};

		// 增加成员
		const BYTE PARA_UNION_ADDMEMBER = 102;
		struct t_addUnionMember_SceneSession : t_NullCmd
		{
			DWORD dwUnionID;
			UnionDef::stUnionMemberInfo member;
			t_addUnionMember_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_UNION_ADDMEMBER) {};
		};
		// 开除成员
		const BYTE PARA_UNION_FIREMEMBER = 103;
		struct t_fireUnionMember_SceneSession : t_NullCmd
		{
			DWORD dwCharID;
			DWORD dwMapTempID;
			t_fireUnionMember_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_UNION_FIREMEMBER) {}
		};
		//////////////////////////////////////////////////////////////
		/// 场景服务器之帮会指令
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		/// 场景服务器之家族指令
		//////////////////////////////////////////////////////////////
		// 增加帮会
		const BYTE PARA_SEPT_ADDSEPT = 121;
		struct t_addSept_SceneSession : t_NullCmd
		{
			DWORD byRetcode;            /// 创建返回时：0 表示创建失败名称重复， 1表示成功
			DWORD dwMapTempID;          /// 地图临时编号
			SeptDef::stSeptInfo info;
			t_addSept_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SEPT_ADDSEPT) 
				{
					byRetcode = 0;
					bzero(&info, sizeof(info));
				};
		};

		// 增加成员
		const BYTE PARA_SEPT_ADDMEMBER = 122;
		struct t_addSeptMember_SceneSession : t_NullCmd
		{
			DWORD dwSeptID;
			SeptDef::stSeptMemberInfo member;
			t_addSeptMember_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SEPT_ADDMEMBER) {};
		};
		// 开除成员
		const BYTE PARA_SEPT_FIREMEMBER = 123;
		struct t_fireSeptMember_SceneSession : t_NullCmd
		{
			DWORD dwCharID;
			DWORD dwMapTempID;
			t_fireSeptMember_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_SEPT_FIREMEMBER) {}
		};
		
		// 经验分配
		const BYTE PARA_SEPT_EXP_DISTRIBUTE = 124;
		struct t_distributeSeptExp_SceneSession : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwExp;
			t_distributeSeptExp_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_SEPT_EXP_DISTRIBUTE) {}
		};

		// 发送全国通知
		const BYTE PARA_COUNTRY_NOTIFY = 125;
		struct t_countryNotify_SceneSession : t_NullCmd
		{
			int infoType;
			DWORD dwCountryID;
			char info[MAX_CHATINFO];

			t_countryNotify_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_COUNTRY_NOTIFY)
				{
					infoType = Cmd::INFO_TYPE_GAME;
				}
		};
		
		// 家族争夺NPC之对战请求
		const BYTE PARA_SEPT_NPCDARE_DARE = 127;
		struct t_NpcDare_Dare_SceneSession : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwCountryID;
			DWORD dwMapID;
			DWORD dwNpcID;
			t_NpcDare_Dare_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_SEPT_NPCDARE_DARE) {}
		};

		// 家族争夺NPC之场景通知
		const BYTE PARA_SEPT_NPCDARE_NOTIFYSCENE = 128;
		struct t_NpcDare_NotifyScene_SceneSession : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwCountryID;
			DWORD dwMapID;
			DWORD dwNpcID;
			DWORD dwPosX;
			DWORD dwPoxY;
			t_NpcDare_NotifyScene_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_SEPT_NPCDARE_NOTIFYSCENE) {}
		};

		// 家族争夺NPC之结果反馈
		const BYTE PARA_SEPT_NPCDARE_RESULT = 129;
		struct t_NpcDare_Result_SceneSession : t_NullCmd
		{
			DWORD dwSeptID;
			t_NpcDare_Result_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_SEPT_NPCDARE_RESULT) {}
		};

		// 家族争夺NPC之结果反馈
		const BYTE PARA_SEPT_NPCDARE_GETGOLD = 130;
		struct t_NpcDare_GetGold_SceneSession : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwGold;
			DWORD dwNpcID;
			DWORD dwMapID;
			DWORD dwCountryID;

			t_NpcDare_GetGold_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_SEPT_NPCDARE_GETGOLD) {}
		};

		// 家族争夺NPC之道具返还
		const BYTE PARA_SEPT_NPCDARE_ITEMBACK = 131;
		struct t_NpcDare_ItemBack_SceneSession : t_NullCmd
		{
			DWORD dwUserID;
			t_NpcDare_ItemBack_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_SEPT_NPCDARE_ITEMBACK) {}
		};

		// 家族争夺NPC之GM指令，立即触发开战
		const BYTE PARA_SEPT_NPCDARE_GMCMD = 132;
		struct t_NpcDare_GMCmd_SceneSession : t_NullCmd
		{
			t_NpcDare_GMCmd_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_SEPT_NPCDARE_GMCMD) {}
		};

		
		// 由场景发往会话，查询城市所属
		const BYTE PARA_QUESTION_NPCDARE = 135;
		struct t_questionNpcDare_SceneSession : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwCountryID;
			DWORD dwMapID;
			DWORD dwNpcID;
			BYTE  byType;

			t_questionNpcDare_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_QUESTION_NPCDARE) {};
		};

		// 由会话发往场景，通知国家税率
		const BYTE PARA_TAX_COUNTRY = 136;
		struct t_taxCountry_SceneSession : t_NullCmd
		{
			DWORD dwCountryID;
			DWORD dwTempID;
			BYTE  byTax;

			t_taxCountry_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_TAX_COUNTRY) {};
		};

		// 由场景发会话，通知国家税收增加
		const BYTE PARA_TAXADD_COUNTRY = 137;
		struct t_taxAddCountry_SceneSession : t_NullCmd
		{
			DWORD dwCountryID;
			QWORD qwTaxMoney;
			t_taxAddCountry_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_TAXADD_COUNTRY) {};
		};
		//////////////////////////////////////////////////////////////
		/// 场景服务器之家族指令
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		/// 场景服务器之好友度指令
		//////////////////////////////////////////////////////////////
		struct stCountMember
		{
			DWORD	dwUserID;
			WORD	wdDegree;
			BYTE	byType;
		};

		struct stRequestMember
		{
			char name[MAX_NAMESIZE];
		};

		enum{
			TYPE_FRIEND,	// 朋友类型
			TYPE_CONSORT,	// 夫妻类型
			TYPE_TEACHER,	// 师傅类型
			TYPE_PRENTICE,	// 徒弟类型
		};
		struct stRequestReturnMember
		{
			DWORD dwUserID;
			WORD  wdDegree;
			BYTE  byType; // 使用枚举 RELATION_TYPE
		};

		// 向会话服务器请求计算所有的友好度
		const BYTE PARA_FRIENDDEGREE_COUNT = 150;
		struct t_CountFriendDegree_SceneSession : t_NullCmd
		{
			char		name[MAX_NAMESIZE];	// 玩家名称
			WORD		size;				// 团队列表的大小
			stCountMember	namelist[0];		// 剩余团队列表
			t_CountFriendDegree_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_FRIENDDEGREE_COUNT) {}
		};

		// 向会话服务器重新请求友好度关系列表，将会分发到各个队员身上
		const BYTE PARA_FRIENDDEGREE_REQUEST = 151;
		struct t_RequestFriendDegree_SceneSession : t_NullCmd
		{
			WORD  size;				// 队伍人数
			stRequestMember namelist[0];	// 团队列表
			t_RequestFriendDegree_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_FRIENDDEGREE_REQUEST) {}
		};


		// 会话服务器想场景服务器中的组队队员分发友好度关系列表。
		const BYTE PARA_FRIENDDEGREE_RETURN = 152;
		struct t_ReturnFriendDegree_SceneSession : t_NullCmd
		{
			DWORD dwID;				// 队长的tempID
			DWORD dwMapTempID;		// 地图的tempID
			WORD  size;				// 队伍人数
			stRequestReturnMember memberlist[0];	// 团队列表
			t_ReturnFriendDegree_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_FRIENDDEGREE_RETURN) {}
		};
		//////////////////////////////////////////////////////////////
		/// 场景服务器之好友度指令
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		/// 场景服务器之门派指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_SCHOOL_CREATE_SUCCESS = 170;
		struct t_SchoolCreateSuccess_SceneSession : t_NullCmd
		{
			DWORD	dwID;						// 师尊的ID
			DWORD	dwSchoolID;					// 门派ID
			char	schoolName[MAX_NAMESIZE];	// 门派名称
			t_SchoolCreateSuccess_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_SCHOOL_CREATE_SUCCESS) {}
		};
		//////////////////////////////////////////////////////////////
		/// 场景服务器之门派指令
		//////////////////////////////////////////////////////////////

		//soke 会话服务器向场景服务器返回关系检查结果
	    const BYTE PARA_CHECKRELATION_RESULT = 248;// 3
	    struct t_CheckRelationEmptyResult : t_NullCmd
	    {
		    DWORD dwUserID;//用户ID
		    bool isEmpty;
		    t_CheckRelationEmptyResult()
			   : t_NullCmd(CMD_SCENE,PARA_CHECKRELATION_RESULT) {}
	    };

		//////////////////////////////////////////////////////////////
		///  场景服务器之卡通宝宝指令开始
		//////////////////////////////////////////////////////////////

		const BYTE PARA_CARTOON_CMD = 179;
		struct t_CartoonCmd : t_NullCmd
		{
			BYTE cartoonPara;
			t_CartoonCmd()
				: t_NullCmd(CMD_SCENE, PARA_CARTOON_CMD) 
				{
					cartoonPara = 0;
				};
		};

		const BYTE PARA_CARTOON_BUY = 1;
		struct t_buyCartoon_SceneSession : t_CartoonCmd
		{
			Cmd::t_CartoonData data;
			t_buyCartoon_SceneSession()
				: t_CartoonCmd()
				{
					cartoonPara = PARA_CARTOON_BUY;

					bzero(&data, sizeof(data));
				};
		};

		const BYTE PARA_CARTOON_ADD = 2;
		struct t_addCartoon_SceneSession : t_CartoonCmd
		{
			DWORD userID;//ID
			DWORD cartoonID;
			Cmd::t_CartoonData data;
			t_addCartoon_SceneSession()
				: t_CartoonCmd()
				{
					cartoonPara = PARA_CARTOON_ADD;

					userID = 0;
					cartoonID = 0;
					bzero(&data, sizeof(Cmd::t_CartoonData));
				};
		};

		const BYTE PARA_CARTOON_GET_LIST = 3;
		struct t_getListCartoon_SceneSession : t_CartoonCmd
		{
			DWORD userID;//ID
			t_getListCartoon_SceneSession()
				: t_CartoonCmd()
				{
					cartoonPara = PARA_CARTOON_GET_LIST;

					userID = 0;
				};
		};

		enum saveType
		{
			SAVE_TYPE_DONTSAVE,
			SAVE_TYPE_PUTAWAY,
			SAVE_TYPE_LETOUT,
			SAVE_TYPE_ADOPT,
			SAVE_TYPE_RETURN,
			SAVE_TYPE_TIMEOVER,
			SAVE_TYPE_TIMETICK,
			//SAVE_TYPE_CHARGE,
			SAVE_TYPE_SYN
		};
		const BYTE PARA_CARTOON_SAVE = 4;
		struct t_saveCartoon_SceneSession : t_CartoonCmd
		{
			char userName[MAX_NAMESIZE];
			saveType type;
			DWORD cartoonID;//ID
			Cmd::t_CartoonData data;
			t_saveCartoon_SceneSession()
				: t_CartoonCmd()
				{
					cartoonPara = PARA_CARTOON_SAVE;

					bzero(userName, MAX_NAMESIZE);
					type = SAVE_TYPE_DONTSAVE;
					cartoonID = 0;
					bzero(&data, sizeof(Cmd::t_CartoonData));
				};
		};

		const BYTE PARA_CARTOON_ADOPT = 5;
		struct t_adoptCartoon_SceneSession : t_CartoonCmd
		{
			DWORD userID;//ID
			DWORD cartoonID;
			BYTE masterState;//领养时，其主人的状态 0:不在线 1:在线
			Cmd::t_CartoonData data;
			t_adoptCartoon_SceneSession()
				: t_CartoonCmd()
				{
					cartoonPara = PARA_CARTOON_ADOPT;

					userID = 0;
					cartoonID = 0;
					masterState = 0;
					bzero(&data, sizeof(Cmd::t_CartoonData));
				};
		};

		const BYTE PARA_CARTOON_GET_BACK = 6;
		struct t_getBackCartoon_SceneSession : t_CartoonCmd
		{
			DWORD userID;//ID
			DWORD cartoonID;
			t_getBackCartoon_SceneSession()
				: t_CartoonCmd()
				{
					cartoonPara = PARA_CARTOON_GET_BACK;

					userID = 0;
					cartoonID = 0;
				};
		};

		const BYTE PARA_CARTOON_NOTIFY = 7;//通知领养者主人上下线
		struct t_notifyCartoon_SceneSession : t_CartoonCmd
		{
			char adopter[MAX_NAMESIZE];
			BYTE state;//0:下线 1:上线
			DWORD cartoonID;
			t_notifyCartoon_SceneSession()
				: t_CartoonCmd()
				{
					cartoonPara = PARA_CARTOON_NOTIFY;

					bzero(adopter, MAX_NAMESIZE);
					state = 0;
					cartoonID = 0;
				};
		};

		const BYTE PARA_CARTOON_LOAD = 8;//上线时，获取宠物列表
		struct t_loadCartoon_SceneSession : t_CartoonCmd
		{
			DWORD userID;
			t_loadCartoon_SceneSession()
				: t_CartoonCmd()
				{
					cartoonPara = PARA_CARTOON_LOAD;

					userID = 0;
				};
		};

		const BYTE PARA_CARTOON_CORRECT = 9;//纠错，标记为被领养但是领养者没有宠物时，改为等待
		struct t_correctCartoon_SceneSession : t_CartoonCmd
		{
			DWORD cartoonID;
			t_correctCartoon_SceneSession()
				: t_CartoonCmd()
				{
					cartoonPara = PARA_CARTOON_CORRECT;

					cartoonID = 0;
				};
		};

		const BYTE PARA_CARTOON_SALE = 10;//上线时，获取宠物列表
		struct t_saleCartoon_SceneSession : t_CartoonCmd
		{
			DWORD userID;
			DWORD cartoonID;
			t_saleCartoon_SceneSession()
				: t_CartoonCmd()
				{
					cartoonPara = PARA_CARTOON_SALE;

					userID = 0;
					cartoonID = 0;
				};
		};

		const BYTE PARA_CARTOON_CHARGE = 11;//充值
		struct t_chargeCartoon_SceneSession : t_CartoonCmd
		{
			DWORD masterID;
			DWORD cartoonID;
			DWORD time;//秒数
			t_chargeCartoon_SceneSession()
				: t_CartoonCmd()
				{
					cartoonPara = PARA_CARTOON_CHARGE;

					masterID = 0;
					cartoonID = 0;
					time = 0;
				};
		};

		const BYTE PARA_CARTOON_CHARGE_NOTIFY = 12;//通知领养者宠物充值
		struct t_chargeNotifyCartoon_SceneSession : t_CartoonCmd
		{
			char adopter[MAX_NAMESIZE];
			DWORD cartoonID;
			DWORD time;//秒数
			t_chargeNotifyCartoon_SceneSession()
				: t_CartoonCmd()
				{
					cartoonPara = PARA_CARTOON_CHARGE_NOTIFY;

					bzero(adopter, MAX_NAMESIZE);
					cartoonID = 0;
					time = 0;
				};
		};

		const BYTE PARA_CARTOON_LEVEL_NOTIFY = 13;//通知领养者宠物充值
		struct t_levelNotifyCartoon_SceneSession : t_CartoonCmd
		{
			DWORD userID;
			DWORD cartoonID;
			DWORD level;
			t_levelNotifyCartoon_SceneSession()
				: t_CartoonCmd()
				{
					cartoonPara = PARA_CARTOON_LEVEL_NOTIFY;

					userID = 0;
					cartoonID = 0;
					level = 0;
				};
		};

		const BYTE PARA_CARTOON_DRAW = 14;//提取经验
		struct t_drawCartoon_SceneSession : t_CartoonCmd
		{
			DWORD userID;
			DWORD cartoonID;
			DWORD num;
			t_drawCartoon_SceneSession()
				: t_CartoonCmd()
				{
					cartoonPara = PARA_CARTOON_DRAW;

					userID = 0;
					cartoonID = 0;
					num = 0;
				};
		};

		const BYTE PARA_CARTOON_CONSIGN = 15;//委托领养
		struct t_consignCartoon_SceneSession : t_CartoonCmd
		{
			DWORD userID;//委托人ID
			DWORD cartoonID;//委托的宠物ID
			char name[MAX_NAMESIZE];//被委托人名字 发给玩家时是委托人的名字
			t_consignCartoon_SceneSession()
				: t_CartoonCmd()
				{
					cartoonPara = PARA_CARTOON_CONSIGN;

					userID = 0;
					cartoonID = 0;
					bzero(name, sizeof(name));
				};
		};

		const BYTE PARA_CARTOON_CONSIGN_RET = 16;//委托领养返回
		struct t_consignRetCartoon_SceneSession : t_CartoonCmd
		{
			DWORD userID;//被委托人ID
			DWORD cartoonID;
			BYTE ret;//0：拒绝 1：同意 2：已经领养5个了
			t_consignRetCartoon_SceneSession()
				: t_CartoonCmd()
				{
					cartoonPara = PARA_CARTOON_CONSIGN_RET;

					userID = 0;
					ret = 0;
					cartoonID = 0;
				};
		};

		const BYTE PARA_CARTOON_SET_REPAIR = 17;//设置自动修理
		struct t_setRepairCartoon_SceneSession : t_CartoonCmd
		{
			DWORD userID;//主人ID
			DWORD cartoonID;
			BYTE repair;//0：关闭 1：打开
			t_setRepairCartoon_SceneSession()
				: t_CartoonCmd()
				{
					cartoonPara = PARA_CARTOON_SET_REPAIR;

					userID = 0;
					cartoonID = 0;
					repair = 0;
				};
		};

		//////////////////////////////////////////////////////////////
		///  场景服务器之卡通宝宝指令结束
		//////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////
		///  场景服务器之邮件指令开始
		//////////////////////////////////////////////////////////////
		// 由场景往服务器发送，给玩家发邮件

		enum mailType
		{
			MAIL_TYPE_MAIL		= 1,	//邮件
			MAIL_TYPE_MSG	 	= 2,	//留言
			MAIL_TYPE_RETURN	=3,	//退回的邮件
			MAIL_TYPE_AUCTION	=4,	//拍卖用邮件
			MAIL_TYPE_SYS		=5	//系统邮件，不可退回
		};
		enum mailState
		{
			MAIL_STATE_NEW		= 1,
			MAIL_STATE_OPENED	= 2,
			MAIL_STATE_DEL		= 3
		};

		struct mailInfo
		{
			//DWORD id;
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
			DWORD sendMoney;
			DWORD recvMoney;
			DWORD sendGold;
			DWORD recvGold;
			DWORD fromID;
			DWORD toID;
			DWORD itemID;
		};

		/*
		enum
		{
			CHECKSEND_RETURN_OK,
			CHECKSEND_RETURN_FAILED,
			CHECKSEND_RETURN_NOPLAYER,
			CHECKSEND_RETURN_FULL
		};
		*/
		const BYTE PARA_SCENE_CHECKSEND = 180;
		struct t_checkSend_SceneSession : t_NullCmd
		{
			mailInfo mail;
			DWORD itemID;
			//BYTE retCode;
			t_checkSend_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_CHECKSEND) 
				{
					bzero(&mail, sizeof(mail));
					itemID = 0;
					//retCode = CHECKSEND_RETURN_FAILED;
				};
		};
		/*
		const BYTE PARA_SCENE_CHECKRETURN = 301;
		struct t_checkReturn_SceneSession : t_NullCmd
		{
			mailInfo mail;
			DWORD itemID;
			BYTE retCode;
			t_checkReturn_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_CHECKRETURN) 
				{
					bzero(&mail, sizeof(mail));
					itemID = 0;
					retCode = CHECKSEND_RETURN_FAILED;
				};
		};
		*/

		struct SessionObject
		{
			time_t createtime;
			t_Object object;
		};
		const BYTE PARA_SCENE_SENDMAIL = 181;
		struct t_sendMail_SceneSession : t_NullCmd
		{
			mailInfo mail;
			SessionObject item;
			t_sendMail_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_SENDMAIL) 
				{
					bzero(&mail, sizeof(mail)+sizeof(SessionObject));
				};
		};

		/*
		enum
		{
			SENDMAIL_RETURN_OK,
			SENDMAIL_RETURN_FAILED,
			SENDMAIL_RETURN_NOPLAYER
		};
		const BYTE PARA_SCENE_SENDMAIL_RETURN = 303;
		struct t_sendMailReturn_SceneSession : t_NullCmd
		{
			BYTE retCode;
			t_sendMailReturn_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_SENDMAIL_RETURN) 
				{
					retCode = SENDMAIL_RETURN_FAILED;
				};
		};
		*/

		const BYTE PARA_SCENE_GET_MAIL_LIST = 182;
		struct t_getMailList_SceneSession : t_NullCmd
		{
			DWORD tempID;
			t_getMailList_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_GET_MAIL_LIST) 
				{
					tempID = 0;
				};
		};

		const BYTE PARA_SCENE_OPEN_MAIL = 183;
		struct t_openMail_SceneSession : t_NullCmd
		{
			DWORD tempID;
			DWORD mailID;
			t_openMail_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_OPEN_MAIL) 
				{
					tempID = 0;
					mailID = 0;
				};
		};

		const BYTE PARA_SCENE_GET_MAIL_ITEM = 184;
		struct t_getMailItem_SceneSession : t_NullCmd
		{
			DWORD tempID;
			DWORD mailID;
			WORD space;
			DWORD money;
			DWORD gold;
			t_getMailItem_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_GET_MAIL_ITEM) 
				{
					tempID = 0;
					mailID = 0;
					space = 0;
					money = 0;
					gold = 0;
				};
		};

		const BYTE PARA_SCENE_GET_MAIL_ITEM_RETURN = 185;
		struct t_getMailItemReturn_SceneSession : t_NullCmd
		{
			DWORD userID;
			DWORD mailID;
			DWORD sendMoney;
			DWORD recvMoney;
			DWORD sendGold;
			DWORD recvGold;
			SessionObject item;
			t_getMailItemReturn_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_GET_MAIL_ITEM_RETURN) 
				{
					userID = 0;
					mailID = 0;
					sendMoney = 0;
					recvMoney = 0;
					sendGold = 0;
					recvGold = 0;
					bzero(&item, sizeof(SessionObject));
				};
		};

		const BYTE PARA_SCENE_GET_MAIL_ITEM_CONFIRM = 186;
		struct t_getMailItemConfirm_SceneSession : t_NullCmd
		{
			DWORD userID;
			DWORD mailID;
			t_getMailItemConfirm_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_GET_MAIL_ITEM_CONFIRM) 
				{
					userID = 0;
					mailID = 0;
				};
		};

		const BYTE PARA_SCENE_DEL_MAIL = 187;
		struct t_delMail_SceneSession : t_NullCmd
		{
			DWORD tempID;
			DWORD mailID;
			t_delMail_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_DEL_MAIL) 
				{
					tempID = 0;
					mailID = 0;
				};
		};

		const BYTE PARA_SCENE_CHECK_NEW_MAIL = 188;
		struct t_checkNewMail_SceneSession : t_NullCmd
		{
			DWORD userID;
			t_checkNewMail_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_CHECK_NEW_MAIL) 
				{
					userID = 0;
				};
		};

		const BYTE PARA_SCENE_TURN_BACK_MAIL = 189;
		struct t_turnBackMail_SceneSession : t_NullCmd
		{
			DWORD userID;
			DWORD mailID;
			t_turnBackMail_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_TURN_BACK_MAIL) 
				{
					userID = 0;
				};
		};
		//////////////////////////////////////////////////////////////
		///  场景服务器之邮件指令结束
		//////////////////////////////////////////////////////////////

		//从session发起的怪物攻城
		const BYTE PARA_SCENE_CREATE_RUSH = 198;
		struct t_createRush_SceneSession : t_NullCmd
		{
			DWORD rushID;
			DWORD delay;
			DWORD countryID;
			t_createRush_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_CREATE_RUSH) 
				{
					rushID = 0;
					delay = 0;
					countryID = 0;
				};
		};

		//////////////////////////////////////////////////////////////
		///  场景服务器之拍卖指令开始
		//////////////////////////////////////////////////////////////
		enum auctionState
		{
			AUCTION_STATE_NEW	= 1,
			AUCTION_STATE_DEAL	= 2,
			AUCTION_STATE_CANCEL	= 3,
			AUCTION_STATE_TIMEOVER	= 4,
			AUCTION_STATE_DEL	= 5
		};

		struct auctionInfo
		{
			DWORD ownerID;
			char owner[MAX_NAMESIZE+1];
			BYTE state;
			DWORD charge;
			DWORD itemID;
			char itemName[MAX_NAMESIZE+1];
			BYTE type;
			BYTE quality;
			WORD needLevel;
			DWORD minMoney;
			DWORD maxMoney;
			DWORD minGold;
			DWORD maxGold;
			DWORD startTime;
			DWORD endTime;
			BYTE bidType;
		};

		const BYTE PARA_AUCTION_CMD = 199;
		struct t_AuctionCmd : t_NullCmd
		{
			BYTE auctionPara;
			t_AuctionCmd()
				: t_NullCmd(CMD_SCENE, PARA_AUCTION_CMD) 
				{
					auctionPara = 0;
				};
		};

		const BYTE PARA_AUCTION_SALE = 1;
		struct t_saleAuction_SceneSession : t_AuctionCmd
		{
			DWORD userID;//临时ID
			auctionInfo info;
			SessionObject item;
			t_saleAuction_SceneSession()
				: t_AuctionCmd()
				{
					auctionPara = PARA_AUCTION_SALE;

					bzero(&info, sizeof(info));
					bzero(&item, sizeof(item));
				};
		};

		const BYTE PARA_AUCTION_CHECK_BID = 2;
		struct t_checkBidAuction_SceneSession : t_AuctionCmd
		{
			DWORD userID;//临时ID
			DWORD auctionID;
			DWORD money;
			DWORD gold;
			BYTE bidType;
			t_checkBidAuction_SceneSession()
				: t_AuctionCmd()
				{
					auctionPara = PARA_AUCTION_CHECK_BID;

					userID = 0;
					auctionID = 0;
					money = 0;
					gold = 0;
					bidType = 0;
				};
		};

		const BYTE PARA_AUCTION_BID = 3;
		struct t_bidAuction_SceneSession : t_AuctionCmd
		{
			DWORD userID;//临时ID
			DWORD auctionID;
			DWORD money;
			DWORD gold;
			t_bidAuction_SceneSession()
				: t_AuctionCmd()
				{
					auctionPara = PARA_AUCTION_BID;

					userID = 0;
					auctionID = 0;
					money = 0;
					gold = 0;
				};
		};

		const BYTE PARA_AUCTION_QUERY = 4;
		struct t_queryAuction_SceneSession : t_AuctionCmd
		{
			DWORD userID;//临时ID
			BYTE type;
			char name[MAX_NAMESIZE];
			BYTE quality;
			WORD level;
			WORD page;
			t_queryAuction_SceneSession()
				: t_AuctionCmd()
				{
					auctionPara = PARA_AUCTION_QUERY;

					userID = 0;
					type = 0;
					bzero(name, sizeof(name));
					quality = 0;
					level = 0;
					page = 0;
				};
		};

		const BYTE PARA_AUCTION_CHECK_CANCEL = 5;
		struct t_checkCancelAuction_SceneSession : t_AuctionCmd
		{
			DWORD userID;//临时ID
			DWORD auctionID;
			DWORD charge;
			t_checkCancelAuction_SceneSession()
				: t_AuctionCmd()
				{
					auctionPara = PARA_AUCTION_CHECK_CANCEL;

					userID = 0;
					auctionID = 0;
					charge = 0;
				};
		};

		const BYTE PARA_AUCTION_CANCEL = 6;
		struct t_cancelAuction_SceneSession : t_AuctionCmd
		{
			DWORD userID;//临时ID
			DWORD auctionID;
			DWORD charge;
			t_cancelAuction_SceneSession()
				: t_AuctionCmd()
				{
					auctionPara = PARA_AUCTION_CANCEL;

					userID = 0;
					auctionID = 0;
					charge = 0;
				};
		};

		const BYTE PARA_AUCTION_GET_LIST = 7;
		struct t_getListAuction_SceneSession : t_AuctionCmd
		{
			DWORD userID;//临时ID
			BYTE list;
			t_getListAuction_SceneSession()
				: t_AuctionCmd()
				{
					auctionPara = PARA_AUCTION_GET_LIST;

					userID = 0;
					list = 0;
				};
		};

		/*
		const BYTE PARA_AUCTION_BID_LIST = 7;
		struct t_bidListAuction_SceneSession : t_AuctionCmd
		{
			DWORD userID;//临时ID
			DWORD bidList[100];
			t_bidListAuction_SceneSession()
				: t_AuctionCmd()
				{
					auctionPara = PARA_AUCTION_BID_LIST;

					bzero(&bidList[0], sizeof(bidList));
				};
		};
		*/

		//////////////////////////////////////////////////////////////
		///  场景服务器之拍卖指令结束
		//////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////
		/// 场景服务器之临时档案
		//////////////////////////////////////////////////////////////
		const BYTE PARA_USER_ARCHIVE_REQ = 200;
		struct t_ReqUser_SceneArchive : t_NullCmd
		{
			DWORD id;						/// 角色
			DWORD dwMapTempID;				/// 地图临时ID
			t_ReqUser_SceneArchive()
				: t_NullCmd(CMD_SCENE, PARA_USER_ARCHIVE_REQ) 
				{
					id = 0;
					dwMapTempID = 0;
				};
		};
		const BYTE PARA_USER_ARCHIVE_READ = 201;
		struct t_ReadUser_SceneArchive : t_NullCmd
		{
			DWORD id;						/// 角色
			DWORD dwMapTempID;				/// 地图临时ID
			DWORD dwSize;
			char data[0];
			t_ReadUser_SceneArchive()
				: t_NullCmd(CMD_SCENE, PARA_USER_ARCHIVE_READ) 
				{
					id = 0;
					dwMapTempID = 0;
					dwSize = 0;
				};
		};
		const BYTE PARA_USER_ARCHIVE_WRITE = 202;
		struct t_WriteUser_SceneArchive : t_NullCmd
		{
			DWORD id;						/// 角色
			DWORD dwMapTempID;				/// 地图临时ID
			DWORD dwSize;
			char data[0];
			t_WriteUser_SceneArchive()
				: t_NullCmd(CMD_SCENE, PARA_USER_ARCHIVE_WRITE) 
				{
					id = 0;
					dwMapTempID = 0;
					dwSize = 0;
				};
		};
		//////////////////////////////////////////////////////////////
		/// 场景服务器之临时档案
		//////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////
		/// 场景服务器之队伍指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_USER_TEAM_ADDMEMBER = 203;
		struct t_Team_AddMember : t_NullCmd
		{
			DWORD dwLeaderID;						/// 角色
			DWORD dwMemberID;						/// 角色
			t_Team_AddMember()
				: t_NullCmd(CMD_SCENE, PARA_USER_TEAM_ADDMEMBER) 
				{
					dwLeaderID = 0;
					dwMemberID = 0;
				};
		};
		const BYTE PARA_USER_TEAM_DELMEMBER = 204;
		struct t_Team_DelMember : t_NullCmd
		{
			DWORD dwLeaderID;						/// 角色
			DWORD dwMemberID;						/// 角色
			t_Team_DelMember()
				: t_NullCmd(CMD_SCENE, PARA_USER_TEAM_DELMEMBER) 
				{
					dwLeaderID = 0;
					dwMemberID = 0;
				};
		};
		const BYTE PARA_USER_TEAM_REQUEST = 205;
		struct t_Team_Request : t_NullCmd
		{
			DWORD dwUserID;						/// 角色
			t_Team_Request()
				: t_NullCmd(CMD_SCENE, PARA_USER_TEAM_REQUEST) 
				{
					dwUserID = 0;
				};
		};
		const BYTE PARA_USER_TEAM_DATA = 206;
		struct t_Team_Data : t_NullCmd
		{
			DWORD dwLeaderID;						/// 角色
			WORD dwSize;						/// 队伍人数
			DWORD dwMember[0];				/// 队员列表
			t_Team_Data()
				: t_NullCmd(CMD_SCENE, PARA_USER_TEAM_DATA) 
				{
					dwLeaderID = 0;
					dwSize = 0;
				};
		};
		//////////////////////////////////////////////////////////////
		/// 场景服务器之队伍指令
		//////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////
		///  场景服务器之对战指令开始
		//////////////////////////////////////////////////////////////
				
		// 由会话发向场景，通知其在某玩家上进行竞赛奖励
		const BYTE  PARA_QUIZ_AWARD = 214;
		struct t_quizAward_SceneSession : t_NullCmd
		{
			DWORD dwUserID;			/// 角色ID
			DWORD dwExp;			/// 奖励经验
			DWORD dwMoney;			/// 奖励银两
			DWORD dwGrace;			/// 本次所获文采点数
			BYTE  byType;			/// 竞赛类型,默认为全国
			
			t_quizAward_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_QUIZ_AWARD) 
				{
					byType = 0;
				};
		};

		
		// 改变国籍，场景通知会话
		const BYTE  PARA_CHANGE_COUNTRY = 215;
		struct t_changeCountry_SceneSession : t_NullCmd
		{
			DWORD dwUserID;			/// 角色ID
			DWORD dwToCountryID;		/// 改变到的国家
			t_changeCountry_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_CHANGE_COUNTRY) {};
		};

		// 改变国籍，会话通知场景
		const BYTE  PARA_RETURN_CHANGE_COUNTRY = 216;
		struct t_returnChangeCountry_SceneSession : t_NullCmd
		{
			DWORD dwUserID;			/// 角色ID

			t_returnChangeCountry_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_RETURN_CHANGE_COUNTRY) {};
		};

		// 会话发向场景，让玩家跳到战场所在城市
		const BYTE PARA_TRANS_DARE_COUNTRY = 217;
		struct t_transDareCountry_SceneSession : t_NullCmd
		{
			DWORD dwUserID;	/// 角色ID
			DWORD dwMoney;	/// 需要扣除金额
			DWORD dwCountry; /// 要去的国家ID
			
			t_transDareCountry_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_TRANS_DARE_COUNTRY){};
		};

		// 由场景发往会话，确认该人能否跳往指定城市的帮会领地
		const BYTE PARA_ENTER_UNION_MANOR = 218;
		struct t_enterUnionManor_SceneSession : t_NullCmd
		{
			DWORD dwUserID; // 申请人的ID
			DWORD dwCountryID; // 跳去的国家ID
			DWORD dwCityID;	// 所在城市ID

			t_enterUnionManor_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_ENTER_UNION_MANOR) {};
		};
		
		// 由会话发往场景，让玩家跳到自己帮会的属地
		const BYTE PARA_RETURN_ENTER_UNION_MANOR = 219;
		struct t_returnEnterUnionManor_SceneSession : t_NullCmd
		{
			DWORD dwUserID;     // 申请人的ID
			DWORD dwCountryID;  // 要跳去的国家ID
			DWORD dwAreaID;     // 属地ID:只含有计算出来的REALID，需要场景通过COUNTRYID进行计算

			t_returnEnterUnionManor_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_RETURN_ENTER_UNION_MANOR) {};
		};
		
		// 由会话发往场景,清除帮会领地里非帮会成员
		const BYTE PARA_CLEAR_UNION_MANOR = 220;
		struct t_clearUnionManor_SceneSession : t_NullCmd
		{
			DWORD dwUnionID;	// 该场景上的所有人，除了指定帮会的人，其余全relive
			DWORD dwCountryID;  // 要跳去的国家ID
			DWORD dwAreaID;     // 属地ID:只含有计算出来的REALID，需要场景通过COUNTRYID进行计算

			t_clearUnionManor_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_CLEAR_UNION_MANOR) {};
		};
		
		// 由场景发往会话，查询城市所属
		const BYTE PARA_QUESTION_UNION_CITY = 221;
		struct t_questionUnionCity_SceneSession : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwCountryID;
			DWORD dwCityID;
			BYTE  byType;

			t_questionUnionCity_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_QUESTION_UNION_CITY) 
				{
					dwUserID = 0;
					dwCountryID = 0;
					dwCityID = 0;
					byType = 0;
				};
		};
		
				
		
		// 由场景发往会话，通知其用户数据变更
		const BYTE PARA_CHANGE_USER_DATA = 224;
		struct t_changeUserData_SceneSession : t_NullCmd
		{
			WORD  wdLevel;	    // 类型，0为接受挑战的，1可接受挑战的,3,所有国家挑战信息
			DWORD dwExploit;    // 功勋值
			DWORD dwGrace;	    // 文采值
			DWORD dwUserID;	    // 玩家ID
			
			t_changeUserData_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_CHANGE_USER_DATA) {};
		};
		
		// 通知场景帮会战开始和结束
		const BYTE PARA_UNION_DARE_NOTIFY = 225;
		struct t_unionDareNotify_SceneSession : t_NullCmd
		{
			DWORD sceneID;
			BYTE state;//1 开始 0 结束

			t_unionDareNotify_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_UNION_DARE_NOTIFY) {};
		};
		
		// 会话发往场景, 师傅领取进贡
		const BYTE PICKUP_MASTER_SCENE_PARA = 226;
		struct t_PickupMaster_SceneSession : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwMoney;

			t_PickupMaster_SceneSession()
				: t_NullCmd(CMD_SCENE, PICKUP_MASTER_SCENE_PARA)
				{
					dwUserID = 0;
					dwMoney = 0;
				}
		};

		//////////////////////////////////////////////////////////////
		///  场景服务器之对战指令结束
		//////////////////////////////////////////////////////////////
	
		/************************************************************
		***********场景服务器之任务相关开始************
		************************************************************/
		// 公告      
		const BYTE QUEST_BULLETIN_USERCMD_PARA = 240;
		struct t_QuestBulletinUserCmd : public t_NullCmd
		{
			t_QuestBulletinUserCmd(): t_NullCmd(CMD_SCENE, QUEST_BULLETIN_USERCMD_PARA)
			{
			}

			WORD kind; // 0: reserve 1: tong 2 : family
			DWORD id; //family id or tong id
			char name[MAX_NAMESIZE+1];//soke 名字
			char content[MAX_CHATINFO]; //chat msg
		};

		const BYTE QUEST_CHANGE_AP = 241;
		struct t_ChangeAP : public t_NullCmd //更改行动力
		{
			t_ChangeAP(): t_NullCmd(CMD_SCENE, QUEST_CHANGE_AP)
			{
			}
			DWORD id; 
			int point; 
		};

		const BYTE QUEST_CHANGE_RP = 242; //更改威望
		struct t_ChangeRP : public t_NullCmd
		{
			t_ChangeRP(): t_NullCmd(CMD_SCENE, QUEST_CHANGE_RP)
			{
			}
			WORD kind; // 1: tong 2 :fam
			int point; 
		};
		 
		//soke 2016 - 3 - 10 寻找师父,分配银子
		const BYTE OVERMAN_MONEY_ADD = 243; 
		struct t_OvermanMoneyAdd : public t_NullCmd
		{
			t_OvermanMoneyAdd(): t_NullCmd(OVERMAN_MONEY_ADD, QUEST_CHANGE_RP)
			{
			}
			DWORD id;//	师父id
			DWORD money;//soke 应该得到的银子
			char name[MAX_NAMESIZE+1];//徒弟名字
		};
		
		/************************************************************
		***********场景服务器之任务相关结束************
		************************************************************/
		
	//////////////////////////////////////////////////////////////
		///  场景服务器之新的指令开始
		//////////////////////////////////////////////////////////////
		//战力榜奖励通知场景 CCC
		struct stZhanliRankReward
		{
			char name[MAX_NAMESIZE];
			DWORD charID;
		};
		const BYTE PARA_SCENE_ZHANLI_RANK_REWARD = 160;  
		struct t_sendZhanliRankReward: t_NullCmd
		{
			//std::vector<char [MAX_NAMESIZE]> name;
			DWORD len;
			stZhanliRankReward zhanliName[0];
			t_sendZhanliRankReward()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_ZHANLI_RANK_REWARD) {
					len = 0;
				};
		};
		//护国榜指令
		const BYTE PARA_HEOR_KILL_UESR = 161;
		struct t_heorKill_SceneSession : t_NullCmd
		{
			char name[MAX_NAMESIZE];
			DWORD charID;
			DWORD country;
			DWORD kCharID;
			t_heorKill_SceneSession()
				:	t_NullCmd(CMD_SCENE, PARA_HEOR_KILL_UESR) {}
		};
		//英雄排行榜奖励 CCC
		struct stHeroRankReward
		{
			char name[MAX_NAMESIZE];
			DWORD charID;
		};
		const BYTE PARA_SCENE_HERO_RANK_REWARD = 162;  
		struct t_sendHeroliRankReward: t_NullCmd
		{
			//std::vector<char [MAX_NAMESIZE]> name;
			DWORD len;
			stHeroRankReward heroName[0];
			t_sendHeroliRankReward()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_HERO_RANK_REWARD) {
					len = 0;
				};
		};
        //积分榜奖励通知场景 CCC
		struct stTicketRankReward
		{
			char name[MAX_NAMESIZE];
			DWORD charID;
		};
		const BYTE PARA_SCENE_TICKET_RANK_REWARD= 163;  
		struct t_sendTicketRankReward: t_NullCmd
		{
			//std::vector<char [MAX_NAMESIZE]> name;
			DWORD len;
			stTicketRankReward ticketName[0];
			t_sendTicketRankReward()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_TICKET_RANK_REWARD) {
					len = 0;
				};
		};
		//鲜花榜奖励通知场景 CCC
		struct stFlowerRankReward
		{
			char name[MAX_NAMESIZE];
			DWORD charID;
		};
		const BYTE PARA_SCENE_FLOWER_RANK_REWARD = 164;  
		struct t_sendFlowerRankReward: t_NullCmd
		{
			//std::vector<char [MAX_NAMESIZE]> name;
			DWORD len;
			stFlowerRankReward flowerName[0];
			t_sendFlowerRankReward()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_FLOWER_RANK_REWARD) {
					len = 0;
				};
		};

		const BYTE PARA_SCENE_GONGXUN_RANK_REWARD = 165;  
		struct t_sendGongxunRankReward: t_NullCmd
		{

			char name[MAX_NAMESIZE+1];
			DWORD charID;
			DWORD itemId;
			DWORD itemNum;
			DWORD itemLevel;
			DWORD beishu;
			t_sendGongxunRankReward()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_GONGXUN_RANK_REWARD) {};
		};

		const BYTE PARA_SCENE_GONGXUN_RANK_REWARD2 = 166;  
		struct t_sendGongxunRankReward2: t_NullCmd
		{
			DWORD charID;
			t_sendGongxunRankReward2()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_GONGXUN_RANK_REWARD2) {};
		};

		//战车系统
		const BYTE PARA_SCENE_ZHANCHELINGYONG_RANK_REWARD = 167;  
		struct t_sendZhancheLingYongRankReward: t_NullCmd
		{
			DWORD charID;
			DWORD state;
			DWORD zhancheid;
			DWORD zhanchetype;
			t_sendZhancheLingYongRankReward()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_ZHANCHELINGYONG_RANK_REWARD) {};
		};
		//战车归还
		const BYTE PARA_SCENE_ZHANCHEGUIHUAN_RANK_REWARD = 168;  
		struct t_sendZhancheGuihuanRankReward: t_NullCmd
		{
			DWORD charID;
			DWORD zhancheid;
			t_sendZhancheGuihuanRankReward()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_ZHANCHEGUIHUAN_RANK_REWARD) {};
		};
		//召唤家族BOSS
		const BYTE PARA_SCENE_ZHAOHUANJIAZUBOSS_RANK_REWARD = 169;  
		struct t_sendZhaoHuanJiazuBossRankReward: t_NullCmd
		{
			DWORD charID;
			DWORD level;
			t_sendZhaoHuanJiazuBossRankReward()
				: t_NullCmd(CMD_SCENE, PARA_SCENE_ZHAOHUANJIAZUBOSS_RANK_REWARD) {};
		};
		//////////////////////////////////////////////////////////////
		///  场景服务器之新的指令结束
		//////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////
		///  GM维护指令
		//////////////////////////////////////////////////////////////
		// GM停机维护指令
		const BYTE PARA_SHUTDOWN = 1;
		struct t_shutdown_SceneSession : t_NullCmd
		{
			time_t time;
			char info[MAX_CHATINFO];
			t_shutdown_SceneSession()
				: t_NullCmd(CMD_SCENE_SHUTDOWN, PARA_SHUTDOWN) {
					bzero(&time,sizeof(time));
					bzero(info,sizeof(info));
				};
		};
		//////////////////////////////////////////////////////////////
		///  GM维护指令结束
		//////////////////////////////////////////////////////////////



		//////////////////////////////////////////////////////////////
		/// 场景服务器指令
		//////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////
		/// 网关服务器指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_GATE_REGUSER = 1;
		struct t_regUser_GateSession : t_NullCmd
		{
			DWORD accid;
			DWORD dwID;
			DWORD dwTempID;
			DWORD dwMapID;
			WORD wdLevel;
			WORD wdOccupation;
			WORD wdCountry;
			BYTE byCountryName[MAX_NAMESIZE+1];
			BYTE byName[MAX_NAMESIZE+1];
			BYTE byMapName[MAX_NAMESIZE+1];
			t_regUser_GateSession()
				: t_NullCmd(CMD_GATE, PARA_GATE_REGUSER) {};
		};

		const BYTE PARA_GATE_UNREGUSER = 2;
		struct t_unregUser_GateSession : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwSceneTempID;
			BYTE retcode;
			t_unregUser_GateSession()
				: t_NullCmd(CMD_GATE, PARA_GATE_UNREGUSER) {};
		};
		const BYTE PARA_GATE_CHANGE_SCENE_USER = 3;
		struct t_changeUser_GateSession : t_NullCmd
		{
			DWORD accid;
			DWORD dwID;
			DWORD dwTempID;
			BYTE byName[MAX_NAMESIZE+1];
			BYTE byMapFileName[MAX_NAMESIZE+1];
			///////////////////////////////////
			//soke太庙资源数量显示
            DWORD scnTempID;
			////////////////////////////////////
			t_changeUser_GateSession()
				: t_NullCmd(CMD_GATE, PARA_GATE_CHANGE_SCENE_USER) 
            {
			//////////////////////////////////////////////
                accid = 0;
                dwID = 0;
                dwTempID = 0;
                bzero( byName, sizeof(byName) );
                bzero( byMapFileName, sizeof(byMapFileName) );
                scnTempID = 0;
			///////////////////////////////////////////////	
            };
		};

		/// 请求国家在线人数排序
		const BYTE REQUEST_GATE_COUNTRY_ORDER = 5;
		struct t_request_Country_GateSession : t_NullCmd
		{
			t_request_Country_GateSession()
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
		/// 国家在线人数排序
		const BYTE PARA_GATE_COUNTRY_ORDER = 6;
		struct t_order_Country_GateSession : t_NullCmd
		{
			DWORD dwID;					//id
			CountrOrder order;			//排序的国家 
			t_order_Country_GateSession()
				: t_NullCmd(CMD_GATE, PARA_GATE_COUNTRY_ORDER) {};
		};

		///直接转发到用户的指令
		const BYTE PARA_GATE_FORWARD_USER = 7;
		struct t_forwardUser_GateSession : t_NullCmd
		{
			DWORD id;
			DWORD tempid;
			char name[MAX_NAMESIZE];
			DWORD cmd_len;
			BYTE cmd[0];
			t_forwardUser_GateSession()
				: t_NullCmd(CMD_GATE, PARA_GATE_FORWARD_USER)
				{
					id = 0;
					tempid = 0;
					bzero(name, MAX_NAMESIZE);
				}
		};

		const BYTE PARA_UNION_DISBAND = 100;
		struct t_disbandUnion_GateSession : t_NullCmd
		{
			DWORD dwCharID;
			DWORD dwUnionID;
			t_disbandUnion_GateSession()
				: t_NullCmd(CMD_GATE, PARA_UNION_DISBAND) {};
		};
		const BYTE PARA_SEPT_DISBAND = 101;
		struct t_disbandSept_GateSession : t_NullCmd
		{
			DWORD dwCharID;
			DWORD dwSeptID;
			t_disbandSept_GateSession()
				: t_NullCmd(CMD_GATE, PARA_SEPT_DISBAND) {};
		};
		
		const BYTE PARA_GATE_DELCHAR = 102;
		struct t_DelChar_GateSession : t_NullCmd
		{
			DWORD accid;						/// 账号
			DWORD id;							/// 角色编号
			char name[MAX_NAMESIZE+1];
			DWORD status;
			t_DelChar_GateSession()
				: t_NullCmd(CMD_GATE, PARA_GATE_DELCHAR) {};
		};		

		const BYTE PARA_EXIT_QUIZ = 103;
		struct t_exitQuiz_GateSession : t_NullCmd
		{
			DWORD dwUserID;
			BYTE  type; // type=0，退出.type=1,进入

			t_exitQuiz_GateSession()
				: t_NullCmd(CMD_GATE, PARA_EXIT_QUIZ)
				{
					type = 0;
				};
		};
		
		//////////////////////////////////////////////////////////////
		/// 查询类指令
		//////////////////////////////////////////////////////////////

		const BYTE PARA_QUESTION_OBJECT = 244;
		struct t_questionObject_SceneSession  : t_NullCmd
		{
			char from_name[MAX_NAMESIZE+1];  // 物品拥有者姓名
			char to_name[MAX_NAMESIZE+1];    // 物品查询者姓名
			DWORD dwObjectTempID;
			
			t_questionObject_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_QUESTION_OBJECT) {};
		};		

		const BYTE PARA_RETURN_OBJECT = 245;
		struct t_returnObject_SceneSession  : t_NullCmd
		{
			char from_name[MAX_NAMESIZE+1]; // 物品拥有者姓名
			char to_name[MAX_NAMESIZE+1];   // 物品查询者姓名

			t_Object object;
			
			t_returnObject_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_RETURN_OBJECT) {};
		};		

		const BYTE PARA_CLOSE_NPC = 246;
		struct t_CloseNpc_SceneSession  : t_NullCmd
		{
			t_CloseNpc_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_CLOSE_NPC) {};
		};

		const BYTE PARA_DEBUG_COUNTRYPOWER = 247;
		struct t_DebugCountryPower_SceneSession  : t_NullCmd
		{
			t_DebugCountryPower_SceneSession()
				: t_NullCmd(CMD_SCENE, PARA_DEBUG_COUNTRYPOWER) {};
		};

		//////////////////////////////////////////////////////////////
		/// 网关服务器指令
		//////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////
		/// 会话服务器转发指令
		//////////////////////////////////////////////////////////////
		/// 网关到Session
		const BYTE PARA_FORWARD_USER = 1;
		struct t_Session_ForwardUser : t_NullCmd
		{
			DWORD dwID;
			WORD  size;
			BYTE  data[0];
			t_Session_ForwardUser()
				: t_NullCmd(CMD_FORWARD, PARA_FORWARD_USER) {};
		};
		/// Session到全世界
		const BYTE PARA_FORWARD_WORLD = 2;
		struct t_Session_ForwardWorld : t_NullCmd
		{
			WORD  size;
			BYTE  data[0];
			t_Session_ForwardWorld()
				: t_NullCmd(CMD_FORWARD, PARA_FORWARD_WORLD) {};
		};
		/// Session到国家
		const BYTE PARA_FORWARD_COUNTRY = 3;
		struct t_Session_ForwardCountry : t_NullCmd
		{
			DWORD dwCountry;
			WORD  size;
			BYTE  data[0];
			t_Session_ForwardCountry()
				: t_NullCmd(CMD_FORWARD, PARA_FORWARD_COUNTRY) {};
		};
		//////////////////////////////////////////////////////////////
		/// 会话服务器转发指令
		//////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////
		/// 会话服务器指令
		//////////////////////////////////////////////////////////////
		// 会话服务器向Gateway发送黑名单管理消息。
		enum{
			BLACK_LIST_REMOVE,
			BLACK_LIST_ADD
		};
		const BYTE HANDLE_BLACK_LIST_PARA = 1;
		struct t_Session_HandleBlackList : t_NullCmd
		{
			DWORD dwID;
			char name[MAX_NAMESIZE+1];
			BYTE byOper;   // 0为删除 1为增加
			t_Session_HandleBlackList()
				: t_NullCmd(CMD_SESSION, HANDLE_BLACK_LIST_PARA) {};
		};
		//////////////////////////////////////////////////////////////
		/// 会话服务器指令
		//////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////
		/// GM工具指令
		//////////////////////////////////////////////////////////////

		const BYTE MSGTIME_GMTOL_PARA = 1;
		struct t_MsgTime_GmTool : t_NullCmd
		{
			DWORD id;
			DWORD time;
			t_MsgTime_GmTool()
				: t_NullCmd(CMD_GMTOOL, MSGTIME_GMTOL_PARA)
				{
					id = 0;
					time = 0;
				}
		};

		//////////////////////////////////////////////////////////////
		/// GM工具指令
		//////////////////////////////////////////////////////////////
		
		//////////////////////////////////////////////////////////////
		///  临时指令开始
		//////////////////////////////////////////////////////////////
		// 临时指令
		// 场景发往会话，清除所有帮会，清除所有家族成员，以适应新的家族帮会合并模式
		const BYTE CLEARRELATION_PARA = 1;
		struct t_ClearRelation_SceneSession : t_NullCmd
		{
			DWORD dwUserID;
			
			t_ClearRelation_SceneSession()
				: t_NullCmd(CMD_SCENE_TMP, CLEARRELATION_PARA)
				{
					dwUserID = 0;
				}
		};

		// 会话发往场景，获取一个天羽令
		const BYTE GET_CREATE_UNION_ITEM_PARA = 2;
		struct t_GetCreateUnionItem_SceneSession : t_NullCmd
		{
			DWORD dwUserID; // 给指定的玩家
			
			t_GetCreateUnionItem_SceneSession()
				: t_NullCmd(CMD_SCENE_TMP, GET_CREATE_UNION_ITEM_PARA)
				{
					dwUserID = 0;
				}
		};
		
		// 场景发往会话，返回一个天羽令
		const BYTE RETURN_CREATE_UNION_ITEM_PARA = 3;
		struct t_ReturnCreateUnionItem_SceneSession : t_NullCmd
		{
			DWORD dwUserID; // 给指定的玩家
			SessionObject item;

			t_ReturnCreateUnionItem_SceneSession()
				: t_NullCmd(CMD_SCENE_TMP, RETURN_CREATE_UNION_ITEM_PARA)
				{
				}
		};

		//////////////////////////////////////////////////////////////
		///  临时指令结束
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		///  家族指令开始
		//////////////////////////////////////////////////////////////
		// 场景发往会话，增加声望
		const BYTE OP_REPUTE_PARA = 1;
		struct t_OpRepute_SceneSession : t_NullCmd
		{
			DWORD dwSeptID; // 家族ID
			int dwRepute; //  声望值
			
			t_OpRepute_SceneSession()
				: t_NullCmd(CMD_SCENE_SEPT, OP_REPUTE_PARA)
				{
					dwSeptID = 0;
					dwRepute = 0;
				}
		};
		
		// 场景发往会话，更改家族等级
		const BYTE OP_LEVEL_PARA = 2;
		struct t_OpLevel_SceneSession : t_NullCmd
		{
			DWORD dwSeptID; // 家族ID
			int   dwLevel; //  家族值
			
			t_OpLevel_SceneSession()
				: t_NullCmd(CMD_SCENE_SEPT, OP_LEVEL_PARA)
				{
					dwSeptID = 0;
					dwLevel = 0;
				}
		};
		
		// 会话发往场景，提取家族经验
		const BYTE GET_SEPT_EXP_PARA = 3;
		struct t_GetSeptExp_SceneSession : t_NullCmd
		{
			DWORD dwSeptID; // 家族ID
			DWORD dwUserID; // 族长ID
			
			t_GetSeptExp_SceneSession()
				: t_NullCmd(CMD_SCENE_SEPT, GET_SEPT_EXP_PARA)
				{
					dwSeptID = 0;
					dwUserID = 0;
				}
		};

		// 会话发往场景，更新有关家族的普通信息，不用存档
		const BYTE SEND_SEPT_NORMAL_PARA = 4;
		struct t_SendSeptNormal_SceneSession : t_NullCmd
		{
			DWORD dwUserID; // 族长ID
			DWORD dwRepute; // 家族声望
			
			t_SendSeptNormal_SceneSession()
				: t_NullCmd(CMD_SCENE_SEPT, SEND_SEPT_NORMAL_PARA)
				{
					dwUserID = 0;
					dwRepute = 0;
				}
		};

		// 会话发往场景，提取家族经验
		const BYTE GET_SEPT_NORMAL_EXP_PARA = 5;
		struct t_GetSeptNormalExp_SceneSession : t_NullCmd
		{
			DWORD dwSeptID; // 家族ID
			DWORD dwUserID; // 族长ID

			t_GetSeptNormalExp_SceneSession()
				: t_NullCmd(CMD_SCENE_SEPT, GET_SEPT_NORMAL_EXP_PARA)
				{
					dwSeptID = 0;
					dwUserID = 0;
				}
		};


		//////////////////////////////////////////////////////////////
		///  家族指令结束
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		///  国家指令
		//////////////////////////////////////////////////////////////
		// 由场景发往会话，查看国家对战信息
		const BYTE PARA_VIEW_COUNTRY_DARE = 1;
		struct t_viewCountryDare_SceneSession : t_NullCmd
		{
			BYTE  byType;	    // 类型，0为接受挑战的，1可接受挑战的,3,所有国家挑战信息
			DWORD dwUserID;	    // 查询者
			
			t_viewCountryDare_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_VIEW_COUNTRY_DARE) {};
		};

		// 由场景发往会话，进行国家捐献
		const BYTE PARA_CONTRIBUTE_COUNTRY = 2;
		struct t_ContributeCountry_SceneSession : t_NullCmd
		{
			BYTE byType; // 0,普通物资，1,丝线,2,矿石,3,矿产,4,木材，5,皮毛,6,草药
			DWORD dwValue;	// 捐献度
			DWORD dwCountry;

			t_ContributeCountry_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_CONTRIBUTE_COUNTRY){};
		};
		
		// 国战结果
		enum
		{
			COUNTRY_FORMAL_DARE,	// 正式国战
			COUNTRY_ANNOY_DARE,	// 骚扰国战
			EMPEROR_DARE,		// 皇城争夺战
			COUNTRY_FORMAL_ANTI_DARE, // 国战反攻
		};
		
		const BYTE PARA_COUNTRY_DARE_RESULT = 3;
		struct t_countryDareResult_SceneSession : t_NullCmd
		{
			DWORD dwAttCountryID;
			DWORD dwDefCountryID;

			char attCountryName[MAX_NAMESIZE];	// 挑战者国家名称
			char defCountryName[MAX_NAMESIZE];	// 防御方国家名称
			DWORD dwAttUserID;			// 打死大将军的玩家ID
			
			BYTE byType;	// 国战类型
			
			t_countryDareResult_SceneSession()
				:	t_NullCmd(CMD_SCENE_COUNTRY, PARA_COUNTRY_DARE_RESULT) {}
		};

		// 由会话发往场景，让设置王城场景为战场
		const BYTE PARA_SET_COUNTRY_WAR = 4;
		struct t_setCountryWar_SceneSession : t_NullCmd
		{
			DWORD dwCountryID;  // 要设置战场的国家ID // 防守方
			//DWORD dwAttCountryID; // 攻击方国家ID
			DWORD dwAreaID;     // 战场
			BYTE  byStatus;	    // 类型，0为退出，1,为进入
			
			t_setCountryWar_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_SET_COUNTRY_WAR) 
				{
				    this->dwCountryID = 0;
				    //this->dwAttCountryID = 0;
				    this->dwAreaID = 0;
				    this->byStatus = 0;
				};
		};
		
		// 由会话发往场景，让符合条件的玩家，跳转到边境
		const BYTE PARA_SEL_TRANS_COUNTRY_WAR = 5;
		struct t_selTransCountryWar_SceneSession : t_NullCmd
		{
			DWORD dwCountryID;  // 要跳转的玩家国家
			DWORD dwLevel;     // 玩家等级条件
			
			t_selTransCountryWar_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_SEL_TRANS_COUNTRY_WAR) {};
		};

		//国王处罚一个玩家
		const BYTE PARA_COUNTRY_PUNISH_USER = 6;
		struct t_countryPunishUser_SceneSession : t_NullCmd
		{
			char name[MAX_NAMESIZE];
			DWORD method;//处罚方式 1禁言 2关监狱

			t_countryPunishUser_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_COUNTRY_PUNISH_USER)
				{
					bzero(name, sizeof(name));
					method = 0;
				};
		};

		struct _techItem
		{
			DWORD dwType;
			DWORD dwLevel;
		};	
		
		// 更新国家科技到场景
		const BYTE PARA_UPDATE_TECH = 7;
		struct t_updateTech_SceneSession : t_NullCmd
		{
			t_updateTech_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_UPDATE_TECH)
				{
					bzero(data, sizeof(data));
					dwCountryID = 0;
				};
			
			DWORD dwCountryID;
			_techItem data[14];
		};

		// 场景发往会话，进行科技投票的启动与结束
		const BYTE PARA_OP_TECH_VOTE = 8;
		struct t_opTechVote_SceneSession : t_NullCmd
		{

			t_opTechVote_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_OP_TECH_VOTE)
				{
					byOpType = 0;
					dwCountryID = 0;
				};

			DWORD dwCountryID;
			BYTE byOpType; //1为启动，0为结束
		};
		
		// 更新领主帮会到场景
		const BYTE PARA_UPDATE_SCENE_UNION = 9;
		struct t_updateSceneUnion_SceneSession : t_NullCmd
		{

			t_updateSceneUnion_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_UPDATE_SCENE_UNION)
				{
					dwSceneID = 0;
					dwUnionID = 0;
				};
			
			DWORD dwSceneID;
			DWORD dwUnionID;
		};

		// 更新战胜国标志
		const BYTE PARA_WINNER_EXP_SCENE_COUNTRY = 10;
		struct t_updateWinnerExp_SceneSession : t_NullCmd
		{

			t_updateWinnerExp_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_WINNER_EXP_SCENE_COUNTRY)
				{
				};
			DWORD countryID;
			bool type;	//false表示结束,true表示开始
		};
		
		struct _allyItem
		{
			DWORD dwCountryID;
			DWORD dwAllyCountryID;
			DWORD dwFriendDegree;
		};	
		
		// 更新国家同盟信息到场景
		const BYTE PARA_UPDATE_ALLY = 11;
		struct t_updateAlly_SceneSession : t_NullCmd
		{
			t_updateAlly_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_UPDATE_ALLY)
				{
					dwSize = 0;
				};
			
			DWORD dwSize;
			_allyItem data[0];
		};

		// 场景到会话,增加国家联盟友好度
		const BYTE PARA_OP_ALLY_FRIENDDEGREE = 12;
		struct t_opAllyFrienddegree_SceneSession : t_NullCmd
		{
			t_opAllyFrienddegree_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_OP_ALLY_FRIENDDEGREE)
				{
					dwCountryID = 0;
					friendDegree = 0;
				};
			
			DWORD dwCountryID;
			int friendDegree;
		};

		// 会话到场景, 触发联盟镖车
		const BYTE PARA_SUMMON_ALLY_NPC = 13;
		struct t_summonAllyNpc_SceneSession : t_NullCmd
		{
			t_summonAllyNpc_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_SUMMON_ALLY_NPC)
				{
					dwCountryID = 0;
				};
			
			DWORD dwCountryID;
		};

		// 场景到会话,当外交车队到达时,增加国家联盟友好度
		const BYTE PARA_ALLY_NPC_CLEAR = 14;
		struct t_allyNpcClear_SceneSession : t_NullCmd
		{
			t_allyNpcClear_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_ALLY_NPC_CLEAR)
				{
					dwCountryID = 0;
				};

			DWORD dwCountryID;
		};

		// 设置皇城占领国(会话到场景)
		const BYTE PARA_SET_EMPEROR_HOLD = 15;
		struct t_setEmperorHold_SceneSession : t_NullCmd
		{
			t_setEmperorHold_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_SET_EMPEROR_HOLD)
				{
					dwCountryID = 0;
				};

			DWORD dwCountryID;
		};

		// 会话到场景, 通知大将军的等级
		const BYTE PARA_REFRESH_GEN = 16;
		struct t_refreshGen_SceneSession : t_NullCmd
		{
			t_refreshGen_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_REFRESH_GEN)
				{
					dwCountryID = 0;
					level = 0;
					exp = 0;
					maxExp = 0;
				};

			DWORD dwCountryID;
			DWORD level;
			DWORD exp;
			DWORD maxExp;
		};

		// 场景到会话,给大将军加经验
		const BYTE PARA_ADD_GEN_EXP = 17;
		struct t_addGenExp_SceneSession : t_NullCmd
		{
			t_addGenExp_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_ADD_GEN_EXP)
				{
					dwCountryID = 0;
					exp = 0;
				};

			DWORD dwCountryID;
			DWORD exp;
		};

		//皇帝处罚一个玩家
		const BYTE PARA_EMPEROR_PUNISH_USER = 18;
		struct t_emperorPunishUser_SceneSession : t_NullCmd
		{
			char name[MAX_NAMESIZE];
			DWORD method;//处罚方式 1禁言 2关监狱

			t_emperorPunishUser_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_EMPEROR_PUNISH_USER)
				{
					bzero(name, sizeof(name));
					method = 0;
				};
		};

		// 检查一个玩家的善恶度
		const BYTE PARA_CHECK_USER = 19;
		struct t_checkUser_SceneSession : t_NullCmd
		{
			t_checkUser_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_CHECK_USER)
				{
					byState   = 0;
					dwCheckID  = 0;
					dwCheckedID = 0;
				};
			
			BYTE byState; // 0, 未通过检查, 1,通过检查
			DWORD dwCheckID; // 发起检查的玩家ID
			DWORD dwCheckedID; // 待检测玩家的ID
		};

		//soke 检查一个玩家的善恶度(巡捕（右）)
		const BYTE PARA_CHECK_USER_CATCHEX = 20;
		struct t_checkUserCatchex_SceneSession : t_NullCmd
		{
			t_checkUserCatchex_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_CHECK_USER_CATCHEX)
				{
					byState   = 0;
					dwCheckID  = 0;
					dwCheckedID = 0;
				};
			
			BYTE byState; // 0, 未通过检查, 1,通过检查
			DWORD dwCheckID; // 发起检查的玩家ID
			DWORD dwCheckedID; // 待检测玩家的ID
		};

		//soke 2016 - 3 - 11 设置外交官状态（停用）
		const BYTE PARA_SET_DIPLOMAT_STATE = 21;
		struct t_setDiplomatState_SceneSession : t_NullCmd
		{
			t_setDiplomatState_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_SET_DIPLOMAT_STATE)
				{
					byState   = 0;
					dwUserID = 0;
				};
			
			BYTE byState; // 0, 取消状态, 1,设置状态
			DWORD dwUserID; // 待设置的玩家ID
		};

        //soke 2016 - 3 - 11 设置巡捕（左上）状态
		const BYTE PARA_SET_CATCHER_STATE = 22;
		struct t_setCatcherState_SceneSession : t_NullCmd
		{
			t_setCatcherState_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_SET_CATCHER_STATE)
				{
					byState   = 0;
					dwUserID = 0;
				};
			
			BYTE byState; // 0, 取消状态, 1,设置状态
			DWORD dwUserID; // 待设置的玩家ID
		};

		//soke 2016 - 3 - 11 设置巡捕（左下）状态
		const BYTE PARA_SET_CATCHEX_STATE = 23;
		struct t_setCatchexState_SceneSession : t_NullCmd
		{
			t_setCatchexState_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_SET_CATCHEX_STATE)
				{
					byState   = 0;
					dwUserID = 0;
				};
			
			BYTE byState; // 0, 取消状态, 1,设置状态
			DWORD dwUserID; // 待设置的玩家ID
		};

        //soke 2016 - 3 - 11 设置御史大夫（右上）状态
		const BYTE PARA_SET_CENSOR_STATE = 24;
		struct t_setCensorState_SceneSession : t_NullCmd
		{
			t_setCensorState_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_SET_CENSOR_STATE)
				{
					byState   = 0;
					dwUserID = 0;
				};
			
			BYTE byState; // 0, 取消状态, 1,设置状态
			DWORD dwUserID; // 待设置的玩家ID
		};
		
		//soke 2016 - 3 - 11 设置御史大夫（右下）状态
		const BYTE PARA_SET_CENSOS_STATE = 25;
		struct t_setCensosState_SceneSession : t_NullCmd
		{
			t_setCensosState_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_SET_CENSOS_STATE)
				{
					byState   = 0;
					dwUserID = 0;
				};
			
			BYTE byState; // 0, 取消状态, 1,设置状态
			DWORD dwUserID; // 待设置的玩家ID
		};
		
		//soke 2016 - 3 - 11 设置元帅状态
		const BYTE PARA_SET_YUANSHUAI_STATE = 26;
		struct t_setYuanshuaiState_SceneSession : t_NullCmd
		{
			t_setYuanshuaiState_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_SET_YUANSHUAI_STATE)
				{
					byState   = 0;
					dwUserID = 0;
				};
			
			BYTE byState; // 0, 取消状态, 1,设置状态
			DWORD dwUserID; // 待设置的玩家ID
		};

		//soke 2016 - 3 - 11 设置宰相状态
		const BYTE PARA_SET_ZAIXIANG_STATE = 27;
		struct t_setZaixiangState_SceneSession : t_NullCmd
		{
			t_setZaixiangState_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_SET_ZAIXIANG_STATE)
				{
					byState   = 0;
					dwUserID = 0;
				};
			
			BYTE byState; // 0, 取消状态, 1,设置状态
			DWORD dwUserID; // 待设置的玩家ID
		};

		const BYTE PARA_COUNTRY_POWER_SORT = 28;
		struct t_countryPowerSort_SceneSession : t_NullCmd
		{
			BYTE country[13];
			t_countryPowerSort_SceneSession() : t_NullCmd(CMD_SCENE_COUNTRY, PARA_COUNTRY_POWER_SORT)
			{
				bzero(country, sizeof(country));
			}
		};

		//soke 场景到会话,场景启动时向会话请求国家砖块数量
		const BYTE PARA_REQ_COUNTRY_LOONG_PEARL_NUM = 29;
		struct ReqCountryLoongPearlNum_SceneSession : t_NullCmd
		{
			ReqCountryLoongPearlNum_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_REQ_COUNTRY_LOONG_PEARL_NUM)
				{
                    serverID = 0;
					countryID = 0;
				};
            DWORD serverID;
			DWORD countryID;
		};

		//soke 会话到场景,会话回复场景国家砖块数量
		const BYTE PARA_REPLY_COUNTRY_PEARL_NUM = 29;
		struct ReplyCountryPearlNum_SceneSession : t_NullCmd
		{
			ReplyCountryPearlNum_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_REPLY_COUNTRY_PEARL_NUM)
				{
					countryID = 0;
                    num = 0;
				};
			DWORD countryID;
            DWORD num;
		};
        
		//soke 场景到会话,场景增加或减少砖块时同步到会话
		const BYTE PARA_SYNC_COUNTRY_LOONG_PEARL_NUM = 30;
		struct SyncCountryLoongPearlNum_SceneSession : t_NullCmd
		{
			SyncCountryLoongPearlNum_SceneSession()
				: t_NullCmd(CMD_SCENE_COUNTRY, PARA_SYNC_COUNTRY_LOONG_PEARL_NUM)
				{
                    serverID = 0;
					countryID = 0;
                    num = 0;
				};
            DWORD serverID;
			DWORD countryID;
            DWORD num;
		};

		//////////////////////////////////////////////////////////////
		///  国家指令结束
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		///  对战指令开始
		//////////////////////////////////////////////////////////////
		// 由场景往服务器发送，通知其有关对战的处理是否成功，并不仅限于激活对战斗时
		enum
		{
			SCENE_ACTIVEDARE_SUCCESS,    // 场景处理激活战斗成功
			SCENE_ACTIVEDARE_FAIL        // 场景处理激活战斗失败
		};

		const BYTE PARA_ACTIVEDARE = 1;
		struct t_activeDare_SceneSession : t_NullCmd
		{
			DWORD dwWarID;
			DWORD dwStatus;

			t_activeDare_SceneSession()
				: t_NullCmd(CMD_SCENE_DARE, PARA_ACTIVEDARE) 
				{
					dwWarID = 0;
					dwStatus = SCENE_ACTIVEDARE_FAIL;
				};
		};

		
		// 由会话向场景发送，通知其交战状态，场景收到后，会给角色加上交战记录，并由此计算出是否显示交战标示
		const BYTE PARA_ENTERWAR = 2;
		struct t_enterWar_SceneSession : t_NullCmd
		{
			DWORD dwWarType;        // 交战类型
			DWORD dwUserID;		//
			DWORD dwFromRelationID; // 
			DWORD dwToRelationID;   // 与之交战的社会关系ID,守方(在夺旗类型对战时使用)
			DWORD dwSceneTempID;
			DWORD dwStatus;         // 1,为开始交战的记录，0为结束交战的记录
			bool isAtt;		// false,为守方，true为攻方（夺旗类型对战使用）
			bool isAntiAtt;		// 是否可以反攻

			t_enterWar_SceneSession()
				: t_NullCmd(CMD_SCENE_DARE, PARA_ENTERWAR)
				{
					dwWarType = 0;
					dwSceneTempID = 0;
					dwUserID = 0;
					dwToRelationID = 0;
					dwFromRelationID = 0;
					dwStatus = 1;
					isAtt = false;
					isAntiAtt = false;
				};
		};

		// 由场景发送给会话，通知社会团体交战结果
		const BYTE PARA_DARE_PK = 3;
		struct t_darePk_SceneSession : t_NullCmd
		{
			DWORD attID;    // 攻击方用户ID
			DWORD defID;    // 防御方用户ID

			t_darePk_SceneSession()
				: t_NullCmd(CMD_SCENE_DARE, PARA_DARE_PK)
				{
					attID = 0;
					defID = 0;
				};
		};
			
		enum
		{
			DARE_GOLD,	// 挑战金
			RETURN_DARE_GOLD,// 返还挑战金
			WINNER_GOLD, // 奖励金
			EMPEROR_GOLD, // 皇帝每日奖励金
			KING_GOLD, //soke 国王每日薪资
			YS_GOLD,   //soke 元帅每日薪资
			ZX_GOLD,   //soke 宰相每日薪资
			CR_GOLD,   //soke 捕头（左）每日薪资
			DT_GOLD,   //soke 御史（左）每日薪资
			CX_GOLD,   //soke 捕头（右）每日薪资
			DX_GOLD,   //soke 御史（右）每日薪资

		};
		
		// 由会话发向场景，通知其在某玩家上扣钱，或加钱
		const BYTE  PARA_DARE_GOLD = 4;
		struct t_dareGold_SceneSession : t_NullCmd
		{
			DWORD dwUserID;			/// 角色ID
			int   dwNum;			/// 增或减的金钱数
			DWORD dwWarID;			/// 对战ID
			DWORD dwType;                   /// 扣钱类型, 扣挑战金0,还挑战金1,对战奖励2
			DWORD dwWarType;		/// 对战类型
				
			t_dareGold_SceneSession()
				: t_NullCmd(CMD_SCENE_DARE, PARA_DARE_GOLD) 
				{
					dwNum = 0;
					dwUserID = 0;
					dwWarID = 0;
				};
		};
		
		// 新建帮会夺城战(场景发往会话)
		const BYTE PARA_UNION_CITY_DARE = 5;
		struct t_UnionCity_Dare_SceneSession : t_NullCmd
		{
			DWORD dwCountryID;
			DWORD dwCityID;
			DWORD dwFromUserID;	// 发起挑战者
			DWORD dwFromUnionID;	// 
			DWORD dwToCountryID;
				
			t_UnionCity_Dare_SceneSession()
				:	t_NullCmd(CMD_SCENE_DARE, PARA_UNION_CITY_DARE) {}
		};

		// 帮会夺城战结果(场景发往会话)
		const BYTE PARA_UNION_CITY_DARE_RESULT = 6;
		struct t_UnionCity_DareResult_SceneSession : t_NullCmd
		{
			DWORD dwCountryID;
			DWORD dwCityID;
			DWORD dwUserID;	// 赢家帮主
			DWORD dwUnionID; // 对战赢家帮会
				
			t_UnionCity_DareResult_SceneSession()
				:	t_NullCmd(CMD_SCENE_DARE, PARA_UNION_CITY_DARE_RESULT) {}
		};
		
		// GM指令触发帮会夺城战(场景发往会话)
		const BYTE PARA_GM_CREATE_UNION_CITY = 7;
		struct t_GMCreateUnionCity_SceneSession : t_NullCmd
		{
			DWORD dwCityID;
			DWORD dwCountryID;
			BYTE  byOpType;
				
			t_GMCreateUnionCity_SceneSession()
				:	t_NullCmd(CMD_SCENE_DARE, PARA_GM_CREATE_UNION_CITY) {}
		};
		
		// 设置反攻标志(会话发往场景)
		const BYTE PARA_SET_ANTI_ATT_FLAG = 8;
		struct t_setAntiAttFlag_SceneSession : t_NullCmd
		{
			DWORD dwFromRelationID;
			DWORD dwToRelationID;
			DWORD dwType;
				
			t_setAntiAttFlag_SceneSession()
				:	t_NullCmd(CMD_SCENE_DARE, PARA_SET_ANTI_ATT_FLAG) {}
		};
		
		// 设置皇城争夺战开始标志(会话发往场景)
		const BYTE PARA_SET_EMPEROR_DARE = 9;
		struct t_setEmperorDare_SceneSession : t_NullCmd
		{
			t_setEmperorDare_SceneSession()
				:	t_NullCmd(CMD_SCENE_DARE, PARA_SET_EMPEROR_DARE) 
				{
					byState = 0;
					dwDefCountryID = 0;
				}
			
			BYTE byState; // 1,皇城进入争夺战,0,皇城退出争夺战
			DWORD dwDefCountryID; // 目前皇城的拥有者,为0表示,没有
		};

		// 触发皇城争夺战(场景发往会话)
		const BYTE PARA_BEGIN_EMPEROR_DARE = 10;
		struct t_beginEmperorDare_SceneSession : t_NullCmd
		{
			t_beginEmperorDare_SceneSession()
				:	t_NullCmd(CMD_SCENE_DARE, PARA_BEGIN_EMPEROR_DARE) 
				{
				}
		};
		//令牌抓人
		const BYTE PARA_GOTO_LEADER = 11;
		struct t_GoTo_Leader_SceneSession  : t_NullCmd
		{
			t_GoTo_Leader_SceneSession()
				: t_NullCmd(CMD_SCENE_DARE, PARA_GOTO_LEADER) {};
			BYTE type;					//令牌类型
			DWORD leaderTempID;	
			char mapName[MAX_NAMESIZE]; //地图名称
			WORD x;						//坐标x	
			WORD y;						//坐标y
		};
		//令牌抓人到场景验证
		const BYTE PARA_GOTO_LEADER_CHECK = 12;
		struct t_GoTo_Leader_Check_SceneSession  : t_NullCmd
		{
			t_GoTo_Leader_Check_SceneSession()
				: t_NullCmd(CMD_SCENE_DARE, PARA_GOTO_LEADER_CHECK) {};
			BYTE type;					//令牌类型
			DWORD leaderTempID;	
			DWORD userTempID;	
			char mapName[MAX_NAMESIZE]; //地图名称
			WORD x;						//坐标x	
			WORD y;						//坐标y
		};

		//令牌抓人次数检查
		const BYTE PARA_CHECK_CALLTIMES_LEADER = 13;
		struct t_Check_CallTimes_SceneSession  : t_NullCmd
		{
			t_Check_CallTimes_SceneSession()
				: t_NullCmd(CMD_SCENE_DARE, PARA_CHECK_CALLTIMES_LEADER) {};
			BYTE type;					//令牌类型
			DWORD leaderTempID;	
			DWORD qwThisID;				//令牌id
		};
		//返回剩余可用次数
		const BYTE PARA_RETURN_CALLTIMES_LEADER = 14;
		struct t_Return_CallTimes_SceneSession  : t_NullCmd
		{
			t_Return_CallTimes_SceneSession()
				: t_NullCmd(CMD_SCENE_DARE, PARA_RETURN_CALLTIMES_LEADER) {};
			BYTE type;					//令牌类型
			DWORD leaderTempID;	
			DWORD times;
			DWORD qwThisID;				//令牌id
		};
		//令牌清0GM指令
		const BYTE PARA_RESET_CALLTIMES_LEADER = 15;
		struct t_Reset_CallTimes_SceneSession  : t_NullCmd
		{
			t_Reset_CallTimes_SceneSession()
				: t_NullCmd(CMD_SCENE_DARE, PARA_RESET_CALLTIMES_LEADER) {};
			DWORD leaderTempID;	
		};
		//////////////////////////////////////////////////////////////
		///  对战指令结束
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		///  帮会指令开始
		//////////////////////////////////////////////////////////////
		// 场景发往会话，修改帮会资金
		const BYTE OP_UNION_MONEY_PARA = 1;
		struct t_OpUnionMoney_SceneSession : t_NullCmd
		{
			DWORD dwUnionID; // 帮会ID
			int dwMoney; //  资金
			
			t_OpUnionMoney_SceneSession()
				: t_NullCmd(CMD_SCENE_UNION, OP_UNION_MONEY_PARA)
				{
					dwUnionID = 0;
					dwMoney = 0;
				}
		};

		// 场景发往会话，修改帮会行动力
		const BYTE OP_UNION_ACTION_PARA = 2;
		struct t_OpUnionAction_SceneSession : t_NullCmd
		{
			DWORD dwUnionID; // 帮会ID
			int dwAction; //  资金
			
			t_OpUnionAction_SceneSession()
				: t_NullCmd(CMD_SCENE_UNION, OP_UNION_ACTION_PARA)
				{
					dwUnionID = 0;
					dwAction = 0;
				}
		};

		// 会话发往场景，更新有关帮会的普通信息，不用存档
		const BYTE SEND_UNION_NORMAL_PARA = 3;
		struct t_SendUnionNormal_SceneSession : t_NullCmd
		{
			DWORD dwUserID; // 族长ID
			DWORD dwMana; // 家族声望
			
			t_SendUnionNormal_SceneSession()
				: t_NullCmd(CMD_SCENE_UNION, SEND_UNION_NORMAL_PARA)
				{
					dwUserID = 0;
					dwMana = 0;
				}
		};
		
		// 检查一个玩家的善恶度
		const BYTE PARA_CHECK_USER_CITY = 4;
		struct t_checkUserCity_SceneSession : t_NullCmd
		{
			t_checkUserCity_SceneSession()
				: t_NullCmd(CMD_SCENE_UNION, PARA_CHECK_USER_CITY)
				{
					byState   = 0;
					dwCheckID  = 0;
					dwCheckedID = 0;
				};
			
			BYTE byState; // 0, 未通过检查, 1,通过检查
			DWORD dwCheckID; // 发起检查的玩家ID
			DWORD dwCheckedID; // 待检测玩家的ID
		};

		//////////////////////////////////////////////////////////////
		///  帮会指令结束
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		///  军队指令开始
		//////////////////////////////////////////////////////////////
		// 场景发往会话，转发军队列表命令
		const BYTE REQ_ARMY_LIST_SCENE_PARA = 1;
		struct t_ReqArmyList_SceneSession : t_NullCmd
		{
			BYTE byType;
			DWORD dwUserID;
			DWORD dwCityID;
			
			t_ReqArmyList_SceneSession()
				: t_NullCmd(CMD_SCENE_ARMY, REQ_ARMY_LIST_SCENE_PARA)
				{
					byType = 0;
					dwUserID = 0;
					dwCityID = 0;
				}
		};

		const BYTE SEND_USER_ARMY_INFO_PARA = 2;
		struct t_sendUserArmyInfo_SceneSession : t_NullCmd
		{
			char title[MAX_NAMESIZE]; // 所属城市名称
			BYTE byType; // 1为队长，2为将军
			DWORD dwUserID;
			
			t_sendUserArmyInfo_SceneSession()
				: t_NullCmd(CMD_SCENE_ARMY, SEND_USER_ARMY_INFO_PARA)
				{
					bzero(title, MAX_NAMESIZE);
					byType = 0;
					dwUserID = 0;
				}
		};
		//////////////////////////////////////////////////////////////
		///  军队指令结束
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		///  护宝指令开始
		//////////////////////////////////////////////////////////////
		// 会话发往场景，刷出龙精或虎魄罗汉
		const BYTE SUMMON_GEMNPC_SCENE_PARA = 1;
		struct t_SummonGemNPC_SceneSession : t_NullCmd
		{
			DWORD dwMapID;
			DWORD x;
			DWORD y;
			DWORD dwBossID;

			t_SummonGemNPC_SceneSession()
				: t_NullCmd(CMD_SCENE_GEM, SUMMON_GEMNPC_SCENE_PARA)
				{
					dwMapID = 0;
					x = 0;
					y = 0;
					dwBossID = 0;
				}
		};
		
		// 会话发往场景，清除龙精或虎魄罗汉
		const BYTE CLEAR_GEMNPC_SCENE_PARA = 2;
		struct t_ClearGemNPC_SceneSession : t_NullCmd
		{
			DWORD dwMapID;
			DWORD dwBossID;

			t_ClearGemNPC_SceneSession()
				: t_NullCmd(CMD_SCENE_GEM, CLEAR_GEMNPC_SCENE_PARA)
				{
					dwMapID = 0;
					dwBossID = 0;
				}
		};

		// 会话发往场景，设置护宝状态（龙精或虎魄）
		const BYTE SET_GEMSTATE_SCENE_PARA = 3;
		struct t_SetGemState_SceneSession : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwState; // 1为龙精，2为虎魄, 0为清除这两种状态

			t_SetGemState_SceneSession()
				: t_NullCmd(CMD_SCENE_GEM, SET_GEMSTATE_SCENE_PARA)
				{
					dwUserID = 0;
					dwState = 0;
				}
		};
		
		// 会话发往场景，指定龙精或虎魄NPC自暴掉落物品
		const BYTE BLAST_GEMNPC_SCENE_PARA = 4;
		struct t_BlastGemNPC_SceneSession : t_NullCmd
		{
			DWORD dwUserID; // 在指定玩家身旁
			DWORD dwBossID; // 为指定NPC的ID: 1002	龙睛	1003	虎魄	

			t_BlastGemNPC_SceneSession()
				: t_NullCmd(CMD_SCENE_GEM, BLAST_GEMNPC_SCENE_PARA)
				{
					dwUserID = 0;
					dwBossID = 0;
				}
		};
		
		// 场景发往会话,进行护宝状态转移
		const BYTE CHANGE_GEMSTATE_SCENE_PARA = 5;
		struct t_ChangeGemState_SceneSession : t_NullCmd
		{
			DWORD fromUserID; // 从某玩家
			DWORD toUserID; // 转移到某玩家

			DWORD dwState; // 1,龙精，2,虎魄

			t_ChangeGemState_SceneSession()
				: t_NullCmd(CMD_SCENE_GEM, CHANGE_GEMSTATE_SCENE_PARA)
				{
					fromUserID = 0;
					toUserID = 0;
					dwState = 0;
				}
		};

		// 场景发往会话, 取消并重置某护宝状态
		const BYTE CANCEL_GEMSTATE_SCENE_PARA = 6;
		struct t_CancelGemState_SceneSession : t_NullCmd
		{
			DWORD dwUserID; // 要取消的玩家
			DWORD dwState; // 待取消的状态1,龙精，2,虎魄,0所有状态取消

			t_CancelGemState_SceneSession()
				: t_NullCmd(CMD_SCENE_GEM, CANCEL_GEMSTATE_SCENE_PARA)
				{
					dwUserID = 0;
					dwState = 0;
				}
		};

		// 场景发往会话, 设置护宝任务开始和结束
		const BYTE OP_GEMSTATE_SCENE_PARA = 7;
		struct t_OpGemState_SceneSession : t_NullCmd
		{
			DWORD dwState; // 1为开始，0为结束

			t_OpGemState_SceneSession()
				: t_NullCmd(CMD_SCENE_GEM, OP_GEMSTATE_SCENE_PARA)
				{
					dwState = 0;
				}
		};

		//////////////////////////////////////////////////////////////
		///  护宝指令结束
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		///  推荐系统指令开始
		//////////////////////////////////////////////////////////////
		// 会话发往场景，进行奖励
		const BYTE PICKUP_RECOMMEND_SCENE_PARA = 1;
		struct t_PickupRecommend_SceneSession : t_NullCmd
		{
			DWORD dwUserID;
			DWORD dwMoney;
			BYTE  byType; // 0,推荐人提取奖励, 1被推荐人提取奖励

			t_PickupRecommend_SceneSession()
				: t_NullCmd(CMD_SCENE_RECOMMEND, PICKUP_RECOMMEND_SCENE_PARA)
				{
					dwUserID = 0;
					dwMoney = 0;
					byType = 0;
				}
		};
		//////////////////////////////////////////////////////////////
		///  推荐系统指令结束
		//////////////////////////////////////////////////////////////
		struct stCountryList
		{
			DWORD countryID;
			char countryName[MAX_NAMESIZE+1];
		};
		const BYTE PARA_NPC_BOSSDIE_INFO = 1;
		struct t_BossDieInfo_SceneSession : t_NullCmd
		{
			char MapName[MAX_NAMESIZE+1];  //BOSS出现地图名称
			DWORD NpcID; //BOSSID
			DWORD CountryID; //BOSS所属国家ID
			DWORD KillState; //存活状态
			QWORD ReviveTime;  //复活时间
			t_BossDieInfo_SceneSession()
				:	t_NullCmd(CMD_SCENE_BOSSINFO, PARA_NPC_BOSSDIE_INFO) {}
		};

		//////////////////////////////////////////////////////////////
		///  副本指令开始
		//////////////////////////////////////////////////////////////
		// 场景发往会话，请求开启或进入副本
		const BYTE REQ_OPEN_ENTER_FUBEN_SESSION_PARA = 1;
		struct Req_OpenEnterFuBen_SceneSession : t_NullCmd
		{
            FBActionType actType;       // 开启或进入
			DWORD reqSvrID;             // 发出请求的服务器的ID
	        DWORD fbType;		        // 副本类型
            DWORD lastFBScnID;          // 玩家上一个副本场景ID           
            DWORD userID;               // 请求者ID
            DWORD counID;               // 国家ID，也就是副本所在的线或区
            DWORD mapID;                // 副本地图ID
			Req_OpenEnterFuBen_SceneSession()
				: t_NullCmd(CMD_SCENE_FUBEN, REQ_OPEN_ENTER_FUBEN_SESSION_PARA)
				{
                    actType = FB_ACTION_TYPE_NONE;
                    reqSvrID = 0;
                    fbType = 0;
                    lastFBScnID = 0;
                    userID = 0;
                    counID = 0;
                    mapID = 0;
				}
		};
		
		//家族地图
		const BYTE REQ_OPEN_ENTER_JIAZU_SESSION_PARA = 3;
		struct Req_OpenEnterJiaZu_SceneSession : t_NullCmd
		{
            FBActionType actType;       // 开启或进入
			DWORD reqSvrID;             // 发出请求的服务器的ID
	        DWORD fbType;		        // 副本类型
            DWORD lastFBScnID;          // 玩家上一个副本场景ID           
            DWORD userID;               // 请求者ID
            DWORD counID;               // 国家ID，也就是副本所在的线或区
            DWORD mapID;                // 副本地图ID
			Req_OpenEnterJiaZu_SceneSession()
				: t_NullCmd(CMD_SCENE_FUBEN, REQ_OPEN_ENTER_JIAZU_SESSION_PARA)
				{
                    actType = FB_ACTION_TYPE_NONE;
                    reqSvrID = 0;
                    fbType = 0;
                    lastFBScnID = 0;
                    userID = 0;
                    counID = 0;
                    mapID = 0;
				}
		};
		
        ///请求副本返回的结果枚举
        enum ReqFBRetVal
        {
            REQFB_NONE_RETVAL          = 0,  ///初始值，无意义
            REQFB_NO_OVER_MAXCNT       = 1,  ///开启副本失败,超出最大数量
            REQFB_NO_SYS_BUSY          = 2,  ///开启副本失败,系统繁忙
            REQFB_YES_OPEN_NEW         = 3,  ///开启副本成功,新开启副本
            REQFB_EXIST_RUN            = 4,  ///上一个开启的副本还存在,正常运行
            REQFB_EXIST_CLOSING        = 5,  ///上一个开启的副本还存在,但要关闭
            REQFB_EXIST_UNLOAD         = 6,  ///上一个开启的副本还存在,但要卸载
            REQFB_NEED_REOPEN          = 7,  ///上一个开启的副本已经关闭,需要重新开启
            REQFB_CAN_ENTER            = 8,  ///上一个开启的副本正在运行，可以进入 
        };

		// 会话发往场景，回复
		const BYTE REPLY_OPEN_ENTER_FUBEN_SESSION_PARA = 2;
		struct Reply_OpenEnterFuBen_SceneSession : t_NullCmd
		{
            FBActionType actType;               // 开启或进入
			ReqFBRetVal retVal;                 // 回复请求的结果
            DWORD userID;                       // 请求者ID
			DWORD reqSvrID;                     // 发出请求的服务器的ID
			DWORD replySvrID;                   // 回复的服务器的ID
	        DWORD fbType;		                // 副本类型
            DWORD fbScnID;                      // 返回的副本ID
            char  fbName[MAX_NAMESIZE];         // 副本的完整名字
            DWORD closeCDTime;                  // 副本关闭倒计时，当请求的副本正在关闭时才有意义 
            zPos  dstPos;                       // 副本入口中心坐标点
			Reply_OpenEnterFuBen_SceneSession()
				: t_NullCmd(CMD_SCENE_FUBEN, REPLY_OPEN_ENTER_FUBEN_SESSION_PARA)
				{
                    actType = FB_ACTION_TYPE_NONE;
                    retVal = REQFB_NONE_RETVAL;
                    userID = 0;
                    reqSvrID = 0;
                    replySvrID = 0;
                    fbType = 0;
                    fbScnID = 0;
                    bzero( fbName, sizeof(fbName) );
                    closeCDTime = 0;
				}
		};

		// 家族地图
		const BYTE REPLY_OPEN_ENTER_JIAZU_SESSION_PARA = 4;
		struct Reply_OpenEnterJiaZu_SceneSession : t_NullCmd
		{
            FBActionType actType;               // 开启或进入
			ReqFBRetVal retVal;                 // 回复请求的结果
            DWORD userID;                       // 请求者ID
			DWORD reqSvrID;                     // 发出请求的服务器的ID
			DWORD replySvrID;                   // 回复的服务器的ID
	        DWORD fbType;		                // 副本类型
            DWORD fbScnID;                      // 返回的副本ID
            char  fbName[MAX_NAMESIZE];         // 副本的完整名字
            DWORD closeCDTime;                  // 副本关闭倒计时，当请求的副本正在关闭时才有意义 
            zPos  dstPos;                       // 副本入口中心坐标点
			Reply_OpenEnterJiaZu_SceneSession()
				: t_NullCmd(CMD_SCENE_FUBEN, REPLY_OPEN_ENTER_JIAZU_SESSION_PARA)
				{
                    actType = FB_ACTION_TYPE_NONE;
                    retVal = REQFB_NONE_RETVAL;
                    userID = 0;
                    reqSvrID = 0;
                    replySvrID = 0;
                    fbType = 0;
                    fbScnID = 0;
                    bzero( fbName, sizeof(fbName) );
                    closeCDTime = 0;
				}
		};
		//////////////////////////////////////////////////////////////
		///  副本指令结束
		//////////////////////////////////////////////////////////////

	};
};

#pragma pack()

#endif

