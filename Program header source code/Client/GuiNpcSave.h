#pragma once
#include "..\gui\include\guidialog.h"

#include <map>
//class CGuiEquip;
/**
 * \brief 简短描述 包裹界面
 * 
 * 详细描述
 * 
 */
class CGuiSavingBoxDialog :
	public CGuiDialog
{
public:

	CGuiSavingBoxDialog(void);
	void OnClose(void);
	void OnCreate();

    ///存放物品的格子
	CGuiTable* m_pTable[3];
	//CGuiImage* m_pImage;
	///钱币数输入框
	//CGuiEditBox* m_pMoneyBox;

	void SetMoney(DWORD dwMoney);
	DWORD GetMoney();
	void OnInitScript();
	void OnSetCloseLocation();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void SetCharName(const char* name);
	void OnMoveMoney();

	DEFINE_TYPENAME("CGuiDialog");

	BEGIN_REGLUALIB("Gui.StoreItemDialog")
	END_REGLUALIB()

	BEGIN_REGLUALIB_MEMBER()
	LUALIB_ITEM_FUNC("OnMoveMoney",void(CGuiSavingBoxDialog*),CGuiSavingBoxDialog::OnMoveMoney)
	END_REGLUALIB_MEMBER()
};
