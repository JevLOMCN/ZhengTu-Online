#pragma once

class CGuiMasterBountyDlg : public CGuiDialog
{
public:
	CGuiMasterBountyDlg(void);
	~CGuiMasterBountyDlg(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
    HRESULT OnRender(float fElapsedTime);

	void OnOk();

	void OnRefreshUIData(stRtnMasterBountyUserCmd* pCmd);

private:

};
