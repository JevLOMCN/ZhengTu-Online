#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GuiRecvMailDialog.h"
#include "./GuiSendMailDialog.h"
#include "./RoleItem.h"
#include "./GuiMessageBox.h"
#include "GameScene.h"
#include "MainCharacter.h"

#define IDC_MAIL_TXT	1
#define IDC_RECV_ITEM	2
#define IDC_OPEN_MAIL	3
#define IDC_WRITE_BACK	4
#define IDC_SEND_BACK	5
#define IDC_PRE_PAGE	6
#define IDC_NEXT_PAGE	7
#define IDC_SWITCH_TO_SEND	8
#define IDC_MAIL_LIST	9
#define IDC_DELETE_MAIL	11

#define SYSTEM_MAIL_COLOR	COLOR_ARGB(255,0,255,0)

const static int g_iPageMailNum = 6;

const static stRectI g_scCurrentPageRect(140,513,200,533);
const static stRectI g_scTotalPageRect(216,513,238,533);

#define RecvMailDlg_ButtonSetEnable(id,bEnable)	{CGuiButton* pBtn = GetButton(id);if( NULL != pBtn ){ pBtn->SetEnabled(bEnable);}}
#define RecvMailDlg_ButtonSetVisible(id,bShow)	{CGuiButton* pBtn = GetButton(id);if( NULL != pBtn ){ pBtn->SetVisible(bShow);}}

CGuiRecvMailDialog::CGuiRecvMailDialog(void)
{
	FUNCTION_BEGIN;
	m_pMoneyBmp = NULL;				//银子显示
	m_pGoldCoinBmp = NULL;			//金子显示
	m_pNoMoneyBmp = NULL;			//银子显示(没有银子)
	m_pNoGoldCoinBmp = NULL;		//金子显示(没有金子)   

	m_iCurPage = 0;			//当前页
	m_iTotalPage = 0;		//总共页数

	m_iSendToMeMoney = 0;
	m_iSendToMeGoldCoin = 0;

	m_iPayBackMoney = 0;
	m_iPayBackGoldCoin = 0;

	m_rcSendMoney = stRectI(0,0,0,0);
	m_rcSendGoldCoin = stRectI(0,0,0,0);

	m_pMailMLTextBox = NULL;
	m_pMailList = NULL;
	m_dwCurMailId = 0;

	m_pConfirmRecvItem = NULL;

	m_pNotReadMailBmp = NULL;
	m_pReadedMailBmp = NULL;
}

CGuiRecvMailDialog::~CGuiRecvMailDialog(void)
{
	FUNCTION_BEGIN;
	if( NULL != m_pItemTable )
	{
		CRoleItem* pItem = GetCellItem(m_pItemTable,stPointI(0,0));
		if( NULL != pItem)
		{
			pItem->OnRemove();
			delete pItem;
		}
	}
}

void CGuiRecvMailDialog::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	m_pItemTable = GetTable(10);
	if( NULL == m_pItemTable )
	{
		GetGameGuiManager()->AddMessageBox("收件箱脚本出错");
		Engine_WriteLog("发件箱对话脚本出错");
		Close();
		return ;
	}
	m_pItemTable->m_iTableType = OBJECTCELLTYPE_MAIL;
	m_pItemTable->m_iTableID = 1;

	m_rcSendMoney = stRectI(138,323,175,353);
	m_rcSendGoldCoin = stRectI(186,323,222,353);

	m_pMailMLTextBox = GetMLTextBox(IDC_MAIL_TXT);
	m_pMailList = GetListBox(IDC_MAIL_LIST);

	if( NULL == m_pMailMLTextBox || NULL == m_pMailList )
	{
		GetGameGuiManager()->AddMessageBox("收件箱脚本出错");
		Engine_WriteLog("收件箱脚本出错");
		Close();
	}

	stResourceLocation rl( GetGuiGraphicPackName(),51,25 );	
	m_pNotReadMailBmp = GetDevice()->FindBitmaps( &rl );

	stResourceLocation rl1( GetGuiGraphicPackName(),51,26 );	
	m_pReadedMailBmp = GetDevice()->FindBitmaps( &rl1 );	

	RecvMailDlg_ButtonSetEnable(IDC_PRE_PAGE,false);
	RecvMailDlg_ButtonSetEnable(IDC_NEXT_PAGE,false);
	RecvMailDlg_ButtonSetEnable(IDC_RECV_ITEM,false);
	RecvMailDlg_ButtonSetEnable(IDC_OPEN_MAIL,false);
	RecvMailDlg_ButtonSetEnable(IDC_DELETE_MAIL,false);

	m_iCurPage = m_iTotalPage = 0;

	stResourceLocation rl2( GetGuiGraphicPackName(),0,30 );	
	m_pMoneyBmp = GetDevice()->FindBitmaps( &rl2 );
