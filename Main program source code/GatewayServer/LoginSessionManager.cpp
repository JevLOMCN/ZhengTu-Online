/**
 * \file
 * \version  $Id: LoginSessionManager.cpp  $
 * \author  
 * \date
 * \brief 保存登陆会话建立之前的信息
 *
 * 
 */


#include <ext/hash_map>

#include "zMutex.h"
#include "SuperCommand.h"
#include "LoginSessionManager.h"

LoginSessionManager *LoginSessionManager::instance = NULL;

//更换一下试试
// /**
//  * \brief 向管理器中添加一条记录
//  *
//  * \param session 会话信息
//  */
// void LoginSessionManager::put(const t_NewLoginSession &session)
// {
//   //soke 取消日志显示
//   //Zebra::logger->debug("LoginSessionManager::put");
// 	mlock.lock();
// 	//Zebra::logger->debug("put: %u %u %u %u %u %u %u %u", session.des_key[0], session.des_key[1], session.des_key[2], session.des_key[3], session.des_key[4], session.des_key[5], session.des_key[6], session.des_key[7]);
// 	LoginSessionHashmap_iterator it = sessionData.find(session.accid);
// 	if (it != sessionData.end())
// 	{
// 		//找到了，只是更新
// 		it->second.session = session;
// 		it->second.timestamp.now();
// 	}
// 	else
// 	{
// 		//没有找到，需要插入新的记录
// 		sessionData.insert(LoginSessionHashmap_pair(session.accid, LoginSession(session)));
// 	}
// 	mlock.unlock();
// }

// /**
//  * \brief 验证登陆连接是否正确
//  *
//  * \param loginTempID 登陆临时编号
//  * \param accid 登陆帐号
//  * \return 验证是否成功
//  */
// bool LoginSessionManager::verify(const DWORD loginTempID, const DWORD accid,char *numPassword, DES_cblock *key)
// {
// 	Zebra::logger->trace("LoginSessionManager::verify");
// 	bool retval = false;
// 	mlock.lock();
// 	LoginSessionHashmap_iterator it = sessionData.find(accid);
// 	if (it != sessionData.end()
// 			&& loginTempID == it->second.session.loginTempID)
// 	{
// 		retval = true;
// 		if (0!=key)
// 		{
// 			bcopy(it->second.session.des_key, key, sizeof(DES_cblock));
// 			//Zebra::logger->debug("复制密钥：%u %u %u %u %u %u %u %u", it->second.session.des_key[0], it->second.session.des_key[1], it->second.session.des_key[2], it->second.session.des_key[3], it->second.session.des_key[4], it->second.session.des_key[5], it->second.session.des_key[6], it->second.session.des_key[7]);
// 		}
// 		bcopy(it->second.session.numpasswd,numPassword,sizeof(it->second.session.numpasswd));
// 		sessionData.erase(it);
// 	}
// 	mlock.unlock();
// 	return retval;
// }

// /**
//  * \brief 更新超时的登陆信息
//  */
// void LoginSessionManager::update(const zRTime &ct)
// {
// 	return ; //soke 返回
// //	Zebra::logger->trace("LoginSessionManager::update");
// 	if (checkUpdateTime(ct))
// 	{
// 		mlock.lock();
// 		if (!sessionData.empty())
// 		{
// 			zTime currentTime;
// 			LoginSessionHashmap_iterator it;
// 			for(it = sessionData.begin(); it != sessionData.end();)
// 			{
// 				if (it->second.timestamp.elapse(currentTime) >= 10)
// 				{
// 					Zebra::logger->debug("登陆信息超时：%u, %u, %u", it->second.session.accid, it->second.session.loginTempID, it->second.session.wdGatewayID);
// 					LoginSessionHashmap_iterator tmp = it;
// 					it++;
// 					sessionData.erase(tmp);
// 				}
// 				else
// 					it++;
// 			}
// 		}
// 		mlock.unlock();
// 	}
// }

/*2025-01-08 重构解决登陆体验问题 */
/**
 * \brief 向管理器中添加一条记录
 *
 * \param session 会话信息
 */
void LoginSessionManager::put(const t_NewLoginSession &session)
{
    bool shouldInsert = false;
    LoginSessionHashmap_iterator it;
    // 缩小锁的范围
    {
        mlock.lock();
        it = sessionData.find(session.accid);
        if (it == sessionData.end())
        {
            shouldInsert = true;
        }
        mlock.unlock();
    }
    if (shouldInsert)
    {
        mlock.lock();
        sessionData.insert(LoginSessionHashmap_pair(session.accid, LoginSession(session)));
        mlock.unlock();
    }
    else
    {
        mlock.lock();
        it->second.session = session;
        it->second.timestamp.now();
        mlock.unlock();
    }
}

/**
 * \brief 验证登陆连接是否正确
 *
 * \param loginTempID 登陆临时编号
 * \param accid 登陆帐号
 * \return 验证是否成功
 */
bool LoginSessionManager::verify(const DWORD loginTempID, const DWORD accid, char *numPassword, DES_cblock *key)
{
    bool retval = false;
    LoginSessionHashmap_iterator it;
    mlock.lock();
    it = sessionData.find(accid);
    if (it != sessionData.end() && loginTempID == it->second.session.loginTempID)
    {
        retval = true;
        if (key != NULL)
        {
            bcopy(it->second.session.des_key, key, sizeof(DES_cblock));
        }
        bcopy(it->second.session.numpasswd, numPassword, sizeof(it->second.session.numpasswd));
        sessionData.erase(it);
    }
    else
    {
        // 更详细的日志记录验证失败的情况
        Zebra::logger->error("LoginSessionManager::verify failed for accid: %u, loginTempID: %u", accid, loginTempID);
    }
    mlock.unlock();
    return retval;
}

/**
 * \brief 更新超时的登陆信息
 */
void LoginSessionManager::update(const zRTime &ct)
{
    const int timeoutSeconds = 20; // 调整超时时间，可根据实际情况修改
    mlock.lock();
    if (!sessionData.empty())
    {
        zTime currentTime;
        LoginSessionHashmap_iterator it = sessionData.begin();
        while (it != sessionData.end())
        {
            if (it->second.timestamp.elapse(currentTime) >= timeoutSeconds)
            {
                Zebra::logger->debug("登陆信息超时：%u, %u, %u", it->second.session.accid, it->second.session.loginTempID, it->second.session.wdGatewayID);
                sessionData.erase(it++);
            }
            else
            {
                ++it;
            }
        }
    }
    mlock.unlock();
}
