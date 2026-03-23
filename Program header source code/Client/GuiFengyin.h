 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiFengyinDlg :
	public CGuiDialog
{	
public:
	CGuiFengyinDlg();
	~CGuiFengyinDlg();
	
	
	void shuaxin(int level);
	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	
private:

};
