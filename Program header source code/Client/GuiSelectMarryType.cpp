#include "public.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "GuiSelectMarryType.h"
#include ".\gamestate.h"
#include "RoleItem.h"
#include "./GuiMessageBox.h"
#include "../gui/include/GuiManager.h"

CGuiSelectMarryTypeDlg::CGuiSelectMarryTypeDlg(void)
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuiSelectMarryTypeDlg::~CGuiSelectMarryTypeDlg()
{
	FUNCTION_BEGIN;

	FUNCTION_END;
}

void CGuiSelectMarryTypeDlg::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_x = GetDevice()->GetWidth() / 2 + 30;

	FUNCTION_END;
}

void CGuiSelectMarryTypeDlg::OnClose()
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiSelectMarryTypeDlg = NULL;

	FUNCTION_END;
}

bool CGuiSelectMarryTypeDlg::OnGuiEvent( UINT nEvent,UINT nID,CGuiControl* pControl )
{
	FUNCTION_BEGIN;

	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		stMarryStatusCheckCmd cmd;
		cmd.byStep = 0;
		char strInfo[MAX_PATH];

		switch( nID )
		{
		case 1:
			cmd.byStep = MARRY_PAY_MONEY;
			sprintf( strInfo, "选择普通婚礼需要30两,确认结婚吗?" );
			break;
		case 2:
			cmd.byStep = MARRY_PAY_MONEY1;
			sprintf( strInfo, "选择特级婚礼需要50两,确认结婚吗?" );
			break;
		case 3:
			cmd.byStep = MARRY_PAY_MONEY2;
			sprintf( strInfo, "选择豪华婚礼需要1锭,确认结婚吗?" );
			break;
		case 4:
			Close();
			return true;
			break;
		}

		if ( cmd.byStep )
		{
			//SEND_USER_CMD( cmd );
			GetGameGuiManager()->AddMessageBox( strInfo,eAction_ConfirmMarryMoney,(void*)(cmd.byStep),c_szMBOKCancel);
			//Close();
			return true;
		}
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}



