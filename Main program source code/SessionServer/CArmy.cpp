/**
 * \file
 * \version  $Id: CArmy.cpp  $
 * \author 
 * \date 
 * \brief 国家军队
 *
 * 
 */
#include <sstream>
#include <memory>
#include "zDBConnPool.h"
#include "zMetaData.h"
#include "zTime.h"
#include "SessionServer.h"
#include "SessionManager.h"
#include "Command.h"
#include "CCountryManager.h"
#include "CCityManager.h"
#include "CArmy.h"
#include "CUnion.h"
#include "SessionChat.h"

CArmyM::CArmyM()
{
}

CArmyM::~CArmyM()
{
}

bool CArmyM::init()
{
	return this->load();
}

void CArmyM::destroyMe()
{
	for (unsigned int i=0; i<CArmyM::getMe().armys.size(); i++)
	{       
		CArmyM::getMe().armys[i]->writeDatabase();
	}

	delMe();
}

bool CArmyM::load()
{
	//FunctionTimes times(77,__FUNCTION__);
	FieldSet* army = SessionService::metaData->getFields("ARMY");
	
	if (army)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return false;
		}

		RecordSet* recordset = NULL;

		if ((connHandleID)-1 != handle)
		{
			recordset = SessionService::dbConnPool->exeSelect(handle, army, NULL, NULL);
		}

		SessionService::dbConnPool->putHandle(handle);

		if (recordset)
		{
			for (unsigned int i=0; i<recordset->size(); i++)
			{
				Record* rec = recordset->get(i);
				CArmy* pArmy = new CArmy();

				if (pArmy)
				{
					pArmy->init(rec);
					pArmy->loadCaptainFromDB();
				}
					
				armys.push_back(pArmy);
			}

			SAFE_DELETE(recordset)
		}
	}
	else
	{
		Zebra::logger->error("军队数据加载失败，ARMY表不存在");
		return false;
	}

	return true;
}

void CArmyM::userOnline(UserSession* pUser)
{
	capIter sIterator;
	CCaptain *pCaptain = NULL; 

	rwlock.rdlock();
	sIterator = captainIndex.find(pUser->id);
	if (sIterator != captainIndex.end()) pCaptain = (*sIterator).second;
	rwlock.unlock();

	if (pCaptain!=NULL) 
	{
		pCaptain->update_scene();
	}
}

bool CArmyM::processSceneMessage(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen)
{
	switch (ptNullCmd->para)        
	{                               
		case Cmd::Session::REQ_ARMY_LIST_SCENE_PARA:
			{              
				Cmd::Session::t_ReqArmyList_SceneSession* cmd = 
					(Cmd::Session::t_ReqArmyList_SceneSession*)ptNullCmd;

				this->processReqArmyList(cmd);
				return true;
			}
		default:
			break;
	} 

	return false;
}

bool CArmyM::processUserMessage(UserSession *pUser, const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen)
{
	switch (ptNullCmd->byParam) 
	{
		case FIRE_ARMY_CAPTAIN_PARA:
			{
				Cmd::stFireArmyCaptainUserCmd* rev = (Cmd::stFireArmyCaptainUserCmd*)ptNullCmd;
				this->processFireCaptain(pUser, rev);
				return true;
			}
			break;
		case ADD_ARMY_CAPTAIN_PARA:
			{
				Cmd::stAddArmyCaptainUserCmd* rev = (Cmd::stAddArmyCaptainUserCmd*)ptNullCmd;
				this->processAddCaptain(pUser, rev);
				return true;
			}
			break;
		case REMOVE_ARMY_PARA:
			{
				Cmd::stRemoveArmyUserCmd* rev = (Cmd::stRemoveArmyUserCmd*)ptNullCmd;
				this->processRemoveArmy(pUser, rev);
				return true;
			}
			break;
		case EXIT_ARMY_PARA:
			{
				Cmd::stExitArmyUserCmd* rev = (Cmd::stExitArmyUserCmd*)ptNullCmd;
				this->processExitArmy(pUser, rev);
				return true;
			}
			break;
		case CHANGE_ARMY_NAME_PARA:
			{
				Cmd::stChangeArmyNameUserCmd* rev = (Cmd::stChangeArmyNameUserCmd*)ptNullCmd;
				this->processChangeArmyName(pUser, rev);
				return true;
			}
			break;
		case REQ_ARMY_SPEC_PARA:
			{
				Cmd::stReqArmySpecUserCmd* rev = (Cmd::stReqArmySpecUserCmd*)ptNullCmd;
				this->processReqArmySpec(pUser, rev);
				return true;
			}
			break;
		case REQ_ARMY_GEN_PARA:
			{
				Cmd::stReqArmyGenUserCmd* rev = (Cmd::stReqArmyGenUserCmd*)ptNullCmd;
				
				if (rev->byStatus == Cmd::YES_ARMY_GEN)
				{
					CArmy* pArmy = this->findByGenID(pUser->id);
					
					if (pArmy)
					{
						pArmy->status(CArmy::FINISH_CREATE);
						pArmy->insertDatabase();
						pArmy->hireCaptain(pUser->id);
						pUser->sendSysChat(Cmd::INFO_TYPE_EXP, "恭喜你成为 %s 军队将军", 
								pArmy->name);
						Zebra::logger->trace("[军队]: %s 军队建立成功", pArmy->name);
						
						SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pArmy->dwCountryID, "恭喜 %s 军队成立", 
								pArmy->name);

					}
					else
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "对你的任命已过期");
					}
				}

				return true;
			}
			break;
		case CREATE_ARMY_PARA:
			{
				Cmd::stCreateArmyUserCmd* rev = (Cmd::stCreateArmyUserCmd*)ptNullCmd;
				this->processCreateArmy(pUser, rev);
				return true;
			}
			break;
		case REQ_WAIT_GEN_PARA:
			{
				Cmd::stReqWaitGenUserCmd* rev = (Cmd::stReqWaitGenUserCmd*)ptNullCmd;
				this->processReqWaitGen(pUser, rev);

				return true;
			}
			break;
		default:
			break;
	}

	return false;
}

