/**
 * \file
 * \version  $Id: LoginManager.h  $
 * \author  
 * \date 
 * \brief 登陆连接管理容器
 *
 * 
 */


#ifndef _LoginManager_h_
#define _LoginManager_h_

#include <ext/hash_map>

#include "zEntry.h"
#include "LoginTask.h"
#include "DBAccessCommand.h"

/**
 * \brief 登陆连接管理容器
 *
 * 管理所有的登陆连接的容器，方便查找连接
 *
 */
class LoginManager
{

	public:

		/**
		 ** \brief 网关最大容纳用户数目
		 **
		 **/
		static DWORD maxGatewayUser;

		/**
		 * \brief 定义回调函数类
		 *
		 */
		typedef zEntryCallback<LoginTask, void> LoginTaskCallback;

		/**
		 * \brief 析构函数
		 *
		 */
		~LoginManager() {};

		/**
		 * \brief 获取管理容器的唯一实例
		 *
		 * 容器实现了Singleton设计模式，保证了一个进程中只有一个类的实例
		 */
		static LoginManager &getInstance()
		{
			if (NULL == instance)
				instance = new LoginManager();

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

		bool add(LoginTask *task);
		void remove(LoginTask *task);
		bool broadcast(const DWORD loginTempID, const void *pstrCmd, int nCmdLen);
		void verifyReturn(const DWORD loginTempID, const BYTE retcode, const t_NewLoginSession &session);
		void loginReturn(const DWORD loginTempID, const BYTE retcode, const bool tm = true);
		void execAll(LoginTaskCallback &cb);

	private:

		/**
		 * \brief 构造函数
		 *
		 */
		LoginManager(){};

		/**
		 * \brief 类的唯一实例指针
		 *
		 */
		static LoginManager *instance;
		/**
		 * \brief 定义容器类型
		 *
		 */
		typedef __gnu_cxx::hash_map<DWORD, LoginTask *> LoginTaskHashmap;
		/**
		 * \brief 定义容器迭代器类型
		 *
		 */
		typedef LoginTaskHashmap::iterator LoginTaskHashmap_iterator;
		/**
		 * \brief 定义容器常量迭代器类型
		 *
		 */
		typedef LoginTaskHashmap::const_iterator LoginTaskHashmap_const_iterator;
		/**
		 * \brief 定义容器键值对类型
		 *
		 */
		typedef LoginTaskHashmap::value_type LoginTaskHashmap_pair;
		/**
		 * \brief 互斥变量，保证原子访问容器
		 *
		 */
		zMutex mlock;
		/**
		 * \brief 子连接管理容器类型
		 *
		 */
		LoginTaskHashmap loginTaskSet;

};

#endif

