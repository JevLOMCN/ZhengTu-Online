/**
 * \file
 * \version  $Id: GateUser.cpp  $
 * \author  
 * \date 
 * \brief 实现网关用户类
 */

#include "GateUser.h"
#include "GateUserManager.h"
#include "RecordCommand.h"
#include "RecordClient.h"
#include "GatewayTask.h"
#include "SessionClient.h"
#include "SceneClient.h"
#include "GatewayServer.h"
#include "CountryInfo.h"
#include "zJpeg.h"
#include "GmToolCommand.h"


GateUser::GateUser(DWORD accID,GatewayTask *histask):zUser(),GateSelectUserSession(accID)
{
	initState();
	gatewaytask=histask;
	backSelect=false;
	scene=NULL;
	sceneTempID=0;
	mapScreenIndex=(DWORD)-1;
	hide=false;
	inserted=false;
	quiz = false;
	logout=false; 
	bzero(jpegPassport, sizeof(jpegPassport));
	bzero(sysSetting,sizeof(sysSetting));
	if(gatewaytask)
	{
		gatewaytask->pUser=this;
	}
}

GateUser::~GateUser()
{
	//lock();
	//Zebra::logger->debug(__PRETTY_FUNCTION__);
	//if(gatewaytask)
	//{
		//gatewaytask->pUser=NULL;
		//gatewaytask->Terminate();
		//gatewaytask=NULL;
	//}
	//unlock();
}


/**
 * \brief 卸载一个网关用户的信息
 *
 *
 */
void GateUser::final()
{
	lock();
	//Zebra::logger->debug(__PRETTY_FUNCTION__);
	// */
	if(tempid!=0)
	{
		//GateUserManager::getInstance()->getUserByID(this->id);
		if(this->scene && this->isPlayState())
		{
			this->scene->removeIndex(this , sceneTempID);
		}
		GateUserManager::getInstance()->removeUser(this);
		GateUserManager::getInstance()->removeCountryUser(this);
		tempid=0;
	}
	unlock();
	GateUserManager::getInstance()->removeUserOnlyByAccID(accid);
}

/**
 * \brief 刷新角色信息
 *
 */
void GateUser::refreshCharInfo()
{
	using namespace Cmd;
	BYTE buf[zSocket::MAX_DATASIZE];
	stUserInfoUserCmd *cmd = (stUserInfoUserCmd *)buf;
	constructInPlace(cmd);
	bcopy(userinfo,cmd->charInfo,sizeof(cmd->charInfo));
	int size = 0;
	cmd->size = size;
	if (GatewayService::getInstance().jpeg_passport)
	{
		void *ret = Zebra::jpegPassport(jpegPassport, sizeof(jpegPassport), &size);
		if (ret)
		{
			if (size > 0 && size <= (int)(zSocket::MAX_DATASIZE - sizeof(stUserInfoUserCmd) - 100))
			{
				//Zebra::logger->debug("生成图形验证码：%s", jpegPassport);
				cmd->size = size;
				bcopy(ret, cmd->data, size);
			}
			free(ret);
		}
	}
	if (sendCmd(cmd, sizeof(stUserInfoUserCmd) + cmd->size))
	{
		selectState();
	}
}

/**
 * \brief 验证码检测
 *
 *
 * \param passport: 验证码序列
 * \return 验证是否通过
 */
bool GateUser::checkPassport(const char *passport)
{
	if (GatewayService::getInstance().jpeg_passport)
	{
		if (0 == strncmp(jpegPassport, passport, sizeof(jpegPassport)))
		{
			Zebra::logger->debug("验证码输入成功");
			return true;
		}
		else
		{
			Zebra::logger->error("验证码错误 %s %s", jpegPassport, passport);
			using namespace Cmd;
			stServerReturnLoginFailedCmd cmd;
			cmd.byReturnCode = LOGIN_RETURN_JPEG_PASSPORT;
			if (sendCmd(&cmd, sizeof(cmd)))
			{
				refreshCharInfo();
			}
			return false;
		}
	}

	return true;
}

/**
 * \brief 设置游戏状态
 *
 *
 */
