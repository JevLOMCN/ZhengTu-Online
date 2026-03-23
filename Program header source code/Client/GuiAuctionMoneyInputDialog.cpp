#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GuiAuctionMoneyInputDialog.h"
#include "./RoleItem.h"
#include "./MainCharacter.h"
#include "./GuiAuctionDialog.h"


const static stRectI g_MoneyInputDefaultIngotRect(54,240,109,260);
const static stRectI g_MoneyInputDefaultTaelRect(135,240,177,260);
const static stRectI g_MoneyInputDefaultCentRect(207,240,246,260);

const static stRectI g_MoneyInputBidTypeRect(288,97,351,111);


#define MoneyInputDlg_DrawString(str,rect)		{stRectI rc = rect;rc.OffsetRect( m_x,m_y );GetDevice()->DrawString(str,rc,-1,DT_CENTER);}
#define MoneyInputDlg_DrawInt(n,rect)			{char szTemp[32];sprintf(szTemp,"%u",n);MoneyInputDlg_DrawString(szTemp,rect);}


CGuiAuctionMoneyInputDialog::CGuiAuctionMoneyInputDialog(BYTE type)
{
	FUNCTION_BEGIN;
	m_iMinPrice = 0;
	m_dwAuctionId = 0;
	m_iMaxPrice = 0;
	m_type = type;
	FUNCTION_END;
}

CGuiAuctionMoneyInputDialog::~CGuiAuctionMoneyInputDialog(void)
{
	FUNCTION_BEGIN;
}

void CGuiAuctionMoneyInputDialog::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	m_ptIcon = stPointI(30,84);
	if(m_type == AUCTION_BID_TYPE_MONEY)			//银子
	{
		SetIcon(&stResourceLocation("data\\interfaces.gl",55,85));
	}
	else if(m_type == AUCTION_BID_TYPE_GOLD)	//金子
	{
		SetIcon(&stResourceLocation("data\\interfaces.gl",55,84));
	}

	FUNCTION_END;
}

bool CGuiAuctionMoneyInputDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case 5: return OnIDOK();
		default:
			break;
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

bool CGuiAuctionMoneyInputDialog::OnIDOK()
{
	FUNCTION_BEGIN;
	stBidAuction cmd;
	cmd.auctionID = m_dwAuctionId;

	char szTemp[MAX_PATH];
	sprintf(szTemp,"%d%s%d%s%d%s",strtoul(GetEditBox(2)->GetText(),NULL,10),INGOT_TXT,
		strtoul(GetEditBox(3)->GetText(),NULL,10),TAEL_TXT,
		strtoul(GetEditBox(4)->GetText(),NULL,10),CENT_TXT);

	int money = String2Money(szTemp);
	if( m_iMaxPrice != 0 && money > m_iMaxPrice )
	{
		GetGameGuiManager()->AddMessageBox("你的出价超过了该物品的一口价,出价无效");
		return true;
	}

	if( m_iMinPrice == m_iMaxPrice && money == m_iMinPrice )
	{
			cmd.money = money;
			SEND_USER_CMD(cmd);
			Close();
			return true;
	}
	if( money <= m_iMinPrice )
	{
		GetGameGuiManager()->AddMessageBox("你的出价应该大于该物品的起价,出价无效");
		return true;
	}

	if(m_type == AUCTION_BID_TYPE_MONEY)			//银子
	{
		cmd.money = money;
	}
	else if(m_type == AUCTION_BID_TYPE_GOLD)	//金子
	{
		cmd.gold = money;
	}
	SEND_USER_CMD(cmd);
	Close();
	return true;
	FUNCTION_END;
}

void CGuiAuctionMoneyInputDialog::OnClose(void)
{
	FUNCTION_BEGIN;
	if ( GetGameGuiManager()->m_guiAuctionDlg )
	{
		GetGameGuiManager()->m_guiAuctionDlg->CloseMoneyInputDialog();
	}
	GetGameGuiManager()->m_guiAucMoneyInputDlg = NULL;
	FUNCTION_END;
}

void CGuiAuctionMoneyInputDialog::SetMaxPrice(int price)
{
	FUNCTION_BEGIN;
	m_iMaxPrice = price;
	FUNCTION_END;
}

void CGuiAuctionMoneyInputDialog::SetMinPrice(int price)
{
	FUNCTION_BEGIN;
	m_iMinPrice = price;
	FUNCTION_END;
}

HRESULT CGuiAuctionMoneyInputDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	
	int iNgot = GETMONEYINGOT(m_iMinPrice);
	int iTael = GETMONEYTAEL(m_iMinPrice);
	int iCent = GETMONEYCENT(m_iMinPrice);

	int iFont = GetDevice()->SetFont(1);
	MoneyInputDlg_DrawInt(iNgot,g_MoneyInputDefaultIngotRect);
	MoneyInputDlg_DrawInt(iTael,g_MoneyInputDefaultTaelRect);
	MoneyInputDlg_DrawInt(iCent,g_MoneyInputDefaultCentRect);

	if( m_type == AUCTION_BID_TYPE_MONEY)
	{
		MoneyInputDlg_DrawString("--银子",g_MoneyInputBidTypeRect)
	}
	else
	{
		MoneyInputDlg_DrawString("--金子",g_MoneyInputBidTypeRect)
	}
	GetDevice()->SetFont(iFont);

	return hr;
	FUNCTION_END;
}

void CGuiAuctionMoneyInputDialog::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt( m_pBtnClose->GetLocation() );
		m_pBtnClose->SetLocation(pt.x - 4,pt.y);
	}

	FUNCTION_END;
}