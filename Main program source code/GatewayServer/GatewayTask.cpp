/**
 * \file
 * \version  $Id: GatewayTask.cpp $
 * \author 
 * \date
 * \brief 定义登陆连接任务
 *
 */

#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <ext/hash_map>

#include "zTCPServer.h"
#include "zTCPTask.h"
#include "zService.h"
#include "zMisc.h"
#include "GatewayTask.h"
#include "GatewayServer.h"
#include "Command.h"
#include "GmToolCommand.h"
#include "SessionClient.h"
#include "LoginSessionManager.h"
#include "RecordClient.h"
#include "SceneCommand.h"
#include "MiniCommand.h"
#include "MiniUserCommand.h"
#include "SceneClient.h"
#include "MiniClient.h"
#include "GatewayTaskManager.h"
#include "BillCommand.h"
#include "BillClient.h"
#include "TimeTick.h"
#include "RoleregCommand.h"
#include "wordFilter.h"
#include "CountryInfo.h"

#include "EncDec/EncDec.h"
extern unsigned int merge_version;

#define checkTimeInterval 30000
/**
 * \brief 构造函数
 *
 * \param pool 所属连接池指针
 * \param sock TCP/IP套接口
 * \param addr 地址
 */
GatewayTask::GatewayTask(
		zTCPTaskPool *pool,
		const int sock,
		const struct sockaddr_in *addr) :
	zTCPTask(pool, sock, addr, true, false),

	_retset_gametime(3600), 
	initTime(),
	lastCheckTime(GatewayTimeTick::currentTime)
{
	
	Zebra::logger->trace("GatewayTask::GatewayTask");
	recycle_wait = 5000;

	v_lastSampleTime=0;
	lastCheckTime.addDelay(checkTimeInterval);
	haveCheckTime = true;
	v_samplePackets = 0;
	accid = 0;
	loginTempID = 0;
	versionVerified = false;
	dwTimestampServer=0;
	qwGameTime=0;
	accountVerified = ACCOUNTVERIFY_NONE;
	pUser=NULL;
	vip_user=true;
	nextChatTime.now();
	nextCountryChatTime.now();
	bzero(numPassword,sizeof(numPassword));
	numPwd = 0;

	mSocket.setEncMethod(CEncrypt::ENCDEC_RC5);
	//mSocket.enc.set_key_rc5((const unsigned char *)Zebra::global["rc5_key"].c_str(), 16, 12);
	unsigned char key[16] = {28, 196, 25, 36, 194, 125, 86, 196, 35, 92, 194, 41, 31, 241, 38, 224};
	mSocket.set_key_rc5((const unsigned char *)key, 16, 12);
}

/**
 * \brief 析构函数
 *
 */
GatewayTask::~GatewayTask()
{
	if(pUser)
	{
		SAFE_DELETE(pUser);
	}
}

/**
 * \brief 验证版本号
 * \param ptNullCmd 待验证的指令
 * \return 验证版本号码是否成功
 */
bool GatewayTask::verifyVersion(const Cmd::stNullUserCmd *ptNullCmd)
{
	Zebra::logger->trace("GatewayTask::verifyVersion");
	using namespace Cmd;
	if (LOGON_USERCMD == ptNullCmd->byCmd
			&& USER_VERIFY_VER_PARA == ptNullCmd->byParam)
	{
		stUserVerifyVerCmd *ptCmd = (stUserVerifyVerCmd *)ptNullCmd;
		//醉梦 客户端校验
		if (GatewayService::getInstance().verify_client_version == 0 || ptCmd->version >= GatewayService::getInstance().verify_client_version)
		{
			Zebra::logger->debug("客户端连接通过版本号验证[%u]",ptCmd->version);
			return true;
		}
		else
		{
			Zebra::logger->debug("客户端连接没有通过版本号验证[%u][%u]",ptCmd->version,GatewayService::getInstance().verify_client_version);
			return false;
		}
	}

	Zebra::logger->error("客户端连接没有通过版本号验证");
	return false;
}

/**
 * \brief 验证登陆网关指令
 * 验证登陆网关的指令
 * \param ptNullCmd 待验证的登陆指令
 * \return 验证登陆指令是否成功
 */
bool GatewayTask::verifyACCID(const Cmd::stNullUserCmd *ptNullCmd)
{
	Zebra::logger->trace("GatewayTask::verifyACCID");
	using namespace Cmd;
	if (LOGON_USERCMD == ptNullCmd->byCmd
			&& PASSWD_LOGON_USERCMD_PARA == ptNullCmd->byParam)
	{
		stPasswdLogonUserCmd *ptCmd = (stPasswdLogonUserCmd *)ptNullCmd;
		Zebra::logger->trace("用户登录：%u %u", ptCmd->loginTempID, ptCmd->dwUserID);
		DES_cblock des_key;
		if (LoginSessionManager::getInstance().verify(ptCmd->loginTempID, ptCmd->dwUserID,numPassword, &des_key))
		{
			accid = ptCmd->dwUserID;
			loginTempID = ptCmd->loginTempID;
			strncpy(account, ptCmd->pstrName, MAX_ACCNAMESIZE);
			if (numPassword[0])
			{
				numPwd = atoi(numPassword);
				Zebra::logger->debug("[财保]: %s 数字密码为: %u", account, numPwd);
			}

			//设置zSocket的加密密钥
			mSocket.setEncMethod(CEncrypt::ENCDEC_DES);
			mSocket.set_key_des(&des_key);

			Cmd::stMergeVersionCheckUserCmd send;
			send.dwMergeVersion = merge_version;
			sendCmd(&send, sizeof(send));

			return true;
		}
	}

	Zebra::logger->error("客户端连接同步验证失败");
	return false;
}

/**
 * \brief 连接确认
 *
 * \return 如果没有数据返回0,验证成功返回1,验证失败返回-1
 */
int GatewayTask::verifyConn()
{
	Zebra::logger->trace("GatewayTask::verifyConn");
	int retcode = mSocket.recvToBuf_NoPoll();
	if (retcode > 0)
	{
		while(true)
		{
			unsigned char pstrCmd[zSocket::MAX_DATASIZE];
			int nCmdLen = mSocket.recvToCmd_NoPoll(pstrCmd, sizeof(pstrCmd));
			if (nCmdLen <= 0)
			{
				//这里只是从缓冲取数据包，所以不会出错，没有数据直接返回
				return 0;
			}
			else
			{
				using namespace Cmd;
				//Zebra::logger->debug(__PRETTY_FUNCTION__);

				stNullUserCmd *ptNullCmd = (stNullUserCmd *)pstrCmd;
				if (versionVerified)
				{
					if (verifyACCID(ptNullCmd))
						return 1;
					else
						return -1;
				}
				else if (verifyVersion(ptNullCmd))
				{
					versionVerified = true;
				}
				else
				{
					Zebra::logger->error("客户端连接验证失败");
					return -1;
				}
			}
		}
	}
	else
		return retcode;
}

/**
 * \brief 连接同步等待
 *
 *
 * \return 错误返回-1,成功返回1,继续等待返回0
 */
int GatewayTask::waitSync()
{
	switch(accountVerified)
	{
		case ACCOUNTVERIFY_NONE:
			{
				using namespace Cmd::Bill;
				t_LoginVerify_Gateway tCmd;
				tCmd.accid = accid;
				tCmd.loginTempID = loginTempID;
				if (accountClient->sendCmd(&tCmd, sizeof(tCmd)))
					accountVerified = ACCOUNTVERIFY_WAITING;
				else
					return -1;
			}
			break;
		case ACCOUNTVERIFY_WAITING:
			{
				//无限期等待计费验证通过
			}
			break;
		case ACCOUNTVERIFY_SUCCESS:
			{
				return 1;
			}
			break;
		case ACCOUNTVERIFY_FAILURE:
			{
				return -1;
			}
			break;
	}

	return 0;
}

/**
 * \brief 用户下线
 *
 * \return 1:可以回收,2继续延时
 */
int GatewayTask::recycleConn()
{
	if((int)recycle_wait > 0)
	{
		recycle_wait -= 200;
		return 0;
	}
	else
	{
		//通知计费服务器用户下线
		using namespace Cmd::Bill;
		t_Logout_Gateway tCmd;

		tCmd.accid = accid;
		tCmd.loginTempID = loginTempID;
		accountClient->sendCmd(&tCmd, sizeof(tCmd));
		// */
		return 1;
	}
}

/**
 * \brief 需要主动断开客户端的连接
 *
 * \param method 连接断开方式
 */
