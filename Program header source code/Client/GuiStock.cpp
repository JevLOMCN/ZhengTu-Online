#include "./public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GuiItem.h"
#include "./RoleItem.h"
#include ".\guistock.h"


#define   OFFSETX         50
#define   OFFSETY         50
#define   PICWIDTH        450
#define   PICHEIGHT       250

CStockManager* GetStockManager()
{
	static CStockManager stock;
	return &stock;
}

CGuiStock::CGuiStock(void)
{
	//m_bSmDialog = true;

	m_bCloseIsHide = true;
}

CGuiStock::~CGuiStock(void)
{
}

void CGuiStock::UpData()
{
	POINTLIST    ptList;

	m_lineList.clear();

	for (int i=0;i<m_historyDataList.size();i++)
	{
		ptList.clear();

		
	}
}

void CGuiStock::OnDraw()
{
    POINT pt[]=
	{{m_x+OFFSETX,m_y+OFFSETY},{m_x+OFFSETX+PICWIDTH,m_y+OFFSETY} ,
	{m_x+OFFSETX+PICWIDTH,m_y+OFFSETY+PICHEIGHT},
	{m_x+OFFSETX,m_y+OFFSETY+PICHEIGHT},{m_x+OFFSETX,m_y+OFFSETY}};

	GetDevice()->DrawPolyLine(pt,5,0xffffffff);


}

/**
* \brief 设置关闭按钮
* 
* 设置关闭按钮
* 
* \return 无
*/
void CGuiStock::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();


	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());

		m_pBtnClose->SetLocation(pt.x,pt.y+24);
	}


	FUNCTION_END;
}

void CGuiStock::OnCreate()
{
	CGuiDialog::OnCreate();

	m_pListBox_Buy = GetListBox(6);
	m_pListBox_Sell = GetListBox(5);
	m_pListBox_Buy->SetEnabled(false);
	m_pListBox_Sell->SetEnabled(false);

	GetButton(10)->SetEnabled(false);

	//stRequstHistoryStockUserCmd cmd;
	//cmd.begintime = 0;
	//cmd.num = 240;
	//SEND_USER_CMD(cmd);


	stRequestFirstTenListStockUserCmd cmd2;
	SEND_USER_CMD(cmd2);

	stRequestStateStockUserCmd  cmd3;
	SEND_USER_CMD(cmd3);

	InitData();

	if (g_bSinglePlayer)
	{
		{
			stRequestStateStockUserCmd  cmd3;
			cmd3.active = true;
			cmd3.login = true;
			GetClient()->PushCommand(&cmd3,sizeof(stRequestStateStockUserCmd));
		}
		{
		
		stFirstFiveListStockUserCmd* pCmd;
		pCmd=(stFirstFiveListStockUserCmd*)malloc(sizeof(stFirstFiveListStockUserCmd)+1024);

		memset(pCmd,0,sizeof(stFirstFiveListStockUserCmd)+1024);
		pCmd->byCmd=STOCK_BILL_USERCMD;
		pCmd->byParam=FIRSTFIVE_LIST_STOCKPARA;
		pCmd->size=3;
		pCmd->byType=STOCK_GOLD;

		pCmd->list[0].dwNum = 1000000;
		pCmd->list[0].dwPrice = 100;

		pCmd->list[1].dwNum = 10000;
		pCmd->list[1].dwPrice = 200;

		pCmd->list[2].dwNum = 10000;
		pCmd->list[2].dwPrice = 200;

		/*pCmd->list[3].dwNum = 10000;
		pCmd->list[3].dwPrice = 200;

		pCmd->list[4].dwNum = 10000;
		pCmd->list[4].dwPrice = 200;*/
		

		GetClient()->PushCommand(pCmd,sizeof(stFirstFiveListStockUserCmd)+1024);
		}

	
		{
		stFirstFiveListStockUserCmd* pCmd;
		pCmd=(stFirstFiveListStockUserCmd*)malloc(sizeof(stFirstFiveListStockUserCmd)+1024);

		memset(pCmd,0,sizeof(stFirstFiveListStockUserCmd)+1024);
		pCmd->byCmd=STOCK_BILL_USERCMD;
		pCmd->byParam=FIRSTFIVE_LIST_STOCKPARA;
		pCmd->size=3;
		pCmd->byType=STOCK_MONEY;

		pCmd->list[0].dwNum = 1000000;
		pCmd->list[0].dwPrice = 100;

		pCmd->list[1].dwNum = 10000;
		pCmd->list[1].dwPrice = 200;

		pCmd->list[2].dwNum = 10000;
		pCmd->list[2].dwPrice = 200;

		/*pCmd->list[3].dwNum = 10000;
		pCmd->list[3].dwPrice = 200;

		pCmd->list[4].dwNum = 10000;
		pCmd->list[4].dwPrice = 200;*/


		GetClient()->PushCommand(pCmd,sizeof(stFirstFiveListStockUserCmd)+1024);
		}
	}
}

