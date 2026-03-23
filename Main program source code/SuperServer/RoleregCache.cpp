/**
 * \file
 * \version  $Id: RoleregCache.cpp  $
 * \author  
 * \date 
 * \brief 缓冲一些角色名称相关的指令
 */


#include "zType.h"
#include "zMutex.h"
#include "zTime.h"
#include "SuperCommand.h"
#include "RoleregCache.h"
#include "RoleregCommand.h"
#include "RoleregClient.h"
#include "RoleregClientManager.h"
#include "SuperServer.h"
#include "Zebra.h"

RoleregCache *RoleregCache::instance = NULL;

void RoleregCache::add(const Cmd::Super::t_Charname_Gateway &cmd)
{
	Data d(cmd);
	mlock.lock();
	datas.push_back(d);
	mlock.unlock();
}

void RoleregCache::timeAction(const zTime &ct)
{
	if (actionTimer.elapse(ct) > 10)
	{
		mlock.lock();
		while(!datas.empty())
		{
			Data &rd = datas.front();
			Cmd::RoleReg::t_Charname_Rolereg cmd;
			cmd.wdServerID = rd.wdServerID;
			cmd.accid = rd.accid;
			cmd.gameZone = SuperService::getInstance().getZoneID();
			bcopy(rd.name, cmd.name, sizeof(cmd.name));
			cmd.state = rd.state;
			if (RoleregClientManager::getInstance().broadcastOne(&cmd, sizeof(cmd)))
			{
				Zebra::logger->debug("处理缓冲中的角色名称指令完成：%u, %s, %u", rd.accid, rd.name, rd.state);
				datas.pop_front();
			}
			else
				break;
		}
		mlock.unlock();
		actionTimer = ct;
	}
}

