 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiAutoUseDlg :
	public CGuiDialog
{	
public:
	CGuiAutoUseDlg();
	~CGuiAutoUseDlg();
	
	CGuiTable*	m_pTableMaterial1;
	bool onuse;
	bool canuse(void);
	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender(float fElapsedTime);
	
private:
CRoleItem* GetMaterialItem(); // 根据格子索引得到CRoleItem（索引为0,1,2,3）
CGuiTable*			m_pTableMaterials[1];
};
