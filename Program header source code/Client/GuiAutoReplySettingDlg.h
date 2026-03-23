#pragma once

//#include "..\gui\include\guidialog.h"

class CGuiAutoReplySettingDlg :	public CGuiDialog
{
public:
	CGuiAutoReplySettingDlg(void);
	~CGuiAutoReplySettingDlg(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	void OnSetCloseLocation();

	bool OnIDOK();
	bool OnIDCancel();

	void SetAutoReplyMenu( CGuiMenu* pMenu ) 
	{
		m_pMenu = pMenu;
	}

private:
	CGuiMLEditBox*	m_pMLEditBox;	//¡Ù—‘’˝Œƒ

	CGuiMenu*		m_pMenu;
};
