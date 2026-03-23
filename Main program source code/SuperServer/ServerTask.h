/**
 * \file
 * \version  $Id: ServerTask.h  $
 * \author  
 * \date 
 * \brief 定义服务器连接任务
 *
 * 一个区中的每一个服务器都需要和服务器管理器建立连接
 * 
 */

#ifndef _ServerTask_h_
#define _ServerTask_h_

#include <ext/hash_map>

#include "zTCPServer.h"
#include "zTCPTask.h"
#include "zService.h"
#include "zMisc.h"
#include "zTime.h"
#include "SuperCommand.h"

/**
 * \brief 服务器连接任务
 *
 * 一个区中的每一个服务器都需要和服务器管理器建立连接
 * 
 */
class ServerTask : public zTCPTask
{

	public:

		/**
		 * \brief 构造函数
		 *
		 * 用于创建一个服务器连接任务
		 *
		 * \param pool 所属连接池指针
		 * \param sock TCP/IP套接口
		 * \param addr 地址
		 */
		ServerTask(
				zTCPTaskPool *pool,
				const int sock,
				const struct sockaddr_in *addr = NULL) : zTCPTask(pool, sock, addr)
		{
			wdServerID = 0;
			wdServerType = UNKNOWNSERVER;
			bzero(pstrName, sizeof(pstrName));
			bzero(pstrIP, sizeof(pstrIP));
			wdPort = 0;
			bzero(pstrExtIP, sizeof(pstrExtIP));
			wdExtPort = 0;

			OnlineNum = 0;

			sequenceOK = false;
			hasNotifyMe = false;
			hasprocessSequence = false;
		}

		/**
		 * \brief 虚析构函数
		 *
		 */
		virtual ~ServerTask() {};

		int verifyConn();
		int waitSync();
		int recycleConn();
		void addToContainer();
		void removeFromContainer();
		bool uniqueAdd();
		bool uniqueRemove();
		bool msgParse(const Cmd::t_NullCmd *, const unsigned int);
		void responseOther(const WORD wdServerID);

		/**
		 * \brief 获取服务器编号
		 *
		 * \return 服务器编号
		 */
		const WORD getID() const
		{
			return wdServerID;
		}

		/**
		 * \brief 获取服务器类型
		 * \return 服务器类型
		 */
		const WORD getType() const
		{
			return wdServerType;
		}

		/**
		 * \brief 返回服务器在线人数
		 * \return 服务器在线人数
		 */
		const DWORD getOnlineNum() const
		{
			return OnlineNum;
		}

		/**
		 * \brief 检查最后一次处理启动顺序的时间
		 *
		 * \return 检查是否成功
		 */
		bool checkSequenceTime()
		{
			//启动顺序处理已经完成了，不需要再次处理
			if (sequenceOK)
				return false;

			//检测两次处理的间隔时间
			zTime currentTime;
			if (lastSequenceTime.elapse(currentTime) > 2)
			{
				lastSequenceTime = currentTime;
				return true;
			}

			return false;
		}

	private:

		WORD wdServerID;					/**< 服务器编号，一个区唯一的 */
		WORD wdServerType;					/**< 服务器类型，创建类实例的时候已经确定 */
		char pstrName[MAX_NAMESIZE];		/**< 服务器名称 */
		char pstrIP[MAX_IP_LENGTH];			/**< 服务器内网地址 */
		WORD wdPort;						/**< 服务器内网端口，也就是邦定端口 */
		char pstrExtIP[MAX_IP_LENGTH];		/**< 服务器外网地址，也就是防火墙地址 */
		WORD wdExtPort;						/**< 服务器外网端口，也就是映射到防火墙的端口 */

		DWORD			OnlineNum;			/**< 在线人数统计 */

		zTime lastSequenceTime;				/**< 最后一次处理启动顺序的时间 */
		bool sequenceOK;					/**< 是否已经处理完成了启动顺序 */
		bool hasNotifyMe;
		bool hasprocessSequence;

		bool verify(WORD wdType, const char *pstrIP);
		bool verifyTypeOK(const WORD wdType, std::vector<ServerTask *> &sv);
		bool processSequence();
		bool notifyOther();
		bool notifyOther(WORD dstID);
		bool notifyMe();

		bool msgParse_Startup(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);
		bool msgParse_Bill(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);
		bool msgParse_Gateway(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);
		bool msgParse_GmTool(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);
		bool msgParse_CountryOnline(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);

		struct key_hash
		{
			size_t operator()(const Cmd::Super::ServerEntry &x) const
			{
				__gnu_cxx::hash<WORD> H;
				return H(x.wdServerID);
			}
		};
		struct key_equal : public std::binary_function<Cmd::Super::ServerEntry, Cmd::Super::ServerEntry, bool>
		{
			bool operator()(const Cmd::Super::ServerEntry &s1, const Cmd::Super::ServerEntry &s2) const
			{
				return s1.wdServerID == s2.wdServerID;
			}
		};
		typedef __gnu_cxx::hash_map<Cmd::Super::ServerEntry, bool, key_hash, key_equal> Container;
		// 连接进来的服务器列表
		Container ses;

};

#endif

