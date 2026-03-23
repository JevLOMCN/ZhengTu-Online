#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GuiItem.h"
#include "./RoleItem.h"
#include ".\GuiStockDialog.h"

#define Stock_Button_Click(id,fun)		case id:fun();return true;break;
#define Stock_Control_SetVisible(id,b)	{CGuiControl* p = GetControl(id);if(p) p->SetVisible(b);}
#define Stock_Return_If_Not_Active		if(!GetStockAccount()->GetActive()) return;
#define Stock_Static_SetText(id,txt)	{CGuiStatic* p = GetStatic(id);if(p) p->SetText(txt);}
#define Stock_Static_SetNum(id,num)		{char sz[32];_snprintf(sz,32,"%d",num);sz[31]=0;Stock_Static_SetText(id,sz);}
#define Stock_EditBox_SetText(id,txt)	{CGuiEditBox* p = GetEditBox(id);if(p) p->SetText(txt);}


inline const char* StockTime2String(DWORD time)
{
	static char szTime[128];
	struct tm *t;
	__int64 ltime(time*60);
	t = _gmtime64( &ltime );
	int year = t->tm_year - 100;
	_snprintf(szTime,128,"%02d/%d/%d",year,t->tm_mon+1,t->tm_mday);
	szTime[127] = 0;
	return szTime;
}



CGuiStockDialog::CGuiStockDialog(void)
{
	m_pListBox_Buy  = NULL;
	m_pListBox_Sell = NULL;
}

CGuiStockDialog::~CGuiStockDialog(void)
{
}

void CGuiStockDialog::OnCreate()
{
	CGuiDialog::OnCreate();

	UpdateAccountManagerButtonState();
	m_pListBox_Buy = GetListBox(6);
	m_pListBox_Sell = GetListBox(2);

	m_pHistoryList	= GetListBox(501);
	m_pGoldList		= GetListBox(701);
	m_pMoneyList	= GetListBox(601);

	AssertCreateCtrl( m_pListBox_Buy,"股票脚本出错" );
	AssertCreateCtrl( m_pListBox_Sell,"股票脚本出错" );
	AssertCreateCtrl( m_pHistoryList,"股票脚本出错" );
	AssertCreateCtrl( m_pGoldList,"股票脚本出错" );
	AssertCreateCtrl( m_pMoneyList,"股票脚本出错" );

	{//请求股票账户状态
		stRequestStateStockUserCmd cmd;
		SEND_USER_CMD(cmd);
	}
	{//请求前10位大盘信息		
		stRequestFirstTenListStockUserCmd cmd;
		SEND_USER_CMD(cmd);
	}
	Stock_Control_SetVisible(1,false);
	Stock_Control_SetVisible(101,false);
	Stock_Control_SetVisible(102,false);
	Stock_Control_SetVisible(103,false);
	Stock_Control_SetVisible(104,false);
	Stock_Control_SetVisible(105,false);
	Stock_Control_SetVisible(106,false);
	Stock_Control_SetVisible(107,false);
	Stock_Control_SetVisible(207,false);
	GetTab(1)->SetCurItem(7);
}

void CGuiStockDialog::OnClose()
{
	GetGameGuiManager()->m_guiStockDlg = NULL;
}

