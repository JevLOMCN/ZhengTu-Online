#pragma once

class CMGMoneyInputDlg : public CGuiDialog
{
public:
	CMGMoneyInputDlg(void);
	~CMGMoneyInputDlg(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);


	bool OnOK();
	
private:

	
};
