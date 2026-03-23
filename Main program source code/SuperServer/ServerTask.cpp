/**
 * \file
 * \version  $Id: ServerTask.cpp  $
 * \author  
 * \date 
 * \brief 实现服务器连接类
 *
 * 
 */


#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <ext/hash_map>
#include <stdio.h>

#include "zTCPServer.h"
#include "zTCPTask.h"
#include "zService.h"
#include "ServerTask.h"
#include "SuperCommand.h"
#include "ServerManager.h"
#include "Zebra.h"
#include "zDBConnPool.h"
#include "SuperServer.h"
#include "zString.h"
#include "SuperServer.h"
#include "FLCommand.h"
#include "FLClient.h"
#include "FLClientManager.h"
#include "RoleregCommand.h"
#include "RoleregClient.h"
#include "RoleregClientManager.h"
#include "RoleregCache.h"
#include "InfoCommand.h"
#include "GmToolCommand.h"
#include "InfoClient.h"
#include "InfoClientManager.h"

/**
 * \brief 验证一个服务器连接是否合法
 *
 * 每一台服务器的信息在数据库中都有记录，如果数据库中没有相应的记录，那么这个服务器连接任务就是不合法的，需要立即断开连接<br>
 * 这样保证了一个区中的服务器之间的信任关系
 *
 * \param wdType 服务器类型
 * \param pstrIP 服务器地址
 * \return 验证是否成功
 */
bool ServerTask::verify(WORD wdType, const char *pstrIP)
{
	Zebra::logger->trace("ServerTask::verify(%s:%u)",pstrIP,wdType);
	char where[64];

	this->wdServerType = wdType;
	strncpy(this->pstrIP, pstrIP, sizeof(this->pstrIP) - 1);
	Zebra::logger->debug("%s \t%u, %s", __FUNCTION__, wdType, pstrIP);

	connHandleID handle = SuperService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能从数据库连接池获取连接句柄");
		return false;
	}

	bzero(where, sizeof(where));
	std::string escapeIP;
	snprintf(where, sizeof(where) - 1, "`TYPE`=%u AND `IP`='%s'", wdType, SuperService::dbConnPool->escapeString(handle,pstrIP,escapeIP).c_str());
	static const dbCol col_define[] =
	{
		{"ID",zDBConnPool::DB_WORD,sizeof(WORD)},
		{"NAME",zDBConnPool::DB_STR,sizeof(char[MAX_NAMESIZE])},
		{"PORT",zDBConnPool::DB_WORD,sizeof(WORD)},
		{"EXTIP",zDBConnPool::DB_STR,sizeof(char[MAX_IP_LENGTH])},
		{"EXTPORT",zDBConnPool::DB_WORD,sizeof(WORD)},
		{NULL, 0, 0}
	};
	struct
	{
		WORD wdServerID;
		char pstrName[MAX_NAMESIZE];
		WORD wdPort;
		char pstrExtIP[MAX_IP_LENGTH];
		WORD wdExtPort;
	} __attribute__ ((packed))
	*pData = NULL;

	unsigned int retcode = SuperService::dbConnPool->exeSelect(handle, "`SERVERLIST`", col_define, where, "`ID`",(unsigned char **)&pData);
	if (retcode == (unsigned int)-1
			|| retcode == 0
			|| NULL == pData)
	{
		Zebra::logger->error("数据库中没有相应的服务器记录 %u",wdType);
		SuperService::dbConnPool->putHandle(handle);
		return false;
	}

	SuperService::dbConnPool->putHandle(handle);

	//某些类型服务器在一个区中只能由一台
	if (retcode > 1
			&& (wdType == BILLSERVER
			|| wdType == SESSIONSERVER)
	)
	{
		SAFE_DELETE_VEC(pData);
		Zebra::logger->error("这种类型的服务器只能有一台 %u", wdType);
		return false;
	}

	//从数据库中取数据成功，需要从这些数据中取得一个可用项
	unsigned int i;
	for(i = 0; i < retcode; i++)
	{
		if (ServerManager::getInstance().uniqueVerify(pData[i].wdServerID))
		{
			wdServerID = pData[i].wdServerID;
			strncpy(pstrName, pData[i].pstrName, sizeof(pstrName) - 1);
			wdPort = pData[i].wdPort;
			strncpy(pstrExtIP, pData[i].pstrExtIP, sizeof(pstrExtIP) - 1);
			wdExtPort = pData[i].wdExtPort;
			break;
		}
	}
	SAFE_DELETE_VEC(pData);
	if (i == retcode)
	{
		Zebra::logger->error("服务器已经启动完成了，没有可用记录");
		return false;
	}

	//返回服务器信息到服务器
	using namespace Cmd::Super;
	t_Startup_Response tCmd;
	tCmd.wdServerID = wdServerID;
	tCmd.wdPort = wdPort;
	bcopy(pstrExtIP, tCmd.pstrExtIP, sizeof(pstrExtIP));
	tCmd.wdExtPort = wdExtPort;
	if (!sendCmd(&tCmd, sizeof(tCmd)))
	{
		
		Zebra::logger->error("向服务器发送指令失败%u(%s:%u),wdServerID,pstrExtIP,wdExprot");
		return false;
	}

	return true;
}

