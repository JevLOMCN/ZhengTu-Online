 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiXiuxianDlg :
	public CGuiDialog
{	
public:
	CGuiXiuxianDlg();
	~CGuiXiuxianDlg();
	
	
	
	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	
private:

};