void GateUser::playState(SceneClient *s , DWORD scene_tempid)
{
	lock();
	if(scene_tempid)
	{
		this->sceneTempID=scene_tempid;
	}
	if(s)
	{
		this->scene=s;
		this->scene->freshIndex(this , this->sceneTempID ,(DWORD)-1);
	}
	systemstate = SYSTEM_STATE_PLAY;
	unlock();
}

/**
 * \brief 对选择到一个角色进行初始处理
 *
 *
 * \return 处理是否成功
 */
bool GateUser::beginSelect()
{
	lock();
	if(this->isWaitUnregState())
	{
		initState(); 
	}
	if(!logout && isInitState())
	{
		if(this->scene)
		{
		}
		//将用户加入管理器
		if(GateUserManager::getInstance()->addUserOnlyByAccID(this))
		{
			//向RecordServer请求人物选择信息
			Cmd::Record::t_Get_SelectInfo_GateRecord send;
			send.accid=accid;
			if(recordClient->sendCmd(&send,sizeof(send)))
			{
				unlock();
				return true;
			}
			else
			{
				GateUserManager::getInstance()->removeUserOnlyByAccID(accid);
				unlock();
				return false;
			}
		}
		Zebra::logger->error("添加帐号(%ld)失败",accid);


		//此时的用户可能是无效,也可能用户已经在别的网关登陆,不能对Sesion和场景进行卸载(出现down机一次,原因未知)
		/*
		GateUser *pUser = GateUserManager::getInstance()->getUserByAccID(accid);
		if (pUser)
		{
			if(sessionClient)
			{
				//清理Session中数据
				Cmd::Session::t_unregUser_GateSession send;
				send.dwUserID=pUser->id;
				send.dwSceneTempID=pUser->sceneTempID;
				send.retcode=Cmd::Session::UNREGUSER_RET_ERROR;
				sessionClient->sendCmd(&send,sizeof(send));
			}
			if (pUser->scene)
			{
				//清理Scene中数据
				Cmd::Scene::t_Unreg_LoginScene scnd;
				scnd.dwUserID=pUser->id;
				scnd.dwSceneTempID=pUser->sceneTempID;
				scnd.retcode=Cmd::Scene::UNREGUSER_RET_ERROR;
				pUser->scene->sendCmd(&scnd,sizeof(scnd));
			}
		}
		// */
	}
	unlock();
	return false;
}

/**
 * \brief 开始游戏
 *
 *
 * \return true
 */
bool GateUser::beginGame()
{
	return true;
}


/**
 * \brief 指令过虑
 *
 *
 * \return true
 */
void GateUser::cmdFilter(Cmd::stNullUserCmd *cmd,DWORD &type,char *name,DWORD &cmdLen)
{
	using namespace Cmd;
	switch (cmd->byCmd)
	{
		case CHAT_USERCMD:
			if (ALL_CHAT_USERCMD_PARAMETER == cmd->byParam)
			{
				Cmd::stChannelChatUserCmd *rev = (Cmd::stChannelChatUserCmd *)cmd;
				type=rev->dwType;
				strncpy(name,rev->pstrName,MAX_NAMESIZE);
				/**
				 * \brief 针对聊天指令先压缩下指令
				 * 这样节省压缩和加密
				 *
				 */
				BYTE buf[zSocket::MAX_DATASIZE];//={0};
				Cmd::stServerChannelChatUserCmd *ret = (Cmd::stServerChannelChatUserCmd *)buf;
				constructInPlace(ret);
				ret->dwType=rev->dwType;
				ret->dwSysInfoType=rev->dwSysInfoType;
				ret->dwCharType=rev->dwCharType;
				ret->dwChannelID=rev->dwChannelID;
				ret->dwFromID=rev->dwFromID;
				ret->dwChatTime=rev->dwChatTime;
				ret->size=rev->size;
				int nameLen=strlen(rev->pstrName)+1;
				int chatLen=strlen(rev->pstrChat)+1;
				if(nameLen>1)
					strncpy(ret->info,rev->pstrName,nameLen);
				else
					bzero((void*)ret->info,1);
				if(chatLen>1)
					bcopy(rev->pstrChat,ret->info+nameLen,chatLen);
				else
					bzero((void*)(ret->info+nameLen),1);
				char *temp =ret->info + nameLen + chatLen;
				if(rev->size)
				{
					bcopy(rev->tobject_array,temp,rev->size * sizeof(stTradeObject));
				}
				cmdLen = cmdLen - (MAX_NAMESIZE - nameLen) - (MAX_CHATINFO - chatLen);
				bcopy(ret,cmd,cmdLen);
				return;
			}
			break;
		case RELATION_USERCMD:
			if (RELATION_STATUS_PARA == cmd->byParam)
			{

				stRelationStatusCmd *rev = (stRelationStatusCmd *)cmd;

				if (rev->byState == Cmd::RELATION_QUESTION)
				{
					strncpy(name,rev->name,MAX_NAMESIZE);
					return;
				}
			}
			break;
		default:
			break;
	}
}
/**
 * \brief 发送数据给客户端
 *
 *
 * \param pstrCmd: 发送的指令
 * \param nCmdLen: 指令长度
 * \return 发送是否成功
 */
