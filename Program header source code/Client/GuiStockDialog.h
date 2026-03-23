#pragma once
#include "..\gui\include\guidialog.h"


class CStockAccount
{
public:
	CStockAccount();
	typedef std::vector<FirstTen>				tListStockFirstTen;
	typedef tListStockFirstTen::iterator		tListStockFirstTenIt;

	void SetActive(bool bActive);
	void SetLogin(bool bLogin);
	void SetCountFinance(DWORD dwMoney,DWORD dwGold);

	bool GetActive() { return m_bActive; }
	bool GetLogin()  { return m_bLogin; }
	
	DWORD GetMoney() { return m_dwMoney; }
	DWORD GetGold()  { return m_dwGold; }
private:
	bool m_bActive;				//是否激活
	bool m_bLogin;				//是否登陆
	DWORD m_dwMoney;			//账户银子
	DWORD m_dwGold;				//账户金子
};

inline CStockAccount* GetStockAccount()
{
	static CStockAccount sa;
	return &sa;
}

class CGuiStockDialog :
	public CGuiDialog
{
public:
	struct t_StockList : public StockList
	{
		t_StockList(const StockList& sl)
		{
			id		= sl.id;			/// 流水号
			dwNum	= sl.dwNum;			/// 数量
			dwPrice = sl.dwPrice;		/// 委托价格
			dwTime	= sl.dwTime;		/// 委托时间
		}
		StockType type;
	};

	typedef std::vector<t_StockList>				tListStockList;
	typedef tListStockList::iterator				tListStockListIt;

	typedef std::vector<ConsignSelfHistoryType>		tListConsignSelfHistory;
	typedef tListConsignSelfHistory::iterator		tListConsignSelfHistoryIt;	

	CGuiStockDialog(void);
	~CGuiStockDialog(void);

	void OnCreate();
	void OnClose();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnSetCloseLocation();
	HRESULT OnRender(float fElapsedTime);
public:

	void ParseConsignBillCmd(DWORD size,StockList* lst,StockType t);
	void ParseFirst10Cmd(stFirstFiveListStockUserCmd* pCmd);
	void ParseSelfHistoryCmd(DWORD size,ConsignSelfHistoryType* lst,StockType t);

	void CancelStock(DWORD id,StockType type);
	void CleanConsignBill();

	void UpdateAccountManagerButtonState();
	void UpdateAccountFinance();

	void AddConsignBill(const ConsignSelfHistoryType& csht,StockType  type);		//添加委托单
	void UpdateConsignMoneyBillList();
	void UpdateConsignGoldBillList();

	void AddHistoryBill(const StockList& stock,StockType  type);		//添加历史记录委托单
	void UpdateHistoryBillList();

private:
	void OnClickActiveCount();		//账户激活
	void OnClickManageCount();		//账户管理
	void OnClickDelegate();			//委托
	void OnClickQuery();			//查询

	void OnClickPopOutFinance();	//取出资金
	void OnClickPushInFinance();	//注入资金

	void OnClickBuyGold();			//买入金子
	void OnClickSaleGold();			//卖出金子

	void OnClickCancelBill();		//撤销单子

	void OnClickBuyQuery();			//买入查询
	void OnClickSaleQuery();		//卖出查询

	void OnPushInOK();				//注入资金确定
	void OnPopoutOK();				//取出资金确定

	void OnConsignBuyOK();			//委托买进确定
	void OnConsignSaleOK();			//委托卖出确定

	void OnCancelBill();			//撤销
private:
	tListStockListIt FindConsignHistoryItByID(DWORD id);
	int  GetMoneyNum(const char* pIngot,const char* pTeal,const char* pCent);
	int  GetMoneyNum(int IngotId,int TealId,int CentId);
	void ClearSelfHistory(StockType type);
private:
	tListConsignSelfHistory	m_goldList;
	tListConsignSelfHistory	m_moneyList;

	tListStockList	m_HistoryList;		

	CGuiListBox*	m_pListBox_Buy;
	CGuiListBox*	m_pListBox_Sell;

	CGuiListBox*	m_pHistoryList;		//历史委托单
	CGuiListBox*	m_pGoldList;		//查询金子
	CGuiListBox*	m_pMoneyList;		//查询银子
};
