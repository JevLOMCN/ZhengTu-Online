#include "public.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "GuiSelectSeptDareType.h"
#include ".\gamestate.h"
#include "RoleItem.h"
#include "./GuiMessageBox.h"
#include "../gui/include/GuiManager.h"

CGuiSelectSeptDareTypeDlg::CGuiSelectSeptDareTypeDlg(void)
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuiSelectSeptDareTypeDlg::~CGuiSelectSeptDareTypeDlg()
{
	FUNCTION_BEGIN;

	FUNCTION_END;
}

void CGuiSelectSeptDareTypeDlg::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_x = GetDevice()->GetWidth() / 2 + 30;

	FUNCTION_END;
}

void CGuiSelectSeptDareTypeDlg::OnClose()
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiSelectSeptDareTypeDlg = NULL;

	FUNCTION_END;
}

bool CGuiSelectSeptDareTypeDlg::OnGuiEvent( UINT nEvent,UINT nID,CGuiControl* pControl )
{
	FUNCTION_BEGIN;

	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		BYTE byDareType = 0;

		switch( nID )
		{
		case 1:
		case 2:
		case 3:
		case 4:
			{
				byDareType = nID;
			}			
			break;		
		case 6:
			Close();
			return true;
			break;
		}

		if ( byDareType )
		{
			//SEND_USER_CMD( cmd );
			GetGameGuiManager()->AddMessageBox( "确定要挑战吗?",eAction_ConfirmSeptDareType,(void*)byDareType,c_szMBOKCancel);
			//Close();
			return true;
		}
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}



