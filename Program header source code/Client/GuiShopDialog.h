/**
 *\file		GuiShopDialog.h
 *\version	$ID$
 *\author	$$$04公司
 *			$$$04公司@###.com
 *\date		2005-07-06 11:33:00
 * \brief	摆摊对话框
 * 
 * 摆摊(还未实现)
 * 
 */

#ifndef _GUI_SHOP_DIALOG_H
#define _GUI_SHOP_DIALOG_H




/**
 * \brief 摆摊对话框
 * 
 * 摆摊
 * 
 */
class CGuiShopDialog :public CGuiDialog
{
public:
	enum ShopType
	{
		MyShop,
		OthersShop
	};
	CGuiShopDialog(ShopType type = MyShop);
	~CGuiShopDialog();
public:
	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);
	void OnSetCloseLocation();

	void OnMainRoleShopOpen();
	void RemoveAllItem();
	bool RemoveItem(int x,int y);
	bool RemoveItem(DWORD dwThisID);

	bool AddItem(CRoleItem* pItem);
	void SetItemPrice(DWORD dwThisID,int price);
	bool bItemCanput(CRoleItem* pItem,POINT &pt);
	bool IsItemIn(DWORD dwThisID);
	/*bool GetCanPutPos(POINT &pt);*/
	CGuiTable*	m_shopTable;
	void SendAllItemToServer();
	static bool OnOpenShop( DWORD dwShopOwnerID );
private:
	bool RenderText();
	bool OnIDOK();
	bool OnIDCancel();	
private:
	ShopType	m_shopType;
};

#endif