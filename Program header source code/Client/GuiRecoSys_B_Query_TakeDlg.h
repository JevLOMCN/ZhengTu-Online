#pragma once

class CGuiRecoSys_B_Query_TakeDlg : public CGuiDialog
{
public:
	CGuiRecoSys_B_Query_TakeDlg(void);
	~CGuiRecoSys_B_Query_TakeDlg(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
    HRESULT OnRender(float fElapsedTime);

	void OnOk();

	void OnRefreshUIData(stRtnRecommendedBountyUserCmd* pCmd);

private:

};
