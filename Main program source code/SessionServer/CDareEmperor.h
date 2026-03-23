/**
 * \file
 * \version  $Id: CDareEmperor.h $
 * \author 
 * \date
 * \brief 皇城争夺战
 *
 * 
 */

#ifndef _CDARE_EMPEROR_H
#define _CDARE_EMPEROR_H
#include "CDare.h"

class CDareEmperor : public CDare
{
	public:
		CDareEmperor();
		CDareEmperor(DWORD active_time, DWORD ready_time);
		virtual ~CDareEmperor();

	public:		
		// 以下为设置对战状态的处理方法。对战状态具体查看对战系统状态转换图
		virtual void setSecondID(DWORD dwID);
		virtual void addFirstID(DWORD dwID);
		virtual bool isInvalid();

		virtual void setActiveState();
		virtual void setReadyOverState();
		
		virtual void sendCmdToAllDarePlayer(Cmd::Session::t_enterWar_SceneSession* cmd, unsigned int cmdLen, unsigned int relationID)
		{}

		virtual void addGrade(UserSession* pAtt, UserSession* pDef){};
		virtual void notifyWarResult(int winner_type) {};

		virtual void timer();
};

#endif

