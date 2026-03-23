
#include "public.h"
#include "miniusercommand.h"
#include "./GameGuiManager.h"
#include "./GuiQuestGameDlg.h"
#include "../gui/include/GuiManager.h"
#include "./Game.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include "SafetyUser.h"
///////////////////////////////////////////////////////////////////////////////
#define ID_BUTTON_ROLE_SELECT2  3    // 保护设置
#define ID_BUTTON_LOGIN2        4    // 临时取消
#define ID_BUTTON_AUTOUPDATE2   5    // 修改密码
#define ID_BUTTON_EXITGAME2     6    // 取消密码
 
#define ID_BUTTON_CLOSE         0
///////////////////////////////////////////////////////////////////////////////

CGuiQuestGameDlg::CGuiQuestGameDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}
 
CGuiQuestGameDlg::~CGuiQuestGameDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

void CGuiQuestGameDlg::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
 
	FUNCTION_END;
}

bool CGuiQuestGameDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
	//	case ID_BUTTON_ROLE_SELECT2:  // 保护设置
	//		{
			//	GetGameGuiManager()->AddMessageBox("确定要返回角色选择界面吗？",eAction_Logout,(void *)1,c_szMBOKCancel);
	//		}
	//		break;
	//	case ID_BUTTON_LOGIN2:  //临时取消
	//		{
			//	GetGameGuiManager()->AddMessageBox("确定要返回输入帐号界面吗？",eAction_Logout,(void *)2,c_szMBOKCancel);
	//		}
	//		break;
	//	case ID_BUTTON_AUTOUPDATE2: // 修改密码
	//		{                                   		
			//	GetGameGuiManager()->AddMessageBox("暂未开放修改密码功能");
	//		}			
	//		break;
	//	case ID_BUTTON_EXITGAME2: // 取消密码
	//		{
			//	GetGameGuiManager()->AddMessageBox("暂未开放取消密码功能");
	//		}
	//		break;
		case ID_BUTTON_CLOSE:
			{
				Close();
			}
			break;
		}
		
		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}


void CGuiQuestGameDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiQuestGame = NULL;

	FUNCTION_END;
}