#ifdef __USE_GOLD
	stResourceLocation rl3( GetGuiGraphicPackName(),0,38 );	
	m_pGoldCoinBmp = GetDevice()->FindBitmaps( &rl3 );
#endif

	UpdateCtrlState();

	FUNCTION_END;
}

bool CGuiRecvMailDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case IDC_RECV_ITEM: return OnRecvItem();
		case IDC_OPEN_MAIL: return OnOpenMail();
		case IDC_WRITE_BACK: return OnWriteBack();
		case IDC_SEND_BACK: return OnSendBack();
		case IDC_PRE_PAGE: return OnPreBtn();
		case IDC_NEXT_PAGE: return OnNextBtn();
		case IDC_SWITCH_TO_SEND: return OnSwitchToSendMailDlg();
		case IDC_DELETE_MAIL: return OnDeleteMail();
		}
	}
	if( nEvent == EVENT_LISTBOX_ITEM_DBLCLK )
	{
		if ( nID == IDC_MAIL_LIST )
		{
			int curIndex = m_pMailList->GetCurItem();
			DWORD dwMailId = GetMailIDFromList(curIndex);
			if( 0 != dwMailId )
			{
				GetScene()->GetMainCharacter()->GetMailManager()->OpenMail(dwMailId);
			}
			return true;
		}
	}
	if( nEvent == EVENT_LISTBOX_SELECTION)
	{
		if ( nID == IDC_MAIL_LIST )
		{
			RecvMailDlg_ButtonSetEnable(IDC_DELETE_MAIL,false);
			int curIndex = m_pMailList->GetCurItem();
			DWORD dwMailId = GetMailIDFromList(curIndex);
			RecvMailDlg_ButtonSetEnable(IDC_OPEN_MAIL,(dwMailId!=0));
			if( dwMailId != 0 )
			{				
				CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
				if( pMainRole )
				{
					tMailList::iterator it = pMainRole->GetMailManager()->GetMailTitle(dwMailId);
					if( it != pMainRole->GetMailManager()->GetMailList()->end() )
					{
						RecvMailDlg_ButtonSetEnable(IDC_DELETE_MAIL,!it->accessory);
					}
				}
			}
		}
		return true;
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiRecvMailDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	if( NULL != m_pItemTable )
	{
		CRoleItem* pItem = GetCellItem(m_pItemTable,stPointI(0,0));
		if( NULL != pItem)
		{
			pItem->OnRemove();
			delete pItem;
		}
	}

	bool bHaveNewMail = false;
	tMailList* pList = GetScene()->GetMainCharacter()->GetMailManager()->GetMailList();
	for ( tMailList::iterator it = pList->begin(); it != pList->end(); ++it )
	{
		if ( it->state == 1 )	//1 表示未读  2表示已读
		{
			bHaveNewMail = true;
			break;
		}
	}
	if ( !bHaveNewMail )
	{
		GetScene()->GetMainCharacter()->GetMailManager()->SetNewMailNotify(false);
	}	

	pList->clear();

	if ( NULL != m_pConfirmRecvItem )
	{
		m_pConfirmRecvItem->Close();
	}
	GetGameGuiManager()->m_guiRecvMailDlg = NULL;
	FUNCTION_END;
}

