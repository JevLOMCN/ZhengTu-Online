/**
 * \file
 * \version  $Id: CCityManager.cpp  $
 * \author  
 * \date 
 * \brief 城市管理器
 *
 * 
 */

#include <sstream>
#include "CCityManager.h"
#include "zDBConnPool.h"
#include "zMetaData.h"
#include "SessionServer.h"
#include "SessionCommand.h"
#include "CUnion.h"
#include "SessionChat.h"
#include "CDare.h"

/**
 * \brief 城市管理器初始化
 * \return true 初始化成功 false初始化失败
 */
bool CCityM::init()
{
	return this->load();
}

/**
 * \brief 析构管理器
 */
void CCityM::destroyMe()
{
	for (unsigned int i=0; i<CCityM::getMe().citys.size(); i++)
	{
		CCityM::getMe().citys[i]->writeDatabase();
	}
}

CCityM::CCityM()
{
	this->isBeging = false;	
}

/**
 * \brief 从数据库中加载争夺目标记录
 * \return true 加载成功
 */
bool CCityM::load()
{
	//FunctionTimes times(80,__FUNCTION__);
	FieldSet* city = SessionService::metaData->getFields("CITY");

	if (city)
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
			recordset = SessionService::dbConnPool->exeSelect(handle, city, NULL, NULL);
		}

		SessionService::dbConnPool->putHandle(handle);

		if (recordset)
		{
			for (unsigned int i=0; i<recordset->size(); i++)
			{
				Record* rec = recordset->get(i);


				CCity* pCity = new CCity();

				if (pCity)
				{
					pCity->init(rec);

					if (CUnionM::getMe().getUnionByID(pCity->dwUnionID) == NULL)
					{//如果拥有者
						pCity->dwUnionID = 0;
						pCity->isAward = 0;
						pCity->dwGold = 50000; 
						Zebra::logger->trace("帮会已不存在，城市归属清空");
					}
				}


				citys.push_back(pCity);
			}

			SAFE_DELETE(recordset)
		}
	}
	else
	{
		Zebra::logger->error("城市数据加载失败，CITY表不存在");
		return false;
	}

	return true;
}

bool CCityM::addNewCity(Cmd::Session::t_UnionCity_Dare_SceneSession* pCmd)
{
	bool ret = false;

	CCity* pCity = new CCity();

	if (pCity)
	{
		pCity->dwCountry = pCmd->dwToCountryID;
		if (pCmd->dwCityID > 1000)
		{
			pCity->dwCityID = pCmd->dwCityID/10;
		}
		else
		{
			pCity->dwCityID = pCmd->dwCityID;
		}
		
		if (pCity->dwCountry == PUBLIC_COUNTRY || pCity->dwCityID == KING_CITY_ID) 
		{
			pCity->dwGold = 50000; // 王城5锭 中立城市5锭
		}
		else
		{
			pCity->dwGold = 20000; // 2锭
		}

		pCity->dwUnionID = pCmd->dwFromUnionID;

		if (pCity->insertDatabase())
		{
			rwlock.wrlock();
			citys.push_back(pCity);
			rwlock.unlock();
			this->refreshUnion(pCity->dwCountry, pCity->dwCityID);
			ret = true;
		}
		else
		{
			SAFE_DELETE(pCity);
		}
	}


	return ret;	
}

CCity* CCityM::find(DWORD country, DWORD cityid, DWORD unionid)
{
	CCity* pCity = NULL;

	if (!country || !cityid || !unionid)
	{
		return NULL;
	}

	rwlock.rdlock();
	for (unsigned int i=0; i<citys.size(); i++)
	{
		if (citys[i]->dwCountry==country && citys[i]->dwCityID==cityid && citys[i]->dwUnionID==unionid)
		{
			pCity = citys[i];
			break;
		}
	}
	rwlock.unlock();

	return pCity;
}

CCity* CCityM::find(DWORD country, DWORD cityid)
{
	CCity* pCity = NULL;

	if (!country || !cityid)
		return NULL;

	rwlock.rdlock();
	for (unsigned int i=0; i<citys.size(); i++)
	{
		if (citys[i]->dwCountry==country && citys[i]->dwCityID==cityid)
		{
			pCity = citys[i];
			break;
		}
	}
	rwlock.unlock();

	return pCity;
}


