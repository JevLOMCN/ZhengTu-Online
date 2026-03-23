/**
 * \file
 * \version  $Id: CDare.cpp  $
 * \author  
 * \date 
 * \brief 实现对战管理类
 *
 */

#include "zRWLock.h"
#include "SchoolManager.h"
#include "CUnion.h"
#include "CSept.h"
#include "CDare.h"
#include "Command.h"
#include "zDBConnPool.h"
#include "SessionServer.h"
#include "SessionTaskManager.h"
#include "Session.h"
#include "CRelationManager.h"
#include "zTime.h"
#include <vector>
#include "CCityManager.h"
#include "CCountryManager.h"
#include "CDareRecordManager.h"
#include "SessionChat.h"
#include "Dare.h"
#include "CDareSept.h"
#include "CDareSeptNpc.h"
#include "CDareEmperor.h"
#include "TimeTick.h"

using namespace DareDef;

CDare::CDare(DWORD active_time, DWORD ready_time)
{
	count = 0;
	secondID = 0;
	userid1 = 0;
	userid2 = 0;
	grade1 = 0;
	grade2 = 0;
	pk1 = 0;
	pk2 = 0;
	attList.clear();
	dwWinnerID = 0;
	isAntiAtt = false;

	if (active_time == 0)
	{
		this->active_time = ::dare_active_time;
	}
	else 
	{
		this->active_time = active_time;
	}

	if (ready_time == 0)
	{
		this->ready_time = ::dare_ready_time;
	}
	else 
	{
		this->ready_time = ready_time;
	}

	start_time = time(NULL);
}

CDare::~CDare()
{
}

void CDare::timer()
{
}

void CDare::printState()
{
	for (DWORD i=0; i<attList.size(); i++)
	{
		Zebra::logger->trace("[对战跟踪]:%d(%d,%d,%s), %s", this->tempid, attList[i], secondID, str_type[this->type],
				str_state[this->state]);
	}
}

void CDare::setReadyState()
{
	rwlock.wrlock();
	this->state = CDare::DARE_READY;
	rwlock.unlock();
	this->printState();
}

void CDare::setReadyActiveState(UserSession* pUser)
{
	Cmd::Session::t_dareGold_SceneSession send;

	if (this->type == Cmd::UNION_DARE)
	{
		send.dwNum = (-dare_need_gold);
	}
		
	send.dwWarID = this->tempid;
	if (pUser && pUser->scene)
	{
		send.dwUserID = pUser->id; // 应战者ID
		this->userid2 = pUser->id;

		send.dwType = Cmd::Session::DARE_GOLD;
		send.dwWarType = this->type;

		pUser->scene->sendCmd(&send, sizeof(send));
		rwlock.wrlock();
		this->count = 0;
		this->state = CDare::DARE_READY_ACTIVE;
		rwlock.unlock();
		
		Zebra::logger->trace("对战银子跟踪：%s(%d) 扣除 %d 文挑战金", pUser->name, pUser->unionid, send.dwNum);

		this->printState();
	}
	else
	{
		// TODO:如果找不到应战者
		setReturnGoldState();
	}
}

void CDare::setReadyOverState()
{
	// TODO:通知所有相关玩家，结束其交战状态。向相关玩家发送系统消息，告之交战结束。并向赢家加入银子
	// 判断玩家是否在线

	Cmd::Session::t_dareGold_SceneSession send;     // 给胜者奖励金币
	Cmd::Session::t_enterWar_SceneSession exit_war; // 通知场景，退出对战状态

	UserSession* pUser1 = NULL;
	UserSession* pUser2 = NULL;

	bool wait_bound = false;  // 是否进入DARE_WAIT_BOUND状态
	int winner_type = 0;  // 为0，挑战者胜，为1，应战者胜，2，战平
	send.dwWarID = this->id;

	rwlock.wrlock();
	this->state = CDare::DARE_READY_OVER;
	rwlock.unlock();
	
	if (this->type == Cmd::UNION_DARE)
	{
		send.dwNum = dare_winner_gold; // 胜者所奖励的银子数
	}
	
	send.dwWarType = this->type;
	send.dwType = Cmd::Session::WINNER_GOLD;


	this->printState();
	
	exit_war.dwWarType = this->type;
	exit_war.dwStatus = 0;

	for (DWORD i=0; i<attList.size(); i++)
	{
		this->sendCmdToAllDarePlayer(&exit_war, sizeof(exit_war), attList[i]);
	}
	
	this->sendCmdToAllDarePlayer(&exit_war, sizeof(exit_war), this->secondID);

	pUser1 = UserSessionManager::getInstance()->getUserByID(this->userid1);
	pUser2 = UserSessionManager::getInstance()->getUserByID(this->userid2);

	if (grade1>grade2)
	{
		send.dwUserID = this->userid1;
		winner_type = 0;

		if (pUser1)
		{
			pUser1->scene->sendCmd(&send, sizeof(send));
		}
		else
		{// 赢方帮主不在线，进入DARE_WAIT_BOUND状态
			wait_bound = true;
		}
	}
	else if (grade1<grade2)
	{
		send.dwUserID = this->userid2;
		winner_type = 1;

		if (pUser2)
		{
			pUser2->scene->sendCmd(&send, sizeof(send));
		}
		else
		{
			wait_bound = true;
		}
	}
	else if (grade1 == grade2)
	{
		if (this->type == Cmd::UNION_DARE)
		{
			send.dwNum = dare_winner_gold/2;
		}
			
		winner_type = 2;

		if (pUser1 && pUser2)
		{
			send.dwUserID = this->userid1;
			pUser1->scene->sendCmd(&send, sizeof(send));

			send.dwUserID = this->userid2;
			pUser2->scene->sendCmd(&send, sizeof(send));
		}
		else
		{
			wait_bound = true;
		}
	}

	// 通告对战结果
	this->notifyWarResult(winner_type);
	
	if (wait_bound)
	{
		rwlock.wrlock();
		this->state = CDare::DARE_WAIT_BOUNTY;
		rwlock.unlock();
		this->printState();
	}
	else
	{
		this->setOverState();
	}
}

void CDare::setWaitBountyState()
{
	if (this->computeResult())
	{
		this->setOverState();
	}
	// 如果还未奖励成功，则保持该状态不变
}

