#pragma once
#include "..\gui\include\guidialog.h"

class CGuiChangeWarField :
	public CGuiDialog
{
	CGuiListBox*   m_pListBox;

	std::vector<stCountryInfo>  countryList;
	BYTE           m_eState;

public:
	CGuiChangeWarField(BYTE state);

	void OnClose(void);
	void OnCreate();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	void OnSetCloseLocation();

	void RefreshWarField(stReturnDareCountryCmd* pCmd);
};
