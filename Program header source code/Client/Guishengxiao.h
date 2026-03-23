 
#pragma once 
 #include "..\gui\include\guidialog.h"
class CGuishengxiaoDlg :
	public CGuiDialog
{	
public:
	CGuishengxiaoDlg();
	~CGuishengxiaoDlg();

	void OnClose(void);
	void OnCreate(void);
	void update(int num);
	void SHUAXIN();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	CGuiTable*	m_pTableMaterial1;
	CGuiStatic *cltext;
	CGuiStatic *moneytext;
	int select;
	int sx1;
	int sx2;
	int sx3;
	int sx4;
	int sx5;
	int sx6;
	int sx7;
	int sx8;
	int sx9;
	int sx10;
	int sx11;
	int sx12;


private:
CRoleItem* GetMaterialItem();
CGuiButton *shengx1;
CGuiButton *shengx2;
CGuiButton *shengx3;
CGuiButton *shengx4;
CGuiButton *shengx5;
CGuiButton *shengx6;
CGuiButton *shengx7;
CGuiButton *shengx8;
CGuiButton *shengx9;
CGuiButton *shengx10;
CGuiButton *shengx11;
CGuiButton *shengx12;
CGuiStatic *wenzi;
CGuiTable*			m_pTableMaterials[1];
};
