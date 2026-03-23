/**
 * \file: Ally.cpp
 * \version  $Id: Ally.cpp $
 * \author   
 * \date
 * \brief 同盟国管理器实现
 *
 * 
 */

#include <sstream>
#include "zDBConnPool.h"
#include "zMetaData.h"
#include "SessionServer.h"
#include "CCountryManager.h"
#include "CUnion.h"
#include "SessionChat.h"
#include "SessionTaskManager.h"
#include "Ally.h"

CAlly::CAlly()
{
	this->dwCountryID = 0;
	this->dwAllyCountryID = 0;
	this->dwFriendDegree = 0;
	this->dwCreateTime = 0;
	this->dwLastUpTime = 0;
	this->byStatus = 2;
}

CAlly::~CAlly()
{
}

void CAlly::init(Record* rec)
{
	if (rec)
	{
		this->dwCountryID  = rec->get("countryid");
		this->dwAllyCountryID = rec->get("allycountryid");
		this->dwFriendDegree  = rec->get("frienddegree");
		this->dwCreateTime = rec->get("createtime");
		this->dwLastUpTime = rec->get("lastuptime");
	}
}

void CAlly::writeDatabase()
{
	Record rec,where;
	std::ostringstream oss;

	oss << "countryid='" << this->dwCountryID << "'";
	where.put("countryid", oss.str());
	oss.str("");

	oss << "allycountryid='" << this->dwAllyCountryID << "'";
	where.put("allycountryid", oss.str());

	rec.put("frienddegree", this->dwFriendDegree);
	rec.put("lastuptime", this->dwLastUpTime);
	FieldSet* ally = SessionService::metaData->getFields("ALLY");

	if (ally)
	{       
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return;
		}

		if ((connHandleID)-1 != handle)
		{
			SessionService::dbConnPool->exeUpdate(handle, ally, &rec, &where);
		}

		SessionService::dbConnPool->putHandle(handle);
	}
	else
	{
		Zebra::logger->error("国家联盟数据保存失败，ALLY表不存在");
		return;
	}
}

bool CAlly::insertDatabase()
{
	Record rec;
	rec.put("countryid", this->dwCountryID);
	rec.put("allycountryid", this->dwAllyCountryID);
	rec.put("createtime", this->dwCreateTime);
	rec.put("frienddegree", this->dwFriendDegree);

	FieldSet* ally = SessionService::metaData->getFields("ALLY");

	if (ally)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return false;
		}
		else
		{
			SessionService::dbConnPool->exeInsert(handle, ally, &rec);
			Zebra::logger->trace("[国家联盟]: %s, %s 建立联盟关系",
				        CCountryM::getMe().find(this->dwCountryID)->name,
				 CCountryM::getMe().find(this->dwAllyCountryID)->name);

		}

		SessionService::dbConnPool->putHandle(handle);
	}
	else
	{
		Zebra::logger->error("国家联盟数据新建失败，ALLY表不存在");
		return false;
	}

	return true;
}

bool CAlly::deleteMeFromDB()
{
	Record where;
	std::ostringstream oss;

	oss << "countryid='" << this->dwCountryID << "'";
	where.put("countryid", oss.str());
	oss.str("");

	oss << "allycountryid='" << this->dwAllyCountryID << "'";
	where.put("allycountryid", oss.str());

	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	FieldSet* ally = SessionService::metaData->getFields("ALLY");

	if (ally)
	{
		SessionService::dbConnPool->exeDelete(handle, ally, &where);
		Zebra::logger->trace("[国家联盟]: %s, %s 解除联盟关系",
				        CCountryM::getMe().find(this->dwCountryID)->name,
				 CCountryM::getMe().find(this->dwAllyCountryID)->name);

		this->refreshAlly(true);
		this->refreshAllyToAllUser(true);
	}
	else
	{
		Zebra::logger->error("国家联盟数据删除失败，ALLY表不存在");
		SessionService::dbConnPool->putHandle(handle);
		return false;
	}

	SessionService::dbConnPool->putHandle(handle);
	return true;
}