bool CGuiStockDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	if( nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
			Stock_Button_Click(3,Close);
			Stock_Button_Click(10,OnClickActiveCount);
			Stock_Button_Click(11,OnClickManageCount);
			Stock_Button_Click(12,OnClickDelegate);
			Stock_Button_Click(13,OnClickCancelBill);
			Stock_Button_Click(14,OnClickQuery);
			Stock_Button_Click(15,OnClickPopOutFinance);
			Stock_Button_Click(16,OnClickPushInFinance);
			Stock_Button_Click(17,OnClickBuyGold);
			Stock_Button_Click(18,OnClickSaleGold);
			Stock_Button_Click(19,OnClickBuyQuery);
			Stock_Button_Click(20,OnClickSaleQuery);
			Stock_Button_Click(107,OnPopoutOK);
			Stock_Button_Click(207,OnPushInOK);
			Stock_Button_Click(305,OnConsignBuyOK);
			Stock_Button_Click(405,OnConsignSaleOK);
			Stock_Button_Click(502,OnCancelBill);
		default:Debug_Int(nID);break;
		}
	}

	if( nEvent == EVENT_EDITBOX_CHANGE )
	{
		if( nID == 301 || nID == 302 )
		{
			//
			const char* pszGold  = GetEditBox(301)->GetText();
			int gold  = atoi(pszGold);
			const char* pszPrice = GetEditBox(302)->GetText();
			int price = atoi(pszPrice);
			int money = gold * price * 100;
			int tax = money * 0.02f;

			const char* pszMoney = Money2String(money);
			Stock_Static_SetText(303,pszMoney);
			const char* pszTax = Money2String(tax);
			Stock_Static_SetText(304,pszTax);
		}

		if( nID == 401 || nID == 402 )
		{
			//
			const char* pszGold  = GetEditBox(401)->GetText();
			int gold  = atoi(pszGold);
			const char* pszPrice = GetEditBox(402)->GetText();
			int price = atoi(pszPrice);
			int money = gold * price * 100;
			int tax = gold * 2;
			const char* pszMoney = Money2String(money);
			Stock_Static_SetText(403,pszMoney);
			const char* pszTax = Money2String(tax);
			Stock_Static_SetText(404,pszTax);
		}
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}

void CGuiStockDialog::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	FUNCTION_END;
}

HRESULT CGuiStockDialog::OnRender(float fElapsedTime)
{
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	static float time = 0.0f;
	time += fElapsedTime;
	if (time>5.0f)
	{
		time = 0.0f;
		stRequestFirstTenListStockUserCmd cmd2;
		SEND_USER_CMD(cmd2);
	}
	
	return hr;
}

void CGuiStockDialog::OnClickActiveCount()		//账户激活
{
	//登陆股票账户		
	stPassowrdStockUserCmd  cmd;
	strncpy(cmd.byNew1,"111111",sizeof(cmd.byPawword));
	strncpy(cmd.byNew2,"111111",sizeof(cmd.byPawword));
	SEND_USER_CMD(cmd);	
}

void CGuiStockDialog::OnClickManageCount()		//账户管理
{
	OnClickPushInFinance();
}

void CGuiStockDialog::OnClickDelegate()			//委托
{
	Stock_Return_If_Not_Active;
	OnClickBuyGold();
}

void CGuiStockDialog::OnClickQuery()			//查询
{
	Stock_Return_If_Not_Active;

	//请求自己的交易信息
	stRequstSelfHistoryStockUserCmd  cmd;
	cmd.begintime = 0;
	cmd.num = 10;
	SEND_USER_CMD(cmd);

	OnClickBuyQuery();
}

void CGuiStockDialog::OnClickPopOutFinance()	//取出资金
{
	GetTab(1)->SetCurItem(0);
	GetButton(15)->SetChecked(true);
	GetButton(16)->SetChecked(false);
	SetBackImage(stResourceLocation("data\\interfaces.gl",66,10));
}

void CGuiStockDialog::OnClickPushInFinance()	//注入资金
{
	GetTab(1)->SetCurItem(1);
	SetBackImage(stResourceLocation("data\\interfaces.gl",66,0));
	GetButton(15)->SetChecked(false);
	GetButton(16)->SetChecked(true);
}

void CGuiStockDialog::OnClickBuyGold()			//买入金子
{
	GetTab(1)->SetCurItem(2);
	GetButton(17)->SetChecked(true);
	GetButton(18)->SetChecked(false);
	SetBackImage(stResourceLocation("data\\interfaces.gl",67,1));
}

void CGuiStockDialog::OnClickSaleGold()			//卖出金子
{
	GetTab(1)->SetCurItem(3);
	GetButton(17)->SetChecked(false);
	GetButton(18)->SetChecked(true);
	SetBackImage(stResourceLocation("data\\interfaces.gl",67,0));
}

void CGuiStockDialog::OnClickCancelBill()		//撤销单子
{
	Stock_Return_If_Not_Active;
	GetTab(1)->SetCurItem(4);
	SetBackImage(stResourceLocation("data\\interfaces.gl",68,0));

	//发送未成交金子委托单列表请求
	stRequestConsignListStockUserCmd cmd;
	SEND_USER_CMD(cmd);
}

