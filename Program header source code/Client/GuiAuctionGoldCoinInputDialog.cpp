#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GuiAuctionGoldCoinInputDialog.h"
#include "./RoleItem.h"
#include "./MainCharacter.h"


CGuiAuctionGoldCoinInputDialog::CGuiAuctionGoldCoinInputDialog(void)
{
}

CGuiAuctionGoldCoinInputDialog::~CGuiAuctionGoldCoinInputDialog(void)
{
}

void CGuiAuctionGoldCoinInputDialog::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	FUNCTION_END;
}

bool CGuiAuctionGoldCoinInputDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case 5 : return OnIDOK();
		default:
			break;
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}

bool CGuiAuctionGoldCoinInputDialog::OnIDOK()
{
	FUNCTION_BEGIN;
	return true;
}

void CGuiAuctionGoldCoinInputDialog::OnClose(void)
{
	FUNCTION_BEGIN;
	GetGameGuiManager()->m_guiSendMailDlg = NULL;
}