bool GateUser::sendCmd(const void *pstrCmd, const unsigned int nCmdLen,const unsigned int type,const char *strName,const bool hasPacked)
{
		if(!hasPacked)
		{
			DWORD cmdLen=nCmdLen;
			using namespace Cmd;
			stNullUserCmd *cmd = (stNullUserCmd *)pstrCmd;
			switch (cmd->byCmd)
			{
				case CHAT_USERCMD:
					if (ALL_CHAT_USERCMD_PARAMETER == cmd->byParam)
					{
						stChannelChatUserCmd *rev = (stChannelChatUserCmd *)pstrCmd;
						/**
						 * \brief 针对聊天指令先压缩下指令
						 * 这样节省压缩和加密
						 *
						 */
						BYTE buf[zSocket::MAX_DATASIZE];//={0};
						stServerChannelChatUserCmd *ret = (stServerChannelChatUserCmd *)buf;
						constructInPlace(ret);
						ret->dwType=rev->dwType;
						ret->dwSysInfoType=rev->dwSysInfoType;
						ret->dwCharType=rev->dwCharType;
						ret->dwChannelID=rev->dwChannelID;
						ret->dwFromID=rev->dwFromID;
						ret->dwChatTime=rev->dwChatTime;
						ret->size=rev->size;
						int nameLen=strlen(rev->pstrName)+1;
						int chatLen=strlen(rev->pstrChat)+1;
						if(nameLen>1)
							strncpy(ret->info,rev->pstrName,nameLen);
						else
							bzero((void*)ret->info,1);
						if(chatLen>1)
							bcopy(rev->pstrChat,ret->info+nameLen,chatLen);
						else
							bzero(ret->info+nameLen,1);
						char *temp =ret->info + nameLen + chatLen;
						if(rev->size)
						{
							bcopy(rev->tobject_array,temp,rev->size * sizeof(stTradeObject));
						}
						cmdLen = cmdLen - (MAX_NAMESIZE - nameLen) - (MAX_CHATINFO - chatLen);
						//系统设置检查
						if(!this->checkChatCmd(rev->dwType,rev->pstrName)) return true; 
						if(gatewaytask)
						{
							return gatewaytask->sendCmd(ret,cmdLen);
						}
					}
					break;
				case RELATION_USERCMD:
					if (RELATION_STATUS_PARA == cmd->byParam)
					{
						stRelationStatusCmd *rev = (stRelationStatusCmd *)pstrCmd;
						std::set<std::string>::iterator sIterator;
						if (rev->byState == Cmd::RELATION_QUESTION)
						{
							rwlock.rdlock();
							sIterator = blacklist.find(rev->name);
							rwlock.unlock();
							if (sIterator != blacklist.end()) return true;
						}
					}
					break;
				default:
					break;
			}
			if(gatewaytask)
			{
				return gatewaytask->sendCmd(pstrCmd,cmdLen);
			}
		}
		else
		{
			if(type)
			{
				if(!this->checkChatCmd(type,strName)) return true;
			}
			else if(strName[0])
			{
				std::set<std::string>::iterator sIterator;

				rwlock.rdlock();
				sIterator = blacklist.find(strName);
				rwlock.unlock();
				if (sIterator != blacklist.end()) return true;
			}
			if(gatewaytask)
			{
				return gatewaytask->sendCmdNoPack(pstrCmd,nCmdLen);
			}
		}
	return false;
}

