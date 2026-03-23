 
#pragma once 
 
class CGuiQuestVipGameDlg :
	public CGuiDialog
{	
public:
	CGuiQuestVipGameDlg();
	~CGuiQuestVipGameDlg();

	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender(float fElapsedTime);
};
