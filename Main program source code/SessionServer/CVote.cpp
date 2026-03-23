/**
 * \file
 * \version  $Id: CVote.cpp  $
 * \author   
 * \date 
 * \brief 投票管理系统实现
 *
 * 
 */
#include <sstream>
#include <memory>
#include "zDBConnPool.h"
#include "zMetaData.h"
#include "zTime.h"
#include "SessionServer.h"
#include "Command.h"
#include "CVote.h"
#include "CCountryManager.h"

CVoteM::CVoteM()
{
}

CVoteM::~CVoteM()
{
}

bool CVoteM::init()
{
	return this->load();
}

void CVoteM::timer()
{
	std::vector<DWORD> vRemoveList;
	
	rwlock.rdlock();
	
	for (DWORD i=0; i<votes.size(); i++)
	{
		if (votes[i])
		{
			if (votes[i]->getState()==CVote::VOTE_ACTIVE && !votes[i]->isActiveState())
			{
				votes[i]->setReadyOverState();
			}

			if (votes[i]->getState()==CVote::VOTE_OVER)
			{
				vRemoveList.push_back(votes[i]->dwID);
			}
		}
	}
	rwlock.unlock();

	for (std::vector<DWORD>::iterator vIter=vRemoveList.begin(); vIter!=vRemoveList.end(); vIter++)
	{
		this->removeVoteByID(*vIter);
	}
}

void CVoteM::destroyMe()
{
	for (unsigned int i=0; i<CVoteM::getMe().votes.size(); i++)
	{       
		CVoteM::getMe().votes[i]->writeDatabase();
	}

	delMe();
}

bool CVoteM::load()
{
	//FunctionTimes times(66,__FUNCTION__);
	FieldSet* vote = SessionService::metaData->getFields("VOTE");
	
	if (vote)
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
			recordset = SessionService::dbConnPool->exeSelect(handle, vote, NULL, NULL);
		}

		SessionService::dbConnPool->putHandle(handle);

		if (recordset)
		{
			for (unsigned int i=0; i<recordset->size(); i++)
			{
				Record* rec = recordset->get(i);
				CVote* pVote = new CVote();

				if (pVote)
				{
					pVote->init(rec);
					pVote->loadItemFromDB();
				}
					
				votes.push_back(pVote);
			}

			SAFE_DELETE(recordset)
		}
	}
	else
	{
		Zebra::logger->error("国家数据加载失败，VOTE表不存在");
		return false;
	}

	return true;
}

bool CVoteM::processUserMessage(UserSession *pUser,const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen)
{
	switch (ptNullCmd->byParam) 
	{
		case REQUEST_VOTE_LIST_PARA:
			{
				Cmd::stRequestVoteListUserCmd* rev = (Cmd::stRequestVoteListUserCmd*)ptNullCmd;
				CVote* pVote = this->find(pUser->country, (DWORD)rev->byType);
				
				if (pVote && pVote->getState()==CVote::VOTE_ACTIVE)
				{
					BYTE buf[zSocket::MAX_DATASIZE];
					Cmd::stVoteItem *tempPoint;

					Cmd::stReturnVoteListUserCmd *retCmd=(Cmd::stReturnVoteListUserCmd*)buf;
					constructInPlace(retCmd);
					tempPoint = (Cmd::stVoteItem *)retCmd->data;

					pVote->rwlock.rdlock();
					retCmd->byType = pVote->dwType;
					retCmd->dwVoteID = pVote->dwID;
						
					for (DWORD i=0; i<pVote->items.size(); i++)
					{
						if (pVote->items[i])
						{
							tempPoint->dwOption = pVote->items[i]->dwOption;
							tempPoint->dwBallot = pVote->items[i]->dwBallot;
							strncpy(tempPoint->szOptionDesc, pVote->items[i]->szOptionDesc,
									MAX_NAMESIZE);

							tempPoint++;
							retCmd->dwSize++;
						}
					}
					pVote->rwlock.unlock();

					pUser->sendCmdToMe(retCmd, (retCmd->dwSize*sizeof(Cmd::stVoteItem)+
								sizeof(Cmd::stReturnVoteListUserCmd)));
				}
				else
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "当前没有任何投票");
				}
			}
			break;
		case COMMIT_VOTE_PARA:
			{
				Cmd::stCommitVoteUserCmd* rev = (Cmd::stCommitVoteUserCmd*)ptNullCmd;
				CVote* pVote = this->findByID(rev->dwVoteID);
				if (pVote)
				{
					pVote->vote(pUser, rev->dwOption);
				}
				else
				{
					pUser->sendSysChat(Cmd::INFO_TYPE_FAIL, "该投票已无效");
				}
			}
			break;
		default:
			break;
	}

	return false;
}