void CArmyM::processFireCaptain(UserSession* pUser, Cmd::stFireArmyCaptainUserCmd* rev)
{
	capIter sIterator;
	CCaptain *pCaptain = NULL; 

	sIterator = captainIndex.find(pUser->id);
	if (sIterator != captainIndex.end()) pCaptain = (*sIterator).second;

	if (pCaptain!=NULL) 
	{
		if (pCaptain->myArmy && pCaptain->myArmy->dwGenID == pUser->id && pUser->id != rev->dwUserID)
		{
			if (pCaptain->myArmy->fireCaptain(rev->dwUserID))
			{
				Zebra::logger->trace("[军队]: %s 离开 %s 军队", pUser->name, pCaptain->myArmy->name);
				pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "开除成功");
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不能开除自己或你不是将军");
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是将军不能开除队长");
	}

}

void CArmyM::processAddCaptain(UserSession* pUser, Cmd::stAddArmyCaptainUserCmd* rev)
{
	if (rev->byState == Cmd::QUESTION_CAPTAIN) 
	{
		capIter sIterator;
		CCaptain *pCaptain = NULL; 

		sIterator = captainIndex.find(pUser->id);
		if (sIterator != captainIndex.end()) pCaptain = (*sIterator).second;

		if (pCaptain!=NULL) 
		{
			if (pCaptain->myArmy && pCaptain->myArmy->dwGenID == pUser->id)
			{
				if (pCaptain->myArmy->canAddCaptain())
				{
					UserSession* pAddUser = UserSessionManager::getInstance()->
						getUserSessionByName(rev->capName);
					
					if (pAddUser)
					{
						if (captainIndex.find(pAddUser->id) != captainIndex.end())
						{
							pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, 
									"对方已加入其它军队，不能招收");
							return;
						}

						if (!CCityM::getMe().isCastellan(pAddUser))	
						{
							rev->capID = pAddUser->id;
							strncpy(rev->armyName, pCaptain->myArmy->name, MAX_NAMESIZE);
							rev->armyID = pCaptain->myArmy->dwID;
							pAddUser->sendCmdToMe(rev, sizeof(Cmd::stAddArmyCaptainUserCmd));
						}
					}
					else
					{
						pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "对方不在线，不能招收");
					}
				}
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是将军，不能招收队长");
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是将军，不能招收队长");
		}
	}
	else if (rev->byState == Cmd::ANSWER_CAPTAIN_YES)
	{
		CArmy* pArmy = this->findByID(rev->armyID);

		if (pArmy)
		{
			pArmy->hireCaptain(rev->capID);
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "要参加的军队不存在");
		}
	}
	else if (rev->byState == Cmd::ANSWER_CAPTAIN_NO)
	{
		CArmy* pArmy = this->findByID(rev->armyID);

		if (pArmy)
		{
			UserSession* pGenUser = UserSessionManager::getInstance()->getUserByID(pArmy->dwGenID);
			if (pGenUser)
			{
				pGenUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "%s 拒绝参军", pUser->name);
			}
		}
	}
}

