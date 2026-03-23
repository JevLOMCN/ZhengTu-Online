 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiJiazubossDlg :
	public CGuiDialog
{	
public:
	CGuiJiazubossDlg();
	~CGuiJiazubossDlg();
	CGuiTable*	m_pTableMaterial1;
	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	DWORD level;
	DWORD exp;
	void shuaxin(void);


private:
CRoleItem* GetMaterialItem(); // 根据格子索引得到CRoleItem（索引为0,1,2,3）
CGuiTable*			m_pTableMaterials[1];
};
