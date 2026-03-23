/**
 * \file
 * \version  $Id: BillSessionManager.h  $
 * \author  
 * \date 
 * \brief 容器，用于保存金币兑换的消息
 *
 * 
 */


#ifndef _MONEYSESSIONMANAGER_H_
#define _MONEYSESSIONMANAGER_H_

#include <map>

#include "zType.h"
#include "zMutex.h"
#include "zTime.h"
#include "Command.h"
#include "UserCommand.h"
#include "BillCommand.h"
#include <string>

class BillTask;

/**
 * \brief 角色档案读取写入的会话记录
 *
 */
struct BillSession
{
	DWORD  accid;			/// 帐号编号
	DWORD  charid;			/// 角色编号
	char   tid[Cmd::UserServer::SEQ_MAX_LENGTH+1];                     /// 交易流水号
	char   account[Cmd::UserServer::ID_MAX_LENGTH+1];     /// 帐号(需要发送给记费服务器)
	char   name[MAX_NAMESIZE+1];     /// 角色名称
	DWORD point;                    /// 扣费点数


	/**
	 * \brief 缺省构造函数
	 *
	 */
	/*
	BillSession(const char* tid, const Cmd::Bill::t_Change_Gold_GateMoney* cmd, BillTask* task = NULL)
	{
		strncpy(this->tid, tid, Cmd::UserServer::SEQ_MAX_LENGTH);
		strncpy(this->account, cmd->account, Cmd::UserServer::ID_MAX_LENGTH);
		this->accid = cmd->accid;
		this->charid = cmd->charid;
		this->point = cmd->point;
		this->task = task;
//		strncpy(this->name, cmd->name, MAX_NAMESIZE);
	}
	// */
	
	BillSession()
	{
		accid = 0;
		charid = 0;
		tid[0] = '\0';
		account[0]  = '\0';
		name[0] = '\0';
		point = 0;
	}

};

class BillSessionManager
{

	public:

		/**
		 * \brief 默认析构函数
		 *
		 */
		~BillSessionManager()
		{
			sessionMap.clear();
		}

		/**
		 * \brief 返回类的唯一实例
		 *
		 * 实现了Singleton设计模式，保证了一个进程中只有一个类的实例
		 *
		 */
		static BillSessionManager &getInstance()
		{
			if (NULL == instance)
				instance = new BillSessionManager;

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

		bool add(BillSession &bs);
//		bool verify(const DWORD accid, const DWORD id, const WORD wdServerID);
		bool remove(const std::string& tid);
//	BillSession find(const std::string& tid);
		BillSession get(const std::string& tid);
//		void removeAllByServerID(const WORD wdServerID);

	private:

		/**
		 * \brief 类的唯一实例指针
		 *
		 */
		static BillSessionManager *instance;

		/**
		 * \brief 默认构造函数
		 *
		 */
		BillSessionManager() {};

		/**
		 * \brief 定义容器类型
		 *
		 */
		typedef std::map<std::string, BillSession> BillSessionHashmap;
		/**
		 * \brief 定义容器迭代器类型
		 *
		 */
		typedef BillSessionHashmap::iterator BillSessionHashmap_iterator;
		/**
		 * \brief 定义容器键值对类型
		 *
		 */
		typedef BillSessionHashmap::value_type BillSessionHashmap_pair;
		
		/**
		 * \brief 存储在线帐号列表信息的容器
		 *
		 */
		BillSessionHashmap sessionMap;
		
		/**
		 * \brief 互斥变量
		 *
		 */
		zMutex mlock;
};

#endif

