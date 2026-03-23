/**
 * \file
 * \version  $Id: BillCommand.h  $
 * \author  
 * \date 
 * \brief 定义计费服务器相关指令
 *
 */


#ifndef _BILLCOMMAND_H_
#define _BILLCOMMAND_H_

#include "zNullCmd.h"
#include "Command.h"
#include "UserCommand.h"
#pragma pack(1)

namespace Cmd
{

	namespace Bill
	{
		const BYTE CMD_LOGIN = 1;
		const BYTE CMD_GATE = 2;
		const BYTE CMD_FORWARD = 3;
		const BYTE CMD_REDEEM = 4;
		const BYTE CMD_STOCK = 5;


		//////////////////////////////////////////////////////////////
		/// 登陆计费服务器指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_LOGIN = 1;
		struct t_LoginBill : t_NullCmd
		{
			WORD wdServerID;
			WORD wdServerType;
			t_LoginBill()
				: t_NullCmd(CMD_LOGIN, PARA_LOGIN) {};
		};
		//////////////////////////////////////////////////////////////
		/// 登陆计费服务器指令
		//////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////
		/// 计费服务器与网关服务器交互的指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_GATE_NEWSESSION = 1;
		struct t_NewSession_Gateway : t_NullCmd
		{
			t_NewLoginSession session;

			t_NewSession_Gateway()
				: t_NullCmd(CMD_GATE, PARA_GATE_NEWSESSION) {};
		};

		const BYTE PARA_GATE_LOGINVERIFY = 2;
		struct t_LoginVerify_Gateway : t_NullCmd
		{
			DWORD accid;
			DWORD loginTempID;

			t_LoginVerify_Gateway()
				: t_NullCmd(CMD_GATE, PARA_GATE_LOGINVERIFY) {};
		};

		const BYTE PARA_GATE_LOGINVERIFY_RETURN = 3;
		struct t_LoginVerify_Gateway_Return : t_NullCmd
		{
			DWORD accid;
			DWORD loginTempID;
			BYTE retcode;

			t_LoginVerify_Gateway_Return()
				: t_NullCmd(CMD_GATE, PARA_GATE_LOGINVERIFY_RETURN) {};
		};

		const BYTE PARA_GATE_LOGOUT = 4;
		struct t_Logout_Gateway : t_NullCmd
		{
			DWORD accid;
			DWORD loginTempID;

			t_Logout_Gateway()
				: t_NullCmd(CMD_GATE, PARA_GATE_LOGOUT) {};
		};

		/// 请求兑换金币
		const BYTE PARA_REQUEST_GATE_REDEEM_GOLD = 5;
		struct t_Request_Redeem_Gold_Gateway : t_NullCmd
		{
			char account[Cmd::UserServer::ID_MAX_LENGTH+1];      		/// 帐号     
			DWORD accid;			        /// 账号编号
			DWORD charid;				/// 角色ID
			DWORD point;				/// 需兑换的点数
			
			t_Request_Redeem_Gold_Gateway()
				: t_NullCmd(CMD_REDEEM, PARA_REQUEST_GATE_REDEEM_GOLD) 
				{
					bzero(account , sizeof(account));
				};
		};

		/// 兑换金币
		const BYTE PARA_GATE_REDEEM_GOLD = 6;
		struct t_Redeem_Gold_Gateway : t_NullCmd
		{
			char account[Cmd::UserServer::ID_MAX_LENGTH+1];      		/// 帐号     
			DWORD accid;			        /// 账号编号
			DWORD charid;				/// 角色ID
			DWORD dwGold;				/// 	当前拥有金币数
			DWORD dwBalance;			/// 点卡余额
			BYTE byReturn;	//返回类型
			
			t_Redeem_Gold_Gateway()
				: t_NullCmd(CMD_REDEEM, PARA_GATE_REDEEM_GOLD) 
				{
					bzero(account , sizeof(account));
					charid=0;
					dwGold=0;
					dwBalance=0;
					byReturn=0;
				};
		};
		/// 请求兑换月卡
		const BYTE PARA_REQUEST_GATE_REDEEM_MONTH_CARD = 7;
		struct t_Request_Redeem_MonthCard_Gateway : t_NullCmd
		{
			char account[Cmd::UserServer::ID_MAX_LENGTH+1];      		/// 帐号     
			DWORD accid;			        /// 账号编号
			DWORD charid;				/// 角色ID
			t_Request_Redeem_MonthCard_Gateway()
				: t_NullCmd(CMD_REDEEM, PARA_REQUEST_GATE_REDEEM_MONTH_CARD)
				{
					bzero(account , sizeof(account));
				};
		};
		/// 兑换月卡
		const BYTE PARA_GATE_REDEEM_MONTH_CARD = 8;
		struct t_Redeem_MonthCard_Gateway : t_NullCmd
		{
			char account[Cmd::UserServer::ID_MAX_LENGTH+1];      		/// 帐号     
			DWORD accid;			        /// 账号编号
			DWORD charid;				/// 角色ID
			DWORD dwNum;	//冲值后的到期时间(time_t)
			DWORD dwBalance;			/// 点卡余额
			BYTE byReturn;	//返回类型
			t_Redeem_MonthCard_Gateway()
				: t_NullCmd(CMD_REDEEM, PARA_GATE_REDEEM_MONTH_CARD)
				{
					bzero(account , sizeof(account));
				};
		};

