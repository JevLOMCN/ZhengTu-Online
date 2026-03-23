#include "CHero.h"
#include "zDBConnPool.h"
#include "SessionServer.h"
#include "SessionTaskManager.h"

CHero *CHero::um(NULL);

CHero& CHero::getMe()
{
	if (!um)
		um = new CHero();
	return *um;
}

void CHero::addHeroKill(const Cmd::t_NullCmd *ptNullCmd, const unsigned int cmdLen)
{
    Cmd::Session::t_heorKill_SceneSession * rev = (Cmd::Session::t_heorKill_SceneSession *)ptNullCmd;
	Zebra::logger->debug("名称:%s,ID:%d,国家:%d,击杀ID:%d",rev->name,rev->charID,rev->country,rev->kCharID);
	time_t timValue = time(NULL);
	if(allKillTime.size() > 0)
	{
		if(allKillTime.count(rev->charID))
		{
			if (allKillTime[rev->charID].count(rev->kCharID))
			{
				double diff_t = difftime(timValue, allKillTime[rev->charID][rev->kCharID]);
				if (diff_t < 60)
				{
					// 1分钟内不记录
					Zebra::logger->debug("1分钟内不记录");
					return;
				}
			}
		}
	}
	allKillTime[rev->charID][rev->kCharID] = timValue;
	

	/* std::map<DWORD,std::map<DWORD,time_t> >::iterator iter;
	Zebra::logger->debug("11111");
	iter = allKillTime.find(rev->charID);
	Zebra::logger->debug("22222");
	if (iter != allKillTime.end())
	{
		std::map<DWORD,time_t>::iterator iter2 = iter->second.find(rev->kCharID);
		if (iter2 != iter->second.end())
		{
			double diff_t = difftime(timValue, iter2->second);
			if (diff_t < 60)
			{
				// 1分钟内不记录
				Zebra::logger->debug("1分钟内不记录");
				return;

			}
		}
		else
		{
			iter->second.insert(std::pair<DWORD,time_t>(rev->kCharID,timValue));
		}
	}
	else
	{
		allKillTime.insert(std::pair<DWORD,std::map<DWORD,time_t> >(rev->charID,std::map<DWORD,time_t>()));
	}
	allKillTime[rev->charID].insert(std::pair<DWORD,time_t>(rev->kCharID,timValue)); */

    FieldSet* fs = SessionService::metaData->getFields("HEROLIST");
	if (fs)
    {
        connHandleID handle = SessionService::dbConnPool->getHandle();
		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return;
		}
        RecordSet* recordset = NULL;
		Record rec,where;   
		std::ostringstream oss; 

		oss << "COUNTRYID='" << rev->country << "'";
		where.put("COUNTRYID", oss.str());
		oss.str("");
		oss << "CHARID='" << rev->charID << "'";
		where.put("CHARID", oss.str());
		oss.str("");
		oss << "DATEVALUE='1'";
		where.put("DATEVALUE", oss.str());
		oss.str("");

		if ((connHandleID)-1 != handle)
		{
			recordset = SessionService::dbConnPool->exeSelect(handle, fs, NULL, &where);
		}

		if (recordset)
		{
			Record* recnum = recordset->get(0);
			DWORD num = recnum->get("NUM");
			rec.put("NUM",num+1);
			
			if ((connHandleID)-1 != handle)
			{
				SessionService::dbConnPool->exeUpdate(handle, fs, &rec, &where);
			}
		}
		else
		{
			rec.put("COUNTRYID",rev->country);
			rec.put("DATEVALUE",1);
			rec.put("CHARID",rev->charID);
			rec.put("NUM",1);
			rec.put("CHARNAME",rev->name);
			SessionService::dbConnPool->exeInsert(handle, fs, &rec);
		}

		SessionService::dbConnPool->putHandle(handle);
    }


    /* static const dbCol herolist_define[] = {
		{ "`COUNTRYID`", zDBConnPool::DB_WORD, sizeof(WORD) }, 
        { "`DATEVALUE`", zDBConnPool::DB_WORD, sizeof(WORD) }, 
        { "`CHARID`", zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
        { "`NUM`", zDBConnPool::DB_DWORD,	sizeof(DWORD) }, 
		{ "CHARNAME",		zDBConnPool::DB_STR,	sizeof(char[MAX_NAMESIZE+1]) }, 
		{ NULL, 0, 0}
	};
	struct {
		WORD  countryid;
        WORD datevalue;
        DWORD charid;
        DWORD num;
		char  charname[MAX_NAMESIZE+1];
	} __attribute__ ((packed))
	updateseptmember_data;
	char where[128];
    connHandleID handle = SessionService::dbConnPool->getHandle();
	if ((connHandleID)-1 == handle)
	{
		Zebra::logger->error("不能获取数据库句柄");
		return;
	}
	bzero(&updateseptmember_data,sizeof(updateseptmember_data));
    updateseptmember_data.countryid = rev->country;
    updateseptmember_data.datevalue = 1;
    updateseptmember_data.charid = rev->charID;
    updateseptmember_data.num = 1;
    strncpy(updateseptmember_data.charname, rev->name, MAX_NAMESIZE);

    bzero(where, sizeof(where));
	snprintf(where, sizeof(where) - 1, "COUNTRYID = '%u' AND CHARID = '%u'", updateseptmember_data.countryid, updateseptmember_data.charid);

    //unsigned int retcode = SessionService::dbConnPool->exeInsert(handle, "`HEROLIST`", herolist_define, (const unsigned char *)(&updateseptmember_data));
    unsigned int retcode = SessionService::dbConnPool->exeUpdate(handle, "`HEROLIST`", herolist_define, (BYTE *)(&updateseptmember_data), where);
	SessionService::dbConnPool->putHandle(handle);
	if ((unsigned int)-1 == retcode)
	{
		Zebra::logger->error("插入护国榜数据库出错 ");
	} */
    
}