bool CAlly::changeFriendDegree(int degree)
{
	int old_degree = this->dwFriendDegree;

	if (degree<0)
	{
		if (this->dwFriendDegree>abs(degree))
		{
			this->dwFriendDegree = this->dwFriendDegree - abs(degree);
			/*if (old_degree>=10000 && this->dwFriendDegree<10000)
			{
				SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, 
				" %s 与 %s 的国家关系更改为 友善关系",
				CCountryM::getMe().find(this->dwCountryID)->name, 
				CCountryM::getMe().find(this->dwAllyCountryID)->name);
			}
			// */
		}
		else
		{
			this->dwFriendDegree = 0;
		}
	}
	else
	{
		this->dwFriendDegree += degree;
		/* if (old_degree<10000 && this->dwFriendDegree>=10000)
		{
			SessionChannel::sendAllInfo(Cmd::INFO_TYPE_GAME, 
					" %s 与 %s 的国家关系更改为 协力关系",
					CCountryM::getMe().find(this->dwCountryID)->name, 
					CCountryM::getMe().find(this->dwAllyCountryID)->name);
		}
		// */
	}

	this->writeDatabase();

	if (degree>0 && ((1000 - (old_degree%1000))<=degree))
	{
#ifdef _ZJW_DEBUG
		Zebra::logger->debug("[国家联盟]: 发出镖队");
#endif		
		SessionChannel::sendAllInfo(Cmd::INFO_TYPE_EXP, 
				" %s 的外交车队已从王城出发, %s 的外交车队已从王城出发,将经南郊前往边境",
				CCountryM::getMe().find(this->dwCountryID)->name, 
				CCountryM::getMe().find(this->dwAllyCountryID)->name);

		Cmd::Session::t_summonAllyNpc_SceneSession send;
		send.dwCountryID = this->dwCountryID;
		SessionTaskManager::getInstance().broadcastScene(&send, sizeof(send));
		
		send.dwCountryID = this->dwAllyCountryID;
		SessionTaskManager::getInstance().broadcastScene(&send, sizeof(send));
	}

	this->refreshAlly();

	return true;
}

void CAlly::refreshAlly(bool isFire)
{
	BYTE buf[zSocket::MAX_DATASIZE];
	Cmd::Session::_allyItem*  tempItem = NULL;

	Cmd::Session::t_updateAlly_SceneSession* send = (Cmd::Session::t_updateAlly_SceneSession*)buf;
	constructInPlace(send);
	tempItem = (Cmd::Session::_allyItem*)send->data;
	send->dwSize = 0;

	if (isFire)	
	{
		tempItem->dwCountryID = this->dwCountryID;
		tempItem->dwAllyCountryID = this->dwAllyCountryID;
		tempItem->dwFriendDegree =  0;
	}
	else
	{
		tempItem->dwCountryID = this->dwCountryID;
		tempItem->dwAllyCountryID = this->dwAllyCountryID;
		tempItem->dwFriendDegree =  this->friendDegree();
	}

	send->dwSize++;

	SessionTaskManager::getInstance().broadcastScene(send, send->dwSize*sizeof(Cmd::Session::_allyItem) + 
			sizeof(Cmd::Session::t_updateAlly_SceneSession));
}

void CAlly::refreshAllyToAllUser(bool isFire)
{
	Cmd::stUpdateCountryAlly send;
	
	if (isFire)
	{
		send.dwAllyCountryID = 0;
		SessionTaskManager::getInstance().sendCmdToCountry(this->dwCountryID, &send, sizeof(send));

		send.dwAllyCountryID = 0;
		SessionTaskManager::getInstance().sendCmdToCountry(this->dwAllyCountryID, &send, sizeof(send));
	}
	else
	{
		send.dwAllyCountryID = this->dwAllyCountryID;
		SessionTaskManager::getInstance().sendCmdToCountry(this->dwCountryID, &send, sizeof(send));

		send.dwAllyCountryID = this->dwCountryID;
		SessionTaskManager::getInstance().sendCmdToCountry(this->dwAllyCountryID, &send, sizeof(send));
	}
}

//------------------CAllyM----------
bool CAllyM::init()
{
	return this->loadAllyFromDB();
}