void CGuiStock::SetFirstFiveList(stFirstFiveListStockUserCmd* pCmd)
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
				sprintf(szTxt,"%s",Money2String(pCmd->list[i].dwNum));
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
				sprintf(szTxt,"%s",Money2String(pCmd->list[i].dwNum/pCmd->list[i].dwPrice));
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

void CGuiStock::InitData()
{
	m_pListBox_Buy->RemoveAllItems();
	m_pListBox_Sell->RemoveAllItems();
	//GetStatic(7)->SetText("");

	m_historyDataList.clear();
	m_lineList.clear();
}

void CGuiStock::OnClose()
{
	GetStockManager()->m_pStock = NULL;
}
bool CGuiStock::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent) {
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) {
			case 1:
				GetStockManager()->AddStockOut();
				break;
			case 2:
				GetStockManager()->AddStockConsign();
				break;
			case 3:
				GetStockManager()->AddStockCanel();
				break;
			case 4:
				GetStockManager()->AddStockQuery();
				break;
			case 9:
				Close();
				break;
			}
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}
HRESULT CGuiStock::OnRender(float fElapsedTime)
{
	static float time = 0.0f;

	time += fElapsedTime;
	if (time>5.0f)
	{
		time = 0.0f;

		stRequestFirstTenListStockUserCmd cmd2;
		SEND_USER_CMD(cmd2);
	}

	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);


	return hr;
}


CGuiStockAccount::CGuiStockAccount(void)
{
	m_bSmDialog = true;
}

CGuiStockAccount::~CGuiStockAccount(void)
{

}

void CGuiStockAccount::Refresh()
{
	if (GetStatic(5))
		GetStatic(5)->SetText(Money2String(GetStockManager()->GetGold()));

	if (GetStatic(6))
		GetStatic(6)->SetText(Money2String(GetStockManager()->GetMoney()));
}


void CGuiStockAccount::OnCreate()
{
	CGuiDialog::OnCreate();

	GetButton(1)->SetButtonGroup(1);
	GetButton(2)->SetButtonGroup(1);

	Refresh();

	if (GetEditBox(7))
	{
		GetGuiManager()->RequestFocus(this);
		this->SetFocus(GetEditBox(7));
	}

	OnGuiEvent(EVENT_BUTTON_CLICKED,1,NULL);
}