/**
 * \brief 网关注销一个用户
 *
 */
void GateUser::unreg(bool out)
{
	//防止锁加的太大
	bool need=false;
	lock();
	if(out)
	{
		logout=true;
	}
	if(isWaitPlayState())
	{
	}
	else if(isPlayState())
	{
		need=true;
		this->waitUnregState();
		if(this->scene)
		{
			this->scene->removeIndex(this , sceneTempID);
		}
		Zebra::logger->trace("注销%s(%ld)",name,id);
		//*
	}
	unlock();
	if(need)
	{
		Cmd::Session::t_unregUser_GateSession send;
		send.dwUserID=id;
		send.dwSceneTempID=sceneTempID;
		send.retcode=Cmd::Session::UNREGUSER_RET_LOGOUT;
		sessionClient->sendCmd(&send,sizeof(send));
	}
	/*/
	  Cmd::Scene::t_Unreg_LoginScene send;
	  send.dwUserID=id;
	  send.dwSceneTempID=sceneTempID;
	  scene->sendCmd(&send,sizeof(send));
	// */
}

/**
 * \brief 网关注册一个用户
 *
 *
 * \param charno: 角色序号
 */
void GateUser::reg(int charno)
{
	Cmd::Session::t_regUser_GateSession send;
	
	send.accid=accid;
	send.dwID=id;
	send.dwTempID=tempid;
	send.dwMapID=userinfo[charno].mapid ;
	send.wdLevel = userinfo[charno].level;
	send.wdOccupation = userinfo[charno].face;	
	send.wdCountry = userinfo[charno].country;
	strcpy((char*)send.byCountryName, userinfo[charno].countryName);
	strncpy((char *)send.byName,name,MAX_NAMESIZE);
	strncpy((char *)send.byMapName,userinfo[charno].mapName,MAX_NAMESIZE);
	waitPlayState();
	
	sessionClient->sendCmd(&send,sizeof(send));
}

/**
 * \brief 将一个角色添加到黑名单
 *
 *
 * \param name: 角色名称
 */
void GateUser::addBlackList(const char *name)
{
	rwlock.wrlock();
	blacklist.insert(blackListValueType(name));
	rwlock.unlock();
}

/**
 * \brief 将一个角色从黑名单中删除
 *
 *
 * \param name: 角色名称
 */
void GateUser::removeBlackList(const char *name)
{
	std::set<std::string>::iterator sIterator;
	rwlock.wrlock();
	sIterator = blacklist.find(name);
	if (sIterator != blacklist.end()) blacklist.erase(sIterator);
	rwlock.unlock();
}

/**
 * \brief 中断连接
 *
 */
void GateUser::Terminate()
{
	if(gatewaytask)
		gatewaytask->TerminateWait();
}

/**
	 * \brief 获取帐号
	 *
	 */
const char* GateUser::getAccount()
{
	if (gatewaytask)
		return gatewaytask->account;
	return NULL;
}
bool GateUser::forwardSceneBill(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen)
{
	if(gatewaytask)
	{
		return gatewaytask->forwardSceneBill(ptNullCmd,nCmdLen);
	}
	return false;
}
bool GateUser::forwardScene(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen)
{
	if(gatewaytask)
	{
		return gatewaytask->forwardScene(ptNullCmd,nCmdLen);
	}
	return false;
}
bool GateUser::forwardBillScene(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen)
{
	if(gatewaytask)
	{
		return gatewaytask->forwardBillScene(ptNullCmd,nCmdLen);
	}
	return false;
}
bool GateUser::forwardBill(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen)
{
	if(gatewaytask)
	{
		return gatewaytask->forwardBill(ptNullCmd,nCmdLen);
	}
	return false;
}
void GateUser::setVip(bool b)
{
	if(gatewaytask)
	{
		gatewaytask->setVip(b);
	}
}
void GateSelectUserSession::setSelectUserInfo(const Cmd::Record::t_Ret_SelectInfo_GateRecord *ptCmd)
{
	bcopy(ptCmd->info, userinfo, sizeof(userinfo));
	for(int i = 0; i < Cmd::MAX_CHARINFO; i++)
	{
		CountryInfo::Info *info = GatewayService::getInstance().country_info.getInfo(userinfo[i].country);
		if(info)
		{
			strcpy(userinfo[i].countryName , info->countryname.c_str());
		}
	}
}
/**
 * \brief 保存用户选择的角色
 *
 *
 * \param info: 选择的角色信息
 */
