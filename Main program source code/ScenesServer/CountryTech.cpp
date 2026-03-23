/**
 * \file
 * \version  $Id: CountryTech.cpp $
 * \author 
 * \date 
 * \brief 国家科技管理器
 *
 * 
 */
#include "CountryTech.h"
#include "SceneManager.h"
#include "SceneNpc.h"
#include "TimeTick.h"
#include "SessionClient.h"
#include "ScenesServer.h"
#include "SceneUser.h"

void CountryTechM::init()
{
/*	if (!countryTechIndex.empty())
	{
		return;
	}

	for (SceneManager::CountryMap_iter iter=SceneManager::getInstance().country_info.begin(); 
			iter!=SceneManager::getInstance().country_info.end(); iter++)
	{
		addCountryTech(iter->second.id);
	}
*/
	TechMap.insert(std::map<DWORD, DWORD>::value_type(101, 1));
	TechMap.insert(std::map<DWORD, DWORD>::value_type(102, 2));
	TechMap.insert(std::map<DWORD, DWORD>::value_type(103, 3));
	TechMap.insert(std::map<DWORD, DWORD>::value_type(104, 4));
	TechMap.insert(std::map<DWORD, DWORD>::value_type(105, 4));
	TechMap.insert(std::map<DWORD, DWORD>::value_type(106, 4));
	TechMap.insert(std::map<DWORD, DWORD>::value_type(107, 4));
	TechMap.insert(std::map<DWORD, DWORD>::value_type(112, 4));
	TechMap.insert(std::map<DWORD, DWORD>::value_type(109, 5));
	TechMap.insert(std::map<DWORD, DWORD>::value_type(111, 6));
	TechMap.insert(std::map<DWORD, DWORD>::value_type(108, 7));
	TechMap.insert(std::map<DWORD, DWORD>::value_type(110, 8));
	TechMap.insert(std::map<DWORD, DWORD>::value_type(117, 9));
	TechMap.insert(std::map<DWORD, DWORD>::value_type(118, 10));
	TechMap.insert(std::map<DWORD, DWORD>::value_type(115, 11));
	TechMap.insert(std::map<DWORD, DWORD>::value_type(114, 12));
	TechMap.insert(std::map<DWORD, DWORD>::value_type(116, 13));
	TechMap.insert(std::map<DWORD, DWORD>::value_type(113, 14));

    ////////////////////////////////////////////////////////
	//soke 会话请求自己国家的太庙砖块数量 
    for ( SceneManager::CountryMap_iter iter = SceneManager::getInstance().country_info.begin(); 
        iter != SceneManager::getInstance().country_info.end(); 
        iter++ )
    {       
        Cmd::Session::ReqCountryLoongPearlNum_SceneSession send;
        send.serverID = ScenesService::getInstance().getServerID();
        send.countryID = iter->second.id;
        sessionClient->sendCmd(&send, sizeof(send));	
    }
	////////////////////////////////////////////////////////
    DWORD tmiaoX=atoi(Zebra::global["tai_miao_x"].c_str());
    DWORD tmiaoY=atoi(Zebra::global["tai_miao_y"].c_str());
    if(tmiaoX && tmiaoY)
        LOONG_ALTAR_POS = zPos(tmiaoX,tmiaoY);

}

CountryTech* CountryTechM::getCountryTech(DWORD dwCountryID)
{
	CountryTech* pCountryTech = NULL;

	rwlock.rdlock();
	CountryTechMap::iterator pos;
	pos = countryTechIndex.find(dwCountryID);

	if (pos != countryTechIndex.end())
	{
		pCountryTech = pos->second;
	}       	
	rwlock.unlock();
		
	return pCountryTech;
}

void CountryTechM::addCountryTech(DWORD dwCountryID)
{
	CountryTech* pCountryTech = new CountryTech();
	rwlock.wrlock();
	if (pCountryTech)
	{
		pCountryTech->id = dwCountryID;
		countryTechIndex[dwCountryID] = pCountryTech;
	}
	rwlock.unlock();
}

