 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiFubenDlg :
	public CGuiDialog
{	
public:
	CGuiFubenDlg();
	~CGuiFubenDlg();
	
	void OnClose(void);
	void OnCreate(void);
	void select_fb(void);
	int select;
	int fbcs1;
	int fbcs2;
	int fbcs3;
	int fbcs4;
	int fbcs5;
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	
private:
CGuiTab*               m_pTab;
};
