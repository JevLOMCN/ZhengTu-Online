#include "public.h"
#include "miniusercommand.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "LordStrikeManager.h"
#include "./LSChangeMoneyDlg.h"
#include "roleitem.h"
#include "./MainCharacter.h"
#include "./GameScene.h"

//////////////////////////////////////////////////////////////
#define ID_TAB_DOUZI   2
#define ID_TAB_MONEY   3
#define ID_TAB         4
#define ID_STATIC_NOW_DOUZI   5 
#define ID_STATIC_NOW_MONEY   6
#define ID_BUTTON_OK          7
#define ID_BUTTON_CANCEL      8
#define ID_BUTTON_CLOSE       9


#define ID_EDITBOX_M2D   103
#define ID_STATIC_PRE_COST_MONEY    105

//#define ID_EDITBOX_INGOT    203
//#define ID_EDITBOX_TAEL     205
//#define ID_EDITBOX_CENT     207
#define ID_STATIC_PRE_COST_DOUZI 209 
#define ID_EDIT_D2M   210
//////////////////////////////////////////////////////////////

CGuiLSChangeMoneyDlg::CGuiLSChangeMoneyDlg(void)
: m_pTab(NULL)
{
	m_bSmDialog = true;
}

CGuiLSChangeMoneyDlg::~CGuiLSChangeMoneyDlg(void)
{
}

void CGuiLSChangeMoneyDlg::OnCreate(void)
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();

	m_pTab = GetTab(ID_TAB);
	if(m_pTab)
		m_pTab->SetVisible(false);


	OnGuiEvent(EVENT_BUTTON_CLICKED,ID_TAB_DOUZI,NULL);

	SetNowDouziAndMoneyText();

	FUNCTION_END;
}

bool CGuiLSChangeMoneyDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		/////////////////////////////////////////////////////////////////////////
		// On Tab Button Click 
		case ID_TAB_DOUZI:
		case ID_TAB_MONEY:
			{
				if(m_pTab)
				{
					m_pTab->SetCurItem(nID - ID_TAB_DOUZI);
				}
				GetButton(nID)->SetChecked(true);
				return true;
			}
			break;
		/////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////
		case ID_BUTTON_OK : 
			return OnOK();
		case ID_BUTTON_CANCEL:
		case ID_BUTTON_CLOSE:
			Close();
			break;
		/////////////////////////////////////////////////////////////////////////
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}

HRESULT CGuiLSChangeMoneyDlg::OnRender(float fElapsedTime)
{
	/*if(m_pTab)
	{
		int nCurItem = m_pTab->GetCurItem();
		if(nCurItem == 0 )
		{
			SetPreCostMoneyText();
                    
		}
		else if(nCurItem == 1)
		{
			SetPreCostDouziText();
		}

	}*/
	return CGuiDialog::OnRender(fElapsedTime);
}

DWORD DouziToMoney(DWORD dwDouzi)
{
	return dwDouzi;
}

DWORD MoneyToDouzi(DWORD dwMoney)
{
	return dwMoney;
}

// 用钱换豆子时,想要多少豆子
DWORD CGuiLSChangeMoneyDlg::GetWantDouzi()
{
	CGuiEditBox * pEditBox = GetEditBox(ID_EDITBOX_M2D);
	if(pEditBox)
	{
		int iwantdz = strtoul(pEditBox->GetText(),NULL,10);
		return iwantdz;
	}
	return 0;
}

//// 用钱换豆子提示 需要钱数
//void CGuiLSChangeMoneyDlg::SetPreCostMoneyText()
//{
//	CGuiStatic * pStatic = GetStatic(ID_STATIC_PRE_COST_MONEY);
//	if(pStatic)
//	{
//		DWORD dwWantDz = GetWantDouzi();
//		DWORD dwMoney = DouziToMoney(dwWantDz);
//	        const char * pszMoney = Money2String(dwMoney); 
//		char szText[100];
//		_snprintf(szText,100,"你将支付 %s",pszMoney);
//		szText[99] = 0;
//		pStatic->SetText(szText);
//	}	
//}