bool CDare::computeResult()
{
	Cmd::Session::t_dareGold_SceneSession send;

	UserSession* pUser1 = NULL;
	UserSession* pUser2 = NULL;

	send.dwWarID = this->id;
	if (this->type == Cmd::UNION_DARE)
	{
		send.dwNum = dare_winner_gold;
	}

	send.dwType = Cmd::Session::WINNER_GOLD;
	send.dwWarType = this->type;

	
	pUser1 = UserSessionManager::getInstance()->getUserByID(this->userid1);
	pUser2 = UserSessionManager::getInstance()->getUserByID(this->userid2);

	if (pUser1 == NULL || pUser2 == NULL)
	{
		return false;
	}

	if (grade1>grade2)
	{
		send.dwUserID = this->userid1;

		if (pUser1 && pUser1->scene)
		{
			pUser1->scene->sendCmd(&send, sizeof(send));
		}
		else
		{// 赢方帮主不在线，进入DARE_WAIT_BOUND状态
			return false;
		}
	}
	else if (grade1<grade2)
	{
		send.dwUserID = this->userid2;

		if (pUser2 && pUser2->scene)
		{
			pUser2->scene->sendCmd(&send, sizeof(send));
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (this->type == Cmd::UNION_DARE)
		{
			send.dwNum = dare_winner_gold/2;
		}
	
		if (pUser1 && pUser2 && pUser1->scene && pUser2->scene)
		{
			send.dwUserID = this->userid1;
			pUser1->scene->sendCmd(&send, sizeof(send));

			send.dwUserID = this->userid2;
			pUser2->scene->sendCmd(&send, sizeof(send));
		}
		else
		{
			return false;
		}
	}


	return true;

}

void CDare::setOverState()
{
	rwlock.wrlock();
	this->state = CDare::DARE_OVER;
	rwlock.unlock();
	this->printState();
}

void CDare::setReturnGoldState()
{// 如果挑战者玩家已经不在线，就处于DARE_RETURN_GOLD状态。如果玩家在线，则退钱，并进入OVER状态
	Cmd::Session::t_dareGold_SceneSession send;
	UserSession* pUser = UserSessionManager::getInstance()->getUserByID(this->userid1);

#ifdef _ZJW_DEBUG
	Zebra::logger->debug("应战者拒绝或条件不足，返回金币给挑战者:%d", dare_need_gold);
#endif	
	rwlock.wrlock();

	if (pUser && pUser->scene)
	{
		send.dwUserID = this->userid1;
		if (this->type == Cmd::UNION_DARE)
		{
			send.dwNum = dare_need_gold;
		}
		send.dwType = Cmd::Session::RETURN_DARE_GOLD;
		send.dwWarType = this->type;
		
		send.dwWarID = this->tempid; // 不需要再处理
		pUser->scene->sendCmd(&send, sizeof(send));
		Zebra::logger->trace("对战银子跟踪：%s(%d) 返还 %d 文挑战金", pUser->name, pUser->unionid, send.dwNum);

		this->state = CDare::DARE_OVER;
	}
	else
	{

		this->state = CDare::DARE_RETURN_GOLD;
	}

	rwlock.unlock();

	this->printState();
}

void CDare::setActiveState()
{
	Cmd::Session::t_enterWar_SceneSession send;

	send.dwWarType = this->type;
	
	send.dwToRelationID = this->secondID;
	send.isAtt = true;
	
	for (DWORD i=0; i<attList.size(); i++)
	{
		this->sendCmdToAllDarePlayer(&send, sizeof(send), attList[i]);
	}
	
	send.isAtt = false;
	
	for (DWORD i=0; i<attList.size(); i++)
	{
		send.dwToRelationID = attList[i];
		this->sendCmdToAllDarePlayer(&send, sizeof(send), this->secondID);
	}

	rwlock.wrlock();
	this->state = CDare::DARE_ACTIVE;
	rwlock.unlock();

	this->printState();
}

void CDare::setReadyQuestionState()
{
	DWORD toUserID = 0;
	char  fromRelationName[MAX_NAMESIZE+1];
	bzero(fromRelationName, sizeof(fromRelationName));

	strncpy(fromRelationName, this->getFirstName(), MAX_NAMESIZE);
	toUserID = getSecondUserID();
	
	rwlock.wrlock();

	if (toUserID != 0)
	{
		UserSession* pDareUser = UserSessionManager::getInstance()->getUserByID(this->userid1);
		if (this->type==Cmd::UNION_DARE)
		{
			CUnion* pUnion = CUnionM::getMe().getUnionByID(this->secondID);
			if (pUnion && pDareUser)
			{
				pUnion->sendUnionDare(pDareUser, fromRelationName, this->tempid);
			}
		}
		else
		{
			userid2 = toUserID;               // 保存应战者ID
			UserSession* pUser = UserSessionManager::getInstance()->getUserByID(this->userid2);
			UserSession* pDareUser = UserSessionManager::getInstance()->getUserByID(this->userid1);

			if (pUser && pDareUser)
			{//找到有效应战者,向其发送挑战询问命令
				Cmd::stActiveDareCmd send;

				send.dwMsgType = Cmd::DARE_QUESTION;
				send.dwWarID   = this->tempid;
				send.dwDareType = this->type;
				
				strncpy(send.name, pDareUser->name, MAX_NAMESIZE);
				strncpy(send.fromRelationName, fromRelationName, MAX_NAMESIZE);
				pUser->sendCmdToMe(&send, sizeof(send));
#ifdef _ZJW_DEBUG		    
				Zebra::logger->debug("向应战者发出询问。");
#endif		    
			}
			else
			{//应战者
#ifdef _ZJW_DEBUG		    
				Zebra::logger->debug("应战者未在线");
#endif		    
			}
		}
	}
	else
	{
#ifdef _ZJW_DEBUG	    
		Zebra::logger->debug("未找到有效的应战者");
#endif	    
	}

	state = CDare::DARE_READY_QUESTION;
	rwlock.unlock();
	this->printState();
}

bool CDare::isAtt(DWORD dwID)
{
	for (DWORD i=0; i<attList.size(); i++)
	{
		if (attList[i] == dwID)
		{
			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------
CDareUnion::CDareUnion() : CDare(0,0)
{
	//pFirst = NULL;
	//pSecond = NULL;
}

CDareUnion::CDareUnion(DWORD active_time, DWORD ready_time) : CDare(active_time, ready_time)
{
}

CDareUnion::~CDareUnion()
{
}

void CDareUnion::setSecondID(DWORD dwID)
{
	this->secondID = dwID;
}

void CDareUnion::addFirstID(DWORD dwID)
{
	attList.push_back(dwID);
}

void CDareUnion::addGrade(UserSession* pAtt, UserSession* pDef)
{
	if (pAtt==NULL || pDef == NULL)
		return;

	if (pAtt->unionid == secondID)
	{
		pk2 = pk2 + 1;
		grade2 = pDef->level/10 + grade2;
	}
	else
	{
		for (DWORD i=0; i<attList.size(); i++)
		{
			if (pAtt->unionid == attList[i])
			{
				pk1 = pk1 + 1; //累加PK人数
				grade1 = pDef->level/10 + grade1;
			}
		}
	}
}

void CDareUnion::sendCmdToAllDarePlayer(Cmd::Session::t_enterWar_SceneSession* cmd, unsigned int cmdLen, unsigned int relationID)
{
	if (cmd->isAtt)
	{// 如果是给攻击方发送消息
		CUnion *pAttUnion = (CUnion*)CUnionM::getMe().getUnionByID(relationID);
		CUnion *pDefUnion = (CUnion*)CUnionM::getMe().getUnionByID(cmd->dwToRelationID);

		if (pAttUnion && pDefUnion) 
		{
			if (cmd->dwStatus ==1)
			{
				for (int i=0; i<3; i++)
				{
					pAttUnion->sendUnionNotify("我帮对 %s 帮发起了帮会战，请各位帮众注意安全", 
							pDefUnion->name);
				}
			}

			pAttUnion->sendCmdToAllMemberScene(cmd, cmdLen);
		}
	}
	else
	{
		CUnion *pDefUnion = (CUnion*)CUnionM::getMe().getUnionByID(relationID);
		CUnion *pAttUnion = (CUnion*)CUnionM::getMe().getUnionByID(cmd->dwToRelationID);

		if (pAttUnion && pDefUnion) 
		{
			if (cmd->dwStatus ==1)
			{
				for (int i=0; i<3; i++)
				{
					pDefUnion->sendUnionNotify("我帮对 %s 帮发起了帮会战，请各位帮众注意安全", 
							pAttUnion->name);
				}
			}

			pDefUnion->sendCmdToAllMemberScene(cmd, cmdLen);	
		}
	}
}

void CDareUnion::sendActiveStateToScene(UserSession* pUser)
{
	Cmd::Session::t_enterWar_SceneSession send;
        send.dwWarType = type;
	
	if (secondID != pUser->unionid)
	{
		send.dwToRelationID = secondID;
		send.isAtt = true;
		send.dwUserID = pUser->id;
		send.dwSceneTempID = pUser->scene->tempid;
		pUser->scene->sendCmd(&send, sizeof(send));
	}
	else
	{
		for (DWORD i=0; i<attList.size(); i++)
		{
			send.dwToRelationID = attList[i];
			send.isAtt = false;
			send.dwUserID = pUser->id;
			send.dwSceneTempID = pUser->scene->tempid;
			pUser->scene->sendCmd(&send, sizeof(send));
		}
	}
}

void CDareUnion::notifyWarResult(int winner_type)
{
	CUnion *pDefUnion = (CUnion*)CUnionM::getMe().getUnionByID(this->secondID);
	CUnion *pAttUnion = NULL;

	if (winner_type == 0)
	{
		if (pDefUnion)
		{
			for (DWORD i=0; i<attList.size(); i++)
			{
				pAttUnion = (CUnion*)CUnionM::getMe().getUnionByID(attList[i]);
				if (pAttUnion)
				{
					pAttUnion->sendUnionNotify("\n帮会对战结束， 对战结果 %s 获胜\n %s PK人数:%d 得分:%d\n %s PK人数:%d 得分:%d", 
					pAttUnion->name, pAttUnion->name,
					this->pk1, this->grade1,
					pDefUnion->name,
					this->pk2, this->grade2);
					
					pDefUnion->sendUnionNotify("\n帮会对战结束， 对战结果 %s 失败\n %s PK人数:%d 得分:%d\n %s PK人数:%d 得分:%d", 
					pDefUnion->name, pAttUnion->name,
					this->pk1, this->grade1,
					pDefUnion->name,
					this->pk2, this->grade2);

					SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pAttUnion->dwCountryID,
							"%s 帮今日被 %s 帮打的折戟沉沙丢盔弃甲，努力奋斗啊！",
							pDefUnion->name, pAttUnion->name);
				}
			}
		}
	}
	else if (winner_type == 1)
	{
		if (pDefUnion)
		{
			for (DWORD i=0; i<attList.size(); i++)
			{
				pAttUnion = (CUnion*)CUnionM::getMe().getUnionByID(attList[i]);
				if (pAttUnion)
				{
					pAttUnion->sendUnionNotify("\n帮会对战结束， 对战结果 %s 失败\n %s PK人数:%d 得分:%d\n %s PK人数:%d 得分:%d", 
					pAttUnion->name, pAttUnion->name,
					this->pk1, this->grade1,
					pDefUnion->name,
					this->pk2, this->grade2);
				
				pDefUnion->sendUnionNotify("\n帮会对战结束， 对战结果 %s 获胜\n %s PK人数:%d 得分:%d\n %s PK人数:%d 得分:%d", 
					pDefUnion->name, pAttUnion->name,
					this->pk1, this->grade1,
					pDefUnion->name,
					this->pk2, this->grade2);

				SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pAttUnion->dwCountryID,
							"%s 帮今日被 %s 帮打的折戟沉沙丢盔弃甲，努力奋斗啊！",
							pAttUnion->name, pDefUnion->name);

				}
			}
		}
	}
	else
	{
		if (pDefUnion)
		{
			 for (DWORD i=0; i<attList.size(); i++)
			{
				pAttUnion = (CUnion*)CUnionM::getMe().getUnionByID(attList[i]);
				if (pAttUnion)
				{
					pAttUnion->sendUnionNotify("\n帮会对战结束， 对战结果 %s 战平\n %s PK人数:%d 得分:%d\n %s PK人数:%d 得分:%d", 
							pAttUnion->name, pAttUnion->name,
							this->pk1, this->grade1,
							pDefUnion->name,
							this->pk2, this->grade2);

					pDefUnion->sendUnionNotify("\n帮会对战结束， 对战结果 %s 战平\n %s PK人数:%d 得分:%d\n %s PK人数:%d 得分:%d", 
							pDefUnion->name, pAttUnion->name,
							this->pk1, this->grade1,
							pDefUnion->name,
							this->pk2, this->grade2);
				}
			}
		}
	}
}

char* CDareUnion::getFirstName()
{
	if (attList.size()>0)
	{
		CUnion *pAttUnion = (CUnion*)CUnionM::getMe().getUnionByID(attList[0]);
		if (pAttUnion)	
		{
			return pAttUnion->name;
		}
	}
	
	return NULL;
}

DWORD CDareUnion::getSecondUserID()
{
	CUnion *pSecond = (CUnion*)CUnionM::getMe().getUnionByID(this->secondID);
	if (pSecond && pSecond->master && pSecond->master->byStatus)
	{
		return pSecond->master->id;
	}

	return 0;
}

void CDareUnion::timer()
{
	rwlock.wrlock();
	CUnion *pSecond = (CUnion*)CUnionM::getMe().getUnionByID(this->secondID);
	CUnion *pAttUnion = NULL;
	
	if (this->state == CDare::DARE_ACTIVE)
	{
		if (this->count%900 == 0 && this->count!=active_time)
		{
			if (pSecond)
			{
				for (DWORD i=0; i<attList.size(); i++)
				{
					pAttUnion = (CUnion*)CUnionM::getMe().getUnionByID(attList[i]);
					if (pAttUnion)
					{
						pAttUnion->sendUnionNotify("我帮对 %s 帮发起了帮会战，请各位帮众注意安全", 
								pSecond->name);

						pSecond->sendUnionNotify("我帮对 %s 帮发起了帮会战，请各位帮众注意安全", 
								pAttUnion->name);
					}
				}
			}
		}
		else if (this->count==3000)
		{
			if (pSecond) 
			{
				for (DWORD i=0; i<attList.size(); i++)
				{
					pAttUnion = (CUnion*)CUnionM::getMe().getUnionByID(attList[i]);
					
					if (pAttUnion)
					{	
						pAttUnion->sendUnionNotify("我帮与 %s 帮对战将于 10 分钟内结束", 
								pSecond->name);
						
						pSecond->sendUnionNotify("我帮与 %s 帮对战将于 10 分钟内结束", 
								pAttUnion->name);
					}
				}
			}
		}
		else if (this->count>=3300 && (this->count%60 ==0))
		{
			if (pSecond) 
			{
				for (DWORD i=0; i<attList.size(); i++)
				{
					 pAttUnion = (CUnion*)CUnionM::getMe().getUnionByID(attList[i]);

					 if (pAttUnion)
					 {    
						 pAttUnion->sendUnionNotify("我帮与 %s 帮对战将于 %d 分钟内结束", 
								 pSecond->name, 
							(this->active_time-this->count)/60);

						 pSecond->sendUnionNotify("我帮与 %s 帮对战将于 %d 分钟内结束", 
								 pAttUnion->name, 
							(this->active_time-this->count)/60);
					 }
				}
			}
		}
	}
	
	rwlock.unlock();
}

bool CDareUnion::isInvalid()
{
	return false;
}

//-------------------------------------------------------------------------------------
CDareSchool::CDareSchool() : CDare(0, 0)
{
}

CDareSchool::~CDareSchool()
{
}

void CDareSchool::setSecondID(DWORD dwID)
{
	this->secondID = dwID;
}

void CDareSchool::addFirstID(DWORD dwID)
{
	attList.push_back(dwID);
}

void CDareSchool::addGrade(UserSession* pAtt, UserSession* pDef)
{
	if (pAtt==NULL || pDef == NULL)
		return;

	if (pAtt->schoolid == secondID)
	{
		pk2 = pk2 + 1;
		grade2 = pDef->level/10 + grade2;
	}
	else
	{
		for (DWORD i=0; i<attList.size(); i++)
		{
			if (pAtt->schoolid == attList[i])
			{
				pk1 = pk1 + 1; //累加PK人数
				grade1 = pDef->level/10 + grade1;
			}
		}
	}
}

void CDareSchool::sendCmdToAllDarePlayer(Cmd::Session::t_enterWar_SceneSession* cmd, unsigned int cmdLen, unsigned int relationID)
{
	if (cmd->isAtt)
	{// 如果是给攻击方发送消息
		CSchool *pAttSchool = (CSchool*)CSchoolM::getMe().getSchool(relationID);
		pAttSchool->sendCmdToAllMemberScene(cmd, cmdLen);
	}
	else
	{
		CSchool *pDefSchool = (CSchool*)CSchoolM::getMe().getSchool(relationID);
		pDefSchool->sendCmdToAllMemberScene(cmd, cmdLen);
	}
}

void CDareSchool::sendActiveStateToScene(UserSession* pUser)
{
	Cmd::Session::t_enterWar_SceneSession send;
	send.dwWarType = type;

	if (secondID != pUser->schoolid)
	{
		send.dwToRelationID = secondID;
		send.isAtt = true;
		send.dwUserID = pUser->id;
		send.dwSceneTempID = pUser->scene->tempid;
		pUser->scene->sendCmd(&send, sizeof(send));
	}
	else
	{
		for (DWORD i=0; i<attList.size(); i++)
		{
			send.dwToRelationID = attList[i];
			send.isAtt = false;
			send.dwUserID = pUser->id;
			send.dwSceneTempID = pUser->scene->tempid;
			pUser->scene->sendCmd(&send, sizeof(send));
		}
	}
}

void CDareSchool::notifyWarResult(int winner_type)
{
	 CSchool *pDefSchool = (CSchool*)CSchoolM::getMe().getSchool(this->secondID);
         CSchool *pAttSchool = NULL;
	 
	if (winner_type == 0)
	{
		if (pDefSchool)
		{
			for (DWORD i=0; i<attList.size(); i++)
			{
				 pAttSchool = (CSchool*)CSchoolM::getMe().getSchool(attList[i]);
                                 if (pAttSchool)
				 {
					 pAttSchool->notifyWarResult("师门对战结束， 对战结果: %s 获胜\n %s PK人数:%d 得分:%d\n %s PK人数:%d 得分:%d", 
							 pAttSchool->getSchoolName(), pAttSchool->getSchoolName(),
							 this->pk1, this->grade1,
							 pDefSchool->getSchoolName(), 
							 this->pk2, this->grade2);

					 pDefSchool->notifyWarResult("师门对战结束， 对战结果 %s 失败\n %s PK人数:%d 得分:%d\n %s PK人数:%d 得分:%d", 
							 pDefSchool->getSchoolName(), pAttSchool->getSchoolName(),
							 this->pk2, this->grade2,
							 pDefSchool->getSchoolName(),
							 this->pk2, this->grade2);
				 }
			}
		}
	}
	else if (winner_type == 1)
	{
		if (pDefSchool)
		{
			for (DWORD i=0; i<attList.size(); i++)
			{
				pAttSchool = (CSchool*)CSchoolM::getMe().getSchool(attList[i]);
				if (pAttSchool)
				{
					pAttSchool->notifyWarResult("师门对战结束， 对战结果: %s 战败\n %s PK人数:%d 得分:%d\n %s PK人数:%d 得分:%d", 
							pAttSchool->getSchoolName(), pAttSchool->getSchoolName(),
							this->pk1, this->grade1,
							pDefSchool->getSchoolName(), 
							this->pk2, this->grade2);

					pDefSchool->notifyWarResult("师门对战结束， 对战结果 %s 获胜\n %s PK人数:%d 得分:%d\n %s PK人数:%d 得分:%d", 
							pDefSchool->getSchoolName(), pAttSchool->getSchoolName(),
							this->pk2, this->grade2,
							pDefSchool->getSchoolName(),
							this->pk2, this->grade2);
				}
			}
		}
	}
	else 
	{
		if (pDefSchool)
		{
			for (DWORD i=0; i<attList.size(); i++)
			{
				pAttSchool = (CSchool*)CSchoolM::getMe().getSchool(attList[i]);
				if (pAttSchool)
				{
					pAttSchool->notifyWarResult("师门对战结束， 对战结果: %s 战平\n %s PK人数:%d 得分:%d\n %s PK人数:%d 得分:%d", 
							pAttSchool->getSchoolName(), pAttSchool->getSchoolName(),
							this->pk1, this->grade1,
							pDefSchool->getSchoolName(), 
							this->pk2, this->grade2);

					pDefSchool->notifyWarResult("师门对战结束， 对战结果 %s 战平\n %s PK人数:%d 得分:%d\n %s PK人数:%d 得分:%d", 
							pDefSchool->getSchoolName(),pAttSchool->getSchoolName(),
							this->pk2, this->grade2,
							pDefSchool->getSchoolName(),
							this->pk2, this->grade2);
				}
			}
		}
	}
}


char* CDareSchool::getFirstName()
{
	if (attList.size()>0)
	{
		CSchool *pAttSchool = (CSchool*)CSchoolM::getMe().getSchool(attList[0]);
		if (pAttSchool)
		{
			return pAttSchool->name;
		}
	}
	
	return NULL;
}

DWORD CDareSchool::getSecondUserID()
{
	CSchool* pSecond = CSchoolM::getMe().getSchool(this->secondID);
	if (pSecond && pSecond->getMasterNode())
	{
		return pSecond->getMasterNode()->getCharID();	
	}
	
	return 0;
}

//-------------------------------------------------------------------------------------
CDareUnionCity::CDareUnionCity(DWORD active_time, DWORD ready_time) : CDareUnion(active_time, ready_time)
{
	last_fulltime = 0;
}

CDareUnionCity::~CDareUnionCity()
{
}
		
/**
 * \brief 重载结束处理
 *
 * 重载结束处理，只发送退出对战的通知
 *
 * \param 
 * \return 
 */
void CDareUnionCity::setReadyOverState()
{
	Cmd::Session::t_enterWar_SceneSession exit_war; // 通知场景，退出对战状态
	Cmd::Session::t_clearUnionManor_SceneSession send;
	
	exit_war.dwWarType = this->type;
	exit_war.dwStatus = 0;
	
	for (DWORD i=0; i<attList.size(); i++)
	{
		this->sendCmdToAllDarePlayer(&exit_war, sizeof(exit_war), attList[i]);
	}
	
	this->sendCmdToAllDarePlayer(&exit_war, sizeof(exit_war), this->secondID);

	CCity* pCity = CCityM::getMe().findByUnionID(this->secondID);
	CUnion *pSecond = (CUnion*)CUnionM::getMe().getUnionByID(this->secondID);

	// 清理帮属场景，把失败者帮会人员清理出场景
	if (this->grade1>this->grade2)
	{
		if (pSecond) pSecond->changeAllSeptRepute(-5);
		send.dwUnionID = this->dwWinnerID;

		CUnion *pAttUnion = (CUnion*)CUnionM::getMe().getUnionByID(this->dwWinnerID);
		if (pAttUnion) 
		{
			if (pSecond && pSecond->dwCountryID!=pAttUnion->dwCountryID && pCity)
			{
				pSecond->sendUnionNotify(
						"系统公告：我国 %s 被 %s 的 %s 帮会占领",   
						pCity->getName(), CCountryM::getMe().find(pAttUnion->dwCountryID)->name, 
						pAttUnion->name);
				
				pAttUnion->sendUnionNotify(
						"系统公告：我国 %s 帮会成功占领 %s 的 %s", 
						pAttUnion->name, CCountryM::getMe().find(pSecond->dwCountryID)->name,
						pCity->getName());
				
				if (pCity->dwCountry != pAttUnion->dwCountryID)
				{//攻占了外国城市,进行世界公告
					CCountry* pAttCountry = CCountryM::getMe().find(pAttUnion->dwCountryID);
					CCountry* pDefCountry = CCountryM::getMe().find(pSecond->dwCountryID);

					SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, 
							"%s %s 帮经过一场激战成功占领 %s %s 帮会的 %s 城",
							pAttCountry->name, pAttUnion->name,
							pDefCountry->name, pSecond->name,
							pCity->getName());
				}
			}
		}
	}
	else
	{
		CCountry* pDefCountry = CCountryM::getMe().find(pSecond->dwCountryID);
		for (DWORD i=0; i<attList.size(); i++)
		{
			CUnion *pAttUnion = (CUnion*)CUnionM::getMe().getUnionByID(attList[i]);
			if (pAttUnion) 
			{
				pAttUnion->changeAllSeptRepute(-5);
				//我国X帮会成功保护了我国凤凰城/王城/凤尾村未被X国占领
				if (pSecond && pSecond->dwCountryID!=pAttUnion->dwCountryID && pCity)
				{
					pSecond->sendUnionNotify(
					"系统公告：我国 %s 帮会成功保护了 %s 未被 %s 占领",   
		    					pSecond->name, pCity->getName(), 
							CCountryM::getMe().find(pAttUnion->dwCountryID)->name);

					pAttUnion->sendUnionNotify(
					"系统公告：我国 %s 帮会进攻 %s的 %s 失败，请下次努力",   
		    					pAttUnion->name, CCountryM::getMe().find(pSecond->dwCountryID)->name,
							pCity->getName());

					if (pCity->dwCountry != pAttUnion->dwCountryID)
					{//攻占了外国城市,进行世界公告
						CCountry* pAttCountry = CCountryM::getMe().find(pAttUnion->dwCountryID);

						SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, 
								"%s %s帮攻占 %s %s帮会的 %s 城的战斗失败",
								pAttCountry->name, pAttUnion->name,
								pDefCountry->name, pSecond->name,
								pCity->getName());
					}
				}
			}
		}

		send.dwUnionID = this->secondID;
	}
	
	if (pCity)
	{
		send.dwCountryID = pCity->dwCountry;
		send.dwAreaID = pCity->dwCityID*10+1;

		DWORD map_id = (pCity->dwCountry << 16) + pCity->dwCityID*10+1;

		SceneSession *pScene = SceneSessionManager::getInstance()->getSceneByID(map_id);
		if (pScene)
		{
			Cmd::Session::t_unionDareNotify_SceneSession u;
			u.sceneID = map_id;
			u.state = 0;
			pScene->sendCmd(&u, sizeof(u));

			pScene->sendCmd(&send, sizeof(send));
		}
	}


	/*send.dwAreaID = pCity->dwCityID*10+2;
	map_id = (pCity->dwCountry << 16) + pCity->dwCityID*10+2;
	pScene = SceneSessionManager::getInstance()->getSceneByID(map_id);
	if (pScene)
	{
		pScene->sendCmd(&send, sizeof(send));
	}*/

	if (pCity)
	{
		pCity->endDare();
		pCity->writeDatabase();
	}

	this->setOverState();
}

