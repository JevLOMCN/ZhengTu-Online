 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiZhancheDlg :
	public CGuiDialog
{	
public:
	CGuiZhancheDlg();
	~CGuiZhancheDlg();

	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

private:

};