bool CAllyM::loadAllyFromDB()
{
	FieldSet* ally = SessionService::metaData->getFields("ALLY");

	if (ally)
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
			recordset = SessionService::dbConnPool->exeSelect(handle, ally, NULL, NULL);
		}

		SessionService::dbConnPool->putHandle(handle);
		if (recordset)
		{
			for (unsigned int i=0; i<recordset->size(); i++)
			{
				Record* rec = recordset->get(i);
				CAlly* pAlly = new CAlly();

				if (pAlly)
				{
					pAlly->init(rec);

					if (this->findAlly(pAlly->dwCountryID, pAlly->dwAllyCountryID) != NULL)
					{
						Zebra::logger->error("[国家联盟]: %d 与 %d 有重复的联盟数据.", 
								pAlly->dwCountryID, pAlly->dwAllyCountryID);
						SAFE_DELETE(pAlly);
					}
					else
					{
						rwlock.wrlock();
						allies.push_back(pAlly);
						rwlock.unlock();
					}
				}
			}

			SAFE_DELETE(recordset)
		}
	}
	else
	{
		Zebra::logger->error("国家联盟数据加载失败，ALLY表不存在");
		return false;
	}

	return true;

}

bool CAllyM::processUserMessage(UserSession* pUser, const Cmd::stNullUserCmd* ptNullCmd, const unsigned int cmdLen)
{
	switch (ptNullCmd->byParam)
	{
		case REQ_COUNTRY_ALLY_INFO_PARA:
			{
				Cmd::stReqCountryAllyInfo* rev = (Cmd::stReqCountryAllyInfo*)ptNullCmd;
				processReqCountryAllyInfo(pUser, rev);
				return true;
			}
			break;
		case CANCEL_COUNTRY_ALLY_PARA:
			{
				Cmd::stCancelCountryAlly* rev = (Cmd::stCancelCountryAlly*)ptNullCmd;
				processCancelCountryAlly(pUser, rev);
				return true;
			}
			break;
		case REQ_COUNTRY_ALLY_PARA:
			{
				Cmd::stReqCountryAlly* rev = (Cmd::stReqCountryAlly*)ptNullCmd;
				processReqCountryAlly(pUser, rev);
				return true;
			}
			break;
		default:
			break;
	}

	return false;
}

bool CAllyM::processSceneMessage(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen)
{
	return false;
}

void CAllyM::processReqCountryAllyInfo(UserSession* pUser, Cmd::stReqCountryAllyInfo* rev)
{
	CAlly* pAlly = this->findAlly(pUser->country);
	if (pAlly)
	{
		Cmd::stRtnCountryAllyInfo send;
		if (pAlly->dwCountryID == pUser->country)
		{
			send.dwAllyCountryID = pAlly->dwAllyCountryID;
		}
		else
		{
			send.dwAllyCountryID = pAlly->dwCountryID;
		}
		
		send.dwFriendDegree = pAlly->friendDegree();
		pUser->sendCmdToMe(&send, sizeof(send));
	}
}