CCity* CCityM::findByUnionID(DWORD unionid)
{
	CCity* pCity = NULL;

	if (!unionid)
		return NULL;

	rwlock.rdlock();
	for (unsigned int i=0; i<citys.size(); i++)
	{
		if (citys[i]->dwUnionID==unionid)
		{
			pCity = citys[i];
			break;
		}
	}
	rwlock.unlock();

	return pCity;

}

CCity* CCityM::findDareUnionByID(DWORD unionid)
{
	CCity* pCity = NULL;

	if (!unionid)
		return NULL;

	rwlock.rdlock();
	for (unsigned int i=0; i<citys.size(); i++)
	{
		if (citys[i]->isDare(unionid))
		{
			pCity = citys[i];
			break;
		}
	}
	rwlock.unlock();

	return pCity;
}

void CCityM::timer()
{
	struct tm tv1;
	time_t timValue = time(NULL);
	zRTime::getLocalTime(tv1, timValue);
	static int last_fulltime = 0;
	std::vector<CCity*>::iterator vIterator;
	for (unsigned int i=0; i< citys.size(); i++)
	{
		//if (tv1.tm_hour >21 && tv1.tm_min>30)
		if ((timValue-citys[i]->isAward) > 24*60*60)
		{//当天的钱被刷新后，置为true
			if (citys[i]->dwCountry == PUBLIC_COUNTRY || citys[i]->dwCityID == KING_CITY_ID) 
			{
				citys[i]->dwGold = 50000; // 王城5锭 中立城市5锭
			}
			else
			{
				citys[i]->dwGold = 20000; // 2锭
			}
			
			citys[i]->isAward = timValue;
			citys[i]->writeDatabase();
		}
	}

	if (tv1.tm_hour>last_fulltime && tv1.tm_min == 0)
	{// 整点发送通知
		rwlock.rdlock();
		for(vIterator = citys.begin(); vIterator!=citys.end(); vIterator++)
		{
			if ((*vIterator)->dareSize()>0)
			{
				CUnionM::getMe().sendUnionNotify((*vIterator)->dwUnionID, 
						"请注意:19:30分, 开始帮会夺城战");

				for (DareSet::iterator pos = (*vIterator)->vDareList.begin(); 
						pos!=(*vIterator)->vDareList.end(); ++pos)	
				{		
					CUnionM::getMe().sendUnionNotify(*pos, 
							"请注意:19:30分, 开始帮会夺城战");
				}
			}
		}
		rwlock.unlock();

		last_fulltime = tv1.tm_hour;
	}

	if (tv1.tm_hour==19)
	{       
		if (!isBeging && tv1.tm_min>=30 && tv1.tm_min<33)
		{       
			this->beginDare();
			isBeging = true;
		}

		if (tv1.tm_min==20)
		{      
			rwlock.rdlock();	
			for(vIterator = citys.begin(); vIterator!=citys.end(); vIterator++)
			{
				if ((*vIterator)->dareSize()>0)
				{
					CUnionM::getMe().sendUnionNotify((*vIterator)->dwUnionID, 
							"请注意:10分后, 开始帮会夺城战");
					
					for (DareSet::iterator pos = (*vIterator)->vDareList.begin(); 
							pos!=(*vIterator)->vDareList.end(); ++pos)	
					{		
						CUnionM::getMe().sendUnionNotify(*pos, 
								"请注意:10分后, 开始帮会夺城战");
					}
				}
			}
			rwlock.unlock();
		}
	}

	if (tv1.tm_hour==20 && tv1.tm_min>=30 && tv1.tm_min<33)
	{       
		//交战结束
		//this->endDare();
		isBeging = false;
	}	
}

void   CCityM::awardTaxGold(UserSession *pUser)
{
	CUnion* pUnion = CUnionM::getMe().getUnionByID(pUser->unionid);

	if (pUnion && pUnion->master)
	{
		if (pUnion->master->byStatus)
		{
			if (pUnion->master->id != pUser->id)
			{
				if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "只有帮主才能领取税金");
				return;
			}
		}
		else
		{
			if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "只有帮主才能领取税金");
			return;
		}
	}
	else
	{
		if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "帮会不存在");
		return;
	}
	
	CCity* pCity = NULL;
	pCity = this->findByUnionID(pUser->unionid);
	rwlock.wrlock();

	if (pCity)
	{
		if (pCity->dwGold>0)
		{
			Cmd::Session::t_dareGold_SceneSession send;
			send.dwUserID = pUser->id;
			send.dwNum = pCity->dwGold;
			send.dwWarID = 0;

			if (pUser->scene) 
			{
				pUser->scene->sendCmd(&send, sizeof(Cmd::Session::t_dareGold_SceneSession));
				Zebra::logger->trace("角色 %s 领取了城市税金%u文", pUser->name, pCity->dwGold);
			}

			if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "领取税金 %d 文", pCity->dwGold);
			pCity->dwGold = 0;
			pCity->writeDatabase();
		}
		else
		{
			if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "今天的税金你已领取");
		}
	}
	else
	{
		if (pUser) pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你没拥有城市，不能领取税金");
	}

	rwlock.unlock();
}