void CDareUnionCity::timer()
{
}

void CDareUnionCity::setReadyState()
{
	CDareUnion::setReadyState();
	
	CCity* pCity = CCityM::getMe().findByUnionID(this->secondID);
	DWORD map_id = (pCity->dwCountry << 16) + pCity->dwCityID*10+1;
	SceneSession *pScene = SceneSessionManager::getInstance()->getSceneByID(map_id);
	if (pScene)
	{
		Cmd::Session::t_unionDareNotify_SceneSession u;
		u.sceneID = map_id;
		u.state = 1;
		pScene->sendCmd(&u, sizeof(u));
	}

	CUnion *pSecond = (CUnion*)CUnionM::getMe().getUnionByID(this->secondID);
	for (DWORD i=0; i<attList.size(); i++)
	{
		CUnion *pAttUnion = (CUnion*)CUnionM::getMe().getUnionByID(attList[i]);
		if (pSecond && pAttUnion) {
			if (pAttUnion->dwCountryID != pSecond->dwCountryID)
			{
				SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME,
						pSecond->dwCountryID, "%s 国 %s 帮会对我国 %s 的夺城战开始，请支援",
						CCountryM::getMe().find(pAttUnion->dwCountryID)->name,
						pAttUnion->name, pCity->getName());

				SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME,
						pAttUnion->dwCountryID, "我国 %s 帮对 %s国的 %s 的夺城战开始，请支援",
						pAttUnion->name, 
						CCountryM::getMe().find(pSecond->dwCountryID)->name,
						pCity->getName());
			}
		}
	}
}

//-------------------------------------------------------------------------------------


CDareM::CDareM()
{
	channelUniqeID =new zUniqueDWORDID(1000);
}

CDareM::~CDareM()
{
	rwlock.wrlock();
	for(zEntryTempID::hashmap::iterator it=zEntryTempID::ets.begin();it!=zEntryTempID::ets.end();it++)
	{
		CDare *temp =(CDare *)it->second;
		SAFE_DELETE(temp);
	}
	clear();
	rwlock.unlock();
	SAFE_DELETE(channelUniqeID);
}

void CDareM::destroyMe()
{
	delMe();
}

bool CDareM::getUniqeID(DWORD &tempid)
{
	tempid=channelUniqeID->get();
	return (tempid!=channelUniqeID->invalid());
}

void CDareM::putUniqeID(const DWORD &tempid)
{
	channelUniqeID->put(tempid);
}

bool CDareM::init()
{
	return true;
}


/**
 * \brief 处理场景回复的t_activeDare命令
 * \param dare 对战记录
 */
void CDareM::activeDare_sceneSession(Cmd::Session::t_activeDare_SceneSession* cmd)
{
	CDare* pDare = (CDare*)CDareM::getMe().getEntryByTempID(cmd->dwWarID);
	Zebra::logger->debug("处理对战 %ld 的ACTIVEDARE消息", cmd->dwWarID);

	if (pDare)
	{//找到对应的战斗ID
		if (cmd->dwStatus == Cmd::Session::SCENE_ACTIVEDARE_SUCCESS)
		{
			switch (pDare->state)
			{
				case CDare::DARE_READY:
					{// 收到成功扣除挑战者银子的消息,进入询问应战者状态
						Zebra::logger->debug("对战 %ld 现在处于 DARE_READY 状态，收到 ACTIVEDARE_SUCCESS", pDare->tempid);
						pDare->setReadyQuestionState();
					}
					break;
				case CDare::DARE_READY_ACTIVE:
					{ // 收到成功扣除应战者银子的消息，进入对战状态
						Zebra::logger->debug(
							"对战 %ld 现在处于 DARE_READY_ACTIVE 状态， 收到 ACTIVEDARE_SUCCESS", 								pDare->tempid);
						pDare->setActiveState();
					}
					break;
				default:
					break;
			}
		}
		else if (cmd->dwStatus == Cmd::Session::SCENE_ACTIVEDARE_FAIL)
		{
			switch (pDare->state)
			{
				case CDare::DARE_READY:
					{// 收到扣除挑战者银子失败的消息，进入对战结束状态
						Zebra::logger->debug("对战 %ld 现在处于 DARE_READY 状态，收到ACTIVEDARE_FAIL", pDare->tempid);
						pDare->setOverState();
					}
					break;
				case CDare::DARE_READY_ACTIVE:
					{// 收到扣除应战者银子失败的消息，进入退还挑战者金币的状态
						Zebra::logger->debug("对战 %ld 现在处于 DARE_READY_ACTIVE 状态，收到ACTIVEDARE_FAIL", pDare->tempid);
						pDare->setReturnGoldState();
					}
					break;
				default:
					break;

			}
		}
	}
	else
	{//不存在该战斗或该战斗已被系统取消
		Zebra::logger->error("战斗%ld不存在，或已取消", cmd->dwWarID);
	}

}

/**
 * \brief 处理场景发送过来的t_darePk_SceneSession命令
 * \param cmd 命令指令
 */

void CDareM::darePk_sceneSession(Cmd::Session::t_darePk_SceneSession* cmd)
{
	UserSession* pAtt = UserSessionManager::getInstance()->getUserByID(cmd->attID);
	UserSession* pDef = UserSessionManager::getInstance()->getUserByID(cmd->defID);
	CDare* pDare = NULL;

#ifdef _ZJW_DEBUG
	Zebra::logger->debug("对战状态，PK记分");
#endif    

	if (pAtt==NULL || pDef==NULL)
	{
		Zebra::logger->debug("用户ID无效");
		return;
	}

	pDare = findDareRecord(Cmd::UNION_DARE, pAtt->unionid, pDef->unionid);

	if (pDare)
	{
		pDare->addGrade(pAtt, pDef);

#ifdef _ZJW_DEBUG	
		Zebra::logger->debug(" union grade(%d,%d)", pDare->grade1, pDare->grade2);
#endif	
		pDare = NULL;
	}


	pDare = findDareRecord(Cmd::SCHOOL_DARE, pAtt->schoolid, pDef->schoolid);

	if (pDare)
	{
#ifdef _ZJW_DEBUG	
		Zebra::logger->debug("school grade(%d,%d)", pDare->grade1, pDare->grade2);
#endif	

		pDare->addGrade(pAtt, pDef);
		pDare = NULL;
	}

	pDare = findDareRecord(Cmd::SEPT_DARE, pAtt->septid, pDef->septid);

	if (pDare)
	{
#ifdef _ZJW_DEBUG	
		Zebra::logger->debug("sept grade(%d,%d)", pDare->grade1, pDare->grade2);
#endif	

		pDare->addGrade(pAtt, pDef);
	}
}

void CDareM::createDare_sceneSession(Cmd::Session::t_createDare_SceneSession* pCmd, std::vector<DWORD>& dare_list)
{
	if (findDareRecordByFull(pCmd->type, pCmd->relationID2, dare_list) != NULL) return;

	CDare* pDare = NULL;

	switch (pCmd->type)
	{
		case Cmd::SEPT_NPC_DARE:
			{
				pDare = new CDareSeptNpc(pCmd->active_time, pCmd->ready_time);
			}
			break;
		case Cmd::COUNTRY_FORMAL_DARE:
			{
				pDare = new CDareCountry(pCmd->active_time, pCmd->ready_time);
			}
			break;
		case Cmd::EMPEROR_DARE:
			{
				pDare = new CDareEmperor(pCmd->active_time, pCmd->ready_time);
			}
			break;
		case Cmd::COUNTRY_FORMAL_ANTI_DARE:
			{
				pDare = new CDareAntiCountry(pCmd->active_time, pCmd->ready_time);
			}
			break;
		case Cmd::UNION_CITY_DARE:
			{
				pDare = new CDareUnionCity(pCmd->active_time, pCmd->ready_time);
			}
			break;
		default:
			break;
	}

	if (pDare == NULL) return;

	pDare->type = pCmd->type;

	for (DWORD i=0; i<dare_list.size(); i++)
	{
		pDare->addFirstID(dare_list[i]);
	}
	
	pDare->setSecondID(pCmd->relationID2);

	pDare->count = 0;
	pDare->setReadyState();
	pDare->setActiveState();

	rwlock.wrlock();
	addEntry(pDare);
	rwlock.unlock();
	return;
}

