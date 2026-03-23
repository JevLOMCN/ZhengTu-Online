/**
 * \file
 * \version  $Id: BillManager.cpp  $
 * \author  
 * \date 
 * \brief 账号管理，记录一个区中所有已经登陆的账号
 *
 * 
 */

#include <iostream>
#include <ext/hash_map>

#include "Zebra.h"
#include "BillManager.h"

BillManager *BillManager::instance = NULL;

/**
 * \brief 验证帐号是否已经登陆
 * 如果已经登陆需要返回错误信息到登陆服务器
 * \param session 待验证的会话信息
 * \return 验证是否成功
 */
bool BillManager::verify(const t_NewLoginSession &session)
{
	mlock.lock();
	BillInfoMap_iterator it = infoMap.find(session.accid);
	if (it != infoMap.end())
	{
		mlock.unlock();
		return false;
	}

	BillInfo info;
	info.accid = session.accid;
	info.loginTempID = session.loginTempID;
	info.wdGatewayID = session.wdGatewayID;
	strncpy(info.client_ip,session.client_ip,sizeof(info.client_ip));
	infoMap.insert(BillInfoMap_pair(info.accid, info));

	mlock.unlock();
	return true;
}

/**
 * \brief 更新登陆超时信息
 * 登陆服务器验证通过以后，如果在一定时间没有登陆网关，那么将删除这个会话过程
 */
void BillManager::update()
{
	mlock.lock();
	if (!infoMap.empty())
	{
		zTime current;
		for(BillInfoMap_iterator it = infoMap.begin(); it != infoMap.end();)
		{
			//登陆超时，删除
			if (BillInfo::WAIT_LOGIN == it->second.state
					&& it->second.timestamp.elapse(current) >= session_timeout_value)
			{
				Zebra::logger->debug("等待网关登陆信息超时：%u, %u, %u", it->second.accid, it->second.loginTempID, it->second.wdGatewayID);
				BillInfoMap_iterator tmp = it;
				it++;
				infoMap.erase(tmp);
			}
			else
				it++;
		}
	}
	mlock.unlock();
}

/**
 * \brief 当网关关闭的时候，需要清除在这个网关上面登陆的所有会话信息
 * \param wdGatewayID 网关编号
 */
void BillManager::updateByGatewayID(const WORD wdGatewayID)
{
	mlock.lock();
	if (!infoMap.empty())
	{
		for(BillInfoMap_iterator it = infoMap.begin(); it != infoMap.end();)
		{
			if (wdGatewayID == it->second.wdGatewayID)
			{
				Zebra::logger->debug("网关关闭，清除登陆数据：%u, %u, %u", it->second.accid, it->second.loginTempID, it->second.wdGatewayID);
				BillInfoMap_iterator tmp = it;
				it++;
				infoMap.erase(tmp);
			}
			else
				it++;
		}
	}
	mlock.unlock();
}

/**
 * \brief 验证客户端登陆网关的信息
 * 当客户端登陆网关的是否，需要验证这个登陆是否是合法的
 * \param accid 帐号编号
 * \param loginTempID 登陆临时编号
 * \return 验证是否成功
 */
bool BillManager::login(const DWORD accid, const DWORD loginTempID)
{
	mlock.lock();
	BillInfoMap_iterator it = infoMap.find(accid);
	if (it != infoMap.end()
			&& loginTempID == it->second.loginTempID
			&& BillInfo::WAIT_LOGIN == it->second.state)
	{
		it->second.state = BillInfo::CONF_LOGIN;
		mlock.unlock();
		return true;
	}
	mlock.unlock();
	return false;
}

/**
 * \brief 当退出的时候，需要清除玩家的登陆会话过程
 * 并不是马上删除记录，而是让这个账号锁定一段时间，可以有效避免重复登陆
 * \param accid 帐号编号
 * \param loginTempID 登陆临时编号
 * \return 退出登陆是否成功
 */
bool BillManager::logout(const DWORD accid, const DWORD loginTempID)
{
	mlock.lock();
	BillInfoMap_iterator it = infoMap.find(accid);
	if (it != infoMap.end()
			&& loginTempID == it->second.loginTempID
			&& BillInfo::CONF_LOGIN == it->second.state)
	{
		//退出登陆的时候，需要等待2秒钟的时间，才能允许下一次登陆
		it->second.state = BillInfo::WAIT_LOGIN;
		it->second.timestamp.now();
		it->second.timestamp -= (session_timeout_value - 2);
		mlock.unlock();
		return true;
	}
	mlock.unlock();
	return false;
}

bool BillManager::updateGold(DWORD acc , double gold)
{
	mlock.lock();
	BillInfoMap_iterator it = infoMap.find(acc);
	if (it != infoMap.end()
			&& BillInfo::CONF_LOGIN == it->second.state)
	{
		it->second.gold = gold;
		mlock.unlock();
		return true;
	}
	mlock.unlock();
	return false;
}
bool BillManager::updateVipTime(DWORD acc , DWORD vip)
{
	mlock.lock();
	BillInfoMap_iterator it = infoMap.find(acc);
	if (it != infoMap.end()
			&& BillInfo::CONF_LOGIN == it->second.state)
	{
		it->second.vip_time = vip;
		mlock.unlock();
		return true;
	}
	mlock.unlock();
	return false;
}
DWORD BillManager::getVipTime(DWORD acc)
{
	DWORD ret_vip = 0;
	mlock.lock();
	BillInfoMap_iterator it = infoMap.find(acc);
	if (it != infoMap.end()
			&& BillInfo::CONF_LOGIN == it->second.state)
	{
		ret_vip = it->second.vip_time;
	}
	mlock.unlock();
	return ret_vip;
}
double BillManager::getGold(DWORD acc)
{
	double ret_gold = 0.0;
	mlock.lock();
	BillInfoMap_iterator it = infoMap.find(acc);
	if (it != infoMap.end()
			&& BillInfo::CONF_LOGIN == it->second.state)
	{
		ret_gold = it->second.gold;
	}
	mlock.unlock();
	return ret_gold;
}
