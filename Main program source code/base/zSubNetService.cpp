/**
 * \file
 * \version  $Id: zSubNetService.cpp  $
 * \author  
 * \date 
 * \brief 实现网络服务器的框架代码
 *
 * 
 */

#include <iostream>
#include <string>
#include <deque>
#include <ext/numeric>

#include "zService.h"
#include "zThread.h"
#include "zSocket.h"
#include "zTCPServer.h"
#include "zNetService.h"
#include "zSubNetService.h"
#include "Zebra.h"
#include "SuperCommand.h"

zSubNetService *zSubNetService::subNetServiceInst = NULL;

/**
 * \brief 服务器管理器的连接客户端类
 *
 */
class SuperClient : public zTCPBufferClient
{

	public:

		friend class zSubNetService;

		/**
		 * \brief 构造函数
		 *
		 */
		SuperClient() : zTCPBufferClient("服务器管理器客户端"), verified(false)
		{
			//Zebra::logger->trace("SuperClient::SuperClient");
		}

		/**
		 * \brief 析构函数
		 *
		 */
		~SuperClient() {};

		void run();
		bool msgParse_Startup(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);
		bool msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen);

	private:

		bool verified;			/**< 是否已经通过了服务器管理器的验证 */

};

/**
 * \brief 重载zThread中的纯虚函数，是线程的主回调函数，用于处理接收到的指令
 *
 */
void SuperClient::run()
{
	Zebra::logger->trace("SuperClient::run");
	zTCPBufferClient::run();

	//与服务器管理器之间的连接断开，需要关闭服务器
	zSubNetService::subNetServiceInstance()->Terminate();
}

/**
 * \brief 解析来自服务器管理器的关于启动的指令
 *
 * \param ptNullCmd 待处理的指令
 * \param nCmdLen 指令长度
 * \return 解析是否成功
 */
