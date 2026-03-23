/**
 * \file
 * \version  $Id: CDareSept.cpp  $
 * \author  
 * \date 
 * \brief 家族随意战
 *
 * 
 */

#include "CDareSept.h"
#include "CSept.h"
#include "Command.h"
#include "zRWLock.h"
#include "SessionServer.h"
#include "SessionTaskManager.h"
#include "Session.h"
#include "SessionChat.h"


CDareSept::CDareSept() : CDare(0, 0)
{
	//pFirst = NULL;
	//pSecond = NULL;
	dwDareRepute = 0;
}

CDareSept::CDareSept(DWORD active_time, DWORD ready_time) : CDare(active_time, ready_time)
{
	dwDareRepute = 0;
}

CDareSept::~CDareSept()
{
}

void CDareSept::setSecondID(DWORD dwID)
{
	this->secondID = dwID;
	//this->pSecond = (CSept*)CSeptM::getMe().getSeptByID(dwID);
}

void CDareSept::addFirstID(DWORD dwID)
{
	attList.push_back(dwID);
}

void CDareSept::addGrade(UserSession* pAtt, UserSession* pDef)
{
	if (pAtt==NULL || pDef == NULL)
		return;

	if (pAtt->septid == secondID)
	{
		pk2 = pk2 + 1;
		grade2 = pDef->level/10 + grade2;
	}
	else
	{
		for (DWORD i=0; i<attList.size(); i++)
		{
			if (pAtt->septid == attList[i])
			{
				pk1 = pk1 + 1; //累加PK人数
				grade1 = pDef->level/10 + grade1;
			}
		}
	}
}

void CDareSept::sendCmdToAllDarePlayer(Cmd::Session::t_enterWar_SceneSession* cmd, unsigned int cmdLen, unsigned int relationID)
{
	if (cmd->isAtt)
	{// 如果是给攻击方发送消息
		CSept *pAttSept = (CSept*)CSeptM::getMe().getSeptByID(relationID);
		if (pAttSept) pAttSept->sendCmdToAllMemberScene(cmd, cmdLen);
	}
	else
	{
		CSept *pDefSept = (CSept*)CSeptM::getMe().getSeptByID(relationID);
		if (pDefSept) pDefSept->sendCmdToAllMemberScene(cmd, cmdLen);
	}
}