/**
 * \brief 等待接受验证指令并进行验证
 *
 * 实现虚函数<code>zTCPTask::verifyConn</code>
 *
 * \return 验证是否成功，或者超时
 */
int ServerTask::verifyConn()
{
	Zebra::logger->trace("ServerTask::verifyConn");
	int retcode = mSocket.recvToBuf_NoPoll();
	if (retcode > 0)
	{
		unsigned char pstrCmd[zSocket::MAX_DATASIZE];
		int nCmdLen = mSocket.recvToCmd_NoPoll(pstrCmd, sizeof(pstrCmd));
		if (nCmdLen <= 0)
			//这里只是从缓冲取数据包，所以不会出错，没有数据直接返回
			return 0;
		else
		{
			using namespace Cmd::Super;

			t_Startup_Request *ptCmd = (t_Startup_Request *)pstrCmd;
			if (CMD_STARTUP == ptCmd->cmd
					&& PARA_STARTUP_REQUEST == ptCmd->para)
			{
				if (verify(ptCmd->wdServerType, ptCmd->pstrIP))
				{
					Zebra::logger->debug("客户端连接通过验证(%s:%u)",ptCmd->pstrIP,ptCmd->wdServerType);
					return 1;
				}
				else
				{
					Zebra::logger->error("客户端连接验证失败(%s:%u)",ptCmd->pstrIP,ptCmd->wdServerType);
					return -1;
				}
			}
			else
			{
				Zebra::logger->error("客户端连接指令验证失败(%s:%u)",ptCmd->pstrIP,ptCmd->wdServerType);
				return -1;
			}
		}
	}
	else
		return retcode;
}

/**
 * \brief 保存服务器之间的依赖关系
 *
 */
static __gnu_cxx::hash_map<int, std::vector<int> > serverSequence;

/**
 * \brief 初始化服务器之间的依赖关系
	
   gcc 
 *
 */
static void initServerSequence() __attribute__ ((constructor));
void initServerSequence()
{
	//printf("initServerSequence\r\n");
	serverSequence[UNKNOWNSERVER]	=	std::vector<int>();
	serverSequence[SUPERSERVER]	=	std::vector<int>();
	serverSequence[LOGINSERVER]	=	std::vector<int>();
	serverSequence[RECORDSERVER]	=	std::vector<int>();
	serverSequence[MINISERVER]	=	std::vector<int>();

	int data0[] = { RECORDSERVER };
	serverSequence[SESSIONSERVER]	=	std::vector<int>(data0, data0 + sizeof(data0) / sizeof(int));
	int data1[] = { RECORDSERVER, SESSIONSERVER , MINISERVER};
	serverSequence[SCENESSERVER]	=	std::vector<int>(data1, data1 + sizeof(data1) / sizeof(int));
	int data2[] = { RECORDSERVER, BILLSERVER, SESSIONSERVER, SCENESSERVER ,MINISERVER};
	serverSequence[GATEWAYSERVER]	=	std::vector<int>(data2, data2 + sizeof(data2) / sizeof(int));
	
}

/**
 * \brief 验证某种类型的所有服务器是否完全启动完成
 *
 * \param wdType 服务器类型
 * \param sv 容器，容纳启动成功的服务器列表
 * \return 验证是否成功
 */
