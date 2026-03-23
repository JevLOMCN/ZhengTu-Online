 
#pragma once 
 #include "..\gui\include\guidialog.h"
class CGuijianzhongDlg :
	public CGuiDialog
{	
public:
	CGuijianzhongDlg();
	~CGuijianzhongDlg();
	CGuiTable*	m_pTableMaterial1;
	CGuiStatic *cltext;
	CGuiStatic *moneytext;
	DWORD j1;
	DWORD j2;
	DWORD j3;
	DWORD j4;
	DWORD j5;
	DWORD j6;
	int select;
	void OnClose(void);
	void OnCreate(void);
	void UPDATE(void);
	void UPDATEJZSX(DWORD j1);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
private:
CRoleItem* GetMaterialItem(); // 根据格子索引得到CRoleItem（索引为0,1,2,3）
CGuiImage *jianimg;


CGuiImage *jianname;
CGuiButton *jian1;
CGuiButton *jian2;
CGuiButton *jian3;
CGuiButton *jian4;
CGuiButton *jian5;
CGuiButton *jian6;
CGuiTable*			m_pTableMaterials[1];
};