CVote* CVoteM::findByID(DWORD dwVoteID)
{
	CVote* pVote = NULL;
	rwlock.rdlock();
	
	for (std::vector<CVote*>::iterator vIter=votes.begin(); vIter!=votes.end(); vIter++)
	{
		CVote* temp = *vIter;
		if (temp->dwID == dwVoteID)
		{
			pVote = temp;
			break;
		}
	}
		
	rwlock.unlock();
	return pVote;

}

CVote* CVoteM::find(DWORD dwCountryID, DWORD dwType)
{
	CVote* pVote = NULL;

	rwlock.rdlock();
	
	for (std::vector<CVote*>::iterator vIter=votes.begin(); vIter!=votes.end(); vIter++)
	{
		CVote* temp = *vIter;
		if (temp->dwCountryID==dwCountryID && temp->dwType==dwType)
		{
			pVote = temp;
			break;
		}
	}
		
	rwlock.unlock();
	return pVote;
}

bool CVoteM::createNewVote(DWORD dwCountry, DWORD dwType, std::vector<CTech*>& items)
{
	if (this->find(dwCountry, dwType) != NULL)
	{
		return false;
	}
	
	CVote* pVote = new CVote();
	pVote->dwCountryID = dwCountry;
	pVote->dwType = dwType;
	if (!pVote->insertDatabase())
	{
		SAFE_DELETE(pVote);
		return false;
	}
	
	if (!pVote->loadItemFromVec(items))
	{
		SAFE_DELETE(pVote);
		return false;
	}

	rwlock.wrlock();
	votes.push_back(pVote);
	rwlock.unlock();

	pVote->setActiveState();
	
	return true;
}

void CVoteM::removeVoteByID(DWORD dwID)
{
	std::vector<CVote*>::iterator pos;
	rwlock.wrlock();
	for (pos = votes.begin(); pos!=votes.end(); pos++)
	{
		CVote* pVote = *pos;
		if (pVote->dwID == dwID)
		{
			pVote->deleteMeFromDB();
			votes.erase(pos);
			SAFE_DELETE(pVote);
			break;
		}
	}
	rwlock.unlock();
}

void CVoteM::removeVote(DWORD dwCountryID, DWORD dwType)
{
	std::vector<CVote*>::iterator pos;
	rwlock.wrlock();
	for (pos = votes.begin(); pos!=votes.end(); pos++)
	{
		CVote* pVote = *pos;
		if (pVote->dwCountryID==dwCountryID && pVote->dwType == dwType)
		{
			pVote->deleteMeFromDB();
			votes.erase(pos);
			SAFE_DELETE(pVote);
			break;
		}
	}
	rwlock.unlock();
}

void CVoteM::force_close_vote(DWORD dwCountryID, DWORD dwType)
{
	CVote* pVote = CVoteM::getMe().find(dwCountryID, dwType);
	if (pVote)
	{
		pVote->setReadyOverState();
	}

	this->removeVote(dwCountryID, dwType);
}
	
//------------------------CVote--------------------------------------------
CVote::CVote()
{
	this->dwID = 0;
	this->dwCountryID = 0;
	this->dwType = 0;
	this->dwStatus = 0;
}

CVote::~CVote()
{
	for (std::vector<CVoteItem*>::iterator pos = items.begin(); pos!=items.end(); pos++)
	{
		SAFE_DELETE(*pos);
	}
}

void CVote::init(Record* rec)
{
	rwlock.wrlock();
	this->dwID = rec->get("id");
	this->dwCountryID = rec->get("countryid");
	this->dwType = rec->get("type");
	this->dwStatus = rec->get("status");

	if (this->dwStatus==CVote::VOTE_READY) {
		this->dwStatus=CVote::VOTE_ACTIVE;
	}
	rwlock.unlock();
	this->writeDatabase();
}

void CVote::writeDatabase()
{
	//FunctionTimes times(65,__FUNCTION__);
	Record rec,where;
	std::ostringstream oss;

	oss << "id='" << this->dwID << "'";
	where.put("id", oss.str());
	
	rec.put("status", this->dwStatus);
	rec.put("type", this->dwType);

	FieldSet* vote = SessionService::metaData->getFields("VOTE");

	if (vote)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();
		
		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return;
		}

		if ((connHandleID)-1 != handle)
		{
			SessionService::dbConnPool->exeUpdate(handle, vote, &rec, &where);
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
		Zebra::logger->error("投票数据保存失败，VOTE表不存在");
		return;
	}	
}

