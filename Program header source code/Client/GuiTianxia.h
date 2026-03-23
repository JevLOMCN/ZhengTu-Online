 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiTianxiaDlg :
	public CGuiDialog
{	
public:
	CGuiTianxiaDlg();
	~CGuiTianxiaDlg();
	// sky 天下第一结构体
	struct txdy_info
	{
		int userid;//角色id
		char username[MAX_NAMESIZE];//角色名
		int saidian;//赛点
		int jifen;//积分
	};
	txdy_info tianxia[32];
	
	void shuaxin(void);
	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender(float fElapsedTime);
private:

};
