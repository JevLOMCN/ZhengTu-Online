 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiXinshouDlg :
	public CGuiDialog
{	
public:
	CGuiXinshouDlg();
	~CGuiXinshouDlg();
	
	
	
	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	
private:

};
