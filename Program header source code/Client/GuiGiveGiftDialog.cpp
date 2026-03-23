#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GuiGiveGiftDialog.h"

#define  GIVEGIFT_EDIT_LIMTLEN_U 16
#define  GIVEGIFT_EDIT_LIMTLEN_P 8
#define  ID_EDIT_U 5
#define  ID_EDIT_P 6

CGuiGiveGiftDialog::CGuiGiveGiftDialog(void)
{
	m_bSmDialog = true;
}

CGuiGiveGiftDialog::~CGuiGiveGiftDialog(void)
{
}

void CGuiGiveGiftDialog::OnCreate(void)
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();
	this->GetEditBox(ID_EDIT_U)->SetText("请在这里输入卡号!");
	//this->GetEditBox(ID_EDIT_P)->SetText("请在这里输入密码!");
	this->GetEditBox(ID_EDIT_U)->SetMaxCharCount(GIVEGIFT_EDIT_LIMTLEN_U);
	this->GetEditBox(ID_EDIT_P)->SetMaxCharCount(GIVEGIFT_EDIT_LIMTLEN_P);
	FUNCTION_END;
}

bool CGuiGiveGiftDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
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

HRESULT CGuiGiveGiftDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	return hr;
	FUNCTION_END;
}

void CGuiGiveGiftDialog::OnClose(void)
{
	FUNCTION_BEGIN;
	GetGameGuiManager()->m_guiGiveGiftDlg = NULL;
	CGuiDialog::OnClose();
	FUNCTION_END;
}

bool CGuiGiveGiftDialog::VerifyEditBox( const int index ,int lenLimt)
{
	const char* pStr = this->GetEditBox(index)->GetText();
	if ( NULL == pStr )
		return false;

	if ( strlen(pStr) != lenLimt )
		return false;

	for( int i=0;i<strlen(pStr);i++ )
	{
		if ( (pStr[i] & 0x80) )
			return false;

		if ( !( ((pStr[i] >= '0') && (pStr[i] <= '9'))
			|| ( (pStr[i] >= 'a') && (pStr[i] <= 'z') )
			|| ( (pStr[i] >= 'A') && (pStr[i] <= 'Z') ) ) )
			return false;
	}

	return true;
}

bool CGuiGiveGiftDialog::OnOK()
{
	FUNCTION_BEGIN;
	if( (strcmp(this->GetEditBox(ID_EDIT_U)->GetText(),"") == 0) || 
		(strcmp(this->GetEditBox(ID_EDIT_P)->GetText(),"") == 0) )
	{
		GetGameGuiManager()->AddMessageBox("礼品卡序列号或者密码输入为空!");
		return true;
	}
	if ( !VerifyEditBox(ID_EDIT_U,GIVEGIFT_EDIT_LIMTLEN_U) || !VerifyEditBox(ID_EDIT_P,GIVEGIFT_EDIT_LIMTLEN_P) )
	{
		GetGameGuiManager()->AddMessageBox("你输入不合法,请注意卡号必须是16位,密码必须是8位,只能包括0-9,英文字符！");
		return true;
	}
	
	//stConSumeGIVEGIFT cmd;
	stConSumeCardCard cmd;
	int len[2];
	len[0] = GIVEGIFT_EDIT_LIMTLEN_U;
	len[1] = GIVEGIFT_EDIT_LIMTLEN_P;
	for( int i=0;i<2;i++)
	{
		strncat( cmd.cardid, this->GetEditBox(i+5)->GetText(),len[i]);
	}
	cmd.cardid[strlen(cmd.cardid)] = 0L;
	cmd.type = ZONE_CARD_PROFRESSION;
	SEND_USER_CMD( cmd );
	
	//测试-->wyc
	//stReturnConSumeCardCard rcmd;
	//rcmd.byReturn = REDEEM_SUCCESS;
	//GetClient()->PushCommand(&rcmd,sizeof(rcmd));

	Close();
	return true;
	FUNCTION_END;
}

bool CGuiGiveGiftDialog::OnCancel()
{
	FUNCTION_BEGIN;
	Close();
	return true;
	FUNCTION_END;
}