void CDareM::createUnionCityDare_sceneSession(Cmd::Session::t_UnionCity_Dare_SceneSession* pCmd)
{
	CCity* pCity = CCityM::getMe().find(pCmd->dwToCountryID, pCmd->dwCityID); 
	CUnion* pUnion = CUnionM::getMe().getUnionByID(pCmd->dwFromUnionID);
	UserSession* pUser = UserSessionManager::getInstance()->getUserByID(pCmd->dwFromUserID);
	int need_money = 0;

	//判断攻打的城市是不是中立国的城市
	if ( pCmd->dwToCountryID == PUBLIC_COUNTRY)
		need_money = CREATE_UNION_NEUTRAL_CITY_DARE_NEED_PRICE_MONEY;
	else
		need_money = (pCmd->dwCityID==KING_CITY_ID)?CREATE_UNION_KING_CITY_DARE_NEED_PRICE_MONEY:CREATE_UNION_CITY_DARE_NEED_PRICE_MONEY;

	if (pUser == NULL)
	{
		Zebra::logger->error("[夺城战]:挑战者用户已不能找到。");
		return;
	}

	if (pUnion==NULL)
	{
		if (pUser) 
		{
			Cmd::Session::t_dareGold_SceneSession send;
			send.dwUserID = pCmd->dwFromUserID;
			send.dwNum = need_money;
			send.dwWarID = 0;
			send.dwType = Cmd::Session::RETURN_DARE_GOLD;
			send.dwWarType = Cmd::UNION_CITY_DARE;
			
			if (pUser && pUser->scene) 
				pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));

			Zebra::logger->trace("银子跟踪：%s(%d) 返还 %d 文挑战金(你还没有建立帮会)", pUser->name, 
					pUser->unionid, send.dwNum);

			pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你还没有建立帮会，不能发起挑战");
		}
		
		return;
	}
	
	if (pUnion && pUnion->master)
	{
		if (pUnion->master->id != pCmd->dwFromUserID)
		{
			if (pUser)
			{
				Cmd::Session::t_dareGold_SceneSession send;
				send.dwUserID = pCmd->dwFromUserID;
				send.dwNum = need_money;
				send.dwWarID = 0;
				send.dwType = Cmd::Session::RETURN_DARE_GOLD;
				send.dwWarType = Cmd::UNION_CITY_DARE;

				if (pUser && pUser->scene) 
					pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));

				Zebra::logger->trace("银子跟踪：%s(%d) 返还 %d 文挑战金(你不是帮主)", pUser->name, 
						pUser->unionid, send.dwNum);

				pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "只有帮主才能发起夺城挑战");
			}
			return;
		}
		else
		{
		///////////////////////////////////////////////
		//soke 不限制行动力可以夺城
		//if (pUnion->getActionPoint()>100000) 
		///////////////////////////////////////////////
		//soke 需要行动力夺城
		if (pUnion->getActionPoint()<1)	
		///////////////////////////////////////////////
			{
				if (pUser)
				{
					Cmd::Session::t_dareGold_SceneSession send;
					send.dwUserID = pCmd->dwFromUserID;
					send.dwNum = need_money;
					send.dwWarID = 0;
					send.dwType = Cmd::Session::RETURN_DARE_GOLD;
					send.dwWarType = Cmd::UNION_CITY_DARE;

					if (pUser && pUser->scene) 
						pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));

					Zebra::logger->trace("银子跟踪：%s(%d) 返还 %d 文挑战金(行动力不足)", 
							pUser->name, pUser->unionid, send.dwNum);
					pUser->sendSysChat(Cmd::INFO_TYPE_MSG, "行动力不足，不能发起帮会战, 请找帮会管理员，了解行动力相关信息");
				}
				return;
			}
			
			CCity* pAttCity = CCityM::getMe().findByUnionID(pUser->unionid); 
			if (pAttCity)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你已拥有城市，不能再挑战。");
				Cmd::Session::t_dareGold_SceneSession send;
				send.dwUserID = pCmd->dwFromUserID;
				send.dwNum = need_money;
				send.dwWarID = 0;
				send.dwType = Cmd::Session::RETURN_DARE_GOLD;
				send.dwWarType = Cmd::UNION_CITY_DARE;
				
				Zebra::logger->trace("银子跟踪：%s(%d) 返还 %d 文挑战金(已拥有城市)", 
							pUser->name, pUser->unionid, send.dwNum);

				if (pUser && pUser->scene) 
					pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));

				return;
			}
			
			if (pUnion->dwCountryID != pCmd->dwToCountryID && pCmd->dwCityID==KING_CITY_ID)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你不能夺取他国王城。");
				Cmd::Session::t_dareGold_SceneSession send;
				send.dwUserID = pCmd->dwFromUserID;
				send.dwNum = need_money;
				send.dwWarID = 0;
				send.dwType = Cmd::Session::RETURN_DARE_GOLD;
				send.dwWarType = Cmd::UNION_CITY_DARE;
				
				Zebra::logger->trace("银子跟踪：%s(%d) 返还 %d 文挑战金(不能挑战他国王城)", 
							pUser->name, pUser->unionid, send.dwNum);

				if (pUser && pUser->scene) 
					pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));

				return;
			}
		}
	}
	else
	{
		if (pUser) 
		{
			Cmd::Session::t_dareGold_SceneSession send;
			send.dwUserID = pCmd->dwFromUserID;
			send.dwNum = need_money;
			send.dwWarID = 0;
			send.dwType = Cmd::Session::RETURN_DARE_GOLD;
			send.dwWarType = Cmd::UNION_CITY_DARE;
	
			if (pUser && pUser->scene) 
				pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));

			Zebra::logger->trace("银子跟踪：%s(%d) 返还 %d 文挑战金(帮会不存在)", pUser->name, 
					pUser->unionid, send.dwNum);
			pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "帮会不存在");
		}
		
		return;
	}
                         
	struct tm tv1;
	time_t timValue = time(NULL);
	zRTime::getLocalTime(tv1, timValue);

	if (tv1.tm_hour<18 || tv1.tm_hour>21 || (tv1.tm_hour=21 && tv1.tm_min>30))
	{
		if (pCity == NULL)
		{
			if (pUser && CCityM::getMe().addNewCity(pCmd))
			{//添加新的拥有者，发送扣钱消息给场景
				Zebra::logger->trace("[夺城战]: %s 已获得该城所有权", pUser->name);
				pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你已获得该城的所有权");
			//	SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_ZISETROLL, pUser->country, "仰天长啸，气吞长河，我欲拔剑冲云霄，披云斩月，任逍遥风萧萧兮易水寒，壮士一去兮不复还！我命由我不由天。恭喜我国的%s登上王位。", pUser->name);
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//soke 第一次攻城全新公告 支持时间判断 显示不同公告（系统红色滚动公告）
			//soke 先取消 第一次拿城主也会显示这个公告
		/*	{
			 time_t timValue = time(NULL);
			 struct tm tmValue; 
			 zRTime::getLocalTime(tmValue, timValue);
			 if (tmValue.tm_hour<6)
			 	SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_ZISETROLL, pUser->country, "仰天长啸，气吞长河，我欲拔剑冲云霄，披云斩月，任逍遥风萧萧兮易水寒，壮士一去兮不复还！我命由我不由天。恭喜我国的%s登上王位。", pUser->name);
			 else if (tmValue.tm_hour<11)
				SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_ZISETROLL, pUser->country, "霸气无双谁与谁，血染战甲凶魔煞，一人一刀仗天涯，凶名天下无与比，封刀种田清闲年，若有谁敢犯华夏，启刀穿甲灭满门。恭喜我国的%s登上王位。", pUser->name);
			 else if (tmValue.tm_hour<15)
				SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_ZISETROLL, pUser->country, "天行健，君子以自強不息，地勢坤，君子以厚德载物。恭喜我国的%s登上王位。", pUser->name);
			 else if (tmValue.tm_hour<20)
				SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_ZISETROLL, pUser->country, "与生俱来人中首，唯我与天同齐寿. 双脚踢翻尘世浪，一肩担尽古今愁。恭喜我国的%s登上王位。", pUser->name);
			 else//soke 最后没有指定时间的 在加载下面这个公告
				SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_ZISETROLL, pUser->country, "大风泱泱，大潮滂滂。洪水图腾蛟龙，烈火涅磐凤凰。文明圣火，千古未绝者，惟我无双;和天地并存，与日月同光。恭喜我国的%s登上王位。", pUser->name);
			}*/
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				return;
			}
		}
		else
		{	if (pCmd->dwFromUnionID == pCity->dwUnionID)
			{//判断该城是否已属于自己
				if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "不能向自己发出夺城挑战");
				Cmd::Session::t_dareGold_SceneSession send;
				send.dwUserID = pCmd->dwFromUserID;
				send.dwNum = need_money;
				send.dwWarID = 0;
				send.dwType = Cmd::Session::RETURN_DARE_GOLD;
				send.dwWarType = Cmd::UNION_CITY_DARE;
				
				if (pUser && pUser->scene) 
				{
					pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));

					Zebra::logger->trace("银子跟踪：%s(%d) 返还 %d 文挑战金(不能向自己发出夺城挑战)", 
							pUser->name, pUser->unionid, send.dwNum);
				}
				return;
			}

			// 判断今天是否已挑战别人
			if (CCityM::getMe().findDareUnionByID(pUser->unionid) != NULL)
			{
				if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "今天已挑战别的帮会");

				Cmd::Session::t_dareGold_SceneSession send;
				send.dwUserID = pCmd->dwFromUserID;
				send.dwNum = need_money;
				send.dwWarID = 0;
				send.dwType = Cmd::Session::RETURN_DARE_GOLD;
				send.dwWarType = Cmd::UNION_CITY_DARE;
				
				if (pUser && pUser->scene) 
				{
					pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));
				
					Zebra::logger->trace("银子跟踪：%s(%d) 返还 %d 文挑战金(今天已挑战别的帮会)", 
							pUser->name, pUser->unionid, send.dwNum);
				}

				return;
			}
			
			// 判断拥有者是否还有效,有可能在拥有期间，解散了帮会
			CUnion* pOwn = CUnionM::getMe().getUnionByID(pCity->dwUnionID);
			if (pOwn == NULL)
			{
				if (pCity)
				{
					pCity->changeUnion(pCmd->dwFromUnionID);
					if (pCity->dwCityID == KING_CITY_ID) // 王城
					{//如果皇城改变了帮会，则同步更新国王信息
						CCountry* pCountry = CCountryM::getMe().find(pUser->country);
						if (pCountry)
						{
							pCountry->changeKing(pUser);
						}
					}
					
					if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你获得了该城的所有权");
				}
				return;
			}
			
			// 该城已经被帮会拥有，创建对战
			if (pCity->addDareList(pCmd->dwFromUnionID))
			{
				pCity->writeDatabase();
				pUnion->changeActionPoint(-1);
				
				if (pOwn && pUnion)
				{
					Zebra::logger->trace("[夺城战]:挑战成功:%s 与 %s 于19:30分开始夺城战", 
							pUnion->name, pOwn->name);
					
					
					pOwn->sendUnionNotify(
					"系统公告：%s 帮和 %s 帮争夺 本城 的战争将于今晚，\n19：30开始，请准时参加。",   
		    					pUnion->name, pOwn->name);

					pUnion->sendUnionNotify(
					"系统公告：%s 帮和 %s 帮争夺 本城 的战争将于今晚，\n19：30开始，请准时参加。",
							pUnion->name, pOwn->name);

					if (pOwn->dwCountryID != pUnion->dwCountryID)
					{
						SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME,
								pOwn->dwCountryID, "%s 国 %s 帮会对我国 %s 发起夺城战",
								CCountryM::getMe().find(pUnion->dwCountryID)->name,
								pUnion->name, pCity->getName());
							
						SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME,
								pUnion->dwCountryID, "我国 %s 帮会对 %s国 %s 发起了夺城战",
							pUnion->name,CCountryM::getMe().find(pOwn->dwCountryID)->name,
								pCity->getName());
						SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_XG1,
							pUnion->dwCountryID, "我国 %s 帮会对 %s国 %s 发起了夺城战",
							pUnion->name,CCountryM::getMe().find(pOwn->dwCountryID)->name,
							pCity->getName());
					}
				}
			}
			else
			{
				if (pUser) 
				{
					Cmd::Session::t_dareGold_SceneSession send;
					send.dwUserID = pCmd->dwFromUserID;
					send.dwNum = need_money;
					send.dwWarID = 0;
					send.dwType = Cmd::Session::RETURN_DARE_GOLD;
					send.dwWarType = Cmd::UNION_CITY_DARE;
						
					if (pUser && pUser->scene) 
						pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));

					Zebra::logger->trace("银子跟踪：%s(%d) 返还 %d 文挑战金(已接受其它帮会挑战)", 
							pUser->name, pUser->unionid, send.dwNum);

					pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "已接受其它帮会挑战");
				}
				
				return;
			}
		}
		
	}
	else
	{
		if (pUser) 
		{
			Cmd::Session::t_dareGold_SceneSession send;
			send.dwUserID = pCmd->dwFromUserID;
			send.dwNum = need_money;
			send.dwWarID = 0;
			send.dwType = Cmd::Session::RETURN_DARE_GOLD;
			send.dwWarType = Cmd::UNION_CITY_DARE;
	
			if (pUser && pUser->scene) 
				pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));

			pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "请在21点30分到第二天18点之间来挑战");
			return;
		}
	}
}

