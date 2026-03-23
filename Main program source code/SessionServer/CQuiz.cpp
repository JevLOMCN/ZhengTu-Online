/**
 * \file
 * \version  $Id: CQuiz.cpp $
 * \author  
 * \date 
 * \brief 实现竞赛管理类
 *
 */

#include "zRWLock.h"
#include "SchoolManager.h"
#include "CUnion.h"
#include "CSept.h"
#include "CQuiz.h"
#include "Command.h"
#include "zDBConnPool.h"
#include "SessionServer.h"
#include "Session.h"
#include "CRelationManager.h"
#include "zMetaData.h"
#include "zTime.h"
#include <vector>
#include <set>
#include <math.h>
#include "Quiz.h"
#include "SessionTaskManager.h"
using namespace QuizDef;
class PrintSet
{
	public:
		PrintSet(){}

		void operator() (int subid)
		{
			Zebra::logger->debug("题目跟踪[全国竞赛]:本轮已答过的题目:%d", subid);
		}
};

CQuiz::CQuiz(DWORD active_time, DWORD ready_time)
{
	count = 0;
	ready_question_time = 10;
	cur_subject = 0;
	
	if (active_time == 0)
	{
		this->active_time = ::quiz_active_time;
	}
	else 
	{
		this->active_time = active_time;
	}

	if (ready_time == 0)
	{
		this->ready_time = ::quiz_ready_time;
	}
	else 
	{
		this->ready_time = ready_time;
	}

	subject_type = 0;
	answered.clear();
}

CQuiz::CQuiz()
{
	this->active_time = ::quiz_active_time;
	this->ready_time = ::quiz_ready_time;

	count = 0;
	ready_question_time = 10;
	cur_subject = 0;
	subject_type = 0;	

}

CQuiz::~CQuiz()
{
}

void CQuiz::timer()
{
}


/**
 * \brief 进入答题准备状态
 *
 *      全区智力竞赛和个人问答，皆可共用此处理，该处理从数据库中随机抽取一组题目。
 *
 * \param 
 * \return 
 */
void CQuiz::setReadyState()
{
	int db_answer_count = 0; //atoi(Zebra::global["answer_count"].c_str());
	
	switch(this->subject_type)
	{
		case 0:
			db_answer_count = CSubjectM::getMe().gsubjects.size() - 1;
			break;
		case 1:
			db_answer_count = CSubjectM::getMe().personals.size() - 1;
			break;
		case 2:
			db_answer_count = CSubjectM::getMe().levels.size() - 1;
			break;
		default:
			db_answer_count = 0;
	}
	
	if (db_answer_count<=0)
	{
		Zebra::logger->error("题库记录为零，无法进行答题");
		this->setReadyOverState();
		return;
	}
		
	std::set<int> temp_set;
	
	rwlock.wrlock();
	DWORD i=0;
	while (i<(total_subject+5))
	{
		int subject_id = zMisc::randBetween(0, db_answer_count);
				
		while (temp_set.find(subject_id) != temp_set.end())
		{
			subject_id = zMisc::randBetween(0, db_answer_count);
		}

		CSubject sub;
		
		switch (this->subject_type)
		{
			case 0:
				sub = CSubjectM::getMe().gsubjects[subject_id];
				break;
			case 1:
				sub = CSubjectM::getMe().personals[subject_id];
				break;
			case 2:
				sub = CSubjectM::getMe().levels[subject_id];
				break;
			default:
				break;
		}
		
		if (strlen(sub.title)>0 && sub.quiz_type == this->subject_type)
		{
			temp_set.insert(subject_id);
			subjects.push_back(sub);
			i++;
		}

#ifdef _ALL_SUPER_GM		
//		Zebra::logger->debug("[题库跟踪[选题]]:(%d,%d),title:%s", i, 
//		subject_id, CSubjectM::getMe().gsubjects[subject_id].title );
#endif		
	}

	this->state = CQuiz::QUIZ_READY;
	rwlock.unlock();
	
	this->printState();	
}

void CQuiz::setReadyOverState()
{
	rwlock.wrlock();
	this->state = CQuiz::QUIZ_READY_OVER;
	rwlock.unlock();

	this->printState();	
}

void CQuiz::setReadSortState()
{
	rwlock.wrlock();
	this->state = CQuiz::QUIZ_READ_SORT;
	this->count=0;
	rwlock.unlock();
	this->sendExitToAll();

	this->printState();	
}

void CQuiz::setReturnGoldState()
{
	rwlock.wrlock();
	this->state = CQuiz::QUIZ_RETURN_GOLD;
	rwlock.unlock();

	this->printState();	
}

void CQuiz::setOverState()
{
	rwlock.wrlock();
	this->state = CQuiz::QUIZ_OVER;
	rwlock.unlock();

	this->printState();	
}

void CQuiz::setActiveQuestionState()
{
	rwlock.wrlock();
	this->state = CQuiz::QUIZ_ACTIVE_QUESTION;
	rwlock.unlock();

	this->printState();	
}

void CQuiz::setReadyQuestionState()
{
	rwlock.wrlock();
	Cmd::stQueryQuiz send;

	if (this->subject_type == 2)
	{
		struct QuizCompare : public UserSessionManager::Compare
		{
			bool operator()(UserSession* pUser)
			{
				if (pUser->level<=20)
				{
					return true;
				}
				
				return false;
			}
		};

		QuizCompare compare;
		send.byQuizType = 2;
		UserSessionManager::getInstance()->sendCmdByCondition(&send, sizeof(send), &compare);
	}
	else
	{
		SessionTaskManager::getInstance().sendCmdToWorld(&send,sizeof(send));
	}

	state = CQuiz::QUIZ_READY_QUESTION;
	count = 0;
	rwlock.unlock();

	this->printState();	
}

void CQuiz::setSendQuestionState()
{
	rwlock.wrlock();
	state = CQuiz::QUIZ_SEND_QUESTION;
	rwlock.unlock();
	
	this->printState();
}

