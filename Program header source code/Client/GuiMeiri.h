 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiMeiriDlg :
	public CGuiDialog
{	
public:
	CGuiMeiriDlg();
	~CGuiMeiriDlg();
	
	
	
	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	
private:

};