bool CDareM::processSceneMessage(const Cmd::t_NullCmd *cmd, const unsigned int cmdLen)
{
	if (cmd)
	{
		switch (cmd->para)
		{
			case Cmd::Session::PARA_BEGIN_EMPEROR_DARE:
				{
					CCountry* pCountry = CCountryM::getMe().find(NEUTRAL_COUNTRY_ID);
					
					if (pCountry)
					{
						pCountry->beginDare();
					}
					return true;
				}
				break;
			case Cmd::Session::PARA_GM_CREATE_UNION_CITY:
				{
					Cmd::Session::t_GMCreateUnionCity_SceneSession* rev = 
						(Cmd::Session::t_GMCreateUnionCity_SceneSession*)cmd;
					
					if (rev->byOpType==1)
					{
						CCity* pCity = CCityM::getMe().find(rev->dwCountryID, rev->dwCityID);
						if (pCity) pCity->beginDare();
					}
				}
				break;
			case Cmd::Session::PARA_ACTIVEDARE:
				{
					activeDare_sceneSession((Cmd::Session::t_activeDare_SceneSession*)cmd);
					return true;
				}

				break;
			case Cmd::Session::PARA_DARE_PK:
				{
					darePk_sceneSession((Cmd::Session::t_darePk_SceneSession*)cmd);
					return true;
				}
				break;
			case Cmd::Session::PARA_UNION_CITY_DARE:
				{
					createUnionCityDare_sceneSession((Cmd::Session::t_UnionCity_Dare_SceneSession*)cmd);
					return true;
				}
				break;
			case Cmd::Session::PARA_UNION_CITY_DARE_RESULT:
				{
					Cmd::Session::t_UnionCity_DareResult_SceneSession* rev = 
						(Cmd::Session::t_UnionCity_DareResult_SceneSession*)cmd;
	
					UserSession *pUser = UserSessionManager::getInstance()->getUserByID(rev->dwUserID);
					CDare* pDare = findDareRecordByID(Cmd::UNION_CITY_DARE, rev->dwUnionID);
					
					if (pDare)
					{
						pDare->grade1 = 1;
						pDare->dwWinnerID= rev->dwUnionID;
						pDare->setReadyOverState();
					}
						
					CCity* pCity = NULL;
					
					if (rev->dwCityID >1000)
					{
						pCity = CCityM::getMe().find(rev->dwCountryID, rev->dwCityID/10);
					}
					else
					{
						pCity = CCityM::getMe().find(rev->dwCountryID, rev->dwCityID);
					}

					if (pCity)
					{
						pCity->changeUnion(rev->dwUnionID);
						if (pCity->dwCityID == KING_CITY_ID && pUser) // 无双城
						{//如果王城改变了帮会，则同步更新国王信息
							CCountry* pCountry = CCountryM::getMe().find(pUser->country);
							if (pCountry)
							{
								pCountry->changeKing(pUser);
							}
						}
												
						if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你获得了该城的所有权");
					}

					return true;
				}
				break;
			case Cmd::Session::PARA_CHECK_CALLTIMES_LEADER:
				{
					Cmd::Session::t_Check_CallTimes_SceneSession *rev = (Cmd::Session::t_Check_CallTimes_SceneSession*)cmd;
					UserSession *leader = UserSessionManager::getInstance()->getUserByTempID(rev->leaderTempID);
					if(leader)
					{
						Cmd::Session::t_Return_CallTimes_SceneSession ret;
						ret.type = rev->type;
						ret.leaderTempID = rev->leaderTempID;
						ret.qwThisID = rev->qwThisID;
						ret.times=0;
						
						switch(rev->type)
						{
							case Cmd::CALL_DUTY_KING:
								{
									if(CCountryM::getMe().isKing(leader))
									{
										CCountry* pCountry = CCountryM::getMe().find(leader->country);
										if(pCountry)
										{
											if(pCountry->calltimes < 10000)//soke 国王令限用10000次
											{
												ret.times=10000-pCountry->calltimes;
												pCountry->calltimes++;
												pCountry->writeDatabase();
											}
										}
									}
								}
								break;
							case Cmd::CALL_DUTY_UNION:
								{

									CUnion* pUnion = CUnionM::getMe().getUnionByID(leader->unionid);
									if (pUnion && pUnion->tempid == leader->id)
									{
										if(pUnion->calltimes < 10000) //soke 帮会令限用10000次
										{
											ret.times=10000-pUnion->calltimes;
											pUnion->calltimes++;
											pUnion->writeDatabase();
										}
									}
								}
								break;
							case Cmd::CALL_DUTY_SEPT:
								{
									CSept* pSept = CSeptM::getMe().getSeptByID(leader->septid);
									if (pSept && pSept->tempid == leader->id)
									{
										struct tm tv1,tv2;
										zRTime::getLocalTime(tv1,SessionTimeTick::currentTime.sec());
										zRTime::getLocalTime(tv2, pSept->calldaytime);
										if(tv1.tm_mday == tv2.tm_mday)
										{
											if(pSept->calltimes < 10000) //soke 家族令限用10000次
											{
												ret.times=10000-pSept->calltimes;
												pSept->calltimes++;
												pSept->writeDatabase();
											}
										}
										else
										{
											pSept->calldaytime = SessionTimeTick::currentTime.sec();
											pSept->calltimes = 0;
											ret.times=5-pSept->calltimes;
											pSept->calltimes ++;
											pSept->writeDatabase();
										}
									}
								}
								break;
						}
						leader->scene->sendCmd(&ret,sizeof(ret));
					}
					return true;
				}
				break;
			case Cmd::Session::PARA_GOTO_LEADER:
				{
					Cmd::Session::t_GoTo_Leader_SceneSession * rev =(Cmd::Session::t_GoTo_Leader_SceneSession *)cmd;
					UserSession *leader = UserSessionManager::getInstance()->getUserByTempID(rev->leaderTempID);
					if(leader)
					{

						Cmd::Session::t_GoTo_Leader_Check_SceneSession ret;
						ret.leaderTempID = leader->tempid;
						ret.type = rev->type;
						strncpy(ret.mapName,rev->mapName,sizeof(ret.mapName));
						ret.x = rev->x;
						ret.y = rev->y;
						switch(rev->type)
						{
							case Cmd::CALL_DUTY_KING:
								{
									// struct execAll : public execEntry<CUnion>
									// {
									// 	execAll(Cmd::Session::t_GoTo_Leader_Check_SceneSession &cmd,DWORD country)
									// 	{
									// 		_ret.leaderTempID = cmd.leaderTempID;
									// 		_ret.type = cmd.type;
									// 		strncpy(_ret.mapName,cmd.mapName,sizeof(_ret.mapName));
									// 		_ret.x = cmd.x;
									// 		_ret.y = cmd.y;
									// 		_country = country;
									// 	}
									// 	Cmd::Session::t_GoTo_Leader_Check_SceneSession _ret;
									// 	DWORD _country;
									// 	bool exec(CUnion *u)
									// 	{
									// 		UserSession *pUser=NULL; 
									// 		if (u && u->master)//国王令判断是否族长
									// 		{
									// 			pUser= UserSessionManager::getInstance()->getUserByID(u->master->id);
									// 		}
									// 		if(pUser && pUser->country == _country)
									// 		{
									// 			_ret.userTempID = pUser->tempid;
									// 			pUser->scene->sendCmd(&_ret,sizeof(_ret));
									// 		}
									// 		return true;
									// 	}
									// };
									// execAll exec(ret,leader->country);
									// CUnionM::getMe().execEveryOne(exec);
									
									//ZM修改 国王令全体接令
									Cmd::stDareCallDuty ret;
									ret.leaderTempID = rev->leaderTempID;
									ret.byCallerType = rev->type;
									strncpy(ret.mapName,rev->mapName,sizeof(ret.mapName));
									ret.x = rev->x;
									ret.y = rev->y;
									SessionTaskManager::getInstance().sendCmdToCountry(leader->country,&ret,sizeof(ret));
								}
								break;
							case Cmd::CALL_DUTY_UNION:
								{
									CUnion* pUnion = CUnionM::getMe().getUnionByID(leader->unionid);
									if (pUnion)
									{
										pUnion->sendCallCmdToAllMemberScene(&ret,sizeof(ret),leader->tempid);
									}
								}
								break;
							case Cmd::CALL_DUTY_SEPT:
								{
									
									CSept* pSept = CSeptM::getMe().getSeptByID(leader->septid);
									if (pSept)
									{
										pSept->sendCallCmdToAllMemberScene(&ret,sizeof(ret),leader->tempid);
									}
								}
								break;
							default:
								{
								}
								break;
						}
					}
					else
					{
						Zebra::logger->debug("使用令牌时找不到领导");
					}
					return true;
				}
				break;
			case Cmd::Session::PARA_RESET_CALLTIMES_LEADER:
				{
					Cmd::Session::t_Reset_CallTimes_SceneSession *rev = (Cmd::Session::t_Reset_CallTimes_SceneSession*)cmd;
					UserSession *leader = UserSessionManager::getInstance()->getUserByTempID(rev->leaderTempID);
					if(leader)
					{
						CCountry* pCountry = CCountryM::getMe().find(leader->country);
						if(pCountry)
						{
							pCountry->calltimes=0;
							pCountry->writeDatabase();
						}
						CUnion* pUnion = CUnionM::getMe().getUnionByID(leader->unionid);
						if (pUnion && pUnion->tempid == leader->id)
						{
							pUnion->calltimes=0;
							pUnion->writeDatabase();
						}
						CSept* pSept = CSeptM::getMe().getSeptByID(leader->septid);
						if (pSept && pSept->tempid == leader->id)
						{
							pSept->calldaytime = SessionTimeTick::currentTime.sec();
							pSept->calltimes = 0;
							pSept->writeDatabase();
						}
					}
					return true;
				}
			default:
				break;

		}

	}

	return false;
}

void CDare::sendActiveStateToScene(UserSession* pUser)
{
	rwlock.wrlock();
	this->state = CDare::DARE_ACTIVE;
	rwlock.unlock();
}

void CDareM::userOnlineCountry(UserSession* pUser)
{
	struct execAll : public execEntry<CDare>
	{
		UserSession* u;
		execAll(UserSession* pUser) : u(pUser) {}
		
		bool exec(CDare *pDare)
		{
			if ((pDare->type == Cmd::COUNTRY_FORMAL_DARE 
					|| pDare->type == Cmd::COUNTRY_FORMAL_ANTI_DARE)
					&& (pDare->secondID== u->country || pDare->isAtt(u->country)))
			{
				pDare->sendActiveStateToScene(u);
			}

			return true;
		}
	};

	execAll myList(pUser);
	execEveryOne(myList);
}

void CDareM::userOnline(UserSession* pUser)
{// 判断该用户是否处于交战状态，如果是，则往场景发送交战记录
	CDare* pDare = NULL;

	/*if (pUser->unionid<=0 && pUser->schoolid<=0 && pUser->septid<=0)
	{
		return;
	}*/

	if (pUser->unionid>0 && ((pDare = findDareRecordByID(Cmd::UNION_DARE, pUser->unionid)) != NULL) )
	{
		if (pDare->state == CDare::DARE_ACTIVE)
		{
			pDare->sendActiveStateToScene(pUser);
		}
		else if (pDare->state == CDare::DARE_READY_QUESTION)
		{
			pDare->setReadyQuestionState();
		}
		else if (pDare->state == CDare::DARE_WAIT_BOUNTY)
		{
			pDare->setWaitBountyState();
		}
		else if (pDare->state == CDare::DARE_RETURN_GOLD)
		{
			pDare->setReturnGoldState();
		}

		pDare = NULL;
	}

	/*if (pUser->schoolid>0 && ((pDare = findDareRecordByID(Cmd::SCHOOL_DARE, pUser->schoolid)) != NULL))
	{
		if (pDare->state == CDare::DARE_ACTIVE)
		{
			pDare->sendActiveStateToScene(pUser);
		}
		else if (pDare->state == CDare::DARE_READY_QUESTION)
		{
			pDare->setReadyQuestionState();
		}
		else if (pDare->state == CDare::DARE_WAIT_BOUNTY)
		{
			pDare->setWaitBountyState();
		}
		else if (pDare->state == CDare::DARE_RETURN_GOLD)
		{
			pDare->setReturnGoldState();
		}

		pDare = NULL;
	}*/

	if (pUser->septid>0 && ((pDare = findDareRecordByID(Cmd::SEPT_DARE, pUser->septid)) != NULL))
	{
		if (pDare->state == CDare::DARE_ACTIVE)
		{
			pDare->sendActiveStateToScene(pUser);
		}
		else if (pDare->state == CDare::DARE_READY_QUESTION)
		{
			pDare->setReadyQuestionState();
		}
		else if (pDare->state == CDare::DARE_WAIT_BOUNTY)
		{
			pDare->setWaitBountyState();
		}
		else if (pDare->state == CDare::DARE_RETURN_GOLD)
		{
			pDare->setReturnGoldState();
		}

		pDare = NULL;
	}

	if (pUser->unionid>0 && ((pDare = findDareRecordByID(Cmd::UNION_CITY_DARE, pUser->unionid)) != NULL))
	{
		if (pDare->state == CDare::DARE_ACTIVE)
		{
			pDare->sendActiveStateToScene(pUser);
		}
		pDare = NULL;
	}

	/*struct onlineCallback : public CCityM::cityCallback
	{
		UserSession* _pUser;
		onlineCallback(UserSession* pUser) : _pUser(pUser){}
		
		void exec(CCity * pCity)
		{
			if (pCity && _pUser && (pCity->dwUnionID == _pUser->unionid) && pCity->dwDareUnionID>0)
			{
				if (pCity->getName()!=NULL)
				{
					_pUser->sendSysChat(Cmd::INFO_TYPE_EXP, 
							"贵帮，%s 帮会夺城战将于20:30分开始，请准时参加",
							pCity->getName());
				}
			}
		}
	};

	if (pUser->unionid>0)
	{
		onlineCallback oc(pUser);
		CCityM::getMe().execEveryCity(oc);
	}*/

	if (pUser->unionid>0)
	{
		CCity* pCity = CCityM::getMe().findByUnionID(pUser->unionid);

		if (pCity && pCity->dareSize()>0)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_EXP, "贵帮，帮会夺城战将于19:30分开始，请准时参加");
		}
		else
		{
			pCity = CCityM::getMe().findDareUnionByID(pUser->unionid);
			if (pCity)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_EXP, "贵帮，帮会夺城战将于19:30分开始，请准时参加");
			}
		}
	}

	CCountry* pCountry = CCountryM::getMe().find(pUser->country);
	if (pCountry && pCountry->dwDareCountryID>0)
	{
		CCountry* pAttCountry = CCountryM::getMe().find(pCountry->dwDareCountryID);
		if (pAttCountry)
		{
			struct tm tv1, tv2;
			time_t timValue = time(NULL);
			zRTime::getLocalTime(tv1, pAttCountry->dwDareTime);
			zRTime::getLocalTime(tv2, timValue);

			if (tv1.tm_mday!=tv2.tm_mday)
		{
				pUser->sendSysChat(Cmd::INFO_TYPE_EXP, "%s 将于今晚20:40分攻打我国，请做好防守准备", pAttCountry->name);
			pUser->sendSysChat(Cmd::INFO_TYPE_XG1, "%s 将于今晚20:40分攻打我国，请做好防守准备", pAttCountry->name);
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_EXP, "%s 将于明晚20:40分攻打我国，请做好防守准备", pAttCountry->name);
			pUser->sendSysChat(Cmd::INFO_TYPE_XG1, "%s 将于明晚20:40分攻打我国，请做好防守准备", pAttCountry->name);
		    }
		}
	}
	
	CCountry* pAttCountry = CCountryM::getMe().findByDare(pUser->country);
	if (pAttCountry)
	{
		CCountry* pDefCountry = CCountryM::getMe().findByDare(pUser->country, false);
		if (pDefCountry)
		{
			struct tm tv1, tv2;
			time_t timValue = time(NULL);
			zRTime::getLocalTime(tv1, pDefCountry->dwLastDareTime);
			zRTime::getLocalTime(tv2, timValue);

			if (tv1.tm_mday!=tv2.tm_mday)
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_EXP, "我国将于今晚20:40分攻打 %s，请做好进攻准备", pDefCountry->name);
			pUser->sendSysChat(Cmd::INFO_TYPE_XG1, "我国将于今晚20:40分攻打 %s，请做好进攻准备", pDefCountry->name);
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_EXP, "我国将于明晚20:40分攻打 %s，请做好进攻准备", pDefCountry->name);
			pUser->sendSysChat(Cmd::INFO_TYPE_XG1, "我国将于明晚20:40分攻打 %s，请做好进攻准备", pDefCountry->name);
		   }
		}
	}
	
	if (CDareM::getMe().findDareRecord(Cmd::EMPEROR_DARE, 6, 0) != NULL)
	{
		Cmd::stEnterEmperorDareActive send;
		pUser->sendCmdToMe(&send, sizeof(send));
	}
}

