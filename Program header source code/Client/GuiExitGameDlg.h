 
#pragma once 
 
class CGuiExitGameDlg :
	public CGuiDialog
{	
public:
	CGuiExitGameDlg();
	~CGuiExitGameDlg();

	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
};