		/// 查询月卡和金币
		const BYTE PARA_GATE_REQUECT_CARD_GOLD = 9;
		struct t_Request_Card_Gold_Gateway : t_NullCmd
		{
			char account[Cmd::UserServer::ID_MAX_LENGTH+1];      		/// 帐号     
			DWORD accid;			        /// 账号编号
			DWORD charid;				/// 角色ID
			t_Request_Card_Gold_Gateway()
				: t_NullCmd(CMD_REDEEM, PARA_GATE_REQUECT_CARD_GOLD)
				{
					bzero(account , sizeof(account));
				};
		};
		 
		/// 查询返回
		const BYTE PARA_GATE_RETURN_CARD_GOLD = 10;
		struct t_Return_Card_Gold : t_NullCmd
		{
			char account[Cmd::UserServer::ID_MAX_LENGTH+1];      		/// 帐号     
			DWORD accid;			        /// 账号编号
			DWORD charid;				/// 角色ID
			DWORD dwMonthCard;	//月卡
			DWORD dwGold;	//金币
			BYTE byReturn;	//返回类型
			t_Return_Card_Gold()
				: t_NullCmd(CMD_REDEEM, PARA_GATE_RETURN_CARD_GOLD)
				{
					bzero(account , sizeof(account));
				};
		};

		/// 查询点数
		const BYTE PARA_GATE_REQUECT_POINT = 10;
		struct t_Request_Point_Gateway : t_NullCmd
		{
			char account[Cmd::UserServer::ID_MAX_LENGTH+1];      		/// 帐号     
			DWORD accid;			        /// 账号编号
			DWORD charid;				/// 角色ID
			t_Request_Point_Gateway()
				: t_NullCmd(CMD_REDEEM, PARA_GATE_REQUECT_POINT)
				{
					bzero(account , sizeof(account));
				};
		};
		 
		/// 查询返回
		const BYTE PARA_GATE_RETURN_POINT = 11;
		struct t_Return_Point : t_NullCmd
		{
			char account[Cmd::UserServer::ID_MAX_LENGTH+1];      		/// 帐号     
			DWORD accid;			        /// 账号编号
			DWORD charid;				/// 角色ID
			DWORD dwPoint;	//剩余点数
			BYTE byReturn;	//返回类型
			t_Return_Point()
				: t_NullCmd(CMD_REDEEM, PARA_GATE_RETURN_POINT)
				{
					bzero(account , sizeof(account));
				};
		};
		 
		/// 消费道具卡
		const BYTE PARA_GATE_CONSUME_CARD = 12;
		struct stConSumeCardCard_Gateway : t_NullCmd
		{
			char account[Cmd::UserServer::ID_MAX_LENGTH+1];      		/// 帐号     
			DWORD accid;			        /// 账号编号
			char cardid[CARDID_LENGTH_PARA];
			DWORD type;						///卡类型
			stConSumeCardCard_Gateway()
				: t_NullCmd(CMD_REDEEM, PARA_GATE_CONSUME_CARD)
				{
					bzero(account , sizeof(account));
					bzero(cardid,sizeof(cardid));
				};
		};
		/// 消费道具卡返回 
		const BYTE PARA_GATE_RETURN_CARD = 13;
		struct t_Return_ObjCard : t_NullCmd
		{
			char account[Cmd::UserServer::ID_MAX_LENGTH+1];      		/// 帐号     
			DWORD accid;			        /// 账号编号
			DWORD subatt;	//消费子类型
			DWORD type;						///卡类型
			DWORD balance;						///专区卡点数
			BYTE byReturn;	//返回类型
			t_Return_ObjCard()
				: t_NullCmd(CMD_REDEEM, PARA_GATE_RETURN_CARD)
				{
					bzero(account , sizeof(account));
				};
		};
		/*
		const BYTE PARA_GATE_QUERY_GOLD = 6;
		struct t_Query_Gold_GateMoney : t_NullCmd
		{
			char account[Cmd::UserServer::ID_MAX_LENGTH+1];      		/// 帐号     
			DWORD accid;			        /// 账号编号
			DWORD charid;				/// 角色ID
			DWORD dwNum;	//到期时间(time_t,4字节结构)
			BYTE byReturn;	//返回类型

			t_Query_Gold_GateMoney()
				: t_NullCmd(CMD_GATE, PARA_GATE_QUERY_GOLD) {};
		};
	
		enum
		{
			QUERY_SUCCESS,    // 查询成功
			QUERY_BUSY,       // 服务器忙
			QUERY_FAIL,       // 查询失败
			QUERY_EMPTY,      // 该帐号，没有记录
		};
	  	  
		const BYTE PARA_GATE_RETURN_QUERY_GOLD = 7;
		struct t_Return_Query_Gold_GateMoney : t_NullCmd
		{
			DWORD accid;			        /// 账号编号
			DWORD charid;				/// 角色ID
			double gold;				/// 帐户余额
			BYTE  state;                            /// 查询状态

			t_Return_Query_Gold_GateMoney()
				: t_NullCmd(CMD_GATE, PARA_GATE_CHANGE_GOLD) {
				gold = 0;
				accid = 0;
				};
		};
		
		const BYTE PARA_GATE_TRADE_GOLD = 8;
		struct t_Trade_Gold_GateMoney : t_NullCmd
		{
			DWORD accid;			      			/// 账号编号
			DWORD charid;						/// 角色ID
			double gold;						/// 交易的金额，为正为要减少的金币数，为负为要增加的
										/// 金币数
			DWORD object_id;					/// 交易物品的ID
			DWORD object_num;					/// 交易物品的数量

			t_Trade_Gold_GateMoney()
				: t_NullCmd(CMD_GATE, PARA_GATE_TRADE_GOLD) {
				accid = 0;
				gold = 0;
				object_id = 0;
				};
		};

		
		const BYTE PARA_RETURN_GATE_TRADE_GOLD = 9;
		struct t_Return_Trade_Gold_GateMoney : t_NullCmd
		{
			DWORD accid;			      			/// 账号编号
			DWORD charid;						/// 角色ID
			double gold;						/// 金币余额
			DWORD object_id;					/// 交易物品的ID
			DWORD object_num;					/// 交易物品的数量
			BYTE  state;						/// 交易结果状态

			t_Return_Trade_Gold_GateMoney()
				: t_NullCmd(CMD_GATE, PARA_GATE_TRADE_GOLD) {
				accid = 0;
				gold = 0;
				object_id = 0;
				};
		};
		// */

