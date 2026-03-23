/**
 * \file
 * \version  $Id: RecordSessionManager.h  $
 * \author  
 * \date 
 * \brief 容器，用于保证不会出现重复登陆
 *
 * 
 */


#ifndef _RecordSessionManager_h_
#define _RecordSessionManager_h_

#include <ext/hash_map>

#include "zType.h"
#include "zMutex.h"
#include "zTime.h"

/**
 * \brief 角色档案读取写入的会话记录
 *
 */
struct RecordSession
{
	DWORD accid;			/// 帐号
	DWORD id;				/// 角色编号
	WORD  wdServerID;		/// 服务器编号
	zTime lastsavetime;		/// 最后一次存档时间

	/**
	 * \brief 缺省构造函数
	 *
	 */
	RecordSession(const DWORD accid, const DWORD id, const WORD wdServerID) : lastsavetime()
	{
		this->accid = accid;
		this->id = id;
		this->wdServerID = wdServerID;
	}

	/**
	 * \brief 拷贝构造函数
	 *
	 */
	RecordSession(const RecordSession& rs)
	{
		accid = rs.accid;
		id = rs.id;
		wdServerID = rs.wdServerID;
		lastsavetime = rs.lastsavetime;
	}

	/**
	 * \brief 赋值操作符号，没有实现，禁用掉了
	 *
	 */
	RecordSession & operator= (const RecordSession &rs);

	const bool operator== (const RecordSession &rs);
	/*{
		return (accid == rs.accid && id == rs.id);
	}*/

};

class RecordSessionManager
{

	public:

		/**
		 * \brief 默认析构函数
		 *
		 */
		~RecordSessionManager()
		{
			sessionMap.clear();
		}

		/**
		 * \brief 返回类的唯一实例
		 *
		 * 实现了Singleton设计模式，保证了一个进程中只有一个类的实例
		 *
		 */
		static RecordSessionManager &getInstance()
		{
			if (NULL == instance)
				instance = new RecordSessionManager;

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

		bool add(const DWORD accid, const DWORD id, const WORD wdServerID);
		bool verify(const DWORD accid, const DWORD id, const WORD wdServerID);
		bool remove(const DWORD accid, const DWORD id, const WORD wdServerID);
		void removeAllByServerID(const WORD wdServerID);
		bool empty()
		{
			return sessionMap.empty(); 
		}

	private:

		/**
		 * \brief 类的唯一实例指针
		 *
		 */
		static RecordSessionManager *instance;

		/**
		 * \brief 默认构造函数
		 *
		 */
		RecordSessionManager() {};

		/**
		 * \brief 定义容器类型
		 *
		 */
		typedef __gnu_cxx::hash_map<DWORD, RecordSession> RecordSessionHashmap;
		/**
		 * \brief 定义容器迭代器类型
		 *
		 */
		typedef RecordSessionHashmap::iterator RecordSessionHashmap_iterator;
		/**
		 * \brief 定义容器键值对类型
		 *
		 */
		typedef RecordSessionHashmap::value_type RecordSessionHashmap_pair;
		/**
		 * \brief 存储在线帐号列表信息的容器
		 *
		 */
		RecordSessionHashmap sessionMap;
		/**
		 * \brief 互斥变量
		 *
		 */
		zMutex mlock;

};

#endif