void CQuiz::setEndQuestionState()
{
	rwlock.wrlock();
	state = CQuiz::QUIZ_END_QUESTION;
	cur_subject++;
	//cur_pos++;
	rwlock.unlock();
}

void CQuiz::printState()
{
	//Zebra::logger->trace("竞赛跟踪:%d(%s)", this->tempid, str_state[this->state]);
}


//-------------------------------------------------------------------------------------
CQuizWorld::CQuizWorld() : CQuiz(0, 0)
{
	this->type = QuizDef::WORLD_QUIZ;
	this->total_subject = 59;
	this->subject_type = 0;
}

CQuizWorld::CQuizWorld(DWORD active_time, DWORD ready_time, BYTE subject_type) : CQuiz(active_time, ready_time)
{
	this->type = QuizDef::WORLD_QUIZ;
	this->total_subject = 59;
	this->subject_type = subject_type;
}

CQuizWorld::~CQuizWorld()
{
	pothunters.clear();
}

void CQuizWorld::setSendQuestionState()
{
	char buf[zSocket::MAX_DATASIZE];
	
	CQuiz::setSendQuestionState();

	if (cur_subject>total_subject)
	{
		this->setReadSortState();
		return;
	}

	rwlock.wrlock();
	CSubject temp_subject = subjects[cur_subject];
	
	Cmd::stQuestionQuiz *send = (Cmd::stQuestionQuiz*)buf;
	constructInPlace(send);
	
	send->dwID = cur_subject + 1;
	send->bySpareTime = 25;
#ifdef _ALL_SUPER_GM
	Zebra::logger->debug("----题库跟踪[发题]:subject:%d title:%s answer:%d(全国)", cur_subject+1, 
			temp_subject.title, temp_subject.answer);
#endif	
		
	char* pos = send->subject;
	sprintf(pos, "%s", temp_subject.title);
	pos = pos + strlen(temp_subject.title);
	*pos = 0x00;
	pos++;
		
	sprintf(pos, "%s", temp_subject.answer_a);
	pos = pos + strlen(temp_subject.answer_a);
	*pos = 0x00;
	pos++;
	
	sprintf(pos, "%s", temp_subject.answer_b);
	pos = pos + strlen(temp_subject.answer_b);
	*pos = 0x00;
	pos++;

	sprintf(pos, "%s", temp_subject.answer_c);
	pos = pos + strlen(temp_subject.answer_c);
	*pos = 0x00;
	pos++;

	sprintf(pos, "%s", temp_subject.answer_d);
	pos = pos + strlen(temp_subject.answer_d);
	*pos = 0x00;
	pos++;
	
	sprintf(pos, "%s", temp_subject.answer_e);
	pos = pos + strlen(temp_subject.answer_e);
	*pos = 0x00;
	pos++;
	
	sprintf(pos, "%s", temp_subject.answer_f);
	pos = pos + strlen(temp_subject.answer_f);
	*pos = 0x00;


	unsigned int cmdLen = sizeof(Cmd::stQuestionQuiz) + strlen(temp_subject.title) + strlen(temp_subject.answer_a) +
		strlen(temp_subject.answer_b) + strlen(temp_subject.answer_c) + strlen(temp_subject.answer_d) + 
		strlen(temp_subject.answer_e) + strlen(temp_subject.answer_f) +	7;

	send->dwTotal = pothunters.size();

	for (unsigned int i=0; i<pothunters.size(); i++)
	{
		UserSession* pUser = UserSessionManager::getInstance()->getUserByID(pothunters[i].dwUserID);
		
		if (pUser)
			pUser->sendCmdToMe(send, cmdLen);

	}
	question_count = 0;
	rwlock.unlock();
	CQuiz::setActiveQuestionState();
}

bool CQuizWorld::addPothunters(UserSession* pUser)
{
	CPothunter elem;
	bool ret = false;
	
	if (pUser)
	{
		rwlock.wrlock();
		
		//if (this->getState() == CQuiz::QUIZ_READY_QUESTION)
		//{
		unsigned int i=0;
		for (i=0; i<pothunters.size(); i++)
		{
			if (pothunters[i].dwUserID == pUser->id)
			{
				break;
			}
		}

		if (i>=pothunters.size())
		{
			elem.dwUserID = pUser->id;
			pothunters.push_back(elem);
			ret = true;
		}
		//}

		rwlock.unlock();
	}

	return ret;
}

/**
 * \brief 回答问题
 *
 *
 * \param pCmd 回答命令
 * \return -1：迟到的回答，0:回答正确, 1:回答错误
 */
int CQuizWorld::answer(Cmd::stAnswerQuiz* pCmd, DWORD dwUserID)
{
	int ret = -1;
	int hunterpos = -1;
	
	for (unsigned int i=0; i<pothunters.size(); i++)
	{
		if (pothunters[i].dwUserID == dwUserID)
		{
			hunterpos = i;
			break;
		}
	}

	if (this->question_count<5)
	{
		return ret;
	}
	
	rwlock.wrlock();
#ifdef	_ALL_SUPER_GM
	/*Zebra::logger->debug("----题目跟踪[答题]:(%d)subject:%d title:%s answer:%d user_answer:%d(全国)", 
			dwUserID,
			cur_subject+1, 
			CSubjectM::getMe().gsubjects[subjects[this->cur_subject]].title, 
			CSubjectM::getMe().gsubjects[subjects[this->cur_subject]].answer,
			pCmd->answerID);*/
#endif	
	
	if (pCmd->dwID == (this->cur_subject+1) && this->getState() == QUIZ_ACTIVE_QUESTION && hunterpos>=0)
	{
		answered.insert(pCmd->dwID);
		if ((this->cur_subject>=0) && (this->cur_subject<100))
		{
			CSubject temp = subjects[this->cur_subject];
			int cur_score = abs(pCmd->dwScore)>15?15:abs(pCmd->dwScore);

			if (pCmd->byLuck==1 && pothunters[hunterpos].dwLuck<3)
			{// 使用幸运星
				cur_score = cur_score * 2;
				pothunters[hunterpos].dwLuck++;
			}
			
			if (temp.answer == pCmd->answerID)
			{
				pothunters[hunterpos].dwScore = pothunters[hunterpos].dwScore + cur_score;
				pothunters[hunterpos].dwGrace++;
				ret = 0;
			}
			else
			{
				// 策划文档2006.2.4版，答错不扣分
				/*if (pothunters[hunterpos].dwScore>cur_score)
				{
					pothunters[hunterpos].dwScore = pothunters[hunterpos].dwScore - cur_score;
				}
				else
				{
					pothunters[hunterpos].dwScore = 0;
				}
				*/
		
				if ((int)pothunters[hunterpos].dwGrace>1)
				{
					pothunters[hunterpos].dwGrace--;
				}
				else
				{
					pothunters[hunterpos].dwGrace = 0;
				}
				
				ret = 1;
			}
		}
		pothunters[hunterpos].dwAnswerStatus = ret;
	}
	else
	{
		Zebra::logger->debug("题目跟踪[全国竞赛]:(%d,%d) curid:%d answerid:%d title:%s answer:%d user_answer:%d", 
			dwUserID,
			question_count,
			cur_subject+1, 
			pCmd->dwID,
			subjects[this->cur_subject].title, 
			subjects[this->cur_subject].answer,
			pCmd->answerID);

	}
	rwlock.unlock();

	return ret;
}

