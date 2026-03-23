#include "public.h"
#include "miniusercommand.h"
#include "GameGuiManager.h"
#include "./guimessagebox.h"
#include "./Game.h"
#include "./GameScene.h"
#include "./GameAccelKeyManager.h"
#include "./GuiMain.h"
#include "./MainCharacter.h"
#include "./GameState.h"
#include "./RoleItem.h"
#include "./GuiTeam.h"
#include "./GuiWorldConn.h"
#include "./CreateRole.h"
#include "./GuiNpcTrade.h"
#include "./GuiSalaryTrade.h" //工资
#include "./GuiTicketTrade.h" //积分
#include "./GuiChongzhiTrade.h" //充值点
#include "../gui/include/GuiFloatItem.h"
#include "guitaskdialog.h"
#include "GuiShopDialog.h"
#include "GuiPetDialog.h"
#include "GuiUserInfo.h"
#include "./GuiBallot.h"
#include "./GuiCompetition.h"
#include "./GuiRecvMailDialog.h"
#include "./GuiAuctionDialog.h"
#include "./GuiOthersShopDialog.h"
#include "./GuiChatSelect.h"
#include "./GuiGmMsgBoard.h"
#include ".\gamecursor.h"
#include "./GuiMoneyInputBox.h"
#include "./CountryInfo.h"
#include "./GuiSelectMarryType.h"
#include "./GuiSelectSeptDareType.h"
#include "./GuiSystem.h"
#include "./MGNumberDlg.h"
#include "./LordStrikeGameBkDlg.h"
#include "./LordStrikeManager.h"
#include "./LordStrikeLobby.h"
#include "./LordStrikeLobbyDlg.h"
#include "SafetyUser.h"
#include ".\guiitem.h"
#include "./DlgEquiphandbook.h" //soke 百兽图鉴

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CGuiMessageBox::CGuiMessageBox(enumGuiActionType type,void* pData)
: m_type(type)
, m_pData(pData)
, m_fTime(0.0f)
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiMessageBox::OnClose(void)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

HRESULT CGuiMessageBox::OnRender( float fElapsedTime )
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	OnTimeOut(fElapsedTime);
	return hr;
	FUNCTION_END;
}