		//////////////////////////////////////////////////////////////
		/// 股票指令
		//////////////////////////////////////////////////////////////
		//CMD_BILL_GATE_SCENE
		/// 股票套现
		const BYTE PARA_STOCK_FETCH = 1;
		struct t_Stock_Fetch: t_NullCmd
		{
			DWORD dwGold;					/// 套现金币数量
			DWORD dwMoney;					/// 套现银币数量
			t_Stock_Fetch()
				: t_NullCmd(CMD_STOCK, PARA_STOCK_FETCH)
				{
					dwGold=0;
					dwMoney=0;
				};
		};
		/// 股票冲值
		const BYTE PARA_STOCK_SAVE = 2;
		struct t_Stock_Save: t_NullCmd
		{
			DWORD dwGold;					/// 冲入金币数量
			DWORD dwMoney;					/// 冲入银币数量
			t_Stock_Save()
				: t_NullCmd(CMD_STOCK, PARA_STOCK_SAVE)
				{
					dwGold=0;
					dwMoney=0;
				};
		};
		/// 密码登陆成功
		const BYTE PARA_STOCK_LOGIN = 2;
		struct t_Stock_Login: t_NullCmd
		{
			t_Stock_Login()
				: t_NullCmd(CMD_STOCK, PARA_STOCK_LOGIN)
				{
				};
		};
		/// 密码登陆成功
		const BYTE PARA_STOCK_LOGOUT = 2;
		struct t_Stock_Logout: t_NullCmd
		{
			t_Stock_Logout()
				: t_NullCmd(CMD_STOCK, PARA_STOCK_LOGOUT)
				{
				};
		};
		//////////////////////////////////////////////////////////////
		/// 股票指令结束
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		/// 计费服务器与网关服务器交互的指令
		//////////////////////////////////////////////////////////////
		
		//////////////////////////////////////////////////////////////
		/// 会话服务器转发指令
		//////////////////////////////////////////////////////////////
		const BYTE PARA_FORWARD_USER = 1;
		struct t_Bill_ForwardUser : t_NullCmd
		{
			DWORD dwAccid;
			WORD  size;
			BYTE  data[0];
			t_Bill_ForwardUser()
				: t_NullCmd(CMD_FORWARD, PARA_FORWARD_USER) {};
		};
		const BYTE PARA_FORWARD_BILL = 2;
		struct t_Bill_ForwardBill : t_NullCmd
		{
			DWORD dwAccid;
			WORD  size;
			BYTE  data[0];
			t_Bill_ForwardBill()
				: t_NullCmd(CMD_FORWARD, PARA_FORWARD_BILL) {};
		};
		const BYTE PARA_FORWARD_BILL_TO_SCENE = 3;
		struct t_Bill_ForwardBillToScene : t_NullCmd
		{
			DWORD id;
			WORD  size;
			BYTE  data[0];
			t_Bill_ForwardBillToScene()
				: t_NullCmd(CMD_FORWARD, PARA_FORWARD_BILL_TO_SCENE) {};
		};
		const BYTE PARA_SCENE_FORWARD_BILL = 4;
		struct t_Scene_ForwardBill : t_NullCmd
		{
			DWORD dwAccid;
			WORD  size;
			BYTE  data[0];
			t_Scene_ForwardBill()
				: t_NullCmd(CMD_FORWARD, PARA_SCENE_FORWARD_BILL) {};
		};
		// */
	};

};

#pragma pack()

#endif

