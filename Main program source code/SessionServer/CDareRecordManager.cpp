/**
 * \file
 * \version  $Id: CDareRecordManager.cpp  $
 * \author  
 * \date 
 * \brief 对战记录管理器
 *
 * 
 */

#include <sstream>
#include "CDareRecordManager.h"
#include "zDBConnPool.h"
#include "zMetaData.h"
#include "SessionServer.h"
#include "SessionCommand.h"
#include "CUnion.h"
#include "CCountryManager.h"

/**
 * \brief 对战管理器初始化
 * \return true 初始化成功 false初始化失败
 */
bool CDareRecordM::init()
{
	return this->load();
}

/**
 * \brief 从数据库中加载争夺目标记录
 * \return true 加载成功
 */
bool CDareRecordM::load()
{
	//FunctionTimes times(82,__FUNCTION__);
	FieldSet* dareRecord = SessionService::metaData->getFields("DARERECORD");
	
	if (dareRecord)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return false;
		}

		RecordSet* recordset = NULL;
		Record rec;
		rec.put("daretime", "DESC");

		if ((connHandleID)-1 != handle)
		{
			recordset = SessionService::dbConnPool->exeSelect(handle, dareRecord, NULL, NULL, &rec, 100);
		}

		SessionService::dbConnPool->putHandle(handle);

		if (recordset)
		{
			for (unsigned int i=0; i<recordset->size(); i++)
			{
				Record* rec = recordset->get(i);
				CDareRecord* pDareRecord = new CDareRecord();

				if (pDareRecord)
				{
					pDareRecord->init(rec);
				}
					
				vDareRecord.push_back(pDareRecord);
			}

			SAFE_DELETE(recordset)
		}
	}
	else
	{
		Zebra::logger->error("对战记录数据加载失败，DARERECORD表不存在");
		return false;
	}

	return true;
}

bool CDareRecordM::addNewDareRecord(DWORD dwAttCountry, DWORD dwDefCountry, DWORD dwResult)
{
	bool ret = false;
	
	rwlock.wrlock();
	CDareRecord* pDareRecord = new CDareRecord();

	if (pDareRecord)
	{
		pDareRecord->dwTime = time(NULL);
		CCountry* pAttCountry = CCountryM::getMe().find(dwAttCountry);
		CCountry* pDefCountry = CCountryM::getMe().find(dwDefCountry);
			
		pDareRecord->dwAttCountryID = dwAttCountry;
		pDareRecord->dwDefCountryID = dwDefCountry;
		pDareRecord->dwResult = dwResult;
		
		if (pAttCountry)
		{
			strncpy(pDareRecord->attKingName, pAttCountry->kingName, MAX_NAMESIZE);
		}

		if (pDefCountry)
		{
			strncpy(pDareRecord->defKingName, pDefCountry->kingName, MAX_NAMESIZE);
		}

		if (pDareRecord->insertDatabase())
		{
			vDareRecord.push_back(pDareRecord);
			ret = true;
		}
		else
		{
			SAFE_DELETE(pDareRecord);
		}
	}

	rwlock.unlock();
	
	return ret;	
}

void CDareRecordM::timer()
{
	/*struct tm tv1;
	time_t timValue = time(NULL);
	zRTime::getLocalTime(tv1, timValue);

	rwlock.wrlock();
	rwlock.unlock();*/
}