bool CountryTechM::canProduce(DWORD dwCountry, DWORD dwObjectType, DWORD dwObjectLevel)
{
	CountryTech* pCountryTech = this->getCountryTech(dwCountry);
	if (pCountryTech)
	{
		std::map<DWORD, DWORD>::iterator pos;
		pos = TechMap.find(dwObjectType);
		DWORD dwTechType = 0;

		if (pos != TechMap.end())
		{               
			 dwTechType = pos->second;
		}      	
		
		CTech* pTech = pCountryTech->getTech(dwTechType);
		if (pTech)
		{
			return pTech->canProduce(dwObjectLevel);
		}
		else {
			return true;
		}
	}
	
	return false;
}

////////////////////////////////////////////////////////////
void CountryTechM::addLoongAltar( DWORD countryID, DWORD num )
{
    CountryAltarMapIt it = counAltarList.find(countryID);
    if ( it == counAltarList.end() )
    {
        LoongAltar altar;
        altar.countryID = countryID;
        altar.pearlNum = num;
        counAltarList[countryID] = altar;

        summonLoongAltar(countryID, num, false);
    }
}

bool CountryTechM::summonLoongAltar( DWORD countryID, DWORD num, bool needReplace )
{
    Scene * scene = SceneManager::getInstance().getSceneByID((countryID<<16)+139);
    if( !scene ) return false;

    SceneNpc * old = NULL;

    if ( needReplace )
    {
        old = SceneNpcManager::getMe().getNpcByTempID(scene->loongAltarNpcTmpID);
        if( !old ) return false;

        //通知客户端删除NPC
        Cmd::stRemoveMapNpcMapScreenUserCmd removeMapNpc;
        removeMapNpc.dwMapNpcDataPosition = old->tempid;
        scene->sendCmdToNine(old->getPosI(), &removeMapNpc, sizeof(removeMapNpc));

        //从地图和管理器中删除这个NPC
        scene->removeNpc(old);
        SceneNpcManager::getMe().removeSceneNpc(old);
        SceneNpcManager::getMe().removeSpecialNpc(old);
        //Zebra::logger->trace("%s, 删除npc %s(%u)", __FUNCTION__, old->name, old->tempid);
        SAFE_DELETE(old);

        scene->loongAltarNpcTmpID = 0;
    }

    DWORD altarNpcID = getAltarNpcIDByPearlNum(num);
    zNpcB *base = npcbm.get( altarNpcID );
    if ( !base )
    {
        Zebra::logger->debug("[太庙系统]%s 从NPC表获取太庙数据 失败,NPC id=%d", scene->name, altarNpcID);
        return false;
    }

    t_NpcDefine define;
    define.id = base->id;
    strcpy(define.name, base->name);
    define.pos = old?old->getPos():LOONG_ALTAR_POS;
    define.num = 1;
    define.interval = 5; //soke 太庙刷新时间
    define.initstate = zSceneEntry::SceneEntry_Normal;
    define.width = 2;
    define.height = 2;
    define.scriptID = 0;
    scene->initRegion(define.region, define.pos, define.width, define.height);

    SceneNpc * boss = scene->summonOneNpc<SceneNpc>(define, define.pos, base);
    if (!boss)
    {
        Zebra::logger->debug("[太庙系统]%s 加载太庙失败(%u,%u),NPC id=%d",scene->name,LOONG_ALTAR_POS.x,LOONG_ALTAR_POS.y,altarNpcID);
        return false;
    }
    else
    {
        scene->loongAltarNpcTmpID = boss->tempid;
        Zebra::logger->debug("[太庙系统]%s 加载太庙(%u,%u)成功,NPC id=%d", scene->name,boss->getPos().x, boss->getPos().y,altarNpcID);
    }
    return true;
}

DWORD CountryTechM::getAltarPearlNumByCountryID( DWORD countryID )
{
    CountryAltarMapIt it = counAltarList.find(countryID);
    if ( it != counAltarList.end() )
    {
        if ( it->second.countryID == countryID )
        {
            return it->second.pearlNum;
        }
    }
    return 0;
}