void CArmyM::processRemoveArmy(UserSession* pUser, Cmd::stRemoveArmyUserCmd* rev)
{
	CArmy* pArmy = this->findByID(rev->dwArmyID);
	
	if (pArmy)
	{
		CCity* pCity = CCityM::getMe().findByUnionID(pUser->unionid);
		CUnion* pUnion = CUnionM::getMe().getUnionByID(pUser->unionid);

		if ((pCity && pUnion && pArmy->dwCityID == pCity->dwCityID &&  
			(pUnion->tempid == pUser->id)) || (pArmy->dwGenID == pUser->id))
		{//判断是否为该军队所在城的城主或将军
			this->removeArmyByID(rev->dwArmyID);
		}
		else
		{//该军队的将军
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是该城城主或该军将军，不能解散该军队");
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "该军队不存在");
	}
}

int    CArmyM::countByCity(DWORD dwCountryID, DWORD dwCityID)
{
	int ret = 0;
	rwlock.rdlock();
	
	for (std::vector<CArmy*>::iterator vIter=armys.begin(); vIter!=armys.end(); vIter++)
	{
		CArmy* temp = *vIter;
		if (temp->dwCountryID == dwCountryID && temp->dwCityID == dwCityID)
		{
			ret++;
		}
	}
	rwlock.unlock();
	return ret;

}

void CArmyM::processExitArmy(UserSession* pUser, Cmd::stExitArmyUserCmd* rev)
{
	capIter sIterator;
	CCaptain *pCaptain = NULL; 

	sIterator = captainIndex.find(pUser->id);
	if (sIterator != captainIndex.end()) pCaptain = (*sIterator).second;

	if (pCaptain!=NULL)
	{
		if (pCaptain->myArmy && pCaptain->myArmy->dwGenID == pCaptain->dwCharID)
		{
			this->removeArmyByGenID(pCaptain->dwCharID);
		}
		else
		{
			if (pCaptain->myArmy) 
			{
				CArmy* pArmy = pCaptain->myArmy;
				if (pArmy->fireCaptain(pUser->id))
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_EXP, "你已离开军队");
				}
			}
		}	
	}

}

void CArmyM::processChangeArmyName(UserSession* pUser, Cmd::stChangeArmyNameUserCmd* rev)
{
	CArmy* pArmy = this->findByID(rev->dwArmyID);

	if (pArmy)
	{
		CCity* pCity = CCityM::getMe().findByUnionID(pUser->unionid);
		CUnion* pUnion = CUnionM::getMe().getUnionByID(pUser->unionid);
		
		if (pCity && pUnion && pCity->dwCityID==pArmy->dwCityID && pUnion->tempid == pUser->id)
		{
			if (this->findByName(rev->newArmyName)==NULL)
			{
				pArmy->changeName(rev->newArmyName);
				pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "军队更名成功");
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "新的军队名称重复，请重新输入");
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是该城城主，不能更改该军队名称");
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "没有该军队的信息");
	}
}

void CArmyM::processReqArmySpec(UserSession* pUser, Cmd::stReqArmySpecUserCmd* rev)
{
	CArmy* pArmy = this->findByID(rev->dwArmyID);
	if (pArmy)
	{
		BYTE buf[zSocket::MAX_DATASIZE];
		Cmd::stCaptainBase* tempPoint;
		Cmd::stRtnArmySpecUserCmd* retCmd = (Cmd::stRtnArmySpecUserCmd*)buf;
		constructInPlace(retCmd);
		tempPoint = (Cmd::stCaptainBase*)retCmd->data;
		
		strncpy(retCmd->name, pArmy->name, MAX_NAMESIZE); // 军队名
		strncpy(retCmd->genname, pArmy->genName, MAX_NAMESIZE); // 将军名
		SceneSession * pScene = SceneSessionManager::getInstance()->getSceneByID((pArmy->dwCountryID<<16) 
				+ pArmy->dwCityID);

		if (pScene)
		{
			strncpy(retCmd->cityname , pScene->name, MAX_NAMESIZE); // 所属城市
		}

		pArmy->rwlock.rdlock();
		for (std::vector<CCaptain*>::iterator vIter=pArmy->captains.begin(); vIter!=pArmy->captains.end(); ++vIter)
		{
			CCaptain* pCaptain = *vIter;
			strncpy(tempPoint->name, pCaptain->szCapName, MAX_NAMESIZE);
			tempPoint->dwCharID = pCaptain->dwCharID;
				
			tempPoint++;
			retCmd->dwSize++;
		}

		pArmy->rwlock.unlock();

		if (retCmd->dwSize>0)                   
		{                                       
			pUser->sendCmdToMe(retCmd, (retCmd->dwSize*sizeof(Cmd::stCaptainBase) + 
						sizeof(Cmd::stRtnArmySpecUserCmd)));
		}     
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "没有该军队的信息");
	}
}

