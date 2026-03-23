#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GuiSendMailDialog.h"
#include "./GameCursor.h"
#include "./RoleItem.h"
#include "./GuiMoneyInputBox.h"
#include "./GuiInputBox.h"
#include "../gui/include/GuiFloatItem.h"

#define IDC_MAILTEXT_MLEDITBOX	1
#define IDC_ITEM_TABLE			10
#define IDC_SENDTO_EDITBOX		11
//#define IDC_GOLDCOIN_EDITBOX	12
#define IDC_WEN_EDITBOX			13
#define IDC_LIANG_EDITBOX		14
#define IDC_DING_EDITBOX		15



#define SendMailDlg_ButtonSetEnable(id,bEnable)	{CGuiButton* pBtn = GetButton(id);if( NULL != pBtn ){ pBtn->SetEnabled(bEnable);}}
#define SendMailDlg_ButtonSetVisible(id,bShow)	{CGuiButton* pBtn = GetButton(id);if( NULL != pBtn ){ pBtn->SetVisible(bShow);}}
#define SendMailDlg_ButtonGetCheck(id,bCheck)	{CGuiButton* pBtn = GetButton(id);if( NULL != pBtn ){ bCheck = pBtn->GetChecked();}}
#define SendMailDlg_ButtonSetCheck(id,bCheck)   {CGuiButton* pBtn = GetButton(id);if( NULL != pBtn ){ pBtn->SetChecked(bCheck);}}
#define SendMailDlg_EditSetEnable(id,bEnable)	{CGuiEditBox* pEdit = GetEditBox(id);if(NULL != pEdit){ pEdit->SetEnabled(bEnable);}}	
#define SendMailDlg_EditSetVisible(id,bShow)	{CGuiEditBox* pEdit = GetEditBox(id);if(NULL != pEdit){ pEdit->SetVisible(bShow);}}	
#define SendMailDlg_EditClear(id)	{CGuiEditBox* pEdit = GetEditBox(id);if(NULL != pEdit){ pEdit->ClearText();}}	
#define SendMailDlg_Edit_ClearAndHide(id) SendMailDlg_EditClear(id) SendMailDlg_EditSetVisible(id,false)

//const stRectI g_sPostageRect(280,330,345,353);
const stRectI g_sPostageRect(245,333,345,353);
const char* g_szPostageOfSendMail = "50文";

CGuiSendMailDialog::CGuiSendMailDialog(void)
{
	m_pToNameEditBox = NULL;
	m_pDingEditBox = NULL;
	m_pLiangEditBox = NULL;
	m_pWenEditBox = NULL;
	m_pMailTextMLEditBox = NULL;

	m_pMoneyInputDlg = NULL;
	m_pGoldCoinInputDlg = NULL;

	m_iSendMoney = 0;
	m_iSendGoldCoin = 0;

	m_rcSendMoney = stRectI(0,0,0,0);
	m_rcSendGoldCoin = stRectI(0,0,0,0);

	m_pMoneyBmp = NULL;				//银子显示
	m_pGoldCoinBmp = NULL;			//金子显示
	m_pNoMoneyBmp = NULL;			//银子显示(没有银子)
	m_pNoGoldCoinBmp = NULL;		//金子显示(没有金子)   
}

CGuiSendMailDialog::~CGuiSendMailDialog(void)
{
}

void CGuiSendMailDialog::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
#ifndef __USE_GOLD
	SendMailDlg_ButtonSetEnable(5,false);

	SendMailDlg_EditSetEnable(16,false);
	SendMailDlg_EditSetEnable(17,false);
	SendMailDlg_EditSetEnable(18,false);
#endif

	m_pItemTable = GetTable(IDC_ITEM_TABLE);
	if( NULL == m_pItemTable )
	{
		GetGameGuiManager()->AddMessageBox("发件箱脚本出错");
		Engine_WriteLog("发件箱对话脚本出错");
		Close();
		return ;
	}
	m_pItemTable->m_iTableType = OBJECTCELLTYPE_MAIL;
	m_pItemTable->m_iTableID = 0;

	m_pToNameEditBox = GetEditBox(IDC_SENDTO_EDITBOX);
	m_pDingEditBox = GetEditBox(IDC_DING_EDITBOX);
	m_pLiangEditBox = GetEditBox(IDC_LIANG_EDITBOX);
	m_pWenEditBox = GetEditBox(IDC_WEN_EDITBOX);
	m_pMailTextMLEditBox = GetMLEditBox(IDC_MAILTEXT_MLEDITBOX);

	stResourceLocation rl( GetGuiGraphicPackName(),0,30 );	
	m_pMoneyBmp = GetDevice()->FindBitmaps( &rl );
