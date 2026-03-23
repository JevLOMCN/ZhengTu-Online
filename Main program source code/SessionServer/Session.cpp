/**
 * \file
 * \version  $Id: Session.cpp  $
 * \author  
 * \date 
 * \brief 实现会话类
 *
 * 
 */

#include <stdarg.h>
#include "Session.h"
#include "zMisc.h"
#include "SessionCommand.h"
#include "SceneCommand.h"
#include "SchoolManager.h"
#include "TimeTick.h"
#include "SessionChat.h"
#include "CCountryManager.h"
#include "CartoonPetService.h"
#include "SessionManager.h"

/**
 * \brief 构造函数
 * \param task 该会话的连接
 */
UserSession::UserSession(SessionTask *task):zUser(),Session(task)
{
	regTime = SessionTimeTick::currentTime;
	user_count++;
	accid=0;
	scene=NULL;
	face = 1;
	nextBlessTime.now();
	septExp =0;

	reqAdopter = 0;
}

/**
 * \brief 析构函数
 */
UserSession::~UserSession()
{
	Zebra::logger->trace("用户%s(%u)游戏时间:%u分钟",name,id,(regTime.elapse(SessionTimeTick::currentTime)/1000)/60); 
	UserSession::country_map[this->country]--;
	if((int)UserSession::country_map[this->country] < 0)
	{
		UserSession::country_map.erase(this->country);
	}
	Zebra::logger->debug("Session目前在线人数:%u",--user_count);
	SessionChannelManager::getMe().removeUser(this);
}
/**
 * \brief 检查消息类型，根据系统设置进行过滤
 * \param pstrCmd 待检查的消息
 * \param nCmdLen 消息长度
 * \return 该消息是否通过检查
 */
bool UserSession::checkChatCmd(const Cmd::stNullUserCmd *pstrCmd, const unsigned int nCmdLen) const
{
	using namespace Cmd;
	switch (pstrCmd->byCmd)
	{
	case CHAT_USERCMD:
		{
			switch (pstrCmd->byParam)
			{
			case REQUEST_TEAM_USERCMD_PARA://邀请组队
				{
					if (!isset_state(sysSetting , USER_SETTING_TEAM))
						return false;
				}
				break;
			default:
				break;
			}
		}
		break;
	case TRADE_USERCMD:
		{
			//请求交易
			if (REQUEST_TRADE_USERCMD_PARAMETER==pstrCmd->byParam)
				if (!isset_state(sysSetting , USER_SETTING_TRADE))
					return false;
		}
		break;
	case SCHOOL_USERCMD://邀请加入师门
		{
			if (ADD_MEMBER_TO_SCHOOL_PARA==pstrCmd->byParam)
			{
				stAddMemberToSchoolCmd * rev = (stAddMemberToSchoolCmd *)pstrCmd;
				if(TEACHER_QUESTION==rev->byState)
					if (!isset_state(sysSetting , USER_SETTING_SCHOOL))
					{
						UserSession * pUser = UserSessionManager::getInstance()->getUserSessionByName(rev->memberName);
						if (pUser)
								pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "%s 加入师门未开启", name);
						return false;
					}
			}
		}
		break;
	case UNION_USERCMD://邀请加入帮会
		{
			if (ADD_MEMBER_TO_UNION_PARA==pstrCmd->byParam)
				if(QUESTION==((stAddMemberToUnionCmd*)pstrCmd)->byState)
					if (!isset_state(sysSetting , USER_SETTING_UNION))
						return false;
		}
		break;
	case SEPT_USERCMD://邀请加入家族
		{
			if (ADD_MEMBER_TO_SEPT_PARA==pstrCmd->byParam)
				if(SEPT_QUESTION==((stAddMemberToSeptCmd*)pstrCmd)->byState)
					if (!isset_state(sysSetting , USER_SETTING_FAMILY))
						return false;
		}
		break;
	case RELATION_USERCMD://邀请加为好友
		{
			if (RELATION_STATUS_PARA==pstrCmd->byParam)
			{
				stRelationStatusCmd * rev = (stRelationStatusCmd *)pstrCmd;
				if(RELATION_QUESTION==rev->byState)
					if (!isset_state(sysSetting , USER_SETTING_FRIEND))
					{
						UserSession * pUser = UserSessionManager::getInstance()->getUserSessionByName(rev->name);
						if (pUser)
								pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "%s 添加好友未开启", name);
						return false;
					}
			}
		}
		break;
	default:
		break;
	}
	return true;
}

