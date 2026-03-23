 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiXiulianDlg :
	public CGuiDialog
{	
public:
	CGuiXiulianDlg();
	~CGuiXiulianDlg();
	
	DWORD xiulianexp;
	
	DWORD level1;
	DWORD level2;
	DWORD level3;
	DWORD level4;
	DWORD level5;

	DWORD exp1;
	DWORD exp2;
	DWORD exp3;
	DWORD exp4;
	DWORD exp5;

	DWORD money1;
	DWORD money2;
	DWORD money3;
	DWORD money4;
	DWORD money5;

	DWORD num1;
	DWORD num2;
	DWORD num3;
	DWORD num4;
	DWORD num5;
	void shuaxin(void);
	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	
private:

};