void CGuiMessageBox::OnTimeOut(float fElapsedTime)
{
	FUNCTION_BEGIN;
	m_fTime += fElapsedTime;

	switch(m_type) {
	case eAction_ExitGameByUnlaw:
		{
			if (m_fTime>10.0f)
			{
				GetGameApplication()->Exit();
				Close();
			}
		}
		break;
	case eAction_RequestTrade:
		{
			if (m_fTime>30.0f)
			{
				ProcessRequestTrade(GUI_IDCANCEL);
				Close();
			}
		}
		break;
	case eAction_Competition:
		{
			if (m_fTime>60.0f)
			{
				Competition(GUI_IDCANCEL);
				Close();
			}
		}
		break;
	case eAction_TongUser:
		{
			if (m_fTime>30.0f)
			{
				AddTongUser(255);
				Close();
			}
		}
		break;
	case eAction_SeptUser:
		{
			if (m_fTime>30.0f)
			{
				AddSeptUser(255);
				Close();
			}
		}
		break;
	case eAction_Tong:
		{
			if (m_fTime>30.0f)
			{
				TongQuery(GUI_IDCANCEL);
				Close();
			}
		}
		break;
	case eAction_Sept:
		{
			if (m_fTime>30.0f)
			{
				SeptQuery(GUI_IDCANCEL);
				Close();
			}
		}
		break;
	case eAction_Friend:
		{
			if (m_fTime>30.0f)
			{
				FriendQuery(GUI_IDCANCEL);
				Close();
			}
		}
		break;
	case eAction_AnswerTeam_Honor:
		{
			if (m_fTime>30.0f)
			{
				ProcessAnswerTeam(GUI_IDCANCEL,TEAM_HONOR);
				Close();
			}
		}
		break;
	case eAction_AnswerTeam_Normal:
		{
			if (m_fTime>10.0f) //soke 组队时间改成10秒，超过时间自动拒绝 if (m_fTime>30.0f)
			{
				ProcessAnswerTeam(GUI_IDCANCEL,TEAM_NORMAL);
				Close();
			}
		}
		break;
	case eAction_BuyMonthCard:
		{
			//请求兑换月卡
			stRequestRedeemMonthCard cmd;
			SEND_USER_CMD(cmd);
			Close();
		}
		break;
	case eAction_CreateGen:
		{
			if (m_fTime>60.0f)
			{
				ActionBecomeArmyGen(GUI_IDCANCEL);
				Close();
			}
		}
		break;
	}
	FUNCTION_END;
}
/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  购买物品
*/
void CGuiMessageBox::ProcessBuyItem(UINT nID)
{
	FUNCTION_BEGIN;

	switch(nID)
	{
	case GUI_IDOK:
		{
			stObjectLocation * pos = (stObjectLocation*)m_pData;
			CRoleItem* pItem = GetScene()->GetMainCharacter()->FindItemByLocation(*pos);
			if(pItem)
			{
				stBuyObjectNpcTradeUserCmd cmd;
				cmd.dwObjectID = pItem->GetObjectID();
				cmd.dwNum = pItem->GetCount();
				//soke 高级杂货
				cmd.nLevel = pItem->GetUpGrade();
				SEND_USER_CMD(cmd);
			}
		}
		break;
	case GUI_IDCANCEL:
		break;
	}

	if (GetGameGuiManager()->m_guiNpcDialog)
		GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  购买物品(工资)
*/
void CGuiMessageBox::ProcessSalaryBuyItem(UINT nID)
{
	FUNCTION_BEGIN;

	switch(nID)
	{
	case GUI_IDOK:
		{
			stObjectLocation * pos = (stObjectLocation*)m_pData;
			CRoleItem* pItem = GetScene()->GetMainCharacter()->FindItemByLocation(*pos);
			if(pItem)
			{
				stBuyObjectSalaryTradeUserCmd cmd;
				cmd.dwObjectID = pItem->GetObjectID();
				cmd.dwNum = pItem->GetCount();
				//soke 高级杂货
				cmd.nLevel = pItem->GetUpGrade();
				SEND_USER_CMD(cmd);
			}
		}
		break;
	case GUI_IDCANCEL:
		break;
	}

	if (GetGameGuiManager()->m_guiNpcDialog)
		GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL

	FUNCTION_END;
}


/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  购买物品(积分)
*/
void CGuiMessageBox::ProcessTicketBuyItem(UINT nID)
{
	FUNCTION_BEGIN;

	switch(nID)
	{
	    case GUI_IDOK:
		{
			stObjectLocation * pos = (stObjectLocation*)m_pData;
			CRoleItem* pItem = GetScene()->GetMainCharacter()->FindItemByLocation(*pos);
			if(pItem)
			{	
				stBuyObjectTicketTradeUserCmd cmd;
				cmd.dwObjectID = pItem->GetObjectID();
				cmd.dwNum = pItem->GetCount();
				cmd.nLevel = pItem->GetUpGrade();
				SEND_USER_CMD(cmd);
				
			}

		} 	 
		break;
	    case GUI_IDCANCEL:
		break;
	}

	if (GetGameGuiManager()->m_guiNpcDialog)
		GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL

	FUNCTION_END;
}
/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  购买物品(充值点)
*/
void CGuiMessageBox::ProcessChongzhiBuyItem(UINT nID)
{
	FUNCTION_BEGIN;

	switch(nID)
	{
	    case GUI_IDOK:
		{
			stObjectLocation * pos = (stObjectLocation*)m_pData;
			CRoleItem* pItem = GetScene()->GetMainCharacter()->FindItemByLocation(*pos);
			if(pItem)
			{	
				stBuyObjectChongzhiTradeUserCmd cmd;
				cmd.dwObjectID = pItem->GetObjectID();
				cmd.dwNum = pItem->GetCount();
				cmd.nLevel = pItem->GetUpGrade();
				SEND_USER_CMD(cmd);
				
			}

		} 	 
		break;
	    case GUI_IDCANCEL:
		break;
	}

	if (GetGameGuiManager()->m_guiNpcDialog)
		GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL

	FUNCTION_END;
}
/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  请求交易
*/
void CGuiMessageBox::ProcessSelfRequestTrade(UINT nID)
{
	FUNCTION_BEGIN;

	switch(nID)
	{
	case GUI_IDOK:
		{
			// 发送请求交易指令
			stRequestTradeUserCmd cmd;
			cmd.dwAskerTempID = GetScene()->GetMainCharacter()->GetID();
			cmd.dwAnswerTempID = (DWORD)m_pData;
			SEND_USER_CMD(cmd);
			if(g_bSinglePlayer)
			{
				GetClient()->PushCommand(&cmd,sizeof(cmd));
			}
		}
		break;
	case GUI_IDCANCEL:
		break;
	}
	GetGameState()->SetStateItem(eGameStateItem_Normal);

	FUNCTION_END;
}

void CGuiMessageBox::TongBallot(UINT nID,int ballot)
{
	FUNCTION_BEGIN;

	stBallot* pBallot = (stBallot*) m_pData;

	if (nID == GUI_IDOK)
	{
		stVoteUnionCmd cmd;
		strncpy(cmd.unionName,pBallot->szBallot,sizeof(cmd.unionName));
		cmd.bySuccess = ballot;
		SEND_USER_CMD(cmd);

	}
	if (GetGameGuiManager()->m_guiNpcDialog)
		GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;
	FUNCTION_END;
}

void CGuiMessageBox::SeptBallot(UINT nID,int ballot)
{
	FUNCTION_BEGIN;
	stBallot* pBallot = (stBallot*) m_pData;

	if (nID == GUI_IDOK)
	{
		stVoteSeptCmd cmd;
		strncpy(cmd.septName,pBallot->szBallot,sizeof(cmd.septName));
		cmd.bySuccess = ballot;
		SEND_USER_CMD(cmd);

	}
	if (GetGameGuiManager()->m_guiNpcDialog)
		GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;

	FUNCTION_END;
}

void CGuiMessageBox::LeaveCountry(UINT nID)
{
	FUNCTION_BEGIN;
	
	if (nID == GUI_IDOK)
	{
		stCancelCountryCmd cmd;

		SEND_USER_CMD(cmd);
	}

	if (GetGameGuiManager()->m_guiNpcDialog)
		GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;

	GetGameGuiManager()->EndNpcDialog();
	FUNCTION_END;
}


void CGuiMessageBox::AddCountry(UINT nID)
{
	FUNCTION_BEGIN;


	if (nID == GUI_IDOK)
	{
		/*stApplyCountryCmd cmd;
		cmd.dwToCountryID = (DWORD) m_pData;
		SEND_USER_CMD(cmd);*/
		stChangeCountryCmd cmd;
		cmd.dwToCountryID = (DWORD) m_pData;
		SEND_USER_CMD(cmd);
	}

	if (GetGameGuiManager()->m_guiNpcDialog)
		GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;

	GetGameGuiManager()->EndNpcDialog();

	FUNCTION_END;
}

//soke 工资移民加入国家
void CGuiMessageBox::AddCountrySalary(UINT nID)
{
	FUNCTION_BEGIN;


	if (nID == GUI_IDOK)
	{
		/*stApplyCountryCmd cmd;
		cmd.dwToCountryID = (DWORD) m_pData;
		SEND_USER_CMD(cmd);*/
		stChangeCountrySalaryCmd cmd;
		cmd.dwToCountryID = (DWORD) m_pData;
		SEND_USER_CMD(cmd);
	}

	if (GetGameGuiManager()->m_guiNpcDialog)
		GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;

	GetGameGuiManager()->EndNpcDialog();

	FUNCTION_END;
}

//soke 领取工资
void CGuiMessageBox::Add2Salary(UINT nID)
{
	FUNCTION_BEGIN;


	if (nID == GUI_IDOK)
	{
		stAddCountrySalaryCmd cmd;
		SEND_USER_CMD(cmd);
	}

	if (GetGameGuiManager()->m_guiNpcDialog)
		GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;

	GetGameGuiManager()->EndNpcDialog();

	FUNCTION_END;
}

//soke 领取登陆奖励
void CGuiMessageBox::Add2Login(UINT nID)
{
	FUNCTION_BEGIN;


	if (nID == GUI_IDOK)
	{
		stAddCountryLoginCmd cmd;
		SEND_USER_CMD(cmd);
	}

	if (GetGameGuiManager()->m_guiNpcDialog)
		GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;

	GetGameGuiManager()->EndNpcDialog();

	FUNCTION_END;
}

//soke 领取vip奖励
void CGuiMessageBox::Add2Vip(UINT nID)
{
	FUNCTION_BEGIN;


	if (nID == GUI_IDOK)
	{
		stAddCountryVipCmd cmd;
		SEND_USER_CMD(cmd);
	}

	if (GetGameGuiManager()->m_guiNpcDialog)
		GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;

	GetGameGuiManager()->EndNpcDialog();

	FUNCTION_END;
}

//soke 副本激活
void CGuiMessageBox::AddFubenjh(UINT nID)
{
	FUNCTION_BEGIN;


	if (nID == GUI_IDOK)
	{
		stClearUserSkillPointFubenjhUserCmd cmd;
		SEND_USER_CMD(cmd);
	}

	if (GetGameGuiManager()->m_guiNpcDialog)
		GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;

	GetGameGuiManager()->EndNpcDialog();

	FUNCTION_END;
}

void CGuiMessageBox::HonorToPk(UINT nID)
{
	FUNCTION_BEGIN;

	if (nID == GUI_IDOK)
	{
		if (GetScene()->GetMainCharacter()->GetProperty()->honor>0)
		{
			stHonorToPkValuePropertyUserCmd cmd;
			
			SEND_USER_CMD(cmd);
		}
		else
			GetGameGuiManager()->AddClientSystemMessage("对不起， 你没有荣誉点！");
	}
	if (GetGameGuiManager()->m_guiNpcDialog)
		GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;
	FUNCTION_END;
}

void CGuiMessageBox::CompetitionNpc(UINT nID)
{
	FUNCTION_BEGIN;
	if (nID == GUI_IDOK)
	{
		stCreateQuiz cmd;
		cmd.byType = CREATE_YES;
		SEND_USER_CMD(cmd);

		if(g_bSinglePlayer)
		{

			stQuizParam _cmd;
			_cmd.byReadQuestionTime = 5;
			_cmd.byAnswerQuestionTime = 10;
			_cmd.byCalcTime = 5;
			_cmd.bySubjectNumber = 30;
			_cmd.byType = QUIZ_PERSONAL;
			GetClient()->PushCommand(&_cmd,sizeof(stQuizParam));


			stQuestionQuiz* pCmd;
			pCmd=(stQuestionQuiz*)malloc(sizeof(stQuestionQuiz)+1024);

			memset(pCmd,0,sizeof(stQuestionQuiz)+1024);
			pCmd->byCmd=QUIZ_USERCMD;
			pCmd->byParam=QUESTION_QUIZ_PARA;
			pCmd->dwID=100;
			pCmd->bySpareTime=20;
			pCmd->dwSize=5;
			char* pStr=pCmd->subject;
			sprintf(pStr,"%s","三国演义中吕布是被谁杀死的？");

			pStr+=strlen(pStr)+1;
			sprintf(pStr,"%s","刘备");

			pStr+=strlen(pStr)+1;
			sprintf(pStr,"%s","曹操");

			pStr+=strlen(pStr)+1;
			sprintf(pStr,"%s","董卓");

			pStr+=strlen(pStr)+1;
			sprintf(pStr,"%s","孙权");

			pStr+=strlen(pStr)+1;
			sprintf(pStr,"%s","关羽");

			pStr+=strlen(pStr)+1;
			sprintf(pStr,"%s","华雄");

			GetClient()->PushCommand(pCmd,sizeof(stQuestionQuiz)+1024);

		}
	}
	//test
	else
	{
		GetGameGuiManager()->OnNpcActionEnd();

		if(g_bSinglePlayer)
		{
			stQueryQuiz cmd;
			GetClient()->PushCommand(&cmd,sizeof(cmd));
		}
	}
	//
	if(GetGameGuiManager()->m_guiNpcDialog)
	{
		GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;
	}
	FUNCTION_END;

}

void CGuiMessageBox::Competition(UINT nID)
{
	if (nID == GUI_IDOK)
	{
		if (GetGameGuiManager()->m_guiCompetition)
			GetGameGuiManager()->m_guiCompetition->Close();

		stQueryQuiz cmd;
		cmd.byMsgType = QUIZ_YES;
		SEND_USER_CMD(cmd);

		if(g_bSinglePlayer)
		{
			stQuizParam _cmd;
			_cmd.byReadQuestionTime = 5;
			_cmd.byAnswerQuestionTime = 10;
			_cmd.byCalcTime = 5;
			_cmd.bySubjectNumber = 30;
			_cmd.byType = QUIZ_WORLD;
			GetClient()->PushCommand(&_cmd,sizeof(stQuizParam));


			stQuestionQuiz* pCmd;
			pCmd=(stQuestionQuiz*)malloc(sizeof(stQuestionQuiz)+1024);

			memset(pCmd,0,sizeof(stQuestionQuiz)+1024);
			pCmd->byCmd=QUIZ_USERCMD;
			pCmd->byParam=QUESTION_QUIZ_PARA;
			pCmd->dwID=100;
			pCmd->bySpareTime=20;
			pCmd->dwSize=7;
			char* pStr=pCmd->subject;
			sprintf(pStr,"%s","三国演义中吕布是被谁杀死的？");

			pStr+=strlen(pStr)+1;
			sprintf(pStr,"%s","刘备");

			pStr+=strlen(pStr)+1;
			sprintf(pStr,"%s","曹操");

			pStr+=strlen(pStr)+1;
			sprintf(pStr,"%s","董卓");

			pStr+=strlen(pStr)+1;
			sprintf(pStr,"%s","孙权");

			pStr+=strlen(pStr)+1;
			sprintf(pStr,"%s","关羽");

			pStr+=strlen(pStr)+1;
			sprintf(pStr,"%s","华雄");

			GetClient()->PushCommand(pCmd,sizeof(stQuestionQuiz)+1024);

		}
	}
	else
	{
		GetGameGuiManager()->AddClientSystemMessage("你放弃了这期的智力竞赛！");
	}
	

}
/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  回答交易请求
*/
void CGuiMessageBox::ProcessRequestTrade(UINT nID)
{
	FUNCTION_BEGIN;

	stAnswerTradeUserCmd cmd;
	stRequestTradeUserCmd* pData = (stRequestTradeUserCmd*) m_pData;
	cmd.dwAskerTempID = pData->dwAskerTempID;
	cmd.dwAnswerTempID = pData->dwAnswerTempID;
	if(!pData) return;
	switch(nID)
	{
	case GUI_IDOK:
		{
			cmd.byAgree = true;
			SEND_USER_CMD(cmd);
			if(g_bSinglePlayer)
			{
				stBeginTradeUserCmd cmd1;
				cmd1.dwAskerTempID = pData->dwAskerTempID;
				cmd1.dwAnswerTempID = pData->dwAnswerTempID;
				GetClient()->PushCommand(&cmd1,sizeof(cmd1));
			}
		}
		break;
	case GUI_IDCANCEL:
		cmd.byAgree = false;
		SEND_USER_CMD(cmd);
		break;
	}
	GetGameState()->SetStateItem(eGameStateItem_Normal);

	FUNCTION_END;
}

/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  离开队伍
*/
void CGuiMessageBox::ProcessExitTeam(UINT nID)
{
	FUNCTION_BEGIN;

	switch(nID)
	{
	case GUI_IDOK:
		{
			if( !GetGameGuiManager()->m_guiTeam )
				return;
			if( !GetGameGuiManager()->m_guiTeam->IsTeam())
				return ;
			stRemoveTeamMemberUserCmd cmd;
			cmd.dwTeamID = (DWORD)m_pData;
			//strncpy(cmd.pstrName,GetScene()->GetMainCharacter()->GetID(),MAX_NAMESIZE);
			cmd.dwTempID = GetScene()->GetMainCharacter()->GetID();
			SEND_USER_CMD(cmd);
		}
		break;
	case GUI_IDCANCEL:
		break;
	}

	FUNCTION_END;
}
/*
*	函数:		
*	返回值：	
*	参数：		
*	
*	用途: 		  回答组队请求
*/
void CGuiMessageBox::ProcessAnswerTeam(UINT nID,DWORD nMode)
{
	FUNCTION_BEGIN;

	assert(nMode == TEAM_NORMAL || nMode ==TEAM_HONOR);
	if( nMode!= TEAM_NORMAL && nMode !=TEAM_HONOR)
		return;

#ifndef __REMOTE__CAN__MAKE__TEAM__
	stAnswerTeamUserCmd cmd;
	cmd.dwAnswerUserID = GetScene()->GetMainCharacter()->GetID();
	cmd.dwTeamMode = nMode;
	cmd.dwRequestUserID = (DWORD)m_pData;
	switch(nID)
	{
	case GUI_IDOK:
		cmd.byAgree = 1;
		break;
	case GUI_IDCANCEL:
		cmd.byAgree = 0;
		break;
	}
	SEND_USER_CMD(cmd);

	if( TEAM_NORMAL == nMode)
		GetQueryManager()->DeleteListData(eAction_AnswerTeam_Normal,cmd.dwRequestUserID);
	else if( TEAM_HONOR == nMode)
		GetQueryManager()->DeleteListData(eAction_AnswerTeam_Honor,cmd.dwRequestUserID);

#else 
	if(GetGameGuiManager()->m_guiTeam)
	{
		stAnswerNameTeamUserCmd cmd;
		cmd.dwTeamMode = nMode;
		strncpy(cmd.byRequestUserName,GetGameGuiManager()->m_guiTeam->GetBackUpAnswerUserName(),sizeof(cmd.byRequestUserName));
		strncpy(cmd.byAnswerUserName,GetScene()->GetMainCharacter()->GetName(),sizeof(cmd.byAnswerUserName));
		switch(nID)
		{
			case GUI_IDOK:
				cmd.byAgree = 1;
				break;
			case GUI_IDCANCEL:
				cmd.byAgree = 0;
				break;
		}
		SEND_USER_CMD(cmd);
	}	
#endif 

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nID : 描述
 * \return 返回值的描述
 */
void CGuiMessageBox::DelUser(UINT nID)
{
	FUNCTION_BEGIN;

	switch(nID)
	{
	case GUI_IDOK:
		{
			stDeleteSelectUserCmd cmd;
			cmd.charNo=(DWORD)m_pData;
			SEND_USER_CMD(cmd);
                        if(GetCreateRoleManager()->m_guiRoleSel)
				GetCreateRoleManager()->m_guiRoleSel->SetEnabled(false);
			
		}
		break;
	}
	
	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param nID : 描述
* \return 返回值的描述
*/
void CGuiMessageBox::TongQuery(UINT nID)
{
	FUNCTION_BEGIN;

	stAddMemberToUnionCmd cmd;
	cmd.memberID = (DWORD)m_pData;
	
	switch(nID)
	{
	case GUI_IDOK:
		cmd.byState = ANSWER_YES;
		break;
	case GUI_IDCANCEL:
		cmd.byState = ANSWER_NO;
		break;
	}
	SEND_USER_CMD(cmd);

	GetQueryManager()->DeleteListData(eAction_Tong,cmd.memberID);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nID : 描述
 * \return 返回值的描述
 */
void CGuiMessageBox::SeptQuery(UINT nID)
{
	FUNCTION_BEGIN;

	stAddMemberToSeptCmd cmd;
	cmd.memberID = (DWORD)m_pData;

	switch(nID)
	{
	case GUI_IDOK:
		cmd.byState = SEPT_ANSWER_YES;
		break;
	case GUI_IDCANCEL:
		cmd.byState = SEPT_ANSWER_NO;
		break;
	}
	SEND_USER_CMD(cmd);

	GetQueryManager()->DeleteListData(eAction_Sept,cmd.memberID);

	FUNCTION_END;
}

void CGuiMessageBox::AddTongUser(UINT nID)
{
	FUNCTION_BEGIN;

	stRequestJoinUnionCmd cmd;
	strncpy(cmd.name,(const char*)m_pData,MAX_NAMESIZE);

	switch(nID)
	{
	case GUI_IDOK:
		AddTong(cmd.name);
		break;
	case GUI_IDCANCEL:
		cmd.status = REQUEST_JOIN_CANCEL;
		SEND_USER_CMD(cmd);
		break;
	default:
		cmd.status = REQUEST_JOIN_TIMEOUT;
		SEND_USER_CMD(cmd);
		break;
	}
	

	GetQueryManager()->DeleteListData(eAction_TongUser,(DWORD)m_pData);

	FUNCTION_END;
}

void CGuiMessageBox::AddSeptUser(UINT nID)
{
	FUNCTION_BEGIN;

	stRequestJoinSeptCmd cmd;
	strncpy(cmd.name,(const char*)m_pData,MAX_NAMESIZE);

	switch(nID)
	{
	case GUI_IDOK:
		AddSept(cmd.name);
		break;
	case GUI_IDCANCEL:
		cmd.status = REQUEST_JOIN_CANCEL;
		SEND_USER_CMD(cmd);
		break;
	default:
		cmd.status = REQUEST_JOIN_TIMEOUT;
		SEND_USER_CMD(cmd);
		break;
	}

	GetQueryManager()->DeleteListData(eAction_SeptUser,(DWORD)m_pData);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nID : 描述
 * \return 返回值的描述
 */
void CGuiMessageBox::FriendQuery(UINT nID)
{
	FUNCTION_BEGIN;

	stRelationStatusCmd cmd;
	cmd.userid = (DWORD)m_pData;
	cmd.type=RELATION_TYPE_FRIEND;
	switch(nID)
	{
	case GUI_IDOK:
		cmd.byState = RELATION_ANSWER_YES;
		break;
	case GUI_IDCANCEL:
		cmd.byState = RELATION_ANSWER_NO;
		break;
	}
	SEND_USER_CMD(cmd);

	GetQueryManager()->DeleteListData(eAction_Friend,cmd.userid);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nID : 描述
 * \return 返回值的描述
 */
void CGuiMessageBox::TeacherQuery(UINT nID)
{
	FUNCTION_BEGIN;

	stAddMemberToSchoolCmd cmd;
	cmd.memberID = (DWORD)m_pData;
	switch(nID)
	{
	case GUI_IDOK:
		cmd.byState = TEACHER_ANSWER_YES;
		break;
	case GUI_IDCANCEL:
		cmd.byState = TEACHER_ANSWER_NO;
		break;
	}
	SEND_USER_CMD(cmd);

	GetQueryManager()->DeleteListData(eAction_Teacher,cmd.memberID);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nID : 描述
 * \return 返回值的描述
 */
void CGuiMessageBox::ExitSchool(UINT nID)
{
	FUNCTION_BEGIN;

	stSchoolMemberLeaveSchoolCmd  cmd;
	switch(nID)
	{
	case GUI_IDOK:
		SEND_USER_CMD(cmd);
		break;
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nID : 描述
 * \return 返回值的描述
 */
void CGuiMessageBox::ExitTong(UINT nID)
{
	FUNCTION_BEGIN;

	stUnionMemberLeaveUnionCmd cmd;

	switch(nID)
	{
	case GUI_IDOK:
		SEND_USER_CMD(cmd);
		break;
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nID : 描述
 * \return 返回值的描述
 */
void CGuiMessageBox::ExitSept(UINT nID)
{
	FUNCTION_BEGIN;

	stSeptMemberLeaveSeptCmd cmd;

	switch(nID)
	{
	case GUI_IDOK:
		SEND_USER_CMD(cmd);
		break;
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nID : 描述
 * \return 返回值的描述
 */
void CGuiMessageBox::DelTong(UINT nID)
{
	FUNCTION_BEGIN;

	stFireMemberFromUnionCmd cmd;
	switch(nID)
	{
	case GUI_IDOK:
		{
			stUnionRember* pRember = (stUnionRember*) m_pData;
			strncpy(cmd.memberName,pRember->memberName,
				sizeof(cmd.memberName));
	
			SEND_USER_CMD(cmd);
		}
		
		break;
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nID : 描述
 * \return 返回值的描述
 */
void CGuiMessageBox::DelSept(UINT nID)
{
	FUNCTION_BEGIN;

	stFireMemberFromSeptCmd cmd;

	switch(nID)
	{
	case GUI_IDOK:
		{
			stSeptRember* pRember = (stSeptRember*) m_pData;
			strncpy(cmd.memberName,pRember->memberName,
				sizeof(cmd.memberName));
			SEND_USER_CMD(cmd);
		}
		break;
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nID : 描述
 * \return 返回值的描述
 */
void CGuiMessageBox::DelSchool(UINT nID)
{
	FUNCTION_BEGIN;

	stFireMemberFromSchoolCmd cmd;
	switch(nID)
	{
	case GUI_IDOK:
		{
			switch(GetGameGuiManager()->m_guiWorldConn->m_SchoolInfo.byStatus) {
			case SCHOOL_STATUS_TEACHER:
				{
					stTeacherMember* pRember = (stTeacherMember*) m_pData;
					strncpy(cmd.memberName,pRember->name,
						sizeof(cmd.memberName));
				}
				break;
			case SCHOOL_STATUS_SCHOOL:
				{
					stSchool* pRember = (stSchool*) m_pData;
					strncpy(cmd.memberName,pRember->name,
						sizeof(cmd.memberName));
				}
				break;
			}
			
			SEND_USER_CMD(cmd);
		}
		break;
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nID : 描述
 * \return 返回值的描述
 */
void CGuiMessageBox::DelFriend(UINT nID)
{
	FUNCTION_BEGIN;

	stRelationStatusCmd cmd;

	switch(nID)
	{
	case GUI_IDOK:
		{
			stRelation* pRember = (stRelation*) m_pData;

			strncpy(cmd.name,pRember->name,
				sizeof(cmd.name));
			cmd.type=RELATION_TYPE_FRIEND;
			cmd.byState=RELATION_REMOVE;

			SEND_USER_CMD(cmd);
		}
		break;
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nID : 描述
 * \return 返回值的描述
 */
void CGuiMessageBox::MarryTaskQuery(UINT nID)
{
	FUNCTION_BEGIN;

	switch(nID)
	{
	case GUI_IDOK:
		{
			
		}
		break;
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nID : 描述
 * \return 返回值的描述
 */
void CGuiMessageBox::DelBad(UINT nID)
{
	FUNCTION_BEGIN;

	stRelationStatusCmd cmd;

	switch(nID)
	{
	case GUI_IDOK:
		{
			stRelation* pRember = (stRelation*) m_pData;

			strncpy(cmd.name,pRember->name,
				sizeof(cmd.name));
			cmd.type=RELATION_TYPE_BAD;
			cmd.byState=RELATION_REMOVE;

			SEND_USER_CMD(cmd);
		}
		break;
	}

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nID : 描述
 * \return 返回值的描述
 */
void CGuiMessageBox::TongAttack(UINT nID)
{
	FUNCTION_BEGIN;

	stActiveDareCmd  cmd;
	cmd.dwWarID = (DWORD) m_pData;
	sprintf(cmd.toRelationName,
		GetGameGuiManager()->m_guiWorldConn->m_TongInfo.unionName,sizeof(cmd.toRelationName));
	cmd.dwDareType = UNION_DARE;
	
	switch(nID)
	{
	case GUI_IDOK:
		cmd.dwMsgType = DARE_YES;
		break;
	case GUI_IDCANCEL:
		cmd.dwMsgType = DARE_NO;
		break;
	}
	SEND_USER_CMD(cmd);

	GetQueryManager()->DeleteListData(eAction_TongAttack,cmd.dwWarID);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nID : 描述
 * \return 返回值的描述
 */
void CGuiMessageBox::SeptAttack(UINT nID)
{
	FUNCTION_BEGIN;
	
	stActiveDareCmd  cmd;
	cmd.dwWarID = (DWORD) m_pData;
	sprintf(cmd.toRelationName,
		GetGameGuiManager()->m_guiWorldConn->m_SeptInfo.septName,sizeof(cmd.toRelationName));
	cmd.dwDareType = SEPT_DARE;

	switch(nID)
	{
	case GUI_IDOK:
		cmd.dwMsgType = DARE_YES;
		break;
	case GUI_IDCANCEL:
		cmd.dwMsgType = DARE_NO;
		break;
	}
	SEND_USER_CMD(cmd);

	GetQueryManager()->DeleteListData(eAction_SeptAttack,cmd.dwWarID);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nID : 描述
 * \return 返回值的描述
 */
void CGuiMessageBox::SchoolAttack(UINT nID)
{
	FUNCTION_BEGIN;
	
	stActiveDareCmd  cmd;
	cmd.dwWarID = (DWORD) m_pData;
	sprintf(cmd.toRelationName,
		GetGameGuiManager()->m_guiWorldConn->m_SchoolInfo.schoolName,sizeof(cmd.toRelationName));
	cmd.dwDareType = SCHOOL_DARE;

	switch(nID)
	{
	case GUI_IDOK:
		cmd.dwMsgType = DARE_YES;
		break;
	case GUI_IDCANCEL:
		cmd.dwMsgType = DARE_NO;
		break;
	}
	SEND_USER_CMD(cmd);

	GetQueryManager()->DeleteListData(eAction_SchoolAttack,cmd.dwWarID);

	FUNCTION_END;
}

/**
 * \brief 退出斗地主画面
 * 
 * 详细描述
 * 
 * \param nID : 描述
 * \return 返回值的描述
 */
void CGuiMessageBox::OnExitLordStrikeLobby(UINT nID)
{ 
	switch(nID)
	{
	case GUI_IDOK:
		CLordStrikeManager::GetSingleton()->LeaveLobby();
		break;
	}

}

void CGuiMessageBox::OnExitLordStrikeRoom(UINT nID)
{
	switch(nID)
	{
	case GUI_IDOK:
		CLordStrikeManager::GetSingleton()->GetLobby()->MeLeaveRoom();
		break;
	}
}

void CGuiMessageBox::OnExitLordStrikeGame(UINT nID)
{
	switch(nID)
	{
	case GUI_IDOK:
		CLordStrikeManager::GetSingleton()->GetLobby()->MeLeaveGame();
		break;
	}
}

bool CGuiMessageBox::OnButtonClick(UINT nID)
{
	FUNCTION_BEGIN;

	if ( 1000 == nID )
		return true;

	switch(m_type)
	{
	case eAction_ExitLordStrikeLobby:
		OnExitLordStrikeLobby(nID);
		Close();
		break;
	/*case eAction_ExitLordStrikeRoom:
		OnExitLordStrikeRoom(nID);
		Close();
		break;*/
	case eAction_ExitLordStrikeGameFromRoom:
		OnExitLordStrikeRoom(nID);
		OnExitLordStrikeLobby(nID);
		Close();
		break;
	case eAction_ExitLordStrikeGameIng:
	case eAction_ExitLordStrikeGame:
		OnExitLordStrikeGame(nID);
		Close();
		break;
	case eAction_ExitLordStrikeLobbyAndGame:
		{
			OnExitLordStrikeGame(nID);
			OnExitLordStrikeLobby(nID);
			Close();
		}
	        break; 
	case eAction_Logout:
		switch(nID)
		{
		case GUI_IDOK:
			{
			
				DWORD dwExitContextID = (DWORD)m_pData;

				if( dwExitContextID == 5)
				{
					CLordStrikeManager::GetSingleton()->LogintoLobby();
					Close();
					break;
				}
				else
				{
					
					CLordStrikeManager::GetSingleton()->LeaveLobby();
					
					//Save access key
					if ( GetGameAccelKeyManager() && GetGameGuiManager()->m_guiMain )
						GetGameAccelKeyManager()->Save( GetGameGuiManager()->m_guiMain->GetCurAccessKeyGroup() );
					//TRACE( "Send cur access key group = %d\n", GetGameGuiManager()->m_guiMain->GetCurAccessKeyGroup() );
					//
					LogoutGame(false);

					switch(dwExitContextID)
					{
					case 1: //// 返回人物界面
						{
							//LogoutGame(false);
						}
						break;
					case 2: // //登出帐号
						{
							GetClient()->Destroy();
						}
						break;
					case 3: //// 切换游戏大区
						{
							STARTUPINFO si;
							PROCESS_INFORMATION pi;

							ZeroMemory( &si, sizeof(si) );
							si.cb = sizeof(si);
							ZeroMemory( &pi, sizeof(pi) );

							// Start the child process. 
							if( !CreateProcess( NULL, // No module name (use command line). 
								"PatchUpdate.exe", // Command line. 
								NULL,             // Process handle not inheritable. 
								NULL,             // Thread handle not inheritable. 
								FALSE,            // Set handle inheritance to FALSE. 
								0,                // No creation flags. 
								NULL,             // Use parent's environment block. 
								NULL,             // Use parent's starting directory. 
								&si,              // Pointer to STARTUPINFO structure.
								&pi )             // Pointer to PROCESS_INFORMATION structure.
								) 
							{

							}

							// Close process and thread handles. 
							CloseHandle( pi.hProcess );
							CloseHandle( pi.hThread );

							// close self 
							PostQuitMessage(0);
						}
						break;
					case 4: // close
						{
							PostQuitMessage(0);
						}
						break;
					}
				}				
				Close();
			}
			break;
		case GUI_IDCANCEL:
			Close();
			break;
		return true;
		}
		break;
	case eAction_GiveUnoinMoney:
		{
			switch(nID)
			{
			case GUI_IDOK:
				{
					CGuiMoneyInputBox* pMoneyInputBox =(CGuiMoneyInputBox*) m_pData;
					pMoneyInputBox->OnGiveUnionMoney(false);
				}
				break;
			}
			Close();
			return true;
		}
		break;
	case eAction_ExitGame:
		switch(nID)
		{
		case GUI_IDOK:
			ExitGame();
			break;
		case GUI_IDCANCEL:
			Close();
			break;
		}
		return true;
		break;
	case eAction_DelUser:
		DelUser(nID);
		Close();
		return true;
		break;
	case eAction_RequestTrade:
		ProcessRequestTrade(nID);
		Close();
		return true;
		break;
	case eAction_SelfRequestTrade:
		ProcessSelfRequestTrade(nID);
		Close();
		return true;
		break;
    case eAction_BuyItem:      //soke 金子
		ProcessBuyItem(nID);
		Close();
		return true;
	case eAction_SalaryBuyItem: //soke 工资
		ProcessSalaryBuyItem(nID);
		Close();
		return true;
	case eAction_TicketBuyItem: //soke 积分
		ProcessTicketBuyItem(nID);
		Close();
		return true;
	case eAction_ChongzhiBuyItem: //soke 充值点
		ProcessChongzhiBuyItem(nID);
		Close();
		return true;			
	case eAction_ExitTeam:
		ProcessExitTeam(nID);
		Close();
		return true;
	case eAction_AnswerTeam_Normal:
		ProcessAnswerTeam(nID,TEAM_NORMAL);
		Close();
		return true;
	case eAction_AnswerTeam_Honor:
		ProcessAnswerTeam(nID,TEAM_HONOR);
		Close();
		return true;
	case eAction_Tong:
		TongQuery(nID);
		Close();
		return true;
	case eAction_Sept:
		SeptQuery(nID);
		Close();
		return true;
	case eAction_TongUser:
		AddTongUser(nID);
		Close();
		return true;
	case eAction_SeptUser:
		AddSeptUser(nID);
		Close();
		return true;
	case eAction_Friend:
		FriendQuery(nID);
		Close();
		return true;
	case eAction_Teacher:
		TeacherQuery(nID);
		Close();
		return true;
	case eAction_CancelTask:
		if(nID == GUI_IDOK)
		{
			DWORD dwTaskID = (DWORD)m_pData;
			if( dwTaskID != -1)
			{
				stAbandonQuestUserCmd	cmd;
				cmd.id = dwTaskID;	
				SEND_USER_CMD(cmd);
			}
		}
		Close();
		return true;
	case eAction_ExitSept:
		ExitSept(nID);
		Close();
		return true;
	case eAction_ExitTong:
		ExitTong(nID);
		Close();
		return true;
	case eAction_ExitSchool:
		ExitSchool(nID);
		Close();
		return true;
	case eAction_DelTong:
		DelTong(nID);
		Close();
		return true;
	case eAction_DelSept:
		DelSept(nID);
		Close();
		return true;
	case eAction_CompetitionNpc:
		CompetitionNpc(nID);
		Close();
		return true;
	case eAction_Competition:
		Competition(nID);
		Close();
		return true;
	case eAction_DelSchool:
		DelSchool(nID);
		Close();
		return true;
	case eAction_DelFriend:
		DelFriend(nID);
		Close();
		return true;
	case eAction_DelBad:
		DelBad(nID);
		Close();
		return true;
	case eAction_ConnectLoginFailed:
		//ExitGame();
		Close();
		return true;
	case eAction_CloseShop:
		ActionCloseShop(nID);
		Close();
		return true;
	case eAction_MarryTask:
		MarryTaskQuery(nID);
		Close();
		return true;
	case eAction_ConfirmMarriage:
		{
			stMarryStatusCheckCmd cmd;
			if ( nID == GUI_IDOK )
				cmd.byStep = MARRY_AGREE;
			else
				cmd.byStep = MARRY_REFUSE;

			//
			SEND_USER_CMD( cmd );
			Close();
		}
		return true;
	case eAction_MoneyRelive:
		{		
			if( nID == GUI_IDOK )
			{
				DWORD dwUserTempID = (DWORD)m_pData;
				stOKReliveUserCmd cmd;
				cmd.byType = ReliveMoney;//银子复活
				cmd.dwUserTempID = dwUserTempID;
				SEND_USER_CMD(cmd);
			}
			Close();			
		}
		return true;
		case eAction_MaxeyRelive: //soke 增加原地满状态复活
		{		
			if( nID == GUI_IDOK )
			{
				DWORD dwUserTempID = (DWORD)m_pData;
				stOKReliveUserCmd cmd;
				cmd.byType = ReliveMaxey;//银子复活
				cmd.dwUserTempID = dwUserTempID;
				SEND_USER_CMD(cmd);
			}
			Close();			
		}
		return true;
	case eAction_DelItemConfirm:
		{
			//GetGuiManager()->SetMouseDlg( GetGuiFloatItem() );
			CGuiItemCell* pItemCell = GetGuiFloatItem()->GetItem();
			if (pItemCell==NULL)
			{
				Close();
				return true;
			}

			CRoleItem* pFloatItem = (CRoleItem*) pItemCell->m_pItemData;
			switch( nID )
			{
			case 1:		//delete
				if ( pFloatItem )
				{
					stObjectLocation dst;
					dst.dwLocation = OBJECTCELLTYPE_NONE;
					//msg
					stSwapObjectPropertyUserCmd cmd;
					cmd.qwThisID = pFloatItem->GetThisID();
					cmd.dst = dst;
					SEND_USER_CMD(cmd);

					GetGuiManager()->SetMouseDlg(NULL);

					if ( g_bSinglePlayer )
					{
						// 丢到地上
						stAddMapObjectMapScreenUserCmd cmd1;
						cmd1.data.dwMapObjectTempID = 1;
						cmd1.data.dwObjectID = pFloatItem->GetObjectID();
						strcpy(cmd1.data.pstrName,pFloatItem->GetName());
						cmd1.data.wdLevel  = 1;
						cmd1.data.wdNumber = 1;
						cmd1.data.x = GetScene()->GetMainCharacter()->GetGridPos().x;
						cmd1.data.y = GetScene()->GetMainCharacter()->GetGridPos().y;
						GetClient()->PushCommand(&cmd1,sizeof(cmd1));

						stRemoveObjectPropertyUserCmd cmd2;
						cmd2.qwThisID = pFloatItem->GetThisID();
						GetClient()->PushCommand(&cmd2,sizeof(cmd2));
					}
				}
				break;
			case 2:		//cancel
				/*if ( pFloatItem )
				{
					pFloatItem->MoveBackOldLocation();
				}*/
				break;
			}
			GetQueryManager()->DeleteListData(eAction_DelItemConfirm,1);
		}
		Close();
		return true;
	case eAction_RemoveHorseConfirm:
		{
			if(nID == GUI_IDOK)
			{
				GetScene()->GetMainCharacter()->RemoveHorse();
			}
			if(GetGameGuiManager()->m_guiNpcDialog)
			{
				GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;
			}
			Close();
		}		
		return true;
	case eAction_TongAttack:
		{
			DWORD dwID = (DWORD) m_pData;
			if (dwID == 0)
			{
				if (nID == GUI_IDOK)
				{
					//GetGameGuiManager()->AddInputBox("请输入要战斗的帮会名称!",m_type);
					stQueryDareList cmd;
					cmd.byType = UNION_DARE;
					SEND_USER_CMD(cmd);
				}
			}
			else
				TongAttack(nID);
		}
		Close();
		return true;
	case eAction_SeptAttack:
		{
			DWORD dwID = (DWORD) m_pData;
			if (dwID == 0)
			{
				if (nID == GUI_IDOK)
				{
					//GetGameGuiManager()->AddInputBox("请输入要战斗的家族名称!",m_type);
					stQueryDareList cmd;
					cmd.byType = SEPT_DARE;
					SEND_USER_CMD(cmd);
				}
			}
			else
				SeptAttack(nID);
		}
		Close();
		return true;
	case eAction_SchoolAttack:
		{
			DWORD dwID = (DWORD) m_pData;
			if (dwID == 0)
			{
				if (nID == GUI_IDOK)
					GetGameGuiManager()->AddInputBox("请输入要战斗的门派名称!",m_type);
			}
			else
				SchoolAttack(nID);
		}
		Close();
		return true;
	case eAction_HorseSellConfirm:
		switch( nID )
		{
		case GUI_IDOK:
			//Sell self horse
			GetScene()->GetMainCharacter()->SellHorse();
			break;
		case GUI_IDCANCEL:
			break;
		}
		Close();
		break;
	case eAction_UseClearPoint: // 清除属性点 ||  清除技能点
		{
		  DWORD dwID = (DWORD)m_pData;
		  switch(nID)
		  {
		  case GUI_IDOK:
			  GetScene()->GetMainCharacter()->ClearPointOrSkill( dwID );
			  break;
		  case GUI_IDCANCEL:
			  break;
		  }
		}
		Close();
		break;
	case eAction_Chibang: //kai
			switch( nID )
			{
				case GUI_IDOK:
				{
					stGoAngelPropertyUserCmd cmd;
					cmd.level = 1;
					SEND_USER_CMD(cmd);
				}
				break;
			case GUI_IDCANCEL:
				break;
			}
			Close();
			break;
	case eAction_Chibang2: //guan
			switch( nID )
			{
			case GUI_IDOK:
				{
					stGoAngelPropertyUserCmd cmd;
					cmd.level = 2;
					SEND_USER_CMD(cmd);
				}
				break;
			case GUI_IDCANCEL:
				break;
		}
		Close();
		break;
	case eAction_Hanbing: //kai
			switch( nID )
			{
				case GUI_IDOK:
				{
					stGoHanbingPropertyUserCmd cmd;
					cmd.level = 1;
					SEND_USER_CMD(cmd);
				}
				break;
				case GUI_IDCANCEL:
					break;
			}
			Close();
			break;
	case eAction_Hanbing2: //guan
			switch( nID )
			{
				case GUI_IDOK:
				{
					stGoHanbingPropertyUserCmd cmd;
					cmd.level = 2;
					SEND_USER_CMD(cmd);
				}
				break;
				case GUI_IDCANCEL:
					break;
			}
			Close();
			break;
    //桃子增加定时传送1
	case eAction_GotoMap: //读取传送通知存储
		switch( nID )
		{
		case GUI_IDOK:
			{
				stGoChuanSongPropertyUserCmd cmd;
				SEND_USER_CMD(cmd);
			}
			break;
		case GUI_IDCANCEL:
			break;
		}
		Close();
		break;	
	////////////////////结束
    //桃子增加定时传送2
	case eAction_GotoMap2: //读取传送通知存储
		switch( nID )
		{
		case GUI_IDOK:
			{
				stGoChuanSong2PropertyUserCmd cmd;
				SEND_USER_CMD(cmd);
			}
			break;
		case GUI_IDCANCEL:
			break;
		}
		Close();
		break;	
	case eAction_biaoche1: //国家镖车传送
		switch( nID )
		{
		case GUI_IDOK:
			{
				stGoChuanSong3PropertyUserCmd cmd;
				SEND_USER_CMD(cmd);
			}
			break;
		case GUI_IDCANCEL:
			break;
		}
		Close();
		break;	
	case eAction_biaoche2: //国家镖车传送
		switch( nID )
		{
		case GUI_IDOK:
			{
				stGoChuanSong4PropertyUserCmd cmd;
				SEND_USER_CMD(cmd);
			}
			break;
		case GUI_IDCANCEL:
			break;
		}
		Close();
		break;	
	
	////////////////////结束
	case eAction_Xiulian: //能力修炼
			switch( nID )
			{
				case GUI_IDOK:
				{
					stXiulianResetCmd cmd;
					SEND_USER_CMD(cmd);
				}
				break;
			case GUI_IDCANCEL:
				break;
			}
			Close();
			break;				
	case eAction_SkillUp:
		switch( nID )
		{
		case GUI_IDOK:
			{
				SkillBase_t* pSkill = (SkillBase_t*)m_pData;
				if ( pSkill )
				{
					stAddSkillPointPropertyUserCmd cmd;
					cmd.dwSkillID = pSkill->dwID;
					SEND_USER_CMD( cmd );
				}
			}
			break;
		case GUI_IDCANCEL:
			break;
		}
		Close();
		break;
	case eAction_Bind:
		switch( nID )
		{
		case GUI_IDOK:
			{
				CRoleItem* pItem = (CRoleItem*)m_pData;
				if ( pItem )
				{
					pItem->Sell();
				}
			}
			break;
		case GUI_IDCANCEL:
			break;
		}
		Close();
		break;
	case eAction_boss: //世界boss
		switch( nID )
		{
		case GUI_IDOK:
			{
				stGoChuanSong5PropertyUserCmd cmd;
				SEND_USER_CMD(cmd);
			}
			break;
		case GUI_IDCANCEL:
			break;
		}
		Close();
		break;	
	case eAction_bike: //自行车
		switch( nID )
		{
		case GUI_IDOK:
			{
				stGoChuanSong6PropertyUserCmd cmd;
				SEND_USER_CMD(cmd);
			}
			break;
		case GUI_IDCANCEL:
			break;
		}
		Close();
		break;	
	case eAction_MaxSkillLevel:
		Close();
		break;
	case eAction_SkillUpFail:
		Close();
		break;
	case eAction_TongBallot_OK:
		TongBallot(nID,1);
		Close();
		break;
	case eAction_SeptBallot_OK:
		SeptBallot(nID,1);
		Close();
		break;
	case eAction_TongBallot_CANCEL:
		TongBallot(nID,0);
		Close();
		break;
	case eAction_SeptBallot_CANCEL:
		SeptBallot(nID,0);
		Close();
		break;
	case eAction_BuyShopItem:
		BuyObjectFromOthersShop(nID);
		Close();
		break;	
	case eAction_HonorToPk:
		HonorToPk(nID);
		Close();
		break;
	case eAction_ConfirmReceiveMailItem:
		ActionConfirmRecvMailItem(nID);
		Close();
		break;
	case eAction_LeaveCountry:
		LeaveCountry(nID);
		Close();
		break;
	case eAction_AddCountry:
		AddCountry(nID);
		Close();
		break;
	case eAction_AddCountrySalary:
		AddCountrySalary(nID);
		Close();
		break;
	case eAction_AddSalary:
		Add2Salary(nID);
		Close();
		break;
	case eAction_AddLogin:
		Add2Login(nID);
		Close();
		break;
	case eAction_AddVip:
		Add2Vip(nID);
		Close();
		break;
	case eAction_Fubenjhxt:
		AddFubenjh(nID);
		Close();
		break;
	case eAction_ExitGameByUnlaw:
		GetGameApplication()->Exit();
		Close();
		break;
	case eAction_ConfirmStartAuction:
		{
			if( nID == GUI_IDOK )
			{
				if( GetGameGuiManager()->m_guiAuctionDlg )
				{
					GetGameGuiManager()->m_guiAuctionDlg->OnConfirmStartAuction();
				}
			}
			Close();
		}
		break;
	case eAction_SeptAttackNpc:
		{
			if( nID == GUI_IDOK )
			{
				stDareNpcDare cmd;

				cmd.dwNpcID = (DWORD )(m_pData);

				SEND_USER_CMD(cmd);
			}
			
			if (GetGameGuiManager()->m_guiNpcDialog)
				GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;

			Close();
		}
		break;
	case eAction_LeaveMarry:
		{
			if( nID == GUI_IDOK )
			{
				stUnmarryCmd cmd;

				SEND_USER_CMD(cmd);
			}

			if (GetGameGuiManager()->m_guiNpcDialog)
				GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;

			Close();
		}
		break;
	case eAction_ConfirmCannelAuction:
		{
			if( nID == GUI_IDOK )
			{
				DWORD dwAuctionID = (DWORD )(m_pData);
				stCancelSaleAuction cmd;
				cmd.auctionID = dwAuctionID;
				SEND_USER_CMD(cmd);
			}
			Close();
		}
		break;
	case eAction_SendCountryChat:
		{
			if( nID == GUI_IDOK)
			{
				if(GetGameGuiManager()->m_guiChatSelect)
					GetGameGuiManager()->m_guiChatSelect->SendChat();
			}
			else if (nID == GUI_IDCANCEL)
			{
			        if(GetGameGuiManager()->m_guiChatSelect)
					GetGameGuiManager()->m_guiChatSelect->ChatEditSetFocus();
			}
			Close();
		}
		break;
	case eAction_GMBox:
		{
			if( nID == GUI_IDOK)
			{
				if(GetGameGuiManager()->m_guiGmMsgBoxDialog)
					GetGameGuiManager()->m_guiGmMsgBoxDialog->SendMsg();
			}
			else if( nID == GUI_IDCANCEL)
			{
				if(GetGameGuiManager()->m_guiGmMsgBoxDialog)
                                      GetGameGuiManager()->m_guiGmMsgBoxDialog->Close();
			}
			Close();
		}
		break;
	case eAction_TakeCountryWar:
		{
			if( nID == GUI_IDOK)
			{
				stAnswerCountryDareUserCmd cmd;
				cmd.byStatus = ANSWER_COUNTRY_DARE_YES;
				SEND_USER_CMD(cmd);
			}
			else if( nID == GUI_IDCANCEL)
			{
				stAnswerCountryDareUserCmd cmd;
				cmd.byStatus = ANSWER_COUNTRY_DARE_NO;
				SEND_USER_CMD(cmd);
			}
			Close();
		}
		break;
	case eAction_BecomeResearcher:
		{
			if( nID == GUI_IDOK)
			{  
				stConfirmSearcherUserCmd cmd;
				cmd.dwTechID = (DWORD )(m_pData);
				cmd.byStatus = YES_CONFIRM_SEARCHER;
				SEND_USER_CMD(cmd);
			}
			/*else if( nID == GUI_IDCANCEL)
			{
				stConfirmSearcherUserCmd cmd;
				cmd.dwTechID = (DWORD )(m_pData);
				cmd.byStatus = NO_CONFIRM_SEARCHER;
				SEND_USER_CMD(cmd);
			}*/
			Close();
		}
	case eAction_CreateGen:
		{
			ActionBecomeArmyGen(nID);
		}
		break;
	case eAction_FabaoShow:
		{
			ActionFabao(nID);
		}
		break;
	case eAction_DelBike:
		{
			if(nID == GUI_IDOK)
			{
				stDelBikeCmd cmd;
				SEND_USER_CMD(cmd)
			}
			Close();
		}
		break;
	case eAction_RecruitArmy:  // 队长应答将军的招收
		{
			if(NULL == CArmyManager::GetSingleton()->m_pAddArmyCaptainMsg)
			{
#if defined _DEBUG || defined RELEASE_TEST
				Engine_WriteLog("招收队员保存消息不再存在");
#endif 
				break;
			}

			stAddArmyCaptainUserCmd   cmd;
			memcpy(&cmd,CArmyManager::GetSingleton()->m_pAddArmyCaptainMsg,sizeof(stAddArmyCaptainUserCmd));

			if(nID == GUI_IDOK)
			{
				cmd.byState =ANSWER_CAPTAIN_YES; 
				SEND_USER_CMD(cmd);

			}
			else if( nID == GUI_IDCANCEL)
			{
				cmd.byState = ANSWER_CAPTAIN_NO;
				SEND_USER_CMD(cmd);
			}
		}
		Close();
		CArmyManager::GetSingleton()->ClearAddArmyCaptionUserCmd();
		break;
	case eAction_LeaveArmy:
		{
			if(nID == GUI_IDOK)
			{
				stExitArmyUserCmd cmd;
				SEND_USER_CMD(cmd);
				
			}
			Close();
			
		}
		break;
	case eAction_AskSeptExp:
		{
			if( nID == GUI_IDOK )
			{
				stRequestSeptExpCmd cmd;

				SEND_USER_CMD(cmd);
			}

			if (GetGameGuiManager()->m_guiNpcDialog)
				GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = NULL;

			Close();
		}
		break;
	case eAction_EmperorRequDailyMoney: //soke 皇帝请求每日奖励
		{
			if ( nID == GUI_IDOK )
			{
				stReqDailyEmperorMoneyCmd cmd;
				 
				SEND_USER_CMD( cmd );
			}

			Close();
		}
		break;
	case eAction_KingRequDailyMoney: //soke 国王请求每日薪资
		{
			if ( nID == GUI_IDOK )
			{
				stReqDailyKingMoneyCmd cmd;
				 
				SEND_USER_CMD( cmd );
			}

			Close();
		}
		break;
	case eAction_YuanshuaiRequDailyMoney: //soke 元帅请求每日薪资
		{
			if ( nID == GUI_IDOK )
			{
				stReqDailyYuanshuaiMoneyCmd cmd;
				 
				SEND_USER_CMD( cmd );
			}

			Close();
		}
		break;
	case eAction_ZaixiangRequDailyMoney: //soke 宰相请求每日薪资
		{
			if ( nID == GUI_IDOK )
			{
				stReqDailyZaixiangMoneyCmd cmd;
				 
				SEND_USER_CMD( cmd );
			}

			Close();
		}
		break;
	case eAction_CatcherRequDailyMoney: //soke 捕头（左）请求每日薪资
		{
			if ( nID == GUI_IDOK )
			{
				stReqDailyCatcherMoneyCmd cmd;
				 
				SEND_USER_CMD( cmd );
			}

			Close();
		}
		break;
	case eAction_CensorRequDailyMoney: //soke 御史（左）请求每日薪资
		{
			if ( nID == GUI_IDOK )
			{
				stReqDailyCensorMoneyCmd cmd;
				 
				SEND_USER_CMD( cmd );
			}

			Close();
		}
		break;
	case eAction_CatchexRequDailyMoney: //soke 捕头（右）请求每日薪资
		{
			if ( nID == GUI_IDOK )
			{
				stReqDailyCatchexMoneyCmd cmd;
				 
				SEND_USER_CMD( cmd );
			}

			Close();
		}
		break;
	case eAction_CensosRequDailyMoney: //soke 御史（右）请求每日薪资
		{
			if ( nID == GUI_IDOK )
			{
				stReqDailyCensosMoneyCmd cmd;
				 
				SEND_USER_CMD( cmd );
			}

			Close();
		}
		break;
	case eAction_ConsignCartoon:
		{
			ActionConsignCartoon(nID);
			Close();
		}
		break;
	case eAction_SeptGiveupOccupy:
		{
			if ( nID == GUI_IDOK )
			{
				stRequestAbjurationSeptNpcSeptCmd cmd;
				SEND_USER_CMD( cmd );
			}
			
			Close();
		}
		break;
	case eAction_UnionGiveupOccupy:
		{
			if ( nID == GUI_IDOK )
			{
				stRequestAbjurationCitynCmd cmd;
				SEND_USER_CMD( cmd );
			}
			Close();
		}
		break;
	case eAction_ConfirmMarryMoney:
		{
			if ( nID == GUI_IDOK )
			{
				stMarryStatusCheckCmd cmd;
				cmd.byStep = (BYTE) m_pData;

				SEND_USER_CMD( cmd );

				if ( GetGameGuiManager()->m_guiSelectMarryTypeDlg )
					GetGameGuiManager()->m_guiSelectMarryTypeDlg->Close();
			}
			Close();
		}
		break;
	case eAction_ConfirmSeptDareType:
		{
			if ( nID == GUI_IDOK )
			{
				if ( GetGameGuiManager()->m_guiSelectSeptDareTypeDlg )
				{
					stActiveDareCmd cmd = GetGameGuiManager()->m_guiSelectSeptDareTypeDlg->m_cmdSeptDare;
					cmd.byDareRepute = (BYTE) m_pData;

					SEND_USER_CMD( cmd );

					GetGameGuiManager()->m_guiSelectSeptDareTypeDlg->Close();
				}	
			}
			Close();
		}
		break;
	case eAction_ConfirmAllyCountry:
		ConfirmAllyCountry(nID);
		Close();
		return true;
	case eAction_CancelAllyCountry:
		ConfirmCancelAllyCountry(nID);
		Close();
		return true;
	case eAction_ChangeDisplaySettings:
		{
			switch( nID )
			{
			case GUI_IDOK:
				{
					GetGameGuiManager()->m_guiSystem->ApplaySetting();
					GetGameGuiManager()->m_guiSystem->Close();
				}
				break;
			case GUI_IDCANCEL:
				{
					//GetGameGuiManager()->m_guiSystem->Close();
				}
				break;
			}
			Close();
		}
		break;
	case eAction_SelectCallDuty:
		{
			BYTE ret = PIPELINE_NUM;
			switch( nID )
			{
			case GUI_IDOK:
				ret = PIPELINE_AGREE;
				break;
			case GUI_IDCANCEL:
				ret = PIPELINE_REFUSE;
				break;
			}
			
			if ( (ret == 0) || (ret >=PIPELINE_NUM) )
				break;

			stDareCallDuty cmd = *((stDareCallDuty*)m_pData);

			cmd.byPipeline = ret;
			SEND_USER_CMD( cmd );

			Close();
		}
		break;
	case eAction_MiniGame_Invite:
		{
			stInviteMiniGame* ptemp = (stInviteMiniGame*)m_pData;
			switch( nID )
			{
			case GUI_IDOK:
				{
					stInviteRetMiniGame cmd;
					cmd.ret = 1;
					cmd.money = ptemp->money;
					cmd.tempid = ptemp->tempid;
					SEND_USER_CMD( cmd );
				}
				break;
			case GUI_IDCANCEL:
				{
					stInviteRetMiniGame cmd;
					cmd.ret = 0;
					cmd.money = ptemp->money;
					cmd.tempid = ptemp->tempid;
					SEND_USER_CMD( cmd );
				}
				break;
			}
						
			Close();
		}
		break;
	case eAction_MiniGame_Continue:
		{
			switch( nID )
			{
			case GUI_IDOK:
				{
					stContinueMiniGame cmd;
					cmd.ret = 1;
					SEND_USER_CMD( cmd );
				}
				break;
			case GUI_IDCANCEL:
				{
					stContinueMiniGame cmd;
					cmd.ret = 0;
					SEND_USER_CMD( cmd );
				}
				break;
			}	
			Close();
		}
		break;
	case eAction_MiniGame_GameOver:
		{	
			if ( nID == GUI_IDOK )
			{
				if ( GetGameGuiManager()->m_MGNumberDlg )
				{
					GetGameGuiManager()->m_MGNumberDlg->Close();
					GetGameGuiManager()->m_MGNumberDlg = NULL;
				}	
			}
			Close();
		}
		break;
	case eAction_OpenSafety:
		{
			if(nID == GUI_IDOK)
			{
				CSafetyUser::Instance()->OpenSafety();
				
			}
			Close();
		}
		break;	
	default:
		Close();
		return true;
	}
	return false;

	FUNCTION_END;
}


void CGuiMessageBox::ConfirmAllyCountry(UINT nID)
{
	FUNCTION_BEGIN;
	if(nID == GUI_IDOK)
	{
		stReqCountryAlly  cmd;
	        cmd.dwCountryID = (DWORD)m_pData;
		cmd.dwAllyCountryID = GetScene()->GetMainCharacter()->GetCountry();
		cmd.byStatus = YES_COUNTRY_ALLY;
		SEND_USER_CMD(cmd);
	}
	else if( nID == GUI_IDCANCEL)
	{
		stReqCountryAlly  cmd;
		cmd.dwCountryID = (DWORD)m_pData;
		cmd.dwAllyCountryID = GetScene()->GetMainCharacter()->GetCountry();
		cmd.byStatus = NO_COUNTRY_ALLY;
		SEND_USER_CMD(cmd);
	}
	FUNCTION_END;
}

void CGuiMessageBox::ConfirmCancelAllyCountry(UINT nID)
{
	FUNCTION_BEGIN;
	if( nID == GUI_IDOK)
	{
		stCancelCountryAlly  cmd;
		cmd.dwAllyCountryID = (DWORD)m_pData;
		SEND_USER_CMD(cmd);

	}
	FUNCTION_END;
}

void CGuiMessageBox::ActionBecomeArmyGen(UINT nID)
{
	FUNCTION_BEGIN;
	
	if(nID == GUI_IDOK)
	{
		stReqArmyGenUserCmd cmd;
		cmd.byStatus = YES_ARMY_GEN;
		SEND_USER_CMD(cmd);

	}
	else if( nID == GUI_IDCANCEL)
	{
		stReqArmyGenUserCmd cmd;
		cmd.byStatus = NO_ARMY_GEN;
		SEND_USER_CMD(cmd);
	}
	Close();

	FUNCTION_END;
}

void CGuiMessageBox::ActionFabao(UINT nID)//展示法宝
{
	FUNCTION_BEGIN;
	
	if(nID == GUI_IDOK)
	{
		GetGameGuiManager()->m_guiUserInfo->ShowFabao(true); //sky
	}
	else if( nID == GUI_IDCANCEL)
	{
		GetGameGuiManager()->m_guiUserInfo->ShowFabao(false); //sky
	}
	Close();

	FUNCTION_END;
}

void CGuiMessageBox::ActionConfirmRecvMailItem(UINT nID)
{
	bool bOk = false ;
	if( nID == GUI_IDOK )
	{
		bOk = true;
	}
	if( GetGameGuiManager()->m_guiRecvMailDlg )
	{
		DWORD dwMailId = (DWORD )(m_pData);
		GetGameGuiManager()->m_guiRecvMailDlg->OnConfirmReceiveItem(dwMailId,bOk);
	}
}

//确认收摊
void CGuiMessageBox::ActionCloseShop(UINT nID)
{
	if( nID == GUI_IDOK )
	{
		if( GetGameGuiManager()->m_guiShop)
		{
			GetGameGuiManager()->m_guiShop->Close();
		}	
	}
}

//确认从别人的摊位购买物品
void CGuiMessageBox::BuyObjectFromOthersShop(UINT nID)
{
	if( nID == GUI_IDOK )
	{
		CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
		if(pMainRole)
		{
			CRoleItem* pItem = (CRoleItem*) m_pData;
			if(	NULL == pItem )
			{
				Assert(0);
				return ;
			}		
			if( GetGameGuiManager()->m_guiOthersShop)
			{
				GetGameGuiManager()->m_guiOthersShop->RequestBuyItem(pItem->GetThisID());
			}	
		}
	}
}

void CGuiMessageBox::ActionConsignCartoon(UINT nID)
{
	stConsignRetCartoonCmd cmd;
	cmd.cartoonID = (DWORD )m_pData;
	if(nID == GUI_IDOK)
	{
		cmd.ret = 1;
	}
	else
	{
		cmd.ret = 0;
	}
	SEND_USER_CMD(cmd);
}
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param nEvent : 描述
 * \param nID : 描述
 * \param pControl : 描述
 * \return 返回值的描述
 */
bool CGuiMessageBox::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		return OnButtonClick(nID);
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CQueryManager::ClearList()
{
	FUNCTION_BEGIN;

	m_queryList.clear();


	FUNCTION_END;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param eState : 描述
* \param dwID : 描述
* \return 返回值的描述
*/
bool CQueryManager::FindListData(byte eState,DWORD dwID)
{
	FUNCTION_BEGIN;

	DWORD dwIndex = CreateIndex(eState,dwID);
	if (m_queryList.find(dwIndex)!=m_queryList.end())
		return true;
	else
		m_queryList.insert(QUERYLIST::value_type(dwIndex,dwID));
	return false;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param eState : 描述
 * \param dwID : 描述
 * \return 返回值的描述
 */
DWORD  CQueryManager::CreateIndex(byte eState,DWORD dwID)
{
	FUNCTION_BEGIN;

	const DWORD cdwMul = 10000000;
	return (eState*cdwMul + dwID);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param eState : 描述
 * \param dwID : 描述
 * \return 返回值的描述
 */
void CQueryManager::DeleteListData(byte eState,DWORD dwID)
{
	FUNCTION_BEGIN;

	m_queryList.erase(CreateIndex(eState,dwID));

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CQueryManager* GetQueryManager()
{
	FUNCTION_BEGIN;

	static CQueryManager sm;
	return &sm;

	FUNCTION_END;
}