bool ServerTask::verifyTypeOK(const WORD wdType, std::vector<ServerTask *> &sv)
{
	Zebra::logger->trace("ServerTask::verifyTypeOK(wdType=%u)",wdType);
	static const dbCol col_define[] =
	{
		{"ID",zDBConnPool::DB_WORD,sizeof(WORD)},
		{NULL, 0, 0}
	};
	char where[64];
	WORD *ID = NULL;

	if (0 == wdType)
		return true;

	bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "`TYPE`=%u", wdType);

	connHandleID handle = SuperService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能从数据库连接池获取连接句柄");
		return false;
	}

	unsigned int retcode = SuperService::dbConnPool->exeSelect(handle, "`SERVERLIST`", col_define, where, "`ID`",(unsigned char **)&ID);
	if (retcode == (unsigned int)-1
			|| retcode == 0
			|| NULL == ID)
	{
		Zebra::logger->error("数据库中没有相应的服务器记录 %u",wdType);
		SuperService::dbConnPool->putHandle(handle);
		return false;
	}
	SuperService::dbConnPool->putHandle(handle);

	bool retval = true;
	for(unsigned int i = 0; i < retcode; i++)
	{
		//这个容器里面的肯定是OK状态
		ServerTask * pServer = ServerManager::getInstance().getServer(ID[i]);
		if (NULL == pServer)
		{
			retval = false;
			break;
		}
		else
		{
			sv.push_back(pServer);
		}
	}
	SAFE_DELETE_VEC(ID);

	return retval;
}

/**
 * \brief 通知所有依赖的服务器
 * \return 通知是否成功
 */
bool ServerTask::notifyOther(WORD dstID)
{
	Zebra::logger->trace("ServerTask::notifyOther(dstID =%u)",dstID);
	using namespace Cmd::Super;

	t_Startup_ServerEntry_NotifyOther Cmd;

	bzero(&Cmd.entry, sizeof(Cmd.entry));
	Cmd.entry.wdServerID = wdServerID;
	Cmd.entry.wdServerType = wdServerType;
	strncpy(Cmd.entry.pstrName, pstrName, MAX_NAMESIZE - 1);
	strncpy(Cmd.entry.pstrIP, pstrIP, MAX_IP_LENGTH - 1);
	Cmd.entry.wdPort = wdPort;
	strncpy(Cmd.entry.pstrExtIP, pstrExtIP, MAX_IP_LENGTH - 1);
	Cmd.entry.wdExtPort = wdExtPort;
	Cmd.entry.state = state;

	for(Container::iterator it = ses.begin(); it != ses.end(); ++it)
	{
		if(dstID == it->first.wdServerID)
		{
			ServerTask * pDst = ServerManager::getInstance().getServer(dstID);
			if(pDst)
			{
				pDst->sendCmd(&Cmd, sizeof(Cmd));
			}
			break;
		}
	}

	return true;
}
/**
 * \brief 通知所有依赖的服务器
 * \return 通知是否成功
 */
bool ServerTask::notifyOther()
{
	Zebra::logger->trace("ServerTask::notifyOther()");
	using namespace Cmd::Super;
	t_Startup_ServerEntry_NotifyOther Cmd;
	bool retval = true;

	bzero(&Cmd.entry, sizeof(Cmd.entry));
	Cmd.entry.wdServerID = wdServerID;
	Cmd.entry.wdServerType = wdServerType;
	strncpy(Cmd.entry.pstrName, pstrName, MAX_NAMESIZE - 1);
	strncpy(Cmd.entry.pstrIP, pstrIP, MAX_IP_LENGTH - 1);
	Cmd.entry.wdPort = wdPort;
	strncpy(Cmd.entry.pstrExtIP, pstrExtIP, MAX_IP_LENGTH - 1);
	Cmd.entry.wdExtPort = wdExtPort;
	Cmd.entry.state = state;

	for(Container::iterator it = ses.begin(); it != ses.end(); ++it)
	{
		Cmd.srcID = it->first.wdServerID;
		// 通知依赖的服务器
		bool curval = ServerManager::getInstance().broadcastByID(Cmd.srcID, &Cmd, sizeof(Cmd));
		Zebra::logger->trace("ServerTask::notifyOther()srcid=%u curval = %u",Cmd.srcID,curval);
		retval &= curval;
	}
	return retval;
}

/**
 * \brief 收到notifyOther回复
 * \param wdServerID 目的服务器编号
 */
