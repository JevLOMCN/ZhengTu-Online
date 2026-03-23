#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./MGMoneyInputDlg.h"


CMGMoneyInputDlg::CMGMoneyInputDlg(void)
{
	m_bSmDialog = true;
}

CMGMoneyInputDlg::~CMGMoneyInputDlg(void)
{
}

void CMGMoneyInputDlg::OnCreate(void)
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();
	FUNCTION_END;
}

bool CMGMoneyInputDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case 7 : 
			return OnOK();
		case 8 :
			Close();
			break;
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

HRESULT CMGMoneyInputDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	return hr;
	FUNCTION_END;
}

void CMGMoneyInputDlg::OnClose(void)
{
	FUNCTION_BEGIN;
	//GetGameGuiManager()->m_guiGiveGiftDlg = NULL;
	CGuiDialog::OnClose();
	FUNCTION_END;
}

bool CMGMoneyInputDlg::OnOK()
{
	FUNCTION_BEGIN;
	Close();
	return true;
	FUNCTION_END;
}