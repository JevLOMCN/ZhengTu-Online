/**
 * \file
 * \version  $Id: ServerACL.h  $
 * \author  
 * \date 
 * \brief 存储有效服务器的列表
 * 有效服务器列表存储在xml文件中，服务器启动的时候读取这些信息到内存，
 * 当一个服务器管理器连接过来的时候，可以根据这些信息判断这个连接是否合法的。
 */

#ifndef _ServerACL_h_
#define _ServerACL_h_

#include <string>
#include <ext/hash_map>

#include "zType.h"
#include "zMisc.h"
#include "Zebra.h"
#include "zRWLock.h"
#include "zNoncopyable.h"

struct ACLZone
{
	GameZone_t gameZone;
	std::string ip;
	unsigned short port;
	std::string name;
	std::string desc;

	ACLZone()
	{
		port = 0;
	}
	ACLZone(const ACLZone &acl)
	{
		gameZone = acl.gameZone;
		ip = acl.ip;
		port = acl.port;
		name = acl.name;
		desc = acl.desc;
	}
};

class ServerACL : zNoncopyable
{

	public:

		ServerACL() {};
		~ServerACL() {};

		bool init();
		void final();
		bool check(const char *strIP, const unsigned short port, GameZone_t &gameZone, std::string &name);

	private:

		bool add(const ACLZone &zone);

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
		typedef __gnu_cxx::hash_map<const GameZone_t, ACLZone, GameZone_hash> Container;
		Container datas;
		zRWLock rwlock;

};

typedef singleton_default<ServerACL> ServerACLSingleton;

#endif