void CAllyM::processReqCountryAlly(UserSession* pUser, Cmd::stReqCountryAlly* rev)
{
	switch (rev->byStatus)
	{
		case Cmd::QUESTION_COUNTRY_ALLY:
			{
				if (!CCountryM::getMe().isKing(pUser))
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL,  "你不是国王,不能使用该功能");
					return;
				}

				CCountry* pCountry = CCountryM::getMe().find(pUser->country);
				if (!pCountry) 
				{
					Zebra::logger->error("[国家联盟]:国家数据不完整, 不能使用请求结盟");
					return;
				}

				if (rev->dwAllyCountryID == pUser->country)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL,  
							"不能和自己结盟");
				}

				if ((int)pCountry->getMaterial(COUNTRY_MONEY)<CREATE_ALLY_NEED_MONEY)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL,  
							"国家金库银两不足,不能结盟");
					return;
				}

				CCountry* pAllyCountry = CCountryM::getMe().find(rev->dwAllyCountryID);
				if (!pAllyCountry) 
				{ 
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL,  
							"受邀请的国家不存在");
					return;
				}

				rev->dwCountryID = pUser->country;
				CUnion* pUnion = CUnionM::getMe().getUnionByID(pAllyCountry->dwKingUnionID);

				if (!pUnion)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL,  
							"对方国王不在线");
					return;
				}

				UserSession* pAllyKing = UserSessionManager::getInstance()->getUserByID(pUnion->tempid);
				if (!pAllyKing)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL,  
							"对方国王不在线");
					return;
				}

				if (this->findAlly(pUser->country) != NULL)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL,  
							"你已有盟国,不能再申请");
					return;
				}

				if (this->findAlly(rev->dwAllyCountryID) != NULL)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL,  
							"对方已有盟国,不能再邀请");
					return;
				}


				pAllyKing->sendCmdToMe(rev, sizeof(Cmd::stReqCountryAlly));
				this->addReadyAlly(rev->dwCountryID, rev->dwAllyCountryID);
			}
			break;
		case Cmd::YES_COUNTRY_ALLY:
			{
				if (!CCountryM::getMe().isKing(pUser))
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL,  "你不是国王,不能使用该功能");
					return;
				}

				CCountry* pAllyCountry = CCountryM::getMe().find(pUser->country);
				if (!pAllyCountry) 
				{
					Zebra::logger->error("[国家联盟]:国家数据不完整, 不能使用请求结盟");
					return;
				}

				if ((int)pAllyCountry->getMaterial(COUNTRY_MONEY)<CREATE_ALLY_NEED_MONEY)
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL,  
							"国家金库银两不足,不能结盟");
					return;
				}

				if (NULL == this->findAlly(rev->dwCountryID, rev->dwAllyCountryID))
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "结盟请求已经取消.");
					return;
				}

				CCountry* pCountry = CCountryM::getMe().find(rev->dwCountryID);
				if (pCountry)
				{
					pAllyCountry->changeMaterial(COUNTRY_MONEY, -CREATE_ALLY_NEED_MONEY);
					pCountry->changeMaterial(COUNTRY_MONEY, -CREATE_ALLY_NEED_MONEY);
					this->addNewAlly(rev->dwCountryID, rev->dwAllyCountryID);
				}
			}
			break;
		case Cmd::NO_COUNTRY_ALLY:
			{
				CCountry* pCountry = CCountryM::getMe().find(rev->dwCountryID);
				CUnion* pUnion = NULL;

				if (pCountry)
				{
					pUnion = CUnionM::getMe().getUnionByID(pCountry->dwKingUnionID);
				}

				if (pUnion)
				{
					UserSession* pKing = UserSessionManager::getInstance()->getUserByID(pUnion->tempid);
					if (pKing)
					{
						pKing->sendSysChat(Cmd::INFO_TYPE_FAIL,  
								"对方拒绝联盟");
						return;
					}
				}
			}
			break;
		default:
			break;

	}
}

void CAllyM::processCancelCountryAlly(UserSession* pUser, Cmd::stCancelCountryAlly* rev)
{
	if (!CCountryM::getMe().isKing(pUser))
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL,  "你不是国王,不能使用该功能");
		return;
	}

	if (NULL == this->findAlly(pUser->country))
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_FAIL,  "没有盟国");
		return;
	}


	this->fireAlly(pUser->country, 0);
}

void CAllyM::refreshAlly(SessionTask* scene)
{
	BYTE buf[zSocket::MAX_DATASIZE];
	Cmd::Session::_allyItem*  tempItem= NULL;

	Cmd::Session::t_updateAlly_SceneSession* send = (Cmd::Session::t_updateAlly_SceneSession*)buf;
	constructInPlace(send);
	tempItem = (Cmd::Session::_allyItem*)send->data;
	send->dwSize = 0;

	std::vector<CAlly*>::iterator pos;
	for (pos=allies.begin(); pos!=allies.end(); pos++)	
	{
		CAlly* pAlly = *pos;
		if (pAlly)
		{
			tempItem->dwCountryID = pAlly->dwCountryID;
			tempItem->dwAllyCountryID = pAlly->dwAllyCountryID;
			tempItem->dwFriendDegree =  pAlly->friendDegree();
			tempItem++;
			send->dwSize++;
		}
	}

	if (scene) scene->sendCmd(send, send->dwSize*sizeof(Cmd::Session::_allyItem) + 
			sizeof(Cmd::Session::t_updateAlly_SceneSession));
}

