/**
 * \file
 * \version  $Id: RecordSessionManager.cpp  $
 * \author 
 * \date 
 * \brief 容器，用于保证不会出现重复登陆
 *
 * 
 */


#include <ext/hash_map>
#include "Zebra.h"
#include "RecordSessionManager.h"

RecordSessionManager *RecordSessionManager::instance = NULL;

/**
 * \brief 读取档案信息的时候，需要添加记录，并且验证是否存在重复记录
 *
 * \param accid 帐号
 * \param id 角色编号
 * \param wdServerID 服务器编号
 * \return 添加是否成功
 */
bool RecordSessionManager::add(const DWORD accid, const DWORD id, const WORD wdServerID)
{
	Zebra::logger->trace("RecordSessionManager::add");
	bool retval = false;

	mlock.lock();
	RecordSessionHashmap_iterator it = sessionMap.find(accid);
	if (it == sessionMap.end())
	{
		//没有找到，需要插入新的记录
		RecordSession session(accid, id, wdServerID);
		sessionMap.insert(RecordSessionHashmap_pair(accid, session));
		retval = true;
	}
	mlock.unlock();

	return retval;
}

/**
 * \brief 回写档案需要验证会话信息是否存在
 *
 * \param accid 帐号
 * \param id 角色编号
 * \param wdServerID 服务器编号
 * \return 验证是否成功
 */
bool RecordSessionManager::verify(const DWORD accid, const DWORD id, const WORD wdServerID)
{
	Zebra::logger->trace("RecordSessionManager::verify");
	bool retval = false;

	mlock.lock();
	RecordSessionHashmap_iterator it = sessionMap.find(accid);
	if (it != sessionMap.end()
			&& it->second.accid == accid
			&& it->second.id == id
			&& it->second.wdServerID == wdServerID)
	{
		//找到了
		retval = true;
		it->second.lastsavetime.now();
	}
	mlock.unlock();

	return retval;
}

/**
 * \brief 角色退出时候，回写档案完成以后需要移除会话记录
 *
 * \param accid 帐号
 * \param id 角色编号
 * \param wdServerID 服务器编号
 * \return 移除是否成功
 */
bool RecordSessionManager::remove(const DWORD accid, const DWORD id, const WORD wdServerID)
{
	Zebra::logger->trace("RecordSessionManager::remove");
	bool retval = false;

	mlock.lock();
	RecordSessionHashmap_iterator it = sessionMap.find(accid);
	if (it != sessionMap.end()
			&& it->second.accid == accid
			&& it->second.id == id
			&& it->second.wdServerID == wdServerID)
	{
		//找到了
		retval = true;
		sessionMap.erase(it);
	}
	mlock.unlock();

	return retval;
}

/**
 * \brief 场景服务器关闭的时候，需要把所有的与这个服务器相关的会话记录清除
 *
 * \param wdServerID 服务器编号
 */
void RecordSessionManager::removeAllByServerID(const WORD wdServerID)
{
	Zebra::logger->trace("RecordSessionManager::removeAllByServerID");
	mlock.lock();
	if (!sessionMap.empty())
	{
		for(RecordSessionHashmap_iterator it = sessionMap.begin(); it != sessionMap.end();)
		{
			if (it->second.wdServerID == wdServerID)
			{
				RecordSessionHashmap_iterator tmp = it;
				it++;
				sessionMap.erase(tmp);
			}
			else
				it++;
		}
	}
	mlock.unlock();
}