bool CDareM::processUserMessage(UserSession *pUser,const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen)
{
	switch (ptNullCmd->byParam)
	{
		case Cmd::ACTIVE_DARE_PARA:
			{
				Cmd::stActiveDareCmd *pCmd = (Cmd::stActiveDareCmd *)ptNullCmd;
#ifdef _ZJW_DEBUG							
				Zebra::logger->debug("接受到 %s 的ACTIVE_DARE请求。", pUser->name);
#endif							
				if (pCmd->dwMsgType == Cmd::DARE_QUESTION)
				{
					if (strcmp(pCmd->fromRelationName, 
								pCmd->toRelationName) == 0)
					{// 禁止向自己发起挑战
						if (pUser)
						{
							pUser->sendSysChat(
									Cmd::INFO_TYPE_FAIL, 
									"不能向自己发起挑战");
						}
						return true;
					}

					if (findDareRecordByRelationName(pCmd->dwDareType, 
								pCmd->fromRelationName, NULL) != NULL) 
					{
						if (pUser)
						{
							pUser->sendSysChat(
									Cmd::INFO_TYPE_FAIL, 
									"你已处于对战状态");
						}

						return true;
					}

					if (findDareRecordByRelationName(pCmd->dwDareType, 
								pCmd->toRelationName, NULL) != NULL) 
					{
						if (pUser)
						{
							pUser->sendSysChat(
									Cmd::INFO_TYPE_FAIL, 
									"对方已处于对战状态");
						}

						return true;
					}

					switch (pCmd->dwDareType)
					{
						case Cmd::SCHOOL_DARE:
							{
								CSchool *pSchool1 = (CSchool *)CSchoolM::
									getMe().getSchoolByName
									(pCmd->fromRelationName);

								CSchool *pSchool2 = (CSchool *)CSchoolM::
									getMe().getSchoolByName
									(pCmd->toRelationName);

								Cmd::Session::t_dareGold_SceneSession 
									ready_send;

								if (pSchool1 == NULL || pSchool2 == NULL)
								{
									pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "无效的师门战挑战邀请！");
									return true;
								}

								if (pSchool1->getMasterNode()== NULL)
								{
#ifdef _ZJW_DEBUG											    
									Zebra::logger->debug("师门 %s 没有师尊", pCmd->fromRelationName);
#endif											       
									return true;
								}

								if (pSchool2->getMasterNode() == NULL)
								{
#ifdef _ZJW_DEBUG											    
									Zebra::logger->debug("师门 %s 没有师尊",
											pCmd->toRelationName);
#endif
									return true;												
								}


								if (strncmp(pSchool1->getMasterNode()->name,
											pCmd->name, MAX_NAMESIZE)!=0) 
									return true;

								if (addNewDareRecord(pCmd, Cmd::SCHOOL_DARE,
											pSchool1->id, pSchool2->id, 
											pUser->id))
								{
									ready_send.dwWarID = pCmd->dwWarID;
									ready_send.dwUserID = pUser->id;
									ready_send.dwNum = (-dare_need_gold);
									if (pUser->scene->sendCmd(&ready_send, sizeof(Cmd::Session::t_dareGold_SceneSession)))
									{
#ifdef _ZJW_DEBUG													
										Zebra::logger->debug("发送扣钱成功");
#endif													
									}


								}
								else
								{
									pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "无法发起挑战");
								}
							}
							break;
						case Cmd::SEPT_DARE:
							{
								return processSeptDare(pUser, pCmd, cmdLen);	
							}
							break;
						case Cmd::UNION_DARE:
							{
								return processUnionDare(pUser, pCmd, cmdLen);
							}	
							break;
					}
					return true;
				}

				if (pCmd->dwMsgType == Cmd::DARE_YES)
				{
					// TODO:接受挑战后，向Scene发送消息，扣除相应的银子数。
					//      扣除成功，Scene回一个消息，确定某个战斗ID扣钱成功。
#ifdef _ZJW_DEBUG								
					Zebra::logger->debug("%s 同意挑战。", pUser->name);
#endif								
					CDare* pDare = (CDare*)CDareM::getMe().getEntryByTempID(pCmd->dwWarID);
					if (pDare && ((pDare->userid2<=0 
									&& pDare->type==Cmd::UNION_DARE) 
								|| pDare->type!=Cmd::UNION_DARE)
					   )
					{
						pDare->setReadyActiveState(pUser);
					}
					else
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
								"等待超时，对战已取消,或已由别人接受挑战");
					}

					return true;
				}

				if (pCmd->dwMsgType == Cmd::DARE_NO)
				{
					CDare* pDare = (CDare*)CDareM::getMe().getEntryByTempID(
							pCmd->dwWarID);

					if (pDare)
					{
						if (pDare->type != Cmd::UNION_DARE) {
							UserSession *pFromUser = UserSessionManager::getInstance()->
								getUserByID(pDare->userid1);
#ifdef _ZJW_DEBUG								
							Zebra::logger->debug("%s 拒绝挑战。", pUser->name);
#endif								

							if (pFromUser)
							{
								pFromUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "挑战被拒绝");
							}

							pDare->setReturnGoldState();
						}

						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
								"你拒绝了挑战");
					}
					else
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
								"等待超时，对战已取消");

					}

					return true;
				}
			}
			break;
		case Cmd::QUERY_DARE_LIST_PARA:
			{
				Cmd::stQueryDareList *pCmd = (Cmd::stQueryDareList *)ptNullCmd;

				if (pCmd->byType == Cmd::UNION_DARE)
				{
					CUnionM::getMe().sendDareListToUser(pUser);
				}
				else if (pCmd->byType == Cmd::SEPT_DARE)
				{
					CSeptM::getMe().sendDareListToUser(pUser);
				}

				return true;
			}
			break;
		case Cmd::GET_UNION_CITY_TAX_PARA:
			{
				CCityM::getMe().awardTaxGold(pUser);
				return true;
			}
			break;
		case Cmd::DARE_CALL_DUTY:
			{
				Cmd::stDareCallDuty *pCmd = (Cmd::stDareCallDuty *)ptNullCmd;
				UserSession *leader = UserSessionManager::getInstance()->getUserByTempID(pCmd->leaderTempID);
				if(leader)
				{
					if(pCmd->byPipeline == Cmd::PIPELINE_AGREE)
					{
						Cmd::Session::t_GoTo_Leader_SceneSession ret;
						ret.leaderTempID = pUser->tempid;//pCmd->leaderTempID;
						strncpy(ret.mapName,pCmd->mapName,sizeof(ret.mapName));
						ret.x=pCmd->x;
						ret.y=pCmd->y;
						pUser->scene->sendCmd(&ret,sizeof(ret));
					}
					else
					{
						leader->sendSysChat(Cmd::INFO_TYPE_GAME, "%s拒绝你的召唤",pUser->name);
					}
				}
				else
				{
					Zebra::logger->debug("被令牌拉人时未找到领导:%d",pCmd->leaderTempID);
				}
				return true;
			}
			break;
		default:
			break;
	}

	return false;
}

bool CDareM::processSeptDare(UserSession *pUser, Cmd::stActiveDareCmd *pCmd, const unsigned int cmdLen)
{
	CSept *pSept1 = (CSept *)CSeptM::getMe().getSeptByName(pCmd->fromRelationName);
	CSept *pSept2 = (CSept *)CSeptM::getMe().getSeptByName(pCmd->toRelationName);

	int vDareRepute[] = {0, 1, 5, 10, 20};

	if (pSept1 == NULL || pSept2 == NULL)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "无效的家族战挑战邀请！");
		return true;
	}

	if (pSept1->master == NULL)
	{
#ifdef _ZJW_DEBUG											    
		Zebra::logger->debug("家族 %s 没有族长", pCmd->fromRelationName);
#endif											       
		return true;
	}

	if (pSept2->master == NULL)
	{
#ifdef _ZJW_DEBUG											    
		Zebra::logger->debug("家族 %s 没有族长",
				pCmd->toRelationName);
#endif											    
		return true;
	}

	if (strncmp(pSept1->master->name, 
				pCmd->name, MAX_NAMESIZE)!=0) 
		return true;

	
	if (pCmd->byDareRepute<1 || pCmd->byDareRepute>4  
			|| ((int)pSept1->getRepute() < vDareRepute[pCmd->byDareRepute]) || ((int)pSept2->getRepute() < vDareRepute[pCmd->byDareRepute]))
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "声望不足或对方声望不足，挑战失败");
		return true;
	}

	if (addNewDareRecord(pCmd, Cmd::SEPT_DARE, 
				pSept1->id, pSept2->id, 
				pUser->id))
	{
		pSept1->changeRepute(-vDareRepute[pCmd->byDareRepute]);
		CDare* pDare = findDareRecord(Cmd::SEPT_DARE, pSept1->id, pSept2->id);
		((CDareSept*)pDare)->dwDareRepute = pCmd->byDareRepute;
		 pDare->setReadyQuestionState();
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "无法发起挑战");
	}

	return true;
}

bool CDareM::processUnionDare(UserSession *pUser, Cmd::stActiveDareCmd *pCmd, const unsigned int cmdLen)
{
	CUnion* pUnion1 = NULL;
	CUnion* pUnion2 = NULL;
	Cmd::Session::t_dareGold_SceneSession ready_send;
#ifdef _ZJW_DEBUG										       
	Zebra::logger->debug("%s 发起帮会挑战.", pUser->name);
	Zebra::logger->debug("查找 %s 所在帮会", pCmd->fromRelationName);
#endif											
	pUnion1 = (CUnion *)CUnionM::getMe().getUnionByName(pCmd->fromRelationName);
	if (pUnion1 == NULL)
	{
		Zebra::logger->debug("%s 帮会不存在。",
				pCmd->fromRelationName);
		return true;
	}

	pUnion2 = (CUnion *)CUnionM::getMe().getUnionByName(pCmd->toRelationName);
	if (pUnion2 == NULL)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "%s 帮会不存在", pCmd->toRelationName);
		Zebra::logger->debug("%s 帮会不存在", pCmd->toRelationName);
		return true;
	}

	if (pUnion1 == NULL || pUnion2 == NULL)
	{
		pUser->sendSysChat(
				Cmd::INFO_TYPE_FAIL, 
				"无效的帮会战挑战邀请！");
		return true;
	}

	if (pUnion1->master == NULL)
	{
#ifdef _ZJW_DEBUG											    
		Zebra::logger->debug("帮会 %s 没有帮主", pCmd->fromRelationName);
#endif											       
		return true;
	}

	if (pUnion2->master == NULL)
	{
#ifdef _ZJW_DEBUG											    
		Zebra::logger->debug("帮会 %s 没有帮主",
				pCmd->toRelationName);
#endif											    
	}


	if (!CUnionM::getMe().havePowerByName(pCmd->name, Cmd::DARE)) 
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL,
				"你无权发起挑战");
		return true;
	}

	if (pUnion1 && 
			pUnion1->getActionPoint()<1)
	{
		pUser->sendSysChat(
				Cmd::INFO_TYPE_MSG, 
				"贵帮行动力不足，不能发起挑战, 请找帮会管理员，了解行动力相关信息");
		return true;
	}
	pUnion1->changeActionPoint(-1);

	if (addNewDareRecord(pCmd, 
				Cmd::UNION_DARE, pUnion1->id, 
				pUnion2->id, pUser->id))
	{
		ready_send.dwWarID = pCmd->dwWarID;
		ready_send.dwUserID = pUser->id;
		ready_send.dwNum = (-dare_need_gold);
		ready_send.dwType = Cmd::Session::DARE_GOLD;
		ready_send.dwWarType = Cmd::UNION_DARE;

		if (pUser->scene->sendCmd(&ready_send, sizeof(Cmd::Session::t_dareGold_SceneSession)))
		{

			Zebra::logger->trace("银子跟踪：%s(%d) 扣除 %d 文挑战金", pUser->name, pUser->unionid, ready_send.dwNum);
#ifdef _ZJW_DEBUG													
			Zebra::logger->debug("发送扣钱成功");
#endif													
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "无法发起挑战");
	}

	return true;
}

void  CDareM::timer()
{
	struct execAll : public execEntry<CDare>
	{
		std::vector<CDare *> _removeList;

		execAll(){}

		void removeList()
		{
			std::vector<CDare *>::iterator tIterator;
			for (tIterator = _removeList.begin(); tIterator != _removeList.end(); tIterator++)
			{
				CDare *cd = *tIterator;
				CDareM::getMe().removeEntry(cd);
				SAFE_DELETE(cd);
			}
			_removeList.clear();
		}

		~execAll(){}

		bool exec(CDare *pDare)
		{
			pDare->updateTime();
			//unsigned int active_time = atoi(Zebra::global["dare_active_time"].c_str());
			//unsigned int ready_time  = atoi(Zebra::global["dare_ready_time"].c_str());
			
			if (pDare->type == Cmd::UNION_CITY_DARE)
			{
				if (!pDare->isActivePeriod() && pDare->state == CDare::DARE_ACTIVE)
				{
					pDare->setReadyOverState();
				}
			}
			else if (pDare->type == Cmd::SEPT_NPC_DARE 
				|| pDare->type == Cmd::COUNTRY_FORMAL_DARE 
				|| pDare->type == Cmd::COUNTRY_FORMAL_ANTI_DARE
				|| pDare->type == Cmd::EMPEROR_DARE)
			{
				if (!pDare->isActivePeriod() && pDare->state == CDare::DARE_ACTIVE)
				{
					// TODO 交战结束处理
					pDare->setReadyOverState();
				}
			}
			else
			{
				pDare->timer();

				if (!pDare->isReadyPeriod() && (pDare->state == CDare::DARE_READY))
				{
					// TODO 删除邀请过期的交战记录
					pDare->setReturnGoldState();
					//_removeList.push_back(pDare);
				}
				else if (!pDare->isReadyPeriod() && (pDare->state == CDare::DARE_READY_QUESTION))
				{
					pDare->setReturnGoldState();
				}
				else if (!pDare->isActivePeriod() && pDare->state == CDare::DARE_ACTIVE)
				{
					// TODO 交战结束处理
					pDare->setReadyOverState();
				}
			}

			if (pDare->state == CDare::DARE_OVER)
			{
				// TODO 结束处理
				_removeList.push_back(pDare);
			}

			return true;
		}
	};

	//rwlock.wrlock();
	execAll myList;
	execEveryOne(myList);
	myList.removeList();
	//rwlock.unlock();
}