/**
 * \brief 向该玩家发送消息
 * \param pstrCmd 待发送的消息
 * \param nCmdLen 消息长度
 */
void UserSession::sendCmdToMe(const void *pstrCmd, const unsigned int nCmdLen) const 
{
	using namespace Cmd::Session;
	using namespace Cmd;

	if (!checkChatCmd((stNullUserCmd *)pstrCmd, nCmdLen)) return;

	BYTE buf[zSocket::MAX_DATASIZE];
	t_Session_ForwardUser *scmd=(t_Session_ForwardUser *)buf;
	constructInPlace(scmd);
	scmd->dwID=id;
	scmd->size=nCmdLen;
	bcopy(pstrCmd,scmd->data,nCmdLen);
	sendCmd(scmd,sizeof(t_Session_ForwardUser)+nCmdLen);
}

#define getMessage(msg,msglen,pat)	\
do	\
{	\
	va_list ap;	\
	bzero(msg, msglen);	\
	va_start(ap, pat);		\
	vsnprintf(msg, msglen - 1, pat, ap);	\
	va_end(ap);	\
}while(false)

/**
 * \brief 向玩家发送系统聊天消息
 * \param type 系统消息类型
 * \param pattern 内容
 */
void UserSession::sendSysChat(int type, const char *pattern, ...) const
{
	char buf[MAX_CHATINFO];

	getMessage(buf,MAX_CHATINFO,pattern);
	Cmd::stChannelChatUserCmd send;
	send.dwType=Cmd::CHAT_TYPE_SYSTEM;
	send.dwSysInfoType = type;
	bzero(send.pstrName, sizeof(send.pstrName));
	bzero(send.pstrChat, sizeof(send.pstrChat));
	strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);
	sendCmdToMe(&send,sizeof(send));
}

/**
 * \brief 向玩家发送GM聊天消息
 * \param type 系统消息类型
 * \param pattern 内容
 */
void UserSession::sendGmChat(int type, const char *pattern, ...) const
{
	char buf[MAX_CHATINFO];

	getMessage(buf,MAX_CHATINFO,pattern);
	Cmd::stChannelChatUserCmd send;
	send.dwType=Cmd::CHAT_TYPE_GM;
	send.dwSysInfoType = type;
	bzero(send.pstrName, sizeof(send.pstrName));
	bzero(send.pstrChat, sizeof(send.pstrChat));
	strncpy((char *)send.pstrChat,buf,MAX_CHATINFO-1);
	sendCmdToMe(&send,sizeof(send));
}

/**
 * \brief 根据消息设置玩家的友好度
 * \param rev 存放友好度信息的消息
 */
void UserSession::setFriendDegree(Cmd::Session::t_CountFriendDegree_SceneSession *rev)
{
	relationManager.setFriendDegree(rev);
}

/**
 * \brief 向场景发送好友关系信息
 * 传入的消息包含名字列表，发送玩家和这些名字的友好度
 * \param rev 包含名字列表的消息
 */