void CArmyM::processReqArmyList(Cmd::Session::t_ReqArmyList_SceneSession* rev)
{
	BYTE buf[zSocket::MAX_DATASIZE];
	Cmd::stArmyBaseInfo* tempPoint;
	Cmd::stRtnArmyListUserCmd* retCmd = (Cmd::stRtnArmyListUserCmd*)buf;
	constructInPlace(retCmd);
	UserSession* pUser = UserSessionManager::getInstance()->getUserByID(rev->dwUserID);
	if (pUser == NULL)
	{
		return;
	}

	tempPoint = (Cmd::stArmyBaseInfo*)retCmd->data;
	retCmd->byType = rev->byType;

	rwlock.rdlock();
	for (std::vector<CArmy*>::iterator vIter=armys.begin(); vIter!=armys.end(); vIter++)
	{
		CArmy* temp = *vIter;
		if (temp->byStatus != CArmy::WAIT_CREATE && temp->dwCountryID == pUser->country)
		{
			if ((rev->byType == Cmd::CITY_ARMY_LIST && temp->dwCityID == rev->dwCityID)
			 || (rev->byType != Cmd::CITY_ARMY_LIST))
			{
				tempPoint->dwArmyID = temp->dwID;
				strncpy(tempPoint->name, temp->name, MAX_NAMESIZE); // 军队名称
				SceneSession * pScene = SceneSessionManager::getInstance()->getSceneByID((temp->dwCountryID<<16) + temp->dwCityID);

				if (pScene)
				{
					strncpy(tempPoint->cityname , pScene->name, MAX_NAMESIZE); // 所属城市
				}

				strncpy(tempPoint->genname, temp->genName, MAX_NAMESIZE); // 将军姓名

				tempPoint->dwCapNum = temp->captains.size(); // 队长人数
				tempPoint++;
				retCmd->dwSize++;
			}
		}
	}

	rwlock.unlock();

	if (retCmd->dwSize>0)                   
	{                                       

		if (pUser)
		pUser->sendCmdToMe(retCmd, (retCmd->dwSize*sizeof(Cmd::stArmyBaseInfo) + sizeof(Cmd::stRtnArmyListUserCmd)));
	}    
}

void CArmyM::processCreateArmy(UserSession* pUser, Cmd::stCreateArmyUserCmd* rev)
{
	CCity* pCity = CCityM::getMe().findByUnionID(pUser->unionid);
	CUnion* pUnion = CUnionM::getMe().getUnionByID(pUser->unionid);
	UserSession* pGenUser = UserSessionManager::getInstance()->getUserSessionByName(rev->genName);

	if (strlen(rev->armyName) <=0)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "请输入军队名称");
		return;
	}

	if (pGenUser == NULL)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你选择的将军现在不在线, 请稍后再试");
		return;
	}

	if (pGenUser->id == pUser->id)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不能选择自己成为将军!");
		return;
	}
	
	if (this->findByGenID(pGenUser->id) != NULL)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "%s 已经是将军，请不要重复任命", rev->genName);
		return;
	}

	if (this->findByName(rev->armyName) != NULL)
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "军队名称重复,不能建立军队");
		return;
	}
	

	if (pCity!= NULL && pUnion!=NULL)
	{
		if (this->countByCity(pCity->dwCountry, pCity->dwCityID)>1)
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "军队数已达上限。不能再创建军队");
			return;
		}
			
		if (pUnion->master && pUnion->master->id == pUser->id)
		{
			if (pGenUser)
			{
				CArmy* pArmy = new CArmy();
				if (pArmy) {
				
				pArmy->dwCountryID = pUser->country;
				pArmy->dwCityID = pCity->dwCityID;

				strncpy(pArmy->name, rev->armyName, MAX_NAMESIZE); // 军队名称
				pArmy->dwGenID = pGenUser->id; // 军队将军角色ID
				strncpy(pArmy->genName, pGenUser->name, MAX_NAMESIZE); // 军队将军名字
				pArmy->byStatus = CArmy::WAIT_CREATE;
				pArmy->dwCreateTime = time(NULL);
				pArmy->dwID = 0;

				rwlock.wrlock();
				armys.push_back(pArmy);
				rwlock.unlock();

				Cmd::stReqArmyGenUserCmd send;
				pGenUser->sendCmdToMe(&send, sizeof(send));
				}
			}
			else
			{
				pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "选择的将军不在线，创建军队失败");
			}
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是城主，不能使用该项功能");
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是城主，不能使用该项功能");
	}
}

