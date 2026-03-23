#pragma once
#include "..\gui\include\guidialog.h"


typedef std::vector<stPointI>           POINTLIST;
typedef std::vector<POINTLIST>          LINELIST;

class CGuiStock :
	public CGuiDialog
{
	CGuiListBox* m_pListBox_Buy;
	CGuiListBox* m_pListBox_Sell;


	LINELIST                             m_lineList;
	std::vector<ConsignHistoryType>      m_historyDataList;     

	void UpData();
	void OnDraw();

public:
	CGuiStock(void);
	~CGuiStock(void);

	void SetFirstFiveList(stFirstFiveListStockUserCmd* pCmd);
	void OnCreate();
	void InitData();
	void OnClose();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnSetCloseLocation();
	HRESULT OnRender(float fElapsedTime);
};


class CGuiStockAccount :
	public CGuiDialog
{
public:
	CGuiStockAccount(void);
	~CGuiStockAccount(void);
	
	void Refresh();
	void OnClose();

	void OnCreate();
	
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	
};


class CGuiStockCanel :
	public CGuiDialog
{
	struct stStockListEx : public StockList
	{
		StockType  type;
	};

	std::vector<stStockListEx>     m_stockList;
	CGuiListBox*               m_pListBox;
public:
	CGuiStockCanel(void);
	~CGuiStockCanel(void);

	void OnClose();
	void OnCreate();
	void Refresh();
	
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void ClearStockList(){
		m_stockList.clear();
		Refresh();
	}
	void DeleteStock(DWORD id,StockType type);
	void AddStockList(StockList* pStock,StockType  type){
		stStockListEx stock;
		memcpy(&stock,pStock,sizeof(stock));
		stock.type = type;
		m_stockList.push_back(stock);
	}
	
};


class CGuiStockConsign :
	public CGuiDialog
{
public:
	CGuiStockConsign(void);
	~CGuiStockConsign(void);

	void OnClose();
	void OnCreate();
	void Refresh();
	
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	//HRESULT OnRender(float fElapsedTime);
};


class CGuiStockQuery :
	public CGuiDialog
{
	struct stStockListEx : public ConsignSelfHistoryType
	{
		StockType  type;
	};

	std::vector<stStockListEx>     m_stockList;
	CGuiListBox*                   m_pListBox;

public:
	CGuiStockQuery(void);
	~CGuiStockQuery(void);

	void OnClose();
	void OnCreate();
	void Refresh();

	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void ClearStockList(){
		m_stockList.clear();
		Refresh();
	}
	void AddStockList(ConsignSelfHistoryType* pStock,StockType  type){
		stStockListEx stock;
		memcpy(&stock,pStock,sizeof(stock));
		stock.type = type;
		m_stockList.push_back(stock);
	}
};


class CGuiStockOut :
	public CGuiDialog
{
public:
	CGuiStockOut(void);
	~CGuiStockOut(void);

	void OnClose();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
};


class CGuiStockLogin :
	public CGuiDialog
{
public:
	CGuiStockLogin(void);
	~CGuiStockLogin(void);

	void OnClose();
	void OnCreate();
	
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	
};

class CGuiStockAction :
	public CGuiDialog
{
public:
	CGuiStockAction(void);
	~CGuiStockAction(void);

	void OnClose();
	void OnCreate();
	
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
};

class CGuiStockPwdUp :
	public CGuiDialog
{
public:
	CGuiStockPwdUp(void);
	~CGuiStockPwdUp(void);

	void OnClose();
	void OnCreate();
	
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
};

class CStockManager{

	DWORD m_dwGold;/// 当前帐号金子
	DWORD m_dwMoney;/// 当前帐号银币

	bool  m_bActive;
	bool  m_bLogin;

	bool  IsPass();

public:
	CStockManager();
	~CStockManager();

	void  SetActive(bool active){
		m_bActive = active;
	}
	void  SetLogin(bool login){
		m_bLogin = login;
	}

	DWORD  GetGold() {
		return m_dwGold;
	}
	DWORD  GetMoney() {
		return m_dwMoney;
	}
	void SetAccount(DWORD dwGold,DWORD m_dwMoney);
	bool CheckPwd(CGuiEditBox* pEditBox,const char* pTitle);
	int  GetMoneyNum(const char* pIngot,const char* pTeal,const char* pCent);

	CGuiStock*   AddStock();
	CGuiStock*   m_pStock;

	CGuiStockAccount*  AddStockAccount();
	CGuiStockAccount*  m_pStockAccount;

	CGuiStockCanel*  AddStockCanel();
	CGuiStockCanel*  m_pStockCanel;

	CGuiStockConsign*  AddStockConsign();
	CGuiStockConsign*  m_pStockConsign;

	CGuiStockQuery*  AddStockQuery();
	CGuiStockQuery*  m_pStockQuery;

	CGuiStockOut*  AddStockOut();
	CGuiStockOut*  m_pStockOut;

	CGuiStockLogin*  AddStockLogin();
	CGuiStockLogin*  m_pStockLogin;

	CGuiStockAction*  AddStockAction();
	CGuiStockAction*  m_pStockAction;

	CGuiStockPwdUp*  AddStockPwdUp();
	CGuiStockPwdUp*  m_pStockPwdUp;
};

extern CStockManager* GetStockManager();