/**
 * \file
 * \version  $Id: SessionServer.h $
 * \author  
 * \date 
 * \brief 游戏全局Session服务器 
 */

#ifndef _SessionServer_h_
#define _SessionServer_h_

#include "zSubNetService.h"
#include "Zebra.h"
#include "zMisc.h"
#include "SessionManager.h"

class zDBConnPool;
class MetaData;

/**
 * \brief 遍历每个用户会话给同一国家的角色发送到场景
 */
struct OneCountryScene: public execEntry<UserSession>
{
	DWORD country;
	unsigned int cmdLen;
	Cmd::t_NullCmd* sendCmd;

	void init(Cmd::t_NullCmd * rev, unsigned int len, DWORD countryID)
	{
		sendCmd = rev;
		cmdLen = len;

		country = countryID;
	}

	/**
	 * \brief 遍历每个用户会话给同一国家的角色发送场景命令
	 * \param su 用户会话
	 * \return true 成功 false 失败
	 */
	bool exec(UserSession *su)
	{
		if (country == su->country && su->scene)
		{
			if (sendCmd->para == Cmd::Session::PARA_ENTERWAR)
			{
				((Cmd::Session::t_enterWar_SceneSession*)sendCmd)->dwUserID = su->id;
			}
			
			su->scene->sendCmd(sendCmd, cmdLen);
		}
		return true;
	}
};

struct worldMsg
{
	char msg[256];
	DWORD time;
	DWORD interval;
	DWORD count;
	DWORD country;
	DWORD mapID;
	char GM[32];
	worldMsg()
	{
		bzero(msg, sizeof(msg));
		time = 0;
		interval = 0;
		count = 0;
		country = 0;
		mapID = 0;
		bzero(GM, sizeof(GM));
	}
};

/**
 * \brief Session服务类
 *
 * 游戏全局的Session服务器
 *
 */
class SessionService : public zSubNetService
{

	public:

		bool msgParse_SuperService(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);

		/**
		 * \brief 纯虚析构函数
		 *
		 */
		~SessionService()
		{
			instance = NULL;

			//关闭线程池
			if (taskPool)
			{
				taskPool->final();
				SAFE_DELETE(taskPool);
			}
		}

		const int getPoolSize() const
		{
			if(taskPool)
			{
				return taskPool->getSize();
			}
			else
			{
				return 0;
			}
		}

		/**
		 * \brief 获取类的唯一实例
		 *
		 * 这个类使用了Singleton，保证一个进程中只有一个类的实例
		 *
		 * \return 类的唯一实例
		 */
		static SessionService &getInstance()
		{
			if (NULL == instance)
				instance = new SessionService();

			return *instance;
		}

		/**
		 * \brief 释放类的唯一实例
		 *
		 */
		static void delInstance()
		{
			SAFE_DELETE(instance);
		}

		void reloadConfig();
		bool isSequeueTerminate() 
		{
			return taskPool == NULL;
		}
		
		/**
		 * \brief 指向数据库连接池实例的指针
		 *
		 */
		static zDBConnPool *dbConnPool;
		static MetaData* metaData;
		bool checkShutdown();
		//bool checkGumu();

		/**
		 * \brief 检查国家强弱
		 *
		 */
		void checkCountry(struct tm &tmValue, bool donow = false);
		Cmd::Session::t_shutdown_SceneSession shutdown_time;
		
		static std::map<BYTE, worldMsg> wMsg;
		//static DWORD snCount;
		static std::map<DWORD,BYTE> userMap;
		DWORD countryLevel[13];
		bool uncheckCountryProcess;

		static bool reportGm(const char * fromName, const char *msg, ...);

		static DWORD emperorForbid[10];//皇帝禁言过的玩家
		DWORD loadEmperorForbid();//从数据库读
		void saveEmperorForbid();//写数据库
		void clearEmperorForbid();//每天清除记录
	private:

		DWORD gumutime;
		/**
		 * \brief 类的唯一实例指针
		 *
		 */
		static SessionService *instance;

		zTCPTaskPool *taskPool;				/**< TCP连接池的指针 */

		/**
		 * \brief 构造函数
		 *
		 */
		SessionService() : zSubNetService("Session服务器", SESSIONSERVER)
		{
			taskPool = NULL;
			bzero(&shutdown_time,sizeof(shutdown_time));
			gumutime=0; 
		}

		bool init();
		void newTCPTask(const int sock, const struct sockaddr_in *addr);
		void final();
};

#endif

