 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiLicaiDlg :
	public CGuiDialog
{	
public:
	CGuiLicaiDlg();
	~CGuiLicaiDlg();
	CGuiTable*	m_pTableMaterial1;
	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	int point;
private:
	CGuiTab*       m_pTab;
	CGuiEditBox* m_pEditBox;
};