bool CGuiStockAccount::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent) {
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) {
			case 1:
				{
					SetBackImage(stResourceLocation("data\\interfaces.gl",62,50));
					GetImage(10)->SetVisible(true);
					GetImage(0)->SetVisible(false);

					GetButton(1)->SetChecked(true);
					GetEditBox(7)->SetText("0");
					GetEditBox(8)->SetText("0");
					GetEditBox(11)->SetText("0");
					GetEditBox(12)->SetText("0");
					GetEditBox(13)->SetText("0");
					GetEditBox(14)->SetText("0");
				}
				break;
			case 2:
				{	
					SetBackImage(stResourceLocation("data\\interfaces.gl",62,49));
					GetImage(0)->SetVisible(true);
					GetImage(10)->SetVisible(false);

					GetButton(2)->SetChecked(true);
					GetEditBox(7)->SetText("0");
					GetEditBox(8)->SetText("0");
					GetEditBox(11)->SetText("0");
					GetEditBox(12)->SetText("0");
					GetEditBox(13)->SetText("0");
					GetEditBox(14)->SetText("0");
				}
				break;
			case 3:
				{
					DWORD dwGold = GetStockManager()->GetMoneyNum(GetEditBox(7)->GetText(),
						GetEditBox(11)->GetText(),GetEditBox(13)->GetText());
					DWORD dwMoney = GetStockManager()->GetMoneyNum(GetEditBox(8)->GetText(),
						GetEditBox(12)->GetText(),GetEditBox(14)->GetText());

					if (GetButton(1)->GetChecked())
					{
						stTransferFundStockSaveUserCmd cmd;
						cmd.dwGold = dwGold;
						cmd.dwMoney = dwMoney;

						SEND_USER_CMD(cmd);

					}
					else
					{
						if (GetStockManager()->GetGold()<dwGold)
						{
							GameMessageBox("取出的金子不能大于帐户现有金子数");
							this->SetFocus(GetEditBox(7));
							goto end;
						}

						if (GetStockManager()->GetMoney()<dwMoney)
						{
							GameMessageBox("取出的银子不能大于帐户现有银子数");
							this->SetFocus(GetEditBox(8));
							goto end;
						}

						stTransferFundStockFetchUserCmd cmd;
						cmd.dwGold = dwGold;
						cmd.dwMoney = dwMoney;

						SEND_USER_CMD(cmd);
					}

					//Close();
					OnGuiEvent(EVENT_BUTTON_CLICKED,1,NULL);
					
				}
				break;
			case 4:
			case 9:
				Close();
				break;
			}
		}
		break;
	}

end:
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}

void CGuiStockAccount::OnClose()
{
	GetStockManager()->m_pStockAccount = NULL;
}

CGuiStockCanel::CGuiStockCanel(void)
{
	m_bSmDialog = true;
	m_bCloseIsHide = true;
}

CGuiStockCanel::~CGuiStockCanel(void)
{

}

void CGuiStockCanel::OnCreate()
{
	CGuiDialog::OnCreate();

	m_pListBox = GetListBox(4);

	ClearStockList();
}

void CGuiStockCanel::DeleteStock(DWORD id,StockType type)
{
	for (std::vector<stStockListEx>::iterator pointer=m_stockList.begin();
		pointer!=m_stockList.end();pointer++)
	{
		if ((pointer->id==id)&&(pointer->type==type))
		{
			m_stockList.erase(pointer);
			break;
		}
	}
	Refresh();
}

void CGuiStockCanel::Refresh()
{
	m_pListBox->RemoveAllItems();

	for (int i=0;i<m_stockList.size();i++)
	{
		char szTxt1[10]={0};
		char szTxt2[100]={0};
		char szTmp[64]={0};
		switch(m_stockList[i].type) {
		case STOCK_GOLD:
			strncpy(szTmp,Money2String(m_stockList[i].dwNum),sizeof(szTmp));
			sprintf(szTxt2,"卖%s金子得到%s银子",szTmp,Money2String((m_stockList[i].dwNum*m_stockList[i].dwPrice)) );
			break;
		case STOCK_MONEY:
			strncpy(szTmp,Money2String((m_stockList[i].dwNum/m_stockList[i].dwPrice)),sizeof(szTmp));
			sprintf(szTxt2,"买%s金子花费%s银子",szTmp,Money2String(m_stockList[i].dwNum));
			break;
		}
		m_pListBox->AddItem("",(void*)&(m_stockList[i]));
		m_pListBox->SetItemText(m_pListBox->GetItemCount()-1,0,_itoa(m_stockList[i].id,szTxt1,10),5);
		m_pListBox->SetItemText(m_pListBox->GetItemCount()-1,1,szTxt2);
	}
}

bool CGuiStockCanel::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent) {
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) {
			case 2:
				{
					int i=m_pListBox->GetCurItem();
					if (i>=0)
					{
						stStockListEx* pStockEx =(stStockListEx*) m_pListBox->GetItemData(i);
						switch(pStockEx->type) {
						case STOCK_GOLD:
							{
								stConsignCancelGoldStockUserCmd cmd;
								cmd.dwNum = pStockEx->id;

								SEND_USER_CMD(cmd);
							}
							break;
						case STOCK_MONEY:
							{
								stConsignCancelMoneyStockUserCmd cmd;
								cmd.dwNum = pStockEx->id;

								SEND_USER_CMD(cmd);
							}
							break;
						}
						//Close();

					}
					else
					{
						GameMessageBox("请先选中要撤消的单子！");
					}
				}
				break;
			case 3:
			case 9:
				Close();
				break;
			}
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}