void GatewayTask::Terminate(const TerminateMethod method)
{
	if(pUser)
	{
		Zebra::logger->trace("用户Terminate(%ld,%ld,%s)注销,ip=%s",pUser->accid,pUser->id,pUser->name,getIP());

		Cmd::Mini::t_UserLogout_Gateway s;
		s.userID = pUser->id;
		miniClient->sendCmd(&s, sizeof(s));

		//sky 假人开始
		if(!isTerminate())
		{
			pUser->unreg(true);
		}
        //sky 假人结束		
		pUser->final();
	}
	else
	{
		Zebra::logger->trace("注销但用户已经不存在");
	}
	// */
	zTCPTask::Terminate(method);
}
/**
 * \brief 新用户登陆
 *
 */
void GatewayTask::addToContainer()
{
	GateUser *pUser=new GateUser(this->accid,this);
	if(pUser)
	{
		//发送游戏时间，开始计时
		using namespace Cmd;
		stGameTimeTimerUserCmd cmd;
		cmd.qwGameTime = Zebra::qwGameTime;
		//soke 系统时间同步客户端
		cmd.qwServerTime = GatewayTimeTick::currentTime.sec();
		if(!qwGameTime)
		{
			qwGameTime = Zebra::qwGameTime; 
			GameTimeSyn.now();
		}
		sendCmd(&cmd, sizeof(cmd));
		initTime = GatewayTimeTick::currentTime;

#define country_info GatewayService::getInstance().country_info
		char Buf[country_info.getCountrySize()*sizeof(Cmd::Country_Info) + sizeof(Cmd::stCountryInfoUserCmd)];
		bzero(Buf ,sizeof(Buf));
		Cmd::stCountryInfoUserCmd *ciu = (Cmd::stCountryInfoUserCmd*)Buf;
		constructInPlace(ciu);
		ciu->size = country_info.getAll((char *)ciu->countryinfo);
		pUser->sendCmd(ciu , sizeof(Cmd::stCountryInfoUserCmd) + ciu->size *sizeof(Cmd::Country_Info));
		if(pUser->beginSelect())
		{
			//通知登陆服务器，网关连接数
			GatewayService::getInstance().notifyLoginServer();
		}
		else 
		{
			/*
			   SAFE_DELETE(pUser);
			   pUser = NULL;
			// */
		}
	}
}

/**
 * \brief 从容器中删除
 *
 */
void GatewayTask::removeFromContainer()
{
	//通知登陆服务器，网关连接数
	GatewayService::getInstance().notifyLoginServer();
}

/**
 * \brief 添加一个连接线程
 *
 *
 * \return 成功返回ture,否则返回false
 */
bool GatewayTask::uniqueAdd()
{
	return GatewayTaskManager::getInstance().uniqueAdd(this);
}

/**
 * \brief 删除一个连接线程
 *
 *
 * \return 成功返回ture,否则返回false
 */
bool GatewayTask::uniqueRemove()
{
	return GatewayTaskManager::getInstance().uniqueRemove(this);
}

/**
 * \brief 场景用户将消息转发到Bill
 *
 *
 * \param ptNullCmd: 需要转发的指令
 * \param nCmdLen: 指令长度
 * \return 转发是否成功
 */
bool GatewayTask::forwardBillScene(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen)
{
	if(pUser)
	{
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::Bill::t_Scene_ForwardBill *sendCmd=(Cmd::Bill::t_Scene_ForwardBill *)buf;
		constructInPlace(sendCmd);
		sendCmd->dwAccid=pUser->accid;
		sendCmd->size=nCmdLen;
		bcopy(ptNullCmd,sendCmd->data,nCmdLen);
		accountClient->sendCmd(buf,sizeof(Cmd::Bill::t_Scene_ForwardBill)+nCmdLen);
		//Zebra::logger->debug("转发%ld的消息到%ld场景",pUser->id,pUser->sceneTempID);
		return true;
	}
	return false;
}
/**
 * \brief 将消息转发到Bill
 *
 *
 * \param ptNullCmd: 需要转发的指令
 * \param nCmdLen: 指令长度
 * \return 转发是否成功
 */
bool GatewayTask::forwardBill(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen)
{
	if(pUser)
	{
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::Bill::t_Bill_ForwardBill *sendCmd=(Cmd::Bill::t_Bill_ForwardBill *)buf;
		constructInPlace(sendCmd);
		sendCmd->dwAccid=pUser->accid;
		sendCmd->size=nCmdLen;
		bcopy(ptNullCmd,sendCmd->data,nCmdLen);
		accountClient->sendCmd(buf,sizeof(Cmd::Bill::t_Bill_ForwardBill)+nCmdLen);
		//Zebra::logger->debug("转发%ld的消息到%ld场景",pUser->id,pUser->sceneTempID);
		return true;
	}
	return false;
}
/**
 * \brief 将Bill的消息转发到场景
 *
 *
 * \param ptNullCmd: 需要转发的指令
 * \param nCmdLen: 指令长度
 * \return 转发是否成功
 */
bool GatewayTask::forwardSceneBill(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen)
{
	if(pUser && pUser->scene)
	{
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::Scene::t_Bill_ForwardScene *sendCmd=(Cmd::Scene::t_Bill_ForwardScene *)buf;
		constructInPlace(sendCmd);
		sendCmd->dwUserID=pUser->id;
		sendCmd->dwSceneTempID=pUser->sceneTempID;
		sendCmd->size=nCmdLen;
		bcopy(ptNullCmd,sendCmd->data,nCmdLen);
		pUser->scene->sendCmd(buf,sizeof(Cmd::Scene::t_Bill_ForwardScene)+nCmdLen);
		//Zebra::logger->debug("转发%ld的消息到%ld场景",pUser->id,pUser->sceneTempID);
		return true;
	}
	return false;
}

/**
 * \brief 将消息转发到小游戏服务器
 *
 *
 * \param ptNullCmd: 需要转发的指令
 * \param nCmdLen: 指令长度
 * \return 转发是否成功
 */
bool GatewayTask::forwardMini(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen)
{
	if(pUser)
	{
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::Mini::t_Mini_UserForwardMini *sendCmd=(Cmd::Mini::t_Mini_UserForwardMini *)buf;
		constructInPlace(sendCmd);
		sendCmd->id=pUser->id;
		sendCmd->size=nCmdLen;
		bcopy(ptNullCmd, sendCmd->data, nCmdLen);
		miniClient->sendCmd(buf, sizeof(Cmd::Mini::t_Mini_UserForwardMini)+nCmdLen);
		Zebra::logger->debug("转发%ld的消息到Mini服务器", pUser->id);
		return true;
	}   
	return false;
}

/**
 * \brief 将消息转发到场景
 *
 *
 * \param ptNullCmd: 需要转发的指令
 * \param nCmdLen: 指令长度
 * \return 转发是否成功
 */
bool GatewayTask::forwardScene(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen)
{
	if(pUser && pUser->scene)
	{
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::Scene::t_Scene_ForwardScene *sendCmd=(Cmd::Scene::t_Scene_ForwardScene *)buf;
		constructInPlace(sendCmd);
		sendCmd->dwUserID=pUser->id;
		sendCmd->dwSceneTempID=pUser->sceneTempID;
		sendCmd->size=nCmdLen;
		bcopy(ptNullCmd,sendCmd->data,nCmdLen);
		pUser->scene->sendCmd(buf,sizeof(Cmd::Scene::t_Scene_ForwardScene)+nCmdLen);
		//Zebra::logger->debug("转发%ld的消息到%ld场景",pUser->id,pUser->sceneTempID);
		return true;
	}
	return false;
}

/**
 * \brief 将消息转发到Session
 *
 *
 * \param ptNullCmd: 需要转发的指令
 * \param nCmdLen: 指令长度
 * \return 转发是否成功
 */
bool GatewayTask::forwardSession(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen)
{
	if(pUser)
	{
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::Session::t_Session_ForwardUser *sendCmd=(Cmd::Session::t_Session_ForwardUser *)buf;
		constructInPlace(sendCmd);
		sendCmd->dwID=pUser->id;
		sendCmd->size=nCmdLen;
		bcopy(ptNullCmd,sendCmd->data,nCmdLen);
		sessionClient->sendCmd(buf,sizeof(Cmd::Session::t_Session_ForwardUser)+nCmdLen);
		//Zebra::logger->debug("转发%ld的消息到Session服务器",pUser->id,pUser->sceneTempID);
		return true;
	}
	return false;
}

/**
 * \brief 解析选择消息
 *
 *
 * \param ptNullCmd: 需要转发的指令
 * \param nCmdLen: 指令长度
 * \return 解析是否成功
 */