bool lessScore(const CPothunter& p1, const CPothunter& p2)
{
	return p1.dwScore > p2.dwScore;
}

void CQuizWorld::setEndQuestionState()
{
	CQuiz::setEndQuestionState();

	rwlock.wrlock();
	std::sort(pothunters.begin(), pothunters.end(), lessScore);
	Cmd::stQuizPosList send;
	
	for (unsigned int i=0; i<pothunters.size(); i++)
	{
		UserSession *pUser=UserSessionManager::getInstance()->getUserByID(pothunters[i].dwUserID);
			
		if (pUser && i<20)
		{
			strncpy(send.rank_list[i+1].name, pUser->name, MAX_NAMESIZE);
			strncpy((char*)send.rank_list[i+1].countryName, (char*)pUser->countryName, MAX_NAMESIZE);
			send.rank_list[i+1].score = pothunters[i].dwScore;
			send.rank_list[i+1].grace = pothunters[i].dwGrace;
			send.rank_list[i+1].rank = i+1;
			send.dwSize++;
		}
	}

//	this->valid_pothunter = 0;
	
	for (unsigned int i=0; i<pothunters.size(); i++)
	{
		UserSession *pUser=UserSessionManager::getInstance()->getUserByID(pothunters[i].dwUserID);
		
		if (pUser)
		{
//			this->valid_pothunter++;
			strncpy(send.rank_list[0].name, pUser->name, MAX_NAMESIZE);
			strncpy((char*)send.rank_list[0].countryName, (char*)pUser->countryName, MAX_NAMESIZE);
			send.rank_list[0].score = pothunters[i].dwScore;
			send.rank_list[0].grace = pothunters[i].dwGrace;
			send.rank_list[0].rank = i+1;
			if (pothunters[i].dwAnswerStatus!=-1)
			{
				Cmd::stAnswerReturnQuiz sendAnswerStatus;
				sendAnswerStatus.byAnswerStatus = pothunters[i].dwAnswerStatus;
				pUser->sendCmdToMe(&sendAnswerStatus, sizeof(sendAnswerStatus));
				pothunters[i].dwAnswerStatus = -1;
			}
			
			pUser->sendCmdToMe(&send, sizeof(send));
		}
	}
	
	rwlock.unlock();
}

void CQuizWorld::sendExitToAll()
{
	rwlock.wrlock();
	std::sort(pothunters.begin(), pothunters.end(), lessScore);
	
	for (unsigned int i=0; i<pothunters.size(); i++)
	{
		UserSession *pUser=UserSessionManager::getInstance()->getUserByID(pothunters[i].dwUserID);
		Cmd::stQuizExit send;
		
		if (pUser)
		{
			pUser->sendCmdToMe(&send, sizeof(send));
		}
	}
	
	rwlock.unlock();

}

void CQuizWorld::setReadyOverState()
{
	CQuiz::setReadyOverState();
	
	rwlock.wrlock();
	std::sort(pothunters.begin(), pothunters.end(), lessScore);
	
	for (unsigned int i=0; i<pothunters.size(); i++)
	{
		UserSession *pUser=UserSessionManager::getInstance()->getUserByID(pothunters[i].dwUserID);
		Cmd::Session::t_quizAward_SceneSession sendScene;
		
		Cmd::stQuizExit send;
		
		if (pUser)
		{
			pUser->sendCmdToMe(&send, sizeof(send));
			if (pUser->scene)
			{
				sendScene.dwUserID = pothunters[i].dwUserID;
				sendScene.dwExp = (DWORD)((0.06*pothunters[i].dwScore*(1.4*(int)pow(pUser->level, 2)+pUser->level*5)));
				sendScene.dwMoney = 0;
				
				if (i==0 || i==1 || i==2)
				{
					sendScene.dwExp = (DWORD)(sendScene.dwExp * 2);
				}

				if (i>=3 && i<=9)
				{
					sendScene.dwExp = (DWORD)(sendScene.dwExp * 1.7);
				}

				if (i>9 && i<20)
				{
					sendScene.dwExp = (DWORD)(sendScene.dwExp * 1.4);
				}

				sendScene.dwGrace = pothunters[i].dwGrace;
				pUser->scene->sendCmd(&sendScene,sizeof(sendScene)); /// 通知场景服务器
			}
		}
	}
	
		
	//for_each(answered.begin(), answered.end(), PrintSet());
		
	rwlock.unlock();
	CQuiz::setOverState();
}

