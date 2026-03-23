/**
 * \file
 * \version  $Id: CountryAlly.cpp $
 * \author   
 * \date 
 * \brief 国家科技管理器
 *
 * 
 */
#include "CountryAlly.h"
#include "SceneManager.h"
#include "SceneNpc.h"
#include "TimeTick.h"
#include "SessionClient.h"

void CountryAllyM::init()
{
}

CAlly* CountryAllyM::getAlly(DWORD dwCountryID1, DWORD dwCountryID2)
{
	CAlly* pAlly = NULL;
	std::vector<CAlly*>::iterator pos;

	rwlock.rdlock();

	for (pos = allies.begin(); pos!=allies.end(); pos++)
	{
		CAlly* temp = *pos;
		if (temp 
			&& ((temp->dwCountryID == dwCountryID1 && temp->dwAllyCountryID == dwCountryID2)
			|| (temp->dwCountryID == dwCountryID2 && temp->dwAllyCountryID == dwCountryID1))
			)
		{
			pAlly = temp;
			break;
		}
	}

	rwlock.unlock();
		
	return pAlly;
}

void   CountryAllyM::processUpdate(Cmd::Session::t_updateAlly_SceneSession *rev)
{
	rwlock.rdlock();
	
#ifdef 	_ZJW_DEBUG
	Zebra::logger->debug("[国家联盟]: 收到 %d 条联盟数据", rev->dwSize);
#endif	
	for (unsigned int i=0; i<rev->dwSize; i++)
	{
		CAlly* pAlly = this->getAlly(rev->data[i].dwCountryID, rev->data[i].dwAllyCountryID);
		if (pAlly)
		{//update
#ifdef 	_ZJW_DEBUG
	Zebra::logger->debug("[国家联盟]: 更新 %d,%d 联盟友好度:%d", pAlly->dwCountryID, 
			pAlly->dwAllyCountryID, pAlly->dwFriendDegree);
#endif	

			pAlly->dwFriendDegree = rev->data[i].dwFriendDegree;
		}
		else
		{//new
			pAlly = new CAlly();
			if (pAlly)
			{
				pAlly->dwCountryID = rev->data[i].dwCountryID;
				pAlly->dwAllyCountryID = rev->data[i].dwAllyCountryID;
				pAlly->dwFriendDegree = rev->data[i].dwFriendDegree;
				allies.push_back(pAlly);
#ifdef 	_ZJW_DEBUG
	Zebra::logger->debug("[国家联盟]: 新建 %d,%d 联盟友好度:%d", pAlly->dwCountryID, 
			pAlly->dwAllyCountryID, pAlly->dwFriendDegree);
#endif	
			}
		}
	}

	rwlock.unlock();
}

bool  CountryAllyM::isAlly(DWORD dwCountryID1, DWORD dwCountryID2)
{
	CAlly* pAlly = this->getAlly(dwCountryID1, dwCountryID2);

	if (pAlly && pAlly->dwFriendDegree>0)
	{
		return true;
	}

	return false;
}

DWORD  CountryAllyM::getFriendLevel(DWORD dwCountryID1, DWORD dwCountryID2)
{
	CAlly* pAlly = this->getAlly(dwCountryID1, dwCountryID2);

	if (pAlly && pAlly->dwFriendDegree>0)
	{
		return 2; // 只要友好度大于0就是协力关系,盟国关系的第二级
/*		if (pAlly->dwFriendDegree>10000)
		{
			return 2;
		}
		else
		{
			return 1;
		}
		*/
	}

	return 0;
}