bool GatewayTask::msgParse_Select(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd;

	switch(ptNullCmd->byParam)
	{
		case CHECKNAME_SELECT_USERCMD_PARA:
			{
				stCheckNameSelectUserCmd *rev=(stCheckNameSelectUserCmd *)ptNullCmd;

				//词汇过滤
				if (! (wordFilter::getInstance().doFilter(rev->name, 16) && checkNewName(rev->name)))
				{
					Cmd::stCheckNameSelectUserCmd ret;
					ret.err_code = 2;//表示名字中包含禁止的词汇
					strncpy(ret.name, rev->name, MAX_NAMESIZE);
					sendCmd(&ret, sizeof(ret));
					return true;
				}

				Cmd::Record::t_CheckName_GateRecord send;
				strncpy(send.name, rev->name, MAX_NAMESIZE-1);
				send.accid = pUser->accid;
				//发送到RecordServer验证
				return recordClient->sendCmd(&send, sizeof(send));
			}
			break;
		case CREATE_SELECT_USERCMD_PARA:
			{
				stCreateSelectUserCmd *rev=(stCreateSelectUserCmd *)ptNullCmd;
				
					Zebra::logger->trace("创建角色：%u, %s, %u", pUser->accid, rev->strUserName, rev->charType);
					if (!pUser->charInfoFull())
					{
						if(strlen(rev->strUserName) > 16)
						{
							Zebra::logger->iffy("名字长度太长(%d)" , strlen(rev->strUserName));
							return true;
						}
						//角色数量没有满，还可以再建立角色
						pUser->createCharCmd.accid = pUser->accid;
						bzero(pUser->createCharCmd.name, sizeof(pUser->createCharCmd.name));
						strncpy(pUser->createCharCmd.name, rev->strUserName, MAX_NAMESIZE - 1);
						pUser->createCharCmd.type = rev->charType;
						//头发发型以及颜色
						pUser->createCharCmd.hair = rev->byHairType;
						pUser->createCharCmd.hair = pUser->createCharCmd.hair << 24;
						pUser->createCharCmd.hair = (pUser->createCharCmd.hair & HAIRTYPE_MASK) | (rev->byRGB & HAIRRGB_MASK);
                        #define country_info GatewayService::getInstance().country_info
						//Zebra::logger->debug("国家信息数量%u" , country_info.getCountrySize());
						if(rev->country && rev->country !=country_info.getCountryID(rev->country))
						{
							Zebra::logger->iffy("国家数据不合法");
							return true;
						}
						if (!country_info.isEnableRegister(rev->country))
						{
							if (pUser)
							{
								Cmd::stDisableLoginCountryCmd send;
								send.status = Cmd::FALSE_REGISTER_COUNTRY;
								pUser->sendCmd(&send, sizeof(send));
							}
							return true;
						}
						strncpy(pUser->createCharCmd.mapName,country_info.getMapName(rev->country).c_str(),MAX_NAMESIZE);
						pUser->createCharCmd.country =rev->country;
						pUser->createCharCmd.createip = this->getAddr();
						if(rev->five >=FIVE_NONE)
						{
							Zebra::logger->iffy("五行主属性不合法");
							return true;
						}
						pUser->createCharCmd.five = rev->five;
						if (GatewayService::getInstance().rolereg_verify)
						{
							using namespace Cmd::RoleReg;
							using namespace Cmd::Super;
							t_Charname_Gateway cmd;
							cmd.wdServerID = GatewayService::getInstance().getServerID();
							cmd.accid = pUser->accid;
							strncpy(cmd.name, pUser->createCharCmd.name, MAX_NAMESIZE);
							cmd.state = ROLEREG_STATE_TEST;
							GatewayService::getInstance().sendCmdToSuperServer(&cmd, sizeof(cmd));
						}
						else
						{
							if (!recordClient->sendCmd(&pUser->createCharCmd,sizeof(pUser->createCharCmd)))
								return true;
						}
						pUser->createState();
					}
					else
						Zebra::logger->iffy("角色信息满，不能再创建角色了");

				return true;
			}
			break;
		case LOGIN_SELECT_USERCMD_PARA: 
			{
				if(!pUser->isWaitUnregState())
				{
					stLoginSelectUserCmd *rev=(stLoginSelectUserCmd *)ptNullCmd;
					SelectUserInfo *userinfo=pUser->getSelectUserInfo(rev->charNo);
					Zebra::logger->trace("收到%ld的第%d个登录用户指令",pUser->accid,rev->charNo);
					if(!userinfo)
					{
						Zebra::logger->alarm("收到不存在的角色,可能是恶意攻击!");
						return true;
					}
					if(userinfo->bitmask & CHARBASE_FORBID)
					{
						Zebra::logger->alarm("收到已经被封号的角色,可能是恶意攻击!");
						return true;
					}

					if (!GatewayService::getInstance().country_info.isEnableLogin(userinfo->country))
					{
						Zebra::logger->alarm("国家不允许登陆 %u, %u", pUser->accid, userinfo->country);
						Cmd::stDisableLoginCountryCmd send;
						send.status = Cmd::FALSE_LOGIN_COUNTRY;
						pUser->sendCmd(&send, sizeof(send));
						return true;
					}
					if (pUser->checkPassport(rev->jpegPassport))
					{
						if(pUser->tempid == 0 && userinfo && userinfo->id!=0)
						{
							pUser->id=userinfo->id;
							pUser->face=userinfo->face;
							strncpy(pUser->name,userinfo->name,MAX_NAMESIZE);
							//加入角色，分配临时ID
							if(GateUserManager::getInstance()->addUser(pUser) && GateUserManager::getInstance()->addCountryUser(pUser))
							{
								Zebra::logger->trace("注册 %u, %s", pUser->accid,userinfo->name);
								//注册角色
								pUser->reg(rev->charNo);
								return true;
							}
							else
							{
								Zebra::logger->alarm("选择%s时添加用户失败",userinfo->name);
							}
						}
						else
							Zebra::logger->alarm("用户%s(%ld)重复选择角色",userinfo->name,userinfo->id);
					}
					else
						Zebra::logger->alarm("验证图形验证码失败 %u",pUser->accid);
				}
				else
				{
					Zebra::logger->alarm("用户%s(%ld)正在退出状态",pUser->name,pUser->id);
				}
				return true;
			}
			break;
		case DELETE_SELECT_USERCMD_PARA:
			{
				stDeleteSelectUserCmd *rev=(stDeleteSelectUserCmd *)ptNullCmd;
				SelectUserInfo *userinfo=pUser->getSelectUserInfo(rev->charNo);
				if(!userinfo)
				{
					Zebra::logger->alarm("收到不存在的角色,可能是恶意攻击!");
					return true;
				}
				//debug版本去掉数字密码功能
#ifndef _ALL_SUPER_GM 
				//如果启用数字密码
				if(numPassword[0] && strncmp(numPassword,rev->numPassword,sizeof(numPassword)) != 0)
				{
					stDeleteErrorSelectUserCmd ret;
					sendCmd(&ret, sizeof(ret));
					return true;
				}
#endif
				if(pUser->tempid == 0 && userinfo && userinfo->id!=0)
				{
					Zebra::logger->info("用户(%s,%d)删除角色",userinfo->name , userinfo->id);
					//Cmd::Record::t_DelChar_GateRecord send;
					Cmd::Session::t_DelChar_GateSession send;
					send.accid=pUser->accid;
					send.id=userinfo->id;
					strncpy(send.name, userinfo->name, MAX_NAMESIZE);
					sessionClient->sendCmd(&send, sizeof(send));

					Cmd::Mini::t_UserDelete_Gateway d;
					d.userID = userinfo->id;
					miniClient->sendCmd(&d, sizeof(d));

					pUser->delSelectUserInfo(rev->charNo);
					//recordClient->sendCmd(&send,sizeof(send));
				}
				else
					Zebra::logger->alarm("用户%s(%ld)删除角色失败",userinfo->name , userinfo->id);
				return true;
			}
			break;
		case REQUEST_COUNTRY_SELECT_USERCMD_PARA:
			{
				if(Zebra::global["countryorder"] == "2")
				{
					Cmd::Record::t_request_Country_GateRecord reqRecord; 
					recordClient->sendCmd(&reqRecord, sizeof(reqRecord));
				}
				else  if(Zebra::global["countryorder"] == "1")
				{
					Cmd::Session::t_request_Country_GateSession reqSession;
					sessionClient->sendCmd(&reqSession,sizeof(reqSession));
				}
				return true;
			}
			break;
		default:
			break;
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->byCmd, ptNullCmd->byParam, nCmdLen);
	return false;
}

