/**
 * \file
 * \version  $Id: ScenesServer.h  $
 * \author  
 * \date 
 * \brief zebra项目场景服务器，游戏绝大部分内容都在本实现
 */

#ifndef _ScenesServer_h_
#define _ScenesServer_h_

#include "zSubNetService.h"
#include "Zebra.h"
#include "SceneManager.h"
#include "zMisc.h"

//soke转生增加的技能和属性点
extern WORD trun_point_rate;
extern WORD trun_skill_rate;
/**
 * \brief 定义场景服务类
 *
 * 场景服务器，游戏绝大部分内容都在本实现<br>
 * 这个类使用了Singleton设计模式，保证了一个进程中只有一个类的实例
 *
 */
class ScenesService : public zSubNetService
{

	public:
		int writeBackTimer;
		//sky 天下第一结构体
		struct txdy_info
		{
			int userid;//角色id
			char username[MAX_NAMESIZE];//角色名
			int saidian;//赛点
			int jifen;//积分
		};
		txdy_info tianxia[32];

		//反外挂暂停状态
		bool fanwaigua;
		DWORD fanwaiguarongcuo;
		//迷宫系统 初始化变量
		struct migong_info
		{
			int ceng;
			int password;
		};

		
		migong_info migong[25];
		
		SceneNpc * han_biaoche;//汉国镖车
		SceneNpc * han_biaoche2;//汉国镖车
		
		SceneNpc * chu_biaoche;//楚国镖车 
		SceneNpc * chu_biaoche2;//楚国镖车

		int hanbiaoche_type;
		int chubiaoche_type;
		///////////////////////////////
		//soke 打怪经验倍数
		int double_exp;
		///////////////////////////////
		bool double_drop;
		BYTE levelDoubleDrop;
		bool msgParse_SuperService(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);
		BYTE countryPower[13];
		
		/**
		 * \brief 虚析构函数
		 *
		 */
		virtual ~ScenesService()
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

		inline static void zcadfss()
		{
			ScenesService * aazz = &ScenesService::getInstance();
			delete aazz;
		}

		/**
		 * \brief 返回唯一的类实例
		 *
		 * \return 唯一的类实例
		 */
		static ScenesService &getInstance()
		{
			if (NULL == instance)
				instance = new ScenesService();

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
		void checkAndReloadConfig();
		bool isSequeueTerminate() 
		{
			return taskPool == NULL;
		}

		//GM_logger
		static zLogger* gm_logger;
		//物品log
		static zLogger* objlogger;
		//外挂_logger
		static zLogger* wg_logger;

		inline static bool zcaaadfss();

		static Cmd::stChannelChatUserCmd * pStampData;
		static DWORD updateStampData();
	private:

		/**
		 * \brief 类的唯一实例指针
		 *
		 */
		static ScenesService *instance;
		/**
		 * \brief 设置重新读取配置标志
		 *
		 */
		static bool reload;

		zTCPTaskPool *taskPool;				/**< TCP连接池的指针 */

		/**
		 * \brief 构造函数
		 *
		 */
		ScenesService() : zSubNetService("场景服务器", SCENESSERVER)
		{
			writeBackTimer = 0;

			taskPool = NULL;
		}

		bool init();
		void newTCPTask(const int sock, const struct sockaddr_in *addr);
		void final();
};

#endif