void CQuizWorld::userOnline(UserSession* pUser)
{
	for (unsigned int i=0; i<pothunters.size(); i++)
	{
		if (pothunters[i].dwUserID == pUser->id)
		{
			return;
		}
	}

	rwlock.wrlock();
	if (count<1500 && pUser && this->isActivePeriod())
	{
		Cmd::stQueryQuiz send;
		if (this->subject_type == 2)
		{
			send.byQuizType = 2;
			if (pUser->level<=20)
				pUser->sendCmdToMe(&send, sizeof(send));
		}
		else
		{
			pUser->sendCmdToMe(&send, sizeof(send));
		}
	}

	rwlock.unlock();
}

void CQuizWorld::userOffline(UserSession* pUser)
{
	rwlock.wrlock();
		
	for (std::vector<CPothunter>::iterator it=pothunters.begin(); it!=pothunters.end(); )
	{
		if (it->dwUserID == pUser->id)
		{
			it = pothunters.erase(it);
			break;
		}
		else
		{
			it++;
		}
	}

	rwlock.unlock();

}

void CQuizWorld::exitQuiz(DWORD dwUserID)
{
	rwlock.wrlock();
	Cmd::Session::t_quizAward_SceneSession sendScene;
	Cmd::stQuizExit send;
	UserSession* pUser = UserSessionManager::getInstance()->getUserByID(dwUserID);

			
	for (std::vector<CPothunter>::iterator it=pothunters.begin(); it!=pothunters.end(); )
	{
		if (it->dwUserID == dwUserID)
		{
			it = pothunters.erase(it);

			if (pUser)
			{
				pUser->sendCmdToMe(&send, sizeof(send));
				if (pUser->scene)
				{
					sendScene.dwUserID = dwUserID;
					sendScene.dwExp = 0;
					sendScene.dwGrace = 0;
					sendScene.dwMoney = 0;
					pUser->scene->sendCmd(&sendScene,sizeof(sendScene)); /// 通知场景服务器
				}
			}

			break;
		}
		else
		{
			it++;
		}
	}

	rwlock.unlock();
}

//-------------------------------------------------------------------------------------
CQuizPersonal::CQuizPersonal() : CQuiz(0,0)
{
	this->type = QuizDef::PERSONAL_QUIZ;
	this->total_subject = 19;
	this->subject_type = 1; 
}

CQuizPersonal::CQuizPersonal(DWORD active_time, DWORD ready_time)
{
	count = 0;
	ready_question_time = 10;
	cur_subject = 0;
	this->total_subject = 19;

	if (active_time == 0)
	{
		this->active_time = 600;
	}
	else 
	{
		this->active_time = active_time;
	}

	this->ready_time = 0;
	this->subject_type = 1;

	this->type = QuizDef::PERSONAL_QUIZ;
}

CQuizPersonal::~CQuizPersonal()
{
}

/**
 * \brief 回答问题
 *
 *
 * \param pCmd 回答命令
 * \return -1：迟到的回答，0:回答正确, 1:回答错误
 */
int CQuizPersonal::answer(Cmd::stAnswerQuiz* pCmd, DWORD dwUserID)
{
	int ret = -1;
	rwlock.wrlock();

#ifdef _ALL_SUPER_GM
/*	Zebra::logger->debug("----题目跟踪[答题](%d):subject:%d title:%s answer:%d user_answer:%d(个人)", 
			dwUserID, cur_subject+1, 
			CSubjectM::getMe().gsubjects[subjects[this->cur_subject]].title, 
			CSubjectM::getMe().gsubjects[subjects[this->cur_subject]].answer,
			pCmd->answerID);*/
#endif
	if (pCmd->dwID == (this->cur_subject+1) && this->getState() == QUIZ_ACTIVE_QUESTION)
	{
		if ((this->cur_subject>=0) && (this->cur_subject<100))
		{
			CSubject temp = subjects[this->cur_subject];
//			CSubject temp = CSubjectM::getMe().gsubjects[subjects[cur_subject]];
			int cur_score = abs(pCmd->dwScore)>15?15:abs(pCmd->dwScore);
			//int cur_sec =  question_count>?10;
			//int cur_score = cur_sec<25 ? (25-cur_sec) : 0;
		
			if (temp.answer == pCmd->answerID)
			{
				pothunter.dwScore = pothunter.dwScore + cur_score;
				pothunter.dwGrace++;
				ret = 0;
			}
			else
			{
				/*if (pothunter.dwScore>cur_score)
				{
					pothunter.dwScore = pothunter.dwScore - cur_score;
				}
				else
				{
					pothunter.dwScore = 0;
				}*/
				
				if ((int)pothunter.dwGrace>1)
				{
					pothunter.dwGrace--;
				}
				else
				{
					pothunter.dwGrace = 0;
				}
			
				ret = 1;
			}
		}
		pothunter.dwAnswerStatus = ret;
	}
	rwlock.unlock();

	return ret;
}

void CQuizPersonal::setSendQuestionState()
{
	char buf[zSocket::MAX_DATASIZE];
	
	CQuiz::setSendQuestionState();

	if (cur_subject>total_subject)
	{
		this->setReadyOverState();
		return;
	}
	
	rwlock.wrlock();
	CSubject temp_subject = subjects[cur_subject];
//	CSubject temp_subject = CSubjectM::getMe().gsubjects[subjects[cur_subject]];
	
#ifdef _ALL_SUPER_GM
	Zebra::logger->debug("----题库跟踪[发题]:(%d)subject:%d title:%s answer:%d(个人)", pothunter.dwUserID, 
			cur_subject+1, temp_subject.title, temp_subject.answer);
#endif	

	Cmd::stQuestionQuiz *send = (Cmd::stQuestionQuiz*)buf;
	constructInPlace(send);
	
	send->dwID = cur_subject + 1;
	send->bySpareTime = 25;

	char* pos = send->subject;
	sprintf(pos, "%s", temp_subject.title);
	pos = pos + strlen(temp_subject.title);
	*pos = 0x00;
	pos++;
		
	sprintf(pos, "%s", temp_subject.answer_a);
	pos = pos + strlen(temp_subject.answer_a);
	*pos = 0x00;
	pos++;
	
	sprintf(pos, "%s", temp_subject.answer_b);
	pos = pos + strlen(temp_subject.answer_b);
	*pos = 0x00;
	pos++;

	
	sprintf(pos, "%s", temp_subject.answer_c);
	pos = pos + strlen(temp_subject.answer_c);
	*pos = 0x00;
	pos++;

	sprintf(pos, "%s", temp_subject.answer_d);
	pos = pos + strlen(temp_subject.answer_d);
	*pos = 0x00;
	pos++;

	sprintf(pos, "%s", temp_subject.answer_e);
	pos = pos + strlen(temp_subject.answer_e);
	*pos = 0x00;
	pos++;
	
	sprintf(pos, "%s", temp_subject.answer_f);
	pos = pos + strlen(temp_subject.answer_f);
	*pos = 0x00;


	unsigned int cmdLen = sizeof(Cmd::stQuestionQuiz) + strlen(temp_subject.title) + strlen(temp_subject.answer_a) +
		strlen(temp_subject.answer_b) + strlen(temp_subject.answer_c) + strlen(temp_subject.answer_d) +
		strlen(temp_subject.answer_e) + strlen(temp_subject.answer_f) + 7;

	//UserSession* pUser = UserSessionManager::getInstance()->getUserByID(pothunter.dwUserID);
	UserSession *pUser = user;
		
	if (pUser)
		pUser->sendCmdToMe(send, cmdLen);

	question_count = 0;
	rwlock.unlock();
	CQuiz::setActiveQuestionState();
}

