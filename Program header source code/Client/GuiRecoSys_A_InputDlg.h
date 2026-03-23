#pragma once

class CGuiRecoSys_A_InputDlg : public CGuiDialog
{
public:
	CGuiRecoSys_A_InputDlg(void);
	~CGuiRecoSys_A_InputDlg(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
    HRESULT OnRender(float fElapsedTime);

	void OnOk();

private:

};