bool CVote::insertDatabase()
{
	//FunctionTimes times(64,__FUNCTION__);
	Record rec;
	rec.put("countryid", this->dwCountryID);
	rec.put("type", this->dwType);
	rec.put("status", this->dwStatus);

	FieldSet* vote = SessionService::metaData->getFields("VOTE");

	if (vote)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{       
			Zebra::logger->error("不能获取数据库句柄");
			return false;
		}
		else
		{       
			unsigned int retcode = SessionService::dbConnPool->exeInsert(handle, vote, &rec);
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
		Zebra::logger->error("添加投票记录失败，VOTE表不存在");
		return false;
	}

	return true;
}

bool CVote::deleteMeFromDB()
{
	//FunctionTimes times(63,__FUNCTION__);
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

	FieldSet* vote = SessionService::metaData->getFields("VOTE");
	
	if (vote)
	{
		SessionService::dbConnPool->exeDelete(handle, vote, &where);
	}

	SessionService::dbConnPool->putHandle(handle);

	rwlock.wrlock();	
	for (std::vector<CVoteItem*>::iterator vIter=items.begin(); vIter!=items.end(); vIter++)
	{
		CVoteItem* temp = *vIter;
		temp->deleteMeFromDB();
		SAFE_DELETE(temp);
	}
	items.clear();
	rwlock.unlock();
	
	return true;
}

bool CVote::loadItemFromDB()
{
	//FunctionTimes times(62,__FUNCTION__);
	FieldSet* voteitem = SessionService::metaData->getFields("VOTEITEM");
	
	if (voteitem)
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

		oss << "voteid='" << this->dwID << "'";
		where.put("voteid", oss.str());

		if ((connHandleID)-1 != handle)
		{
			recordset = SessionService::dbConnPool->exeSelect(handle, voteitem, NULL, &where);
		}

		SessionService::dbConnPool->putHandle(handle);

		if (recordset)
		{
			for (unsigned int i=0; i<recordset->size(); i++)
			{
				Record* rec = recordset->get(i);
				CVoteItem* pVoteItem = new CVoteItem();

				if (pVoteItem)
				{
					pVoteItem->init(rec);
				}
					
				items.push_back(pVoteItem);
			}

			SAFE_DELETE(recordset)
		}
	}
	else
	{
		Zebra::logger->error("投票详细数据加载失败，VOTEITEM表不存在");
		return false;
	}

	return true;
}

bool CVote::loadItemFromVec(std::vector<CTech*>& itemset)
{
	for (DWORD i=0; i<itemset.size(); i++)
	{
		CVoteItem* pVoteItem = new CVoteItem();
		pVoteItem->dwOption = itemset[i]->dwType;
		strncpy(pVoteItem->szOptionDesc, itemset[i]->szName, MAX_NAMESIZE);
		pVoteItem->dwVoteID = this->dwID;
		pVoteItem->dwBallot = 0;
		if (!pVoteItem->insertDatabase())
		{
			SAFE_DELETE(pVoteItem);
			return false;
		}
		items.push_back(pVoteItem);
	}

	return true;
}

void CVote::setReadyState()
{
	rwlock.wrlock();
	this->dwStatus = CVote::VOTE_READY;
	rwlock.unlock();
	Zebra::logger->trace("[投票]:(%d,%d) VOTE_READY", this->dwCountryID, this->dwType);
}

void CVote::setActiveState()
{
	rwlock.wrlock();
	this->dwStatus = CVote::VOTE_ACTIVE;
	rwlock.unlock();
	Zebra::logger->trace("[投票]:(%d,%d) VOTE_ACTIVE", this->dwCountryID, this->dwType);
	this->writeDatabase();
}

bool lessBallot(const CVoteItem* p1, const CVoteItem* p2)
{
	return p1->dwBallot > p2->dwBallot;
}  

void CVote::setReadyOverState()
{
	rwlock.wrlock();
	this->dwStatus = CVote::VOTE_READY_OVER;
	rwlock.unlock();
	Zebra::logger->trace("[投票]:(%d,%d) VOTE_READY", this->dwCountryID, this->dwType);

	this->clearVoted();
	// TODO,根据选项前五位的排名，更新国家科技状态
	std::sort(items.begin(), items.end(), lessBallot);
	CCountry* pCountry = CCountryM::getMe().find(this->dwCountryID);
	
	if (pCountry)
	{
		for (DWORD i=0; i<items.size(); i++)
		{
			if (i<5 && items[i])
			{
				if (pCountry->getTech(items[i]->dwOption))
				{
					pCountry->getTech(items[i]->dwOption)->state(CTech::WAIT_TECH);
				}
			}
			else
				break;
		}
	}
	
	this->setOverState();
}

void CVote::setOverState()
{
	rwlock.wrlock();
	this->dwStatus = CVote::VOTE_OVER;
	rwlock.unlock();
	Zebra::logger->trace("[投票]:(%d,%d) VOTE_OVER", this->dwCountryID, this->dwType);
	this->writeDatabase();
}