HRESULT CGuiRecvMailDialog::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	stRectI rcSendMoney =  m_rcSendMoney;
	rcSendMoney.OffsetRect(m_x,m_y);

	stRectI rcSendGoldCoin =  m_rcSendGoldCoin;
	rcSendGoldCoin.OffsetRect(m_x,m_y);

	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	m_toolTips.Clear();
	if( rcSendMoney.PtInRect(ptCursor) )
	{
		m_toolTips.SetBorderColor(0);
		m_toolTips.SetBkColor(COLOR_ARGB(128,0,0,0));
		m_toolTips.SetCurColor(COLOR_ARGB(255,255,255,255));
		m_toolTips.SetAlignType( GUI_ALIGN_LEFT );
		char szText[64] = {'0'};
		sprintf(szText,"附加的银子: %s ",Money2String(m_iSendToMeMoney));
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
		sprintf(szText,"附加的金子: %s ",Money2String(m_iSendToMeGoldCoin));
		m_toolTips.AddText(szText);
		m_toolTips.Resize();
		stRectI rc = m_rcSendGoldCoin;
		rc.OffsetRect(m_x,m_y);
		m_toolTips.RenderTopLeftPrefer(rc,GetDevice()->GetWidth(),GetDevice()->GetHeight());
	}
#endif
	int iFont = GetDevice()->SetFont(1);
	stRectI rcTemp = g_scCurrentPageRect;
	rcTemp.OffsetRect(m_x,m_y);
	char szTemp[32] = {'0'};
	sprintf(szTemp,"%d",m_iCurPage+1);
	GetDevice()->DrawString(szTemp,rcTemp);

	stRectI rcTemp1 = g_scTotalPageRect;
	rcTemp1.OffsetRect(m_x,m_y);
	sprintf(szTemp,"%d",m_iTotalPage+1);
	GetDevice()->DrawString(szTemp,rcTemp1);
	GetDevice()->SetFont(iFont);

	if( NULL != m_pMoneyBmp && m_iSendToMeMoney > 0 )
	{
		m_pMoneyBmp->Render(0,rcSendMoney.left,rcSendMoney.top,NULL,NULL,-1,Blend_Null);
	}
#ifdef __USE_GOLD
	if( NULL != m_pGoldCoinBmp && m_iSendToMeGoldCoin > 0 )
	{
		m_pGoldCoinBmp->Render(0,rcSendGoldCoin.left,rcSendGoldCoin.top,NULL,NULL,-1,Blend_Null);
	}
#endif
	return hr;
	FUNCTION_END;
}

void CGuiRecvMailDialog::OnSetCloseLocation()
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

inline int EndTimeSecondToDay(DWORD dwEndTime)
{
	return dwEndTime / 86400;
}

void CGuiRecvMailDialog::UpdateMailListText()
{
	FUNCTION_BEGIN;
	if( NULL != m_pMailList )
	{
		m_pMailList->RemoveAllItems();
		int index = 0;
		tMailList* list = GetScene()->GetMainCharacter()->GetMailManager()->GetMailList();		
		if( NULL != list )
		{
			if( list->empty() )
			{
				m_iCurPage = 0;
				m_iTotalPage = 0;
				return ;
			}
			m_iTotalPage = (list->size()-1) / g_iPageMailNum;

			//防止当前页翻到最后面去了
			if( m_iCurPage > m_iTotalPage )
			{
				m_iCurPage = m_iTotalPage;
			}

			for( tMailList::iterator it = list->begin(); it != list->end(); ++it )
			{
				int iMinMail = m_iCurPage * g_iPageMailNum;
				int iMaxMail = (m_iCurPage + 1) * g_iPageMailNum;
				if( index >= iMinMail && index < iMaxMail )
				{
					DWORD color = -1;
					int frame = 25;
					char szTemp[MAX_PATH] = {'0'};
					if( it->state == 2)
					{
						frame = 26;
					}
					int day = EndTimeSecondToDay(it->endTime);
					char sz[64];
					if( day < 1 )
					{
						sprintf(sz,"%s","不足一天");
					}
					else
					{
						if( day > 7 ) 
						{
							day = 7;
						}
						sprintf(sz,"%d 天",day);
					}
					if( it->accessory )
					{
						sprintf(szTemp,"%d.%s %s %s",index+1,it->fromName,"(有物品)",sz);
					}
					else
					{
						sprintf(szTemp,"%d.%s  %s",index+1,it->fromName,sz);
					}

					if( day < 3 )
					{
						color = 0xffff0000;	
					}
					m_pMailList->AddItem("",(void*)(it->id));					
					m_pMailList->SetItemText(m_pMailList->GetItemCount()-1,0,"",3,GuiListBoxColFmt_Left);
					m_pMailList->SetItemBmp(m_pMailList->GetItemCount()-1,0,&(stResourceLocation("data\\interfaces.gl",51,frame)));	
					m_pMailList->SetItemText(m_pMailList->GetItemCount()-1,1,szTemp,strlen(szTemp),GuiListBoxColFmt_Right);
					m_pMailList->SetItemColor(m_pMailList->GetItemCount()-1,1,color);
				}
				++index ;
			}			
		}
	}
	FUNCTION_END;
}