void CountryTechM::putPearlIntoAltar( DWORD countryID )
{
    return doMovePearl(countryID, MOVE_PEARL_ACTION_PUT);
}

void CountryTechM::fetchPearlFromAltar( DWORD countryID )
{
    return doMovePearl(countryID, MOVE_PEARL_ACTION_FETCH);
}


bool CountryTechM::checkNeedReplaceAltar( DWORD countryID, DWORD oldNum, DWORD newNum, bool isNotify )
{
    bool needReplace  = false;
    if ( 0 == newNum && 1 == oldNum )
    {
        needReplace = true;
    }
    else if ( newNum >= 1 && newNum < 500 && ( 0 == oldNum || 500 == oldNum ) ) //
    {
        needReplace = true;
    }
    else if ( newNum >= 500 && newNum < 700 && ( 499 == oldNum || 700 == oldNum ) ) //
    {
        needReplace = true;
    }
    else if ( newNum >= 700 && ( 699 == oldNum))
    {
        needReplace = true;
    }

    if ( needReplace && isNotify )
    {
        if ( oldNum < newNum )
            Channel::sendCountryInfo( countryID, Cmd::INFO_TYPE_EXP2, "国家太庙升级了，再接再励～～");
        else
            Channel::sendCountryInfo( countryID, Cmd::INFO_TYPE_EXP2, "国家太庙降级了，加油～～");
    }
    return needReplace;
}

void CountryTechM::doMovePearl( DWORD countryID, MovePearlAction act )
{
    Scene * scene = SceneManager::getInstance().getSceneByID((countryID<<16)+139);
    if( !scene ) return ;

    SceneNpc *loong = SceneNpcManager::getMe().getNpcByTempID(scene->loongAltarNpcTmpID);
    if ( !loong ) return ;

    using namespace Cmd::Session;
    SyncCountryLoongPearlNum_SceneSession send;
    send.countryID = countryID;

    DWORD oldNum = 0;
    DWORD newNum = 0;
    CountryAltarMapIt it = counAltarList.find(countryID);
    if ( it != counAltarList.end() )
    {
        if ( it->second.countryID == countryID )
        {
            if ( MOVE_PEARL_ACTION_FETCH == act )
            {
                if( it->second.pearlNum > 0 )
                {
                    oldNum = it->second.pearlNum;
                    it->second.pearlNum --;
                    if ( 0 == it->second.pearlNum)
                    {
                     //   loong->setStateToNine(Cmd::USTATE_BIG_LOONG_OUT);
                    }
                    else if( it->second.pearlNum > 0)
                    {
                     //   loong->setStateToNine(Cmd::USTATE_LOONG_OUT);
                    }
                }
            }
            else if( MOVE_PEARL_ACTION_PUT == act )
            {
                oldNum = it->second.pearlNum;
                it->second.pearlNum ++;

              //  loong->setStateToNine(Cmd::USTATE_LOONG_IN);
            }
            newNum = it->second.pearlNum;
        }
    }
    send.num = newNum;
    sessionClient->sendCmd(&send, sizeof(send));

    bool needReplace = checkNeedReplaceAltar(countryID, oldNum, newNum, true); 
    if ( needReplace )
    {
        summonLoongAltar(countryID, newNum, needReplace);
        showAltarEff(scene, newNum, act);
    }
}

void CountryTechM::sendLoongAltarList( SceneUser* pUser )
{
    if ( pUser )
    {
        BYTE buf[zSocket::MAX_DATASIZE];
        Cmd::ReplyAltarListLoongUserCmd *pSend = (Cmd::ReplyAltarListLoongUserCmd* )buf;
        constructInPlace(pSend);
        pSend->altarNum = 0;
        for( CountryAltarMapIt it = counAltarList.begin(); it != counAltarList.end(); ++it )
        {
            if ( it->first > 0 && it->first < 3 )//太庙建设次数 13
            {
                const char* counName = SceneManager::getInstance().getCountryNameByCountryID(it->second.countryID);
                bcopy(counName, (pSend->altarList+pSend->altarNum)->counName, MAX_NAMESIZE );
                (pSend->altarList + pSend->altarNum)->pearlNum = it->second.pearlNum;
                pSend->altarNum ++;
            }
        }
        pUser->sendCmdToMe( pSend, sizeof(Cmd::ReplyAltarListLoongUserCmd) + pSend->altarNum * sizeof(pSend->altarList[0]) );
    }
}

