 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiZuoqiDlg :
	public CGuiDialog
{	
public:
	CGuiZuoqiDlg();
	~CGuiZuoqiDlg();

	DWORD level;
	DWORD exp;
	DWORD num1;
	DWORD num2;
	DWORD num3;
	DWORD num4;
	DWORD num5;
	DWORD num6;
	DWORD num7;
	DWORD num8;
	DWORD num9;
	DWORD num10;
	DWORD num11;
	DWORD num12;
	DWORD num13;
	DWORD num14;
	DWORD num15;

	CGuiTable*	m_pTableMaterial1;
	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void shuaxin(void);
	HRESULT OnRender(float fElapsedTime);

private:
CRoleItem* GetMaterialItem(); // 根据格子索引得到CRoleItem（索引为0,1,2,3）
CGuiTable*			m_pTableMaterials[1];
};
