#pragma once
#include "..\gui\include\guidialog.h"

class CGuiAuctionMoneyInputDialog :
	public CGuiDialog
{
public:
	CGuiAuctionMoneyInputDialog(BYTE type);
	~CGuiAuctionMoneyInputDialog(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);
	void OnSetCloseLocation();

	bool OnIDOK();

	void SetMaxPrice(int price);
	void SetMinPrice(int price);

	int		m_iMinPrice;
	int		m_iMaxPrice;

	DWORD	m_dwAuctionId;
private:
	BYTE	m_type;
};
