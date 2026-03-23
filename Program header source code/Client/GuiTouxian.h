 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiTouxianDlg :
	public CGuiDialog
{	
public:
	CGuiTouxianDlg();
	~CGuiTouxianDlg();
	int level;
	int exp;
	int num1;
	int num2;
	int num3;
	int num4;
	int num5;
	int num6;
	CGuiTable*	m_pTableMaterial1;
	CGuiStatic* leveluptext;
	void OnClose(void);
	void OnCreate(void);
	void SHUAXIN(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
private:
	CRoleItem* GetMaterialItem(); // 根据格子索引得到CRoleItem（索引为0,1,2,3）
	
	CGuiTable*			m_pTableMaterials[1];

	CGuiProcess* levelnum;
	CGuiImage* image;
	CGuiStatic* n1;
	CGuiStatic* n2;
	CGuiStatic* n3;
	CGuiStatic* n4;
	CGuiStatic* n5;
	CGuiStatic* n6;
	
};
