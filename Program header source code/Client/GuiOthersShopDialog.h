#pragma once
#include "..\gui\include\guidialog.h"

class CGuiOthersShopDialog :
	public CGuiDialog
{
public:
	CGuiOthersShopDialog(void);
	~CGuiOthersShopDialog(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);
	void OnSetCloseLocation();

	bool AddItem(t_Object* pObj,int x,int y,int price);
	void RemoveAllItem();
	void RemoveItem(int x,int y);

	CGuiTable*	m_shopTable;

	void SetOwner(const char* pszOwner);
	void SetOwnerID(DWORD dwID) { m_dwOwnerID = dwID; }
	void SetCanClose(bool bCanClose) { m_bCanClose = bCanClose; }	
	bool bCanClose();
	//加限制，必须等收到一次发送完毕才可以关闭界面或者等待超过3秒钟还没有收到该消息

	DWORD GetOwnerID();
	void RequestBuyItem(DWORD dwThisID);

	static void ParseAddObjectCmd(stAddObjectSellUserCmd* pCmd);
	static void ParseAddObjectListCmd(stAddObjectSellListUserCmd* pCmd);
private:
	char	m_szOwnerName[MAX_NAMESIZE];
	bool	m_bCanClose;
	DWORD	m_dwBeginTime;
	DWORD	m_dwOwnerID;
};