void CQuizPersonal::setEndQuestionState()
{
	CQuiz::setEndQuestionState();
	rwlock.wrlock();
	// 发送本次答题后，人物的得分和文采
	Cmd::stQuizCurScore send;
	send.dwScore = pothunter.dwScore;
	send.dwGrace = pothunter.dwGrace;
	
	UserSession *pUser=UserSessionManager::getInstance()->getUserByID(pothunter.dwUserID);

	if (pUser)
	{
		if (pothunter.dwAnswerStatus!=-1)
		{
			Cmd::stAnswerReturnQuiz sendAnswerStatus;
			sendAnswerStatus.byAnswerStatus = pothunter.dwAnswerStatus;
			pUser->sendCmdToMe(&sendAnswerStatus, sizeof(sendAnswerStatus));
			pothunter.dwAnswerStatus = -1;
		}
		
		pUser->sendCmdToMe(&send, sizeof(send));
	}
	this->question_count = 25;

	rwlock.unlock();
}

void CQuizPersonal::setReadyOverState()
{

	CQuiz::setReadyOverState();
	
	//UserSession *pUser=UserSessionManager::getInstance()->getUserByID(pothunter.dwUserID);
	
	UserSession *pUser = user;
	Cmd::Session::t_quizAward_SceneSession sendScene;
	Cmd::stQuizExit send;
		
	if (pUser)
	{

		Zebra::logger->trace("[个人答题]:%s 结束个人答题", pUser->name);
		rwlock.wrlock();
		pUser->sendCmdToMe(&send, sizeof(send));
		if (pUser->scene)
		{
			sendScene.dwUserID = pothunter.dwUserID;
			//int(0.14*答题分数*角色等级^2*(0.75+0.25*N)+200)
	//		sendScene.dwExp = (DWORD)((0.14*pothunter.dwScore*((int)pow(pUser->level, 2))));

			//int(0.2*答题分数*角色等级^2*(0.8+0.2*N)+200)
	//		sendScene.dwExp = (DWORD)((0.2*pothunter.dwScore*((int)pow(pUser->level, 2))));


//前4次答题所给经验=int(（0.12+0.05*n）*答题分数*角色等级^2 +200)
//  第五次答题所给经验=int(1*答题分数*角色等级^2 +200)
			sendScene.dwExp = (DWORD)((pothunter.dwScore*((int)pow(pUser->level, 2))));//分数*角色等级^2
			sendScene.dwGrace = pothunter.dwGrace;
			sendScene.dwMoney = 0;
			sendScene.byType = 1;

			pUser->scene->sendCmd(&sendScene,sizeof(sendScene)); /// 通知场景服务器
		}

		rwlock.unlock();
		CQuiz::setOverState();
	}
	else
	{//进入待奖励状态,用户上线后，给出奖励
		this->setReturnGoldState();	
	}
}

void CQuizPersonal::setReturnGoldState()
{
	CQuiz::setReturnGoldState();
	
	if (this->award())
	{
		this->setOverState();
	}
}

bool CQuizPersonal::award()
{
	UserSession *pUser=UserSessionManager::getInstance()->getUserByID(pothunter.dwUserID);
	Cmd::Session::t_quizAward_SceneSession sendScene;
	Cmd::stQuizExit send;

	if (pUser)
	{
		pUser->sendCmdToMe(&send, sizeof(send));
		if (pUser->scene)
		{
			sendScene.dwUserID = pothunter.dwUserID;
			sendScene.dwExp = (DWORD)(((70*pUser->level*1.5+200) * pothunter.dwScore)/300);
			sendScene.dwGrace = pothunter.dwGrace;
			sendScene.dwMoney = 0;
			// （70*人物当前等级^1.5+200）*获得分数 / 300

			pUser->sendSysChat(Cmd::INFO_TYPE_EXP, "得到经验值 %d", 
					sendScene.dwMoney, sendScene.dwExp);

			pUser->scene->sendCmd(&sendScene,sizeof(sendScene)); /// 通知场景服务器
		}

		return true;
	}

	return false;

}

bool CQuizPersonal::addPothunters(UserSession* pUser)
{
	if (pUser)
	{
		rwlock.wrlock();
		pothunter.dwUserID = pUser->id;
		this->user = pUser;

		rwlock.unlock();
	}
	return true;
}

void CQuizPersonal::userOnline(UserSession* pUser)
{
	rwlock.wrlock();
	user = pUser;
	rwlock.unlock();
	this->setReturnGoldState();
}

void CQuizPersonal::userOffline(UserSession* pUser)
{
//	if (pUser->id == pothunter.dwUserID)
	this->setReadyOverState();
	rwlock.wrlock();
	user = NULL;
	rwlock.unlock();
}

