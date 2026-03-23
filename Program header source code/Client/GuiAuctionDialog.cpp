#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GuiAuctionDialog.h"
#include "./GameCursor.h"
#include "./RoleItem.h"
#include "./GuiMoneyInputBox.h"
#include "./GuiInputBox.h"
#include "./GuiQQPannel.h"
#include "GameScene.h"
#include "./GuiAuctionMoneyInputDialog.h"
#include "MainCharacter.h"

const static stRectI g_scPannelRect(27,54,137,480);
const static stRectI g_scMoneyRect(262,488,378,508);
const static stRectI g_scGoldRect(480,488,597,508);

const static stRectI g_scPaymentRect(65,388,187,405);
const static stRectI g_scTotalPageRect(480,23,530,45);
const static stRectI g_scCurrentPageRect(620,23,670,45);

const static stRectI g_scItemInfoRect(200,63,767,83);
const static stRectI g_scItem3InfoRect(261,63,767,83);

#define Auction_DrawStringLeft(str,rect)	{stRectI rc = rect;rc.OffsetRect( m_x,m_y );GetDevice()->DrawString(str,rc);}
#define AuctionDlg_DrawString(str,rect)		{stRectI rc = rect;rc.OffsetRect( m_x,m_y );GetDevice()->DrawString(str,rc,-1,DT_CENTER);}
#define AuctionDlg_DrawInt(n,rect)			{char szTemp[32];sprintf(szTemp,"%u",n);AuctionDlg_DrawString(szTemp,rect);}
#define AuctionDlg_DrawMoney(money,rect)	{char szTemp[64];sprintf(szTemp,"%s",Money2String(money));AuctionDlg_DrawString(szTemp,rect);}

#define AuctionDlg_GetTable(pTable,id,type)	{pTable = GetTable(id);if (pTable) {pTable->m_iTableType = type;pTable->m_iTableID = id;}}
#define AuctionDlg_EventCase(id,fun) case id: return fun();break;
#define AuctionDlg_ButtonGetCheck(id,bCheck)	{CGuiButton* pBtn = GetButton(id);if( NULL != pBtn ){ bCheck = pBtn->GetChecked();}}
#define AuctionDlg_ButtonSetCheck(id,bCheck)	{CGuiButton* pBtn = GetButton(id);if( NULL != pBtn ){ pBtn->SetChecked(bCheck);}}
#define AuctionDlg_EditSetEnable(id,bEnable)	{CGuiEditBox* pEdit = GetEditBox(id);if(NULL != pEdit){ pEdit->SetEnabled(bEnable);}}	
#define AuctionDlg_ButtonSetEnable(id,bEnable)	{CGuiButton* pBtn = GetButton(id);if( NULL != pBtn ){ pBtn->SetEnabled(bEnable);}}
#define AuctionDlg_ButtonSetEnableDelay(id,bEnable)	{CGuiButton* pBtn = GetButton(id);if( NULL != pBtn ){ pBtn->SetEnableDelay(bEnable,5500);}}
#define AuctionDlg_TestBtnEnabled(id)		{CGuiButton* pBtn = GetButton(id);if( NULL == pBtn || !(pBtn->IsEnabled()) ){ return false;}}

#define PAGE_ITEM_NUM	10

#define Time_Second2Minute(time) ((time)/60)


inline int GetPayMent(BYTE kind,DWORD dwNum,DWORD price)
{
	DWORD charge = 0;
	switch (kind%8)
	{               
	case 0:
	case 4: 
		charge = price*dwNum/10;
		break;
	case 1: 
	case 5:
		charge = price*dwNum*3/2/10;//蓝色1.5倍
		break;
	case 2: 
	case 3: 
		charge = price*dwNum*2/10;//金色2倍
		break;
	case 6:
	case 7:
		charge = price*dwNum*4/10;//绿色4倍
		break;
	default:
		return 100;
		break;
	}       
	if ( charge < 100 ) charge = 100;
	return charge;
}

inline int GetPayMent(const t_Object& obj)
{
	return GetPayMent(obj.kind,obj.dwNum,obj.price);
}

CGuiAuctionDialog::CGuiAuctionDialog(void)
{
	FUNCTION_BEGIN;
	m_pBrowsePannel = new CGuiQQPannel(this);

	//m_iBrowseCurPage = 0;
	//m_iBrowseTotalPage = 0;

	//m_iRoupCurPage = 0;
	//m_iRoupTotalPage = 0;

	//m_iAuctionCurPage = 0;
	//m_iAuctionTotalPage = 0;

	m_byBrowseType = 0;
	m_byRoupType = 0;
	m_pMoneyInputDlg = NULL;

	m_bIsWaitingDatas = false;
	m_dwLastQueryTime = 0;

	FUNCTION_END;
}

CGuiAuctionDialog::~CGuiAuctionDialog(void)
{
	FUNCTION_BEGIN;
	delete m_pBrowsePannel;

	GetGameGuiManager()->m_guiAuctionDlg = NULL;
	FUNCTION_END;
}

void CGuiAuctionDialog::OnCreate(void)
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();

	m_pTab = GetTab(2);

	AssertCreateCtrl( m_pTab,"拍卖脚本出错" );
	m_pBrowsePannel->SetRect(g_scPannelRect);
	m_pBrowsePannel->Load("datas\\auctionbrowse.xml");
	m_pBrowsePannel->AddToTab(m_pTab,0);
	m_pBrowsePannel->SetType(ePannel_Browse);

	AuctionDlg_GetTable(m_pTable[0],100,OBJECTCELLTYPE_MAIL);
	AuctionDlg_GetTable(m_pTable[1],200,OBJECTCELLTYPE_MAIL);
	AuctionDlg_GetTable(m_pTable[2],300,OBJECTCELLTYPE_MAIL);
	AuctionDlg_GetTable(m_pAuctionTable,313,OBJECTCELLTYPE_MAIL);//放拍卖物品的的格子	

	m_pListBox[0] = GetListBox(102);
	m_pListBox[1] = GetListBox(202);
	m_pListBox[2] = GetListBox(312);

	m_pImage[0] = GetImage(111);
	m_pImage[1] = GetImage(211);
	m_pImage[2] = GetImage(319);

	AssertCreateCtrl( m_pListBox[0],"拍卖脚本出错" );
	AssertCreateCtrl( m_pListBox[1],"拍卖脚本出错" );
	AssertCreateCtrl( m_pListBox[2],"拍卖脚本出错" );

	AssertCreateCtrl( m_pImage[0],"拍卖脚本出错" );
	AssertCreateCtrl( m_pImage[1],"拍卖脚本出错" );
	AssertCreateCtrl( m_pImage[2],"拍卖脚本出错" );

	m_pTab->SetCurItem(0);
	m_pTab->SetVisible(false);

	//这句话要放在pTab->SetCurItem(0)的后面，否则有些按钮会莫名其妙的显示出来
	m_pBrowsePannel->SetCurrentPannel(0);

	GetButton(314)->SetButtonGroup(1);
	GetButton(315)->SetButtonGroup(1);
	GetButton(314)->SetChecked(true);

	AuctionDlg_EditSetEnable(304,false);
	AuctionDlg_EditSetEnable(308,false);

	AuctionDlg_ButtonSetEnable( 103,false );
	AuctionDlg_ButtonSetEnable( 104,false );

	AuctionDlg_ButtonSetEnable( 203,false );
	AuctionDlg_ButtonSetEnable( 204,false );

	AuctionDlg_ButtonSetEnable( 316,false );
	AuctionDlg_ButtonSetEnable( 317,false );