void CGuiStockDialog::OnClickBuyQuery()			//买入查询
{
	GetTab(1)->SetCurItem(5);
	GetButton(19)->SetChecked(true);
	GetButton(20)->SetChecked(false);
	SetBackImage(stResourceLocation("data\\interfaces.gl",69,0));
}

void CGuiStockDialog::OnClickSaleQuery()		//卖出查询
{
	GetTab(1)->SetCurItem(6);
	GetButton(19)->SetChecked(false);
	GetButton(20)->SetChecked(true);
	SetBackImage(stResourceLocation("data\\interfaces.gl",69,1));
}

void CGuiStockDialog::OnPushInOK()			//注入资金确定
{
	int gold  = GetMoneyNum(201,202,203);
	int money = GetMoneyNum(204,205,206);

	if( gold > 0 || money > 0 )
	{
		stTransferFundStockSaveUserCmd cmd;
		cmd.dwGold = gold;
		cmd.dwMoney = money;
		SEND_USER_CMD(cmd);
	}
	Stock_EditBox_SetText(201,"");
	Stock_EditBox_SetText(202,"");
	Stock_EditBox_SetText(203,"");
	Stock_EditBox_SetText(204,"");
	Stock_EditBox_SetText(205,"");
	Stock_EditBox_SetText(206,"");
}
void CGuiStockDialog::OnPopoutOK()			//取出资金确定
{
	int gold  = GetMoneyNum(101,102,103);
	int money = GetMoneyNum(104,105,106);

	if( gold > 0 || money > 0 )
	{
		stTransferFundStockFetchUserCmd cmd;
		cmd.dwGold = gold;
		cmd.dwMoney = money;
		SEND_USER_CMD(cmd);
		Stock_EditBox_SetText(101,"");
		Stock_EditBox_SetText(102,"");
		Stock_EditBox_SetText(103,"");
		Stock_EditBox_SetText(104,"");
		Stock_EditBox_SetText(105,"");
		Stock_EditBox_SetText(106,"");
	}
}

void CGuiStockDialog::OnConsignBuyOK()			//委托买进确定
{
	const char* pszGold  = GetEditBox(301)->GetText();
	int gold  = atoi(pszGold);
	const char* pszPrice = GetEditBox(302)->GetText();
	int price = atoi(pszPrice);
	int money = gold*100 * price;
	DWORD tax = String2Money(GetStatic(304)->GetText());
	if( money+tax > GetStockAccount()->GetMoney() )
	{
		GameMessageBox("花费的银子不能大于帐户现有银子数");
		return ;
	}
	stPutListStockUserCmd cmd;
	cmd.dwNum = money;
	cmd.dwPrice = price;
	cmd.byType = STOCK_MONEY;

	SEND_USER_CMD(cmd);
	Stock_EditBox_SetText(301,"");
	Stock_EditBox_SetText(302,"");
	Stock_Static_SetText(303,"");
	Stock_Static_SetText(304,"");
}

void CGuiStockDialog::OnConsignSaleOK()			//委托卖出确定
{
	const char* pszGold  = GetEditBox(401)->GetText();
	int gold  = atoi(pszGold);
	const char* pszPrice = GetEditBox(402)->GetText();
	int price = atoi(pszPrice);
	DWORD tax = String2Money(GetStatic(404)->GetText());
	if( gold*100+tax > GetStockAccount()->GetGold() )
	{
		GameMessageBox("卖出的金子不能大于帐户现有金子数");
		return ;
	}
	stPutListStockUserCmd cmd;
	cmd.dwNum = gold*100;
	cmd.dwPrice = price;
	cmd.byType = STOCK_GOLD;

	SEND_USER_CMD(cmd);

	Stock_EditBox_SetText(401,"");
	Stock_EditBox_SetText(402,"");
	Stock_Static_SetText(403,"");
	Stock_Static_SetText(404,"");
}