void CQuizPersonal::exitQuiz(DWORD dwUserID)
{
	if (dwUserID == pothunter.dwUserID)
		this->setReadyOverState();
}

//-------------------------------------------------------------------------------------

CQuizM::CQuizM()
{
	channelUniqeID =new zUniqueDWORDID(10000);
#ifdef _ZJW_DEBUG
	Zebra::logger->trace("CQuizM");
#endif	
}

CQuizM::~CQuizM()
{
	rwlock.wrlock();
	for(zEntryTempID::hashmap::iterator it=zEntryTempID::ets.begin();it!=zEntryTempID::ets.end();it++)
	{
		CQuiz *temp =(CQuiz *)it->second;
		SAFE_DELETE(temp);
	}
	clear();
	rwlock.unlock();
	SAFE_DELETE(channelUniqeID);
#ifdef _ZJW_DEBUG
	Zebra::logger->trace("~CQuizM");
#endif	

}

void CQuizM::destroyMe()
{
	delMe();
}

bool CQuizM::getUniqeID(DWORD &tempid)
{
	tempid=channelUniqeID->get();
	return (tempid!=channelUniqeID->invalid());
}

void CQuizM::putUniqeID(const DWORD &tempid)
{
	channelUniqeID->put(tempid);
}

bool CQuizM::init()
{
	return true;
}

bool CQuizM::processSceneMessage(const Cmd::t_NullCmd *cmd, const unsigned int cmdLen)
{
	using namespace Cmd::Session;
	if (cmd)
	{
		switch (cmd->para)
		{
			case Cmd::Session::PARA_SCENE_CREATE_QUIZ:
				{
					t_createQuiz_SceneSession* rev = (t_createQuiz_SceneSession*)cmd;
					this->addNewQuiz_sceneSession(rev);
					return true;
				}
				break;
			default:
				break;
		}

	}
	return false;
}

bool CQuizM::addNewQuiz_sceneSession(Cmd::Session::t_createQuiz_SceneSession* pCmd)
{
	if (pCmd->type == QuizDef::WORLD_QUIZ)
	{
		if (findWorldQuiz() == NULL)
		{
			CQuiz* pQuiz = new CQuizWorld(0, pCmd->ready_time, pCmd->subject_type);

			if (pQuiz)
			{
				rwlock.wrlock();
				addEntry(pQuiz);
				rwlock.unlock();

				if (pCmd->active_time<60)
				{
					pQuiz->total_subject = abs((int)pCmd->active_time - 1);
					Zebra::logger->debug("---本轮总题目数:%d, 原始设定:%d", pQuiz->total_subject, 
							pCmd->active_time);
				}
				else
				{
					pQuiz->total_subject = 60 - 1;//保存总题数
				}
				
				pQuiz->setReadyState();
			}

			return true;
		}
	}
	else
	{
		if (findPersonalQuiz(pCmd->dwUserID) == NULL)	
		{
			CQuiz* pQuiz = new CQuizPersonal(pCmd->active_time, pCmd->ready_time);
			UserSession* pUser = UserSessionManager::getInstance()->getUserByID(pCmd->dwUserID);
			
			if (pQuiz)
			{
				if (pUser)
				{
					rwlock.wrlock();
					addEntry(pQuiz);
					Zebra::logger->trace("[个人答题]:%s 开始个人答题", pUser->name);

					pQuiz->total_subject = abs((int)pCmd->dwSubjects-1);//保存总题数
					pQuiz->addPothunters(pUser);
					pQuiz->setReadyState();

					Cmd::stQuizParam send;
					send.byType = Cmd::QUIZ_PERSONAL;
					send.byStartTime = 0;
					send.bySubjectNumber = pCmd->dwSubjects;//pQuiz->active_time/30;//以后，按购买的银两进行计算
					pUser->sendCmdToMe(&send, sizeof(send));

					rwlock.unlock();
				}
				else
				{
					SAFE_DELETE(pQuiz)
				}
			}
		}
	}

	return false;
}

CQuiz* CQuizM::findPersonalQuiz(DWORD dwUserID)
{
	struct execAll : public execEntry<CQuiz>
	{
		CQuiz* _pQuiz;
		DWORD _dwUserID;
		
		execAll(DWORD dwUserID)
		{
			_pQuiz = NULL;
			_dwUserID = dwUserID;
		}
		
		~execAll(){}
		
		bool exec(CQuiz *pQuiz)
		{
			if (pQuiz && pQuiz->getType()==QuizDef::PERSONAL_QUIZ)
			{
				CQuizPersonal* temp = (CQuizPersonal*)pQuiz;
				
				if (temp->pothunter.dwUserID == _dwUserID)
				{
					_pQuiz = pQuiz;
					return false;
				}
			}
			
			return true;
		}
	};

	execAll myList(dwUserID);
        execEveryOne(myList);
	return myList._pQuiz;
}

