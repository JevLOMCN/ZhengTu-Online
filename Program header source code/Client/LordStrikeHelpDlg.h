#pragma once


class CGuiGameHelpDlg : public CGuiDialog
{
public:
	CGuiGameHelpDlg();
	~CGuiGameHelpDlg();

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);

private:
	void LoadDescFromFile();
};
