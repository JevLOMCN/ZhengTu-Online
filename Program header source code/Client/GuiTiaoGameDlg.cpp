
#include "public.h"
#include "miniusercommand.h"
#include "./GameGuiManager.h"
#include "./GuiTiaoGameDlg.h"
#include "../gui/include/GuiManager.h"
#include "./Game.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include "./MainCharacter.h"
///////////////////////////////////////////////////////////////////////////////
#define ID_BUTTON_ROLE_DANCE   3    // ¿ªÊ¼ÌøÎè
#define ID_BUTTON_DANCE        4    // Í£Ö¹ÌøÎè
#define ID_BUTTON_ROLE_WABAO   5    // ¿ªÊ¼ÍÚ±¦
#define ID_BUTTON_WABAO        6    // Í£Ö¹ÍÚ±¦
 
#define ID_BUTTON_CLOSE        2   
///////////////////////////////////////////////////////////////////////////////

CGuiTiaoGameDlg::CGuiTiaoGameDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}
 
CGuiTiaoGameDlg::~CGuiTiaoGameDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

//´´½¨¶Ô»°¿ò
void CGuiTiaoGameDlg::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
 
	FUNCTION_END;
}

bool CGuiTiaoGameDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
		case ID_BUTTON_ROLE_DANCE:  // ¿ªÊ¼ÌøÎè
			{
				ReqStartDanceOperateCmd cmd;
				SEND_USER_CMD(cmd);
			}
			break;
		case ID_BUTTON_DANCE:  // Í£Ö¹ÌøÎè
			{
				ReqStopDanceOperateCmd cmd;
				SEND_USER_CMD(cmd);
			}
			break;
		case ID_BUTTON_ROLE_WABAO:  // ¿ªÊ¼ÍÚ±¦
			{
				ReqBeginWabaoMiniFuncCmd Cmd;
				SEND_USER_CMD(Cmd);
			}
			break;
		case ID_BUTTON_WABAO:  //Í£Ö¹ÍÚ±¦
			{
				ReqEndWabaoMiniFuncCmd Cmd;
				SEND_USER_CMD(Cmd);
			}
			break;
		}
		SetVisible(false);
		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}


void CGuiTiaoGameDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiTiaoGame = NULL;

	FUNCTION_END;
}
