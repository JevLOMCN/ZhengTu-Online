 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiBikeDlg :
	public CGuiDialog
{	
public:
	CGuiBikeDlg();
	~CGuiBikeDlg();
	
	int itemtype1;
	int itemtype2;

	void Additem(int itemtype);
	void Clear(void);
	
	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	
private:
	CGuiButton *item1;
	CGuiButton *item2;
	
};
