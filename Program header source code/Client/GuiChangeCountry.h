#pragma once
#include "..\gui\include\guidialog.h"

class CGuiChangeCountry :
	public CGuiDialog
{
	CGuiListBox*   m_pListBox;

public:
	CGuiChangeCountry(void);
	

	void OnClose(void);
	void OnCreate();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender( float fElapsedTime ); //soke ลัน๚หตร๗

	void OnSetCloseLocation();
};
