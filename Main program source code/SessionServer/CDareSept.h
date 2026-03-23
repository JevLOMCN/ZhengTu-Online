/**
 * \file
 * \version  $Id: CDareSept.h $
 * \author  
 * \date 
 * \brief 家族随意战
 *
 * 
 */

#ifndef _CDARE_SEPT_H
#define _CDARE_SEPT_H
#include "CDare.h"

class CDareSept : public CDare
{
	public:
		CDareSept();
		CDareSept(DWORD active_time, DWORD ready_time);
		virtual ~CDareSept();

	public:		
		// 以下为设置对战状态的处理方法。对战状态具体查看对战系统状态转换图
		virtual void setSecondID(DWORD dwID);
		virtual void addFirstID(DWORD dwID);
		virtual bool isInvalid();

		virtual void setReadyQuestionState();
		virtual void setReadyActiveState(UserSession* pUser);
		virtual void setReturnGoldState();
		virtual void setReadyOverState();
		
		virtual void sendCmdToAllDarePlayer(Cmd::Session::t_enterWar_SceneSession* cmd, unsigned int cmdLen, unsigned int relationID);
		virtual void sendActiveStateToScene(UserSession* pUser);
 		virtual void notifyWarResult(int winner_type);

		/**
		 * \brief 战斗加分
		 *
		 *  调用一次，给相应的社会团队加上PK人数一人。得分公式如下：
		 *  grade = grade + user.level/10;
		 *
		 * \param pAtt 攻击方
		 * \param pDef 防守方
		 */
		virtual void addGrade(UserSession* pAtt, UserSession* pDef);
		virtual void timer();

		int dwDareRepute;
	protected:		
		virtual char* getFirstName();
		virtual DWORD getSecondUserID();

};

#endif

