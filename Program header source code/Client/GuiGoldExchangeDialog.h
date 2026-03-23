#pragma once
#include "..\gui\include\guidialog.h"

class CGuiGoldExchangeDialog :
	public CGuiDialog
{
public:
	CGuiGoldExchangeDialog(void);
	~CGuiGoldExchangeDialog(void);

	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	void OnCreate();
	void OnIDOK();

	HRESULT OnRender(float fElapsedTime);
	void UpdatePointNum();

private:
	CGuiEditBox* m_pEditBox;

	int			m_iInputPointNum;
};