void GateSelectUserSession::putSelectUserInfo(const Cmd::SelectUserInfo &info)
{
	for(int i = 0; i < Cmd::MAX_CHARINFO; i++)
	{
		if(userinfo[i].id==0 || userinfo[i].id==(DWORD)-1)
		{
			bcopy(&info, &userinfo[i], sizeof(info));
			CountryInfo::Info *info = GatewayService::getInstance().country_info.getInfo(userinfo[i].country);
			if(info)
			{
				strcpy(userinfo[i].countryName , info->countryname.c_str());
			}
			Zebra::logger->trace("id=%d name=%s level=%d mapName=%s",
					userinfo[i].id,userinfo[i].name,userinfo[i].level,userinfo[i].mapName);
			break;
		}
	}
}
bool GateUser::checkChatCmd(DWORD type, const char *strName)
{               
	using namespace Cmd;
	switch (type)
	{
		case CHAT_TYPE_NINE:
		//case CHAT_TYPE_SHOPADV:
			{
				if (!isset_state(sysSetting , USER_SETTING_CHAT_NINE))
					return false;
			}
			break;  
			/*
		case CHAT_TYPE_PRIVATE: 
		case CHAT_TYPE_FRIEND_PRIVATE:
		case CHAT_TYPE_UNION_PRIVATE:
		case CHAT_TYPE_OVERMAN_PRIVATE:
		case CHAT_TYPE_FAMILY_PRIVATE:
			{
				if (!isset_state(sysSetting , USER_SETTING_CHAT_PRIVATE))
					return false;
			}
			break;
			*/
		case CHAT_TYPE_UNION_AFFICHE: 
		case CHAT_TYPE_UNION:
			{
				if (!isset_state(sysSetting , USER_SETTING_CHAT_UNION))
					return false;
			}
			break;
		case CHAT_TYPE_OVERMAN_AFFICHE:
		case CHAT_TYPE_OVERMAN:
			{
				if (!isset_state(sysSetting , USER_SETTING_CHAT_SCHOOL))
					return false;
			}
			break;
		case CHAT_TYPE_FAMILY_AFFICHE:
		case CHAT_TYPE_FAMILY:
			{
				if (!isset_state(sysSetting , USER_SETTING_CHAT_FAMILY))
					return false;
			}
			break;
		case CHAT_TYPE_FRIEND_AFFICHE:
		case CHAT_TYPE_FRIEND:
			{
				if (!isset_state(sysSetting , USER_SETTING_CHAT_FRIEND))
					return false;
			}
			break;
		case CHAT_TYPE_COUNTRY:
			{
				if (!isset_state(sysSetting , USER_SETTING_CHAT_COUNTRY))
					return false;
			}
			break;
		case CHAT_TYPE_TEAM:
			{
				if (!isset_state(sysSetting , USER_SETTING_CHAT_TEAM))
					return false;
			}
			break;
		case CHAT_TYPE_WHISPER:
			{
				if (!isset_state(sysSetting , USER_SETTING_CHAT_WHISPER))
					return false;
			}
			break;
		case CHAT_TYPE_AREA:
			{
				if (!isset_state(sysSetting , USER_SETTING_CHAT_AREA))
					return false;
			}
			break;
		case CHAT_TYPE_WORLD:
			{
				if (!isset_state(sysSetting , USER_SETTING_CHAT_WORLD))
					return false;
			}
			break;
		default:
			break;
	}
	// 处理黑名单消息
	std::set<std::string>::iterator sIterator;

	rwlock.rdlock();
	sIterator = blacklist.find(strName);
	rwlock.unlock();
	if (sIterator != blacklist.end()) return false;
	return true;
}
