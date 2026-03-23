
#include "public.h"
#include "miniusercommand.h"
#include "./GameGuiManager.h"
#include "./GuiExitGameDlg.h"
#include "../gui/include/GuiManager.h"
#include "./Game.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
///////////////////////////////////////////////////////////////////////////////
#define ID_BUTTON_ROLE_SELECT  3    // 返回人物界面
#define ID_BUTTON_LOGIN        4    // 登出帐号
#define ID_BUTTON_AUTOUPDATE   5    // 切换游戏大区
#define ID_BUTTON_EXITGAME     6    // 关闭游戏客户端
#define ID_BUTTON_RETURN_MINIGMAELOBBY  7 // 返回小游戏大厅
#define ID_BUTTON_RETURN_SHOWHELP       8 // 打开系统帮助
 
#define ID_BUTTON_CLOSE        2   
///////////////////////////////////////////////////////////////////////////////

CGuiExitGameDlg::CGuiExitGameDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}
 
CGuiExitGameDlg::~CGuiExitGameDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

void CGuiExitGameDlg::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
 
	FUNCTION_END;
}

bool CGuiExitGameDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
		case ID_BUTTON_ROLE_SELECT:  // 返回人物界面
			{

				GetGameGuiManager()->AddMessageBox("确定要返回角色选择界面吗？",eAction_Logout,(void *)1,c_szMBOKCancel);
				//LogoutGame(false);
			}
			break;
		case ID_BUTTON_LOGIN:  //登出帐号
			{
				GetGameGuiManager()->AddMessageBox("确定要返回输入帐号界面吗？",eAction_Logout,(void *)2,c_szMBOKCancel);
				//GetClient()->Destroy();
			}
			break;
		case ID_BUTTON_AUTOUPDATE: // 切换游戏大区
			{
				GetGameGuiManager()->AddMessageBox("确定要返回切换游戏大区界面吗？",eAction_Logout,(void *)3,c_szMBOKCancel);
			}			
			break;
		case ID_BUTTON_EXITGAME: // close
			{
				GetGameGuiManager()->AddMessageBox("确定要关闭游戏吗？",eAction_Logout,(void *)4,c_szMBOKCancel);
				//PostQuitMessage(0);
			}
			break;
		case ID_BUTTON_CLOSE:
			{
			}
			break;
		case ID_BUTTON_RETURN_MINIGMAELOBBY:
			{
				CLordStrikeManager::GetSingleton()->LogintoLobby();
				//GetGameGuiManager()->AddMessageBox("确定要到小游戏大厅吗?",eAction_Logout,(void *)5,c_szMBOKCancel);
			}
			break;
		case ID_BUTTON_RETURN_SHOWHELP:
			{
                GetGameGuiManager()->AddHelp();	
				//GetGameGuiManager()->AddMessageBox("确定要打开系统帮助吗?",eAction_Logout,(void *)6,c_szMBOKCancel);
			}
			break;
		}
		SetVisible(false);
		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}


void CGuiExitGameDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiExitGame = NULL;

	FUNCTION_END;
}
