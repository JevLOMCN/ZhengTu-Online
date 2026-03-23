#include "public.h"
#include "GameGuiManager.h"
#include "game_script.h"
#include "./RoleItem.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include ".\GuiGoldExchangeDialog.h"

#define IDC_EDIT_BOX		11

#define _SetLabelText(id,txt)	{CGuiStatic* pLabel = GetStatic(id); if( pLabel ) {pLabel->SetText(txt);}}

CGuiGoldExchangeDialog::CGuiGoldExchangeDialog(void)
{
	m_iInputPointNum = 0;
	m_bSmDialog = true;
}

CGuiGoldExchangeDialog::~CGuiGoldExchangeDialog(void)
{
	GetGameGuiManager()->m_guiGoldExchangeDlg = NULL;
}

void CGuiGoldExchangeDialog::OnClose(void)
{
	GetGameGuiManager()->m_guiGoldExchangeDlg = NULL;
	GetGameGuiManager()->OnNpcActionEnd();
}

void CGuiGoldExchangeDialog::OnCreate()
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();


	m_pEditBox = GetEditBox(IDC_EDIT_BOX);
	if( !m_pEditBox )
	{
		GetGameGuiManager()->AddMessageBox("½ð×Ó¶Ò»»½Å±¾´íÎó");
		Engine_WriteLog("½ð×Ó¶Ò»»½Å±¾´íÎó");
		Close();
		return ;
	}

	UpdatePointNum();

	FUNCTION_END;
}

void CGuiGoldExchangeDialog::UpdatePointNum()
{
	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
	if( pMainRole )
	{
		m_pEditBox->SetStyle(CGuiEditBox::GUI_ES_NUMBER);

		m_pEditBox->SetMinNumber(0);
		m_pEditBox->SetMaxNumber(pMainRole->GetPointNum());
		char szTemp[64];
		_snprintf(szTemp,64,"%d",pMainRole->GetPointNum());
		szTemp[63] = 0;
		_SetLabelText(10,szTemp);
	}	
}

bool CGuiGoldExchangeDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		if( nID == 5 )
		{
			OnIDOK();
			return true;
		}
		if( nID == 6 )
		{
			Close();
			return true;
		}
	}
	if( nEvent == EVENT_EDITBOX_CHANGE && nID == IDC_EDIT_BOX )
	{
		const char* pszText = m_pEditBox->GetText();
		m_iInputPointNum = strtoul(pszText,NULL,10);
		int point = m_iInputPointNum*REDEEM_RATE_GOLD;
		char szTemp[64];
		_snprintf(szTemp,63,"%d",point);
		szTemp[63] = 0;
		_SetLabelText(9,szTemp);

		CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
		if( pMainRole )
		{
			_snprintf(szTemp,64,"%d",pMainRole->GetPointNum() - point );
			szTemp[63] = 0;
			_SetLabelText(10,szTemp);
		}		

		return true;
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

HRESULT CGuiGoldExchangeDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);



	return hr;
	FUNCTION_END;
}

void CGuiGoldExchangeDialog::OnIDOK()
{
	const char* pszText = m_pEditBox->GetText();
	m_iInputPointNum = strtoul(pszText,NULL,10);
	if( m_iInputPointNum > 0 )
	{
		stRequestRedeemGold cmd;
		cmd.dwNum = m_iInputPointNum;
		SEND_USER_CMD(cmd);
		Close();
	}	
}