void ServerTask::responseOther(const WORD wdServerID)
{
	Zebra::logger->trace("ServerTask::responseOther(%u)",wdServerID);
	for(Container::iterator it = ses.begin(); it != ses.end(); ++it)
	{
		if (it->first.wdServerID == wdServerID)
		{
			Zebra::logger->debug("回复成功 %s, %d", it->first.pstrName, it->first.wdServerID);
			it->second = true;
		}
	}
}

/**
 * \brief 通知服务器其依赖的服务器信息列表
 * \return 通知是否成功
 */
bool ServerTask::notifyMe()
{	
	if (hasNotifyMe) return true;

	Zebra::logger->trace("ServerTask::notifyMe(serverid=%u servertype=%u)",wdServerID,wdServerType);

	using namespace Cmd::Super;

	unsigned char pBuffer[zSocket::MAX_DATASIZE];
	t_Startup_ServerEntry_NotifyMe *ptCmd = (t_Startup_ServerEntry_NotifyMe *)pBuffer;
	constructInPlace(ptCmd);
	ptCmd->size = 0;

	//check for notify other response
	for(Container::iterator it = ses.begin(); it != ses.end(); ++it)
	{
		if (it->second)
		{
			bzero(&ptCmd->entry[ptCmd->size], sizeof(ptCmd->entry[ptCmd->size]));
			ptCmd->entry[ptCmd->size].wdServerID = it->first.wdServerID;
			ptCmd->entry[ptCmd->size].wdServerType = it->first.wdServerType;
			strncpy(ptCmd->entry[ptCmd->size].pstrName, it->first.pstrName, MAX_NAMESIZE - 1);
			strncpy(ptCmd->entry[ptCmd->size].pstrIP, it->first.pstrIP, MAX_IP_LENGTH - 1);
			ptCmd->entry[ptCmd->size].wdPort = it->first.wdPort;
			strncpy(ptCmd->entry[ptCmd->size].pstrExtIP, it->first.pstrExtIP, MAX_IP_LENGTH - 1);
			ptCmd->entry[ptCmd->size].wdExtPort = it->first.wdExtPort;
			ptCmd->entry[ptCmd->size].state = it->first.state;

			ptCmd->size++;
		}
		else
			return false;
	}

	if (sendCmd(ptCmd, sizeof(t_Startup_ServerEntry_NotifyMe) + ptCmd->size * sizeof(ServerEntry)))
		hasNotifyMe = true;

	return hasNotifyMe;
}

/**
 * \brief 处理服务器的依赖关系，也就是启动顺序
 * \return 是否所有所依赖的服务器已经启动完成
 */
bool ServerTask::processSequence()
{
	if (hasprocessSequence) return true;
	
	Zebra::logger->trace("ServerTask::processSequence(wdServerType=%u)",wdServerType,wdServerType);
	using namespace Cmd::Super;

	ses.clear();
	
	std::vector<int> sequence = serverSequence[wdServerType];
	for(std::vector<int>::const_iterator it = sequence.begin(); it != sequence.end(); it++)
	{		
		std::vector<ServerTask *> sv;
		if (verifyTypeOK(*it, sv))
		{
			for(std::vector<ServerTask *>::const_iterator sv_it = sv.begin(); sv_it != sv.end(); sv_it++)
			{
				ServerEntry se;
				bzero(&se, sizeof(se));
				se.wdServerID = (*sv_it)->wdServerID;
				se.wdServerType = (*sv_it)->wdServerType;
				strncpy(se.pstrName, (*sv_it)->pstrName, MAX_NAMESIZE - 1);
				strncpy(se.pstrIP, (*sv_it)->pstrIP, MAX_IP_LENGTH - 1);
				se.wdPort = (*sv_it)->wdPort;
				strncpy(se.pstrExtIP, (*sv_it)->pstrExtIP, MAX_IP_LENGTH - 1);
				se.wdExtPort = (*sv_it)->wdExtPort;
				se.state = (*sv_it)->state;

				//将所依赖的服务器加入自己所依赖的列表中,并应答消息设成否
				ses.insert(Container::value_type(se, false));
			}
		}
		else
		{			
			return false;
		}
	}
	hasprocessSequence = true;
	return true;
}

/**
 * \brief 等待服务器启动完成
 *
 * 服务器接受到服务器管理器返回的自身信息，会进行自身的初始化，初始化完毕发送启动完成指令到服务器管理器<br>
 * 当服务器管理器接受到确认启动完成的指令就会把这个服务器加入到完成启动的队列中，这样服务器就完成了整个启动过程<br>
 * 实现了虚函数<code>zTCPTask::waitSync</code>
 *
 * \return 服务器启动是否通过，如果超时还要继续等待
 */