void CGuiRecvMailDialog::UpdatePageButtonState()
{
	FUNCTION_BEGIN;
	RecvMailDlg_ButtonSetEnable(IDC_PRE_PAGE,true);
	RecvMailDlg_ButtonSetEnable(IDC_NEXT_PAGE,true);
	if(m_iCurPage == 0)
	{
		RecvMailDlg_ButtonSetEnable(IDC_PRE_PAGE,false);
		if( m_iCurPage == m_iTotalPage )
		{
			RecvMailDlg_ButtonSetEnable(IDC_PRE_PAGE,false);
			RecvMailDlg_ButtonSetEnable(IDC_NEXT_PAGE,false);
		}
	}
	else if(m_iCurPage == m_iTotalPage)
	{
		RecvMailDlg_ButtonSetEnable(IDC_NEXT_PAGE,false);
	}
	FUNCTION_END;
}

void CGuiRecvMailDialog::UpdateMailList()
{
	FUNCTION_BEGIN;
	if( NULL != m_pMailList )
	{
		m_dwCurMailId = 0;

		UpdateMailListText();
		UpdatePageButtonState();	
		UpdateCtrlState();
	}
	FUNCTION_END;
}

DWORD CGuiRecvMailDialog::GetMailIDFromList(int index)
{
	FUNCTION_BEGIN;
	if( NULL != m_pMailList )
	{
		if( index >= 0 && index < m_pMailList->GetItemCount() )
		{
			return (DWORD)(m_pMailList->GetItemData(index));
		}		
	}
	return 0;
	FUNCTION_END;
}

bool CGuiRecvMailDialog::OnOpenMail()
{
	FUNCTION_BEGIN;
	if( NULL != m_pMailList )
	{
		int curIndex = m_pMailList->GetCurItem();
		DWORD dwMailId = GetMailIDFromList(curIndex);
		if( 0 != dwMailId )
		{
			GetScene()->GetMainCharacter()->GetMailManager()->OpenMail(dwMailId);
		}
	}
	return true;
	FUNCTION_END;
}

bool CGuiRecvMailDialog::IsSystemMail()
{
	FUNCTION_BEGIN;
	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
	if( pMainRole )
	{
		return pMainRole->GetMailManager()->IsSystemMail(m_dwCurMailId);
	}
	return false;
}

bool CGuiRecvMailDialog::ParseMailContent(stContentMail* cmd)
{
	FUNCTION_BEGIN;
	//收到邮件详细信息
	m_dwCurMailId = cmd->mailID;
	if(NULL != m_pMailMLTextBox)
	{
		if( IsSystemMail() )
		{
			m_pMailMLTextBox->SetText("");
			m_pMailMLTextBox->AppendParagraph(cmd->text,SYSTEM_MAIL_COLOR);
		}
		else
		{
			m_pMailMLTextBox->SetText(cmd->text);
		}		
	}
	RecvMailDlg_ButtonSetEnable(IDC_RECV_ITEM,cmd->accessory);
	RecvMailDlg_ButtonSetEnable(IDC_SEND_BACK,cmd->accessory);

	m_iSendToMeMoney = cmd->sendMoney;		//指发件人送出的银子
	m_iSendToMeGoldCoin = cmd->sendGold;	//指发件人送出的金子

	m_iPayBackMoney = cmd->recvMoney;		//指发件人输入的应该返还给发件人的银子
	m_iPayBackGoldCoin = cmd->recvGold;		//指发件人输入的应该返还给发件人的金子

	if( NULL != m_pItemTable )		
	{
		CRoleItem* pItem = GetCellItem(m_pItemTable,stPointI(0,0));
		if( NULL != pItem)
		{
			pItem->OnRemove();
			delete pItem;
		}
		if( cmd->item.dwObjectID != 0 )
		{
			pItem = new CRoleItem();
			if( NULL != pItem)
			{
				t_Object* pObj = &(cmd->item);
				pObj->pos.dwLocation = OBJECTCELLTYPE_MAIL;
				pObj->pos.dwTableID = 1;
				pObj->pos.x = 0;
				pObj->pos.y = 0;
				pItem->SetObject(pObj);
			}
		}
	}
	UpdateMailListItem(cmd->mailID);
	//UpdateMailListText();
	UpdateCtrlState();	//刷新个控件的状态

	return true;
	FUNCTION_END;
}

