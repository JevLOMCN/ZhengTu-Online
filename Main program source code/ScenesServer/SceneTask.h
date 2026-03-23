/**
 * \file
 * \version  $Id: SceneTask.h  $
 * \author  
 * \date 
 * \brief 定义登陆连接任务
 *
 */

#ifndef _SceneTask_h_
#define _SceneTask_h_

#include "zTCPServer.h"
#include "zTCPTask.h"
#include "zService.h"
#include "zMisc.h"
#include "zEntry.h"
#include "SceneCommand.h"
#include "zSocket.h"
#include "MessageQueue.h"
#include <list>

class SceneUser;
/**
 * \brief 服务器连接任务
 *
 */
class SceneTask : public zEntry, public zTCPTask , public MessageQueue
{

	public:

		/**
		 * \brief 构造函数
		 *
		 * \param pool 所属连接池指针
		 * \param sock TCP/IP套接口
		 * \param addr 地址
		 */
		SceneTask(
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
		virtual ~SceneTask();

		int verifyConn();
		int waitSync();
		int recycleConn();
		bool uniqueAdd();
		bool uniqueRemove();
		bool msgParse(const Cmd::t_NullCmd *, const unsigned int);
		bool cmdMsgParse(const Cmd::t_NullCmd *, const unsigned int);

		/**
		 * \brief 返回服务器编号
		 *
		 * 编号在一个区中是唯一的，保存在服务器管理器中
		 *
		 * \return 服务器编号
		 */
		const WORD getID() const
		{
			return wdServerID;
		}

		/**
		 * \brief 返回服务器类型
		 *
		 * \return 服务器类型
		 */
		const WORD getType() const
		{
			return wdServerType;
		}
		bool checkRecycle();

	private:

		bool usermsgParse(SceneUser *pUser,const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen);
		bool usermsgParseBill(SceneUser *pUser,const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen);
		bool loginmsgParse(const Cmd::t_NullCmd *ptNullCmd, unsigned int cmdLen);
		WORD wdServerID;
		WORD wdServerType;

		bool verifyLogin(const Cmd::Scene::t_LoginScene *ptCmd);
		int recycle_state;
		bool veriry_ok;


};

#endif

