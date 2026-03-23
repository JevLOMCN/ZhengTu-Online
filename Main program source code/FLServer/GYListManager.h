/**
 * \file
 * \version  $Id: GYListManager.h  $
 * \author  
 * \date 
 * \brief 网关信息列表
 *
 * 登陆服务器需要保存最新的所有网关的信息列表，便于分配网关
 * 
 */


#ifndef _GYListManager_h_
#define _GYListManager_h_

#include <ext/hash_map>

#include "zType.h"
#include "zMisc.h"
#include "zMutex.h"
#include "Command.h"
#include "FLCommand.h"

/**
 * \brief 网关信息节点
 *
 */
struct GYList
{
	WORD wdServerID;			/**< 服务器编号 */
	BYTE pstrIP[MAX_IP_LENGTH];	/**< 服务器地址 */
	WORD wdPort;				/**< 服务器端口 */
	WORD wdNumOnline;			/**< 网关在线人数 */
	int  state;					/**< 服务器状态 */
	DWORD zoneGameVersion;

	/**
	 * \brief 缺省构造函数
	 *
	 */
	GYList()
	{
		wdServerID = 0;
		bzero(pstrIP, sizeof(pstrIP));
		wdPort = 0;
		wdNumOnline = 0;
		state = state_none;
		zoneGameVersion = 0;
	}

	/**
	 * \brief 拷贝构造函数
	 *
	 */
	GYList(const GYList& gy)
	{
		wdServerID = gy.wdServerID;
		bcopy(gy.pstrIP, pstrIP, sizeof(pstrIP));
		wdPort = gy.wdPort;
		wdNumOnline = gy.wdNumOnline;
		state = gy.state;
		zoneGameVersion = gy.zoneGameVersion;
	}

	/**
	 * \brief 赋值函数
	 *
	 */
	GYList & operator= (const GYList &gy)
	{
		wdServerID = gy.wdServerID;
		bcopy(gy.pstrIP, pstrIP, sizeof(pstrIP));
		wdPort = gy.wdPort;
		wdNumOnline = gy.wdNumOnline;
		state = gy.state;
		zoneGameVersion = gy.zoneGameVersion;
		return *this;
	}

};

/**
 * \brief 网关信息列表管理器
 *
 */
class GYListManager
{

	public:

		/**
		 * \brief 默认析构函数
		 *
		 */
		~GYListManager()
		{
			gyData.clear();
		}

		/**
		 * \brief 返回类的唯一实例
		 *
		 * 实现了Singleton设计模式，保证了一个进程中只有一个类的实例
		 *
		 */
		static GYListManager &getInstance()
		{
			if (NULL == instance)
				instance = new GYListManager;

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

		bool put(const GameZone_t &gameZone, const GYList &gy);
		void disableAll(const GameZone_t &gameZone);
		GYList *getAvl(const GameZone_t &gameZone);
		void full_ping_list(Cmd::stPingList* cmd, const GameZone_t& gameZone);
		bool verifyVer(const GameZone_t &gameZone, DWORD verify_client_version, BYTE &retcode);

	private:

		/**
		 * \brief 类的唯一实例指针
		 *
		 */
		static GYListManager *instance;

		/**
		 * \brief 默认构造函数
		 *
		 */
		GYListManager() {};

		/**
		 * \brief hash函数
		 *
		 */
		struct GameZone_hash
		{
			size_t operator()(const GameZone_t &gameZone) const
			{
				__gnu_cxx::hash<DWORD> H;
				return H(gameZone.id);
			}
		};
		/**
		 * \brief 定义容器类型
		 *
		 */
		typedef __gnu_cxx::hash_multimap<const GameZone_t, GYList, GameZone_hash> GYListContainer;
		/**
		 * \brief 定义容器迭代器类型
		 *
		 */
		typedef GYListContainer::iterator GYListContainer_iterator;
		/**
		 * \brief 定义容器键值对类型
		 *
		 */
		typedef GYListContainer::value_type GYListContainer_value_type;
		/**
		 * \brief 存储网关列表信息的容器
		 *
		 */
		GYListContainer gyData;
		/**
		 * \brief 互斥变量
		 *
		 */
		zMutex mlock;

};

#endif