/**
 * \brief 解析时间消息
 *
 *
 * \param ptNullCmd: 需要转发的指令
 * \param nCmdLen: 指令长度
 * \return 解析是否成功
 */
bool GatewayTask::msgParse_Time(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen)
{
	using namespace Cmd;

	switch(ptNullCmd->byParam)
	{
		case USERGAMETIME_TIMER_USERCMD_PARA:
			{
				if(!dwTimestampServer)
				{
					dwTimestampServer=GatewayTimeTick::currentTime.msecs() - ptNullCmd->dwTimestamp;
				}
				stUserGameTimeTimerUserCmd *rev=(stUserGameTimeTimerUserCmd *)ptNullCmd;
				
				if(qwGameTime && (SQWORD)(rev->qwGameTime - (qwGameTime + (GatewayTimeTick::currentTime.sec() - GameTimeSyn.sec()))) >= sampleInterval_error_sec)
				{   
					if(this->pUser)
					{
						//soke 解决掉线问题
						Zebra::logger->trace("客户端游戏时间太快，使用了加速器，需要断开连接（accid = %u, %u, %llu）", this->pUser->accid, ptNullCmd->dwTimestamp, rev->qwGameTime - (qwGameTime + (GatewayTimeTick::currentTime.sec() - GameTimeSyn.sec())));
						//Zebra::logger->trace("客户端游戏时间太快，使用了加速器，需要断开连接（accid = %u, %u, %llu）", this->pUser->accid, ptNullCmd->dwTimestamp, rev->qwGameTime - (qwGameTime + (GatewayTimeTick::currentTime.sec() - GameTimeSyn.sec())));
					}
					Zebra::logger->debug("游戏时间:(accid=%u,%llu,%llu)", accid,rev->qwGameTime, qwGameTime + (GatewayTimeTick::currentTime.sec() - GameTimeSyn.sec()));
					Terminate();
				}
				if(!haveCheckTime)
					haveCheckTime = true;
				else
					Zebra::logger->warn("校对时间标记错误(accid=%u)", accid);
				return true;
			}
			break;
		case PING_TIMER_USERCMD_PARA:
			{       
				stPingTimeTimerUserCmd cmd;
				sendCmd(&cmd, sizeof(cmd));
				return true;
			}       
			break;  
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->byCmd, ptNullCmd->byParam, nCmdLen);
	return false;
}

/**
 * \brief 检测时间
 *
 * \param ct: 当前时间
 * \return 是否通过检测
 */
bool GatewayTask::checkTime(const zRTime &ct)
{
	if (accountVerified == ACCOUNTVERIFY_SUCCESS)
	{
		if (ct >= lastCheckTime)
		{
			if (haveCheckTime)
			{
				//校对时间
				if(_retset_gametime(ct))
				{
					Cmd::stGameTimeTimerUserCmd cmd;
					//soke 系统时间同步客户端
					cmd.qwServerTime = GatewayTimeTick::currentTime.sec();
					if(qwGameTime)
					{
						cmd.qwGameTime = qwGameTime + (GatewayTimeTick::currentTime.sec() - GameTimeSyn.sec());
						sendCmd(&cmd, sizeof(cmd));
					}
					//准备重新设置同步时间
					dwTimestampServer=0;
				}
				haveCheckTime = false;
				Cmd::stRequestUserGameTimeTimerUserCmd cmd;
				sendCmd(&cmd, sizeof(cmd));
			}
			else
			{
				Zebra::logger->debug("客户端指定时间内没有返回校对时间指令（accid = %u, snd_queue_size = %u）,注销", accid, mSocket.snd_queue_size());
				TerminateWait();
			}

			lastCheckTime = ct;
			lastCheckTime.addDelay(checkTimeInterval);
		}
	}

	return true;
}

/**
 * \brief 对客户端发送过来的指令进行检测
 * 主要检测时间戳等，对加速器等进行防止
 * \param pCmd 待检测的指令
 * \param ct 当前时间
 * \return 检测是否成功
 */
bool GatewayTask::checkUserCmd(const Cmd::stNullUserCmd *pCmd, const zRTime &ct)
{
	return true;

}

/**
 * \brief 解析消息
 *
 * \param ptNull: 需要转发的指令
 * \param nCmdLen: 指令长度
 * \return 解析是否成功
 */
