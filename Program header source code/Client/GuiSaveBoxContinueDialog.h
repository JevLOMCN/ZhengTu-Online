#pragma once
#include "..\gui\include\guidialog.h"

class CGuiSaveBoxContinueDialog :
	public CGuiDialog
{
public:
	CGuiSaveBoxContinueDialog(void);
	~CGuiSaveBoxContinueDialog(void);
	void OnClose(void);
	void OnCreate();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	void SetBoxID(BYTE box);
private:
	bool OnBtnClick(int id);
	bool OnEditDaysChange();

	BYTE m_byBoxId;
	int  m_price;
};