bool CVote::isActiveState()
{
	zTime cur_time;
		
	if (cur_time.getWDay()>=1 && cur_time.getWDay()<=3)
	{
		return true;
	}

	return false;
}

void CVote::vote(UserSession* pUser, DWORD dwOption)
{
	if (this->addVoted(pUser->id))
	{
		for (DWORD i=0; i<items.size(); i++)
		{
			if (items[i] && items[i]->dwOption == dwOption)
			{
				items[i]->dwBallot++;
				items[i]->writeDatabase();
				pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "投票成功");
				break;
			}
		}
	}
	else
	{
		pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "不能重复投票");
	}
}	

bool CVote::addVoted(DWORD dwCharID)
{
	//FunctionTimes times(61,__FUNCTION__);
 	Record rec;
        rec.put("voteid", this->dwID);
	rec.put("charid", dwCharID);

	FieldSet* voted = SessionService::metaData->getFields("VOTEDPLAYER");

	if (voted)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{       
			Zebra::logger->error("不能获取数据库句柄");
			return false;
		}
		else
		{       
			unsigned int retcode = SessionService::dbConnPool->exeInsert(handle, voted, &rec);
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
		Zebra::logger->error("添加玩家投票记录失败，VOTEDPLAYER表不存在");
		return false;
	}

	return true;
}

bool CVote::clearVoted()
{
	//FunctionTimes times(60,__FUNCTION__);
	Record where;
	std::ostringstream oss;
	oss << "voteid='" << this->dwID << "'";
	where.put("voteid", oss.str());

	FieldSet* voted = SessionService::metaData->getFields("VOTEDPLAYER");
		
	if (voted)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();
		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return false;
		}
		else
		{
			unsigned int retcode = SessionService::dbConnPool->exeDelete(handle, voted, &where);
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
		Zebra::logger->error("清除玩家投票记录失败，VOTEDPLAYER表不存在");
		return false;
	}

	return true;
}

//------------------------CVoteItem--------------------------------------------
CVoteItem::CVoteItem()
{
	dwOption = 0;
	bzero(szOptionDesc, sizeof(szOptionDesc));
	dwBallot = 0;
}

CVoteItem::~CVoteItem()
{
}

void CVoteItem::init(Record* rec)
{
	this->dwOption = rec->get("optionid");
	this->dwBallot = rec->get("ballot");
	this->dwVoteID = rec->get("voteid");
	strncpy(this->szOptionDesc, rec->get("optiondesc"), MAX_NAMESIZE);
}

void CVoteItem::writeDatabase()
{
	//FunctionTimes times(59,__FUNCTION__);
	Record rec,where;
	std::ostringstream oss;

	oss << "voteid='" << this->dwVoteID << "'";
	where.put("voteid", oss.str());
	
	oss.str("");
	oss << "`optionid`='" << this->dwOption << "'";
	where.put("optionid", oss.str());

	rec.put("ballot", this->dwBallot);

	FieldSet* voteitem = SessionService::metaData->getFields("VOTEITEM");

	if (voteitem)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();
		
		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return;
		}
		else
		{
			SessionService::dbConnPool->exeUpdate(handle, voteitem, &rec, &where);
			SessionService::dbConnPool->putHandle(handle);
		}
	}
	else
	{
		Zebra::logger->error("投票详细数据保存失败，VOTEITEM表不存在");
		return;
	}	
}

bool CVoteItem::insertDatabase()
{
	//FunctionTimes times(58,__FUNCTION__);
	Record rec;
        rec.put("voteid", this->dwVoteID);
	rec.put("optionid", this->dwOption);
	rec.put("ballot", this->dwBallot);
	rec.put("optiondesc", this->szOptionDesc);

	FieldSet* voteitem = SessionService::metaData->getFields("VOTEITEM");

	if (voteitem)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{       
			Zebra::logger->error("不能获取数据库句柄");
			return false;
		}
		else
		{       
			unsigned int retcode = SessionService::dbConnPool->exeInsert(handle, voteitem, &rec);
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
		Zebra::logger->error("添加投票记录失败，VOTEITEM表不存在");
		return false;
	}

	return true;
}

bool CVoteItem::deleteMeFromDB()
{
	//FunctionTimes times(57,__FUNCTION__);
	Record where;
	std::ostringstream oss;
	oss << "voteid='" << this->dwVoteID << "'";
	where.put("voteid", oss.str());

	//oss.str("");
	//oss << "`optionid`='" << this->dwOption << "'";
	//where.put("optionid", oss.str());
		
	connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return false;
	}

	FieldSet* voteitem = SessionService::metaData->getFields("VOTEITEM");
		
	if (voteitem)
	{
		SessionService::dbConnPool->exeDelete(handle, voteitem, &where);
	}

	SessionService::dbConnPool->putHandle(handle);
	return true;
}

