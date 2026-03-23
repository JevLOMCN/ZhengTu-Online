/**
 * \file
 * \version  $Id: SuperServer.h  $
 * \author  
 * \date 
 * \brief 实现服务器管理器
 *
 * 对一个区中的所有服务器进行管理
 * 
 */

#ifndef _SuperServer_h_
#define _SuperServer_h_

#include <iostream>
#include <string>
#include <ext/numeric>

#include "zService.h"
#include "zThread.h"
#include "zSocket.h"
#include "zTCPServer.h"
#include "zNetService.h"
#include "ServerTask.h"
#include "zDBConnPool.h"
#include "zMisc.h"

/**
 * \brief 服务器管理器类
 *
 * 派生了基类<code>zNetService</code>
 *
 */
class SuperService : public zNetService
{

	public:

		/**
		 * \brief 析构函数
		 *
		 * 虚函数
		 *
		 */
		~SuperService()
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
			if (taskPool)
				return taskPool->getSize();
			else
				return 0;
		}

		/**
		 * \brief 获取类的唯一实例
		 *
		 * 使用了Singleton设计模式，保证了一个进程中只有一个类的实例
		 *
		 * \return 类的唯一实例
		 */
		static SuperService &getInstance()
		{
			if (NULL == instance)
				instance = new SuperService();

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

		/**
		 * \brief 获取游戏区编号
		 * \return 返回游戏区编号
		 */
		const GameZone_t &getZoneID() const
		{
			return gameZone;
		}

		/**
		 * \brief 设置游戏区编号
		 * \param gameZone 游戏区编号
		 */
		void setZoneID(const GameZone_t &gameZone)
		{
			this->gameZone = gameZone;
		}

		/**
		 * \brief 获取游戏区名称
		 * \return 返回游戏区名称
		 */
		const std::string &getZoneName() const
		{
			return zoneName;
		}

		/**
		 * \brief 设置游戏区名称
		 * \param zoneName 待设置的名称
		 */
		void setZoneName(const char *zoneName)
		{
			this->zoneName = zoneName;
		}

		/**
		 * \brief 获取服务器编号
		 * \return 服务器编号
		 */
		const WORD getID() const
		{
			return wdServerID;
		}

		/**
		 * \brief 获取服务器类型
		 * \return 服务器类型
		 */
		const WORD getType() const
		{
			return wdServerType;
		}

		/**
		 * \brief 获取ip地址
		 * \return 返回ip地址
		 */
		const char *getIP() const
		{
			return pstrIP;
		}

		/**
		 * \brief 获取端口
		 * \return 返回端口
		 */
		const WORD getPort() const
		{
			return wdPort;
		}

		/**
		 * \brief 指向数据库连接池实例的指针
		 *
		 */
		static zDBConnPool *dbConnPool;

	private:

		/**
		 * \brief 游戏区编号
		 */
		GameZone_t gameZone;
		/**
		 * \brief 游戏区名称
		 */
		std::string zoneName;

		WORD wdServerID;					/**< 服务器编号，一个区唯一的 */
		WORD wdServerType;					/**< 服务器类型，创建类实例的时候已经确定 */
		char pstrName[MAX_NAMESIZE];		/**< 服务器名称 */
		char pstrIP[MAX_IP_LENGTH];			/**< 服务器内网地址 */
		WORD wdPort;						/**< 服务器内网端口，也就是邦定端口 */
		char pstrExtIP[MAX_IP_LENGTH];		/**< 服务器外网地址，也就是防火墙地址 */
		WORD wdExtPort;						/**< 服务器外网端口，也就是映射到防火墙的端口 */

		/**
		 * \brief 类的唯一实例指针
		 *
		 */
		static SuperService *instance;

		zTCPTaskPool *taskPool;				/**< TCP连接池的指针 */

		/**
		 * \brief 构造函数
		 *
		 */
		SuperService() : zNetService("服务器管理器")
		{
			wdServerID = 1;
			wdServerType = SUPERSERVER;
			bzero(pstrName, sizeof(pstrName));
			bzero(pstrIP, sizeof(pstrIP));
			wdPort = 0;
			bzero(pstrExtIP, sizeof(pstrExtIP));
			wdExtPort = 0;
			taskPool = NULL;
		}

		bool init();
		void newTCPTask(const int sock, const struct sockaddr_in *addr);
		void final();

		bool getServerInfo();

};


#endif