#ifndef __USE_GOLD
	AuctionDlg_ButtonSetEnable( 314,false );
	AuctionDlg_ButtonSetEnable( 315,false );
#endif

	if ( NULL != m_pImage[0] )
	{
		m_pImage[0]->SetVisible(false);
	}
	if ( NULL != m_pImage[1] )
	{
		m_pImage[1]->SetVisible(false);
	}
	if ( NULL != m_pImage[2] )
	{
		m_pImage[2]->SetVisible(false);
	}

	SetTotalPage(0,0);
	SetTotalPage(0,1);
	SetTotalPage(0,2);

	OnCheckMoney();
	FUNCTION_END;
}

bool CGuiAuctionDialog::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( uMsg == WM_KEYDOWN && wParam == VK_RETURN )
	{
		int cur = m_pTab->GetCurItem();
		if( cur >= 0 && cur < 3 )
		{
			CGuiEditBox* pEdit = GetEditBox( (cur+1)*100 + 20 ); //120、220、320
			if( NULL != pEdit && pEdit->IsFocus() )
			{
				GotoPage( GetCurPage(cur),cur );
			}
		}
	}
	return CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);
}

bool CGuiAuctionDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		if ( true == m_pBrowsePannel->OnButtonClick(nID) )
		{
			CGuiButton* pBtn = GetButton(10);
			if( pBtn && pBtn->IsEnabled() )
			{
				SetCurPage(1,0);
				return OnClickSearch();
			}
			return true;
		}
		switch(nID)
		{
			AuctionDlg_EventCase(3,OnClickBrowse);
			AuctionDlg_EventCase(4,OnClickRoup);
			AuctionDlg_EventCase(5,OnClickAuction);
			AuctionDlg_EventCase(9,OnClickClose);
			AuctionDlg_EventCase(10,OnClickSearch);
			AuctionDlg_EventCase(12,OnYiKouJia);
			AuctionDlg_EventCase(101,OnClickBuyOnBrowse);
			AuctionDlg_EventCase(106,OnClickSortBrowseByNeedLevel);
			AuctionDlg_EventCase(107,OnClickSortBrowseByKind);
			AuctionDlg_EventCase(108,OnClickSortBrowseByOwner);
			AuctionDlg_EventCase(109,OnClickSortBrowseByTime);
			AuctionDlg_EventCase(110,OnClickSortBrowseByPrice);

			AuctionDlg_EventCase(201,OnClickAddPrice);
			AuctionDlg_EventCase(206,OnClickSortRoupByNeedLevel);
			AuctionDlg_EventCase(207,OnClickSortRoupByKind);
			AuctionDlg_EventCase(208,OnClickSortRoupByOwner);
			AuctionDlg_EventCase(209,OnClickSortRoupByTime);
			AuctionDlg_EventCase(210,OnClickSortRoupByPrice);

			AuctionDlg_EventCase(309,OnStartAuction);
			AuctionDlg_EventCase(310,OnStopAuction);
			AuctionDlg_EventCase(311,OnClickBuyAuction);
			AuctionDlg_EventCase(314,OnCheckMoney);
			AuctionDlg_EventCase(315,OnCheckGoldCoin);

			AuctionDlg_EventCase(103,OnBrowsePrePage);
			AuctionDlg_EventCase(104,OnBrowseNextPage);
			AuctionDlg_EventCase(203,OnRoupPrePage);
			AuctionDlg_EventCase(204,OnRoupNextPage);
			AuctionDlg_EventCase(316,OnAuctionPrePage);
			AuctionDlg_EventCase(317,OnAuctionNextPage);

		default:break;
		}
	}
	if( nEvent == EVENT_LISTBOX_ITEM_DBLCLK )
	{
		switch( nID )
		{
		case 102: return OnClickBuyOnBrowse();	//浏览页面直接竞价
		case 202: return OnClickAddPrice();		//竞拍页面直接加价
		default:  return true;
		}
	}
	if( nEvent == EVENT_EDITBOX_CHANGE )
	{
		int pre  = 0;
		int next = 0;
		int cur = 0;
		switch( nID )
		{
		case 120: pre = 103;next = 104;cur = 0;break;
		case 220: pre = 203;next = 204;cur = 1;break;
		case 320: pre = 316;next = 317;cur = 2;break;
		default:  return true;
		}

		CGuiEditBox* pEdit = GetEditBox( nID ); //120、220、320
		if( NULL != pEdit && pEdit->IsFocus() )
		{
			int page = GetCurPage(cur);
			AuctionDlg_ButtonSetEnable( pre, (page > 0) );
			AuctionDlg_ButtonSetEnable( next,(page < GetTotalPage(cur)) );
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiAuctionDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	RemoveAll(0);
	RemoveAll(1);
	RemoveAll(2);

	GetScene()->GetAuction()->Release();
	ClearItem();
	if( m_pMoneyInputDlg )
	{
		m_pMoneyInputDlg->Close();
		m_pMoneyInputDlg = NULL;
	}
	GetGameGuiManager()->m_guiAuctionDlg = NULL;
	FUNCTION_END;
}

HRESULT CGuiAuctionDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	int iFont = GetDevice()->SetFont(1);

	{	//银子
		int iMoney = GetScene()->GetMainCharacter()->GetMoney();
		const char* pszMoney = Money2String(iMoney);
		AuctionDlg_DrawString(pszMoney,g_scMoneyRect);
	}	//金子
	{
		int iGold = GetScene()->GetMainCharacter()->GetGoldNum();
		const char* pszGold = Money2String(iGold);
		AuctionDlg_DrawString(pszGold,g_scGoldRect);
	}

	int cur = m_pTab->GetCurItem();
	if( cur >= 0 && cur < 3 )
	{
		AuctionDlg_DrawInt( (GetTotalPage(cur)),g_scTotalPageRect );
	}
	CRoleItem* pItem = GetCellItem(m_pAuctionTable,stPointI(0,0));
	if( pItem  )
	{
		DWORD charge = GetPayMent( pItem->GetTObject() );
		AuctionDlg_DrawMoney(charge,g_scPaymentRect);		
	}

	GetDevice()->SetFont(iFont);

	return hr;
	FUNCTION_END;
}