bool CDareRecordM::processUserMessage(UserSession *pUser,const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen)
{
	switch(ptNullCmd->byParam)
	{
		case Cmd::REQUEST_DARE_RECORD_PARA:
			{
				Cmd::stRequestDareRecordCmd *rev = (Cmd::stRequestDareRecordCmd *)ptNullCmd;

				if (rev->byType == Cmd::DARE_RECORD_RESULT)
				{
					BYTE buf[zSocket::MAX_DATASIZE];
					Cmd::stDareResult *tempResult = NULL;

					std::vector<CDareRecord*>::iterator vIterator;

					rwlock.rdlock();
					Cmd::stReturnDareRecordResultCmd *retCmd=(Cmd::stReturnDareRecordResultCmd *)buf;

					constructInPlace(retCmd);
					tempResult = (Cmd::stDareResult *)retCmd->dare_result;
					retCmd->dwSize = 0;

					for (vIterator = vDareRecord.begin(); vIterator!=vDareRecord.end(); vIterator++)
					{
						tempResult->dareTime = (*vIterator)->dwTime;
						tempResult->attCountry = (*vIterator)->dwAttCountryID;
						tempResult->defCountry = (*vIterator)->dwDefCountryID;
						tempResult->byResult = (*vIterator)->dwResult;
						bzero(tempResult->attKingName, sizeof(tempResult->attKingName));
						bzero(tempResult->defKingName, sizeof(tempResult->defKingName));
							
						strncpy(tempResult->attKingName, (*vIterator)->attKingName, 
								sizeof(tempResult->attKingName));
						
						strncpy(tempResult->defKingName, (*vIterator)->defKingName, 
								sizeof(tempResult->defKingName));

						tempResult++;
						retCmd->dwSize++;
					}       

					rwlock.unlock();
					
					pUser->sendCmdToMe(retCmd,
						(retCmd->dwSize*sizeof(Cmd::stDareResult)+
						 sizeof(Cmd::stReturnDareRecordResultCmd)));
				}

				return true;
			}
			break;
		default:
			break;
	}               
	return false;   
}

CDareRecord* CDareRecordM::findLastRecord(DWORD dwAttCountry, DWORD dwDefCountry)
{
	std::vector<CDareRecord*>::iterator vIterator;
	CDareRecord* pDareRecord = NULL;
	time_t cur_time = time(NULL);
	
	rwlock.rdlock();
	for (vIterator = vDareRecord.begin(); vIterator!=vDareRecord.end(); vIterator++)
	{
		CDareRecord* pTemp = (CDareRecord*)*vIterator;
		
		if (pTemp!=NULL && pTemp->dwAttCountryID == dwAttCountry
				&& pTemp->dwDefCountryID == dwDefCountry
				&& (cur_time - (time_t)pTemp->dwTime)<60*30)
		{
			pDareRecord = pTemp;
			break;
		}
	}
	rwlock.unlock();

	return pDareRecord;
}

//------------------------------------------------------------------------------------------------------------


void CDareRecord::init(Record* rec)
{
	if (rec)
	{
		this->dwAttCountryID = rec->get("attcountry");
		this->dwDefCountryID  = rec->get("defcountry");
		this->dwResult = rec->get("result");
		this->dwTime   = rec->get("daretime");
		
		if (strlen(rec->get("attkingname"))>0)
		{
			strncpy(this->attKingName, (const char*)rec->get("attkingname"), MAX_NAMESIZE);
		}

		if (strlen(rec->get("defkingname"))>0)
		{
			strncpy(this->defKingName, (const char*)rec->get("defkingname"), MAX_NAMESIZE);
		}

	}
}

bool CDareRecord::insertDatabase()
{
	//FunctionTimes times(81,__FUNCTION__);
	Record rec;
	rec.put("daretime", this->dwTime);
	rec.put("attcountry", this->dwAttCountryID);
	rec.put("defcountry", this->dwDefCountryID);
	rec.put("result", this->dwResult);
	rec.put("attkingname", this->attKingName);
	rec.put("defkingname", this->defKingName);
		
	FieldSet* dareRecord = SessionService::metaData->getFields("DARERECORD");

	if (dareRecord)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return false;
		}
		else
		{
			SessionService::dbConnPool->exeInsert(handle, dareRecord, &rec);
		}

		SessionService::dbConnPool->putHandle(handle);
	}
	else
	{
		Zebra::logger->error("城市数据加载失败，CITY表不存在");
		return false;
	}

	return true;
}

