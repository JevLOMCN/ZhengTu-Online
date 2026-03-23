#pragma once
#include "..\gui\include\guidialog.h"

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CGuiItemUp :
	public CGuiDialog
{
	std::vector<ObjectBase_t*> m_aUpObject;
	bool m_bChange;

public:
	CGuiStatic* m_pLabel1;
	CGuiStatic* m_pLabel2;
	CGuiTable*	m_pTable1;
	CGuiTable*	m_pTable2;
	CGuiTable*	m_pTable3;
	CGuiTable*	m_pTable4;

	CGuiItemUp(bool bChange);

	void OnCreate(void);
	void OnClose(void);
	void SetParam(std::vector<std::string> & params);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	void SetChange(bool bChange){ m_bChange = bChange;}

	HRESULT OnRender(float fElapsedTime);
};