void CGuiStockDialog::OnCancelBill()			//撤销
{
	if( m_pHistoryList == NULL )
	{
		return ;
	}
	int cur = m_pHistoryList->GetCurItem();
	if( cur != -1 )
	{
		DWORD id = (DWORD)(m_pHistoryList->GetItemData(cur));
		tListStockListIt it = FindConsignHistoryItByID(id);
		if( it != m_HistoryList.end() )
		{
			if( it->type == STOCK_GOLD )
			{
				stConsignCancelGoldStockUserCmd cmd;
				cmd.dwNum = id;
				SEND_USER_CMD(cmd);
			}
			if( it->type == STOCK_MONEY )
			{
				stConsignCancelMoneyStockUserCmd cmd;
				cmd.dwNum = id;
				SEND_USER_CMD(cmd);
			}
			m_pHistoryList->SetCurItem(-1);
		}
	}	
}

void CGuiStockDialog::CancelStock(DWORD id,StockType type)		//撤销委托单
{
	tListStockListIt it = FindConsignHistoryItByID(id);
	if( it != m_HistoryList.end() && it->type == type )
	{
		m_HistoryList.erase( it );
	}
	UpdateHistoryBillList();
}

void CGuiStockDialog::AddConsignBill(const ConsignSelfHistoryType& csht,StockType  type)		//添加委托单
{
	if( type == STOCK_GOLD )
	{
		for(tListConsignSelfHistoryIt it = m_goldList.begin(); it != m_goldList.end(); ++it )
		{
			if( it->dwID == csht.dwID )
			{
				it->wdNum = csht.wdNum;
				it->wdCommitPrice = csht.wdCommitPrice;
				it->wdPrice = csht.wdPrice;
				it->dwCommitTime = csht.dwCommitTime;
				it->dwOkTime = csht.dwOkTime;
				return ;
			}			
		}
		m_goldList.push_back(csht);
	}
	if( type == STOCK_MONEY )
	{
		for(tListConsignSelfHistoryIt it = m_moneyList.begin(); it != m_moneyList.end(); ++it )
		{
			if( it->dwID == csht.dwID )
			{
				it->wdNum = csht.wdNum;
				it->wdCommitPrice = csht.wdCommitPrice;
				it->wdPrice = csht.wdPrice;
				it->dwCommitTime = csht.dwCommitTime;
				it->dwOkTime = csht.dwOkTime;
				return ;
			}			
		}
		m_moneyList.push_back(csht);
	}
}

void CGuiStockDialog::CleanConsignBill()
{
	m_HistoryList.clear();
	m_pHistoryList->RemoveAllItems();
}

void CGuiStockDialog::UpdateConsignMoneyBillList()
{
	m_pMoneyList->RemoveAllItems();
	int index = 0;
	for(tListConsignSelfHistoryIt it = m_moneyList.begin(); it != m_moneyList.end(); ++it,index++ )
	{
		//TRACE("m_moneyList ConsignSelfHistoryType:[dwID:%d][wdNum:%d][wdCommitPrice:%d][wdPrice:%d][dwCommitTime:%d][dwOkTime:%d]\n",it->dwID,it->wdNum,it->wdCommitPrice,it->wdPrice,it->dwCommitTime,it->dwOkTime);
		char szTemp[32];
		m_pMoneyList->AddItem("",(void*)(it->dwID));
		itoa(index+1,szTemp,10);
		m_pMoneyList->SetItemText(index,0,szTemp,5,GuiListBoxColFmt_Center);
		m_pMoneyList->SetItemText(index,1,"买",7,GuiListBoxColFmt_Center);		
		_snprintf(szTemp,32,"%d手",it->wdNum/it->wdPrice/100);
		szTemp[31]=0;
		m_pMoneyList->SetItemText(index,2,szTemp,11,GuiListBoxColFmt_Center); //数量
		_snprintf(szTemp,32,"%d两",it->wdPrice);
		szTemp[31]=0;
		m_pMoneyList->SetItemText(index,3,szTemp,9,GuiListBoxColFmt_Center); //单价
		_snprintf(szTemp,32,"%d两",it->wdNum/100);
		szTemp[31]=0;
		m_pMoneyList->SetItemText(index,4,szTemp,10,GuiListBoxColFmt_Center); //总价		

		const char* pszTime = StockTime2String(it->dwOkTime);
		m_pMoneyList->SetItemText(index,5,pszTime,9,GuiListBoxColFmt_Center); //成交时间
	}
}

