#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./MiniGameListDlg.h"
#include "./GameScene.h"

CMiniGameListDlg::CMiniGameListDlg(void)
{
	m_bSmDialog = true;
}

CMiniGameListDlg::~CMiniGameListDlg(void)
{
}

void CMiniGameListDlg::OnCreate(void)
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();
	FUNCTION_END;
}

bool CMiniGameListDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case 7 : 
			return OnNumberGame();
		case 8 :
			Close();
			break;
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

HRESULT CMiniGameListDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	return hr;
	FUNCTION_END;
}

void CMiniGameListDlg::OnClose(void)
{
	FUNCTION_BEGIN;
	GetGameGuiManager()->m_MiniGameListDlg = NULL;
	CGuiDialog::OnClose();
	FUNCTION_END;
}

bool CMiniGameListDlg::OnNumberGame()
{
	FUNCTION_BEGIN;
	//
	stSelectMainUserPropertyUserCmd  cmd;
	cmd.dwTempID = GetScene()->m_pInterestNpc->GetID();
	cmd.byType = MAPDATATYPE_USER;
	SEND_USER_CMD(cmd);
	//
	Close();
	return true;
	FUNCTION_END;
}