#ifdef __USE_GOLD
	stResourceLocation rl1( GetGuiGraphicPackName(),0,38 );	
	m_pGoldCoinBmp = GetDevice()->FindBitmaps( &rl1 );
#endif
	if( ( NULL == m_pToNameEditBox ) ||
		( NULL == m_pDingEditBox ) ||
		( NULL == m_pLiangEditBox ) ||
		( NULL == m_pWenEditBox ) ||
		( NULL == m_pMailTextMLEditBox ) )
	{
		GetGameGuiManager()->AddMessageBox("发件箱脚本出错");
		Engine_WriteLog("发件箱对话脚本出错");
		Close();
		return ;
	}

	m_rcSendMoney = stRectI(138,320,173,355);
	m_rcSendGoldCoin = stRectI(188,320,223,355);

	SendMailDlg_EditSetVisible(IDC_DING_EDITBOX,false);

	//默认为普通邮件
	OnClickCommCheck();

	FUNCTION_END;
}

bool CGuiSendMailDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case 2 : return OnClickCommCheck();
		case 3 : return OnClickPaybackCheck();
		case 4 : return OnClickItem();
		case 5 : return OnClickGold();
		case 6 : return OnClickMoney();
		case 7 : return OnSendMail();
		case 8 : return OnClearMail();
		case 9 : return OnSwitchToRecvMailDlg();
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiSendMailDialog::OnClose(void)
{
	FUNCTION_BEGIN;
	GetGameGuiManager()->m_guiSendMailDlg = NULL;
	if( m_pMoneyInputDlg )
	{
		m_pMoneyInputDlg->Close();
		m_pMoneyInputDlg = NULL;
	}
	if( m_pGoldCoinInputDlg )
	{
		m_pGoldCoinInputDlg->Close();
		m_pGoldCoinInputDlg = NULL;
	}
	OnClearMail();
	FUNCTION_END;
}

HRESULT CGuiSendMailDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	stRectI rcSendMoney =  m_rcSendMoney;
	rcSendMoney.OffsetRect(m_x,m_y);

	stRectI rcSendGoldCoin =  m_rcSendGoldCoin;
	rcSendGoldCoin.OffsetRect(m_x,m_y);

	int iFont = GetDevice()->SetFont(1);
	stRectI rcTemp = g_sPostageRect;
	rcTemp.OffsetRect(m_x,m_y);
	GetDevice()->DrawString(g_szPostageOfSendMail,rcTemp);

	if( m_iSendMoney > 0 && NULL != m_pMoneyBmp)
	{
		m_pMoneyBmp->Render(0,rcSendMoney.left,rcSendMoney.top,NULL,NULL,-1,Blend_Null);	
	}
#ifdef __USE_GOLD
	if( m_iSendGoldCoin > 0 && NULL != m_pGoldCoinBmp )
	{
		m_pGoldCoinBmp->Render(0,rcSendGoldCoin.left,rcSendGoldCoin.top,NULL,NULL,-1,Blend_Null);
	}
#endif

	GetDevice()->SetFont(iFont);

	m_toolTips.Clear();
	if( rcSendMoney.PtInRect(ptCursor) )
	{
		m_toolTips.SetBorderColor(0);
		m_toolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
		m_toolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		m_toolTips.SetAlignType( GUI_ALIGN_LEFT );
		char szText[64] = {'0'};
		sprintf(szText,"附加的银子: %s ",Money2String(m_iSendMoney));
		m_toolTips.AddText(szText);
		m_toolTips.Resize();
		stRectI rc = m_rcSendMoney;
		rc.OffsetRect(m_x,m_y);
		m_toolTips.RenderTopLeftPrefer(rc,GetDevice()->GetWidth(),GetDevice()->GetHeight());
	}