void CDareSept::sendActiveStateToScene(UserSession* pUser)
{
	Cmd::Session::t_enterWar_SceneSession send;
	send.dwWarType = type;

	if (secondID != pUser->septid)
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

void CDareSept::notifyWarResult(int winner_type)
{
	CSept *pAttSept = NULL;
	CSept *pSecond = (CSept*)CSeptM::getMe().getSeptByID(this->secondID);

	if (winner_type == 0)
	{
		if (pSecond)
		{
			for (DWORD i=0; i<attList.size(); i++)
			{
				pAttSept = (CSept*)CSeptM::getMe().getSeptByID(attList[i]);	   
				if (pAttSept)
				{
					pAttSept->sendSeptNotify("\n家族对战结束， 对战结果 %s 获胜\n %s PK人数:%d 得分:%d\n %s PK人数:%d 得分:%d", 
							pAttSept->name, pAttSept->name,
							this->pk1, this->grade1,
							pSecond->name,
							this->pk2, this->grade2);

					pSecond->sendSeptNotify("\n家族对战结束， 对战结果 %s 失败\n %s PK人数:%d 得分:%d\n %s PK人数:%d 得分:%d", 
							pSecond->name, pAttSept->name,
							this->pk1, this->grade1,
							pSecond->name,
							this->pk2, this->grade2);

					SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pAttSept->dwCountryID, 
							"%s 家族今日被 %s 家族打的落花流水抱头鼠窜，努力奋斗啊！",
							pSecond->name, pAttSept->name);
				}
			}
		}
	}
	else if (winner_type == 1)
	{
		if (pSecond)
		{
			for (DWORD i=0; i<attList.size(); i++)
			{
				pAttSept = (CSept*)CSeptM::getMe().getSeptByID(attList[i]);
				if (pAttSept)
				{
					pAttSept->sendSeptNotify("\n家族对战结束， 对战结果 %s 失败\n %s PK人数:%d 得分:%d\n %s PK人数:%d 得分:%d", 
							pAttSept->name, pAttSept->name,
							this->pk1, this->grade1,
							pSecond->name,
							this->pk2, this->grade2);

					pSecond->sendSeptNotify("\n家族对战结束， 对战结果 %s 获胜\n %s PK人数:%d 得分:%d\n %s PK人数:%d 得分:%d", 
							pSecond->name, pAttSept->name,
							this->pk1, this->grade1,
							pSecond->name,
							this->pk2, this->grade2);
					
					SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pAttSept->dwCountryID, 
							"%s 家族今日被 %s 家族打的落花流水抱头鼠窜，努力奋斗啊！",
							pAttSept->name, pSecond->name);

				}
			}
		}
	}
	else
	{
		if (pSecond)
		{
			for (DWORD i=0; i<attList.size(); i++)
			{
				pAttSept = (CSept*)CSeptM::getMe().getSeptByID(attList[i]);
				if (pAttSept)
				{
					pAttSept->sendSeptNotify("\n家族对战结束， 对战结果 %s 战平\n %s PK人数:%d 得分:%d\n %s PK人数:%d 得分:%d", 
							pAttSept->name, pAttSept->name,
							this->pk1, this->grade1,
							pSecond->name,
							this->pk2, this->grade2);

					pSecond->sendSeptNotify("\n家族对战结束， 对战结果 %s 战平\n %s PK人数:%d 得分:%d\n %s PK人数:%d 得分:%d", 
							pSecond->name, pAttSept->name,
							this->pk1, this->grade1,
							pSecond->name,
							this->pk2, this->grade2);
				}
			}
		}
	}
}


char* CDareSept::getFirstName()
{
	if (attList.size()>0)
	{
		CSept *pAttSept = (CSept*)CSeptM::getMe().getSeptByID(attList[0]);
		if (pAttSept)
		{
			return pAttSept->name;
		}
	}
	
	return NULL;	
}

DWORD CDareSept::getSecondUserID()
{
	CSept *pSecond = (CSept*)CSeptM::getMe().getSeptByID(this->secondID);
	if (pSecond && pSecond->master && pSecond->master->byStatus)
	{
		return pSecond->master->id;
	}
	
	return 0;
}

bool CDareSept::isInvalid()
{
	return false;
}

void CDareSept::timer()
{
}

void CDareSept::setReadyQuestionState()
{
	DWORD toUserID = 0;
	char  fromRelationName[MAX_NAMESIZE+1];
	bzero(fromRelationName, sizeof(fromRelationName));

	strncpy(fromRelationName, this->getFirstName(), MAX_NAMESIZE);
	toUserID = getSecondUserID();

	if (toUserID != 0)
	{
		UserSession* pDareUser = UserSessionManager::getInstance()->getUserByID(this->userid1);
		rwlock.wrlock();
		userid2 = toUserID;               // 保存应战者ID
		rwlock.unlock();
		UserSession* pUser = UserSessionManager::getInstance()->getUserByID(this->userid2);

		if (pUser && pDareUser)
		{//找到有效应战者,向其发送挑战询问命令
			Cmd::stActiveDareCmd send;

			send.dwMsgType = Cmd::DARE_QUESTION;
			send.dwWarID   = this->tempid;
			send.dwDareType = this->type;
			send.byDareRepute = this->dwDareRepute;
		
			strncpy(send.name, pDareUser->name, MAX_NAMESIZE);
			strncpy(send.fromRelationName, fromRelationName, MAX_NAMESIZE);
			pUser->sendCmdToMe(&send, sizeof(send));
		}
		else
		{//应战者
#ifdef _ZJW_DEBUG		    
			Zebra::logger->debug("应战者未在线");
#endif		    
		}
	}
	else
	{
#ifdef _ZJW_DEBUG	    
		Zebra::logger->debug("未找到有效的应战者");
#endif	    
	}

	rwlock.wrlock();
	state = CDare::DARE_READY_QUESTION;
	rwlock.unlock();
	this->printState();
}

