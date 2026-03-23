#include "public.h"
#include "miniusercommand.h"
#include "./GameGuiManager.h"
#include "../Media/SoundManager.h"
#include "Game.h"
#include ".\lordstrikeManager.h"
#include ".\lordstrikeLobby.h"
#include "LordStrikeRoomDlg.h"
#include "LordStrikeLobbyDlg.h"
#include "LordStrikeGameBkDlg.h"
#include "LordStrikeGame.h"
#include "LSScoreDlg.h"
#include "LSChangeMoneyDlg.h"
#include "Chat.h"
#include "guiMain.h"
#include "LordStrikeHelpDlg.h"

#define ID_BUTTON_HIDE_LORDSTRIKE  20000

CLordStrikeManager * CLordStrikeManager::m_singleton = NULL;
static CLordStrikeManager s_LordStrikeManager;

CLordStrikeManager::CLordStrikeManager(void)
: m_guiLordStrikeLobby(NULL)
, m_guiLordStrikeRoom(NULL)
, m_guiLordStrikeGameBk(NULL)
, m_guiLSScoreDlg(NULL)
, m_guiSortDlg(NULL)
, m_guiLSChangeMoneyDlg(NULL)
, m_pGameHelpDlg(NULL)

{
	m_singleton = this;
	m_pLobby = new CLobby;
	m_pGame  = new CGame;
}

CLordStrikeManager::~CLordStrikeManager(void)
{
	delete m_pLobby;
	delete m_pGame;
	m_pLobby = NULL;
	m_pGame = NULL;
}

void CLordStrikeManager::OnUpdate()
{
	if(m_guiLordStrikeGameBk){
		if(m_pGame){
			m_pGame->Update();
		}
	}
}

void CLordStrikeManager::LogintoLobby()
{
	if(m_guiLordStrikeLobby)
	{
		if( !m_guiLordStrikeLobby->IsVisible())
		{
			OnReOpenLSDialog(ID_BUTTON_HIDE_LORDSTRIKE);                      
		}
	}
	else 
	{
		stLoginCommonMiniGameCmd cmd;
		SEND_USER_CMD(cmd);
	}
}


void CLordStrikeManager::EntryLobby(stLoginRetCommonMiniGameCmd * pCmd)
{
     if(pCmd->ret == 1)
     {
	     AddLordStrikeLobbyDlg();
	     SetGameState(eState_Lobby);
     }
     else 
     {
	     GameMessageBox("登陆大厅失败!");
     }
}

void CLordStrikeManager::LeaveLobby()
{ 
	if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeGameBk)
              CLordStrikeManager::GetSingleton()->GetLobby()->MeLeaveGame();
			
	if(CLordStrikeManager::GetSingleton()->m_guiLordStrikeRoom)
		CLordStrikeManager::GetSingleton()->GetLobby()->MeLeaveRoom();

	if(m_guiLordStrikeLobby)	
		m_guiLordStrikeLobby->Close();
}

CGuiLordStrikeLobbyDlg *  CLordStrikeManager::AddLordStrikeLobbyDlg()
{
	FUNCTION_BEGIN;

	if(!m_guiLordStrikeLobby)
	{
		CGuiLordStrikeLobbyDlg * pDlg = new CGuiLordStrikeLobbyDlg;
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\LordStrikeLobbyDlg.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_guiLordStrikeLobby = pDlg;
	}
	m_guiLordStrikeLobby->SetVisible(true);
	m_guiLordStrikeLobby->SetEnabled(true);

	return m_guiLordStrikeLobby;

	FUNCTION_END;
}


CGameBackGoundDlg *  CLordStrikeManager::AddLordStrikeGameBkDlg()
{
	FUNCTION_BEGIN;

	if(!m_guiLordStrikeGameBk)
	{
		CGameBackGoundDlg * pDlg = new CGameBackGoundDlg;
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\LordStrikeGameBkDlg.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_guiLordStrikeGameBk = pDlg;
	}
	m_guiLordStrikeGameBk->SetVisible(true);
	m_guiLordStrikeGameBk->SetEnabled(true);

	return m_guiLordStrikeGameBk;

	FUNCTION_END;
}

CGuiLordStrikeRoomDlg *  CLordStrikeManager::AddLordStrikeRoomDlg()
{
	FUNCTION_BEGIN;

	if(!m_guiLordStrikeRoom)
	{
		CGuiLordStrikeRoomDlg * pDlg = new CGuiLordStrikeRoomDlg;
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\LordStrikeRoomDlg.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_guiLordStrikeRoom = pDlg;
	}
	m_guiLordStrikeRoom->SetVisible(true);
	m_guiLordStrikeRoom->SetEnabled(true);

	return m_guiLordStrikeRoom;

	FUNCTION_END;
}


CGuiLSScoreDlg *  CLordStrikeManager::AddLordStrikeLSScoreDlg()
{
	FUNCTION_BEGIN;

	if(!m_guiLSScoreDlg)
	{
		CGuiLSScoreDlg * pDlg = new CGuiLSScoreDlg();
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\LordStrikeLSScoreDlg.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_guiLSScoreDlg = pDlg;
	}
	m_guiLSScoreDlg->SetVisible(true);
	m_guiLSScoreDlg->SetEnabled(true);

	return m_guiLSScoreDlg;

	FUNCTION_END;
}

CGuiLordStrikeSortDlg *  CLordStrikeManager::AddLordStrikeSortDlg()
{
	FUNCTION_BEGIN;

	if(!m_guiSortDlg)
	{
		CGuiLordStrikeSortDlg * pDlg = new CGuiLordStrikeSortDlg();
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\LordStrikeSortDlg.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_guiSortDlg = pDlg;
	}
	m_guiSortDlg->SetVisible(true);
	m_guiSortDlg->SetEnabled(true);

	return m_guiSortDlg;

	FUNCTION_END;
}

