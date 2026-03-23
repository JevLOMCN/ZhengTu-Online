/**
 * \file
 * \version  $Id: CDareSeptNpc.h  $
 * \author  
 * \date 
 * \brief 家族NPC争夺战
 *
 * 
 */

#ifndef _CDARE_SEPT_NPC_H
#define _CDARE_SEPT_NPC_H
#include "CDare.h"
#include "CDareSept.h"

/**
 * \brief 家族NPC争夺战 
 *
 */
class CDareSeptNpc : public CDareSept
{
	public:
		CDareSeptNpc(DWORD active_time, DWORD ready_time);
		virtual ~CDareSeptNpc();

	public:		
		virtual void setSecondID(DWORD dwID){CDareSept::setSecondID(dwID);}
		virtual void addFirstID(DWORD dwID){CDareSept::addFirstID(dwID);}
		
		virtual void sendCmdToAllDarePlayer(Cmd::Session::t_enterWar_SceneSession* cmd, unsigned int cmdLen, unsigned int relationID)
		{
			CDareSept::sendCmdToAllDarePlayer(cmd, cmdLen, relationID);
		}
		
		virtual void sendActiveStateToScene(UserSession* pUser)
		{
			CDareSept::sendActiveStateToScene(pUser);
		}

		/**
  		  * \brief 对战结果通知
		  *
		  * \param winner_type 结果类型:0 挑战者胜， 1 应战者胜， 2 战平
		  *		  
		  */
 		virtual void notifyWarResult(int winner_type){};

		/**
		 * \brief 战斗加分
		 *
		 *  调用一次，给相应的社会团队加上PK人数一人。得分公式如下：
		 *  grade = grade + user.level/10;
		 *
		 * \param pAtt 攻击方
		 * \param pDef 防守方
		 */
		virtual void addGrade(UserSession* pAtt, UserSession* pDef){};
		
		virtual void setReadyOverState();
};
#endif

