/**
 * \file
 * \version  $Id: MiniTask.h  $
 * \author  
 * \date 
 * \brief 定义计费连接任务
 *
 */

#ifndef _MINITASK_H_
#define _MINITASK_H_

#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <ext/hash_map>

#include "zTCPServer.h"
#include "zTCPTask.h"
#include "zService.h"
#include "zMisc.h"
#include "MiniCommand.h"
#include "MessageQueue.h"

/**
 * \brief 定义计费连接任务类
 *
 */
class MiniTask : public zTCPTask , public MessageQueue
{

	public:

		/**
		 * \brief 构造函数
		 *
		 * \param pool 所属连接池指针
		 * \param sock TCP/IP套接口
		 * \param addr 地址
		 */
		MiniTask(
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
		~MiniTask() {};

		int verifyConn();
		int recycleConn();
		bool uniqueAdd();
		bool uniqueRemove();
		bool msgParse(const Cmd::t_NullCmd *, const unsigned int);
		bool cmdMsgParse(const Cmd::t_NullCmd *, const unsigned int);
		bool parseGateMsg(const Cmd::t_NullCmd *, const unsigned int);
		bool parseForwardMsg(const Cmd::t_NullCmd *, const unsigned int);
		bool parseSceneMsg(const Cmd::t_NullCmd *, const unsigned int);
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

		bool verifyLogin(const Cmd::Mini::t_LoginMini *ptCmd);
		int recycle_state;
		bool veriry_ok;

		bool addDBMoney(DWORD userID, DWORD num);
};

#endif

