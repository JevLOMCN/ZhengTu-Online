 
#pragma once 
 
class CGuiLoginGameDlg :
	public CGuiDialog
{	
public:
	CGuiLoginGameDlg();
	~CGuiLoginGameDlg();

	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
};
