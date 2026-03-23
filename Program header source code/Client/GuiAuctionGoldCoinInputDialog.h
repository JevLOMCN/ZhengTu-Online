#pragma once
#include "..\gui\include\guidialog.h"

class CGuiAuctionGoldCoinInputDialog :
	public CGuiDialog
{
public:
	CGuiAuctionGoldCoinInputDialog(void);
	~CGuiAuctionGoldCoinInputDialog(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);

	bool OnIDOK();

	DWORD	m_dwAuctionId;
};