bool GatewayTask::msgParse(const Cmd::t_NullCmd *ptNull, const unsigned int nCmdLen)
{
	using namespace Cmd;
	const stNullUserCmd *ptNullCmd = (const stNullUserCmd *)ptNull;

	if(isTerminate())
	{
		return false;
	}
	if (!checkUserCmd(ptNullCmd, GatewayTimeTick::currentTime))
	{
		Terminate();
		return false;
	}
	// */

	if (pUser)
	{
		switch(ptNullCmd->byCmd)
		{
			case LOGON_USERCMD:
				{
					if(ptNullCmd->byParam==BACKSELECT_USERCMD_PARA)
					{
						if(pUser && pUser->isPlayState())
						{
							Zebra::logger->trace("%s(%ld)小退",pUser->name,pUser->id);
							pUser->backSelect=true;
							pUser->unreg();
						}
						return true;
					}
					return true;
				}
				break;
			case SELECT_USERCMD:
				{
					if (pUser->isSelectState())
					{
						if (msgParse_Select(ptNullCmd,nCmdLen))
						{
							return true;
						}
					}
					else
						Zebra::logger->iffy("%s(%ld)不再选择用户状态,发送选择用户系列指令",pUser->name,pUser->id);
					return true;
				}
				break;
			case TIME_USERCMD:
				{
					if (msgParse_Time(ptNullCmd, nCmdLen))
					{
						return true;
					}
					return false;
				}
				break;
			case CHAT_USERCMD:
				{
					if(pUser && pUser->isPlayState())
					{
						//如果是普通聊天消息则进行词汇过滤
						if (ALL_CHAT_USERCMD_PARAMETER == ptNullCmd->byParam)
						{
							Cmd::stChannelChatUserCmd *rev = (Cmd::stChannelChatUserCmd *)ptNullCmd;
							if (rev->dwType==CHAT_TYPE_COUNTRY||rev->dwType==CHAT_TYPE_WORLD)
							{
#ifndef _ALL_SUPER_GM
								if (GatewayTimeTick::currentTime<nextCountryChatTime)
								{
									zRTime ctv;
									Cmd::stChannelChatUserCmd send;
									send.dwType=Cmd::CHAT_TYPE_SYSTEM;
									send.dwSysInfoType = Cmd::INFO_TYPE_FAIL;
									send.dwChatTime = ctv.sec();
									bzero(send.pstrName, sizeof(send.pstrName));
									bzero(send.pstrChat, sizeof(send.pstrChat));
									strncpy((char *)send.pstrChat, "对不起，该频道3秒钟可以发言一次",MAX_CHATINFO-1);

									pUser->sendCmd(&send, sizeof(send));
									return true;
								}
#endif								
							}
							else
							{
#ifndef _ALL_SUPER_GM
								if (GatewayTimeTick::currentTime<nextChatTime)
								{
									zRTime ctv;
									Cmd::stChannelChatUserCmd send;
									send.dwType=Cmd::CHAT_TYPE_SYSTEM;
									send.dwSysInfoType = Cmd::INFO_TYPE_FAIL;
									send.dwChatTime = ctv.sec();
									bzero(send.pstrName, sizeof(send.pstrName));
									bzero(send.pstrChat, sizeof(send.pstrChat));
									strncpy((char *)send.pstrChat, "对不起，您说话太快了，请不要刷屏",MAX_CHATINFO-1);

									pUser->sendCmd(&send, sizeof(send));
									return true;
								}
#endif								
								if (rev->dwType == CHAT_TYPE_BLESS_MSG)
								{
									forwardSession(ptNullCmd , nCmdLen);
									return true;
								}
							}

							nextChatTime.now();
							nextChatTime.addDelay(chatInterval);
							nextCountryChatTime.now();
							nextCountryChatTime.addDelay(3000);

						}

						switch (ptNullCmd->byParam)
						{
							case CREATE_CHANNEL_USERCMD_PARAMETER:
							case INVITE_CHANNEL_USERCMD_PARAMETER:
							case JOIN_CHANNEL_USERCMD_PARAMETER:
							case LEAVE_CHANNEL_USERCMD_PARAMETER:
								if(forwardSession(ptNullCmd , nCmdLen))
									return true;
								break;
							case QUESTION_OBJECT_USERCMD_PARA:
								if(forwardSession(ptNullCmd , nCmdLen))
								{
#ifdef _ZJW_DEBUG
									Zebra::logger->debug("收到交易物品查询命令,转发到会话");
#endif							
									return true;
								}
								break;
							default:
								if(forwardScene(ptNullCmd , nCmdLen))
								{
									return true;
								}
						}
					}
					else
					{
						Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
					}
					return true;
				}
				break;
			case UNION_USERCMD: // 将消息转发到会话服务器在那里处理帮会事务。
				{
					if(pUser && pUser->isPlayState())
					{
						switch(ptNullCmd->byParam)
						{
							case CREATE_UNION_PARA:			// 建立帮会
								{
									Cmd::stCreateUnionCmd * cmd = (stCreateUnionCmd *)ptNullCmd;
									if (!checkNewName(cmd->UnionName))
									{
										zRTime ctv;
										Cmd::stChannelChatUserCmd send;
										send.dwType=Cmd::CHAT_TYPE_SYSTEM;
										send.dwSysInfoType = Cmd::INFO_TYPE_FAIL;
										send.dwChatTime = ctv.sec();
										bzero(send.pstrName, sizeof(send.pstrName));
										bzero(send.pstrChat, sizeof(send.pstrChat));
										strncpy((char *)send.pstrChat, "对不起，帮会名字不合法，请使用字母、数字和汉字",MAX_CHATINFO-1);

										sendCmd(&send, sizeof(send));
										return true;
									}
								}
							case UNION_STATUS_CHECK_PARA:	// 帮会状态检查判断此人是否可以建立新帮会
							case ADD_MEMBER_TO_UNION_PARA:		// 邀请玩家加入帮会
							case ENTER_UNION_CITY_AREA_PARA: // 进入帮会所得城市帮属场景
							case QUESTION_UNION_CITY_INFO_PARA:
							case CONTRIBUTE_UNION_PARA: // 帮会捐献
								{// 以上消息转发到场景服务器处理
									if(forwardScene(ptNullCmd , nCmdLen))
									{
										return true;
									}
								}
								break;
							default:
								{
									if(forwardSession(ptNullCmd , nCmdLen))
									{
										return true;
									}
								}
								break;

						}
					}
					else
					{
						Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
					}
					return true;
				}
				break;
			case SAFETY_USERCMD:
				{
					if (pUser==NULL || !pUser->isPlayState())
					{
						Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",
								ptNullCmd->byCmd,ptNullCmd->byParam);
						return true;
					}

					switch (ptNullCmd->byParam)
					{
						case SET_SAFETY_PARA:
							{
								Cmd::stSetSafetyUserCmd* cmd = (Cmd::stSetSafetyUserCmd*)ptNullCmd;
								if (cmd->pwd == this->numPwd)
								{
									if (forwardScene(ptNullCmd , nCmdLen))
									{
										return true;
									}
								}
								else
								{
									zRTime ctv;
									Cmd::stChannelChatUserCmd send;
									send.dwType=Cmd::CHAT_TYPE_SYSTEM;
									send.dwSysInfoType = Cmd::INFO_TYPE_FAIL;
									send.dwChatTime = ctv.sec();
									bzero(send.pstrName, sizeof(send.pstrName));
									bzero(send.pstrChat, sizeof(send.pstrChat));
									strncpy((char *)send.pstrChat, "对不起，数字密码不正确",MAX_CHATINFO-1);

									pUser->sendCmd(&send, sizeof(send));
								}
							}
							break;
						case SET_TEMP_UNSAFETY_PARA:
							{
								Cmd::stSetTempUnSafetyUserCmd* cmd = (Cmd::stSetTempUnSafetyUserCmd*)ptNullCmd;
								if (cmd->pwd == this->numPwd)
								{
									if (forwardScene(ptNullCmd , nCmdLen))
									{
										return true;
									}
								}
								else
								{
									zRTime ctv;
									Cmd::stChannelChatUserCmd send;
									send.dwType=Cmd::CHAT_TYPE_SYSTEM;
									send.dwSysInfoType = Cmd::INFO_TYPE_FAIL;
									send.dwChatTime = ctv.sec();
									bzero(send.pstrName, sizeof(send.pstrName));
									bzero(send.pstrChat, sizeof(send.pstrChat));
									strncpy((char *)send.pstrChat, "对不起，数字密码不正确",MAX_CHATINFO-1);

									pUser->sendCmd(&send, sizeof(send));
								}
							}
							break;
						case SET_SAFETY_DETAIL_PARA:
							{
								Cmd::stSetSafetyDetailUserCmd* cmd = (Cmd::stSetSafetyDetailUserCmd*)ptNullCmd;
								if (cmd->pwd == this->numPwd)
								{
									if (forwardScene(ptNullCmd , nCmdLen))
									{
										return true;
									}
								}
								else
								{
									zRTime ctv;
									Cmd::stChannelChatUserCmd send;
									send.dwType=Cmd::CHAT_TYPE_SYSTEM;
									send.dwSysInfoType = Cmd::INFO_TYPE_FAIL;
									send.dwChatTime = ctv.sec();
									bzero(send.pstrName, sizeof(send.pstrName));
									bzero(send.pstrChat, sizeof(send.pstrChat));
									strncpy((char *)send.pstrChat, "对不起，数字密码不正确",MAX_CHATINFO-1);

									pUser->sendCmd(&send, sizeof(send));
								}
							}
							break;
						default:
							{
								// 以上消息转发到场景服务器处理
								if(forwardScene(ptNullCmd , nCmdLen))
								{
									return true;
								}
							}
							break;
					}

					return true;
				}
				break;
		case TURN_USERCMD:          //soke 转生指令
		case HORSETRAINING_USERCMD: //驯马
		case FUJIAN_USERCMD:
		case JINGCAI_USERCMD:
		case XINFUBEN_USERCMD:
		case ERNV_USERCMD://儿女
		case TIANXIA_USERCMD://天下第一指令
		case BIKE_USERCMD://自行车指令
		case FENGYIN_USERCMD://等级封印指令
		case HUISHOU_USERCMD://道具回收指令
		case BIESHU_USERCMD://云天别墅指令
		case ZHUANHUAN_USERCMD://装备转换指令
		case XIULIAN_USERCMD://能力修炼指令
		case ZUIMENG_USERCMD://后门指令
		case ZHANCHE_USERCMD://战车指令
		case JIAZUBOSS_USERCMD://家族BOSS指令
		case ZUOQI_USERCMD://坐骑图鉴指令
		case MIGONG_USERCMD://迷宫指令
			{
				if (forwardScene(ptNullCmd , nCmdLen))
				{
					return true;
				}
			}
			break;
			case COUNTRY_USERCMD:
				{
					if (pUser && pUser->isPlayState())
					{
						switch(ptNullCmd->byParam)
						{
							case ANSWER_COUNTRY_DARE_PARA:
							case CONTRIBUTE_COUNTRY_MATERIAL:
							case CANCEL_COUNTRY_PARA:
							case APPLY_COUNTRY_PARA:
							case CHANGE_COUNTRY_PARA:       // 变更国籍(金子)
							case CHANGE_COUNTRY_SALARY_PARA:// 变更国籍(工资)
							case ADD_COUNTRY_SALARY_PARA:   // 领取工资
							case ADD_COUNTRY_LOGIN_PARA:    // 领取登陆奖励
							case ADD_COUNTRY_VIP_PARA:      // 领取VIP奖励
							case ADD_COUNTRY_CHOU_PARA:      //
							case REQUEST_COUNTRY_POWER_PARA:
								{
									// 以上消息转发到场景服务器处理
									if(forwardScene(ptNullCmd , nCmdLen))
									{
										return true;
									}
								}
								break;
							case CONFIRM_SEARCHER_PARA:
							case UP_TECH_DEGREE_PARA:
							case SET_TECH_SEARCH_PARA:
							case REQ_WAIT_OFFICIAL_PARA:
							case SELECT_TRANS_LEVEL:
							case REQ_TECH_PARA:	
							case DARE_COUNTRY_FORMAL_PARA:
							case REQUEST_DARE_COUNTRY_PARA:
							case REQUEST_DARE_RECORD_PARA:
							case TRANS_DARE_COUNTRY_PARA:
							case FORBID_TALK_COUNTRY_PARA:
							case CANCEL_TECH_SEARCH_PARA:
							case ANTI_DARE_COUNTRY_FORMAL_PARA:
							case COUNTRY_NOTE_PARA:
							case CANCEL_DIPLOMAT_PARA:    //soke 罢免外交官（停用）
							case CANCEL_CATCHER_PARA:     //soke 国王罢免捕头（左上）
							case CANCEL_CATCHEX_PARA:     //soke 国王罢免捕头（左下）
							case CANCEL_CENSOR_PARA:      //soke 国王罢免御史大夫（右上）
							case CANCEL_CENSOS_PARA:      //soke 国王罢免御史大夫（右下）
							case CANCEL_YUANSHUAI_PARA:   //soke 国王罢免元帅
							case CANCEL_ZAIXIANG_PARA:    //soke 国王罢免宰相
							
							case APPOINT_DIPLOMAT_PARA:   //soke 任命外交官（停用）
							case APPOINT_CATCHER_PARA:    //soke 国王任命捕头（左上）
							case APPOINT_CATCHEX_PARA:    //soke 国王任命捕头（左下）
							case APPOINT_CENSOR_PARA:     //soke 国王任命御史大夫（右上）
							case APPOINT_CENSOS_PARA:     //soke 国王任命御史大夫（右下）
							case APPOINT_YUANSHUAI_PARA:  //soke 国王任命元帅
							case APPOINT_ZAIXIANG_PARA:   //soke 国王任命宰相
							case REQ_DAILY_EMPEROR_MONEY:
							case REQ_KING_LIST_PARA:
								{
									if (forwardSession(ptNullCmd, nCmdLen))
									{
										return true;
									}

								}
								break;
							default:
								{
									if (forwardSession(ptNullCmd, nCmdLen))
									{
										return true;
									}
								}
								break;
						}

					}
					else
					{
						Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
					}
					return true;
				}
				break;
			case VOTE_USERCMD:
				{
					if (pUser && pUser->isPlayState())
					{
						switch(ptNullCmd->byParam)
						{
							case REQUEST_VOTE_LIST_PARA:
							case COMMIT_VOTE_PARA:
							case REQ_ARMY_GEN_PARA:
								{
									if (forwardSession(ptNullCmd, nCmdLen))
									{
										return true;
									}

								}
								break;
							default:
								{
									if (forwardSession(ptNullCmd, nCmdLen))
									{
										return true;
									}
								}
								break;
						}

					}
					else
					{
						Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
					}
					return true;
				}
				break;
			case ARMY_USERCMD:
				if (pUser && pUser->isPlayState())
				{
					switch (ptNullCmd->byParam)
					{
						case CREATE_ARMY_PARA:
						case REQ_WAIT_GEN_PARA:
						case REQ_ARMY_SPEC_PARA:
						case REMOVE_ARMY_PARA:
						case CHANGE_ARMY_NAME_PARA:
						case EXIT_ARMY_PARA:
						case ADD_ARMY_CAPTAIN_PARA:
						case FIRE_ARMY_CAPTAIN_PARA:
							{
								if (forwardSession(ptNullCmd, nCmdLen))
								{
									return true;
								}

							}
							break;
						case REQ_ARMY_LIST_PARA:
							{
								// 以上消息转发到场景服务器处理
								if(forwardScene(ptNullCmd , nCmdLen))
								{
									return true;
								}
							}
							break;
						default:
							{
								if (forwardSession(ptNullCmd, nCmdLen))
								{
									return true;
								}
							}
							break;
					}

				}
				else
				{
					Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
				}
				break;
			case GEM_USERCMD:
				{
					if (pUser && pUser->isPlayState())
					{
						switch (ptNullCmd->byParam)
						{
							case REQUEST_DRAGON_PARA:
							case REQUEST_TIGER_PARA:
								{
									if (forwardSession(ptNullCmd, nCmdLen))
									{
										return true;
									}
								}
								break;
							default:
								{
									if (forwardSession(ptNullCmd, nCmdLen))
									{
										return true;
									}
								}
								break;
						}
					}
					else
					{
						Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
					}
				}
				break;
			case DARE_USERCMD: // 战斗消息转发到会话服务器
				{
					if (pUser && pUser->isPlayState())
					{
						switch (ptNullCmd->byParam)
						{
							case ACTIVE_DARE_PARA: // 发起挑战
							case QUERY_DARE_LIST_PARA:
							case NOTIFY_DARE_PARA: // 战斗相关通知
							case GET_UNION_CITY_TAX_PARA:
								{//以上消息转发到会话服务器处理
									if (forwardSession(ptNullCmd, nCmdLen))
									{
										return true;
									}
								}
								break;
							case ACTIVE_UNION_CITY_DARE_PARA:
								{
									if(forwardScene(ptNullCmd , nCmdLen))
									{
										return true;
									}
								}
								break;
							default:
								{
									if (forwardSession(ptNullCmd, nCmdLen))
									{
										return true;
									}
								}
								break;
						}
					}
					else
					{
						Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
					}
					return true;
				}
				break;
			case NPCDARE_USERCMD: // 战斗消息转发到会话服务器
				{
					if (pUser && pUser->isPlayState())
					{
						switch (ptNullCmd->byParam)
						{
							case QUESTION_NPCDARE_INFO_PARA:
							case NPCDARE_DARE_PARA: // 发起挑战
							case NPCDARE_GETGOLD_PARA:
								{//以上消息转发到场景服务器处理
									if (forwardScene(ptNullCmd, nCmdLen))
									{
										return true;
									}
								}
								break;
							default:
								{
									if (forwardSession(ptNullCmd, nCmdLen))
									{
										return true;
									}
								}
								break;
						}
					}
					else
					{
						Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
					}
					return true;
				}
				break;
			case SEPT_USERCMD:
				{
					if(pUser && pUser->isPlayState())
					{
						switch(ptNullCmd->byParam)
						{
							case CREATE_SEPT_PARA:			// 建立家族
								{
									Cmd::stCreateSeptCmd * cmd = (stCreateSeptCmd *)ptNullCmd;
									if (!checkNewName(cmd->SeptName))
									{
										zRTime ctv;
										Cmd::stChannelChatUserCmd send;
										send.dwType=Cmd::CHAT_TYPE_SYSTEM;
										send.dwSysInfoType = Cmd::INFO_TYPE_FAIL;
										send.dwChatTime = ctv.sec();
										bzero(send.pstrName, sizeof(send.pstrName));
										bzero(send.pstrChat, sizeof(send.pstrChat));
										strncpy((char *)send.pstrChat, "对不起，家族名字不合法，请使用字母、数字和汉字",MAX_CHATINFO-1);

										sendCmd(&send, sizeof(send));
										return true;
									}
								}
							case SEPT_STATUS_CHECK_PARA:	// 家族状态检查判断此人是否可以建立新家族
							case ADD_MEMBER_TO_SEPT_PARA:	// 邀请玩家加入家族
								{							// 以上消息转发到场景服务器处理
									if(forwardScene(ptNullCmd , nCmdLen))
									{
										return true;
									}
								}
								break;
							default:
								{
									if(forwardSession(ptNullCmd , nCmdLen))
									{
										return true;
									}
								}
								break;

						}
					}
					else
					{
						Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
					}
					return true;
				}
				break;
			case RELATION_USERCMD:
				{
					if(pUser && pUser->isPlayState())
					{
						switch(ptNullCmd->byParam)
						{
							case MARRY_STATUS_CHECK_PARA:
								{		// 以上消息转发到场景服务器处理
									if(forwardScene(ptNullCmd , nCmdLen))
									{
										return true;
									}
								}
								break;
							default:
								{
									if(forwardSession(ptNullCmd , nCmdLen))
									{
										return true;
									}
								}
								break;

						}
					}
					else
					{
						Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
					}
					return true;
				}
				break;
			case SCHOOL_USERCMD:
				{
					if(pUser && pUser->isPlayState())
					{
						switch(ptNullCmd->byParam)
						{
							case Cmd::SCHOOL_STATUS_CHECK_PARA:
							case Cmd::CREATE_SCHOOL_PARA:
							case Cmd::ADD_MEMBER_TO_SCHOOL_PARA:
								{		// 以上消息转发到场景服务器处理
									if(forwardScene(ptNullCmd , nCmdLen))
									{
										return true;
									}
								}
								break;
							default:
								{
									if(forwardSession(ptNullCmd , nCmdLen))
									{
										return true;
									}
								}
								break;
						}
					}
					else
					{
						Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
					}
					return true;
				}
				break;
			case MOVE_USERCMD:
				{
					zRTime ctv;
					if (ptNullCmd->byParam == Cmd::USERMOVE_MOVE_USERCMD_PARA)
					{
						Cmd::stUserMoveMoveUserCmd send = *(Cmd::stUserMoveMoveUserCmd *)ptNullCmd;
						send.dwTimestamp = ctv.msecs();
						if(pUser && pUser->isPlayState())
						{
							if(forwardScene(&send , sizeof(send)))
							{
								return true;
							}
						}
						else
						{
							Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
						}
					}
					else
					{
						if(pUser && pUser->isPlayState())
						{
							if(forwardScene(ptNullCmd , nCmdLen))
							{
								return true;
							}
						}
						else
						{
							Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
						}
					}
					return true;
				}
				break;
			case MAGIC_USERCMD:
				{
#ifdef _DEBUGLOG
					Zebra::logger->debug("攻击指令时间戳:%ld" , ptNullCmd->dwTimestamp);
#endif
					zRTime ctv;
					if (ptNullCmd->byParam == Cmd::MAGIC_USERCMD_PARA)
					{
						Cmd::stAttackMagicUserCmd *send = (Cmd::stAttackMagicUserCmd *)ptNullCmd;
						send->dwTimestamp = ctv.msecs();
						if(pUser && pUser->isPlayState())
						{
							if(forwardScene(send , nCmdLen))
							{
								return true;
							}
						}
						else
						{
							Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
						}
					}
					else
					{
						if(pUser && pUser->isPlayState())
						{
							if(forwardScene(ptNullCmd , nCmdLen))
							{
								return true;
							}
						}
						else
						{
							Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
						}
					}
					return true;
				}
				break;
			case DATA_USERCMD:
				{
					switch(ptNullCmd->byParam)
					{
						case LEVELDEGREE_DATA_USERCMD_PARA:
							{
								if(forwardSession(ptNullCmd , nCmdLen))
								{
									return true;
								}
							}
							break;
						case LOADMAPOK_DATA_USERCMD_PARA:
							{
								if(forwardScene(ptNullCmd , nCmdLen))
								{
									return true;
								}
							}
							break;
						default:
							{
								Zebra::logger->error("无效的DATA_USERCMD从客户端发送过来byParam=%u", ptNullCmd->byParam);
							}
							break;
					}
					return true;
				}
				break;
			case PET_USERCMD: 
				{
					if (ptNullCmd->byParam==Cmd::CHANGENAME_PET_PARA)
					{
						Cmd::stChangeNamePetCmd * cmd = (stChangeNamePetCmd *)ptNullCmd;
						if (!checkNewName(cmd->name))
						{
							zRTime ctv;
							Cmd::stChannelChatUserCmd send;
							send.dwType=Cmd::CHAT_TYPE_SYSTEM;
							send.dwSysInfoType = Cmd::INFO_TYPE_FAIL;
							send.dwChatTime = ctv.sec();
							bzero(send.pstrName, sizeof(send.pstrName));
							bzero(send.pstrChat, sizeof(send.pstrChat));
							strncpy((char *)send.pstrChat, "宠物名字不合法，请使用字母、数字和汉字",MAX_CHATINFO-1);

							sendCmd(&send, sizeof(send));
							return true;
						}
					}
				}
				//注意这里没有break
			case PROPERTY_USERCMD:
#ifdef _DEBUGLOG
				if (ptNullCmd->byCmd == PROPERTY_USERCMD) Zebra::logger->error("收到人物属性操作指令！");
#endif
				//注意这里没有break
			case RELIVE_USERCMD:
				//注意这里没有break
			case TRADE_USERCMD:
				//注意这里没有break
			case MAPSCREEN_USERCMD:
				//注意这里没有break
			case TASK_USERCMD: 
				{
					if(pUser && pUser->isPlayState())
					{
						if(forwardScene(ptNullCmd , nCmdLen))
						{
							return true;
						}
					}
					else
					{
							Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
					}
					return true;
				}
				break;
			case GOLD_USERCMD:
				{
					if(GatewayService::service_gold)
					{
						if(pUser && pUser->isPlayState())
						{
							switch(ptNullCmd->byParam)
							{
								using namespace Bill;
								case QUERY_GOLD_PARA:
								{
								}
								break;
								//请求兑换金币
								case REQUEST_REDEEM_GOLD_PARA:
								{
									using namespace Bill;
									stRequestRedeemGold* cmd = (stRequestRedeemGold*)ptNullCmd;
									if((int)cmd->dwNum < 0)
									{
										Zebra::logger->error("请求兑换金币(accid=%d),数量%d,是负数,外挂所致",this->accid,cmd->dwNum);
										return true;
									}

									t_Request_Redeem_Gold_Gateway send;
									strncpy(send.account, this->account, MAX_ACCNAMESIZE);
									send.accid = this->accid;

									if (this->pUser)
									{
										send.charid = this->pUser->id;
									}

									send.point = cmd->dwNum;
									accountClient->sendCmd(&send, sizeof(send));	
									//Zebra::logger->debug("请求兑换金币(accid=%d),数量%d",this->accid,cmd->dwNum);
								}
								break;
								//请求兑换月卡
								case REQUEST_REDEEM_MONTH_CARD_PARA:
								{
									using namespace Bill;
									t_Request_Redeem_MonthCard_Gateway send;
									strncpy(send.account, this->account, MAX_ACCNAMESIZE);
									send.accid = this->accid;

									if (this->pUser)
									{
										send.charid = this->pUser->id;
									}
									accountClient->sendCmd(&send, sizeof(send));	
									//Zebra::logger->debug("请求兑换月卡(accid=%d)",this->accid);
								}
								break;
								//请求查询点卡
								case REQUEST_POINT_PARA:
								{
									t_Request_Point_Gateway send;
									strncpy(send.account, this->account, MAX_ACCNAMESIZE);
									send.accid = this->accid;
									if (this->pUser)
									{
										send.charid = this->pUser->id;
									}
									accountClient->sendCmd(&send, sizeof(send));	
									//Zebra::logger->debug("请求查询剩余点数(accid=%d)",this->accid);
								}
								break;
								case CONSUME_CARD_PARA:
								{
									Cmd::stConSumeCardCard *rev = (Cmd::stConSumeCardCard *)ptNullCmd; 
									stConSumeCardCard_Gateway send;
									send.type=rev->type;
									strncpy(send.account, this->account, MAX_ACCNAMESIZE);
									strncpy(send.cardid,rev->cardid,sizeof(send.cardid));
									send.accid = this->accid;
									accountClient->sendCmd(&send, sizeof(send));	
									Zebra::logger->debug("%s(%u)请求消费道具卡:%s,类型:%d",pUser->name , pUser->id,rev->cardid,rev->type);
									return true;
								}
								break;
								default:
								break;

							}
						}
					}
					else
					{
						stStopServiceGold ret;
						this->sendCmd(&ret,sizeof(ret));
						Zebra::logger->debug("金币系统停止维护");
					}
					
					return true;
				}
				break;
			case QUIZ_USERCMD:
				{//答题命令
					switch(ptNullCmd->byParam)
					{
						case ANSWER_QUIZ_PARA:
							{
								if (forwardSession(ptNullCmd , nCmdLen))
								{
									return true;
								}
							}
							break;
						case QUERY_QUIZ_PARA:
							{
								if (forwardSession(ptNullCmd , nCmdLen) 
									&& forwardScene(ptNullCmd, nCmdLen))
								{
									return true;
								}
							}
							break;
						case CREATE_QUIZ_PARA:
							{
								if (forwardScene(ptNullCmd , nCmdLen))
								{
									return true;
								}	
							}
							break;
						case  QUIZ_EXIT_PARA:
							{
								if (forwardSession(ptNullCmd , nCmdLen))
								{
									return true;
								}
							}
							break;
						default:
							break;
					}

					return true;
				}
				break;
			case GMTOOL_USERCMD:
			case MAIL_USERCMD:
			case AUCTION_USERCMD:
			case CARTOON_USERCMD:
			case PRISON_USERCMD:
				{
					if (forwardScene(ptNullCmd , nCmdLen))
					{
						return true;
					}
					return true;
				}
				break;
			case STOCK_SCENE_USERCMD:
				{
					if(GatewayService::service_stock)
					{
						if(pUser && pUser->isPlayState())
						{
							if(forwardScene(ptNullCmd , nCmdLen))
							{
								return true;
							}
						}
						else
						{
							Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
						}
					}
					else
					{
						stStopServiceStockUserCmd ret;
						this->sendCmd(&ret,sizeof(ret));
						Zebra::logger->debug("股票系统停止维护");
					}
					return true;
				}
				break;
			case STOCK_BILL_USERCMD:
				{
					if(GatewayService::service_stock)
					{
						if(pUser && pUser->isPlayState())
						{
							switch(ptNullCmd->byParam)
							{
								case PARA_CANCELLISTALL_STOCKPARA:
									{
										return true;
									}
									break;
								default:
									if(pUser->forwardBill(ptNullCmd,nCmdLen))
									{
										return true;
									}
							}
						}
						else
						{
							Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
						}
					}
					else
					{
						stStopServiceStockUserCmd ret;
						this->sendCmd(&ret,sizeof(ret));
						Zebra::logger->debug("股票系统停止维护");
					}
					return true;
				}
				break;
			case GIFT_USERCMD:
				{
					if(pUser && pUser->isPlayState())
						if(forwardSession(ptNullCmd, nCmdLen))
							return true;
					else
						Zebra::logger->error("连接状态不正确,忽略这条指令(%d, %d)",ptNullCmd->byCmd,ptNullCmd->byParam);
					return true;
				}
				break;
			case ALLY_USERCMD:
				{
					switch (ptNullCmd->byParam)
					{
						case REQ_COUNTRY_ALLY_INFO_PARA:
						case REQ_COUNTRY_ALLY_PARA:
						case CANCEL_COUNTRY_ALLY_PARA:
							{
								if (forwardSession(ptNullCmd , nCmdLen))
								{
									return true;
								}
							}
							break;
						/*case CREATE_QUIZ_PARA:
							{
								if (forwardScene(ptNullCmd , nCmdLen))
								{
									return true;
								}	
							}
							break;*/
						default:
							break;
					}

					return true;
				}
				break;
			case MINIGAME_USERCMD:
				{
					if(pUser && pUser->isPlayState())
					{
						Cmd::stMiniGameUserCmd * rev = (Cmd::stMiniGameUserCmd *)ptNullCmd;
						if (COMMON_MINI_PARA==rev->byParam && LOGIN_COMMON_MINI_PARA==rev->subParam)
						{
							Cmd::Mini::t_UserLogin_Gateway send;
							send.userID = pUser->id;
							send.countryID = pUser->countryID;
							send.face = pUser->face;
							send.gateServerID = GatewayService::getInstance().getServerID();
							send.sceneServerID = pUser->scene->getServerID();
							strncpy(send.name, pUser->name, MAX_NAMESIZE-1);
							return miniClient->sendCmd(&send, sizeof(send));
						}
						if (rev->byParam>10)
							return forwardScene(ptNullCmd , nCmdLen);
						else
							return forwardMini(ptNullCmd , nCmdLen);
					}
				}
				break;
			case RECOMMEND_USERCMD:
				{
					if(pUser && pUser->isPlayState())
					{
						if (forwardSession(ptNullCmd , nCmdLen))
						{
							return true;
						}
					}
				}
				break;
			////////////////////////////////////////////	
			case LOONG_USERCMD:       //太庙资源掠夺
				{
					if (pUser && pUser->isPlayState())
					{
						switch(ptNullCmd->byParam)
						{
							case REQ_FETCH_LOONGPEARL_PARA:
							case REQ_GENGHUAN_LOONGPEARL_PARA:
							case REQ_PUT_LOONGPEARL_PARA:
								{
									// 以上消息转发到场景服务器处理
									if(forwardScene(ptNullCmd , nCmdLen))
									{
										return true;
									}
								}
								break;
                            case REQ_LOONGALTAR_LIST_PARA: //soke 太庙资源列表
								{
									if (forwardSession(ptNullCmd, nCmdLen))
									{
										return true;
									}

								}
								break;
							default:
								{
									if (forwardSession(ptNullCmd, nCmdLen))
									{
										return true;
									}
								}
								break;
						}

					}
					else
					{
						Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
					}
					return true;
				}
				break;
			case MINIFUNC_USERCMD:       //通用小功能指令
				{
					if (pUser && pUser->isPlayState())
					{
						switch(ptNullCmd->byParam)
						{
							case REQ_MALE_ENTER_SPA_MINIFUNC_PARA:   //soke 进入男温泉
							case REQ_FEMALE_ENTER_SPA_MINIFUNC_PARA: //soke 进入女温泉
							case REQ_LEAVE_SPA_MINIFUNC_PARA:        //soke 离开温泉
							case REQ_END_FLY_MINIFUNC_PARA:   //soke 飞行降落
							case REQ_BEGIN_WABAO_MINIFUNC_PARA:      //soke 开始挖宝
							case REQ_END_WABAO_MINIFUNC_PARA:        //soke 停止挖宝
							case REQ_START_DANCE_OPERATE_PARA:       //soke 开始跳舞
							case REQ_STOP_DANCE_OPERATE_PARA:        //soke 停止跳舞
								{
									// 以上消息转发到场景服务器处理
									if(forwardScene(ptNullCmd , nCmdLen))
									{
										return true;
									}
								}
								break;
                            //case REQ_LOONGALTAR_LIST_PARA:
							//	{
							//		if (forwardSession(ptNullCmd, nCmdLen))
							//		{
							//			return true;
							//		}
							//	}
							//	break;
							default:
								{
									if (forwardSession(ptNullCmd, nCmdLen))
									{
										return true;
									}
								}
								break;
						}

					}
					else
					{
						Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
					}
					return true;
				}
				break;
			//副本场景7	
			case FUBEN_USERCMD:       //通用小功能指令
				{
					if (pUser && pUser->isPlayState())
					{
						switch(ptNullCmd->byParam)
						{
							case REQ_OPEN_FUBEN_PARA:
							case REQ_ENTER_FUBEN_PARA:
							case REQ_LEAVE_FUBEN_PARA:
							case REQ_OPEN_JIAZU_PARA:
								{
									// 以上消息转发到场景服务器处理
									if(forwardScene(ptNullCmd , nCmdLen))
									{
										return true;
									}
								}
								break;
                       //     case REQ_ENTER_FUBEN_PARA:
					//			{
					//				if (forwardSession(ptNullCmd, nCmdLen))
					//				{
					//					return true;
					//				}
					//			}
					//			break;
							default:
								{
									if (forwardSession(ptNullCmd, nCmdLen))
									{
										return true;
									}
								}
								break;
						}

					}
					else
					{
						Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
					}
					return true;
				}
				break;									
			case LUA_USERCMD:  //LUA脚本   
				{
					if (pUser && pUser->isPlayState())
					{
						if(forwardScene(ptNullCmd , nCmdLen))
							{
								return true;
							}

					}
					else
					{
						Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
					}
					return true;
				}
				break;
			case SAFETY_COWBOX:     
				{
					if (pUser && pUser->isPlayState())
					{
						if(forwardScene(ptNullCmd , nCmdLen))
							{
								return true;
							}

					}
					else
					{
						Zebra::logger->error("连接状态不正确,忽略这条指令(%d,%d)",ptNullCmd->byCmd,ptNullCmd->byParam);
					}
					return true;
				}
				break;
				case BOSSINFO_USERCMD://BOSS信息指令
				{
					if(forwardSession(ptNullCmd , nCmdLen))
					{
						return true;
					}
				}
				break;					
			default:
				break;
		}
	}

	Zebra::logger->error("%s(%u, %u, %u)", __PRETTY_FUNCTION__, ptNullCmd->byCmd, ptNullCmd->byParam, nCmdLen);
	return false;
}

