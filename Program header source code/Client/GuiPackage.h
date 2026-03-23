#pragma once
#include "..\gui\include\guidialog.h"

class CRoleItem;


class CGuiPackage :
	public CGuiDialog
{
	CRoleItem*          m_pRoleItem;
	
public:
	CGuiPackage(CRoleItem* pRoleItem);
	void OnCreate();
	void OnClose(void);
	void OnSetCloseLocation();
	HRESULT OnRender(float fElapsedTime);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	
	CGuiTable*          m_pTable;
	CRoleItem*      GetPackItem()
	{ 
		return m_pRoleItem;
	}
};