void CCityM::beginDare()
{
	std::vector<CCity*>::iterator vIterator;

	for(vIterator = citys.begin(); vIterator!=citys.end(); vIterator++)
	{
		if ((*vIterator)->dareSize() > 0)
		{
			(*vIterator)->beginDare();
		}
		else
		{
			Zebra::logger->trace("[对战]: (UNION_CITY_DARE, %d,%d) 今天无对战", 
					(*vIterator)->dwCountry, (*vIterator)->dwCityID);
		}
		
	}
}

void CCityM::endDare()
{
	std::vector<CCity*>::iterator vIterator;
	rwlock.rdlock();

	for (vIterator = citys.begin(); vIterator!=citys.end(); vIterator++)
	{
		if ((*vIterator)->dareSize() > 0)
		{
			(*vIterator)->endDare();
		}
	}
	rwlock.unlock();
}

void CCityM::execEveryCity(cityCallback& cb)
{
	std::vector<CCity*>::iterator vIterator;
	rwlock.rdlock();

	for (vIterator = citys.begin(); vIterator!=citys.end(); vIterator++)
	{
		cb.exec(*vIterator);
	}
	rwlock.unlock();

}

bool   CCityM::isCastellan(UserSession* pUser)
{
	bool ret = false;
	
	CUnion* pUnion = CUnionM::getMe().getUnionByID(pUser->unionid);
	if (pUnion)
	{
		// TODO:判断是否是城主
		if (pUnion->master && pUnion->master->id == pUser->id)  
		{//是帮主
			if (CCityM::getMe().findByUnionID(pUnion->id) !=NULL)
			{//是城主
				ret = true;
			}
		}
	}
	
	return ret;
}

//soke 巡捕（左）
bool   CCityM::isCatcher(UserSession* pUser)
{
	for (unsigned int i=0; i<CCityM::getMe().citys.size(); i++)
	{
		if (strncmp(CCityM::getMe().citys[i]->catcherName, pUser->name, MAX_NAMESIZE) == 0)
		{
			return true;
		}
	}

	return false;
}

void CCityM::refreshUnion(DWORD dwCountryID, DWORD dwCityID)
{
	Cmd::Session::t_updateSceneUnion_SceneSession send;
	SceneSession * pScene = SceneSessionManager::getInstance()->getSceneByID((dwCountryID<<16)
			+ dwCityID);	 
	
	CCity* pCity = this->find(dwCountryID, dwCityID);
	if (pCity && pScene)
	{
		//if (pCity->dwUnionID>0)
		//{
			send.dwSceneID = pScene->id;
			send.dwUnionID = pCity->dwUnionID;
			pScene->sendCmd(&send, sizeof(send));
		//}
	}
}

//------------------------------------------------------------------------------------------------------------


void CCity::init(Record* rec)
{
	char dare_list[256];
	char seps[]   = " ,\t\n;";
	char *token;
	bzero(dare_list, sizeof(dare_list));
	
	if (rec)
	{
		this->dwCountry = rec->get("country");
		this->dwCityID  = rec->get("cityid");
		this->dwUnionID = rec->get("unionid");
		this->isAward   = rec->get("isaward");
		this->dwGold	= rec->get("gold");
		strncpy(this->catcherName, rec->get("catchername"), MAX_NAMESIZE);
		strncpy(dare_list, rec->get("dareunionlist"), sizeof(dare_list));
		token = strtok( dare_list, seps );
		while( token != NULL )
		{
			this->addDareList(atoi(token));
			token = strtok( NULL, seps );
		}
	}
}

/** 
 * \brief 更新数据库记录
 * \author zjw
 */   