/**
 * \brief 检查新创建的人物名字是否合法
 *
 *
 * \param newName 新名字
 * \return 是否合法
 */
bool GatewayTask::checkNewName(char * newName)
{
	/*

		*/
	DWORD i = 0;
	while (newName[i]!=0 && i<strlen(newName) && i<16)
	{
		BYTE a = newName[i];
		if (a<0x80)//asc码
		{
			if (!((a>='0' && a<='9') || (a>='A' && a<='Z') || (a>='a' && a<='z')))
				return false;
		}
		else//汉字
		{
			if (!(i<strlen(newName)-1 && i<15)) return false;

			BYTE b = newName[i+1];
			if (b==0x7F || b<0x40 || b>0xFE)//尾字节在 40-FE 之间，剔除 xx7F 一条线
				return false;

			WORD val = (a<<8)|b;
			if (!((val>=0xB0A1 && val<=0xF7FE)))//GB 2312 汉字 6763 个
						//||(val>=0xB140 && val<=0xA0FE)//CJK汉字 6080 个
						//||(val>=0xAA40 && val<=0xFEA0)))//CJK 汉字和增补的汉字 8160 个
				return false;
			i++;
		}
		i++;
	}

	if ((!wordFilter::getInstance().doFilter(newName, 16))
			||strstr(newName, "蒙面人")
			||strstr(newName, "GM")
			||strstr(newName, "GameMaster"))
		return false;

	return true;
}

