 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiZhancheCmdDlg :
	public CGuiDialog
{	
public:
	CGuiZhancheCmdDlg();
	~CGuiZhancheCmdDlg();

	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnResetScreenSize();
	int num;
	HRESULT OnRender(float fElapsedTime);
private:

};