// 用豆子换钱时,想要多少钱
DWORD CGuiLSChangeMoneyDlg::GetWantMoney()
{
	
	CGuiEditBox * pEditBox = GetEditBox(ID_EDIT_D2M);
	if(pEditBox)
	{
		int iwantdz = strtoul(pEditBox->GetText(),NULL,10);
		return iwantdz;
	}
	/*CGuiEditBox * pEditBoxIngot = GetEditBox(ID_EDITBOX_INGOT);
	CGuiEditBox * pEditBoxTael = GetEditBox(ID_EDITBOX_TAEL);
	CGuiEditBox * pEditBoxCent = GetEditBox(ID_EDITBOX_CENT);
	if(pEditBoxIngot && pEditBoxTael && pEditBoxCent)
	{
		char szTxt[32]={0};
		sprintf(szTxt,"%d%s%d%s%d%s",
			strtoul(pEditBoxIngot->GetText(),NULL,10),INGOT_TXT,
			strtoul(pEditBoxTael->GetText(),NULL,10),TAEL_TXT,
			strtoul(pEditBoxCent->GetText(),NULL,10),CENT_TXT);
		szTxt[31] = 0;
		DWORD dwMoney = String2Money(szTxt); 
		return dwMoney;
	}*/
	return 0;
}


//// 用豆子换钱提示 需要豆子数
//void CGuiLSChangeMoneyDlg::SetPreCostDouziText()
//{
//	CGuiStatic * pStatic = GetStatic(ID_STATIC_PRE_COST_DOUZI);
//	if(pStatic)
//	{
//		DWORD dwMoney = GetWantMoney();
//	        DWORD dwDouzi = MoneyToDouzi(dwMoney);
//		char szText[40] ={0};
//		_snprintf(szText,40,"你将支付 %d粒",dwDouzi);
//		szText[39] = 0;
//		pStatic->SetText(szText);
//		
//	}
//}

// 现有钱和豆子数
void CGuiLSChangeMoneyDlg::SetNowDouziAndMoneyText()
{
      CGuiStatic * pNowDouzi = GetStatic(ID_STATIC_NOW_DOUZI);
      CGuiStatic * pNowMoney = GetStatic(ID_STATIC_NOW_MONEY);
      if(pNowDouzi && pNowMoney)
      {
	      char szText[100];
	      //银子
	      int iMoney = GetScene()->GetMainCharacter()->GetMoney();
	      const char* pszMoney = Money2String(iMoney);
	      _snprintf(szText,100,"现有银子 %s",pszMoney);
	      szText[99] = 0;
	      pNowMoney->SetText(szText);

	     
	      // 豆子
	      stMainUserInfo&  mainuserinfo = CLordStrikeManager::GetSingleton()->GetMainUserInfo();
	      int iDouzi = mainuserinfo.userdata.score.money;
	      _snprintf(szText,100,"现有仙宿丹 %d粒",iDouzi);
	      szText[99] = 0;
	      pNowDouzi->SetText(szText);
      }
}

void CGuiLSChangeMoneyDlg::OnClose(void)
{
        CLordStrikeManager::GetSingleton()->m_guiLSChangeMoneyDlg = NULL;
}

bool CGuiLSChangeMoneyDlg::OnOK()
{
	if(m_pTab)
	{
		int nCurItem = m_pTab->GetCurItem();
		if(nCurItem == 0 )  // money to douzi (I have money, and I want douzi)
		{
		     DWORD dwWantDz = GetWantDouzi();
		     DWORD dwMoney = DouziToMoney(dwWantDz);
		     if( GetScene()->GetMainCharacter()->GetMoney() < dwMoney)
		     {
                               GameMessageBox("你没有这么多钱来换仙宿丹!");
			       Close();
			       return true;
		     }
		     stDepositCommonMiniGameCmd cmd;		
		     cmd.num = dwWantDz;
		     SEND_USER_CMD(cmd);
		}
		else if(nCurItem == 1) // douzi to money (I have Douzi,and I want money)
		{
			DWORD dwMoney = GetWantMoney();
			DWORD dwDouzi = MoneyToDouzi(dwMoney);

			DWORD dwNowDouzi = CLordStrikeManager::GetSingleton()->GetMainUserInfo().userdata.score.money;
			if(dwNowDouzi <  dwDouzi)
			{
				GameMessageBox("你没有这么多仙宿丹来提取!");
				Close();
				return true;
			} 
			stDrawCommonMiniGameCmd cmd;
			cmd.num = dwMoney;
			SEND_USER_CMD(cmd);
		}
		else 
		{
			assert(0);
		}
	}
	Close();
	return true;
}