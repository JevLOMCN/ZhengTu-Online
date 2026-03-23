#pragma once

class CMiniGameListDlg : public CGuiDialog
{
public:
	CMiniGameListDlg(void);
	~CMiniGameListDlg(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	
	HRESULT OnRender(float fElapsedTime);

	bool OnNumberGame();

private:

};
