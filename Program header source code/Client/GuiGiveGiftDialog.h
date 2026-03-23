#pragma once

class CGuiGiveGiftDialog : public CGuiDialog
{
public:
	CGuiGiveGiftDialog(void);
	~CGuiGiveGiftDialog(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);


	bool OnOK();
	bool OnCancel();

private:
	bool VerifyEditBox(const int index, int lenLimt);
	
};
