
#include "public.h"
#include "miniusercommand.h"
#include "./GameGuiManager.h"
#include "./GuiLoginGameDlg.h"
#include "../gui/include/GuiManager.h"
#include "./Game.h"
#include "LordStrikeManager.h"
#include "LordStrikeLobby.h"
#include "SafetyUser.h"
///////////////////////////////////////////////////////////////////////////////
#define ID_BUTTON_ROLE_SELECT2  3    // ÁìÈ¡µÇÂ½½±Àø
 
#define ID_BUTTON_CLOSE         0
///////////////////////////////////////////////////////////////////////////////

CGuiLoginGameDlg::CGuiLoginGameDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}
 
CGuiLoginGameDlg::~CGuiLoginGameDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

void CGuiLoginGameDlg::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
 
	FUNCTION_END;
}

bool CGuiLoginGameDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
		case ID_BUTTON_ROLE_SELECT2:  // ÁìÈ¡µÇÂ½½±Àø
			{
				stAddCountryLoginCmd Cmd;
				SEND_USER_CMD(Cmd);
				Close();
			}
			break;
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


void CGuiLoginGameDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiLoginGame = NULL;

	FUNCTION_END;
}