void CGuiStockCanel::OnClose()
{
	GetStockManager()->m_pStockCanel = NULL;
}

CGuiStockConsign::CGuiStockConsign(void)
{
	m_bSmDialog = true;
}

CGuiStockConsign::~CGuiStockConsign(void)
{

}

void CGuiStockConsign::OnCreate()
{
	CGuiDialog::OnCreate();


	GetButton(2)->SetButtonGroup(1);
	GetButton(3)->SetButtonGroup(1);
	GetEditBox(13)->SetEnabled(false);

	Refresh();
	

	if (GetEditBox(10))
	{
		GetGuiManager()->RequestFocus(this);
		this->SetFocus(GetEditBox(10));
	}

	OnGuiEvent(EVENT_BUTTON_CLICKED,2,NULL);
}

void CGuiStockConsign::Refresh()
{
	if (GetStatic(6))
		GetStatic(6)->SetText(Money2String(GetStockManager()->GetGold()));

	if (GetStatic(7))
		GetStatic(7)->SetText(Money2String(GetStockManager()->GetMoney()));
}

bool CGuiStockConsign::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent) {
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) {
			case 2:
				{
					GetButton(2)->SetChecked(true);
					SetBackImage(stResourceLocation("data\\interfaces.gl",62,31));
					GetImage(0)->SetVisible(false);
					GetImage(1)->SetVisible(true);
					GetStatic(8)->SetText("0");
					GetStatic(9)->SetText("0");
					GetEditBox(10)->SetText("0");
					GetEditBox(11)->SetText("0");
					GetEditBox(12)->SetText("0");
					GetEditBox(13)->SetText("0");
				}
				break;
			case 3:
				{	
					GetButton(3)->SetChecked(true);
					SetBackImage(stResourceLocation("data\\interfaces.gl",62,30));
					GetImage(0)->SetVisible(true);
					GetImage(1)->SetVisible(false);
					GetStatic(8)->SetText("0");
					GetStatic(9)->SetText("0");
					GetEditBox(10)->SetText("0");
					GetEditBox(11)->SetText("0");
					GetEditBox(12)->SetText("0");
					GetEditBox(13)->SetText("0");
				}
				break;
			case 4:
				{
					DWORD dwGold = atoi(GetEditBox(10)->GetText());
					DWORD dwPrice = GetStockManager()->GetMoneyNum(GetEditBox(11)->GetText(),
						GetEditBox(12)->GetText(),GetEditBox(13)->GetText());
					DWORD dwTax = String2Money(GetStatic(9)->GetText());

					if (GetButton(2)->GetChecked())
					{
						if (GetStockManager()->GetMoney()<dwGold*dwPrice+dwTax)
						{
							GameMessageBox("花费的银子不能大于帐户现有银子数");
							this->SetFocus(GetEditBox(10));
							goto end;
						}

						stPutListStockUserCmd cmd;
						cmd.dwNum = dwGold*dwPrice;
						cmd.dwPrice = dwPrice/100;
						cmd.byType = STOCK_MONEY;

						SEND_USER_CMD(cmd);

					}
					else
					{
						if (GetStockManager()->GetGold()<dwGold+dwTax)
						{
							GameMessageBox("卖出的金子不能大于帐户现有金子数");
							this->SetFocus(GetEditBox(10));
							goto end;
						}

						stPutListStockUserCmd cmd;
						cmd.dwNum = dwGold*100;
						cmd.dwPrice = dwPrice/100;
						cmd.byType = STOCK_GOLD;

						SEND_USER_CMD(cmd);
					}

					//Close();
					OnGuiEvent(EVENT_BUTTON_CLICKED,2,NULL);

				}
				break;
			case 5:
			case 20:
				Close();
				break;
			}
		}
		break;

	case EVENT_EDITBOX_CHANGE:
		{
			switch(nID) {
			case 10:
				{
					DWORD dwGold = atoi(GetEditBox(10)->GetText());
					DWORD dwPrice = GetStockManager()->GetMoneyNum(GetEditBox(11)->GetText(),
						GetEditBox(12)->GetText(),GetEditBox(13)->GetText());

					if (GetButton(2)->GetChecked())
					{
						GetStatic(8)->SetText(Money2String(dwGold*dwPrice));
						GetStatic(9)->SetText(Money2String((int)(dwGold*dwPrice*0.02+0.99)));
					}
					else
					{
						GetStatic(8)->SetText(Money2String(dwGold*dwPrice));
						GetStatic(9)->SetText(Money2String((int)(dwGold*100*0.02+0.99)));
					}
					/*if (dwGold%100!=0)
					{
						char szTmp[64]={0};
						GetEditBox(10)->SetText(_itoa(dwGold*100,szTmp,10));
					}*/
				}
			case 11:
			case 12:
			case 13:
				{
					DWORD dwGold = atoi(GetEditBox(10)->GetText());
					DWORD dwPrice = GetStockManager()->GetMoneyNum(GetEditBox(11)->GetText(),
						GetEditBox(12)->GetText(),GetEditBox(13)->GetText());
					
					if (GetButton(2)->GetChecked())
					{
						GetStatic(8)->SetText(Money2String(dwGold*dwPrice));
						GetStatic(9)->SetText(Money2String((int)(dwGold*dwPrice*0.02+0.99)));
					}
					else
					{
						GetStatic(8)->SetText(Money2String(dwGold*dwPrice));
						GetStatic(9)->SetText(Money2String((int)(dwGold*100*0.02+0.99)));
					}
				}
				break;
			}
		}
		break;
	}

