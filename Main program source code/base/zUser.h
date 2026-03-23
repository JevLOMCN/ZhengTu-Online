/**
 * \file
 * \version  $Id: zUser.h  $
 * \author  
 * \date 
 * \brief 游戏角色定义
 */

#ifndef _ZUSER_H_
#define _ZUSER_H_

#include "zSceneEntry.h"
#include "Zebra.h"

/**
 * \brief 角色定义类,有待扩充
 */
struct zUser:public zSceneEntry
{
	zUser():zSceneEntry(SceneEntry_Player)
	{
	}
	void lock()
	{
		//Zebra::logger->debug("lockuser");
		mlock.lock();
	}

	void unlock()
	{
		//Zebra::logger->debug("unlockuser");
		mlock.unlock();
	}

	private:
		zMutex mlock;
};

#endif

