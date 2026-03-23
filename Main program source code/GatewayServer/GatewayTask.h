/**
 * \file
 * \version  $Id: GatewayTask.h  $
 * \author 
 * \date 
 * \brief 定义网关连接任务
 *
 */

#ifndef _GatewayTask_h_
#define _GatewayTask_h_

#include "zTCPServer.h"
#include "zTCPTask.h"
#include "zService.h"
#include "zMisc.h"
#include "GateUserManager.h"
#include "LoginSessionManager.h"
#include "BillClient.h"
#include "zTime.h"

/**
 * \brief 服务器连接任务
 *
 */
class GatewayTask : public zTCPTask
{
	public:

		GatewayTask(zTCPTaskPool *pool, const int sock, const struct sockaddr_in *addr = NULL);
		~GatewayTask();

		int verifyConn();
		int waitSync();
		int recycleConn();
		void Terminate(const TerminateMethod method = terminate_passive);
		void addToContainer();
		void removeFromContainer();
		bool uniqueAdd();
		bool uniqueRemove();
		bool msgParse(const Cmd::t_NullCmd *ptNull, const unsigned int nCmdLen);
		bool checkTime(const zRTime &ct);

		/**
		 * \brief 得到该玩家的帐号id
		 *
		 * \return 得到的id
		 */
		DWORD getACCID() const
		{
			return accid;
		}

		/**
		 * \brief 设置该帐号是否验证通过
		 *
		 *
		 * \param ok 是否通过
		 */
		void accountVerifyOK(const bool ok)
		{
			if (ok)
				accountVerified = ACCOUNTVERIFY_SUCCESS;
			else
				accountVerified = ACCOUNTVERIFY_FAILURE;
		}

		/**
		 * \brief 设置是否未vip用户
		 *
		 */
		void setVip(bool vip)
		{
			vip_user = vip;
		}

		/**
		 * \brief 是否是vip用户
		 *
		 */
		bool isVip()
		{
			return vip_user;
		}

	private:
		///数字密码
		char numPassword[MAX_NUMPASSWORD];
		/// 数字密码DWORD版
		DWORD numPwd;
		///vip用户
		bool vip_user;
		///时间校对定时器
		Timer _retset_gametime;
		///回收延时等到(毫秒)
		DWORD recycle_wait;
		///校验客户端时间的间隔
		static const unsigned int sampleInterval = 20000;
		static const unsigned int sampleInterval_sec = sampleInterval/1000;
		static const unsigned int sampleInterval_error_sec = sampleInterval/1000;
		static const unsigned int sampleInterval_error_msecs = sampleInterval;

		///聊天消息转发的间隔
		static const unsigned int chatInterval = 1000;
		///下次聊天的时间
		zRTime nextChatTime;
		///下次国家聊天的时间
		zRTime nextCountryChatTime;

		///客户端在sampleInterval时间内发送超过maxSamplePPS个数据包则被判断为使用了外挂
		static const unsigned int maxSamplePPS = 145;
		///上次晴空v_samplePackets的时间
		DWORD v_lastSampleTime;
		///统计数据包个数
		unsigned int v_samplePackets;

		///该task初始化的时间
		zRTime initTime;
		///上次检查客户端是否已经校验了时间的时间
		zRTime lastCheckTime;
		///是否已经校验了时间
		volatile bool haveCheckTime;

		friend class GateUser;
		///使用该连接的玩家
		GateUser *pUser;

		///消息检查工具
		//CheckerTable checker;
		/**
		 * \brief 账号编号
		 *
		 */
		DWORD accid;

		/**     
		 ** \brief 游戏时间
		 **
		 **/
		QWORD qwGameTime;
		zRTime GameTimeSyn;
		QWORD dwTimestampServer;

		/**
		  * \brief 用户帐号
		  *
		  *
		  */
		char account[MAX_ACCNAMESIZE+1];

		///登录时分配的临时id
		DWORD loginTempID;
		///是否验证了版本
		bool versionVerified;
		///帐号验证的状态
		enum
		{
			ACCOUNTVERIFY_NONE,
			ACCOUNTVERIFY_WAITING,
			ACCOUNTVERIFY_SUCCESS,
			ACCOUNTVERIFY_FAILURE
		}accountVerified;

		bool verifyVersion(const Cmd::stNullUserCmd *ptNullCmd);
		bool verifyACCID(const Cmd::stNullUserCmd *ptNullCmd);
		bool forwardScene(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen);
		bool forwardSceneBill(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen);
		bool forwardBill(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen);
		bool forwardBillScene(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen);
		bool forwardSession(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen);
		bool forwardMini(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen);
		bool msgParse_Select(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen);
		bool msgParse_Time(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen);
		bool checkUserCmd(const Cmd::stNullUserCmd *pCmd, const zRTime &ct);

		bool checkNewName(char *);
};

#endif

