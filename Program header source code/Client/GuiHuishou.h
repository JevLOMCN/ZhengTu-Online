 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiHuishouDlg :
	public CGuiDialog
{	
public:
	CGuiHuishouDlg();
	~CGuiHuishouDlg();
	CGuiTable*	m_pTableMaterial1;
	struct huishou_info
	{
		int objectID;//物品id
		char name[MAX_NAMESIZE];//物品名称
		int price;//回收单价
		int type;//回收类型
	};
	
	huishou_info huishou[100];
	void shuaxin(void);
	
	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	HRESULT OnRender(float fElapsedTime);
private:
CRoleItem* GetMaterialItem(); // 根据格子索引得到CRoleItem（索引为0,1,2,3）
CGuiTable*			m_pTableMaterials[1];
};
