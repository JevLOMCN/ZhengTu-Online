 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiBieShuDlg :
	public CGuiDialog
{	
public:
	CGuiBieShuDlg();
	~CGuiBieShuDlg();
	CGuiTable*	m_pTableMaterial1;
	
	DWORD bieshu_level;//别墅等级
	DWORD bieshu_exp;//别墅经验
	DWORD bieshu_state;//别墅激活状态
	DWORD bieshu_gong;//攻击加成
	DWORD bieshu_fang;//防御加成
	DWORD bieshu_hp;//生命加成
	void shuaxin(void);

	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender(float fElapsedTime);
private:
CRoleItem* GetMaterialItem(); // 根据格子索引得到CRoleItem（索引为0,1,2,3）
CGuiTable*			m_pTableMaterials[1];
};