bool SuperClient::msgParse_Startup(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	//Zebra::logger->trace("SuperClient::msgParse_Startup(%u,%u)",ptNullCmd->cmd, ptNullCmd->para);
	using namespace Cmd::Super;

	switch(ptNullCmd->para)
	{
		case PARA_GAMETIME:
			{
				t_GameTime *ptCmd = (t_GameTime *)ptNullCmd;

				//Zebra::logger->debug("刷新游戏时间：%lu", ptCmd->qwGameTime);
				Zebra::qwGameTime = ptCmd->qwGameTime;

				return true;
			}
			break;
		case PARA_STARTUP_RESPONSE:
			{
				t_Startup_Response *ptCmd = (t_Startup_Response *)ptNullCmd;

				Zebra::logger->debug("服务器管理器回应消息 %u %u %u", ptCmd->wdServerID, ptCmd->wdPort, ptCmd->wdExtPort);

				zSubNetService::subNetServiceInstance()->setServerInfo(ptCmd);
			

				return true;
			}

			break;
		case PARA_STARTUP_SERVERENTRY_NOTIFYME:
			{
				t_Startup_ServerEntry_NotifyMe *ptCmd = (t_Startup_ServerEntry_NotifyMe *)ptNullCmd;

				Zebra::logger->debug("收到启动NotifyMe请求,自己所依赖的服务器列表");
				for(WORD i = 0; i < ptCmd->size; i++)
				{
					Zebra::logger->debug("serverid=%u, servertype=%u, servername=%s,serverip=%s, port=%u, extip=%s, extport%u, state=%u",
							ptCmd->entry[i].wdServerID,
							ptCmd->entry[i].wdServerType,
							ptCmd->entry[i].pstrName,
							ptCmd->entry[i].pstrIP,
							ptCmd->entry[i].wdPort,
							ptCmd->entry[i].pstrExtIP,
							ptCmd->entry[i].wdExtPort,
							ptCmd->entry[i].state);
					//需要一个容器来管理这些服务器列表
					zSubNetService::subNetServiceInstance()->addServerEntry(ptCmd->entry[i]);
				}

				if (!verified)
					verified = true;

				return true;
			}

			break;
		case PARA_STARTUP_SERVERENTRY_NOTIFYOTHER://1,4
			{
				t_Startup_ServerEntry_NotifyOther *ptCmd = (t_Startup_ServerEntry_NotifyOther *)ptNullCmd;

				Zebra::logger->debug("返回依赖于自己的服务器");
				Zebra::logger->debug("serverid=%u, servertype=%u, servername=%s,serverip=%s, port=%u, extip=%s, extport%u, state=%u",
						ptCmd->entry.wdServerID,
						ptCmd->entry.wdServerType,
						ptCmd->entry.pstrName,
						ptCmd->entry.pstrIP,
						ptCmd->entry.wdPort,
						ptCmd->entry.pstrExtIP,
						ptCmd->entry.wdExtPort,
						ptCmd->entry.state);
				//需要一个容器来管理这些服务器列表
				zSubNetService::subNetServiceInstance()->addServerEntry(ptCmd->entry);
				// 发送命令到SuperServer
				return sendCmd(ptCmd, nCmdLen);
			}

			break;
		default:
			break;
	}

	//Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

/**
 * \brief 解析来自服务器管理器的指令
 *
 * \param ptNullCmd 待处理的指令
 * \param nCmdLen 指令长度
 * \return 解析是否成功
 */
bool SuperClient::msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
//	Zebra::logger->trace("SuperClient::msgParse(%u,%u)",ptNullCmd->cmd, ptNullCmd->para);
	using namespace Cmd::Super;

	switch(ptNullCmd->cmd)
	{
		case CMD_STARTUP:
			if (msgParse_Startup(ptNullCmd, nCmdLen))
			{
				return true;
			}

			break;
		default:
			if (zSubNetService::subNetServiceInstance()->msgParse_SuperService(ptNullCmd, nCmdLen))
			{
				return true;
			}

			break;
	}

	//Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

/**
 * \brief 构造函数
 * 
 * \param name 名称
 * \param wdType 服务器类型
 */
zSubNetService::zSubNetService(const std::string &name, const WORD wdType) : zNetService(name), superClient(NULL)
{
	//Zebra::logger->trace("zSubNetService::zSubNetService");

	subNetServiceInst = this;

	superPort = atoi(Zebra::global["port"].c_str());
	bzero(superIP, sizeof(superIP));
	strncpy(superIP, Zebra::global["server"].c_str(), sizeof(superIP) - 1);

	superClient = new SuperClient();

	wdServerID = 0;
	wdServerType = wdType;
	bzero(pstrName, sizeof(pstrName));
	bzero(pstrIP, sizeof(pstrIP));
	wdPort = 0;
	bzero(pstrExtIP, sizeof(pstrExtIP));
	wdExtPort = 0;
}

/**
 * \brief 虚析构函数
 *
 */
zSubNetService::~zSubNetService()
{
	Zebra::logger->trace("zSubNetService::~zSubNetService");
	serverList.clear();

	SAFE_DELETE(superClient);

	subNetServiceInst = NULL;
}

/**
 * \brief 初始化网络服务器程序
 *
 * 实现纯虚函数<code>zService::init</code>
 * 建立到服务器管理器的连接，并得到服务器信息
 *
 * \return 是否成功
 */
bool zSubNetService::init()
{
	Zebra::logger->trace("zSubNetService::init");

	//建立到服务器管理器的连接
	if (!superClient->connect(superIP, superPort))
	{
		Zebra::logger->error("连接服务器管理器失败(%s:%u)",superIP,superPort);
		return false;
	}

	//发送登陆服务器管理器的指令
	using namespace Cmd::Super;
	t_Startup_Request tCmd;
	tCmd.wdServerType = wdServerType;
	strcpy(tCmd.pstrIP, pstrIP);
	if (!superClient->sendCmd(&tCmd, sizeof(tCmd)))
	{
		Zebra::logger->error("(zSubNetService::init)向服务器管理器发送登陆指令失败");
		return false;
	}

	//等待服务器管理器返回信息
	while(!superClient->verified)
	{
		
		unsigned char pstrCmd[zSocket::MAX_DATASIZE];
		int nCmdLen = superClient->pSocket->recvToCmd(pstrCmd, sizeof(pstrCmd), true);		
		if (-1 == nCmdLen)
		{
			Zebra::logger->error("(zSubNetService::init)等待服务器管理器返回信息失败");
			return false;
		}
		else if (nCmdLen > 0)
		{
			Cmd::t_NullCmd *cmd = 0;
			cmd = (Cmd::t_NullCmd *)pstrCmd ;			
			if (!superClient->msgParse((Cmd::t_NullCmd *)pstrCmd, nCmdLen))
				return false;
		}
	}
		
	Zebra::logger->trace("zSubNetService::init wdServerID=%u",wdServerID);

	//zThread::sleep(1);

	//建立线程与服务器管理器交互
	superClient->start();

	//调用真实的初始化函数
	if (!zNetService::init(wdPort))
		return false;

	return true;
}

/**
 * \brief 确认服务器初始化成功，即将进入主回调函数
 *
 * 向服务器发送t_Startup_OK指令来确认服务器启动成功
 *
 * \return 确认是否成功
 */
bool zSubNetService::validate()
{
	Zebra::logger->trace("zSubNetService::validate");
	Cmd::Super::t_Startup_OK tCmd;
	tCmd.wdServerID = wdServerID;
	return superClient->sendCmd(&tCmd, sizeof(tCmd));
}

/**
 * \brief 结束网络服务器
 *
 * 实现纯虚函数<code>zService::final</code>
 *
 */
void zSubNetService::final()
{
	Zebra::logger->trace("zSubNetService::final");
	zNetService::final();
	
	//关闭到服务器管理器的连接
	superClient->final();
	superClient->join();
	superClient->close();
}

/**
 * \brief 向服务器管理器发送指令
 *
 * \param pstrCmd 待发送的指令
 * \param nCmdLen 待发送指令的大小
 * \return 发送是否成功
 */
bool zSubNetService::sendCmdToSuperServer(const void *pstrCmd, const int nCmdLen)
{
	Zebra::logger->trace("zSubNetService::sendCmdToSuperServer");
	return superClient->sendCmd(pstrCmd, nCmdLen);
}

/**
 * \brief 根据服务器管理器返回信息，设置服务器的信息
 *
 * \param ptCmd 服务器管理器返回信息
 */
void zSubNetService::setServerInfo(const Cmd::Super::t_Startup_Response *ptCmd)
{	
	wdServerID = ptCmd->wdServerID;
	wdPort = ptCmd->wdPort;
	bcopy(ptCmd->pstrExtIP,pstrExtIP,MAX_IP_LENGTH);
	wdExtPort = ptCmd->wdExtPort;
	Zebra::logger->debug("zSubNetService::setServerInfo IP=%s Port=%u ServerID=%u",ptCmd->pstrExtIP,wdExtPort,wdServerID);
}

/**
 * \brief 添加关联服务器信息到一个容器中
 *
 */
void zSubNetService::addServerEntry(const Cmd::Super::ServerEntry &entry)
{
	Zebra::logger->trace("zSubNetService::addServerEntry(wdserverID=%u)",entry.wdServerID);
	mlock.lock();
	//首先查找有没有重复的
	std::deque<Cmd::Super::ServerEntry>::iterator it;
	bool found = false;
	for(it = serverList.begin(); it != serverList.end(); it++)
	{
		if (entry.wdServerID == it->wdServerID)
		{
			found = true;
			break;
		}
	}

	if (found)
	{
		//已经存在只是更新
		(*it) = entry;
	}
	else
	{
		//还不存在，需要新建立一个节点
		serverList.push_back(entry);
	}
	mlock.unlock();
}

/**
 * \brief 查找相关服务器信息
 *
 * \param wdServerID 服务器编号
 * \return 服务器信息
 */
const Cmd::Super::ServerEntry *zSubNetService::getServerEntry(const WORD wdServerID)
{
	Zebra::logger->trace("zSubNetService::getServerEntry(%u)",wdServerID);
	Cmd::Super::ServerEntry *ret = NULL;
	std::deque<Cmd::Super::ServerEntry>::iterator it;
	mlock.lock();
	for(it = serverList.begin(); it != serverList.end(); it++)
	{
		if (wdServerID == it->wdServerID)
		{
			ret = &(*it);
			break;
		}
	}
	mlock.unlock();
	return ret;
}

/**
 * \brief 查找相关服务器信息
 *
 * \param wdServerType 服务器类型
 * \return 服务器信息
 */
const Cmd::Super::ServerEntry *zSubNetService::getServerEntryByType(const WORD wdServerType)
{
	Zebra::logger->trace("zSubNetService::getServerEntryByType(type=%u)",wdServerType);
	Cmd::Super::ServerEntry *ret = NULL;
	std::deque<Cmd::Super::ServerEntry>::iterator it;
	mlock.lock();
	for(it = serverList.begin(); it != serverList.end(); it++)
	{
		Zebra::logger->debug("服务器信息：%u, %u", wdServerType, it->wdServerType);
		if (wdServerType == it->wdServerType)
		{
			ret = &(*it);
			break;
		}
	}
	mlock.unlock();
	return ret;
}

/**
 * \brief 查找相关服务器信息
 *
 * \param wdServerType 服务器类型
 * \param prev 上一个服务器信息
 * \return 服务器信息
 */
const Cmd::Super::ServerEntry *zSubNetService::getNextServerEntryByType(const WORD wdServerType, const Cmd::Super::ServerEntry **prev)
{
	Zebra::logger->trace("zSubNetService::getNextServerEntryByType");
	Cmd::Super::ServerEntry *ret = NULL;
	bool found = false;
	std::deque<Cmd::Super::ServerEntry>::iterator it;
	mlock.lock();
	for(it = serverList.begin(); it != serverList.end(); it++)
	{
		Zebra::logger->debug("服务器信息：%u, %u", wdServerType, it->wdServerType);
		if (wdServerType == it->wdServerType)
		{
			if (NULL == prev
					|| found)
			{
				ret = &(*it);
				break;
			}
			else if (found == false
					&& (*prev)->wdServerID == it->wdServerID)
			{
				found = true;
			}
		}
	}
	mlock.unlock();
	return ret;
}