void CGuiStockDialog::UpdateConsignGoldBillList()
{
	m_pGoldList->RemoveAllItems();
	int index = 0;
	for(tListConsignSelfHistoryIt it = m_goldList.begin(); it != m_goldList.end(); ++it,index++ )
	{
		//TRACE("m_goldList ConsignSelfHistoryType:[dwID:%d][wdNum:%d][wdCommitPrice:%d][wdPrice:%d][dwCommitTime:%d][dwOkTime:%d]\n",it->dwID,it->wdNum,it->wdCommitPrice,it->wdPrice,it->dwCommitTime,it->dwOkTime);
		char szTemp[32];
		m_pGoldList->AddItem("",(void*)(it->dwID));
		itoa(index+1,szTemp,10);
		m_pGoldList->SetItemText(index,0,szTemp,5,GuiListBoxColFmt_Center);
		m_pGoldList->SetItemText(index,1,"卖",7,GuiListBoxColFmt_Center);	
		_snprintf(szTemp,32,"%d手",it->wdNum/100);
		szTemp[31]=0;
		m_pGoldList->SetItemText(index,2,szTemp,11,GuiListBoxColFmt_Center); //数量
		_snprintf(szTemp,32,"%d两",it->wdPrice);
		szTemp[31]=0;
		m_pGoldList->SetItemText(index,3,szTemp,9,GuiListBoxColFmt_Center); //单价
		_snprintf(szTemp,32,"%d两",it->wdPrice*it->wdNum/100);
		szTemp[31]=0;
		m_pGoldList->SetItemText(index,4,szTemp,10,GuiListBoxColFmt_Center); //总价	

		const char* pszTime = StockTime2String(it->dwOkTime);
		m_pGoldList->SetItemText(index,5,pszTime,9,GuiListBoxColFmt_Center); //成交时间
	}
}

void CGuiStockDialog::AddHistoryBill(const StockList& stock,StockType  type)		//添加历史记录委托单
{
	t_StockList sl(stock);
	sl.type = type;
	m_HistoryList.push_back(sl);
}

void CGuiStockDialog::UpdateHistoryBillList()		//刷新个人历史委托单
{
	m_pHistoryList->RemoveAllItems();
	int index = 0;
	for(tListStockListIt it = m_HistoryList.begin(); it != m_HistoryList.end(); ++it,index++ )
	{
		//TRACE("t_StockList:[id:%d][dwNum:%d][dwPrice:%d][dwTime:%d][type:%s]\n",it->id,it->dwNum,it->dwPrice,it->dwTime,(it->type==STOCK_MONEY)?"STOCK_MONEY":"STOCK_GOLD");
		char szTemp[64];
		m_pHistoryList->AddItem("",(void*)(it->id));
		itoa(index+1,szTemp,10);
		m_pHistoryList->SetItemText(index,0,szTemp,5,GuiListBoxColFmt_Center);
		if( it->type == STOCK_MONEY )
		{
			m_pHistoryList->SetItemText(index,1,"买",7,GuiListBoxColFmt_Center);
		}
		else
		{
			m_pHistoryList->SetItemText(index,1,"卖",7,GuiListBoxColFmt_Center);
		}
		_snprintf(szTemp,64,"%d两",it->dwPrice);
		szTemp[63]=0;
		m_pHistoryList->SetItemText(index,2,szTemp,15,GuiListBoxColFmt_Center); //单价

		if( it->type == STOCK_GOLD )
		{
			_snprintf(szTemp,64,"%d手",it->dwNum/100);
		}
		if( it->type == STOCK_MONEY )
		{
			_snprintf(szTemp,64,"%d手",it->dwNum/it->dwPrice/100);
		}
		szTemp[63]=0;
		m_pHistoryList->SetItemText(index,3,szTemp,15,GuiListBoxColFmt_Center); //挂单数

		const char* pszTime = StockTime2String(it->dwTime);
		m_pHistoryList->SetItemText(index,4,pszTime,11,GuiListBoxColFmt_Center); //挂单时间
	}
}

void CGuiStockDialog::UpdateAccountManagerButtonState()
{
	bool bActive = GetStockAccount()->GetActive();
	Debug_Bool(bActive);
	Stock_Control_SetVisible(10,!bActive);
	Stock_Control_SetVisible(11,bActive);
	Stock_Control_SetVisible(801,!bActive);
	if( bActive )
	{
		//发送登陆请求
		stPassowrdStockUserCmd  cmd;
		strncpy(cmd.byPawword,"111111",sizeof(cmd.byPawword));
		SEND_USER_CMD(cmd);
		
		//请求股票帐号余额		
		stQueryFundStockUserCmd cmd1;
		SEND_USER_CMD(cmd1);

		OnClickPushInFinance();
	}
}