void CAllyM::userOnline(UserSession* pUser)
{
	Cmd::stUpdateCountryAlly send;
	
	rwlock.rdlock();
	
	for (unsigned int i=0; i<allies.size(); i++)
	{
		if (allies[i]->dwCountryID == pUser->country || allies[i]->dwAllyCountryID == pUser->country)
		{
			if (allies[i]->dwCountryID == pUser->country)
			{
				send.dwAllyCountryID = allies[i]->dwAllyCountryID;
			}
			else
			{
				send.dwAllyCountryID = allies[i]->dwCountryID;
			}
			break;
		}
	}
	
	rwlock.unlock();
	pUser->sendCmdToMe(&send, sizeof(send));
}


CAlly*  CAllyM::findAlly(DWORD dwCountryID1, DWORD dwCountryID2)
{
	CAlly* pAlly = NULL;

	rwlock.rdlock();
	for (unsigned int i=0; i<allies.size(); i++)
	{
		CAlly* pTemp = allies[i];
		if (pTemp && ((pTemp->dwCountryID == dwCountryID1 && pTemp->dwAllyCountryID == dwCountryID2)
					|| (pTemp->dwCountryID == dwCountryID2 && pTemp->dwAllyCountryID == dwCountryID1))
		   )
		{
			pAlly = pTemp;
			break;
		}
	}
	rwlock.unlock();

	return pAlly;
}

CAlly*  CAllyM::findAlly(DWORD dwCountryID1)
{
	CAlly* pAlly = NULL;

	rwlock.rdlock();
	for (unsigned int i=0; i<allies.size(); i++)
	{
		CAlly* pTemp = allies[i];
		if (pTemp && (pTemp->dwCountryID == dwCountryID1 || pTemp->dwAllyCountryID == dwCountryID1)
			)
		{
			pAlly = pTemp;
			break;
		}
	}
	rwlock.unlock();

	return pAlly;
}

void 	CAllyM::addReadyAlly(DWORD dwCountryID1, DWORD dwCountryID2)
{
	CAlly* pAlly = new CAlly();

	if (pAlly)
	{
		pAlly->dwCountryID = dwCountryID1;
		pAlly->dwAllyCountryID = dwCountryID2;
		pAlly->dwFriendDegree = 1100;
		pAlly->byStatus = 1;
		pAlly->dwCreateTime = time(NULL);
		rwlock.wrlock();
		allies.push_back(pAlly);
		rwlock.unlock();
	}
}

bool 	CAllyM::addNewAlly(DWORD dwCountryID1, DWORD dwCountryID2)
{
	CAlly* pAlly = this->findAlly(dwCountryID1, dwCountryID2);
	bool ret = false;

	rwlock.wrlock();

	if (pAlly)
	{
		if (pAlly->insertDatabase())
		{
			pAlly->byStatus = 2;
			ret = true;
			pAlly->refreshAlly();
			pAlly->refreshAllyToAllUser();
		}
	}

	rwlock.unlock();

	//TODO 通知全国联盟建立
	if (ret && pAlly)
	{
		SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP,
				pAlly->dwCountryID, "我国与 %s 正式缔结盟约。",
				CCountryM::getMe().find(pAlly->dwAllyCountryID)->name);

		SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME,
				pAlly->dwCountryID, "国王使用 20锭 库银与 %s 国结盟。",
				CCountryM::getMe().find(pAlly->dwAllyCountryID)->name);

		SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP,
				pAlly->dwAllyCountryID, "我国与 %s 国正式缔结盟约。", 
				CCountryM::getMe().find(pAlly->dwCountryID)->name);

		SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME,
				pAlly->dwAllyCountryID, "国王使用 20锭 库银与 %s 国结盟。", 
				CCountryM::getMe().find(pAlly->dwCountryID)->name);

	}

	return true;
}

