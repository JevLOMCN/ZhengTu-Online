/**
 * \file
 * \version  $Id: LoginTask.h  $
 * \author  
 * \date 
 * \brief 定义登陆连接任务
 *
 */

#ifndef _LoginTask_h_
#define _LoginTask_h_

#include "zTCPServer.h"
#include "zTCPTask.h"
#include "zService.h"
#include "zMisc.h"
#include "zDBConnPool.h"
#include "Command.h"
#include "zTCPTask.h"
#include "zTime.h"

using namespace Cmd;//add by Victor

/**
 * \brief 服务器连接任务
 *
 */
class LoginTask : public zTCPTask
{

	public:

		LoginTask( zTCPTaskPool *pool, const int sock);
		/**
		 * \brief 虚析构函数
		 *
		 */
		~LoginTask() {};

		int verifyConn();
		int recycleConn();
		void addToContainer();
		bool uniqueAdd();
		bool uniqueRemove();
		bool msgParse(const Cmd::t_NullCmd *, const unsigned int);

		void genTempID()
		{
			tempid = ++uniqueID;
		}

		const DWORD getTempID() const
		{
			return tempid;
		}

		/**
		 * \brief 登陆错误，返回错误代码到客户端显示
		 *
		 * \param retcode 错误代码
		 * \param tm 是否断开连接
		 */
		void LoginReturn(const BYTE retcode, const bool tm = true)
		{
			using namespace Cmd;
			stServerReturnLoginFailedCmd tCmd;
			
			tCmd.byReturnCode = retcode;
			sendCmd(&tCmd, sizeof(tCmd));

			//由于登陆错误，需要断开连接
			//whj 可能导致coredown,屏蔽测试
			if (tm) Terminate();
		}

		/**
		 * \brief 判断登陆连接是否过长
		 * 如果登陆连接太长，登陆服务器应该主动断开连接
		 * \param ct 当前时间
		 * \return 登陆时间是否过长
		 */
		bool timeout(const zTime &ct)
		{
			if (lifeTime.elapse(ct) >= 600)
				return true;
			else
				return false;
		}

	private:

		/**
		 * \brief 校验客户端版本号
		 */
		DWORD verify_client_version;
		
		/**
		 * \brief 生命期时间
		 */
		zTime lifeTime;
		/**
		 * \brief 临时唯一编号
		 *
		 */
		DWORD tempid;
		/**
		 * \brief 临时唯一编号分配器
		 *
		 */
		static DWORD uniqueID;
		/**
		 * \brief 验证码
		 *
		 */
		char jpegPassport[5];

		bool requestLogin(const Cmd::stUserRequestLoginCmd *ptCmd);

};

#endif