DWORD CountryTechM::getAltarNpcIDByPearlNum( DWORD num )
{
    DWORD altarNpcID = 0;
    if ( 0 == num )
    {
        altarNpcID = BASE_LOONG_ALTAR_ID;
    }
    else if ( num >= 1 && num <= 499 ) //残破的太庙
    {
        altarNpcID = SHABBY_LOONG_ALTAR_ID;
    }
    else if ( num >= 500 && num <= 699 ) //普通太庙
    {
        altarNpcID = LOONG_ALTAR_ID;
    }
    else if ( num >= 700 ) //高级太庙
    {
        altarNpcID = SENIOR_LOONG_ALTAR_ID;
    }
    return altarNpcID;
}

bool CountryTechM::showAltarEff( Scene *scene, DWORD pearlNum, MovePearlAction act )
{
    if( !scene ) return false;
    SceneNpc *loong = SceneNpcManager::getMe().getNpcByTempID(scene->loongAltarNpcTmpID);
    if ( !loong ) return false;

    if ( MOVE_PEARL_ACTION_FETCH == act )
    {
        if ( 0 == pearlNum )
        {
         //   loong->setStateToNine(Cmd::USTATE_BIG_LOONG_OUT);
        }
        else if( 499 == pearlNum  || 699 == pearlNum )
        {
       //     loong->setStateToNine(Cmd::USTATE_LOONG_OUT);
        }
    }
    else if( MOVE_PEARL_ACTION_PUT == act )
    {
        if ( 1 == pearlNum || 500 == pearlNum || 700 == pearlNum )
        {
        //    loong->setStateToNine(Cmd::USTATE_LOONG_IN);
        }
    }
    return true;
}
////////////////////////////////////////////////////////////

//------------------------CountryTech-----------------------
void CountryTech::init(Cmd::Session::t_updateTech_SceneSession* rev)
{
	if (rev)
	{
		for (int i=0; i<14; i++)
		{
			CTech *pTech = this->getTech(rev->data[i].dwType);
			if (!pTech)
			{
				pTech = new CTech();
				this->addTech(rev->data[i].dwType, pTech);
			}

			pTech->init(&rev->data[i]);
		}
	}
	
#ifdef _ZJW_DEBUG
	
	for (CTechMap::iterator mIter=techIndex.begin(); mIter!=techIndex.end(); mIter++)
	{
		CTech* temp = mIter->second;
		Zebra::logger->debug("国家:%d 科技类型:%d 科技等级:%d", this->id, temp->dwType, temp->dwLevel);
	}
#endif	

}

CTech* CountryTech::getTech(DWORD dwType)
{
	CTech* pTech = NULL;

	rwlock.rdlock();
	CTechMap::iterator pos;
	pos = techIndex.find(dwType);

	if (pos != techIndex.end())
	{
		pTech = pos->second;
	}       	
	rwlock.unlock();
	
	return pTech;
}

void CountryTech::addTech(DWORD dwType, CTech* pTech)
{
	rwlock.wrlock();
	if (pTech)
	{
		techIndex.insert(CTechMap::value_type(dwType, pTech));
	}
	rwlock.unlock();

}

//------------------------CTech----------------------
void CTech::init(Cmd::Session::_techItem* rec)
{
	if (rec)
	{
		dwType = rec->dwType;
		dwLevel = rec->dwLevel;
	}
}

bool CTech::canProduce(DWORD dwObjectLevel)
{
	int level_tab[] = {0, 80, 90, 100, 110, 120, 130, 140, 150};

	if ((int)dwObjectLevel>=level_tab[this->dwLevel])
	{
		return false;
	}
	
	return true;	
}

