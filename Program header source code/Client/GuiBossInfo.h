
#pragma once
#include "..\gui\include\guidialog.h"

class CGuiBossInfoDialog :
	public CGuiDialog
{
public:

	void OnClose(void);
	void OnResetScreenSize();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnCreate();
	void OnSetCloseLocation();
	
	void SetInfo(char* c);
	CGuiListBox* m_pListBox;
};


