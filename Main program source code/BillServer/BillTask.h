/**
 * \file
 * \version  $Id: BillTask.h  $
 * \author  
 * \date 2
 * \brief 定义计费连接任务
 *
 */

#ifndef _BillTask_h_
#define _BillTask_h_

#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <ext/hash_map>

#include "zTCPServer.h"
#include "zTCPTask.h"
#include "zService.h"
#include "zMisc.h"
#include "BillCommand.h"
#include "MessageQueue.h"

/**
 * \brief 定义计费连接任务类
 *
 */
class BillTask : public zTCPTask , public MessageQueue
{

	public:

		/**
		 * \brief 构造函数
		 *
		 * \param pool 所属连接池指针
		 * \param sock TCP/IP套接口
		 * \param addr 地址
		 */
		BillTask(
				zTCPTaskPool *pool,
				const int sock,
				const struct sockaddr_in *addr = NULL) : zTCPTask(pool, sock, addr)
		{
			wdServerID = 0;
			wdServerType = UNKNOWNSERVER;
			recycle_state=0;
			veriry_ok=false; 
		}

		/**
		 * \brief 虚析构函数
		 *
		 */
		~BillTask() {};

		int verifyConn();
		int recycleConn();
		bool uniqueAdd();
		bool uniqueRemove();
		bool msgParse(const Cmd::t_NullCmd *, const unsigned int);
		bool cmdMsgParse(const Cmd::t_NullCmd *, const unsigned int);
		bool checkRecycle();

		/**
		 * \brief 获取服务器编号
		 *
		 * \return 服务器编号
		 */
		const WORD getID() const
		{
			return wdServerID;
		}

		/**
		 * \brief 获取服务器类型
		 *
		 * \return 服务器类型
		 */
		const WORD getType() const
		{
			return wdServerType;
		}

		bool sendCmdToUser(DWORD id, const void *pstrCmd, const unsigned int nCmdLen);
		bool sendCmdToScene(DWORD id, const void *pstrCmd, const unsigned int nCmdLen);

	private:

		/**
		 * \brief 容器访问互斥变量
		 *
		 */
		zMutex mlock;
				
		WORD wdServerID;					/**< 服务器编号，一个区唯一的 */
		WORD wdServerType;					/**< 服务器类型 */

		/**
		  * \brief 查询金币
		  *
		  *
		  */
		//void query_gold(const Cmd::Bill::t_Query_Gold_GateMoney* cmd);

		/**
		  * \brief 点卡兑换金币
		  *
		  */
		//void change_gold(const Cmd::Bill::t_Change_Gold_GateMoney* cmd);

		/**
		  * \brief 金币交易
		  *
		  */
		//void trade_gold(const Cmd::Bill::t_Trade_Gold_GateMoney* cmd);

		//void trade_log(const Cmd::Bill::t_Trade_Gold_GateMoney* cmd, const char* account, double money);
		bool verifyLogin(const Cmd::Bill::t_LoginBill *ptCmd);
		int recycle_state;
		bool veriry_ok;

};

#endif

