 
#pragma once 

#include "..\gui\include\guidialog.h"

class CGuiChenghaoDlg :
	public CGuiDialog
{	
public:
	CAnimation m_AniBody[12];
	//sky 称号
	int select;
	struct Chenghao
	{
		int state;
	};
	Chenghao  m_Chenghao[60];
	CGuiChenghaoDlg();
	~CGuiChenghaoDlg();
	void UpdateChenghao(void);
	CGuiTable*	m_pTableMaterial1;
	void OnClose(void);
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
private:
	CRoleItem* GetMaterialItem(); // 根据格子索引得到CRoleItem（索引为0,1,2,3）
	HRESULT OnRender(float fElapsedTime);
	CGuiTable*			m_pTableMaterials[1];
	CGuiTab*               m_pTab;
};
