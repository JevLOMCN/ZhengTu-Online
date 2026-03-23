/**
 * \file
 * \version  $Id: DBAccessClientManager.h  $
 * \author  
 * \date 
 * \brief 定义统一用户平台账号管理客户连接的管理器
 */


#ifndef _DBAccessClientManager_h_
#define _DBAccessClientManager_h_

#include <vector>

#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"
#include "DBAccessClient.h"
#include "zTime.h"

class DBAccessClientManager
{

	public:

		~DBAccessClientManager();

		static DBAccessClientManager &getInstance()
		{
			if (NULL == instance)
				instance = new DBAccessClientManager();

			return *instance;
		}

		static void delInstance()
		{
			SAFE_DELETE(instance);
		}

		bool init();
		void timeAction(const zTime &ct);
		void add(DBAccessClient *dbAccessClient);
		void remove(DBAccessClient *dbAccessClient);
		bool broadcast(const void *pstrCmd, int nCmdLen);

	private:

		DBAccessClientManager();
		static DBAccessClientManager *instance;

		zTCPClientTaskPool *dbAccessClientPool;
		zTime actionTimer;

		typedef std::vector<DBAccessClient *> DBAccessClientContainer;
		DBAccessClientContainer allClients;
		zMutex mlock;

};

#endif

