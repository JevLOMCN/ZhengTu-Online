/**
 * \file
 * \version  $Id: CDareSeptNpc.cpp  $
 * \author 
 * \date 
 * \brief 家族NPC争夺战
 *
 * 
 */

#include "CDareSeptNpc.h"

CDareSeptNpc::CDareSeptNpc(DWORD active_time, DWORD ready_time):CDareSept(active_time, ready_time)
{
}

CDareSeptNpc::~CDareSeptNpc()
{
}

void CDareSeptNpc::setReadyOverState()
{
	Cmd::Session::t_enterWar_SceneSession exit_war; // 通知场景，退出对战状态

	exit_war.dwWarType = this->type;
	exit_war.dwStatus = 0;

	for (DWORD i=0; i<attList.size(); i++)
	{
		this->sendCmdToAllDarePlayer(&exit_war, sizeof(exit_war), attList[i]);
	}
	
	this->sendCmdToAllDarePlayer(&exit_war, sizeof(exit_war), this->secondID);

	this->setOverState();

}