bool CDareM::addNewDareRecord(Cmd::stActiveDareCmd *pCmd, DWORD dwType, DWORD dwID1, DWORD dwID2, DWORD dwUserID)
{
	if (findDareRecord(dwType, dwID1, dwID2) != NULL) return false;

	CDare* pDare = NULL;

	switch (dwType)
	{
		case Cmd::UNION_DARE:
			{
				pDare = new CDareUnion();
				break;
			}
		case Cmd::SEPT_DARE:
			{
				pDare = new CDareSept();
				break;
			}
		default:
			break;
	}

	if (pDare == NULL) return false;

	pDare->type = dwType;

	pDare->addFirstID(dwID1);
	pDare->setSecondID(dwID2);

	pDare->userid1 = dwUserID;
	pDare->count = 0;
	rwlock.wrlock();
	addEntry(pDare);
	pDare->setReadyState();
	pCmd->dwWarID = pDare->tempid;
	rwlock.unlock();
	
	return true;
}

CDare* CDareM::findDareRecordByUser(DWORD dwType, DWORD dwUserID1, DWORD dwUserID2)
{
	struct execAll : public execEntry<CDare>
	{
		DWORD _type;
		DWORD _id1;
		DWORD _id2;
		CDare* _dare;

		execAll(DWORD dwType, DWORD dwID1, DWORD dwID2)
		{
			_type = dwType;
			_id1 = dwID1;
			_id2 = dwID2;
			_dare = NULL;
		}
		~execAll(){}

		bool exec(CDare* pDare)
		{
#ifdef _ZJW_DEBUG	    
			Zebra::logger->debug("findDareByUser(%d, %d, %d)(%d, %d, %d)", _type, _id1, _id2, pDare->type, pDare->userid1, pDare->userid2);
#endif	    
			if ((pDare->type == _type 
						&& pDare->userid1 == _id1 && pDare->userid2 == _id2)
					||
					(pDare->type == _type && pDare->userid1 == _id2 && pDare->userid2 == _id1)
					||
					(pDare->type == _type && _id2==0 && (pDare->userid1 == _id1 || pDare->userid2 == _id1))
			   )
			{
				_dare = pDare;
				return false;
			}

			return true;
		}
	};

	//rwlock.wrlock();
	execAll myList(dwType, dwUserID1, dwUserID2);
	execEveryOne(myList);
	//rwlock.unlock();

	return myList._dare;

}

CDare * CDareM::findDareRecordByRelationName(DWORD dwType, const char* r1, const char* r2)
{
	CDare* pDare = NULL;
	
	switch (dwType)
	{
		case Cmd::UNION_DARE:
			{
				CUnion* pRelation1 = NULL;
				CUnion* pRelation2 = NULL;
				
				if (r1)
				{
					pRelation1 = CUnionM::getMe().getUnionByName(r1);
				}

				if (r2)
				{
					pRelation2 = CUnionM::getMe().getUnionByName(r2);
						
				}

				if (pRelation1)
				{
					if (pRelation2)
					{
						pDare = this->findDareRecord(dwType, pRelation1->id, pRelation2->id);
					}
					else
					{
						pDare = this->findDareRecordByID(dwType, pRelation1->id);
					}
				}
			}
			break;
		case Cmd::SCHOOL_DARE:
			//pDare = new CDareSchool();
			break;
		case Cmd::SEPT_DARE:
			{
				CSept* pRelation1 = NULL;
				CSept* pRelation2 = NULL;

				if (r1)
				{
					pRelation1 = CSeptM::getMe().getSeptByName(r1);
				}

				if (r2)
				{
					pRelation2 = CSeptM::getMe().getSeptByName(r2);

				}

				if (pRelation1)
				{
					if (pRelation2)
					{
						pDare = this->findDareRecord(dwType, pRelation1->id, pRelation2->id);
					}
					else
					{
						pDare = this->findDareRecordByID(dwType, pRelation1->id);
					}
				}
			}
			break;
		default:
			break;
	}

	return pDare;

}

CDare * CDareM::findDareRecordByFull(DWORD dwType, DWORD dwDefID, std::vector<DWORD>& vDareList)
{
	struct execAll : public execEntry<CDare>
	{
		DWORD _type;
		DWORD _dwDefID;
		std::vector<DWORD> &_vDareList;
		CDare *_dare;

		execAll(DWORD dwType, DWORD dwDefID, std::vector<DWORD>& vDareList) : _type(dwType), _dwDefID(dwDefID), _vDareList(vDareList), _dare(NULL)
		{
		}
		~execAll(){}

		bool exec(CDare *pDare)
		{
			bool find = true;
			if (pDare)
			{
				if (pDare->type == _type)
				{
					if (pDare->secondID == _dwDefID)
					{
						for (DWORD i=0; i<_vDareList.size(); i++)
						{
							if (!pDare->isAtt(_vDareList[i]))
							{
								find = false;
								break;
							}
						}

						if (find)
						{
							_dare = pDare;
							return false;
						}
					}
				}
			}

			return true;
		}
	};

	execAll myList(dwType, dwDefID, vDareList);
	execEveryOne(myList);
	return myList._dare;

}
CDare * CDareM::findDareRecord(DWORD dwType, DWORD dwID1, DWORD dwID2)
{
	struct execAll : public execEntry<CDare>
	{
		DWORD _type;
		DWORD _id1;
		DWORD _id2;
		CDare *_dare;
		execAll(DWORD dwType, DWORD dwID1, DWORD dwID2)
		{
			_type = dwType;
			_id1 = dwID1;
			_id2 = dwID2;
			_dare = NULL;
		}
		~execAll(){}
		bool exec(CDare *pDare)
		{
			if (pDare)
			{
#ifdef _ZJW_DEBUG			
				Zebra::logger->debug("findDare(%d, %d, %d)", _type, _id1, _id2);
#endif			
				if (_type == Cmd::EMPEROR_DARE && pDare->type == _type)
				{// 只会有一个皇城争夺战对象
					_dare = pDare;
					return false;
				}
				else if (pDare->type == _type)
				{
					if (pDare->secondID == _id2 && pDare->isAtt(_id1)
					|| (pDare->secondID == _id1 && pDare->isAtt(_id2)))
					{
						_dare = pDare;
						return false;
					}
				}
			}

			return true;
		}
	};

	if (dwID2!=0)
	{
		execAll myList(dwType, dwID1, dwID2);
		execEveryOne(myList);
		return myList._dare;
	}
	
	return this->findDareRecordByID(dwType, dwID1);
}

CDare*   CDareM::findDareRecordByID(DWORD dwType, DWORD dwID)
{
	struct execAll : public execEntry<CDare>
	{
		DWORD _type;
		DWORD _id1;
		
		CDare *_dare;
		execAll(DWORD dwType, DWORD dwID1)
		{
			_type = dwType;
			_id1 = dwID1;
			_dare = NULL;
		}
		~execAll(){}
		bool exec(CDare *pDare)
		{
			if (pDare)
			{
#ifdef _ZJW_DEBUG			
				Zebra::logger->debug("findDare(%d, %d, %d)", _type, _id1);
#endif			
				if (pDare->type == _type)
				{
					if (pDare->secondID == _id1 || pDare->isAtt(_id1))
					{
						_dare = pDare;
						return false;
					}
				}
			}

			return true;
		}
	};

	execAll myList(dwType, dwID);
	execEveryOne(myList);
	return myList._dare;
}

//---------------------------CDareCountry----------------------------------------------------------
CDareCountry::CDareCountry() : CDare(0,0)
{
}

CDareCountry::CDareCountry(DWORD active_time, DWORD ready_time) : CDare(active_time, ready_time)
{
}

CDareCountry::~CDareCountry()
{
}

void CDareCountry::setActiveState()
{
	Cmd::Session::t_enterWar_SceneSession send;
	Cmd::Session::t_setCountryWar_SceneSession setCountryCmd;
	setCountryCmd.byStatus = 1;
	send.dwWarType = this->type;

	// 设置状态
	rwlock.wrlock();
	this->state = CDare::DARE_ACTIVE;
	rwlock.unlock();

	send.dwToRelationID = this->secondID;
	send.isAtt = true;

	for (DWORD i=0; i<attList.size(); i++)
	{
		this->sendCmdToAllDarePlayer(&send, sizeof(send), attList[i]);
	}

	send.isAtt = false;

	for (DWORD i=0; i<attList.size(); i++)
	{
		send.dwToRelationID = attList[i];
		this->sendCmdToAllDarePlayer(&send, sizeof(send), this->secondID);
	}
	
	DWORD map_id = (this->secondID << 16) + KING_CITY_ID;
	setCountryCmd.dwCountryID = this->secondID;
	//setCountryCmd.dwAttCountryID = attList[0];
	setCountryCmd.dwAreaID = KING_CITY_ID;
	

	SceneSession *pScene = SceneSessionManager::getInstance()->getSceneByID(map_id);
	if (pScene)
	{
		pScene->sendCmd(&setCountryCmd, sizeof(setCountryCmd));
	}
	
	this->printState();
}

void CDareCountry::setReadyOverState()
{
	Cmd::Session::t_enterWar_SceneSession exit_war; // 通知场景，退出对战状态
	Cmd::Session::t_setCountryWar_SceneSession setCountryCmd;
	setCountryCmd.byStatus = 0;

	exit_war.dwWarType = this->type;
	exit_war.dwStatus = 0;
	exit_war.dwToRelationID = this->secondID;

	for (DWORD i=0; i<attList.size(); i++)
	{
		this->sendCmdToAllDarePlayer(&exit_war, sizeof(exit_war), attList[i]);
	}
	
	for (DWORD i=0; i<attList.size(); i++)
	{
		exit_war.dwToRelationID = attList[i];
		this->sendCmdToAllDarePlayer(&exit_war, sizeof(exit_war), this->secondID);
	}

	CCountry* pDefCountry = CCountryM::getMe().find(this->secondID);
	CCountry* pAttCountry = NULL;
		
	if (grade1==0 && grade2==0)
	{//relationID2防守成功，挑战国国战失败次数加一，星号减二,防守国战胜次数加一,星号加一
		//平手处理
		pDefCountry->dwFormalWin++;
		pDefCountry->changeStar(1);
	
		for (DWORD i=0; i<attList.size(); i++)	
		{
			CDareRecordM::getMe().addNewDareRecord(attList[i], this->secondID, 1);
			pAttCountry = CCountryM::getMe().find(attList[i]);
			if (pAttCountry==NULL || pDefCountry==NULL)
			{
				continue;
			}

			pAttCountry->dwFormalFail++;
			pAttCountry->changeStar(-2);
			pAttCountry->swapMaterialByPer(pDefCountry, 3);

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, attList[i], "我国 攻击 %s ，国战失败",
								pDefCountry->name);

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, this->secondID, 
					"我国 在 %s 的攻击中，防守成功，国战胜利",
					pAttCountry->name);

			SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, 
					"%s 国王 %s 向 %s 国王 %s 俯首称臣",
					pAttCountry->name, pAttCountry->kingName, pDefCountry->name, pDefCountry->kingName);

/*			SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, 
					"%s 国家物资被 %s 掠夺，%s 3\%的银子与物资被运到 %s。\n %s 在围攻 %s 的战斗未取得成功，%s 的侵略以失败告终。\n %s 在 %s 的围攻下，保住了王城， %s 胜利。",
					pAttCountry->name, pDefCountry->name, pAttCountry->name, pDefCountry->name,
					pAttCountry->name, pDefCountry->name, pAttCountry->name, 
					pDefCountry->name, pAttCountry->name, pDefCountry->name);
					// */

	/*		SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, 
					"%s 在围攻 %s 的战斗未取得成功，%s 的侵略以失败告终。", 
					pAttCountry->name, 
					pDefCountry->name,
					pAttCountry->name);

			SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, "%s 在 %s 的围攻下，保住了王城， %s 胜利。", 
					pDefCountry->name, 
					pAttCountry->name,
					pDefCountry->name);*/

			CSeptM::getMe().changeAllRepute(attList[i], -10);
		}
	}
	else if (grade1==2)
	{//relationID1挑战成功，挑战国国战胜利次数加一，星号加二,防守国国战失败次数加一，星号减二
		//挑战方胜利
		
		pDefCountry->dwFormalFail++;
		pDefCountry->changeStar(-2);
		
		CSeptM::getMe().changeAllRepute(pDefCountry->dwID, -10);

		for (DWORD i=0; i<attList.size(); i++)
		{
			CDareRecordM::getMe().addNewDareRecord(attList[i], this->secondID, 0);
			pAttCountry = CCountryM::getMe().find(attList[i]);
			//战胜方经验加成标志
			pAttCountry->setWinnerExp();

			if (pAttCountry==NULL || pDefCountry==NULL)
			{
				continue;
			}		    

			pAttCountry->dwFormalWin++;
			pAttCountry->changeStar(2);

			pDefCountry->swapMaterialByPer(pAttCountry, 3);

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, attList[i], "我国 攻击 %s 成功，国战胜利.本国境内击杀怪物经验上升百分之五十",
					pDefCountry->name);

			pDefCountry->generalLevelDown();
			pDefCountry->gen_refreshTime = SessionTimeTick::currentTime.sec()+5;
			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, this->secondID, 
					"我国 在 %s 的攻击中，王城失陷，国战失败.可在王城大臣处申请反攻.\n",
					pAttCountry->name, pDefCountry->gen_level);
			
			SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, 
					"%s 国王 %s 向 %s 国王 %s 俯首称臣",
					pDefCountry->name, pDefCountry->kingName, pAttCountry->name, pAttCountry->kingName);