void CCity::writeDatabase()
{
	//FunctionTimes times(79,__FUNCTION__);
	Record rec,where;
	char dare_list[256];
	bzero(dare_list, sizeof(dare_list));

	std::ostringstream oss;
	oss << "country='" << this->dwCountry << "'";
	where.put("country", oss.str());

	oss.str("");
	oss << "cityid='" << this->dwCityID << "'";
	where.put("cityid", oss.str());

	rec.put("unionid", this->dwUnionID);
	rec.put("isaward", this->isAward);
	rec.put("gold", this->dwGold);
	rec.put("catchername", this->catcherName);
	oss.str("");
	
	for (DareSet::iterator pos=vDareList.begin();
			pos!=vDareList.end(); ++pos)
	{
		oss << *pos << ";";
	}

	strncpy(dare_list, oss.str().c_str(), sizeof(dare_list));
	rec.put("dareunionlist", dare_list);

	FieldSet* city = SessionService::metaData->getFields("CITY");

	if (city)       
	{       
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return;
		}

		if ((connHandleID)-1 != handle)
		{
			SessionService::dbConnPool->exeUpdate(handle, city, &rec, &where);
		}

		SessionService::dbConnPool->putHandle(handle);

	}
	else
	{
		Zebra::logger->error("城市数据加载失败，CITY表不存在");
		return;
	}
}


bool CCity::insertDatabase()
{
	//FunctionTimes times(78,__FUNCTION__);
	Record rec;
	rec.put("country", this->dwCountry);
	rec.put("cityid", this->dwCityID);
	rec.put("unionid", this->dwUnionID);
	rec.put("isaward", 0);
	rec.put("gold", 50000);
	FieldSet* city = SessionService::metaData->getFields("CITY");

	if (city)
	{
		connHandleID handle = SessionService::dbConnPool->getHandle();

		if ((connHandleID)-1 == handle)
		{
			Zebra::logger->error("不能获取数据库句柄");
			return false;
		}
		else
		{
			SessionService::dbConnPool->exeInsert(handle, city, &rec);
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

//soke 城主任命捕头
bool CCity::changeCatcher(UserSession* pUser)
{
	if (strncmp(pUser->name, this->catcherName, MAX_NAMESIZE) != 0)
	{
		UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(this->catcherName);
		if (u && u->scene)
		{
			Cmd::Session::t_setCatcherState_SceneSession send;
			send.byState = 0;
			send.dwUserID = pUser->id;
			pUser->scene->sendCmd(&send, sizeof(send));
		}
		
		rwlock.wrlock();
		strncpy(this->catcherName, pUser->name, MAX_NAMESIZE);
		rwlock.unlock();

		Cmd::Session::t_setCatcherState_SceneSession send;
		if (pUser->scene) 
		{
			send.byState = 1;
			send.dwUserID = pUser->id;
			pUser->scene->sendCmd(&send, sizeof(send));

			pUser->sendSysChat(Cmd::INFO_TYPE_GAME, "你被城主任命为捕头");

			///////////////////////////////////////////////////////////////////
			//soke 增加国家公告	
			Cmd::Session::t_countryPunishUser_SceneSession send;

			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, pUser->country, "奉天承运，城主诏曰：任命%s为捕头，即日上任，钦此.", pUser->name);
			SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_EXP, pUser->country, "奉天承运，城主诏曰：任命%s为捕头，即日上任，钦此", pUser->name);
			///////////////////////////////////////////////////////////////////
		}
	}

	return true;
}

bool CCity::cancelCatcher()
{
	UserSession *u = UserSessionManager::getInstance()->getUserSessionByName(this->catcherName);

	if (u && u->scene)
	{
		Cmd::Session::t_setCatcherState_SceneSession send;
		send.byState = 0;
		send.dwUserID = u->id;
		u->scene->sendCmd(&send, sizeof(send));
	}

	rwlock.wrlock();
	bzero(this->catcherName, MAX_NAMESIZE);
	rwlock.unlock();
	this->writeDatabase();

	return true;
}

bool CCity::changeUnion(DWORD unionid)
{
	CUnion* pSrcUnion = CUnionM::getMe().getUnionByID(this->dwUnionID);
	CUnion* pDestUnion = CUnionM::getMe().getUnionByID(unionid);

	SceneSession* pScene =pScene = SceneSessionManager::getInstance()->
		getSceneByID((this->dwCountry<<16)+this->dwCityID);


	rwlock.wrlock();

	this->dwUnionID = unionid;
	this->isAward = time(NULL);
	this->dwGold = 20000; // 2锭
	this->writeDatabase();

	rwlock.unlock();

	if (pSrcUnion)	
	{
		pSrcUnion->update_all_data();
		if (pDestUnion) pSrcUnion->sendUnionNotify("城市所有权已被 %s 获得", pDestUnion->name);
	}
	
	if (pDestUnion) 
	{
		pDestUnion->update_all_data();
		pDestUnion->sendUnionNotify("贵帮已获得城市所有权");

		if (pScene) SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, pDestUnion->dwCountryID, "%s 帮会获得 %s 所有权", pDestUnion->name, pScene->name);
	}

	CCityM::getMe().refreshUnion(this->dwCountry, this->dwCityID);
	
	return true;
}

