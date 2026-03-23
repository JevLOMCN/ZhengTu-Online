/**
 * \file
 * \version  $Id: zSubNetService.h  $
 * \author  
 * \date 
 * \brief 实现网络服务器的框架代码
 *
 * 这个主要是使用与需要连接服务器管理器的网络服务器
 * 
 */

#ifndef _zSubNetService_h_
#define _zSubNetService_h_

#include <iostream>
#include <string>
#include <deque>
#include <ext/numeric>

#include "zService.h"
#include "zThread.h"
#include "zSocket.h"
#include "zTCPServer.h"
#include "zNetService.h"
#include "zTCPClient.h"
#include "zMisc.h"
#include "Zebra.h"
#include "SuperCommand.h"
#include "Zebra.h"

class SuperClient;

/**
 * \brief 网络服务器框架代码
 *
 * 在需要与服务器管理器建立连接的网络服务器中使用
 *
 */
class zSubNetService : public zNetService
{

	public:

		virtual ~zSubNetService();

		/**
		 * \brief 获取类的唯一实例
		 *
		 * 这个类实现了Singleton设计模式，保证了一个进程中只有一个类的实例
		 *
		 */
		static zSubNetService *subNetServiceInstance()
		{
			return subNetServiceInst;
		}

		/**
		 * \brief 解析来自服务器管理器的指令
		 *
		 * 这些指令是与具体的服务器有关的，因为通用的指令都已经处理了
		 *
		 * \param ptNullCmd 待处理的指令
		 * \param nCmdLen 指令长度
		 * \return 解析是否成功
		 */
		virtual bool msgParse_SuperService(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen) = 0;

		bool sendCmdToSuperServer(const void *pstrCmd, const int nCmdLen);
		void setServerInfo(const Cmd::Super::t_Startup_Response *ptCmd);
		void addServerEntry(const Cmd::Super::ServerEntry &entry);
		const Cmd::Super::ServerEntry *getServerEntry(const WORD wdServerID);
		const Cmd::Super::ServerEntry *getServerEntryByType(const WORD wdServerType);
		const Cmd::Super::ServerEntry *getNextServerEntryByType(const WORD wdServerType, const Cmd::Super::ServerEntry **prev);

		/**
		 * \brief 返回服务器编号
		 *
		 * \return 服务器编号
		 */
		const WORD getServerID() const
		{
			return wdServerID;
		}

		/**
		 * \brief 返回服务器类型
		 *
		 * \return 服务器类型
		 */
		const WORD getServerType() const
		{
			return wdServerType;
		}

	protected:

		zSubNetService(const std::string &name, const WORD wdType);

		bool init();
		bool validate();
		void final();

		WORD wdServerID;					/**< 服务器编号，一个区唯一的 */
		WORD wdServerType;					/**< 服务器类型，创建类实例的时候已经确定 */
		char pstrName[MAX_NAMESIZE];		/**< 服务器名称 */
		char pstrIP[MAX_IP_LENGTH];			/**< 服务器内网地址 */
		WORD wdPort;						/**< 服务器内网端口，也就是邦定端口 */
		char pstrExtIP[MAX_IP_LENGTH];		/**< 服务器外网地址，也就是防火墙地址 */
		WORD wdExtPort;						/**< 服务器外网端口，也就是映射到防火墙的端口 */

	private:

		unsigned short superPort;		/**< 服务器管理器的端口 */
		char superIP[MAX_IP_LENGTH];	/**< 服务器管理器的地址 */

		SuperClient *superClient;		/**< 服务器管理器的客户端实例 */

		static zSubNetService *subNetServiceInst;			/**< 类的唯一实例指针，包括派生类，初始化为空指针 */
		zMutex mlock;										/**< 关联服务器信息列表访问互斥体 */
		std::deque<Cmd::Super::ServerEntry> serverList;		/**< 关联服务器信息列表，保证服务器之间的验证关系 */

};

#endif