/*			SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, 
				"%s 国家物资被 %s 掠夺，%s 3\%的银子与物资被运到 %s。\n %s 在围攻 %s 的战斗中攻破对方王城, %s 取得国战的胜利。\n %s 在 %s 的围攻下，王城陷落， %s 战败。",
					pDefCountry->name, pAttCountry->name, pDefCountry->name, pAttCountry->name,
					pAttCountry->name, pDefCountry->name, pAttCountry->name,
					pDefCountry->name, pAttCountry->name, pDefCountry->name
					);*/

			CSeptM::getMe().changeAllRepute(pAttCountry->dwID, 6);
		}
	}
	else if (grade2==2)
	{//relationID2反攻成功，挑战国失败次数加一，星号减二，防守国国战胜利次数加一，星号加二
		//防守方胜利
		//战胜方经验加成标志
		
		pDefCountry->setWinnerExp();
		pDefCountry->dwFormalWin++;
		pDefCountry->changeStar(2);
		CSeptM::getMe().changeAllRepute(pDefCountry->dwID, 6);

		for (DWORD i=0; i<attList.size(); i++)
		{	
			CDareRecordM::getMe().addNewDareRecord(attList[i], this->secondID, 2);
			pAttCountry = CCountryM::getMe().find(attList[i]);

			if (pAttCountry==NULL || pDefCountry==NULL)
			{
				continue;
			}

			pAttCountry->dwFormalFail++;
			pAttCountry->changeStar(-2);
			pAttCountry->swapMaterialByPer(pDefCountry, 3);

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, attList[i], 
					"我国 被 %s 偷袭成功，国战失败",
					pDefCountry->name);

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, this->secondID, 
					"我国 在对 %s 反攻中，占领对方王城，国战胜利",
					pAttCountry->name);

			SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, 
				"%s 在围攻 %s 的战斗中遭到对方偷袭, %s 王城遭到破坏, %s 战败。\n %s 在 %s 的围攻下，偷袭 %s 王城成功， %s 胜利。\n %s 国家物资被 %s 掠夺，%s 3\%的银子与物资被运到 %s", 
					pAttCountry->name, 
					pDefCountry->name,
					pAttCountry->name,
					pAttCountry->name,
					pDefCountry->name, pAttCountry->name, pAttCountry->name, pDefCountry->name,
					pAttCountry->name, pDefCountry->name, pAttCountry->name, pDefCountry->name
					);

			/*SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, "%s 在 %s 的围攻下，偷袭 %s 王城成功， %s 胜利。", 
					pDefCountry->name, 
					pAttCountry->name,
					pAttCountry->name,
					pDefCountry->name);

			SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, "%s 国家物资被 %s 掠夺，%s 3\%的银子与物资被运到 %s",
					pAttCountry->name, pDefCountry->name, pAttCountry->name, pDefCountry->name);
					*/

			CSeptM::getMe().changeAllRepute(pAttCountry->dwID, -10);
		}
	}

	if (attList.size()>0)
	{
/*		DWORD map_id = (attList[0] << 16) + KING_CITY_ID;
		setCountryCmd.dwCountryID = this->firstID;
		setCountryCmd.dwAreaID = KING_CITY_ID;

		SceneSession *pScene = SceneSessionManager::getInstance()->getSceneByID(map_id);
		if (pScene)
		{
			pScene->sendCmd(&setCountryCmd, sizeof(setCountryCmd));
		}*/
	}
	
	DWORD map_id = (this->secondID << 16) + KING_CITY_ID;
	setCountryCmd.dwCountryID = this->secondID;
	setCountryCmd.dwAreaID = KING_CITY_ID;

	SceneSession *pScene = SceneSessionManager::getInstance()->getSceneByID(map_id);
	if (pScene)
	{
		pScene->sendCmd(&setCountryCmd, sizeof(setCountryCmd));
	}

	if (pDefCountry) pDefCountry->endDare();
	if (pAttCountry) pAttCountry->writeDatabase();
	if (pDefCountry )pDefCountry->writeDatabase();
	this->setOverState();
}

void CDareCountry::setSecondID(DWORD dwID)
{
	this->secondID = dwID;
}

void CDareCountry::addFirstID(DWORD dwID)
{
	attList.push_back(dwID);
}

void CDareCountry::sendCmdToAllDarePlayer(Cmd::Session::t_enterWar_SceneSession* cmd, 
		unsigned int cmdLen, unsigned int relationID)
{
	//OneCountryScene ocs;
//	ocs.init(cmd, cmdLen, relationID);
//	UserSessionManager::getInstance()->execEveryUser(ocs);
	cmd->dwFromRelationID = relationID;
	SessionTaskManager::getInstance().broadcastScene(cmd, cmdLen);

	Cmd::stSetCountryDareUserCmd send;
	send.byStatus = cmd->dwStatus;
	if (send.byStatus == 0)
	{//退出国战时，判断胜负
		if (grade1==grade2 || grade2>grade1)	
		{//攻方失败
			if (this->isAtt(relationID))
			{
				send.byResult = 0;
			}
			else
			{
				send.byResult = 1;
			}
		}
		else
		{
			if (this->isAtt(relationID))
			{
				send.byResult = 1;
			}
			else
			{
				send.byResult = 0;
			}
		}
	}
		
	send.dwCountry = cmd->dwToRelationID;
	SessionTaskManager::getInstance().sendCmdToCountry(relationID, &send, sizeof(send));
}

void CDareCountry::sendActiveStateToScene(UserSession* pUser)
{
	Cmd::Session::t_enterWar_SceneSession send;
	send.dwWarType = type;
	send.dwFromRelationID = pUser->country;

	Cmd::stSetCountryDareUserCmd enter_send;
	enter_send.byStatus = 1;

	if (secondID != pUser->country)
	{
		enter_send.dwCountry = secondID;
		send.dwToRelationID = secondID;

		send.isAtt = true;
		send.dwUserID = pUser->id;
		send.dwSceneTempID = pUser->scene->tempid;
		pUser->scene->sendCmd(&send, sizeof(send));

		pUser->sendCmdToMe(&enter_send, sizeof(enter_send));
	}
	else
	{
		for (DWORD i=0; i<attList.size(); i++)
		{
			send.dwToRelationID = attList[i];
			send.isAtt = false;
			send.isAntiAtt = this->isAntiAtt;
			send.dwUserID = pUser->id;
			send.dwSceneTempID = pUser->scene->tempid;
			pUser->scene->sendCmd(&send, sizeof(send));

			enter_send.dwCountry = attList[i];
			pUser->sendCmdToMe(&enter_send, sizeof(enter_send));
		}
	}
}

void CDareCountry::timer()
{
}

bool CDareCountry::isInvalid()
{
	return false;
}

void CDareCountry::sendAntiAtt()
{
	Cmd::Session::t_setAntiAttFlag_SceneSession send_anti; // 通知场景该次国战进入反攻模式状态

	send_anti.dwType = this->type;
	send_anti.dwFromRelationID = this->secondID;

	for (DWORD i=0; i<attList.size(); i++)
	{
		send_anti.dwToRelationID = attList[i];
		SessionTaskManager::getInstance().broadcastScene(&send_anti, sizeof(send_anti));
	}
}

//---------------------------CDareAntiCountry----------------------------------------------------------
CDareAntiCountry::CDareAntiCountry() : CDareCountry(0,0)
{
}

CDareAntiCountry::CDareAntiCountry(DWORD active_time, DWORD ready_time) : CDareCountry(active_time, ready_time)
{
}

CDareAntiCountry::~CDareAntiCountry()
{
}

void CDareAntiCountry::setReadyOverState()
{
	Cmd::Session::t_enterWar_SceneSession exit_war; // 通知场景，退出对战状态
	Cmd::Session::t_setCountryWar_SceneSession setCountryCmd;
	setCountryCmd.byStatus = 0;

	exit_war.dwWarType = this->type;
	exit_war.dwStatus = 0;
	exit_war.dwToRelationID = this->secondID;

	for (DWORD i=0; i<attList.size(); i++)
	{
		this->sendCmdToAllDarePlayer(&exit_war, sizeof(exit_war), attList[i]);
	}
	
	for (DWORD i=0; i<attList.size(); i++)
	{
		exit_war.dwToRelationID = attList[i];
		this->sendCmdToAllDarePlayer(&exit_war, sizeof(exit_war), this->secondID);
	}
	
	CCountry* pDefCountry = CCountryM::getMe().find(this->secondID);
	CCountry* pAttCountry = NULL;
		
	if (grade1==0 && grade2==0)
	{//relationID2防守成功，挑战国国战失败次数加一，星号减二,防守国战胜次数加一,星号加一
		//平手处理
	
		pDefCountry->dwFormalWin++;
		pDefCountry->changeStar(1);

		for (DWORD i=0; i<attList.size(); i++)	
		{
			pAttCountry = CCountryM::getMe().find(attList[i]);
			if (pAttCountry==NULL || pDefCountry==NULL)
			{
				continue;
			}

			pAttCountry->dwFormalFail++;
			pAttCountry->changeStar(-2);
			pAttCountry->swapMaterialByPer(pDefCountry, 3);

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, attList[i], "我国 攻击 %s ，国战失败",
								pDefCountry->name);

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, this->secondID, 
					"我国 在 %s 的攻击中，防守成功，国战胜利",
					pAttCountry->name);

			SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, 
					"%s 国家物资被 %s 掠夺，%s 3\%的银子与物资被运到 %s。\n %s 在围攻 %s 的战斗未取得成功，%s 的侵略以失败告终。\n %s 在 %s 的围攻下，保住了王城， %s 胜利。",
					pAttCountry->name, pDefCountry->name, pAttCountry->name, pDefCountry->name,
					pAttCountry->name, pDefCountry->name, pAttCountry->name, 
					pDefCountry->name, pAttCountry->name, pDefCountry->name);

			CSeptM::getMe().changeAllRepute(attList[i], -10);
		}
	}
	else if (grade1==2)
	{//relationID1挑战成功，挑战国国战胜利次数加一，星号加二,防守国国战失败次数加一，星号减二
		//挑战方胜利
		
		pDefCountry->dwFormalFail++;
		pDefCountry->changeStar(-2);	
		CSeptM::getMe().changeAllRepute(pDefCountry->dwID, -10);

		for (DWORD i=0; i<attList.size(); i++)
		{
			pAttCountry = CCountryM::getMe().find(attList[i]);
			//战胜方经验加成标志
			//pAttCountry->setWinnerExp();

			if (pAttCountry==NULL || pDefCountry==NULL)
			{
				continue;
			}		    

			pAttCountry->dwFormalWin++;
			pAttCountry->changeStar(2);

			pDefCountry->swapMaterialByPer(pAttCountry, 3);

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, attList[i], "我国 攻击 %s 成功，国战胜利.",
					pDefCountry->name);

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, this->secondID, 
					"我国 在 %s 的攻击中，王城失陷，国战失败.",
					pAttCountry->name);
			

			SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, 
				"%s 国家物资被 %s 掠夺，%s 3\%的银子与物资被运到 %s。\n %s 在围攻 %s 的战斗中攻破对方王城, %s 取得国战的胜利。\n %s 在 %s 的围攻下，王城陷落， %s 战败。",
					pDefCountry->name, pAttCountry->name, pDefCountry->name, pAttCountry->name,
					pAttCountry->name, pDefCountry->name, pAttCountry->name,
					pDefCountry->name, pAttCountry->name, pDefCountry->name
					);

			CSeptM::getMe().changeAllRepute(pAttCountry->dwID, 6);
		}
	}
	else if (grade2==2)
	{//relationID2反攻成功，挑战国失败次数加一，星号减二，防守国国战胜利次数加一，星号加二
		//防守方胜利
		//战胜方经验加成标志
		//pDefCountry->setWinnerExp();
		
		pDefCountry->dwFormalWin++;
		pDefCountry->changeStar(2);
		CSeptM::getMe().changeAllRepute(pDefCountry->dwID, 6);

		for (DWORD i=0; i<attList.size(); i++)
		{	
			pAttCountry = CCountryM::getMe().find(attList[i]);

			if (pAttCountry==NULL || pDefCountry==NULL)
			{
				continue;
			}

			pAttCountry->dwFormalFail++;
			pAttCountry->changeStar(-2);
			pAttCountry->swapMaterialByPer(pDefCountry, 3);

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, attList[i], 
					"我国 被 %s 偷袭成功，国战失败",
					pDefCountry->name);

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, this->secondID, 
					"我国 在对 %s 反攻中，占领对方王城，国战胜利",
					pAttCountry->name);

			SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, 
				"%s 在围攻 %s 的战斗中遭到对方偷袭, %s 王城遭到破坏, %s 战败。\n %s 在 %s 的围攻下，偷袭 %s 王城成功， %s 胜利。\n %s 国家物资被 %s 掠夺，%s 3\%的银子与物资被运到 %s", 
					pAttCountry->name, 
					pDefCountry->name,
					pAttCountry->name,
					pAttCountry->name,
					pDefCountry->name, pAttCountry->name, pAttCountry->name, pDefCountry->name,
					pAttCountry->name, pDefCountry->name, pAttCountry->name, pDefCountry->name
					);

			CSeptM::getMe().changeAllRepute(pAttCountry->dwID, -10);
		}
	}

	if (attList.size()>0)
	{
/*		DWORD map_id = (attList[0] << 16) + KING_CITY_ID;
		setCountryCmd.dwCountryID = this->firstID;
		setCountryCmd.dwAreaID = KING_CITY_ID;

		SceneSession *pScene = SceneSessionManager::getInstance()->getSceneByID(map_id);
		if (pScene)
		{
			pScene->sendCmd(&setCountryCmd, sizeof(setCountryCmd));
		}*/
	}
	
	DWORD map_id = (this->secondID << 16) + KING_CITY_ID;
	setCountryCmd.dwCountryID = this->secondID;
	setCountryCmd.dwAreaID = KING_CITY_ID;

	SceneSession *pScene = SceneSessionManager::getInstance()->getSceneByID(map_id);
	if (pScene)
	{
		pScene->sendCmd(&setCountryCmd, sizeof(setCountryCmd));
	}

	//if (pDefCountry) pDefCountry->endDare();
	if (pAttCountry) pAttCountry->writeDatabase();
	if (pDefCountry )pDefCountry->writeDatabase();
	this->setOverState();
}