bool CCity::abandonCity()
{
	CUnion* pSrcUnion = CUnionM::getMe().getUnionByID(this->dwUnionID);

	rwlock.wrlock();
	this->dwUnionID = 0;
	this->isAward = time(NULL);
	if (this->dwCityID == KING_CITY_ID || this->dwCountry == PUBLIC_COUNTRY)
	{
		this->dwGold = 50000; //soke 国王可以领取的金子
	}
	else
	{
		this->dwGold = 20000;//soke 城主可以领取的金子
	}
	rwlock.unlock();

	this->writeDatabase();

	if (pSrcUnion)	
	{
		pSrcUnion->update_all_data();
		pSrcUnion->sendUnionNotify("城市所有权已被放弃");
	}
	
	SceneSession* pScene =pScene = SceneSessionManager::getInstance()->
		getSceneByID((this->dwCountry<<16)+this->dwCityID);

	if (pScene) SessionChannel::sendCountryInfo(Cmd::INFO_TYPE_GAME, this->dwCountry, "%s 帮会放弃 %s 所有权", 
			pSrcUnion->name, pScene->name);

	CCityM::getMe().refreshUnion(this->dwCountry, this->dwCityID);
	this->writeDatabase();
	return true;
}


bool CCity::addDareList(DWORD dwUnionID)
{
	bool ret = false;
	rwlock.wrlock();
	
	if (vDareList.find(dwUnionID) == vDareList.end())
        {
		vDareList.insert(dwUnionID);
		ret = true;
        }
	
	rwlock.unlock();
	return ret;
}

bool CCity::isDare(DWORD dwUnionID)
{
	if (vDareList.find(dwUnionID) != vDareList.end())
	{
		return true;
	}
	
	return false;
}

bool CCity::isMe(DWORD country, DWORD cityid, DWORD unionid)
{
	if (this->dwCountry==country && this->dwCityID==cityid && this->dwUnionID==unionid)
		return true;

	return false;
}

void CCity::beginDare()
{
	if (this->dareSize()>0)
	{
		CUnion* pDefUnion = CUnionM::getMe().getUnionByID(this->dwUnionID);
		std::vector<DWORD> dare_list;

		if (pDefUnion == NULL)
		{
			Zebra::logger->trace("[对战]: 城市:%d 拥有帮会已不存在，取掉对战", this->dwCityID);
			if (this->dareSize()>0)
			{
				DareSet::iterator pos= vDareList.begin();
				this->changeUnion(*pos);
			}
			
			this->endDare();
			return;
		}
		
		for (DareSet::iterator pos = vDareList.begin(); pos!=vDareList.end(); ++pos)
		{
			CUnion* pAttUnion = CUnionM::getMe().getUnionByID(*pos);
			if (pAttUnion)
			{
				dare_list.push_back(*pos);
				Zebra::logger->trace("[对战]: 帮会夺城战在 %s 与 %s 之间举行", 
						pAttUnion->name, pDefUnion->name);
				pAttUnion->sendUnionNotify("帮会夺城战开始，请通过印室管理员进入印室");
			}
		}
		
		pDefUnion->sendUnionNotify("帮会夺城战开始，请通过印室管理员进入印室");
		
		Cmd::Session::t_createDare_SceneSession pCmd;

		//soke 帮会战时间
		pCmd.active_time        =       3600;
		pCmd.ready_time =       1;      
		pCmd.relationID2        =       this->dwUnionID;
		pCmd.type               =       Cmd::UNION_CITY_DARE;

		CDareM::getMe().createDare_sceneSession(&pCmd, dare_list);
	}
}

void CCity::endDare()
{       
	vDareList.clear();
}

char* CCity::getName()
{
	DWORD mapid = (this->dwCountry<<16) + this->dwCityID;
	SceneSession *pScene = SceneSessionManager::getInstance()->getSceneByID(mapid);
	if (pScene)
		return pScene->name;
	
	return NULL;
}