end:
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}

void CGuiStockConsign::OnClose()
{
	GetStockManager()->m_pStockConsign = NULL;
}


CGuiStockQuery::CGuiStockQuery(void)
{
	m_bSmDialog = true;

	m_bCloseIsHide = true;
}

CGuiStockQuery::~CGuiStockQuery(void)
{

}


void CGuiStockQuery::OnCreate()
{
	CGuiDialog::OnCreate();

	m_pListBox = GetListBox(4);

	ClearStockList();
}

void CGuiStockQuery::Refresh()
{
	m_pListBox->RemoveAllItems();

	for (int i=0;i<m_stockList.size();i++)
	{
		char szTxt1[10]={0};
		char szTxt2[100]={0};
		char szTxt3[32]={0};
		char szTmp[64]={0};
		switch(m_stockList[i].type) {
		case STOCK_GOLD:
			strncpy(szTmp,Money2String(m_stockList[i].wdNum),sizeof(szTmp));
			sprintf(szTxt2,"卖%s金子得到%s银子",szTmp,Money2String(m_stockList[i].wdNum*m_stockList[i].wdPrice) );
			break;
		case STOCK_MONEY:
			strncpy(szTmp,Money2String(m_stockList[i].wdNum/m_stockList[i].wdPrice),sizeof(szTmp));
			sprintf(szTxt2,"买%s金子花费%s银子",szTmp,Money2String(m_stockList[i].wdNum));
			break;
		}
		tm* chattime=NULL;
		long times=m_stockList[i].dwOkTime*60;///1000;
		chattime=localtime((const time_t*)&times);
		sprintf(szTxt3,"%d-%d-%d",chattime->tm_year+1900,chattime->tm_mon+1,chattime->tm_mday);

		m_pListBox->AddItem("",(void*)&(m_stockList[i]));
		m_pListBox->SetItemText(m_pListBox->GetItemCount()-1,0,_itoa(m_stockList[i].dwID,szTxt1,10),5);
		m_pListBox->SetItemText(m_pListBox->GetItemCount()-1,1,szTxt2,26);
		m_pListBox->SetItemText(m_pListBox->GetItemCount()-1,2,szTxt3);
	}
}