bool CGuiRecvMailDialog::OnSendBack()
{
	FUNCTION_BEGIN;
	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
	if( pMainRole )
	{
		pMainRole->GetMailManager()->SendBackMail(m_dwCurMailId);
	}
	return true;
	FUNCTION_END;
}


bool CGuiRecvMailDialog::OnWriteBack()
{
	FUNCTION_BEGIN;
	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
	if( pMainRole )
	{
		CGuiSendMailDialog* pSendMailDlg = GetGameGuiManager()->AddSendMailDlg();
		if( NULL != pSendMailDlg )
		{
			const char* pszFromName = pMainRole->GetMailManager()->GetMailFromName(m_dwCurMailId);
			if( NULL != pszFromName && strlen(pszFromName) > 0 )
			{
				pSendMailDlg->SetSendToName(pszFromName);
			}
		}				
	}
	return true;
	FUNCTION_END;
}

bool CGuiRecvMailDialog::OnDeleteMail()
{
	FUNCTION_BEGIN;
	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
	if( pMainRole )
	{
		if( NULL != m_pMailList )
		{
			int curIndex = m_pMailList->GetCurItem();
			DWORD dwMailId = GetMailIDFromList(curIndex);
			if ( dwMailId != 0 )
			{
				pMainRole->GetMailManager()->DeleteMail(dwMailId);
			}			
		}
	}
	return true;
	FUNCTION_END;
}

bool CGuiRecvMailDialog::OnRecvItem()
{
	FUNCTION_BEGIN;
	char szTemp[MAX_PATH] = {'0'};
	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
	if( pMainRole )
	{
		const char* pszFromName = pMainRole->GetMailManager()->GetMailFromName(m_dwCurMailId);
		if( NULL != pszFromName && strlen(pszFromName) > 0 )
		{
			if( m_iPayBackMoney == 0 && m_iPayBackGoldCoin == 0 )
			{
				OnConfirmReceiveItem(m_dwCurMailId,true);
				return true;
			}
			int iMoney = pMainRole->GetMoney();
			int iGoldCoin  = pMainRole->GetGoldNum();

			char pszMoney[64] = {0};
			_snprintf(pszMoney,64,"%s",Money2String(m_iPayBackMoney));
			pszMoney[63] = 0;
#ifdef __USE_GOLD
			char pszGold[64] = {0};
			_snprintf(pszGold,64,"%s",Money2String(m_iPayBackGoldCoin));
			pszGold[63] = 0;
		
			if( iMoney >= m_iPayBackMoney && iGoldCoin >= m_iPayBackGoldCoin )
			{
				sprintf(szTemp,"收取这封信需要支付给发信人 %s 银子: %s 金子:%s",pszFromName,pszMoney,pszGold);
				m_pConfirmRecvItem = GetGameGuiManager()->AddMessageBox(szTemp,eAction_ConfirmReceiveMailItem,(void *)(m_dwCurMailId),c_szMBOKCancel);
			}
			else
			{
				if( iMoney < m_iPayBackMoney )
				{
					sprintf(szTemp,"你的钱不够,%s 要求支付的银子为: %s ",pszFromName,pszMoney);
					GetGameGuiManager()->AddClientSystemMessage(szTemp);
				}
				if( iGoldCoin < m_iPayBackGoldCoin )
				{
					sprintf(szTemp,"你的金子不够,%s 要求支付的金子为: %s ",pszFromName,pszGold);
					GetGameGuiManager()->AddClientSystemMessage(szTemp);
				}
			}
#else
			if( iMoney >= m_iPayBackMoney )
			{
				sprintf(szTemp,"收取这封信需要支付给发信人 %s 银子: %s ",pszFromName,pszMoney);
				m_pConfirmRecvItem = GetGameGuiManager()->AddMessageBox(szTemp,eAction_ConfirmReceiveMailItem,(void *)(m_dwCurMailId),c_szMBOKCancel);
			}
			else
			{
				sprintf(szTemp,"你的银子不够,%s 要求支付的银子为: %s ",pszFromName,pszMoney);
				GetGameGuiManager()->AddClientSystemMessage(szTemp);
			}
#endif
		}
	}
	
	return true;
	FUNCTION_END;
}