CGuiTable* CGuiAuctionDialog::GetTableByPos( const stObjectLocation& pos )
{
	FUNCTION_BEGIN;
	if( pos.dwLocation == OBJECTCELLTYPE_MAIL )
	{
		switch( pos.dwTableID)
		{
		case 100: return m_pTable[0];
		case 200: return m_pTable[1];
		case 300: return m_pTable[2];
		case 313: return m_pAuctionTable;
		default:
			break;
		}
	}
	return NULL;
	FUNCTION_END;
}

bool CGuiAuctionDialog::OnClickBrowse()
//浏览
{
	FUNCTION_BEGIN;
	SetBackImage(stResourceLocation("data\\Interfaces.gl",55,0));
	m_pTab->SetCurItem(0);
	GetButton(3)->SetChecked(true);
	m_pBrowsePannel->SetCurrentPannel(0);
	if ( NULL != m_pImage[0] )
	{
		m_pImage[0]->SetVisible(false);
	}

	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::OnClickRoup()
//竞拍
{
	FUNCTION_BEGIN;
	SetBackImage(stResourceLocation("data\\Interfaces.gl",55,0));
	m_pTab->SetCurItem(1);
	GetButton(4)->SetChecked(true);

	if ( GetScene()->GetAuction()->m_RoupList.empty() )
	{
		stGetListAuction cmd;
		cmd.list = 2;
		SEND_USER_CMD(cmd);
	}

	if ( NULL != m_pImage[1] )
	{
		m_pImage[1]->SetVisible(false);
	}
	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::OnClickAuction()
//拍卖
{
	FUNCTION_BEGIN;
	SetBackImage(stResourceLocation("data\\Interfaces.gl",55,1));
	m_pTab->SetCurItem(2);
	GetButton(5)->SetChecked(true);

	if ( GetScene()->GetAuction()->m_AuctionList.empty() )
	{
		stGetListAuction cmd;
		cmd.list = 3;
		SEND_USER_CMD(cmd);
	}
	if ( NULL != m_pImage[2] )
	{
		m_pImage[2]->SetVisible(false);
	}

	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::OnClickClose()
{
	FUNCTION_BEGIN;
	Close();
	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::OnBrowseNextPage()
{
	FUNCTION_BEGIN;
	//每次点这个按钮的时候就禁用下一页按钮,直道收到往列表中增加新的拍卖物品时
	//才将该按钮设为可用状态
	int page = GetCurPage(0);
	if( page >= GetTotalPage(0) )
	{
		return true;
	}
	GotoPage(page+1,0);
	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::OnRoupNextPage()
{
	FUNCTION_BEGIN;
	GotoPage( GetCurPage(1)+1,1 );	
	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::OnAuctionNextPage()
{
	FUNCTION_BEGIN;
	GotoPage( GetCurPage(2)+1,2 );
	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::OnBrowsePrePage()
{
	FUNCTION_BEGIN;
	int page = GetCurPage(0);
	if( page == 0 )
	{
		return true;
	}
	GotoPage(page-1,0);
	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::OnRoupPrePage()
{
	FUNCTION_BEGIN;
	GotoPage( GetCurPage(1)-1,1 );
	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::OnAuctionPrePage()
{
	FUNCTION_BEGIN;
	GotoPage( GetCurPage(2)-1,2 );
	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::OnClickSearch()
{
	FUNCTION_BEGIN;	

	int index = m_pTab->GetCurItem();
	if( index == 0 ) //浏览
	{
		GotoPage(GetCurPage(0),0);
		return true;
	}
	if( index == 1 ) //竞拍
	{
		GetScene()->GetAuction()->ClearListByType(eAuction_Roup);
		RemoveAll(1);
		stGetListAuction cmd;
		cmd.list = 2;
		SEND_USER_CMD(cmd);
		return true;
	}
	if( index == 2 ) //拍卖
	{
		GetScene()->GetAuction()->ClearListByType(eAuction_Auction);
		RemoveAll(2);
		stGetListAuction cmd;
		cmd.list = 3;
		SEND_USER_CMD(cmd);
		return true;
	}
	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::OnClickBuyOnBrowse()
{
	FUNCTION_BEGIN;
	AuctionSelectItem(eAuction_Browse);
	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::OnClickAddPrice()
{
	FUNCTION_BEGIN;
	AuctionSelectItem(eAuction_Roup);
	return true;
	FUNCTION_END;
}
bool CGuiAuctionDialog::OnClickBuyAuction()
{
	FUNCTION_BEGIN;
	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::OnYiKouJia()
{
	FUNCTION_BEGIN;
	CAuction* pAuction = GetScene()->GetAuction();
	int index = m_pTab->GetCurItem();
	if ( index == 0 )
	{
		int id = GetSelectAuctionId(m_pListBox[0]);
		stAuctionItem* pItem = GetScene()->GetAuction()->GetItem(id,eAuction_Browse);
		if ( NULL != pItem )
		{
			stBidAuction cmd;
			cmd.auctionID = id;
			if( pItem->bidType == AUCTION_BID_TYPE_MONEY)
			{
				cmd.money = pItem->maxMoney;
			}
			else
			{
				cmd.gold = pItem->maxGold;
			}
			cmd.money = pItem->maxMoney;
			SEND_USER_CMD(cmd);
			return true;
		}
	}
	if ( index == 1 )
	{
		int id = GetSelectAuctionId(m_pListBox[1]);
		stAuctionItem* pItem = GetScene()->GetAuction()->GetItem(id,eAuction_Roup);
		if ( NULL != pItem )
		{
			stBidAuction cmd;
			cmd.auctionID = id;
			if( pItem->bidType == AUCTION_BID_TYPE_MONEY)
			{
				cmd.money = pItem->maxMoney;
			}
			else
			{
				cmd.gold = pItem->maxGold;
			}
			SEND_USER_CMD(cmd);
			return true;
		}
	}

	return true;
	FUNCTION_END;
}


bool CGuiAuctionDialog::OnCheckMoney()
{
	FUNCTION_BEGIN;
	GetButton(314)->SetChecked(true);

	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::OnCheckGoldCoin()
{
	FUNCTION_BEGIN;
	GetButton(315)->SetChecked(true);	
	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::OnStartAuction()
{
	FUNCTION_BEGIN;
	CRoleItem* pItem = GetCellItem(m_pAuctionTable,stPointI(0,0));
	if( pItem  )
	{
		bool bIsMoneyAuction = GetButton(314)->IsEnabled();
		char szTemp[MAX_PATH];
		sprintf(szTemp,"%d%s%d%s%d%s",strtoul(GetEditBox(301)->GetText(),NULL,10),INGOT_TXT,
			strtoul(GetEditBox(302)->GetText(),NULL,10),TAEL_TXT,
			strtoul(GetEditBox(303)->GetText(),NULL,10),CENT_TXT);
		int iMin = String2Money(szTemp);	

		sprintf(szTemp,"%d%s%d%s%d%s",strtoul(GetEditBox(305)->GetText(),NULL,10),INGOT_TXT,
			strtoul(GetEditBox(306)->GetText(),NULL,10),TAEL_TXT,
			strtoul(GetEditBox(307)->GetText(),NULL,10),CENT_TXT);
		int iMax = String2Money(szTemp);

		if( iMin > iMax && iMax != 0 )
		{
			GetGameGuiManager()->AddMessageBox("起价不可以大于一口价");
			return true;
		}
		if ( iMin == 0 )
		{
			GetGameGuiManager()->AddMessageBox("起价不可以为零");
			return true;
		}	
		int charge = GetPayMent(pItem->GetObject()->kind,pItem->GetObject()->dwNum,pItem->GetObjectBase()->price);
		if ( GetScene()->GetMainCharacter()->GetMoney() < charge )
		{
			GetGameGuiManager()->AddMessageBox("你的银子不够支付拍卖费用.");
			return true;
		}
		GetGameGuiManager()->AddMessageBox("拍卖这件物品,成交后系统将收取1%的成交额作为佣金,你确定吗?",eAction_ConfirmStartAuction,NULL,c_szMBOKCancel);
	}
	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::OnStopAuction()
{
	FUNCTION_BEGIN;

	int aId = GetSelectAuctionId(m_pListBox[2]);
	if( aId != 0 )
	{
		stAuctionItem* pItem = GetScene()->GetAuction()->GetItem(aId,eAuction_Auction);
		if ( pItem )
		{
			char szTemp[MAX_PATH];
			int charge = GetPayMent(pItem->item) * 3;
			_snprintf(szTemp,MAX_PATH,"取消本次拍卖系统将收取你%s的违约金，你确定要取消本次拍卖吗？",
				Money2String(charge));
			szTemp[MAX_PATH-1] = 0;
			GetGameGuiManager()->AddMessageBox(szTemp,eAction_ConfirmCannelAuction,(void*)aId,c_szMBOKCancel);
		}
	}
	return true;
	FUNCTION_END;
}

//按type类型搜索(浏览)
bool CGuiAuctionDialog::OnBrowseSearch(BYTE type)
{
	FUNCTION_BEGIN;
	m_byBrowseType = type;

	return true;
	FUNCTION_END;
}
//按type类型搜索(竞拍)
bool CGuiAuctionDialog::OnRoupSearch(BYTE type)
{
	FUNCTION_BEGIN;
	if ( m_byRoupType  == type )
	{
		return true;
	}
	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::RemoveAll(int index)
{
	FUNCTION_BEGIN;
	Assert( index >= 0 && index < 3 );
	Assert( m_pListBox[index] );
	Assert( m_pTable[index] );
	Assert( m_pImage[index] );

	if ((index < 0 || index >= 3)	||
		(NULL == m_pListBox[index]) ||
		(NULL == m_pTable[index])	||
		(NULL == m_pImage[index]) )
	{
		return true;
	}


	m_pListBox[index]->SetVisible(false);
	m_pImage[index]->SetVisible(true);

	for( int i = 0; i < PAGE_ITEM_NUM; ++i )
	{
		CRoleItem* pItem = GetCellItem( m_pTable[index],stPointI(0,i) );
		if( pItem )
		{
			pItem->OnRemove();
			delete pItem;
		}
	}
	m_pListBox[index]->RemoveAllItems();

	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::AddAuctionItem(stAuctionItem* pItem,int index)
{
	FUNCTION_BEGIN;
	Assert( pItem );
	Assert( index >= 0 && index < 3 );
	Assert( m_pListBox[index] );
	Assert( m_pTable[index] );
	Assert( m_pImage[index] );

	if ( (NULL == pItem ) || 
		(index < 0 || index >= 3)	||
		(NULL == m_pListBox[index]) ||
		(NULL == m_pTable[index]) ||
		(NULL == m_pImage[index]) )
	{
		return true;
	}

	m_pImage[index]->SetVisible(false);

	if ( m_pTab->GetCurItem() == index )
	{
		m_pListBox[index]->SetVisible(true);
	}	

	if( index == 0 )
	{
		int page = (pItem->max+PAGE_ITEM_NUM-1)/PAGE_ITEM_NUM;
		SetTotalPage( page,0 );
	}
	int iCount = m_pListBox[index]->GetItemCount();
	if( iCount > PAGE_ITEM_NUM - 1 )
	{
		return false;
	}

	CRoleItem* pRoleItem = new CRoleItem();
	pItem->item.pos.dwLocation = m_pTable[index]->m_iTableType;
	pItem->item.pos.dwTableID = m_pTable[index]->m_iTableID;
	pItem->item.pos.x = 0;
	pItem->item.pos.y = iCount;
	pRoleItem->SetObject(&(pItem->item));
	char szTemp[MAX_PATH];
	char szTemp1[MAX_PATH];
	_snprintf(szTemp,MAX_PATH,pItem->item.strName);
	szTemp[MAX_PATH-1]=0;
	m_pListBox[index]->AddItem(szTemp,(void*)(pItem->auctionID));

	SetFrameAllocator fa;
	std::vector<std::stack_string,std::stack_allocator<std::stack_string> > strs;
	_parse_str(strs,szTemp,'0');

	if ( strs.size() > 1 )
	{
		m_pListBox[index]->SetItemTextEx(iCount,0,strs[1].c_str(),strs[0].c_str(),17);
	}
	else
	{
		m_pListBox[index]->SetItemText(iCount,0,szTemp,17,GuiListBoxColFmt_Center);
	}

	_snprintf(szTemp,MAX_PATH,"%d",pItem->item.needlevel);
	szTemp[MAX_PATH-1]=0;
	m_pListBox[index]->SetItemText(iCount,1,szTemp,5,GuiListBoxColFmt_Center);

	_snprintf(szTemp,MAX_PATH,"%s",ItemKind2String(pItem->item.kind));
	szTemp[MAX_PATH-1]=0;
	m_pListBox[index]->SetItemText(iCount,2,szTemp,5,GuiListBoxColFmt_Center);

	if( index == 2 )
	{
		m_pListBox[index]->SetItemText(iCount,3," ",2,GuiListBoxColFmt_Center);
	}
	else
	{
		_snprintf(szTemp,MAX_PATH,"%s",pItem->owner);
		szTemp[MAX_PATH-1]=0;
		m_pListBox[index]->SetItemText(iCount,3,szTemp,17,GuiListBoxColFmt_Center);
	}

	int hour = pItem->endTime/3600;
	if( hour > 0 )
	{
		_snprintf(szTemp,MAX_PATH,"%d小时",hour);
		szTemp[MAX_PATH-1]=0;
	}
	else
	{
		_snprintf(szTemp,MAX_PATH,"小于1小时");
		szTemp[MAX_PATH-1]=0;
	}

	m_pListBox[index]->SetItemText(iCount,4,szTemp,9,GuiListBoxColFmt_Center);

	if( index == 2 )
	{
		//显示空行
		m_pListBox[index]->SetItemText(iCount,5," ",2);
	}
	else
	{
		//显示空行
		m_pListBox[index]->SetItemText(iCount,5," ",1);
	}


	if(pItem->bidType == AUCTION_BID_TYPE_MONEY)
	{
		if( index == 1 )
		{
			if( pItem->mine )
			{
				_snprintf(szTemp,MAX_PATH,"你的出价:%s",Money2String(pItem->minMoney));
				szTemp[MAX_PATH-1]=0;
			}
			else
			{
				_snprintf(szTemp,MAX_PATH,"被压过:%s",Money2String(pItem->minMoney));
				szTemp[MAX_PATH-1]=0;
			}		
		}	
		else
		{
			_snprintf(szTemp,MAX_PATH,"起价:%s",Money2String(pItem->minMoney));
			szTemp[MAX_PATH-1]=0;
		}
		if( pItem->maxMoney == 0 )
		{
			_snprintf(szTemp1,MAX_PATH,"一口价:(无)");
			szTemp1[MAX_PATH-1]=0;
		}
		else
		{
			_snprintf(szTemp1,MAX_PATH,"一口价:%s",Money2String(pItem->maxMoney));
			szTemp1[MAX_PATH-1]=0;
		}
	}
#ifdef __USE_GOLD
	else
	{
		if( index == 1 )
		{
			if( pItem->mine )
			{
				_snprintf(szTemp,MAX_PATH,"你的出价:%s",Money2String(pItem->minGold));
				szTemp[MAX_PATH-1]=0;
			}
			else
			{
				_snprintf(szTemp,MAX_PATH,"被压过:%s",Money2String(pItem->minGold));
				szTemp[MAX_PATH-1]=0;
			}		
		}	
		else
		{
			_snprintf(szTemp,MAX_PATH,"起价:%s",Money2String(pItem->minGold));
			szTemp[MAX_PATH-1]=0;
		}
		if( pItem->maxGold == 0 )
		{
			_snprintf(szTemp1,MAX_PATH,"一口价:(无)");
			szTemp1[MAX_PATH-1]=0;
		}
		else
		{
			_snprintf(szTemp1,MAX_PATH,"一口价:%s",Money2String(pItem->maxGold));
			szTemp1[MAX_PATH-1]=0;
		}
	}
#endif

	m_pListBox[index]->SetItemTextEx(iCount,6,szTemp,szTemp1,20);
	if( pItem->bidType == AUCTION_BID_TYPE_MONEY )
	{
		m_pListBox[index]->SetItemColor(iCount,6,-1,-1);
	}
#ifdef __USE_GOLD
	else
	{
		m_pListBox[index]->SetItemColor(iCount,6,COLOR_ARGB(255,255,255,0),COLOR_ARGB(255,255,255,0));
	}
#endif

	if( !(pItem->mine) && index == 1 )
	{
		m_pListBox[index]->SetItemColor(iCount,0,0xffff0000);
		m_pListBox[index]->SetItemColor(iCount,1,0xffff0000);
		m_pListBox[index]->SetItemColor(iCount,2,0xffff0000);
		m_pListBox[index]->SetItemColor(iCount,3,0xffff0000);
		m_pListBox[index]->SetItemColor(iCount,4,0xffff0000);
		m_pListBox[index]->SetItemColor(iCount,5,0xffff0000);
	}
	if( pItem->mine && index == 0 )
	{
		m_pListBox[index]->SetItemColor(iCount,0,0xff00ff00);
		m_pListBox[index]->SetItemColor(iCount,1,0xff00ff00);
		m_pListBox[index]->SetItemColor(iCount,2,0xff00ff00);
		m_pListBox[index]->SetItemColor(iCount,3,0xff00ff00);
		m_pListBox[index]->SetItemColor(iCount,4,0xff00ff00);
		m_pListBox[index]->SetItemColor(iCount,5,0xff00ff00);
	}

	return true;

	FUNCTION_END;
}


bool CGuiAuctionDialog::RemoveAuctionItem(CGuiTable* &pTable,CGuiListBox* &pList,int index)
{
	FUNCTION_BEGIN;
	CRoleItem* pItem = GetCellItem( pTable,stPointI(0,index) );
	if( pItem )
	{
		pItem->OnRemove();
		delete pItem;
	}
	pList->RemoveItem(index);
	for ( int i = index + 1; i < PAGE_ITEM_NUM; ++i )
	{
		CRoleItem* pItem = GetCellItem( pTable,stPointI(0,i) );
		if ( pItem )
		{
			pItem->GetObject()->pos.y -= 1;
			pItem->OnUpdate(-1);
		}
	}
	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::AddAuctionItem(stAuctionItem* pItem,enumAuctionListType type)
{
	FUNCTION_BEGIN;

	if( type == eAuction_Browse || type == eAuction_All )
	{
		AddAuctionItem(pItem,0);
	}
	if( type == eAuction_Roup || type == eAuction_All )
	{
		AddAuctionItem(pItem,1);
	}
	if( type == eAuction_Auction || type == eAuction_All ) 
	{
		AddAuctionItem(pItem,2);
	}
	return true;
	FUNCTION_END;
}

bool CGuiAuctionDialog::RemoveAuctionItem(DWORD dwId,enumAuctionListType type)
{
	FUNCTION_BEGIN;

	if( type == eAuction_Browse || type == eAuction_All )
	{
		RemoveAuctionItemByPage( dwId,0 );
	}
	if( type == eAuction_Roup || type == eAuction_All )
	{
		RemoveAuctionItemByPage( dwId,1 );
	}
	if( type == eAuction_Auction || type == eAuction_All ) 
	{
		RemoveAuctionItemByPage( dwId,2 );
	}
	return true;

	FUNCTION_END;
}

bool CGuiAuctionDialog::RemoveAuctionItemByPage(DWORD dwId,int page)
{
	FUNCTION_BEGIN;
	Assert( page >= 0 && page < 3 );
	int index = GetIndex( dwId,m_pListBox[page] );
	if ( index >= 0 && index < PAGE_ITEM_NUM )
	{
		return RemoveAuctionItem(m_pTable[page],m_pListBox[page],index);
	}
	return false;
	FUNCTION_END;
}

int  CGuiAuctionDialog::GetIndex(DWORD dwId,CGuiListBox* pList)
{
	FUNCTION_BEGIN;
	int iCount = pList->GetItemCount();
	for ( int i = 0; i < iCount; ++i )
	{
		DWORD dwAuctionId = (DWORD)( pList->GetItemData(i) );
		if( dwAuctionId == dwId )
		{
			return i;
		}
	}
	return -1;
	FUNCTION_END;
}

void CGuiAuctionDialog::ClearItem()
{
	FUNCTION_BEGIN;
	//清除物品(镜像)
	CRoleItem* pItem = GetCellItem(m_pAuctionTable,stPointI(0,0));
	if( pItem  )
	{
		pItem->OnRemove();
		delete pItem;
		pItem = NULL;	
	}
	FUNCTION_END;
}


void CGuiAuctionDialog::UpdateList( tListAuctionItem& list,enumAuctionListType type )
{
	FUNCTION_BEGIN;
	Debug_Int( list.size() );
	if( type == eAuction_Browse )
	{
		RemoveAll(0);
		for( size_t i = 0; i < list.size(); ++i )
		{
			AddAuctionItem(list[i],0);
		}
		return ;
	}
	if ( type == eAuction_Roup )
	{
		SetTotalPage( ((list.size()+PAGE_ITEM_NUM-1)/PAGE_ITEM_NUM),1 );
		int page = max( GetCurPage(1)-1,0 );
		RemoveAll(1);
		for ( int i = page*PAGE_ITEM_NUM; i < list.size(); ++i )
		{
			AddAuctionItem(list[i],1);
		}
		return ;
	}
	if ( type == eAuction_Auction )
	{
		SetTotalPage( ((list.size()+PAGE_ITEM_NUM-1)/PAGE_ITEM_NUM),2 );
		int page = max( GetCurPage(2)-1,0 );
		RemoveAll(2);
		for ( int i = page*PAGE_ITEM_NUM; i < list.size(); ++i )
		{
			AddAuctionItem(list[i],2);
		}
		return ;
	}
	FUNCTION_END;
}

void CGuiAuctionDialog::UpdateAuctionItem(int page,stAuctionItem* pItem)
{
	FUNCTION_BEGIN;
	if( page >= 0 && page < 3 )
	{
		UpdateAuctionItem( m_pListBox[page],pItem );
	}
	else
	{
		UpdateAuctionItem( m_pListBox[0],pItem );
		UpdateAuctionItem( m_pListBox[1],pItem );
		UpdateAuctionItem( m_pListBox[2],pItem );
	}	

	FUNCTION_END;
}

void CGuiAuctionDialog::UpdateAuctionItem(CGuiListBox* &pList,stAuctionItem* pItem)
{
	FUNCTION_BEGIN;
	int index = GetIndex(pItem->auctionID,pList);
	if( index >= 0 && index < PAGE_ITEM_NUM )
	{
		char szTemp[MAX_PATH];
		char szTemp1[MAX_PATH];

		int hour = pItem->endTime/3600;
		if( hour > 0 )
		{
			_snprintf(szTemp,MAX_PATH,"%d小时",hour);
			szTemp[MAX_PATH-1]=0;
		}
		else
		{
			_snprintf(szTemp,MAX_PATH,"小于1小时");
			szTemp[MAX_PATH-1]=0;
		}
		pList->SetItemText(index,4,szTemp,9,GuiListBoxColFmt_Center);

		if( index == 2 )
		{
			//显示空行
			pList->SetItemText(index,5," ",2);
		}
		else
		{
			//显示空行
			pList->SetItemText(index,5," ",1);
		}


		if(pItem->bidType == AUCTION_BID_TYPE_MONEY)
		{
			if( index == 1 )
			{
				if( pItem->mine )
				{
					_snprintf(szTemp,MAX_PATH,"你的出价:%s",Money2String(pItem->minMoney));
					szTemp[MAX_PATH-1]=0;
				}
				else
				{
					_snprintf(szTemp,MAX_PATH,"被压过:%s",Money2String(pItem->minMoney));
					szTemp[MAX_PATH-1]=0;
				}		
			}	
			else
			{
				_snprintf(szTemp,MAX_PATH,"起价:%s",Money2String(pItem->minMoney));
				szTemp[MAX_PATH-1]=0;
			}
			if( pItem->maxMoney == 0 )
			{
				_snprintf(szTemp1,MAX_PATH,"一口价:(无)");
				szTemp1[MAX_PATH-1]=0;
			}
			else
			{
				_snprintf(szTemp1,MAX_PATH,"一口价:%s",Money2String(pItem->maxMoney));
				szTemp1[MAX_PATH-1]=0;
			}
		}
		else
		{
			if( index == 1 )
			{
				if( pItem->mine )
				{
					_snprintf(szTemp,MAX_PATH,"你的出价:%s",Money2String(pItem->minGold));
					szTemp[MAX_PATH-1]=0;
				}
				else
				{
					_snprintf(szTemp,MAX_PATH,"被压过:%s",Money2String(pItem->minGold));
					szTemp[MAX_PATH-1]=0;
				}		
			}	
			else
			{
				_snprintf(szTemp,MAX_PATH,"起价:%s",Money2String(pItem->minGold));
				szTemp[MAX_PATH-1]=0;
			}
			if( pItem->maxGold == 0 )
			{
				_snprintf(szTemp1,MAX_PATH,"一口价:(无)");
				szTemp1[MAX_PATH-1]=0;
			}
			else
			{
				_snprintf(szTemp1,MAX_PATH,"一口价:%s",Money2String(pItem->maxGold));
				szTemp1[MAX_PATH-1]=0;
			}
		}


		pList->SetItemTextEx(index,6,szTemp,szTemp1,20);
		if( pItem->bidType == AUCTION_BID_TYPE_MONEY )
		{
			pList->SetItemColor(index,6,-1,-1);
		}
		else
		{
			pList->SetItemColor(index,6,COLOR_ARGB(255,255,255,0),COLOR_ARGB(255,255,255,0));
		}

		if( !(pItem->mine) && pList->GetID() == 202 )
		{
			pList->SetItemColor(index,0,0xffff0000);
			pList->SetItemColor(index,1,0xffff0000);
			pList->SetItemColor(index,2,0xffff0000);
			pList->SetItemColor(index,3,0xffff0000);
			pList->SetItemColor(index,4,0xffff0000);
			pList->SetItemColor(index,5,0xffff0000);
		}
		if( pItem->mine && pList->GetID() == 102 )
		{
			pList->SetItemColor(index,0,0xff00ff00);
			pList->SetItemColor(index,1,0xff00ff00);
			pList->SetItemColor(index,2,0xff00ff00);
			pList->SetItemColor(index,3,0xff00ff00);
			pList->SetItemColor(index,4,0xff00ff00);
			pList->SetItemColor(index,5,0xff00ff00);
		}
	}
	FUNCTION_END;
}

int	 CGuiAuctionDialog::GetSelectAuctionId(CGuiListBox* pList)
{
	FUNCTION_BEGIN;
	int index = pList->GetCurItem();
	if( index >= 0 && index < PAGE_ITEM_NUM )
	{
		return (int )(pList->GetItemData(index));
	}
	return 0;	
	FUNCTION_END;
}

void CGuiAuctionDialog::CloseMoneyInputDialog()
{
	FUNCTION_BEGIN;
	m_pMoneyInputDlg = NULL;
	FUNCTION_END;
}

void CGuiAuctionDialog::OnConfirmStartAuction()
{
	FUNCTION_BEGIN;

	CRoleItem* pItem = GetCellItem(m_pAuctionTable,stPointI(0,0));
	if( pItem  )
	{
#ifdef __USE_GOLD
		bool bIsMoneyAuction = GetButton(314)->GetChecked();
#else
		bool bIsMoneyAuction = true;
#endif

		stSaleAuction cmd;
		cmd.itemID = pItem->GetThisID();
		char szTemp[MAX_PATH];
		sprintf(szTemp,"%d%s%d%s%d%s",strtoul(GetEditBox(301)->GetText(),NULL,10),INGOT_TXT,
			strtoul(GetEditBox(302)->GetText(),NULL,10),TAEL_TXT,
			strtoul(GetEditBox(303)->GetText(),NULL,10),CENT_TXT);
		int iMin = String2Money(szTemp);	

		sprintf(szTemp,"%d%s%d%s%d%s",strtoul(GetEditBox(305)->GetText(),NULL,10),INGOT_TXT,
			strtoul(GetEditBox(306)->GetText(),NULL,10),TAEL_TXT,
			strtoul(GetEditBox(307)->GetText(),NULL,10),CENT_TXT);
		int iMax = String2Money(szTemp);

		if(bIsMoneyAuction)
		{
			cmd.bidType = AUCTION_BID_TYPE_MONEY;
			cmd.minMoney = iMin;
			cmd.maxMoney = iMax;
			cmd.minGold = 0;
			cmd.maxGold = 0;
		}
#ifdef __USE_GOLD
		else
		{
			cmd.bidType = AUCTION_BID_TYPE_GOLD;
			cmd.minMoney = 0;
			cmd.maxMoney = 0;
			cmd.minGold = iMin;
			cmd.maxGold = iMax;
		}
#endif
		SEND_USER_CMD(cmd);
	}

	ClearItem();
	FUNCTION_END;	
}

bool CGuiAuctionDialog::OnClickSortBrowseByNeedLevel()
{
	static bool bUp = false;
	CAuction* pAuction = GetScene()->GetAuction();
	if ( pAuction )
	{
		pAuction->Sort(eAuction_Browse,eSort_By_NeedLevel,bUp);
		UpdateList(pAuction->m_BrowseList,eAuction_Browse);
		bUp = !bUp;
	}
	return true;
}

bool CGuiAuctionDialog::OnClickSortBrowseByKind()
{
	static bool bUp = false;
	CAuction* pAuction = GetScene()->GetAuction();
	if ( pAuction )
	{
		pAuction->Sort(eAuction_Browse,eSort_By_Quality,bUp);
		UpdateList(pAuction->m_BrowseList,eAuction_Browse);
		bUp = !bUp;
	}
	return true;
}

bool CGuiAuctionDialog::OnClickSortBrowseByOwner()
{
	static bool bUp = false;
	CAuction* pAuction = GetScene()->GetAuction();
	if ( pAuction )
	{
		pAuction->Sort(eAuction_Browse,eSort_By_Owner,bUp);
		UpdateList(pAuction->m_BrowseList,eAuction_Browse);
		bUp = !bUp;
	}
	return true;
}

bool CGuiAuctionDialog::OnClickSortBrowseByTime()
{
	static bool bUp = false;
	CAuction* pAuction = GetScene()->GetAuction();
	if ( pAuction )
	{
		pAuction->Sort(eAuction_Browse,eSort_By_LastTime,bUp);
		UpdateList(pAuction->m_BrowseList,eAuction_Browse);
		bUp = !bUp;
	}
	return true;
}

bool CGuiAuctionDialog::OnClickSortBrowseByPrice()
{
	static bool bUp = false;
	CAuction* pAuction = GetScene()->GetAuction();
	if ( pAuction )
	{
		pAuction->Sort(eAuction_Browse,eSort_By_Price,bUp);
		UpdateList(pAuction->m_BrowseList,eAuction_Browse);
		bUp = !bUp;
	}
	return true;
}

bool CGuiAuctionDialog::OnClickSortRoupByNeedLevel()
{
	static bool bUp = false;
	CAuction* pAuction = GetScene()->GetAuction();
	if ( pAuction )
	{
		pAuction->Sort(eAuction_Roup,eSort_By_NeedLevel,bUp);
		UpdateList(pAuction->m_RoupList,eAuction_Roup);
		bUp = !bUp;
	}
	return true;
}

bool CGuiAuctionDialog::OnClickSortRoupByKind()
{
	static bool bUp = false;
	CAuction* pAuction = GetScene()->GetAuction();
	if ( pAuction )
	{
		pAuction->Sort(eAuction_Roup,eSort_By_Quality,bUp);
		UpdateList(pAuction->m_RoupList,eAuction_Roup);
		bUp = !bUp;
	}
	return true;
}

bool CGuiAuctionDialog::OnClickSortRoupByOwner()
{
	static bool bUp = false;
	CAuction* pAuction = GetScene()->GetAuction();
	if ( pAuction )
	{
		pAuction->Sort(eAuction_Roup,eSort_By_Owner,bUp);
		UpdateList(pAuction->m_RoupList,eAuction_Roup);
		bUp = !bUp;
	}
	return true;
}

bool CGuiAuctionDialog::OnClickSortRoupByTime()
{
	static bool bUp = false;
	CAuction* pAuction = GetScene()->GetAuction();
	if ( pAuction )
	{
		pAuction->Sort(eAuction_Roup,eSort_By_LastTime,bUp);
		UpdateList(pAuction->m_RoupList,eAuction_Roup);
		bUp = !bUp;
	}
	return true;
}

bool CGuiAuctionDialog::OnClickSortRoupByPrice()
{
	static bool bUp = false;
	CAuction* pAuction = GetScene()->GetAuction();
	if ( pAuction )
	{
		pAuction->Sort(eAuction_Roup,eSort_By_Price,bUp);
		UpdateList(pAuction->m_RoupList,eAuction_Roup);
		bUp = !bUp;
	}
	return true;
}

bool CGuiAuctionDialog::AuctionSelectItem(enumAuctionListType type)
{
	FUNCTION_BEGIN;
	if(type != eAuction_Roup && type != eAuction_Browse )
	{
		return false;
	}
	int id = GetSelectAuctionId(m_pListBox[type]);
	if( id != 0 )
	{
		stAuctionItem* pItem = GetScene()->GetAuction()->GetItem(id,type);
		if ( NULL != pItem &&
			!(pItem->mine) &&
			(0 != strcmp(pItem->owner,GetScene()->GetMainCharacter()->GetName())))
		{
			if( pItem->bidType == AUCTION_BID_TYPE_MONEY )
			{
				m_pMoneyInputDlg = GetGameGuiManager()->AddAucMoneyInputDlg(id,AUCTION_BID_TYPE_MONEY);
				m_pMoneyInputDlg->SetMinPrice(pItem->minMoney);
				m_pMoneyInputDlg->SetMaxPrice(pItem->maxMoney);
			}
			else if( pItem->bidType == AUCTION_BID_TYPE_GOLD )
			{
				m_pMoneyInputDlg = GetGameGuiManager()->AddAucMoneyInputDlg(id,AUCTION_BID_TYPE_GOLD);
				m_pMoneyInputDlg->SetMinPrice(pItem->minGold);
				m_pMoneyInputDlg->SetMaxPrice(pItem->maxGold);
			}
		}
		return true;
	}	
	return false;
}


void CGuiAuctionDialog::SetTotalPage(int page,int index)	//index 表示tab页的index
{
	int cur = GetCurPage(index);
	if(  GetTotalPage(index) == page )
	{
		return;
	}
	if( index >= 0 && index < 3 )
	{
		m_iTotalPage[index] = page;
		SetCurPage( min(page,cur),index );
		CGuiEditBox* pEdit = GetEditBox( (index+1)*100 + 20 ); //120、220、320
		if( NULL != pEdit )
		{
			pEdit->SetMinNumber(1);
			pEdit->SetMaxNumber(page+1);
		}
	}
	else
	{
		Assert(0);
	}
}

bool CGuiAuctionDialog::GotoPage(int page,int index)//跳转到第page页
{
	FUNCTION_BEGIN;	
	
	switch( index )
	{
	case 0:
		{
			AuctionDlg_TestBtnEnabled(10);	//if button disabled return false

			GetScene()->GetAuction()->ClearListByType(eAuction_Browse);
			RemoveAll(0);		
			stQueryAuction cmd;
			cmd.type = m_byBrowseType;
			_snprintf(cmd.name,MAX_NAMESIZE,GetEditBox(6)->GetText());
			cmd.quality =(BYTE)((DWORD)(GetComboBox(8)->GetItemData( GetComboBox(8)->GetCurItem() )));
			cmd.page = max(page-1,0);
			const char* szLevel = GetEditBox(7)->GetText();
			if ( szLevel == NULL || strlen(szLevel) < 1 )
			{
				cmd.level = -1;
			}
			else
			{
				cmd.level = strtoul(szLevel,NULL,10);
			}		
			SEND_USER_CMD(cmd);

			AuctionDlg_ButtonSetEnable(10,false);
			AuctionDlg_ButtonSetEnableDelay(10,true);
		}
		break;
	case 1:
		{
			UpdateList(GetScene()->GetAuction()->m_RoupList,eAuction_Roup);
		}
		break;
	case 2:
		{
			UpdateList(GetScene()->GetAuction()->m_AuctionList,eAuction_Auction);
		}
		break;
	default:
		return false;
		break;
	}
	//当查询成功才修改当前page
	SetCurPage(page,index);
	return true;
}

void CGuiAuctionDialog::SetCurPage(int page,int index)
{
	FUNCTION_BEGIN;

	int pre  = 0;
	int next = 0;
	int id = 0;
	switch( index )
	{
	case 0: pre = 103;next = 104;id = 120;break;
	case 1: pre = 203;next = 204;id = 220;break;
	case 2: pre = 316;next = 317;id = 320;break;
	default:  return ;
	}

	CGuiEditBox* pEdit = GetEditBox( id ); 
	if( NULL != pEdit )
	{
		char sz[16];
		itoa( max(1,page),sz,10);
		pEdit->SetText(sz);

		AuctionDlg_ButtonSetEnable( pre, (page > 1) );
		AuctionDlg_ButtonSetEnable( next,(page < GetTotalPage(index)) );
	}
}

int  CGuiAuctionDialog::GetCurPage(int index)
{	
	CGuiEditBox* pEdit = GetEditBox( (index+1)*100 + 20 ); //120、220、320
	if( NULL != pEdit )
	{
		const char* psz = pEdit->GetText();
		int iRes = atoi(psz);
		return  max(0,iRes);
	}
	return 0;
}

int  CGuiAuctionDialog::GetTotalPage(int index)
{
	return m_iTotalPage[index];
}