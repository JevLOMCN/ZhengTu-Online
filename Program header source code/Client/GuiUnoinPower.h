#pragma once
#include "..\gui\include\guidialog.h"

class CGuiUnoinPower :
	public CGuiDialog
{
	CGuiListBox*   m_pListBox1;
	CGuiListBox*   m_pListBox2;
	char           m_szName[MAX_NAMESIZE];
	byte           m_byPower[2];

	void   RefreshData();

public:
	CGuiUnoinPower(char* name);

	void OnClose(void);
	void OnCreate();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	void OnSetCloseLocation();

	void SetPower(byte* byPower);

};
