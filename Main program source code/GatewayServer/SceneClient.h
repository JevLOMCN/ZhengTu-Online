/**
 * \file
 * \version  $Id: SceneClient.h  $
 * \author 
 * \date 
 * \brief 定义场景服务器连接客户端
 * 
 */

#ifndef _SceneClient_h_
#define _SceneClient_h_

#include <unistd.h>
#include <iostream>
#include <vector>

#include "zTCPClient.h"
#include "SceneCommand.h"
#include "zMutex.h"
#include "SuperCommand.h"
#include "ScreenIndex.h"
#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"
#include "SceneClientManager.h"

/**
 * \brief 定义场景服务器连接客户端类
 **/
class SceneClient : public zTCPClientTask
{

	public:

		SceneClient(
				const std::string &ip,
				const unsigned short port);
		SceneClient( const std::string &name,const Cmd::Super::ServerEntry *serverEntry)
			: zTCPClientTask(serverEntry->pstrExtIP, serverEntry->wdExtPort)
			{
				wdServerID=serverEntry->wdServerID;
			};
		~SceneClient()
		{
			Zebra::logger->debug("SceneClient析构");
		}

		int checkRebound();
		void addToContainer();
		void removeFromContainer();
		bool connectToSceneServer();
		bool connect();
		bool msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);
		void freshIndex(GateUser *pUser , const DWORD map , const DWORD screen)
		{
			MapIndexIter iter = mapIndex.find(map); 
			if(iter != mapIndex.end())
			{
				iter->second->refresh(pUser , screen);
			}
		}
		void removeIndex(GateUser *pUser , const DWORD map)
		{
			MapIndexIter iter = mapIndex.find(map); 
			if(iter != mapIndex.end())
			{
				iter->second->removeGateUser(pUser);
			}
		}
		const WORD getServerID() const
		{
			return wdServerID;
		}

	private:

		/**
		 * \brief 网关屏索引
		 *
		 *
		 * \param 
		 * \return 
		 */
		MapIndex mapIndex;
		/**
		 * \brief 服务器编号
		 *
		 */
		WORD wdServerID;



};
#if 0
/**
 * \brief 定义场景服务器连接客户端类
 *
 */
class SceneClient : public zTCPBufferClient
{

	public:
		
		/**
		 * \brief 构造函数
		 *
		 * \param name 名称
		 * \param serverEntry 场景服务器信息
		 */
		SceneClient( const std::string &name,const Cmd::Super::ServerEntry *serverEntry)
			//soke 端口改成7000了
			: zTCPBufferClient(name, serverEntry->pstrExtIP, serverEntry->wdExtPort, false, /*8000*/ 7000) 
			{
				wdServerID=serverEntry->wdServerID;
			};

		~SceneClient()
		{
			Zebra::logger->debug("SceneClient析构");
		}
		bool connectToSceneServer();
		void run();
		bool msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);

		/**
		 * \brief 获取场景服务器的编号
		 *
		 * \return 场景服务器编号
		 */
		const WORD getServerID() const
		{
			return wdServerID;
		}
		void freshIndex(GateUser *pUser , const DWORD map , const DWORD screen)
		{
			MapIndexIter iter = mapIndex.find(map); 
			if(iter != mapIndex.end())
			{
				iter->second->refresh(pUser , screen);
			}
		}
		void removeIndex(GateUser *pUser , const DWORD map)
		{
			MapIndexIter iter = mapIndex.find(map); 
			if(iter != mapIndex.end())
			{
				iter->second->removeGateUser(pUser);
			}
		}

	private:

		/**
		 * \brief 网关屏索引
		 *
		 *
		 * \param 
		 * \return 
		 */
		MapIndex mapIndex;
		/**
		 * \brief 服务器编号
		 *
		 */
		WORD wdServerID;

};

/**
 * \brief 场景服务器连接管理器
 *
 */
class SceneClientManager
{

	public:

		/**
		 * \brief 析构函数
		 *
		 */
		~SceneClientManager() {};

		/**
		 * \brief 获取管理器的唯一实例
		 *
		 * \return 管理器唯一实例
		 */
		static SceneClientManager &getInstance()
		{
			if (NULL == instance)
				instance = new SceneClientManager();

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

		void add(SceneClient *client);
		void remove(SceneClient *client);
		SceneClient *getByServerID(WORD serverid);
		void final();

	private:

		/**
		 * \brief 场景服务器连接管理器唯一实例指针
		 *
		 */
		static SceneClientManager *instance;

		/**
		 * \brief 构造函数
		 *
		 */
		SceneClientManager() {};

		/**
		 * \brief 容器访问互斥变量
		 *
		 */
		zMutex mlock;
		/**
		 * \brief 场景服务器连接容器
		 *
		 */
		std::vector<SceneClient *> sceneClients;

};

#endif
#endif

