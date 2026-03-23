/**
 * \file
 * \version  $Id: PingTask.h  $
 * \author  
 * \date 
 * \brief 定义PING列表连接任务
 *
 */

#ifndef _PINGTASK_H_
#define _PINGTASK_H_

#include "zTCPServer.h"
#include "zTCPTask.h"
#include "zService.h"
#include "zMisc.h"
#include "zDBConnPool.h"
#include "Command.h"
#include "zTCPTask.h"
#include "zTime.h"

/**
 * \brief 服务器连接任务
 *
 */
class PingTask : public zTCPTask
{

	public:

		/**
		 * \brief 构造函数
		 * \param pool 所属的连接池
		 * \param sock TCP/IP套接口
		 */
		PingTask(
				zTCPTaskPool *pool,
				const int sock) : zTCPTask(pool, sock, NULL, true, false)
		{
			/*
#ifdef _ENCDEC_MSG
			pSocket->enc.setEncMethod(CEncrypt::ENCDEC_RC5);
			//pSocket->enc.set_key_rc5((const unsigned char *)Zebra::global["rc5_key"].c_str(), 16, 12);
			unsigned char key[16] = {28, 196, 25, 36, 193, 125, 86, 197, 35, 92, 194, 41, 31, 240, 37, 223};
			pSocket->enc.set_key_rc5((const unsigned char *)key, 16, 12);
#endif
			*/
		}

		/**
		 * \brief 虚析构函数
		 *
		 */
		~PingTask() {};

		int verifyConn();
		int recycleConn();
	//	void addToContainer();
	//	bool uniqueAdd();
	//	bool uniqueRemove();
		bool msgParse(const Cmd::t_NullCmd* ptNull, const unsigned int);
	private:
};

#endif