void CGuiRecvMailDialog::OnConfirmReceiveItem(DWORD dwMailId,bool bOk)
{
	FUNCTION_BEGIN;
	if( bOk )
	{
		GetScene()->GetMainCharacter()->GetMailManager()->RecvMailItem(dwMailId);
	}
	m_pConfirmRecvItem = NULL;
	FUNCTION_END;
}

void CGuiRecvMailDialog::UpdateCtrlState()
{
	FUNCTION_BEGIN;

	if( m_dwCurMailId == 0 )
	{
		//清除信件正文
		if( NULL != m_pMailMLTextBox )
		{
			m_pMailMLTextBox->SetText("");
		}
		//清除物品
		if( NULL != m_pItemTable )
		{
			CRoleItem* pItem = GetCellItem(m_pItemTable,stPointI(0,0));
			if( NULL != pItem)
			{
				pItem->OnRemove();
				delete pItem;
			}
		}
		m_iSendToMeMoney = 0;
		m_iSendToMeGoldCoin = 0;

		m_iPayBackMoney = 0;
		m_iPayBackGoldCoin = 0;
		RecvMailDlg_ButtonSetEnable(IDC_WRITE_BACK,false);
		RecvMailDlg_ButtonSetEnable(IDC_SEND_BACK,false);
	}
	else
	{
		RecvMailDlg_ButtonSetEnable(IDC_WRITE_BACK,true);	
	}

	FUNCTION_END;
}

void CGuiRecvMailDialog::OnRemoveMail(DWORD dwMailId)
{
	FUNCTION_BEGIN;
	
	if( m_dwCurMailId == dwMailId )
	{
		m_dwCurMailId = 0; 
	}

	UpdateMailList();
	UpdateCtrlState();

	FUNCTION_END;
}

bool CGuiRecvMailDialog::OnPreBtn()
{
	FUNCTION_BEGIN;
	if ( m_iCurPage > 0 )
	{
		m_iCurPage --;
	}

	UpdateMailList();
	UpdateCtrlState();
	return true;
	FUNCTION_END;
}

bool CGuiRecvMailDialog::OnNextBtn()
{
	FUNCTION_BEGIN;
	if ( m_iCurPage <  m_iTotalPage)
	{
		m_iCurPage ++;
	}
	UpdateMailList();
	UpdateCtrlState();
	return true;
	FUNCTION_END;
}

bool CGuiRecvMailDialog::OnSwitchToSendMailDlg()
{
	FUNCTION_BEGIN;

	Close();
	GetGameGuiManager()->AddSendMailDlg();
	return true;
	FUNCTION_END;
}


void CGuiRecvMailDialog::UpdateMailListItem(DWORD dwMailID)
{
	if ( NULL != m_pMailList )
	{
		for ( int i = 0; i < m_pMailList->GetItemCount(); ++i )
		{
			if ( dwMailID == GetMailIDFromList(i) )
			{
				BYTE byState = GetScene()->GetMainCharacter()->GetMailManager()->GetMailState(dwMailID);
				if ( byState == 2 )
				{
					m_pMailList->SetItemBmp(i,0,&(stResourceLocation("data\\interfaces.gl",51,26)));	
				}
				else
				{
					m_pMailList->SetItemBmp(i,0,&(stResourceLocation("data\\interfaces.gl",51,25)));	
				}				
				break;
			}
		}
	}
}