bool CQuizM::processUserMessage(UserSession *pUser,const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen)
{
	if (ptNullCmd->byCmd == Cmd::QUIZ_USERCMD)
	{
		switch (ptNullCmd->byParam)
		{
			case Cmd::QUERY_QUIZ_PARA:
				{
					Cmd::stQueryQuiz* rev = (Cmd::stQueryQuiz*)ptNullCmd;
					if (rev->byMsgType == Cmd::QUIZ_YES)
					{
						CQuizWorld* world = (CQuizWorld*)this->findWorldQuiz();
						
						if (world && world->subject_type==2)
						{
							if (pUser->level>20)
							{
								pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
									"超过20级不能参加该类竞赛");
								return true;
							}
						}

						if (world && world->addPothunters(pUser))
						{
							Cmd::stQuizParam send;
							send.byType = Cmd::QUIZ_WORLD;
							if (world->count<=10)
							{
								send.byStartTime = 10-(int)world->count;
							}
							else
							{
								send.byStartTime = abs(30-(int)world->question_count);
							}
							
							if (world->cur_subject == 0)
							{
								send.bySubjectNumber = world->total_subject+1;
							}
							else
							{
								send.bySubjectNumber = 
									world->total_subject - world->cur_subject;
							}

							pUser->sendCmdToMe(&send, sizeof(send));
						}
						else
						{
							pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你不能重复参加智力竞赛!");
						}
					}

					return true;
				}
				break;
			case Cmd::ANSWER_QUIZ_PARA:
				{
					Cmd::stAnswerQuiz* rev = (Cmd::stAnswerQuiz*)ptNullCmd;
//					Cmd::stAnswerReturnQuiz send;

//					CQuiz* pQuiz = (CQuiz*)CQuizM::getMe().getEntryByTempID(rev->dwQuizID);
					if (rev->byType == Cmd::QUIZ_WORLD)
					{
						CQuizWorld* pQuiz = (CQuizWorld*)this->findWorldQuiz();

						if (pQuiz)
						{
							//send.byAnswerStatus = pQuiz->answer(rev, pUser->id);
							pQuiz->answer(rev, pUser->id);
						}
					}
					else
					{
						CQuiz* pQuiz = CQuizM::findPersonalQuiz(pUser->id);

						if (pQuiz)
						{
							//send.byAnswerStatus = pQuiz->answer(rev, pUser->id);
							pQuiz->answer(rev, pUser->id);
						}
					}

//					pUser->sendCmdToMe(&send, sizeof(send));

					return true;
				}
				break;
			case Cmd::QUIZ_EXIT_PARA:
				{
					CQuizWorld* pWorldQuiz = (CQuizWorld*)this->findWorldQuiz();

					if (pWorldQuiz)
						pWorldQuiz->exitQuiz(pUser->id);

					CQuiz* pQuiz = CQuizM::findPersonalQuiz(pUser->id);

					if (pQuiz)
					{
						pQuiz->exitQuiz(pUser->id);
					}

					return true;
				}
				break;
			default:
				break;
		}
	}

	return false;
}

void CQuizM::printSize()
{
//	Zebra::logger->trace("答题目前对象数:%d", this->size());
}

void  CQuizM::timer()
{
	struct execAll : public execEntry<CQuiz>
	{
		std::vector<CQuiz *> _removeList;

		execAll(){}
		~execAll(){}

		void removeList()
		{
			std::vector<CQuiz *>::iterator tIterator;
			for (tIterator = _removeList.begin(); tIterator != _removeList.end(); tIterator++)
			{
				CQuiz* cq = *tIterator;
				CQuizM::getMe().removeEntry(cq);
				SAFE_DELETE(cq);
			}
			_removeList.clear();
		}

		bool exec(CQuiz *pQuiz)
		{
			pQuiz->updateTime();
			
			if (pQuiz && pQuiz->getType()==QuizDef::WORLD_QUIZ)
			{
				if (!pQuiz->isReadyPeriod() && pQuiz->getState()==CQuiz::QUIZ_READY)
				{// 已过准备期，进入发送询问状态
					pQuiz->setReadyQuestionState();
				}
				else if (!pQuiz->isReadyQuestionPeriod() && pQuiz->getState()==CQuiz::QUIZ_READY_QUESTION)
				{// 已过发送询问期, 进入发题状态
					pQuiz->setSendQuestionState(); // 该状态结束后，会自动转入答题状态
				}
				else if (pQuiz->isActivePeriod())
				{
					if (!pQuiz->isActiveQuestionPeriod() 
					&& pQuiz->getState() == CQuiz::QUIZ_ACTIVE_QUESTION)
					{// 已过答题期，进入结果计算期
						pQuiz->setEndQuestionState();
					}
					else if(!pQuiz->isEndQuestionPeriod() 
					&& pQuiz->getState() == CQuiz::QUIZ_END_QUESTION)
					{// 已过结果计算期，进入新一轮发题状态
						pQuiz->setSendQuestionState();
					}
				}
				else if (pQuiz->getState()!=CQuiz::QUIZ_READ_SORT)
				{//进入查看排名期
					pQuiz->setReadSortState();
				}
				else if (pQuiz->getState()==CQuiz::QUIZ_READ_SORT && !pQuiz->isReadSortPeriod())
				{//已过查看排名期
					pQuiz->setReadyOverState();
				}
								
				if (pQuiz->getState() == CQuiz::QUIZ_OVER)
				{
					_removeList.push_back(pQuiz);
				}
			}
			else
			{
				CQuizPersonal* pQuizPersonal = (CQuizPersonal*)pQuiz;

				if (!pQuizPersonal->isReadyPeriod() && pQuizPersonal->getState()==CQuiz::QUIZ_READY)
				{// 已过准备期，进入发送题目状态
					pQuizPersonal->setSendQuestionState();
				}
				else if (pQuizPersonal->isActivePeriod())
				{
					if ((!pQuizPersonal->isActiveQuestionPeriod() && 
					pQuizPersonal->getState() == CQuiz::QUIZ_ACTIVE_QUESTION)
					|| (pQuizPersonal->pothunter.dwAnswerStatus != -1)
					)
					{// 已过答题期或已进行了答题，进入结果计算期
						pQuizPersonal->setEndQuestionState();
					}
					else if(!pQuizPersonal->isEndQuestionPeriod() && 
						pQuizPersonal->getState() == CQuiz::QUIZ_END_QUESTION)
					{// 已过结果计算期，进入新一轮发题状态
						pQuizPersonal->setSendQuestionState();
					}
				}
				else
				{
					pQuizPersonal->setReadyOverState();
				}
				
				if (pQuizPersonal->getState() == CQuiz::QUIZ_OVER)
				{
					_removeList.push_back(pQuiz);
				}
			}
			
			return true;
		}
	};

	execAll myList;
	execEveryOne(myList);
	myList.removeList();
}

CQuiz* CQuizM::findWorldQuiz()
{
	struct execAll : public execEntry<CQuiz>
	{
		CQuiz* _pQuiz;
		execAll()
		{
			_pQuiz = NULL;
		}
		
		~execAll(){}
		
		bool exec(CQuiz *pQuiz)
		{
			if (pQuiz && pQuiz->getType()==QuizDef::WORLD_QUIZ)
			{
				_pQuiz = pQuiz;
				return false;
			}
			
			return true;
		}
	};

	execAll myList;
        execEveryOne(myList);
	return myList._pQuiz;
}


