 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiZhanchelistDlg :
	public CGuiDialog
{	
public:
	CGuiZhanchelistDlg();
	~CGuiZhanchelistDlg();
	struct zhanche_info
	{
		DWORD id;//物品id
		DWORD septid;//战车名字
		char name[MAX_NAMESIZE];//战车名字
		DWORD type;//战车类型
		char lingyongname[MAX_NAMESIZE];//领用人名字
		DWORD lingyongid;//领用人id
	};
	zhanche_info zhanche[20];


	void shuaxin(void);

	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

private:
CGuiListBox* m_pListBoxZhanche; //战车列表
};
