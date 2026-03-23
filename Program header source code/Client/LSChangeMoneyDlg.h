#pragma once

class CGuiLSChangeMoneyDlg : public CGuiDialog
{
public:
	CGuiLSChangeMoneyDlg(void);
	~CGuiLSChangeMoneyDlg(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);


	bool OnOK();
	
private:

	CGuiTab * m_pTab;

	void SetPreCostMoneyText();
	void SetPreCostDouziText();

	void SetNowDouziAndMoneyText();

	DWORD GetWantDouzi();
	DWORD GetWantMoney();
	
};
