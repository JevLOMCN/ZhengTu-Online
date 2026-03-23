 
#pragma once
#include "..\gui\include\guidialog.h"

 
class CGuiKeyBoardDlg :
	public CGuiDialog
{
public:	
	CGuiKeyBoardDlg( void );

	void OnCreate();
	void OnClose();
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender(float fElapsedTime);

	void ClearKey();

private:
	enum  enumCAPSLOCK
	{
		CAPS_LOCK_SMALL,
		CAPS_LOCK_BIG,
	};
	enumCAPSLOCK m_enumCapsLock;

	bool IsCapsLockBig(){return m_enumCapsLock == CAPS_LOCK_BIG;};

	void OnNumKeyClicked(UINT uNumber);
	void OnSubtractKeyClicked();
	void OnBackClicked();
	void OnCapsLockClicked();
	void OnLetterKeyClicked(UINT uLetter);
	void OnCommaKeyClicked();
	void OnEnterKeyClicked();
	void OnShiftClicked();	

	// some special operator 
	void UpdateDataToLogin();
	void _LoginGame();

	char m_szKey;
 
};

 