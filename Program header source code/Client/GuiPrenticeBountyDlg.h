#pragma once

class CGuiPrenticeBountyDlg : public CGuiDialog
{
public:
	CGuiPrenticeBountyDlg(void);
	~CGuiPrenticeBountyDlg(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
    HRESULT OnRender(float fElapsedTime);

	void OnOk();

	void OnRefreshUIData(stRtnPrenticeBountyUserCmd* pCmd);

private:

};
