#pragma once


#include "..\gui\include\guidialog.h"
class CRoleItem;

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CGuiItemMake :
	public CGuiDialog
{
	std::vector<ObjectBase_t*> m_aMakeObject;

public:
	CGuiStatic*  m_pLabel;
	CGuiListBox* m_pListBox;
	CGuiTable*   m_pTable;


	CGuiItemMake(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);
	void SetParam(std::vector<std::string> & params);
private:
	bool ResetMake(ObjectBase_t* pObject);
	void AddMakeObject(ObjectBase_t* pObject);
    void LoadMakeImage(ObjectBase_t* pObject,CGuiItemCell* pCell, bool bSelect);
	
};
