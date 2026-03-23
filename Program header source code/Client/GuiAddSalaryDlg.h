 
#pragma once 
 
class CGuiAddSalaryDlg :
	public CGuiDialog
{	
public:
	CGuiAddSalaryDlg();
	~CGuiAddSalaryDlg();

	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
};