void CArmyM::processReqWaitGen(UserSession* pUser, Cmd::stReqWaitGenUserCmd* rev)
{
	CCity* pCity = CCityM::getMe().findByUnionID(pUser->unionid);
	CUnion* pUnion = CUnionM::getMe().getUnionByID(pUser->unionid);

	if (pCity!= NULL && pUnion!=NULL)
	{
		if (pUnion->master && pUnion->master->id == pUser->id)
		{
			UserSessionManager::getInstance()->sendExploitSort(pUser);
		}
		else
		{
			pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是城主，不能使用该项功能");
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "你不是城主，不能使用该项功能");
	}

}
CArmy* CArmyM::findByID(DWORD dwArmyID)
{
	CArmy* pArmy = NULL;
	rwlock.rdlock();
	
	for (std::vector<CArmy*>::iterator vIter=armys.begin(); vIter!=armys.end(); vIter++)
	{
		CArmy* temp = *vIter;
		if (temp->dwID == dwArmyID)
		{
			pArmy = temp;
			break;
		}
	}
		
	rwlock.unlock();
	return pArmy;
}

CArmy* CArmyM::findByName(const char* value)
{
	CArmy* pArmy = NULL;
	rwlock.rdlock();
	
	for (std::vector<CArmy*>::iterator vIter=armys.begin(); vIter!=armys.end(); vIter++)
	{
		CArmy* temp = *vIter;
		if (strncmp(temp->name, value, MAX_NAMESIZE) == 0)
		{
			pArmy = temp;
			break;
		}
	}
		
	rwlock.unlock();
	return pArmy;
}

CArmy* CArmyM::findByGenID(DWORD dwGenID)
{
	CArmy* pArmy = NULL;
	rwlock.rdlock();
	
	for (std::vector<CArmy*>::iterator vIter=armys.begin(); vIter!=armys.end(); vIter++)
	{
		CArmy* temp = *vIter;
		if (temp->dwGenID == dwGenID)
		{
			pArmy = temp;
			break;
		}
	}
		
	rwlock.unlock();
	return pArmy;
}

void CArmyM::removeArmyByID(DWORD dwArmyID)
{
	std::vector<CArmy*>::iterator pos;
	
	for (pos = armys.begin(); pos!=armys.end(); pos++)
	{
		CArmy* pArmy = *pos;
		if (pArmy->dwID == dwArmyID)
		{
			pArmy->deleteMeFromDB();

			rwlock.wrlock();
			armys.erase(pos);
			rwlock.unlock();

			SAFE_DELETE(pArmy);
			break;
		}
	}

}

void CArmyM::removeArmyByGenID(DWORD dwGenID)
{
	std::vector<CArmy*>::iterator pos;
	
	for (pos = armys.begin(); pos!=armys.end(); pos++)
	{
		CArmy* pArmy = *pos;
		if (pArmy->dwGenID == dwGenID)
		{
			if (pArmy->dwID > 0) {
				pArmy->deleteMeFromDB();
			}

			rwlock.wrlock();
			armys.erase(pos);
			rwlock.unlock();

			SAFE_DELETE(pArmy);
			break;
		}
	}

}
void CArmyM::timer()
{
	time_t cur_time = time(NULL);

	std::vector<CArmy*> vRemoveList;
	
	rwlock.rdlock();
	for (std::vector<CArmy*>::iterator vIter=armys.begin(); vIter!=armys.end(); vIter++)
	{
		CArmy* temp = *vIter;
		if (temp->byStatus == CArmy::WAIT_CREATE && abs(cur_time-temp->dwCreateTime) > 60)
		{
			vRemoveList.push_back(temp);
		}
	}
	rwlock.unlock();
	
	for (std::vector<CArmy*>::iterator vIter=vRemoveList.begin(); vIter!=vRemoveList.end(); vIter++)
	{
		CArmy* temp = *vIter;
		Zebra::logger->trace("[军队]: %s 军队因将军拒绝任命，撤消军队", temp->name);
		this->removeArmyByGenID(temp->dwGenID);
	}
}

bool CArmyM::addCaptain(DWORD dwUserID, CCaptain* pCaptain)
{
	std::pair<std::map<DWORD, CCaptain*>::iterator, bool> retval;
	rwlock.wrlock();
	retval = captainIndex.insert(captainIndexValueType(dwUserID, pCaptain));
	rwlock.unlock();
	return retval.second;
}

bool CArmyM::removeCaptain(DWORD dwUserID)
{
	bool ret;
	std::map <DWORD, CCaptain*>::iterator sIterator;
	rwlock.wrlock();
	ret = true;
	sIterator = captainIndex.find(dwUserID);
	if (sIterator != captainIndex.end()) captainIndex.erase(sIterator);
	else ret = false;
	rwlock.unlock();
	return ret;
}

bool CArmyM::isCaptain(DWORD dwUserID)
{
	bool ret = false;
	std::map <DWORD, CCaptain*>::iterator sIterator;
	rwlock.rdlock();
	sIterator = captainIndex.find(dwUserID);
	rwlock.unlock();

	if (sIterator != captainIndex.end())
	{
		ret = true;
	}

	return ret;
}

