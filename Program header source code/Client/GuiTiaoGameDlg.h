 
#pragma once 
 
class CGuiTiaoGameDlg :
	public CGuiDialog
{	
public:
	CGuiTiaoGameDlg();
	~CGuiTiaoGameDlg();

	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
};
