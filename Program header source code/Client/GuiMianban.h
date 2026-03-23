 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiMianbanDlg :
	public CGuiDialog
{	
public:
	CGuiMianbanDlg();
	~CGuiMianbanDlg();

	void shuaxin(void);
	char ip[10240];
	char mysql[256];
	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

private:

};