/**
 * \brief 用户上线处理
 *
 * 用户上线时，同时判断该用户的竞赛状态，并做相应处理,个人问答上线不需处理。
 * 世界竞赛时，需要判断其是否已经参加了竞赛,如果没有参加，则发出询问，如果已经参加，则不再处理
 *
 * \param pUser 上线用户
 *
 */
void CQuizM::userOnline(UserSession* pUser)
{
	CQuiz* pWorldQuiz = this->findWorldQuiz();
	if (pWorldQuiz)
		pWorldQuiz->userOnline(pUser);

	CQuiz* pQuiz = this->findPersonalQuiz(pUser->id);
	if (pQuiz)
		pQuiz->userOnline(pUser);

}
	
/**
 * \brief 用户下线处理
 *
 * 用户下线时，同时判断该用户的竞赛状态，并做相应处理, 个人问答下线时，进入setReadyOver状态，全国竞赛时不做处理
 *
 * \param pUser 下线用户
 *
 */  
void CQuizM::userOffline(UserSession* pUser)
{
	CQuiz* pQuiz = this->findPersonalQuiz(pUser->id);
	if (pQuiz)
		pQuiz->userOffline(pUser);

	CQuiz* pQuizWorld = this->findWorldQuiz();
	if (pQuizWorld)
		pQuizWorld->userOffline(pUser);
}


//-------------------------------------------------------------------------------------

CSubjectM::CSubjectM()
{
/*	if (atoi(Zebra::global["answer_count"].c_str())>=1)
	{
		gsubjects.resize(atoi(Zebra::global["answer_count"].c_str()));
	}
	else
	{
		gsubjects.resize(1000);
	}
*/
}

CSubjectM::~CSubjectM()
{
	gsubjects.clear();
	levels.clear();
	personals.clear();
}

bool CSubjectM::init()
{

	FieldSet* BossInfo = SessionService::metaData->getFields("BOSSINFO");
	if (BossInfo)
	{
		Zebra::logger->debug("初始化BOSSINFO数据表状态");
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return false;
		}

		RecordSet* recordset = NULL;
		Record rec;

		if ((connHandleID)-1 != handle)
		{
			recordset = SessionService::dbConnPool->exeSelect(handle, BossInfo, NULL, NULL);
		}

		SessionService::dbConnPool->putHandle(handle);

		if (recordset)
		{
			for (unsigned int i=0; i<recordset->size(); i++)
			{
				rec.put("KILLSTATE", 0);
			}
			
			if ((connHandleID)-1 != handle)
            {
                SessionService::dbConnPool->exeUpdate(handle, BossInfo, &rec, NULL);
            }
		}
	    SessionService::dbConnPool->putHandle(handle);
	}
	
	FieldSet* pAnswerTable = SessionService::metaData->getFields("ANSWER");
	Record where, orderby;
	std::ostringstream oss;
	int db_answer_count = atoi(Zebra::global["answer_count"].c_str());
	if (db_answer_count == 0)
	{
		db_answer_count = 1000;
	}

	oss << "id >=1 AND " << "id<=" << db_answer_count;

	where.put("id", oss.str());

	oss.str("");
	oss << "CHAR_LENGTH(ask)<250";
	where.put("ask", oss.str());
	gsubjects.clear();
	personals.clear();
	levels.clear();


	if (pAnswerTable)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();

		RecordSet* recordset = NULL;

		if ((connHandleID)-1 != handle)
		{
			recordset = SessionService::dbConnPool->exeSelect(handle, pAnswerTable, NULL, &where);
		}

		SessionService::dbConnPool->putHandle(handle);

		if (recordset)
		{
			for (unsigned int i=0; i<recordset->size(); i++)
			{
				Record* rec = recordset->get(i);
				if (rec)
				{
					CSubject sub;
					strncpy(sub.title, (const char*)rec->get("ask"), sizeof(sub.title));
					if (strlen(sub.title) <= 1)
					{
						continue;
					}
					
					if ((const char*)rec->get("answer_a")!=NULL)
					{
						strncpy(sub.answer_a, 
								(const char*)rec->get("answer_a"), sizeof(sub.answer_a));
					}

					if ((const char*)rec->get("answer_b")!=NULL)
					{
						strncpy(sub.answer_b, 
								(const char*)rec->get("answer_b"), sizeof(sub.answer_b));
					}


					if ((const char*)rec->get("answer_c")!=NULL)
					{
						strncpy(sub.answer_c, 
								(const char*)rec->get("answer_c"), sizeof(sub.answer_c));
					}


					if ((const char*)rec->get("answer_d")!=NULL)
					{
						strncpy(sub.answer_d, 
								(const char*)rec->get("answer_d"), sizeof(sub.answer_d));
					}


					if ((const char*)rec->get("answer_e")!=NULL)
					{
						strncpy(sub.answer_e, 
								(const char*)rec->get("answer_e"), sizeof(sub.answer_e));
					}


					if ((const char*)rec->get("answer_f")!=NULL)
					{
						strncpy(sub.answer_f, 
								(const char*)rec->get("answer_f"), sizeof(sub.answer_f));
					}

					sub.answer = rec->get("the_answer");
					sub.quiz_type = rec->get("quiz_type");

					if (sub.quiz_type == 0)
					{
						gsubjects.push_back(sub);
					}
					else if (sub.quiz_type == 1)
					{
						personals.push_back(sub);
					}
					else if (sub.quiz_type == 2)
					{
						levels.push_back(sub);
					}
				}
			}

			SAFE_DELETE(recordset)
		}
	}
#ifdef _ALL_SUPER_GM
	Zebra::logger->debug("--答题跟踪:题库题目总数：g:%d, p:%d l:%d", gsubjects.size(), personals.size(), levels.size());
#endif	
	return true;
}

void CSubjectM::destroyMe()
{
	delMe();
}