bool CGuiStockQuery::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent) {
	case EVENT_BUTTON_CLICKED:
		{
		case 1:
		case 9:
			Close();
			break;
			
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}


void CGuiStockQuery::OnClose()
{
	GetStockManager()->m_pStockQuery = NULL;
}

CGuiStockOut::CGuiStockOut(void)
{
	m_bSmDialog = true;
}

CGuiStockOut::~CGuiStockOut(void)
{

}

void CGuiStockOut::OnClose()
{
	GetStockManager()->m_pStockOut = NULL;
}
bool CGuiStockOut::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent) {
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) {
			case 1:
				GetStockManager()->AddStockAccount();
				break;
			case 2:
				GetStockManager()->AddStockAction();
				break;
			case 3:
				GetStockManager()->AddStockLogin();
				break;
			case 4:
				GetStockManager()->AddStockPwdUp();
				break;
			case 9:
				Close();
				break;
			}
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}


CGuiStockLogin::CGuiStockLogin(void)
{
	m_bSmDialog = true;
}

CGuiStockLogin::~CGuiStockLogin(void)
{

}

void CGuiStockLogin::OnCreate()
{
	CGuiDialog::OnCreate();

	if (GetStatic(3))
		GetStatic(3)->SetText(GetScene()->GetMainCharacter()->GetName());

	if (GetEditBox(4))
	{
		GetGuiManager()->RequestFocus(this);
		GetEditBox(4)->SetText("");
		this->SetFocus(GetEditBox(4));
	}
}

bool CGuiStockLogin::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent) {
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) {
			case 1:
				{
					if (GetStockManager()->CheckPwd(GetEditBox(4),"登陆密码不可小于六位！"))
					{
						stPassowrdStockUserCmd  cmd;
						strncpy(cmd.byPawword,GetEditBox(4)->GetText(),sizeof(cmd.byPawword));

						SEND_USER_CMD(cmd);
						Close();
					}
				}
				break;
			case 2:
				Close();
				break;
			}
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}


void CGuiStockLogin::OnClose()
{
	GetStockManager()->m_pStockLogin = NULL;
}

CGuiStockAction::CGuiStockAction(void)
{
	m_bSmDialog = true;
}

CGuiStockAction::~CGuiStockAction(void)
{
	
}

void CGuiStockAction::OnCreate()
{
	CGuiDialog::OnCreate();

	if (GetStatic(3))
		GetStatic(3)->SetText(GetScene()->GetMainCharacter()->GetName());

	if (GetEditBox(4))
	{
		GetGuiManager()->RequestFocus(this);
		GetEditBox(4)->SetText("");
		GetEditBox(5)->SetText("");
		this->SetFocus(GetEditBox(4));
	}
}

bool CGuiStockAction::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent) {
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) {
			case 1:
				{
					if (GetStockManager()->CheckPwd(GetEditBox(4),"密码不可小于六位！"))
					{
						if (GetStockManager()->CheckPwd(GetEditBox(5),"确认密码不可小于六位！"))
						{
							
							stPassowrdStockUserCmd  cmd;
							strncpy(cmd.byNew1,GetEditBox(4)->GetText(),sizeof(cmd.byNew1));
							strncpy(cmd.byNew2,GetEditBox(5)->GetText(),sizeof(cmd.byNew2));

							SEND_USER_CMD(cmd);
							Close();
							
						}
					}
				}
				break;
			case 2:
				Close();
				break;
			}
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}


void CGuiStockAction::OnClose()
{
	GetStockManager()->m_pStockAction = NULL;
}

CGuiStockPwdUp::CGuiStockPwdUp(void)
{
	m_bSmDialog = true;
}

CGuiStockPwdUp::~CGuiStockPwdUp(void)
{
	
}

void CGuiStockPwdUp::OnCreate()
{
	CGuiDialog::OnCreate();

	if (GetStatic(3))
		GetStatic(3)->SetText(GetScene()->GetMainCharacter()->GetName());

	if (GetEditBox(4))
	{
		GetGuiManager()->RequestFocus(this);
		GetEditBox(4)->SetText("");
		GetEditBox(5)->SetText("");
		GetEditBox(6)->SetText("");
		this->SetFocus(GetEditBox(4));
	}
}