bool    CAllyM::fireAlly(DWORD dwCountryID1, DWORD dwCountryID2)
{
	CAlly* pAlly = NULL;
	
	if (0 == dwCountryID2) {
		pAlly = this->findAlly(dwCountryID1);
	}
	else{
		pAlly = this->findAlly(dwCountryID1, dwCountryID2);
	}
		
	if (pAlly)
	{
		if (pAlly->deleteMeFromDB())
		{
			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP,
					pAlly->dwCountryID, "我国与 %s 国解除了盟约。",
					CCountryM::getMe().find(pAlly->dwAllyCountryID)->name);

			
			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP,
					pAlly->dwAllyCountryID, "我国与 %s 国解除了盟约。", 
					CCountryM::getMe().find(pAlly->dwCountryID)->name);

		
			this->removeAlly(dwCountryID1, dwCountryID2);
			return true;
		}
	}

	return false;
}

void CAllyM::removeAlly(DWORD dwCountryID1, DWORD dwCountryID2)
{
	std::vector<CAlly*>::iterator pos;
	rwlock.wrlock();
	for (pos = allies.begin(); pos!=allies.end(); pos++)
	{
		CAlly* pTemp = *pos;
		if (pTemp && ((pTemp->dwCountryID == dwCountryID1 && pTemp->dwAllyCountryID == dwCountryID2)
					|| (pTemp->dwCountryID == dwCountryID2 && pTemp->dwAllyCountryID == dwCountryID1)
				|| ((pTemp->dwCountryID==dwCountryID1 || pTemp->dwAllyCountryID==dwCountryID1)&&dwCountryID2==0) )
		   )
		{
			allies.erase(pos);
			SAFE_DELETE(pTemp);
			break;
		}
	}
	rwlock.unlock();
}

void CAllyM::timer()
{
	std::vector<CAlly*>::iterator pos;
	time_t ct = time(NULL);
	struct tm tv1;
	zRTime::getLocalTime(tv1, ct);
	
	for (pos = allies.begin(); pos!=allies.end(); )
	{
		CAlly* pTemp = *pos;
		if (1 == pTemp->byStatus && (ct - pTemp->dwCreateTime) > 17)
		{
#ifdef _ZJW_DEBUG
			Zebra::logger->debug("[国家联盟]: %d,%d 邀请超时,关闭", pTemp->dwCountryID, pTemp->dwAllyCountryID);
#endif			
			rwlock.wrlock();
			pos = allies.erase(pos);
			rwlock.unlock();
			SAFE_DELETE(pTemp);
		}
		else
		{
			pos++;
		}
	}
	
	if (tv1.tm_hour == 0 && (tv1.tm_min>=0 && tv1.tm_min<=4))
	{
		for (pos = allies.begin(); pos!=allies.end(); pos++)
		{
			CAlly* pTemp = *pos;
#ifdef _ZJW_DEBUG
			Zebra::logger->debug("[国家联盟]: 每日更新联盟友好度: (%d,%d) %d, ct:%u lasttime:%u", 
					pTemp->dwCountryID, 
					pTemp->dwAllyCountryID, pTemp->friendDegree(), 
					ct, pTemp->dwLastUpTime);
#endif			
			if (2 == pTemp->byStatus && ((ct - pTemp->dwLastUpTime) > ((24*60*60)-10)))
			{
				pTemp->changeFriendDegree(-500);
				pTemp->dwLastUpTime = ct;
				pTemp->writeDatabase();
			}
		}

		std::vector<CAlly*> removeAlly;

		for (pos = allies.begin(); pos!=allies.end(); pos++)
		{
			CAlly* pTemp = *pos;
			if (pTemp->friendDegree()  == 0)
			{
				removeAlly.push_back(pTemp);
			}
		}

		for (pos = removeAlly.begin(); pos!=removeAlly.end(); pos++)
		{
			CAlly* pTemp = *pos;
			CAllyM::getMe().fireAlly(pTemp->dwCountryID, pTemp->dwAllyCountryID);
		}
	}
}