int ServerTask::waitSync()
{
	Zebra::logger->trace("ServerTask::waitSync");
	int retcode = mSocket.checkIOForRead();
	if (-1 == retcode)
	{
		Zebra::logger->error("%s", __PRETTY_FUNCTION__);
		return -1;
	}
	else if (retcode > 0)
	{
		//套接口准备好了接收数据，接收数据到缓冲，并尝试处理指令
		retcode = mSocket.recvToBuf_NoPoll();
		if (-1 == retcode)
		{
			Zebra::logger->error("%s", __PRETTY_FUNCTION__);
			return -1;
		}
	}

	unsigned char pstrCmd[zSocket::MAX_DATASIZE];
	int nCmdLen = mSocket.recvToCmd_NoPoll(pstrCmd, sizeof(pstrCmd));
	if (nCmdLen > 0)
	{
		using namespace Cmd::Super;

		t_Startup_OK *ptCmd = (t_Startup_OK *)pstrCmd;		
		if (CMD_STARTUP == ptCmd->cmd
				&& PARA_STARTUP_OK == ptCmd->para
				&& wdServerID == ptCmd->wdServerID)
		{
			Zebra::logger->debug("客户端连接同步验证成功(%u, %u)", ptCmd->wdServerID, wdServerID);
			return 1;
		}
		else
		{
			Zebra::logger->error("客户端连接同步验证失败(%u, %u)", ptCmd->wdServerID, wdServerID);
			return -1;
		}
	}
	
	bool blntime = checkSequenceTime();
	bool blnseq = processSequence();
	bool blnother = notifyOther();

	if (!blntime) 
	{
		Zebra::logger->debug("检查处理启动顺序checkSequenceTime()=false");
		blntime = true;
	}
	if (!blnseq)
	{
		Zebra::logger->debug("检查处理启动顺序processSequence()=false");
		blnseq = true;
	}
	if (!blnother)
	{
		Zebra::logger->debug("检查处理启动顺序notifyOther()=false");
		blnother = true;
	}
	//首先检查处理启动顺序
	if (blntime && blnseq	&& blnother)
		sequenceOK = true;	
	if (sequenceOK)
	{
		notifyMe();		
	}
	//等待超时
	return 0;
}

/**
 * \brief 确认一个服务器连接的状态是可以回收的
 *
 * 当一个连接状态是可以回收的状态，那么意味着这个连接的整个生命周期结束，可以从内存中安全的删除了：）<br>
 * 实现了虚函数<code>zTCPTask::recycleConn</code>
 *
 * \return 是否可以回收
 */
int ServerTask::recycleConn()
{
	return 1;
}

/**
 * \brief 添加到全局容器中
 *
 * 实现了虚函数<code>zTCPTask::addToContainer</code>
 *
 */
void ServerTask::addToContainer()
{
	Zebra::logger->trace("ServerTask::addToContainer");
	ServerManager::getInstance().addServer(this);
}

/**
 * \brief 从全局容器中删除
 *
 * 实现了虚函数<code>zTCPTask::removeToContainer</code>
 *
 */
void ServerTask::removeFromContainer()
{
	Zebra::logger->trace("ServerTask::removeFromContainer");
	//如果是网关服务器关闭，首先通知所有的登陆服务器网关关闭
	if (GATEWAYSERVER == wdServerType)
	{
		Cmd::FL::t_GYList_FL tCmd;

		tCmd.wdServerID = wdServerID;
		bzero(tCmd.pstrIP, sizeof(tCmd.pstrIP));
		tCmd.wdPort = 0;
		tCmd.wdNumOnline = 0;
		tCmd.state = state_maintain;
		tCmd.zoneGameVersion = 0;

		FLClientManager::getInstance().broadcast(&tCmd, sizeof(tCmd));
	}

	ServerManager::getInstance().removeServer(this);
}

/**
 * \brief 添加到唯一性验证容器中
 *
 * 实现了虚函数<code>zTCPTask::uniqueAdd</code>
 *
 */
bool ServerTask::uniqueAdd()
{
	Zebra::logger->trace("ServerTask::uniqueAdd");
	return ServerManager::getInstance().uniqueAdd(this);
}

/**
 * \brief 从唯一性验证容器中删除
 *
 * 实现了虚函数<code>zTCPTask::uniqueRemove</code>
 *
 */