bool CGuiStockPwdUp::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch(nEvent) {
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) {
			case 1:
				{
					if (GetStockManager()->CheckPwd(GetEditBox(4),"旧密码不可小于六位！"))
					{
						if (GetStockManager()->CheckPwd(GetEditBox(5),"新密码不可小于六位！"))
						{
							if (GetStockManager()->CheckPwd(GetEditBox(6),"确认密码不可小于六位！"))
							{
								stPassowrdStockUserCmd  cmd;
								strncpy(cmd.byPawword,GetEditBox(4)->GetText(),sizeof(cmd.byPawword));
								strncpy(cmd.byNew1,GetEditBox(5)->GetText(),sizeof(cmd.byNew1));
								strncpy(cmd.byNew2,GetEditBox(6)->GetText(),sizeof(cmd.byNew2));

								SEND_USER_CMD(cmd);
								Close();
							}
						}
					}
				}
				break;
			case 2:
				Close();
				break;
			}
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}

void CGuiStockPwdUp::OnClose()
{
	GetStockManager()->m_pStockPwdUp = NULL;
}
CStockManager::CStockManager()
:m_bActive(false)
,m_bLogin(false)
{

}

CStockManager::~CStockManager()
{

}

CGuiStock*   CStockManager::AddStock()
{
	FUNCTION_BEGIN;

	if(!m_pStock)
	{
		CGuiStock* pDlg = new CGuiStock();
		if( !pDlg ) return NULL;
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\stockDialog.xml",pDlg);
		//HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\stocktmpDialog.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_pStock = pDlg;
	}
	m_pStock->SetEnabled(true);
	m_pStock->SetVisible(true);
	
	return m_pStock;

	FUNCTION_END;
}

CGuiStockAccount*   CStockManager::AddStockAccount()
{
	FUNCTION_BEGIN;
	
	if(IsPass()==false)
	{
		return NULL;
	}

	if(!m_pStockAccount)
	{
		CGuiStockAccount* pDlg = new CGuiStockAccount();
		if( !pDlg ) return NULL;
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\stockaccountDialog.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_pStockAccount = pDlg;

		stQueryFundStockUserCmd cmd;
		SEND_USER_CMD(cmd);
	}
	m_pStockAccount->SetEnabled(true);
	m_pStockAccount->SetVisible(true);
	GetGuiManager()->RequestFocus( m_pStockAccount);

	return m_pStockAccount;

	FUNCTION_END;
}

CGuiStockCanel*   CStockManager::AddStockCanel()
{
	FUNCTION_BEGIN;

	if(IsPass()==false)
	{
		return NULL;
	}

	if(!m_pStockCanel)
	{
		CGuiStockCanel* pDlg = new CGuiStockCanel();
		if( !pDlg ) return NULL;
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\stockcanelDialog.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_pStockCanel = pDlg;

		stRequestConsignListStockUserCmd cmd;
		SEND_USER_CMD(cmd);
	}
	m_pStockCanel->SetEnabled(true);
	m_pStockCanel->SetVisible(true);
	GetGuiManager()->RequestFocus( m_pStockCanel);

	return m_pStockCanel;

	FUNCTION_END;
}

CGuiStockConsign*   CStockManager::AddStockConsign()
{
	FUNCTION_BEGIN;

	if(IsPass()==false)
	{
		return NULL;
	}

	if(!m_pStockConsign)
	{
		CGuiStockConsign* pDlg = new CGuiStockConsign();
		if( !pDlg ) return NULL;
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\stockconsignDialog.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_pStockConsign = pDlg;

		stQueryFundStockUserCmd cmd;
		SEND_USER_CMD(cmd);
	}
	m_pStockConsign->SetEnabled(true);
	m_pStockConsign->SetVisible(true);
	GetGuiManager()->RequestFocus( m_pStockConsign);

	return m_pStockConsign;

	FUNCTION_END;
}


CGuiStockQuery*   CStockManager::AddStockQuery()
{
	FUNCTION_BEGIN;

	if(IsPass()==false)
	{
		return NULL;
	}

	if(!m_pStockQuery)
	{
		CGuiStockQuery* pDlg = new CGuiStockQuery();
		if( !pDlg ) return NULL;
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\stockqueryDialog.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_pStockQuery = pDlg;

		stRequstSelfHistoryStockUserCmd  cmd;
		cmd.begintime = 0;
		cmd.num = 10;
		SEND_USER_CMD(cmd);
	}
	m_pStockQuery->SetEnabled(true);
	m_pStockQuery->SetVisible(true);
	GetGuiManager()->RequestFocus( m_pStockQuery);

	return m_pStockQuery;

	FUNCTION_END;
}

