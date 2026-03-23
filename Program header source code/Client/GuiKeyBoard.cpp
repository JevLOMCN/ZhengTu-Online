
#include "public.h"
#include "../gui/include/guiTypes.h"
#include ".\GuiKeyBoard.h"
#include ".\GuiLogin.h"
#include ".\GameGuiManager.h"
 
#define MAX_PASS_SIZE 32 

const UINT c_skey[26] = // 按按钮的id从0~26
{                   
     'q','w','e','r','t','y','u','i','o','p',
      'a','s','d','f','g','h','j','k','l',
       'z','x','c','v','b','n','m',
};
 
CGuiKeyBoardDlg::CGuiKeyBoardDlg( void )
: m_enumCapsLock(CAPS_LOCK_SMALL)
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;
 
	FUNCTION_END;
}


void CGuiKeyBoardDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	 
	GetGameGuiManager()->m_guiKeyBoard = NULL;

	FUNCTION_END;
}

 
void CGuiKeyBoardDlg::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();


	FUNCTION_END;
}


HRESULT CGuiKeyBoardDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	
	if( !IsVisible()) return S_OK;

	CGuiDialog::OnRender(fElapsedTime);

	return S_OK;
	
	FUNCTION_END;

}

bool CGuiKeyBoardDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam  )
{
	FUNCTION_BEGIN;

	return CGuiDialog::MsgProc( hWnd, uMsg, wParam, lParam );

	FUNCTION_END;
}

bool CGuiKeyBoardDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			if(nID >= 1 && nID <=10)	// 数字1~10，没有大小写区分
			{
				OnNumKeyClicked(nID);
				UpdateDataToLogin();
			}
			else if( nID >= 20 && nID <= 45)  // 字母，有大小写区分
			{
				int uLetter = nID - 20;
				Assert(uLetter >=0 && uLetter <26);
				if( uLetter < 0 || uLetter >= 26)
					break;
				uLetter = c_skey[uLetter];
				if(IsCapsLockBig())
				   uLetter -= 0x20;
				OnLetterKeyClicked(uLetter);
				UpdateDataToLogin();
			}
			else	// 控制键
			{
				switch(nID)
				{
				case 11:    { OnSubtractKeyClicked();  						} break;  // - 
				case 12:    { OnBackClicked();							} break;  // <--  		 
				case 13:    { OnCapsLockClicked();			     			} break;  // Caps Lock	
				case 50:    { OnCommaKeyClicked();     UpdateDataToLogin();			} break;  // ,	
				case 51:    { OnEnterKeyClicked();     _LoginGame();				} break;  // enter	 
				case 52:    { OnShiftClicked();                            			} break;  // shift 
				}
			}
		}
		break;
	}

	

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}


void    CGuiKeyBoardDlg::UpdateDataToLogin()
{
	FUNCTION_BEGIN;

        Assert(GetGameGuiManager()->m_guiLogin && GetGameGuiManager()->m_guiLogin->IsVisible());
	if(GetGameGuiManager()->m_guiLogin && GetGameGuiManager()->m_guiLogin->IsVisible())
	{
		GetGameGuiManager()->m_guiLogin->AppendPassWord(m_szKey);
	}

	FUNCTION_END;
}
// uNumber: 0~9
void    CGuiKeyBoardDlg::OnNumKeyClicked(UINT uNumber)
{
	FUNCTION_BEGIN;
	if( uNumber >= 0 && uNumber <= 10)
	{
		char szChar[3];
		if( uNumber == 10)
			uNumber = 0;
		char * pchar = itoa(uNumber,szChar,10);
		m_szKey = pchar[0];
	}
	FUNCTION_END;
}

void    CGuiKeyBoardDlg::OnSubtractKeyClicked()
{
	FUNCTION_BEGIN;
	// TO DO:
      
	FUNCTION_END;
}

void    CGuiKeyBoardDlg::OnLetterKeyClicked(UINT uLetter)
{
	FUNCTION_BEGIN;

	bool bBig = IsCapsLockBig();
	if( (bBig && uLetter >= 'A' && uLetter <= 'Z') || (!bBig && uLetter >= 'a' && uLetter <= 'z') )
	{
             m_szKey = char(uLetter);  		
	}

	FUNCTION_END;
}

void    CGuiKeyBoardDlg::OnBackClicked()
{
	FUNCTION_BEGIN;
	Assert(GetGameGuiManager()->m_guiLogin && GetGameGuiManager()->m_guiLogin->IsVisible());
	if(GetGameGuiManager()->m_guiLogin && GetGameGuiManager()->m_guiLogin->IsVisible())
	{
		GetGameGuiManager()->m_guiLogin->OnBackClicked();
	}
	FUNCTION_END;
}

void    CGuiKeyBoardDlg::OnCapsLockClicked()
{
	FUNCTION_BEGIN;

	if( m_enumCapsLock == CAPS_LOCK_SMALL)   
	{
		m_enumCapsLock = CAPS_LOCK_BIG;
		GetButton(13)->SetChecked(true);
	}
	else if( m_enumCapsLock == CAPS_LOCK_BIG)     
	{
		m_enumCapsLock = CAPS_LOCK_SMALL;
		GetButton(13)->SetChecked(false);
	}
		 
	FUNCTION_END;
}

void    CGuiKeyBoardDlg::OnCommaKeyClicked()
{
	FUNCTION_BEGIN;
	m_szKey = ',';
	FUNCTION_END;
}

void    CGuiKeyBoardDlg::OnEnterKeyClicked()
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

void    CGuiKeyBoardDlg::OnShiftClicked()
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}


void CGuiKeyBoardDlg::_LoginGame()
{
	FUNCTION_BEGIN;

	ClearKey();
	Assert(GetGameGuiManager()->m_guiLogin && GetGameGuiManager()->m_guiLogin->IsVisible());
	if(GetGameGuiManager()->m_guiLogin && GetGameGuiManager()->m_guiLogin->IsVisible())
	{
		GetGameGuiManager()->m_guiLogin->ClickOnLoginButton();
	}
	FUNCTION_END;
}


void CGuiKeyBoardDlg::ClearKey()
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}
 