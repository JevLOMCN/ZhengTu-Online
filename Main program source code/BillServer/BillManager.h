/**
 * \file
 * \version  $Id: BillManager.h  $
 * \author  
 * \date 
 * \brief 账号管理，记录一个区中所有已经登陆的账号
 *
 * 
 */

#ifndef _BillManager_h_
#define _BillManager_h_

#include <iostream>
#include <ext/hash_map>

#include "Zebra.h"
#include "SuperCommand.h"
#include "zTime.h"
#include "UserCommand.h"

#pragma pack(1)
/**
 * \brief 账号信息
 *
 */
struct BillInfo
{
	DWORD accid;			    /**< 账号编号 */
	DWORD loginTempID;		    /**< 登陆临时编号 */

	WORD wdGatewayID;		/**< 登陆的网关编号 */

	double gold;		/**< 金币数量 */
	DWORD vip_time;		/**< vip到期时间 */

	enum
	{
		WAIT_LOGIN,			/**< 等待通过登陆验证的客户端登陆网关服务器 */
		CONF_LOGIN			/**< 登陆网关服务器验证已经成功 */
	}
	state;					/**< 会话状态 */
	zTime timestamp;		/**< 时间戳 */
	char client_ip[MAX_IP_LENGTH];

	/**
	 * \brief 缺省构造函数
	 *
	 */
	BillInfo() : timestamp()
	{
		accid = 0;
		loginTempID = 0;
		wdGatewayID = 0;
		state = WAIT_LOGIN;
		bzero(client_ip,sizeof(client_ip));
	}
	
	/**
	 * \brief 拷贝构造函数
	 *
	 */
	BillInfo(const BillInfo &ai)
	{
		accid = ai.accid;
		loginTempID = ai.loginTempID;
		wdGatewayID = ai.wdGatewayID;
		state = ai.state;
		gold = 0;
		vip_time = 0;
		timestamp = ai.timestamp;
		strncpy(client_ip,ai.client_ip,sizeof(client_ip));
	}

	/**
	 * \brief 赋值操作符号
	 *
	 */
	BillInfo & operator= (const BillInfo &ai)
	{
		accid = ai.accid;
		loginTempID = ai.loginTempID;
		wdGatewayID = ai.wdGatewayID;
		state = ai.state;
		timestamp = ai.timestamp;
		strncpy(client_ip,ai.client_ip,sizeof(client_ip));

		return *this;
	}

};
#pragma pack()

/**
 * \brief 账号信息管理容器
 *
 * 保留了一个区中所有在线账号列表
 *
 */
class BillManager
{

	public:

		/**
		 * \brief 会话超时时间
		 * 单位，秒
		 */
		static const int session_timeout_value = 10;

		/**
		 * \brief 缺省析构函数
		 *
		 */
		~BillManager() {};

		/**
		 * \brief 返回类的唯一实例
		 *
		 * \return 类的唯一实例
		 */
		static BillManager &getInstance()
		{
			if (NULL == instance)
				instance = new BillManager();

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

		bool verify(const t_NewLoginSession &session);
		void update();
		void updateByGatewayID(const WORD wdGatewayID);
		bool login(const DWORD accid, const DWORD loginTempID);
		bool logout(const DWORD accid, const DWORD loginTempID);
		bool updateGold(DWORD acc , double gold);
		bool updateVipTime(DWORD acc , DWORD vip);
		DWORD getVipTime(DWORD acc);
		double getGold(DWORD acc);
		BillInfo get(const DWORD accid);

	private:

		/**
		 * \brief 类的唯一实例指针
		 *
		 */
		static BillManager *instance;

		/**
		 * \brief 缺省构造函数
		 *
		 */
		BillManager() {};

		/**
		 * \brief 账号管理容器类型
		 *
		 */
		typedef __gnu_cxx::hash_map<DWORD, BillInfo> BillInfoMap;
		/**
		 * \brief 定义容器的迭代器类型
		 *
		 */
		typedef BillInfoMap::iterator BillInfoMap_iterator;
		/**
		 * \brief 定义容器的键值对类型
		 *
		 */
		typedef BillInfoMap::value_type BillInfoMap_pair;

		/**
		 * \brief 容器访问互斥变量
		 *
		 */
		zMutex mlock;
		/**
		 * \brief 账号管理容器
		 *
		 */
		BillInfoMap infoMap;

};

#endif