//------------------------CArmy--------------------------------------------
CArmy::CArmy()
{
	this->dwID = 0; // 军队ID
	this->dwCountryID = 0; // 所属国家ID
	this->dwCityID = 0; // 所属城市ID
	this->dwGenID = 0; // 军队将军角色ID
	this->byStatus = 0;
	this->dwCreateTime = 0;

	bzero(name, MAX_NAMESIZE); // 军队名称
	bzero(genName, MAX_NAMESIZE); // 军队将军名字
}

CArmy::~CArmy()
{
	for (std::vector<CCaptain*>::iterator pos = captains.begin(); pos!=captains.end(); pos++)
	{
		SAFE_DELETE(*pos);
	}
}

void CArmy::init(Record* rec)
{
	rwlock.wrlock();
	this->dwID = rec->get("id");
	this->dwCountryID = rec->get("countryid");
	this->dwCityID = rec->get("cityid");
	strncpy(this->name, rec->get("name"),  sizeof(this->name));
	strncpy(this->genName, rec->get("genname"), sizeof(this->genName));
	this->dwGenID = rec->get("genid");

	rwlock.unlock();
	//this->writeDatabase();
}

void CArmy::writeDatabase()
{
	//FunctionTimes times(76,__FUNCTION__);
	Record rec,where;
	std::ostringstream oss;

	if (this->byStatus == CArmy::WAIT_CREATE)
	{
		return;
	}
		
	oss << "id='" << this->dwID << "'";
	where.put("id", oss.str());
	
	rec.put("genname", this->genName);
	rec.put("genid", this->dwGenID);
	rec.put("name", this->name);

	FieldSet* army = SessionService::metaData->getFields("ARMY");

	if (army)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();
		
		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return;
		}

		if ((connHandleID)-1 != handle)
		{
			SessionService::dbConnPool->exeUpdate(handle, army, &rec, &where);
		}

		SessionService::dbConnPool->putHandle(handle);

		/*for (std::vector<CVoteItem*>::iterator vIter=items.begin(); vIter!=items.end(); vIter++)
		{
			CVoteItem* pVoteItem = *vIter;
			if (pVoteItem) {
				pVoteItem->writeDatabase();
			}
		}
		// */
	}
	else
	{
		Zebra::logger->error("军队数据保存失败，ARMY表不存在");
		return;
	}	
}

bool CArmy::insertDatabase()
{
	//FunctionTimes times(75,__FUNCTION__);
	Record rec;
	rec.put("countryid", this->dwCountryID);
	rec.put("cityid", this->dwCityID);
	rec.put("name", this->name);
	rec.put("genname", this->genName);
	rec.put("genid", this->dwGenID);

	FieldSet* army = SessionService::metaData->getFields("ARMY");

	if (army)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{       
			Zebra::logger->error("不能获取数据库句柄");
			return false;
		}
		else
		{       
			unsigned int retcode = SessionService::dbConnPool->exeInsert(handle, army, &rec);
			if ((DWORD)-1 == retcode)
			{
				SessionService::dbConnPool->putHandle(handle);
				return false;
			}	
			SessionService::dbConnPool->putHandle(handle);
			this->dwID = retcode;
		}
	}
	else
	{
		Zebra::logger->error("添加军队记录失败，ARMY表不存在");
		return false;
	}

	return true;
}

bool CArmy::deleteMeFromDB()
{
	//FunctionTimes times(74,__FUNCTION__);
	Record where;
	std::ostringstream oss;
	oss << "id='" << this->dwID << "'";
	where.put("id", oss.str());
	
	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	FieldSet* army = SessionService::metaData->getFields("ARMY");
	
	if (army)
	{
		SessionService::dbConnPool->exeDelete(handle, army, &where);
	}

	SessionService::dbConnPool->putHandle(handle);

	rwlock.wrlock();	
	for (std::vector<CCaptain*>::iterator vIter=captains.begin(); vIter!=captains.end(); vIter++)
	{
		CCaptain* temp = *vIter;
		temp->fireMe();
		SAFE_DELETE(temp);
	}
	captains.clear();
	rwlock.unlock();
	
	return true;
}

