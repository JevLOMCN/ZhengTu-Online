#pragma once
#include "..\gui\include\guidialog.h"

class CGuiChangeCountrySalary :
	public CGuiDialog
{
	CGuiListBox*   m_pListBox;

public:
	CGuiChangeCountrySalary(void);
	

	void OnClose(void);
	void OnCreate();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender( float fElapsedTime );

	void OnSetCloseLocation();
};
