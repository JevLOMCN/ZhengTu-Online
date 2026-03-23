 
#pragma once 
 
class CGuiBoardGameDlg :
	public CGuiDialog
{	
public:
	CGuiBoardGameDlg();
	~CGuiBoardGameDlg();

	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
};