void CDareSept::setReadyActiveState(UserSession* pUser)
{
	rwlock.wrlock();
	this->count = 0;
	this->state = CDare::DARE_READY_ACTIVE;
	rwlock.unlock();
	this->printState();
	int vDareRepute[] = {0, 1, 5, 10, 20};

	CSept* pSept = (CSept*)CSeptM::getMe().getSeptByID(pUser->septid);
	if (pUser && pSept)
	{
		if ((int)pSept->getRepute() < vDareRepute[this->dwDareRepute])
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "声望不足，不能接受挑战");
			this->setReturnGoldState();
			return;
		}

		pSept->changeRepute(-vDareRepute[this->dwDareRepute]);
		this->setActiveState();
	}
	else
	{
		// TODO:如果找不到应战者
		setReturnGoldState();
	}
}

void CDareSept::setReturnGoldState()
{
	UserSession* pUser = UserSessionManager::getInstance()->getUserByID(this->userid1);
	CSept* pSept = NULL;
	if (pUser)
	{
		pSept = (CSept*)CSeptM::getMe().getSeptByID(pUser->septid);
	}
	
	int vDareRepute[] = {0, 1, 5, 10, 20};
	
	if (pUser && pSept)
	{
		pSept->changeRepute(vDareRepute[this->dwDareRepute]);
		rwlock.wrlock();
		this->state = CDare::DARE_OVER;
		rwlock.unlock();
	}
	else
	{
		rwlock.wrlock();
		this->state = CDare::DARE_RETURN_GOLD;
		rwlock.unlock();
	}

	this->printState();
}

void CDareSept::setReadyOverState()
{
	Cmd::Session::t_enterWar_SceneSession exit_war; // 通知场景，退出对战状态
	int vDareRepute[] = {0, 1, 5, 10, 20};

	UserSession* pUser1 = NULL;
	UserSession* pUser2 = NULL;

	int winner_type = 0;  // 为0，挑战者胜，为1，应战者胜，2，战平

	rwlock.wrlock();
	this->state = CDare::DARE_READY_OVER;
	rwlock.unlock();

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
		if (pUser1)
		{
			CSept* pSept = (CSept*)CSeptM::getMe().getSeptByID(pUser1->septid);
			if (pSept)
			{
				pSept->changeRepute(vDareRepute[this->dwDareRepute]*2);
			}
		}

		winner_type = 0;
	}
	else if (grade1<grade2)
	{
		if (pUser2)
		{
			CSept* pSept = (CSept*)CSeptM::getMe().getSeptByID(pUser2->septid);
			if (pSept)
			{
				pSept->changeRepute(vDareRepute[this->dwDareRepute]*2);
			}
		}

		winner_type = 1;
	}
	else if (grade1 == grade2)
	{
		if (pUser1)
		{
			CSept* pSept = (CSept*)CSeptM::getMe().getSeptByID(pUser1->septid);
			if (pSept)
			{
				pSept->changeRepute(vDareRepute[this->dwDareRepute]);
			}
		}

		if (pUser2)
		{
			CSept* pSept = (CSept*)CSeptM::getMe().getSeptByID(pUser2->septid);
			if (pSept)
			{
				pSept->changeRepute(vDareRepute[this->dwDareRepute]);
			}
		}

		winner_type = 2;
	}

	// 通告对战结果
	this->notifyWarResult(winner_type);
	this->setOverState();
}



