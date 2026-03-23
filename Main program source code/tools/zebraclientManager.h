/**
 * \file
 * \version  $Id: zebraclientManager.h  $
 * \author   
 * \date 
 * \brief 机器人管理器
 *
 * 
 */


#ifndef _ZEBRACLIENTMANAGER_H_
#define _ZEBRACLIENTMANAGER_H_

#include <vector>

#include "zType.h"
#include "zTCPClientTask.h"
#include "zTCPClientTaskPool.h"
#include "zTime.h"
#include "zLogger.h"
#include "zConfile.h"
#include "zRWLock.h"
#include "Command.h"

class zebraClient;
class ZebraClientManager
{
	private:
		ZebraClientManager()
		{
			taskPool=NULL; 
		}
	public:
		~ZebraClientManager()
		{
			if (taskPool)
			{
				delete taskPool;
				taskPool = NULL;
			}
		}
		bool init();
		static ZebraClientManager *getInstance();
		static void delInstance();
		void add(zebraClient *client);
		bool addClientTask(zebraClient *client,DWORD acc , DWORD tempid , DES_cblock key); 
		void remove(zebraClient *client);
		void timeAction();
	private:
		static ZebraClientManager *_instance;
		typedef std::vector<zebraClient*> ClientContainer;
		typedef ClientContainer::iterator ClientContainerIter;
		ClientContainer client; 
		zRWLock rwlock;
		zTCPClientTaskPool *taskPool;				/**< TCP连接池的指针 */
};
#endif
