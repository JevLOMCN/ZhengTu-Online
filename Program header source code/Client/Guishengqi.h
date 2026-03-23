 
#pragma once 
 #include "..\gui\include\guidialog.h"
class CGuishengqiDlg :
	public CGuiDialog
{	
public:
	CGuishengqiDlg();
	~CGuishengqiDlg();

	void OnClose(void);
	void OnCreate(void);
	void update(int num);
	void SHUAXIN();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	CGuiTable*	m_pTableMaterial1;
	CGuiStatic *cltext;
	CGuiStatic *moneytext;

	int select;
	int sq1;

private:
CRoleItem* GetMaterialItem();
CGuiButton *shengq1; 
CGuiStatic *wenzi;
CGuiTable*			m_pTableMaterials[1];
};
