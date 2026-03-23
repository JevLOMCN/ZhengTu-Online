 
#pragma once

#include "..\gui\include\guidialog.h"
 
class CGuiGoodHelperDlg :
	public CGuiDialog
{	
public:
	CGuiGoodHelperDlg();
	~CGuiGoodHelperDlg();

	void OnClose(void);
	void OnCreate(void);
	void OnResetScreenSize();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	virtual HRESULT OnRender( float fElapsedTime ); 
private:
    time_t	m_lastCreateTime;	
};
