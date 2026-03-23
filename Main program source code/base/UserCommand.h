/**
 * \file
 * \version  $Id: UserCommand.h  $
 * \author  
 * \date 2
 * \brief 用户信息服务类命令
 *
 * 
 */

#ifndef __UserCommand_H__
#define __UserCommand_H__

#include "zType.h"
#include "zNullCmd.h"

#pragma pack(1)

namespace Cmd
{
	namespace UserServer
	{
		const int SEQ_MAX_LENGTH = 32;		//交易序列号长度
		const int ID_MAX_LENGTH = 64;		//帐号长度
		const int REMARK_LENGTH = 200;		//备注长度
		const int CARDID_LENGTH = 32;
		const int PCARD_NO_LENGTH = 20;		//道具卡号长度

		const BYTE CMD_USER_LOGON = 1; 		//登陆服务器命令
		const BYTE CMD_USER_CONSUME = 2; 	//玩家扣费命令
		const BYTE CMD_USER_FILLIN = 3; 	//玩家充值命令
		const BYTE CMD_USER_QBALANCE = 4; 	//帐号余额查询
		const BYTE CMD_USER_MCARD = 5; 		//玩家冲值月卡
		const BYTE CMD_USER_PCARD = 6; 		//玩家道具卡
		const BYTE CMD_USER_SCARD = 7;		//专区卡
		struct t_logon : public Cmd::t_NullCmd
		{
			t_logon() 
				: t_NullCmd(CMD_USER_LOGON) { }
		};
		struct t_logon_OK : t_NullCmd//服务器同步命令
		{
			t_logon_OK()
				: t_NullCmd(CMD_USER_LOGON) {};
			GameZone_t 	gameZone;			//游戏区
			char 		name[MAX_NAMESIZE];	//区名字
			BYTE 		netType;			//网络类型
		};

		struct t_cmd_consume : public Cmd::t_NullCmd
		{
			t_cmd_consume() 
				: t_NullCmd(CMD_USER_CONSUME) 
			{ 
				bzero(tid, SEQ_MAX_LENGTH +1);
				bzero(remark, REMARK_LENGTH +1);
			}
			unsigned int 	uid;									//UID
			int				point;										//扣费点数	
			int 			source;										//来源
			char			tid[SEQ_MAX_LENGTH +1];		//交易序列号
			char			remark[REMARK_LENGTH +1];	//备注
			char 			ip[MAX_IP_LENGTH +1];     //客户请求ip
		};

		struct t_cmd_fillin : public Cmd::t_NullCmd
		{
			t_cmd_fillin() 
				: t_NullCmd(CMD_USER_FILLIN) 
			{ 
				bzero(account, ID_MAX_LENGTH +1);
				bzero(tid, SEQ_MAX_LENGTH +1);
				bzero(cardid, CARDID_LENGTH +1);
				bzero(remark, REMARK_LENGTH +1);
			}
			unsigned int uid;											//uid
			int				point;											//充值点数	
			int 			source;											//来源
			char			account[ID_MAX_LENGTH +1];	//玩家帐号
			char			tid[SEQ_MAX_LENGTH +1];			//交易序列号
			char			cardid[CARDID_LENGTH +1];		//充值卡号
			char			remark[REMARK_LENGTH +1];		//备注
		};

		struct t_cmd_qbalance : public Cmd::t_NullCmd
		{
			t_cmd_qbalance() 
				: t_NullCmd(CMD_USER_QBALANCE) { }
			unsigned int	uid;						//UID
			char			account[ID_MAX_LENGTH +1];	//玩家帐号
			char			tid[SEQ_MAX_LENGTH +1];		//交易序列号
		};

		struct t_cmd_mcard : public Cmd::t_NullCmd
		{
			t_cmd_mcard() 
				: t_NullCmd(CMD_USER_MCARD) 
			{ 
				bzero(account, ID_MAX_LENGTH +1);
				bzero(tid, SEQ_MAX_LENGTH +1);
				bzero(remark, REMARK_LENGTH +1);
			}
			unsigned int 	uid;										//UID
			int				point;											//扣费点数	
			int 			source;											//来源
			char			account[ID_MAX_LENGTH +1];	//玩家帐号
			char			tid[SEQ_MAX_LENGTH +1];			//交易序列号
			char			remark[REMARK_LENGTH +1];		//备注
			char 			ip[MAX_IP_LENGTH +1];     	//客户请求ip
		};

		struct t_cmd_pcard : public Cmd::t_NullCmd
		{
			t_cmd_pcard() 
				: t_NullCmd(CMD_USER_PCARD) 
			{ 
				bzero(tid, SEQ_MAX_LENGTH + 1);
				bzero(pcardid, sizeof(pcardid));
			}
			unsigned int 	uid;									//UID
			int 			source;										//来源
			char			tid[SEQ_MAX_LENGTH +1];		//交易序列号
			char			pcardid[CARDID_LENGTH +1];//充值卡号
			char 			ip[MAX_IP_LENGTH +1];     //客户请求ip
		};

		enum	//billclient与userserver通信操作码返回值
		{
			RET_OK = 0,										//成功
			RET_FAIL = -1,								//失败
			RET_ID_NOT_EXIST = -2,				//用户不存在
			RET_BALANCE_NOT_ENOUGH = -3,	//余额不足
			RET_PCARD_NOT_EXIST = -4,			//道具卡不存在
			RET_NOTUSE_GAMEZONE = -5,			//本道具卡不能在该区使用
			RET_PCARD_NOT_REUSE = -6,			//道具卡不能重复使用
			RET_SCARD_ERR = -7,						//专区卡卡号错误
			RET_SCARD_PASSWD_ERR					//专区卡密码错误
		};
		
		//billclient请求类型
		enum
		{
			AT_FILLIN = 0,			//充值
			AT_CONSUME = 1,			//扣费
			AT_SCARD = 2,				//专区卡
			AT_QBALANCE = 3,		//查询
			AT_MCARD = 4,				//月卡冲值
			AT_PCARD = 5,				//道具卡
		};

		//回复billclient子类型
		enum
		{
			SUBAT_INVALID = -1,	//billclient请求子类型
			SUBAT_GOLD = 4			//专区卡充值返回子类型//道具卡的子类型不能于该值重复
		};

		enum
		{
			KEEP = 0,						//保留
			ESALES_FILLIN = 1,	//电子商城充卡
			ESALES_CONSUME = -1	//电子商城消费
		};

		const BYTE CMD_USER_RET = 5; //服务器端的返回命令
		struct t_cmd_ret : public Cmd::t_NullCmd
		{
			t_cmd_ret() 
				: t_NullCmd(CMD_USER_RET) 
			{
				balance = 0;
				bonus = 0;
				hadfilled = 0;
				ret = RET_FAIL;
				subat = SUBAT_INVALID;
			}
			char		tid[SEQ_MAX_LENGTH + 1];	//交易序列号
			int 		balance;				//余额
			int 		bonus;					//积分
			int 		hadfilled;			//曾经充值的标志,1=曾经充值,0=没有
			int 		ret;						//命令返回代码
			int			subat;					//子类型
		};
	};
};

#pragma pack()

#endif