bool ServerTask::uniqueRemove()
{
	Zebra::logger->trace("ServerTask::uniqueRemove");
	return ServerManager::getInstance().uniqueRemove(this);
}

/**
 * \brief 解析来自服务器管理器的关于启动的指令
 *
 * \param ptNullCmd 待处理的指令
 * \param nCmdLen 指令长度
 * \return 解析是否成功
 */
bool ServerTask::msgParse_Startup(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	Zebra::logger->trace("ServerTask::msgParse_Startup");
	using namespace Cmd::Super;

	switch(ptNullCmd->para)
	{
		case PARA_STARTUP_SERVERENTRY_NOTIFYOTHER://1,4
			{		
				// 响应依赖的服务器的NOTIFYOTHER回应消息		
				t_Startup_ServerEntry_NotifyOther *ptCmd = (t_Startup_ServerEntry_NotifyOther *)ptNullCmd;	
				// 			
				ServerManager::getInstance().responseOther(ptCmd->entry.wdServerID,ptCmd->srcID);

				return true;
			}
			break;
		case PARA_STARTUP_OK:
			{
				//t_Startup_OK *ptCmd= (t_Startup_OK *)ptNullCmd;//soke 注释掉
				Zebra::logger->debug("ServerTask::msgParse_Startup t_Startup_OK");
				return true;
			}
			break;
		case PARA_RESTART_SERVERENTRY_NOTIFYOTHER:
			{
				t_restart_ServerEntry_NotifyOther *notify = (t_restart_ServerEntry_NotifyOther*)ptNullCmd;
				ServerTask * pSrc = ServerManager::getInstance().getServer(notify->srcID);
				if(pSrc)
				{
					pSrc->notifyOther(notify->dstID);
					return true;
				}
			}
			break;
		default:
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

/**
 * \brief 解析来自计费服务器的指令
 *
 * \param ptNullCmd 待处理的指令
 * \param nCmdLen 指令长度
 * \return 处理是否成功
 */
bool ServerTask::msgParse_Bill(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Super;

	switch(ptNullCmd->para)
	{
		case PARA_BILL_IDINUSE:
			{
				t_idinuse_Bill *ptCmd = (t_idinuse_Bill *)ptNullCmd;
				Cmd::FL::t_idinuse_Session tCmd;

				tCmd.accid = ptCmd->accid;
				tCmd.loginTempID = ptCmd->loginTempID;
				tCmd.wdLoginID = ptCmd->wdLoginID;
				bcopy(ptCmd->name, tCmd.name, sizeof(tCmd.name));
				FLClientManager::getInstance().sendTo(tCmd.wdLoginID, &tCmd, sizeof(tCmd));

				return true;
			}
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

/**
 * \brief 解析来自网关服务器的指令
 *
 * \param ptNullCmd 待处理的指令
 * \param nCmdLen 指令长度
 * \return 处理是否成功
 */
bool ServerTask::msgParse_Gateway(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	Zebra::logger->trace("ServerTask::msgParse_Gateway(%u, %u)", ptNullCmd->cmd, ptNullCmd->para);
	using namespace Cmd::Super;

	switch(ptNullCmd->para)
	{
		case PARA_GATEWAY_GYLIST:
			{
				t_GYList_Gateway *ptCmd = (t_GYList_Gateway *)ptNullCmd;
				Cmd::FL::t_GYList_FL tCmd;

				OnlineNum = ptCmd->wdNumOnline;

				Zebra::logger->debug("GYList:serverid=%u,ip=%s,port=%u,onlines=%u, state=%u", ptCmd->wdServerID, ptCmd->pstrIP, ptCmd->wdPort, ptCmd->wdNumOnline, ptCmd->state);
				tCmd.wdServerID = ptCmd->wdServerID;
				bcopy(ptCmd->pstrIP, tCmd.pstrIP, sizeof(pstrIP));
				tCmd.wdPort = ptCmd->wdPort;
				tCmd.wdNumOnline = ptCmd->wdNumOnline;
				tCmd.state = ptCmd->state;
				tCmd.zoneGameVersion = ptCmd->zoneGameVersion;
				FLClientManager::getInstance().broadcast(&tCmd, sizeof(tCmd));

				return true;
			}
			break;
		case PARA_GATEWAY_NEWSESSION:
			{
				t_NewSession_Gateway *ptCmd = (t_NewSession_Gateway *)ptNullCmd;
				Cmd::FL::t_NewSession_Session tCmd;

				//Zebra::logger->debug("%s: %u, %u, %s, %u", __FUNCTION__, ptCmd->session.dwUserID, ptCmd->session.loginTempID, ptCmd->session.pstrIP, ptCmd->session.wdPort);
				tCmd.session = ptCmd->session;
				//bcopy(&ptCmd->session, &tCmd.session, sizeof(tCmd.session));
				FLClientManager::getInstance().sendTo(tCmd.session.wdLoginID, &tCmd, sizeof(tCmd));

				return true;
			}
			break;
		case PARA_CHARNAME_GATEWAY:
			{
				using namespace Cmd::RoleReg;
				t_Charname_Gateway *ptCmd = (t_Charname_Gateway *)ptNullCmd;
				t_Charname_Rolereg cmd;
				cmd.wdServerID = ptCmd->wdServerID;
				cmd.accid = ptCmd->accid;
				cmd.gameZone = SuperService::getInstance().getZoneID();
				bcopy(ptCmd->name, cmd.name, sizeof(cmd.name));
				cmd.state = ptCmd->state;
				Zebra::logger->debug("角色名称指令：%u, %s, %u", ptCmd->accid, ptCmd->name, ptCmd->state);
				if (!RoleregClientManager::getInstance().broadcastOne(&cmd, sizeof(cmd)))
				{
					if (ptCmd->state & ROLEREG_STATE_TEST)
					{
						Zebra::logger->error("请求失败，不允许创建角色：%u, %s", ptCmd->accid, ptCmd->name);
						t_Charname_Gateway tCmd;
						bcopy(ptCmd, &tCmd, sizeof(tCmd));
						tCmd.state |= ROLEREG_STATE_HAS;	//设置已经存在标志
						sendCmd(&tCmd, sizeof(tCmd));
					}
					if (ptCmd->state & (ROLEREG_STATE_WRITE | ROLEREG_STATE_CLEAN))
					{
						RoleregCache::getInstance().add(*ptCmd);
					}
				}

				return true;
			}
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

/**
 * \brief 解析GM工具的指令
 *
 * \param ptNullCmd 待处理的指令
 * \param nCmdLen 指令长度
 * \return 处理是否成功
 */
bool ServerTask::msgParse_GmTool(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::GmTool;

	switch(ptNullCmd->para)
	{
		case PARA_LOG_GMTOOL:
			{
				t_Log_GmTool * rev = (t_Log_GmTool *)ptNullCmd;
				rev->zone = SuperService::getInstance().getZoneID();
				InfoClientManager::getInstance().broadcastOne(rev, nCmdLen);
				return true;
			}
			break;
		case PARA_CHAT_GMTOOL:
			{
				t_Chat_GmTool * rev = (t_Chat_GmTool *)ptNullCmd;
				rev->zone = SuperService::getInstance().getZoneID();
				strncpy(rev->server, SuperService::getInstance().getZoneName().c_str(), MAX_NAMESIZE);

				InfoClientManager::getInstance().broadcastOne(rev, nCmdLen);
				/*
				if (ret)
					Zebra::logger->debug("[GM工具]转发聊天消息到InfoServer, %s 区(%u)", rev->userName, rev->zone.id);
				else
					Zebra::logger->debug("[GM工具]转发聊天消息到InfoServer失败, %s 区(%u)", rev->userName, rev->zone.id);
					*/
				return true;
			}
			break;
		case PARA_MSG_GMTOOL:
			{
				t_Msg_GmTool * rev = (t_Msg_GmTool *)ptNullCmd;
				rev->zone = SuperService::getInstance().getZoneID();
				//strncpy(rev->server, SuperService::getInstance().getZoneName().c_str(), MAX_NAMESIZE);
				InfoClientManager::getInstance().broadcastOne(rev, sizeof(t_Msg_GmTool));
				/*
				if (ret)
					Zebra::logger->debug("[GM工具]转发GM定单到InfoServer, %s 区(%u)", rev->userName, rev->zone.id);
				else
					Zebra::logger->debug("[GM工具]转发GM定单到InfoServer失败, %s 区(%u)", rev->userName, rev->zone.id);
					*/
				return true;
			}
			break;
		case PARA_NEW_MSG_GMTOOL:
			{
				t_NewMsg_GmTool * rev = (t_NewMsg_GmTool *)ptNullCmd;
				rev->zone = SuperService::getInstance().getZoneID();
				InfoClientManager::getInstance().broadcastOne(rev, sizeof(t_NewMsg_GmTool));
				/*
				if (ret)
					Zebra::logger->debug("[GM工具]转发GM定单到InfoServer, %s 区(%u)", rev->userName, rev->zone.id);
				else
					Zebra::logger->debug("[GM工具]转发GM定单到InfoServer失败, %s 区(%u)", rev->userName, rev->zone.id);
					*/
				return true;
			}
			break;
		case PARA_PUNISH_GMTOOL:
			{
				t_Punish_GmTool * rev = (t_Punish_GmTool *)ptNullCmd;
				rev->zone = SuperService::getInstance().getZoneID();
				strncpy(rev->server, SuperService::getInstance().getZoneName().c_str(), MAX_NAMESIZE);
				InfoClientManager::getInstance().broadcastOne(rev, sizeof(t_Punish_GmTool));
				/*
				if (ret)
					Zebra::logger->debug("[GM工具]转发GM处罚到InfoServer, %s 区(%u)", rev->userName, rev->zone.id);
				else
					Zebra::logger->debug("[GM工具]转发GM处罚到InfoServer失败, %s 区(%u)", rev->userName, rev->zone.id);
					*/
				return true;
			}
			break;
		default:
			break;
	}

	return false;
}

bool ServerTask::msgParse_CountryOnline(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd::Super;

	switch(ptNullCmd->para)
	{
		case PARA_COUNTRYONLINE:
			{
				t_CountryOnline * rev = (t_CountryOnline *)ptNullCmd;
				unsigned char pBuffer[zSocket::MAX_DATASIZE];
				Cmd::Info::t_Country_OnlineNum *cmd = (Cmd::Info::t_Country_OnlineNum *)pBuffer;
				constructInPlace(cmd);
				cmd->rTimestamp = rev->rTimestamp;
				cmd->GameZone = SuperService::getInstance().getZoneID();
				cmd->OnlineNum = rev->OnlineNum;

				for(DWORD i = 0; i < cmd->OnlineNum; i++)
				{
					cmd->CountryOnline[i].country = rev->CountryOnline[i].country;
					cmd->CountryOnline[i].num = rev->CountryOnline[i].num;
				}
				return InfoClientManager::getInstance().sendTo(rev->infoTempID, cmd, sizeof(Cmd::Info::t_Country_OnlineNum) + cmd->OnlineNum * sizeof(Cmd::Info::t_Country_OnlineNum::Online));
			}
			break;
	}

	return false;
}

/**
 * \brief 解析来自各个服务器连接的指令
 *
 * \param ptNullCmd 待处理的指令
 * \param nCmdLen 指令长度
 * \return 处理是否成功
 */
bool ServerTask::msgParse(const Cmd::t_NullCmd *ptNullCmd, const unsigned int nCmdLen)
{
	Zebra::logger->trace("ServerTask::msgParse(%u,%u)",ptNullCmd->cmd, ptNullCmd->para);
	switch(ptNullCmd->cmd)
	{
		case Cmd::Super::CMD_STARTUP:
			if (msgParse_Startup(ptNullCmd, nCmdLen))
			{
				return true;
			}
			break;
		case Cmd::Super::CMD_BILL:
			if (msgParse_Bill(ptNullCmd, nCmdLen))
			{
				return true;
			}
			break;
		case Cmd::Super::CMD_GATEWAY:
			if (msgParse_Gateway(ptNullCmd, nCmdLen))
			{
				return true;
			}
			break;
		case Cmd::GmTool::CMD_GMTOOL:
			if (msgParse_GmTool(ptNullCmd, nCmdLen))
			{
				return true;
			}
			break;
		case Cmd::Super::CMD_COUNTRYONLINE:
			if (msgParse_CountryOnline(ptNullCmd, nCmdLen))
			{
				return true;
			}
			break;
		case Cmd::Super::CMD_SESSION:
			{
				switch(ptNullCmd->para)
				{
					case Cmd::Super::PARA_SHUTDOWN:
						{
							SuperService::getInstance().Terminate();
							Zebra::logger->info("Session请求停机维护");
							return true;
						}
						break;
				}
			}
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->cmd, ptNullCmd->para, nCmdLen);
	return false;
}

