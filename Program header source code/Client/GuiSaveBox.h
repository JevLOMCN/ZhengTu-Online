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
	HRESULT OnRender( float fElapsedTime );

    ///存放物品的格子
	CGuiTable* m_pTable[3];

	void OnSetCloseLocation();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	
	bool ResetContents();
private:
	//inline void SetDay(int id,int day);
	inline CGuiTable* GetTableByPosY(const stObjectLocation pos);
public:
	CGuiTable* GetBoxTable( const stObjectLocation & pos );

	const static int csShowColCount = 7;
	const static int csShowRowCount = 6;
};