bool CArmy::loadCaptainFromDB()
{
	//FunctionTimes times(73,__FUNCTION__);
	FieldSet* captain = SessionService::metaData->getFields("CAPTAIN");
	
	if (captain)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return false;
		}

		RecordSet* recordset = NULL;
		Record where;
		std::ostringstream oss;

		oss << "armyid='" << this->dwID << "'";
		where.put("armyid", oss.str());

		if ((connHandleID)-1 != handle)
		{
			recordset = SessionService::dbConnPool->exeSelect(handle, captain, NULL, &where);
		}

		SessionService::dbConnPool->putHandle(handle);

		if (recordset)
		{
			for (unsigned int i=0; i<recordset->size(); i++)
			{
				Record* rec = recordset->get(i);
				CCaptain* pCaptain = new CCaptain();

				if (pCaptain)
				{
					pCaptain->init(rec);
					pCaptain->myArmy = this;
					CArmyM::getMe().addCaptain(pCaptain->dwCharID, pCaptain);			
					captains.push_back(pCaptain);

				}
			}

			SAFE_DELETE(recordset)
		}
	}
	else
	{
		Zebra::logger->error("军队详细数据加载失败，CAPTAIN表不存在");
		return false;
	}

	return true;
}

bool CArmy::hireCaptain(DWORD dwUserID)
{
	bool ret = false;

	UserSession* pUser = UserSessionManager::getInstance()->getUserByID(dwUserID);
	if (pUser)
	{
		CCaptain* pCaptain = new CCaptain();

		if (pCaptain)
		{
			pCaptain->dwArmyID = this->dwID;
			pCaptain->dwCharID = pUser->id;
			strncpy(pCaptain->szCapName, pUser->name, MAX_NAMESIZE);
			pCaptain->dwNpcNum = 0;

			pCaptain->myArmy = this;
			if (pCaptain->insertDatabase())
			{
				CArmyM::getMe().addCaptain(pCaptain->dwCharID, pCaptain);			

				rwlock.wrlock();
				captains.push_back(pCaptain);
				rwlock.unlock();
				Zebra::logger->trace("[军队]: %s 参加 %s 军队", pUser->name, this->name);

				pCaptain->update_scene();
				ret=true;
			}
			else
			{
				SAFE_DELETE(pCaptain);
			}
		}
	}

	return ret;
}

bool CArmy::fireCaptain(DWORD dwUserID)
{
	bool ret = false;
	rwlock.wrlock();

	for (std::vector<CCaptain*>::iterator vIter=this->captains.begin(); vIter!=this->captains.end(); ++vIter)
	{
		CCaptain* pCaptain = *vIter;
		if (pCaptain->dwCharID == dwUserID)
		{
			pCaptain->fireMe();
			captains.erase(vIter);
			SAFE_DELETE(pCaptain);
			ret = true;
			break;
		}
	}

	rwlock.unlock();
	return ret;
}

void CArmy::changeName(const char* newname)
{
	rwlock.wrlock();
	strncpy(name, newname, MAX_NAMESIZE);
	rwlock.unlock();
	
	this->writeDatabase();
	this->update_all_captain();
}

void CArmy::update_all_captain()
{
	for (std::vector<CCaptain*>::iterator vIter=this->captains.begin(); vIter!=this->captains.end(); ++vIter)
	{
		CCaptain* pCaptain = *vIter;
		pCaptain->update_scene();
	}
	
	Cmd::Session::t_sendUserArmyInfo_SceneSession send;
	UserSession* pUser = UserSessionManager::getInstance()->getUserByID(this->dwGenID);
	if (pUser && pUser->scene)
	{
		strncpy(send.title, this->name, MAX_NAMESIZE);
		send.byType = 2;
		send.dwUserID = this->dwGenID;

		pUser->scene->sendCmd(&send, sizeof(send));
	}

}

bool CArmy::canAddCaptain()
{
	int cur_cap =  captains.size();
	int max_cap = 0;
	int vexploit[] = {0, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 660, 770, 880, 990, 1100, 1210,1320,1430, 
			1540,
			1700, 1860, 2020, 2180, 2340, 2500, 2660, 2820, 2980, 3100, 3220, 3340, 3460, 3580, 3700, 3820,
			3940, 4060, 4180, 4300, 4420, 4600, 4780, 4960, 5140, 5320, 5500, 5680, 5860, 7000, 8140, 9280,
			10420, 11560, 12700, 13840, 15000, 16160, 17320, 18480, 19640, 25000, 30360, 35720, 41080, 46440,
			51800, 57160, 62520, 67800};

	UserSession* pUser = UserSessionManager::getInstance()->getUserByID(this->dwGenID);

	if (pUser)
	{
		int cur_exploit = pUser->dwExploit/100;

		if (cur_exploit <100) {
			max_cap = 10;
		}
		else
		{
			for (int i=0; i<69; i++)
			{
				if (cur_exploit>=vexploit[i] && cur_exploit<vexploit[i+1])
				{
					max_cap = i*10 + 10;
					break;
				}
			}
		}

		if (cur_cap<max_cap)
			return true;
	}

	return false;
}