#ifdef __USE_GOLD
	else if( rcSendGoldCoin.PtInRect(ptCursor) )
	{
		m_toolTips.SetBorderColor(0);
		m_toolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
		m_toolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		m_toolTips.SetAlignType( GUI_ALIGN_LEFT );
		char szText[64] = {'0'};
		sprintf(szText,"附加的金子: %s ",Money2String(m_iSendGoldCoin));
		m_toolTips.AddText(szText);
		m_toolTips.Resize();
		stRectI rc = m_rcSendGoldCoin;
		rc.OffsetRect(m_x,m_y);
		m_toolTips.RenderTopLeftPrefer(rc,GetDevice()->GetWidth(),GetDevice()->GetHeight());
	}
#endif
	return hr;
	FUNCTION_END;
}

void CGuiSendMailDialog::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());

		m_pBtnClose->SetLocation(pt.x ,pt.y + 24);
	}

	FUNCTION_END;
}

bool CGuiSendMailDialog::OnSendMail()
{
	FUNCTION_BEGIN;

	if( ( NULL == m_pToNameEditBox ) ||
		( NULL == m_pDingEditBox ) ||
		( NULL == m_pLiangEditBox ) ||
		( NULL == m_pWenEditBox ) ||
		( NULL == m_pMailTextMLEditBox ) )
	{
		GetGameGuiManager()->AddMessageBox("发件箱脚本出错");
		Engine_WriteLog("发件箱对话脚本出错");
		Close();
		return true;
	}

	stSendMail cmd;
	const char* pszToName = m_pToNameEditBox->GetText();
	if( NULL != pszToName && strlen(pszToName) > 0 )
	{
		strcpy( cmd.toName,pszToName );
	}
	else
	{
		GetGameGuiManager()->AddMessageBox("名字不能为空");
		return true;
	}
	
	bool bCheckCommMail = false;
	SendMailDlg_ButtonGetCheck(2,bCheckCommMail);
	if( bCheckCommMail )
	{
		cmd.recvMoney = 0;
		cmd.recvGold = 0;
	}
	else
	{
		char szTxt[64]={0};
		sprintf(szTxt,"%d%s%d%s%d%s",strtoul(m_pDingEditBox->GetText(),NULL,10),INGOT_TXT,
			strtoul(m_pLiangEditBox->GetText(),NULL,10),TAEL_TXT,
			strtoul(m_pWenEditBox->GetText(),NULL,10),CENT_TXT);

		cmd.recvMoney = String2Money(szTxt);
#ifdef __USE_GOLD
		sprintf(szTxt,"%d%s%d%s%d%s",strtoul(GetEditBox(18)->GetText(),NULL,10),INGOT_TXT,
			strtoul(GetEditBox(17)->GetText(),NULL,10),TAEL_TXT,
			strtoul(GetEditBox(16)->GetText(),NULL,10),CENT_TXT);
		cmd.recvGold = String2Money(szTxt);
#else
		cmd.recvGold = 0;
#endif
	}	

	cmd.itemID = INVALID_THISID;
	CRoleItem* pItem = GetCellItem(m_pItemTable,stPointI(0,0));
	if( pItem )
	{
		cmd.itemID = pItem->GetThisID();
	}

	cmd.sendMoney = m_iSendMoney;
	cmd.sendGold = m_iSendGoldCoin;

	const char* pszMailTxt = m_pMailTextMLEditBox->GetText();
	if( NULL != pszMailTxt && strlen(pszMailTxt) > 0 )
	{
		_snprintf(cmd.text,256,pszMailTxt);
		_snprintf(cmd.title,MAX_NAMESIZE,"");
	}	
	SEND_USER_CMD(cmd);
	m_iSendMoney = 0;
	m_iSendGoldCoin = 0;
	OnClearMail();

	GetButton(7)->SetEnabled(false);
	GetButton(7)->SetEnableDelay(true,1000);

	return true;
	FUNCTION_END;
}

bool CGuiSendMailDialog::OnClearMail()
{
	FUNCTION_BEGIN;

	if( NULL != m_pMailTextMLEditBox )
	{
		m_pMailTextMLEditBox->SetText("");
	}

	//清除物品(镜像)
	CRoleItem* pItem = GetCellItem(m_pItemTable,stPointI(0,0));
	if( pItem  )
	{
		pItem->OnRemove();
		delete pItem;
		pItem = NULL;
	}

	m_iSendMoney = 0;
	m_iSendGoldCoin = 0;

	SendMailDlg_EditClear(IDC_SENDTO_EDITBOX);
	SendMailDlg_EditClear(IDC_WEN_EDITBOX);
	SendMailDlg_EditClear(IDC_LIANG_EDITBOX);
	SendMailDlg_EditClear(IDC_DING_EDITBOX);
	SendMailDlg_EditClear(16);
	SendMailDlg_EditClear(17);
	SendMailDlg_EditClear(18);

	return true;
	FUNCTION_END;
}

