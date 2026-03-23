#include "public.h"
#include "./GameGuiManager.h"
#include ".\GuiInputBoxFive.h"

CGuiInputBoxFiveDialog::CGuiInputBoxFiveDialog( const char* pszTitle )
: m_strTitle(pszTitle)
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	for( int i=0;i<ID_BOX_PART_NUM;i++ )
		m_pEditBox[i] = NULL;

	FUNCTION_END;
}

CGuiInputBoxFiveDialog::~CGuiInputBoxFiveDialog(void)
{
}

void CGuiInputBoxFiveDialog::OnClose(void)
{
	GetGameGuiManager()->m_guiInputBoxFiveDlg = NULL;
}

void CGuiInputBoxFiveDialog::OnCreate()
{
	CGuiDialog::OnCreate();

	CGuiStatic* pText = GetStatic(10);
	AssertCreateCtrl(pText,"输入脚本错误");
	pText->SetText( m_strTitle.c_str() );

	//Get five edit box
	for( int i=0;i<ID_BOX_PART_NUM;i++ )
	{
		m_pEditBox[i] = GetEditBox( i+1 );
		AssertCreateCtrl(m_pEditBox[i],"输入脚本错误");
		m_pEditBox[i]->SetMaxCharCount( ID_BOX_CHAR_NUM );
	}

	GetGuiManager()->RequestFocus(this);
	if ( m_pEditBox[0] )
		this->SetFocus( (CGuiControl*)m_pEditBox[0] );
}

const char* CGuiInputBoxFiveDialog::GetText( const int index )
{
	if ( index >= ID_BOX_PART_NUM )
		return NULL;
	if ( index < 0 )
		return NULL;

	if ( m_pEditBox[index] )
		return m_pEditBox[index]->GetText();

	return NULL;
}

bool CGuiInputBoxFiveDialog::VerifyEditBox( const int index )
{
	const char* pStr = GetText( index );
	if ( NULL == pStr )
		return false;

	if ( strlen(pStr) != ID_BOX_CHAR_NUM )
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

void CGuiInputBoxFiveDialog::OnInputBoxEnter(bool bOk)
{
	if ( !bOk )
	{
		Close();
		return;
	}

    //Verify each edit box
	bool bValidChar = false;
	for( int i=0;i<ID_BOX_PART_NUM;i++ )
	{
		if ( m_pEditBox[i] )
		{
			bValidChar = VerifyEditBox( i );
			if ( !bValidChar )
				break;
		}
	}

	if ( !bValidChar )
	{
		GetGameGuiManager()->AddMessageBox( "你输入了非法字符！" );
		return;
	}

	//Send msg to server
	stConSumeCardCard cmd;
	for( int i=0;i<ID_BOX_PART_NUM;i++)
		strncat( cmd.cardid, GetText(i), ID_BOX_CHAR_NUM );

	cmd.cardid[strlen(cmd.cardid)] = 0L;
	cmd.type = ZONE_CARD_OBJECT;

	SEND_USER_CMD( cmd );
	Close();    
}

bool CGuiInputBoxFiveDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch( nEvent )
	{
	case EVENT_BUTTON_CLICKED:
		{
			OnInputBoxEnter( nID == 20 );
			return true;
		}
		break;
	case EVENT_EDITBOX_CHANGE:
		{
			int nFocusIndex = -1;
			for( int i=0;i<ID_BOX_PART_NUM;i++ )
			{
				if ( m_pEditBox[i] && m_pEditBox[i]->IsFocus() )
				{
					nFocusIndex = i;
					break;
				}
			}

			if ( nFocusIndex < 0 )
				return false;

			//m_pEditBox[nFocusIndex]->OnTextChange();
			if ( strlen( GetText(nFocusIndex) ) >= ID_BOX_CHAR_NUM )
			{
				if ( (nFocusIndex < ID_BOX_PART_NUM-1) && (m_pEditBox[nFocusIndex+1]) )
				{
					if ( strlen( GetText(nFocusIndex+1) ) == 0 )
					{
						this->SetFocus( (CGuiControl*)m_pEditBox[nFocusIndex+1] );
					}
				}					
			}
			return false;
		}
		break;
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}