//------------------------CCaptain--------------------------------------------
CCaptain::CCaptain()
{
	this->dwArmyID = 0; // 军队ID
	this->dwCharID = 0; // 队长ID
	bzero(szCapName, MAX_NAMESIZE); // 队长姓名
	this->dwNpcNum = 0; // 队长带领的NPC数，暂时未用
	this->myArmy = NULL;
}

CCaptain::~CCaptain()
{
}

void CCaptain::init(Record* rec)
{
	this->dwArmyID = rec->get("armyid");
	this->dwCharID = rec->get("charid");
	this->dwNpcNum = rec->get("npcnum");
	strncpy(this->szCapName, rec->get("charname"), MAX_NAMESIZE);
}

void CCaptain::writeDatabase()
{
	//FunctionTimes times(72,__FUNCTION__);
	Record rec,where;
	std::ostringstream oss;

	oss << "armyid='" << this->dwArmyID << "'";
	where.put("armyid", oss.str());
	
	oss.str("");
	oss << "`charid`='" << this->dwCharID << "'";
	where.put("charid", oss.str());

	rec.put("npcnum", this->dwNpcNum);
	

	FieldSet* captain = SessionService::metaData->getFields("CAPTAIN");

	if (captain)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();
		
		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return;
		}
		else
		{
			SessionService::dbConnPool->exeUpdate(handle, captain, &rec, &where);
			SessionService::dbConnPool->putHandle(handle);
		}
	}
	else
	{
		Zebra::logger->error("军队详细数据保存失败，CAPTAIN表不存在");
		return;
	}	
}

bool CCaptain::insertDatabase()
{
	//FunctionTimes times(71,__FUNCTION__);
	Record rec;
	rec.put("armyid", this->dwArmyID);
	rec.put("charid", this->dwCharID);
	rec.put("charname", this->szCapName);
	rec.put("npcnum", this->dwNpcNum);

	FieldSet* captain = SessionService::metaData->getFields("CAPTAIN");

	if (captain)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{       
			Zebra::logger->error("不能获取数据库句柄");
			return false;
		}
		else
		{       
			unsigned int retcode = SessionService::dbConnPool->exeInsert(handle, captain, &rec);
			if ((DWORD)-1 == retcode)
			{
				SessionService::dbConnPool->putHandle(handle);
				return false;
			}	
			SessionService::dbConnPool->putHandle(handle);
		}
	}
	else
	{
		Zebra::logger->error("添加军队记录失败，CAPTAIN表不存在");
		return false;
	}

	return true;
}

void CCaptain::fireMe()
{
	if (this->deleteMeFromDB())
	{
		CArmyM::getMe().removeCaptain(this->dwCharID);

		UserSession* pUser = UserSessionManager::getInstance()->getUserByID(this->dwCharID);
		if (pUser && pUser->scene)
		{
			Cmd::Session::t_sendUserArmyInfo_SceneSession send;
			send.dwUserID = this->dwCharID;
			bzero(send.title, MAX_NAMESIZE);
			send.byType = 0;
			pUser->scene->sendCmd(&send, sizeof(send));
			pUser->sendSysChat(Cmd::INFO_TYPE_EXP, "你已离开军队");
		}
	}
}

bool CCaptain::deleteMeFromDB()
{
	//FunctionTimes times(70,__FUNCTION__);
	Record where;
	std::ostringstream oss;

	oss << "armyid='" << this->dwArmyID << "'";
	where.put("armyid", oss.str());
	
	oss.str("");
	oss << "`charid`='" << this->dwCharID << "'";
	where.put("charid", oss.str());
			
	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	FieldSet* captain = SessionService::metaData->getFields("CAPTAIN");
		
	if (captain)
	{
		SessionService::dbConnPool->exeDelete(handle, captain, &where);
	}

	SessionService::dbConnPool->putHandle(handle);
		
	return true;
}

void CCaptain::update_scene()
{
	Cmd::Session::t_sendUserArmyInfo_SceneSession send;
	UserSession* pUser = UserSessionManager::getInstance()->getUserByID(this->dwCharID);
	if (this->myArmy && pUser && pUser->scene)
	{
		/*SceneSession * pScene = SceneSessionManager::getInstance()->getSceneByID((this->myArmy->dwCountryID<<16) 
				+ this->myArmy->dwCityID);

		if (pScene)
		{
			strncpy(send.title , pScene->name, MAX_NAMESIZE); // 所属城市
		}*/

		strncpy(send.title, this->myArmy->name, MAX_NAMESIZE);

		if (this->dwCharID == this->myArmy->dwGenID)
		{
			send.byType = 2;
		}
		else
		{
			send.byType = 1;
		}
		send.dwUserID = this->dwCharID;

		pUser->scene->sendCmd(&send, sizeof(send));
	}
}

/*
 * $log$
 */