bool CGuiSendMailDialog::OnClickItem()
{
	FUNCTION_BEGIN;

	CRoleItem* pRoleItem = CRoleItem::GetFloatRoleItem();
	if( NULL != pRoleItem )
	{
		return true;
	}
	stResourceLocation rl("data\\cursor.gl",0,2);
	GetGameCursor()->SetCursorTypeEx(eCursorGetMailItem);
	GetGameCursor()->SetImage( &rl, stPointI(1,1) );

	return true;
	FUNCTION_END;
}

bool CGuiSendMailDialog::OnClickGold()
{
	FUNCTION_BEGIN;
#ifdef __USE_GOLD
	if( m_pGoldCoinInputDlg )
	{
		m_pGoldCoinInputDlg->Close();
	}
	m_pGoldCoinInputDlg = GetGameGuiManager()->AddMoneyInputBox("请输入你要送多少金子",eAction_InputMailSendGoldCoin);
#endif

	return true;
	FUNCTION_END;
}

bool CGuiSendMailDialog::OnClickMoney()
{
	FUNCTION_BEGIN;

	if( m_pMoneyInputDlg )
	{
		m_pMoneyInputDlg->Close();
	}
	m_pMoneyInputDlg = GetGameGuiManager()->AddMoneyInputBox("请输入你要送多少银子",eAction_InputMailSendMoney);

	return true;
	FUNCTION_END;
}

bool CGuiSendMailDialog::OnSwitchToRecvMailDlg()
{
	FUNCTION_BEGIN;

	Close();
	GetGameGuiManager()->AddRecvMailDlg();
	return true;
	FUNCTION_END;
}

bool CGuiSendMailDialog::OnClickCommCheck()
{	
	FUNCTION_BEGIN;

	SendMailDlg_ButtonSetCheck(2,true);
	SendMailDlg_ButtonSetCheck(3,false);

	SendMailDlg_Edit_ClearAndHide(IDC_WEN_EDITBOX);
	SendMailDlg_Edit_ClearAndHide(IDC_LIANG_EDITBOX);
	SendMailDlg_Edit_ClearAndHide(IDC_DING_EDITBOX);

	SendMailDlg_Edit_ClearAndHide(16);
	SendMailDlg_Edit_ClearAndHide(17);
	SendMailDlg_Edit_ClearAndHide(18);

	return true;
	FUNCTION_END;
}

bool CGuiSendMailDialog::OnClickPaybackCheck()
{
	FUNCTION_BEGIN;

	SendMailDlg_ButtonSetCheck(3,true);
	SendMailDlg_ButtonSetCheck(2,false);
	
	SendMailDlg_EditSetVisible(IDC_WEN_EDITBOX,true);
	SendMailDlg_EditSetVisible(IDC_LIANG_EDITBOX,true);
	SendMailDlg_EditSetVisible(IDC_DING_EDITBOX,true);

	SendMailDlg_EditSetVisible(16,true);
	SendMailDlg_EditSetVisible(17,true);
	SendMailDlg_EditSetVisible(18,true);

	return true;
	FUNCTION_END;
}

void CGuiSendMailDialog::SetSendToName(const char* toName)
{
	FUNCTION_BEGIN;
	if( NULL != m_pToNameEditBox )
	{
		m_pToNameEditBox->SetText(toName);
	}
	FUNCTION_END;
}

void CGuiSendMailDialog::SetSendMoney(int iMoney)
{
	FUNCTION_BEGIN;
	m_iSendMoney = iMoney;
	m_pMoneyInputDlg = NULL;
	FUNCTION_END;
}

void CGuiSendMailDialog::SetSendGoldCoin(int iGold)
{
	FUNCTION_BEGIN;
	m_iSendGoldCoin = iGold;
	m_pGoldCoinInputDlg = NULL;
	FUNCTION_END;
}