void CGuiStockDialog::UpdateAccountFinance()
{
	DWORD money = GetStockAccount()->GetMoney();
	DWORD gold  = GetStockAccount()->GetGold();

	const char* pszGold = Money2String(gold);
	Stock_Static_SetText(4,pszGold);

	const char* pszMoney = Money2String(money);
	Stock_Static_SetText(5,pszMoney);
}

CGuiStockDialog::tListStockListIt CGuiStockDialog::FindConsignHistoryItByID(DWORD id)
{
	for(tListStockListIt it = m_HistoryList.begin(); it != m_HistoryList.end(); ++it )
	{
		if( it->id == id )
		{
			return it;
		}
	}
	return m_HistoryList.end();
}

int  CGuiStockDialog::GetMoneyNum(const char* pIngot,const char* pTeal,const char* pCent)
{	
	char szTxt[32]={0};
	sprintf(szTxt,"%d%s%d%s%d%s",strtoul(pIngot,NULL,10),INGOT_TXT,
		strtoul(pTeal,NULL,10),TAEL_TXT,
		strtoul(pCent,NULL,10),CENT_TXT);

	int nMoney= String2Money(szTxt);

	return nMoney;
}

int  CGuiStockDialog::GetMoneyNum(int IngotId,int TealId,int CentId)
{
	FUNCTION_BEGIN;
	return GetMoneyNum( GetEditBox(IngotId)->GetText(),GetEditBox(TealId)->GetText(),GetEditBox(CentId)->GetText() );
	FUNCTION_END;
}

void CGuiStockDialog::ParseConsignBillCmd(DWORD size,StockList* lst,StockType t)
{
	for ( int i=0; i < size; i++ )
	{
		GetGameGuiManager()->m_guiStockDlg->AddHistoryBill(lst[i],t);
	}
	GetGameGuiManager()->m_guiStockDlg->UpdateHistoryBillList();
}

void CGuiStockDialog::ParseSelfHistoryCmd(DWORD size,ConsignSelfHistoryType* lst,StockType t)
{
	ClearSelfHistory(t);
	for (int i=0;i<size;i++)
	{
		AddConsignBill(lst[i],t);
	}
	if( t == STOCK_GOLD )
	{
		UpdateConsignGoldBillList();
	}
	if( t == STOCK_MONEY )
	{
		UpdateConsignMoneyBillList();
	}
}

void CGuiStockDialog::ClearSelfHistory(StockType type)
{
	if( type == STOCK_GOLD )
	{
		m_goldList.clear();
	}
	if( type == STOCK_MONEY )
	{
		m_moneyList.clear();
	}
}