RecordSet* CHero::queryHeroKill(DWORD countryid,DWORD datevalue)
{
	FieldSet* fs = SessionService::metaData->getFields("HEROLIST");
	if (fs)
    {
        connHandleID handle = SessionService::dbConnPool->getHandle();
		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return NULL;
		}
        RecordSet* recordset = NULL;
		Record rec,where,order;   
		std::ostringstream oss; 

		oss << "COUNTRYID='" << countryid << "'";
		where.put("COUNTRYID", oss.str());
		oss.str("");
		oss << "DATEVALUE='" << datevalue << "'";
		where.put("DATEVALUE", oss.str());
		oss.str("");

		order.put("NUM", "DESC");
		if ((connHandleID)-1 != handle)
		{
			recordset = SessionService::dbConnPool->exeSelect(handle, fs, NULL, &where, &order, 20);
		}

		SessionService::dbConnPool->putHandle(handle);
		if (recordset)
			return recordset;
		else
			return NULL;

    }
}

void CHero::updateDayHeroList()
{
	Zebra::logger->debug("更新英雄排行榜");
	FieldSet* fs = SessionService::metaData->getFields("HEROLIST");
	if (fs)
    {
        connHandleID handle = SessionService::dbConnPool->getHandle();
		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return;
		}

		Record rec,where;   
		std::ostringstream oss; 

		oss << "DATEVALUE='0'";
		where.put("DATEVALUE", oss.str());

		if ((connHandleID)-1 != handle)
			SessionService::dbConnPool->exeDelete(handle, fs, &where);

		where.clear();
		oss.str("");
		oss << "DATEVALUE='1'";
		where.put("DATEVALUE", oss.str());

		rec.put("DATEVALUE",0);
		if ((connHandleID)-1 != handle)
			SessionService::dbConnPool->exeUpdate(handle, fs, &rec, &where);

		SessionService::dbConnPool->putHandle(handle);
    }
}

void CHero::sendHeroRewards()
{
	FieldSet* fs = SessionService::metaData->getFields("HEROLIST");
	if (fs)
    {
        connHandleID handle = SessionService::dbConnPool->getHandle();
		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return;
		}
		Record rec,where,order;   
		std::ostringstream oss; 

		for (int counid = 2; counid < 13; counid++)
		{
			oss << "COUNTRYID='" << counid << "'";
			where.put("COUNTRYID", oss.str());
			oss.str("");
			oss << "DATEVALUE='0'";
			where.put("DATEVALUE", oss.str());
			oss.str("");

			order.put("NUM", "DESC");
			RecordSet* recordset = NULL;
			if ((connHandleID)-1 != handle)
				recordset = SessionService::dbConnPool->exeSelect(handle, fs, NULL, &where, &order, 3);
			if (recordset)
			{
				BUFFER_CMD(Cmd::Session::t_sendHeroliRankReward ,pSend , zSocket::MAX_USERDATASIZE);
				pSend->len = 0;
				for (int i = 0; i < recordset->size(); i++)
				{
					Record* recnum = recordset->get(i);
					DWORD charid = recnum->get("CHARID");
					bcopy(recnum->get("CHARNAME"), (pSend->heroName + pSend->len)->name, MAX_NAMESIZE );
					(pSend->heroName + pSend->len)->charID = charid;
					pSend->len ++;
				}
				SessionTaskManager::getInstance().broadcastScene(pSend, sizeof(Cmd::Session::t_sendHeroliRankReward) + pSend->len * sizeof(pSend->heroName[0]));
			}
		}
		
	
		SessionService::dbConnPool->putHandle(handle);
		/* if (recordset)
			return recordset;
		else
			return NULL; */

    }
}
