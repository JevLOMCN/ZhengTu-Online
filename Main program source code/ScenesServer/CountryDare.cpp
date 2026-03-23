/**
 * \file
 * \version  $Id: CountryDare.cpp  $
 * \author  
 * \date 
 * \brief 国战管理器
 *
 * 
 */
#include "CountryDare.h"
#include "SceneManager.h"
#include "SceneNpc.h"
#include "TimeTick.h"
#include "SessionCommand.h"
#include "SessionClient.h"

CountryDareM *CountryDareM::_instance = 0;


void CountryDareM::timer()
{
	for (unsigned int i=0; i<dares.size(); i++)
	{
		int time_diff = abs(SceneTimeTick::currentTime.sec() - dares[i].last_attack_time);
		int king_time_diff = abs(SceneTimeTick::currentTime.sec() - dares[i].last_king_attack_time);

		int gen_time_diff = abs(SceneTimeTick::currentTime.sec() - dares[i].last_gen_attack_time);
		int gen_att_time_diff  = abs(SceneTimeTick::currentTime.sec() - dares[i].last_att_gen_attack_time);
		
		if (dares[i].last_attack_time!=0 && time_diff < 120)
		{
			Cmd::Session::t_countryNotify_SceneSession send;
			bzero(send.info, sizeof(send.info));
			sprintf(send.info, "%s", "凤凰城在受攻击中，请速回援");
			send.dwCountryID = dares[i].id;
			send.infoType = Cmd::INFO_TYPE_ATT_FLAG;
			sessionClient->sendCmd(&send, sizeof(send));
		}

		if (dares[i].last_king_attack_time!=0 && king_time_diff < 120)
		{
			Cmd::Session::t_countryNotify_SceneSession send;
			bzero(send.info, sizeof(send.info));
			sprintf(send.info, "%s", "王城在受攻击中，请速回援");
			send.infoType = Cmd::INFO_TYPE_ATT_FLAG;
			send.dwCountryID = dares[i].id;
			sessionClient->sendCmd(&send, sizeof(send));
		}
		if (dares[i].last_gen_attack_time!=0 && gen_time_diff < 600)
		{
			Cmd::Session::t_countryNotify_SceneSession send;
			bzero(send.info, sizeof(send.info));
			sprintf(send.info, "%s", "我国王城遭到了攻击，请速回援");
			send.dwCountryID = dares[i].id;
			sessionClient->sendCmd(&send, sizeof(send));
		}
		
		if (dares[i].last_att_gen_attack_time!=0 && gen_att_time_diff < 600)
		{
			Cmd::Session::t_countryNotify_SceneSession send;
			bzero(send.info, sizeof(send.info));
			sprintf(send.info, "%s", "我国王城遭到了攻击，请速回援");
			send.dwCountryID = dares[i].id;
			sessionClient->sendCmd(&send, sizeof(send));
		}

	}
}

/**
 * \brief 统计存活的指定NPC的个数
 */
struct TotalAllNpc : public zSceneEntryCallBack
{
	int count;  /// 存活的NPC个数
	DWORD dwNpcID; // 要统计的NPCID
	TotalAllNpc() : count(0),dwNpcID(0) {};
	
	/**
	 * \brief 回调函数
	 * \param entry 地图物件,这里是玩家
	 * \return 回调是否送成功
	 */
	bool exec(zSceneEntry *entry)
	{
		if (((SceneNpc *)entry)->getState() != zSceneEntry::SceneEntry_Death)
		{
			count++;
		}
		
		return true;
	}
};

/**
 * \brief 复活所有副旗
 */
struct ReliveSpecNpc : public zSceneEntryCallBack
{
	ReliveSpecNpc() {};
	