CGuiStockOut*   CStockManager::AddStockOut()
{
	FUNCTION_BEGIN;

	if(!m_pStockOut)
	{
		CGuiStockOut* pDlg = new CGuiStockOut();
		if( !pDlg ) return NULL;
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\stockoutDialog.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_pStockOut = pDlg;
	}
	m_pStockOut->SetEnabled(true);
	m_pStockOut->SetVisible(true);
	GetGuiManager()->RequestFocus( m_pStockOut);

	return m_pStockOut;

	FUNCTION_END;
}


CGuiStockLogin*   CStockManager::AddStockLogin()
{
	FUNCTION_BEGIN;

	if(m_bActive==false)
	{
		GameMessageBox("请先在帐户选项里激活帐户！");
		return NULL;
	}

	if(!m_pStockLogin)
	{
		CGuiStockLogin* pDlg = new CGuiStockLogin();
		if( !pDlg ) return NULL;
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\stockloginDialog.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_pStockLogin = pDlg;
	}
	m_pStockLogin->SetEnabled(true);
	m_pStockLogin->SetVisible(true);
	GetGuiManager()->RequestFocus( m_pStockLogin);

	return m_pStockLogin;

	FUNCTION_END;
}

CGuiStockAction*   CStockManager::AddStockAction()
{
	FUNCTION_BEGIN;

	if(m_bActive)
	{
		GameMessageBox("帐户已经激活！");
		return NULL;
	}

	if(!m_pStockAction)
	{
		CGuiStockAction* pDlg = new CGuiStockAction();
		if( !pDlg ) return NULL;
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\stockactionDialog.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_pStockAction = pDlg;
	}
	m_pStockAction->SetEnabled(true);
	m_pStockAction->SetVisible(true);
	GetGuiManager()->RequestFocus( m_pStockAction);

	return m_pStockAction;

	FUNCTION_END;
}

CGuiStockPwdUp*   CStockManager::AddStockPwdUp()
{
	FUNCTION_BEGIN;

	if(IsPass()==false)
	{
		return NULL;
	}

	if(!m_pStockPwdUp)
	{
		CGuiStockPwdUp* pDlg = new CGuiStockPwdUp();
		if( !pDlg ) return NULL;
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\stockpwdupDialog.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_pStockPwdUp = pDlg;
	}
	m_pStockPwdUp->SetEnabled(true);
	m_pStockPwdUp->SetVisible(true);
	GetGuiManager()->RequestFocus( m_pStockPwdUp);

	return m_pStockPwdUp;

	FUNCTION_END;
}

bool CStockManager::CheckPwd(CGuiEditBox* pEditBox,const char* pTitle)
{
	if (pEditBox)
	{
		const char* pStr = pEditBox->GetText();
		if (strlen(pStr)<6)
		{
			GameMessageBox(pTitle);
			GetGuiManager()->RequestFocus(pEditBox->m_pDialog);
			pEditBox->m_pDialog->SetFocus(pEditBox);
			return false;
		}
	}
	return true;
}

int  CStockManager::GetMoneyNum(const char* pIngot,const char* pTeal,const char* pCent)
{	
	char szTxt[32]={0};
	sprintf(szTxt,"%d%s%d%s%d%s",strtoul(pIngot,NULL,10),INGOT_TXT,
		strtoul(pTeal,NULL,10),TAEL_TXT,
		strtoul(pCent,NULL,10),CENT_TXT);

	int nMoney= String2Money(szTxt);

	return nMoney;
}

void CStockManager::SetAccount(DWORD dwGold,DWORD dwMoney)
{
	m_dwGold = dwGold;
	m_dwMoney = dwMoney;

	if (m_pStockConsign)
		m_pStockConsign->Refresh();

	if (m_pStockAccount)
		m_pStockAccount->Refresh();
}

bool  CStockManager::IsPass()
{
	if (m_bActive==false)
	{
		GameMessageBox("请先在帐户选项里激活帐户！");
		return false;
	}

	if (m_bLogin==false)
	{
		GameMessageBox("请在帐户选项里登陆帐户！");
		return false;
	}

	return true;
}