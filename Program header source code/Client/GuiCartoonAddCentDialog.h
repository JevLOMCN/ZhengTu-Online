#pragma once
#include "..\gui\include\guidialog.h"

class CGuiCartoonAddCentDialog :
	public CGuiDialog
{
public:
	CGuiCartoonAddCentDialog(DWORD dwCartoonID);
	~CGuiCartoonAddCentDialog(void);

	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	void OnCreate();
	void OnIDOK();

	void UpdateGoldNum();
	void SetCartoonPetID(DWORD dwCartoonID) { m_dwCartoonID = dwCartoonID; }
private:
	int				GetInputGoldNum();
	CGuiEditBox*	m_pEditBox;
	DWORD			m_dwCartoonID;
};