	/**
	 * \brief 回调函数
	 * \param entry 地图物件,这里是玩家
	 * \return 回调是否送成功
	 */
	bool exec(zSceneEntry *entry)
	{
		zRTime cur;
		((SceneNpc*)entry)->reliveTime = cur;
		//((SceneNpc*)entry)->setMoveTime(cur);
		//((SceneNpc*)entry)->setAttackTime(cur);

		return true;
	}
};

/**
 * \brief 修改所有副旗复活时间
 */
struct DelaySpecNpc : public zSceneEntryCallBack
{
	DelaySpecNpc() {};
	
	/**
	 * \brief 回调函数
	 * \param entry 地图物件,这里是玩家
	 * \return 回调是否送成功
	 */
	bool exec(zSceneEntry *entry)
	{
		return true;
	}
};

bool CountryDareM::isAttackMainGen(Scene* scene)
{
	//FunctionTime func_time(500000,__PRETTY_FUNCTION__,"砍旗判断时间" , 32);
	TotalAllNpc totalnpc;

	if (scene)
	{
		scene->execAllOfScene_npc(COUNTRY_SEC_GEN, totalnpc);
		if (totalnpc.count==0)
			return true;
	}

	return false;
}

bool CountryDareM::isAttackMainFlag(Scene* scene, DWORD dwNpcID)
{
	//FunctionTime func_time(500000,__PRETTY_FUNCTION__,"砍旗判断时间" , 32);
	TotalAllNpc totalnpc;
	if (scene)
	{
		scene->execAllOfScene_npc(dwNpcID, totalnpc);
		if (totalnpc.count==0)
			return true;
	}

	return false;
}

void CountryDareM::updateAttGenAttackTime(DWORD countryid, time_t uptime)
{
	for (unsigned int i=0; i<dares.size(); i++)
	{
		if (dares[i].id == countryid)
		{
			rwlock.wrlock();
			dares[i].last_att_gen_attack_time = uptime;
			rwlock.unlock();
		}
	}
}

void CountryDareM::updateKingAttackTime(DWORD countryid, time_t uptime)
{
	for (unsigned int i=0; i<dares.size(); i++)
	{
		if (dares[i].id == countryid)
		{
			rwlock.wrlock();
			dares[i].last_king_attack_time = uptime;
			rwlock.unlock();
		}
	}
}
void CountryDareM::updateAttackTime(DWORD countryid, time_t uptime)
{
	for (unsigned int i=0; i<dares.size(); i++)
	{
		if (dares[i].id == countryid)
		{
			rwlock.wrlock();
			dares[i].last_attack_time = uptime;
			rwlock.unlock();
		}
	}
}
void CountryDareM::updateGenAttackTime(DWORD countryid, time_t uptime)
{
	for (unsigned int i=0; i<dares.size(); i++)
	{
		if (dares[i].id == countryid)
		{
			rwlock.wrlock();
			dares[i].last_gen_attack_time = uptime;
			rwlock.unlock();
		}
	}
}

void CountryDareM::reliveSecondFlag(Scene* scene, DWORD dwNpcID)
{
	//FunctionTime func_time(500000,__PRETTY_FUNCTION__,"砍旗判断时间" , 32);
	ReliveSpecNpc relivenpc;
	if (scene)
		scene->execAllOfScene_npc(dwNpcID, relivenpc);
}

void CountryDareM::delaySecondFlag(Scene* scene)
{
	//FunctionTime func_time(500000,__PRETTY_FUNCTION__,"砍旗判断时间" , 32);
	DelaySpecNpc delaynpc;
	if (scene)
		scene->execAllOfScene_npc(COUNTRY_SEC_FLAG, delaynpc);

}


void CountryDareM::init()
{
	rwlock.wrlock();
	
	if (!dares.empty())
		return;

	for (SceneManager::CountryMap_iter iter=SceneManager::getInstance().country_info.begin(); iter!=SceneManager::getInstance().country_info.end(); iter++)
	{
		CountryDare temp;
		temp.id = iter->second.id;

		dares.push_back(temp);
	}
	
	rwlock.unlock();
}