void UserSession::sendFriendDegree(Cmd::Session::t_RequestFriendDegree_SceneSession *rev)
{
	BYTE buf[zSocket::MAX_DATASIZE];
	Cmd::Session::stRequestReturnMember * temp = NULL;

	Cmd::Session::t_ReturnFriendDegree_SceneSession *retCmd=(Cmd::Session::t_ReturnFriendDegree_SceneSession *)buf;
	constructInPlace(retCmd);
	retCmd->size = 0;
	retCmd->dwID = id;
	retCmd->dwMapTempID = scene->tempid;
	temp = retCmd->memberlist;

	CSchoolMember *me = CSchoolM::getMe().getMember(name);

	for (int j=0; j< rev->size; j++)
	{
		CRelation *rel = NULL;
		rel = relationManager.getRelationByName(rev->namelist[j].name);
		CSchoolMember *member = CSchoolM::getMe().getMember(rev->namelist[j].name);

		if (NULL != rel)
		{
			temp->dwUserID = rel->id;
			switch(rel->type)
			{
				case Cmd::RELATION_TYPE_LOVE:
					temp->byType = Cmd::Session::TYPE_CONSORT;
					break;
				case Cmd::RELATION_TYPE_FRIEND:
					temp->byType = Cmd::Session::TYPE_FRIEND;
					break;
				default:
					Zebra::logger->error("发送好友关系有不能识别的类型 %u",rel->type);
					break;
			}
			temp->wdDegree = rel->level;
			retCmd->size++;
			temp++;
		}

		if (member && me)
		{
			if (member->getTeacher() == me)
			{
				temp->dwUserID = member->getCharID();
				temp->byType = Cmd::Session::TYPE_PRENTICE;
				temp->wdDegree = member->getDegree();
				retCmd->size++;
				temp++;
			}
			else if(me->getTeacher() == member)
			{
				temp->dwUserID = member->getCharID();
				temp->byType = Cmd::Session::TYPE_TEACHER;
				temp->wdDegree = me->getDegree();
				retCmd->size++;
				temp++;
			}
		}
	}

	scene->sendCmd(retCmd, sizeof(Cmd::Session::t_ReturnFriendDegree_SceneSession)+retCmd->size*sizeof(Cmd::Session::stRequestReturnMember));
}

/**
 * \brief 向scene转发消息
 * \param ptNullCmd 待转发的消息
 * \param nCmdLen 消息长度
 * \return 转发是否成功
 */
bool UserSession::forwardScene(const Cmd::stNullUserCmd *ptNullCmd, const unsigned int nCmdLen)
{
	if(scene)
	{
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::Scene::t_Scene_ForwardScene *sendCmd=(Cmd::Scene::t_Scene_ForwardScene *)buf;
		constructInPlace(sendCmd);
		sendCmd->dwUserID=id;
		sendCmd->dwSceneTempID=scene->tempid;;
		sendCmd->size=nCmdLen;
		bcopy(ptNullCmd,sendCmd->data,nCmdLen);
		scene->sendCmd(buf,sizeof(Cmd::Scene::t_Scene_ForwardScene)+nCmdLen);
		//Zebra::logger->debug("转发%ld的消息到%ld场景",pUser->id,pUser->sceneTempID);
		return true;
	}
	return false;
}

void UserSession::updateConsort()
{
	CRelation* relation = NULL;
	relation = relationManager.getMarryRelation();
	Cmd::Session::t_updateConsort send;
	
	if (relation)
	{
		send.dwUserID = id;
		send.dwConsort = relation->charid;
		CCountry* pCountry = CCountryM::getMe().find(this->country);
		if (pCountry && strncmp(pCountry->kingName, relation->name, MAX_NAMESIZE) == 0)
		{
			CCountry* c = CCountryM::getMe().find(NEUTRAL_COUNTRY_ID);
			if (c && strncmp(c->kingName, relation->name, MAX_NAMESIZE) == 0)
			{
				send.byKingConsort = 2;
			}
			else
			{
				send.byKingConsort = 1;
			}
		}
	}
	else
	{
		send.dwUserID = id;
		send.dwConsort = 0;
	}

	if (this->scene)
	{
		scene->sendCmd(&send, sizeof(send));
	}
}


void UserSession::updateCountryStar()
{
	Cmd::Session::t_updateCountryStar send;
	send.dwUserID = id;
	CCountry* pCountry = CCountryM::getMe().find(this->country);

	if (pCountry)
	{
		send.dwCountryStar = pCountry->getStar();
	}
	else
	{
		send.dwCountryStar = 0;
	}

	if (this->scene && send.dwCountryStar>0)
	{
		scene->sendCmd(&send, sizeof(send));
	}
}
