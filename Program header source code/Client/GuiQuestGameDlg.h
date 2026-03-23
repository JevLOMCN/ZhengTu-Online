 
#pragma once 
 
class CGuiQuestGameDlg :
	public CGuiDialog
{	
public:
	CGuiQuestGameDlg();
	~CGuiQuestGameDlg();

	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
};
