/**
 * \file
 * \version  $Id: LoginSessionManager.h  $
 * \author  
 * \date 
 * \brief 保存登陆会话建立之前的信息
 *
 * 
 */


#ifndef _LoginSessionManager_h_
#define _LoginSessionManager_h_

#include <ext/hash_map>

#include "zMutex.h"
#include "SuperCommand.h"
#include "Zebra.h"
#include "zTime.h"

#include "EncDec/EncDec.h"

/**
 * \brief 登陆会话管理器
 *
 */
class LoginSessionManager
{

	public:

		/**
		 * \brief 析构函数
		 *
		 */
		~LoginSessionManager() {};

		/**
		 * \brief 获取登陆会话管理器的唯一实例
		 *
		 * \return 会话管理器唯一实例
		 */
		static LoginSessionManager &getInstance()
		{
			if (NULL == instance)
				instance = new LoginSessionManager();

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

		void put(const t_NewLoginSession &session);
		bool verify(const DWORD loginTempID, const DWORD accid,char *numPassword, DES_cblock *key=0);
		void update(const zRTime &ct);

	private:

		/**
		 * \brief 会话管理容器唯一实例指针
		 *
		 */
		static LoginSessionManager *instance;

		/**
		 * \brief 构造函数
		 *
		 */
		LoginSessionManager() : lastUpdateTime() {};

		/**
		 * \brief 最后一次更新时间
		 *
		 */
		zRTime lastUpdateTime;

		/**
		 * \brief 检查更新间隔
		 *
		 */
		bool checkUpdateTime(const zRTime &ct)
		{
			bool retval = false;
			if (ct >= lastUpdateTime)
			{
				lastUpdateTime = ct;
				lastUpdateTime.addDelay(1000);
				retval = true;
			}
			return retval;
		}

		struct LoginSession
		{
			t_NewLoginSession session;
			zTime timestamp;

			LoginSession(const t_NewLoginSession &session) : session(session), timestamp()
			{
			}
			LoginSession(const LoginSession& ls)
			{
				session = ls.session;
				timestamp = ls.timestamp;
			}
			LoginSession & operator= (const LoginSession &ls)
			{
				session = ls.session;
				timestamp = ls.timestamp;
				return *this;
			}
		};
		/**
		 * \brief 定义容器类型
		 *
		 */
		typedef __gnu_cxx::hash_map<DWORD, LoginSession> LoginSessionHashmap;
		/**
		 * \brief 定义迭代器类型
		 *
		 */
		typedef LoginSessionHashmap::iterator LoginSessionHashmap_iterator;
		/**
		 * \brief 定义键值对类型
		 *
		 */
		typedef LoginSessionHashmap::value_type LoginSessionHashmap_pair;
		/**
		 * \brief 会话容器
		 *
		 */
		LoginSessionHashmap sessionData;
		/**
		 * \brief 容器访问互斥变量
		 *
		 */
		zMutex mlock;

};

#endif