CGuiLSChangeMoneyDlg * CLordStrikeManager::AddLordStrikeChangeMoneyDlg()
{
        FUNCTION_BEGIN;
        
	if(!m_guiLSChangeMoneyDlg)
	{
		CGuiLSChangeMoneyDlg * pDlg = new CGuiLSChangeMoneyDlg();
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\LSChangeMoneyDlg.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_guiLSChangeMoneyDlg = pDlg;
	}
	m_guiLSChangeMoneyDlg->SetVisible(true);
	m_guiLSChangeMoneyDlg->SetEnabled(true);

	return m_guiLSChangeMoneyDlg;

	FUNCTION_END;
}



CGuiGameHelpDlg * CLordStrikeManager::AddGameHelpDlg()
{
	if(!m_pGameHelpDlg)
	{
		CGuiGameHelpDlg * pDlg = new CGuiGameHelpDlg();
		HRESULT hr = GetGuiManager()->LoadGuiFromXML(c_szGuiPack,"gui\\LSGameHelpDlg.xml",pDlg);
		if(FAILED(hr)) 
		{
			delete pDlg;
			return NULL;
		}
		m_pGameHelpDlg = pDlg;
	}
	m_pGameHelpDlg->SetVisible(true);
	m_pGameHelpDlg->SetEnabled(true);

	return m_pGameHelpDlg;
}
///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
bool CLordStrikeManager::IsInLordStrike()
{
	if( (m_guiLordStrikeRoom && m_guiLordStrikeRoom->IsVisible())
		|| (m_guiLordStrikeGameBk && m_guiLordStrikeGameBk->IsVisible()) )
	{
             return true;
	}
	return false;
}

bool CLordStrikeManager::IsLordStrikeMusicOpen()
{
	if(!m_guiLordStrikeLobby)
		return false;
	
	if(!m_guiLordStrikeLobby->IsVisible())
		return false; // open,but minize
	else 
		return true;		
}

void CLordStrikeManager::SendChat()
{
       if(m_guiLordStrikeRoom && m_guiLordStrikeRoom->IsVisible())
	       m_guiLordStrikeRoom->SendEditBox();
       else if(m_guiLordStrikeGameBk && m_guiLordStrikeGameBk->IsVisible())
	       m_guiLordStrikeGameBk->SendEditBox();
}



 
//响应小游戏对话框最小化按钮
void CLordStrikeManager::UpdateHideBtn()
{
	FUNCTION_BEGIN;

	CGuiMain* pMainDlg = GetGameGuiManager()->m_guiMain;
	if( !pMainDlg) return;

	CGuiButton * pButton = pMainDlg->GetButton(ID_BUTTON_HIDE_LORDSTRIKE);
	

	if( !m_guiLordStrikeLobby){
		if(pButton){
			pButton->SetVisible(false);
		}
		return;
	}
	if( m_guiLordStrikeLobby->IsVisible()){
		if(pButton){
			pButton->SetVisible(false);
		}
		return;
	}

	if(!pButton){
		if(FAILED( pMainDlg->AddButton(ID_BUTTON_HIDE_LORDSTRIKE,NULL,0,0,0,0,0,0,&pButton) ) )
	               return;
	}
	assert(pButton != NULL);
	if( !pButton) return;
        pButton->SetVisible(true);   
	
	int nHideBtnNum = GetChatManager().GetHideButtonNum();
	stPointI pt = GetHideChatButtonPos(nHideBtnNum);
	pButton->SetLocation(pt.x,pt.y);
	pButton->SetButtonType(CGuiButton::ButtonType_Bitmap);
#define FLASH_ID  30
#define NO_FLASH_ID 21
	if( GetGame()->IsMeNow()){// flash 				
		pButton->SetBitmap(stResourceLocation("data\\Interfaces.gl",135,FLASH_ID),1);	//闪
	}
	else{// not flash 
		pButton->SetBitmap(stResourceLocation("data\\Interfaces.gl",135,NO_FLASH_ID),1);	//不闪
	}			

	pButton->SetToolTips("斗地主游戏");
}

//响应主界面上 ID_BUTTON_HIDE_LORDSTRIKE  按钮事件
bool CLordStrikeManager::OnReOpenLSDialog(UINT nID)
{
	FUNCTION_BEGIN;
	CGuiMain* pMainDlg = GetGameGuiManager()->m_guiMain;

	if( pMainDlg && nID == ID_BUTTON_HIDE_LORDSTRIKE)
	{
		CGuiButton * pButton = pMainDlg->GetButton(nID);
		if( pButton)
			pButton->SetVisible(false);//隐藏对应的按钮

		ShowAllDialog(true);
	}	
	return false;
}


void CLordStrikeManager::ShowAllDialog(bool bShow)
{
	if(m_guiLordStrikeLobby)
		m_guiLordStrikeLobby->SetVisible(bShow);
	if(m_guiLSScoreDlg)
		m_guiLSScoreDlg->SetVisible(bShow);

	if(m_guiSortDlg)
		m_guiSortDlg->SetVisible(bShow);
	if(m_guiLSChangeMoneyDlg)
		m_guiLSChangeMoneyDlg->SetVisible(bShow);

	if(m_guiLordStrikeGameBk)
	{
		m_guiLordStrikeGameBk->SetVisible(bShow);
	}
	else 
	{
		if(m_guiLordStrikeRoom )
			m_guiLordStrikeRoom->SetVisible(bShow);
	}
}

void CLordStrikeManager::UpdateMainUserInfo(stUserDataCommonMiniGameCmd * pCmd)
{
        m_MainUserInfo.userdata = pCmd->data;
}
 