void CGuiStockDialog::ParseFirst10Cmd(stFirstFiveListStockUserCmd* pCmd)
{
	static DWORD buy = 0;
	static DWORD sell = 0;
	static POINT pt = m_pListBox_Sell->GetLocation();
	char szTxt[64]={0};

	switch(pCmd->byType) {
	case STOCK_GOLD:  //卖
		{
			m_pListBox_Sell->RemoveAllItems();
			m_pListBox_Sell->SetLocation(pt.x,pt.y/*+(5-pCmd->size)*16*/);
			for (int i=5;i>pCmd->size;i--)
			{
				sprintf(szTxt,"卖%d",i);
				m_pListBox_Sell->AddItem("",NULL);
				m_pListBox_Sell->SetItemText(m_pListBox_Sell->GetItemCount()-1,0,szTxt,5);
				m_pListBox_Sell->SetItemText(m_pListBox_Sell->GetItemCount()-1,1,"——",19);
				m_pListBox_Sell->SetItemText(m_pListBox_Sell->GetItemCount()-1,2,"——");

				m_pListBox_Sell->SetItemColor(m_pListBox_Sell->GetItemCount()-1,1,COLOR_ARGB(255,0,255,0));
				m_pListBox_Sell->SetItemColor(m_pListBox_Sell->GetItemCount()-1,2,COLOR_ARGB(255,237,224,19));

			}
			for (int i=pCmd->size-1;i>=0;i--)
			{
				sprintf(szTxt,"卖%d",i+1);
				m_pListBox_Sell->AddItem("",NULL);
				m_pListBox_Sell->SetItemText(m_pListBox_Sell->GetItemCount()-1,0,szTxt,5);
				sprintf(szTxt,"%s",Money2String(pCmd->list[i].dwPrice*100));
				m_pListBox_Sell->SetItemText(m_pListBox_Sell->GetItemCount()-1,1,szTxt,19);
				sprintf(szTxt,"%d手",pCmd->list[i].dwNum/100);
				m_pListBox_Sell->SetItemText(m_pListBox_Sell->GetItemCount()-1,2,szTxt);

				m_pListBox_Sell->SetItemColor(m_pListBox_Sell->GetItemCount()-1,1,COLOR_ARGB(255,0,255,0));
				m_pListBox_Sell->SetItemColor(m_pListBox_Sell->GetItemCount()-1,2,COLOR_ARGB(255,237,224,19));

			}

			sell = pCmd->list[0].dwPrice;
		}
		break;
	case STOCK_MONEY:  //买
		{
			m_pListBox_Buy->RemoveAllItems();
			for (int i=0;i<pCmd->size;i++)
			{
				sprintf(szTxt,"买%d",i+1);
				m_pListBox_Buy->AddItem("",NULL);
				m_pListBox_Buy->SetItemText(m_pListBox_Buy->GetItemCount()-1,0,szTxt,5);
				sprintf(szTxt,"%s",Money2String(pCmd->list[i].dwPrice*100));
				m_pListBox_Buy->SetItemText(m_pListBox_Buy->GetItemCount()-1,1,szTxt,19);
				sprintf(szTxt,"%d手",pCmd->list[i].dwNum/pCmd->list[i].dwPrice/100);
				m_pListBox_Buy->SetItemText(m_pListBox_Buy->GetItemCount()-1,2,szTxt);

				m_pListBox_Buy->SetItemColor(m_pListBox_Buy->GetItemCount()-1,1,COLOR_ARGB(255,0,255,0));
				m_pListBox_Buy->SetItemColor(m_pListBox_Buy->GetItemCount()-1,2,COLOR_ARGB(255,237,224,19));
			}

			for (int i=pCmd->size;i<5;i++)
			{
				sprintf(szTxt,"买%d",i+1);
				m_pListBox_Buy->AddItem("",NULL);
				m_pListBox_Buy->SetItemText(m_pListBox_Buy->GetItemCount()-1,0,szTxt,5);
				m_pListBox_Buy->SetItemText(m_pListBox_Buy->GetItemCount()-1,1,"——",19);
				m_pListBox_Buy->SetItemText(m_pListBox_Buy->GetItemCount()-1,2,"——");

				m_pListBox_Buy->SetItemColor(m_pListBox_Buy->GetItemCount()-1,1,COLOR_ARGB(255,0,255,0));
				m_pListBox_Buy->SetItemColor(m_pListBox_Buy->GetItemCount()-1,2,COLOR_ARGB(255,237,224,19));
			}

			buy = pCmd->list[0].dwPrice;
		}
		break;
	}

	sprintf(szTxt,"买入%d    卖出%d",buy,sell);
	//GetStatic(7)->SetText(szTxt);
}

//股票账户状态信息
CStockAccount::CStockAccount()
{
	m_bActive = false;
	m_bLogin  = false;
	m_dwMoney = 0;
	m_dwGold  = 0;
}

void CStockAccount::SetActive(bool bActive)
{
	m_bActive = bActive;
	if( GetGameGuiManager()->m_guiStockDlg )
	{
		GetGameGuiManager()->m_guiStockDlg->UpdateAccountManagerButtonState();
	}
}

void CStockAccount::SetLogin(bool bLogin)
{
	m_bLogin = bLogin;
}

void CStockAccount::SetCountFinance(DWORD dwMoney,DWORD dwGold)
{
	m_dwMoney = dwMoney;
	m_dwGold  = dwGold;
	if( GetGameGuiManager()->m_guiStockDlg )
	{
		GetGameGuiManager()->m_guiStockDlg->UpdateAccountFinance();
	}
}