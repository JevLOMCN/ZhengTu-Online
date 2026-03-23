#pragma once
#include "..\gui\include\guidialog.h"

class CGuiCongratulateInfo :
	public CGuiDialog
{
public:
	CGuiCongratulateInfo();
	~CGuiCongratulateInfo();

	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	void OnCreate();
	void OnSetCloseLocation();
	HRESULT OnRender(float fElapsedTime);
};
