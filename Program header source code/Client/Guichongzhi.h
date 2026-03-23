 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuichongzhiDlg :
	public CGuiDialog
{	
public:
	CGuichongzhiDlg();
	~CGuichongzhiDlg();

	void OnClose(void);
	void OnCreate(void);

	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	
private:
	
};
