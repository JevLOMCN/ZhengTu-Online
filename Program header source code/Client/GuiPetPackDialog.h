#pragma once
#include "..\gui\include\guidialog.h"

class CGuiPetPackDialog :
	public CGuiDialog
{
public:
	CGuiPetPackDialog(void);
	~CGuiPetPackDialog(void);

	void OnClose(void);
	void OnCreate();
	HRESULT OnRender( float fElapsedTime );
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnSetCloseLocation();

	void UpdateCanUseCells();
	bool bIsCellCanUse(int x,int y);
	bool bIsCellCanUse(POINT pt);
	
	static bool OnTableEvent(CGuiTable* pTable,stPointI pt,UINT uMsg);

	CGuiTable*	m_pPetPackTable;
	const static int m_iPetPackTableID;
private:
	void OnClickOK();
private:
	//因为刚好是8*10的格子
	BYTE m_byCellState[10];
};
