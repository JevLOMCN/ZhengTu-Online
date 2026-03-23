 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiZhuanhuanDlg :
	public CGuiDialog
{	
public:
	CGuiZhuanhuanDlg();
	~CGuiZhuanhuanDlg();
	CGuiTable*	m_pTableMaterial1;
	
	bool isneedtype;
	bool zhuangbei;
	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender(float fElapsedTime);
private:
CRoleItem* GetMaterialItem(); // 根据格子索引得到CRoleItem（索引为0,1,2,3）
CGuiTable*			m_pTableMaterials[1];
};
