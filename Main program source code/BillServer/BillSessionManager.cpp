/**
 * \file
 * \version  $Id: BillSessionManager.cpp  $
 * \author  
 * \date 
 * \brief 容器，用于保存金币兑换消息
 *
 * 
 */


#include <ext/hash_map>

#include "BillSessionManager.h"

BillSessionManager *BillSessionManager::instance = NULL;

/**
 * \brief 添加交易记录，并且验证是否存在重复记录
 *
 * \param tid 点数换金币交易流水号
 * \param cmd 点数换金币命令
 * \param task 处理该交易的TASK
 * \return 添加是否成功
 */
bool BillSessionManager::add(BillSession &bs)
{
	bool retval = false;
	std::string key = bs.tid;

	mlock.lock();
	BillSessionHashmap_iterator it = sessionMap.find(key);

	if (it == sessionMap.end())
	{
		//没有找到，需要插入新的记录
		sessionMap.insert(BillSessionHashmap_pair(bs.tid, bs));
		retval = true;
	}
	mlock.unlock();

	return retval;
}

/**
 * \brief 交易处理完成后，从会话管理器中删除对应记录
 *
 * \param tid 交易序列号
 *
 * \return 移除是否成功
 */
bool BillSessionManager::remove(const std::string& tid)
{
	bool retval = false;

	mlock.lock();
	BillSessionHashmap_iterator it = sessionMap.find(tid);
	
	if (it != sessionMap.end())
	{
		//找到了
		retval = true;
		sessionMap.erase(it);
	}
	mlock.unlock();

	return retval;
}

/**
 * \brief 查找TID对应的交易记录
 *
 * \param tid 交易序列号
 *
 * \return 查找失败，返回的对象中TID=0,否则返回对应的对象
 */

BillSession BillSessionManager::get(const std::string& tid)
{
	BillSession ret;
	
	mlock.lock();
	BillSessionHashmap_iterator it = sessionMap.find(tid);
	
	if (it != sessionMap.end())
	{
		//找到了
		ret = it->second;
		sessionMap.erase(it);
	}
	mlock.unlock();
	return ret;

}

