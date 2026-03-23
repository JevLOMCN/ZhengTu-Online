 
#pragma once 
 
class CGuiSafeGameDlg :
	public CGuiDialog
{	
public:
	CGuiSafeGameDlg();
	~CGuiSafeGameDlg();

	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
};
