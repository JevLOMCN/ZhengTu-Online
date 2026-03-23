#pragma once

#include "..\gui\include\guidialog.h"
#include "Auction.h"

class CGuiQQPannel;
class CGuiAuctionMoneyInputDialog;

class CGuiAuctionDialog :
	public CGuiDialog
{
public:
	CGuiAuctionDialog(void);
	~CGuiAuctionDialog(void);

	void OnCreate(void);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnClose(void);
	HRESULT OnRender(float fElapsedTime);
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	CGuiTable* GetTableByPos( const stObjectLocation& pos );

	bool OnClickBrowse();		//浏览
	bool OnClickRoup();			//竞拍
	bool OnClickAuction();		//拍卖
	bool OnClickClose();		//关闭
	bool OnBrowseSearch(BYTE type);	//按type类型搜索(浏览)
	bool OnRoupSearch(BYTE type);	//按type类型搜索(竞拍)

	bool OnClickSearch();
	bool OnClickBuyOnBrowse();
	bool OnClickAddPrice();
	bool OnClickBuyAuction();
	bool OnYiKouJia();

	bool OnBrowseNextPage();
	bool OnRoupNextPage();
	bool OnAuctionNextPage();

	bool OnBrowsePrePage();
	bool OnRoupPrePage();
	bool OnAuctionPrePage();

	bool OnStartAuction();
	bool OnStopAuction();

	bool OnClickSortBrowseByNeedLevel();
	bool OnClickSortBrowseByKind();
	bool OnClickSortBrowseByOwner();
	bool OnClickSortBrowseByTime();
	bool OnClickSortBrowseByPrice();

	bool OnClickSortRoupByNeedLevel();
	bool OnClickSortRoupByKind();
	bool OnClickSortRoupByOwner();
	bool OnClickSortRoupByTime();
	bool OnClickSortRoupByPrice();

	bool OnCheckMoney();
	bool OnCheckGoldCoin();

	bool RemoveAuctionItem(DWORD dwId,enumAuctionListType type);
	bool AddAuctionItem(stAuctionItem* pItem,enumAuctionListType type);//1:查看列表 2:拍卖列表 3:竞标列表 4:所有列表

	void UpdateAuctionItem(int page,stAuctionItem* pItem);
	void UpdateAuctionItem(CGuiListBox* &pList,stAuctionItem* pItem);
	void UpdateList( tListAuctionItem& list,enumAuctionListType type );
	void CloseMoneyInputDialog();
	void OnConfirmStartAuction();
private:
	bool RemoveAuctionItem(CGuiTable* &pTable,CGuiListBox* &pList,int index);
	bool AddAuctionItem(stAuctionItem* pItem,int index);
	bool RemoveAll(int index);
	int  GetIndex(DWORD dwId,CGuiListBox* pList);
	bool RemoveAuctionItemByPage(DWORD dwId,int page);
	int	 GetSelectAuctionId(CGuiListBox* pList);
	void ClearItem();

	bool AuctionSelectItem(enumAuctionListType type);

	
	void SetTotalPage(int page,int index);		//index 表示tab页的index
	bool GotoPage(int page,int index);			//跳转到第page页
	void SetCurPage(int page,int index);		//设置当前页	
	int  GetCurPage(int index);					//得到当前页
	int  GetTotalPage(int index);				//得到总页数
private:
	CGuiQQPannel*	m_pBrowsePannel;
	CGuiListBox*	m_pListBox[3];
	CGuiTable*		m_pTable[3];
	CGuiImage*		m_pImage[3];
	CGuiTab*		m_pTab;
	CGuiTable*		m_pAuctionTable;

	BYTE			m_byBrowseType;
	BYTE			m_byRoupType;

	CGuiAuctionMoneyInputDialog*	m_pMoneyInputDlg;
	bool			m_bIsWaitingDatas;
	DWORD			m_dwLastQueryTime;

	int				m_iTotalPage[3];
};
