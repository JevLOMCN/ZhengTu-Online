#pragma once
#include "..\gui\include\guidialog.h"


class CGuiCountryWarQuery :
	public CGuiDialog
{
	CGuiTab*               m_pTab;

	char*  GetCountryName(DWORD dwID);


public:
	CGuiCountryWarQuery(void);
	
	void OnClose(void);
	void OnCreate();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);


	void RefreshWarList(stReturnDareRecordStatCmd* pCmd);
	void RefreshWarPlanList(stReturnDareRecordPlanCmd* pCmd);
	void RefreshWarResultList(stReturnDareRecordResultCmd* pCmd);
};
