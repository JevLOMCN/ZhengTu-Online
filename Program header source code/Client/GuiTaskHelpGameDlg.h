 
#pragma once 
 
class CGuiTaskHelpGameDlg :
	public CGuiDialog
{	
public:
	CGuiTaskHelpGameDlg();
	~CGuiTaskHelpGameDlg();

	void OnClose(void);
	void OnCreate(void);
//	void OnResetScreenSize();
	//void OnSetCloseLocation();
	void UpdateLocation(bool bReAlign);
	void InitCustomElements();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender( float fElapsedTime );
};
