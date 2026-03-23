#include "public.h"
#include "./GameGuiManager.h"
#include ".\GuiAdoptCartoonDialog.h"
#include "./GameScene.h"
#include "./CartoonPet.h"
#include "./GuiWorldConn.h"
#include "./GuiChatOutput.h"

#ifdef _DEBUG
#define AdoptCartoon_Trace(fun,id,b)	{TRACE("%s(%d,%d) line:%d\n",fun,id,b,__LINE__); }
#else
#define AdoptCartoon_Trace(fun,id,b)	__noop
#endif

#define AdoptCartoon_DelayEnable(id,b)	{bool bRes = b;CGuiControl *p = GetControl(id);if(p) {p->SetEnabled(false);if(bRes){p->SetEnableDelay(true,1000);}AdoptCartoon_Trace("AdoptCartoon_DelayEnable",id,bRes);}}

#define AdoptCartoon_EnableCtrl(id,b)	{bool bRes = b;CGuiControl *p = GetControl(id);if(p) {p->SetEnabled(bRes);AdoptCartoon_Trace("AdoptCartoon_EnableCtrl",id,b);} }

#define AdoptCartoon_ChatCmdID		1
#define AdoptCartoon_CopyUserName	2

CGuiAdoptCartoonDialog::CGuiAdoptCartoonDialog(void)
{
	m_ChatMenu = NULL;
}

CGuiAdoptCartoonDialog::~CGuiAdoptCartoonDialog(void)
{
}

void CGuiAdoptCartoonDialog::OnCreate(void)
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	AdoptCartoon_EnableCtrl(902,false);
	AdoptCartoon_EnableCtrl(903,false);

	m_guiListBoxCartoon = GetListBox( 904 );
	AssertCreateCtrl(m_guiListBoxCartoon,"宠物领养列表脚本错误");	

	GetButton(FRIENDPAGE)->SetButtonGroup(1);
	GetButton(ENEMYPAGE)->SetButtonGroup(1);
	GetButton(BADPAGE)->SetButtonGroup(1);	
	GetButton(CARTOON_PET)->SetButtonGroup(1);

	m_ChatMenu = GetGuiManager()->CreateMenu( this );
	if( m_ChatMenu )
	{
		m_ChatMenu->AddMenuItem(AdoptCartoon_ChatCmdID,"密聊",CGuiMenu::MenuStyle_Command,true);
		m_ChatMenu->AddMenuItem(AdoptCartoon_CopyUserName,"复制人名",CGuiMenu::MenuStyle_Command,true);		
	}
	//发送查询列表请求
	CCartoonPet::RequestWaitCartoonList();

	UpdateCartoonPetList();

	FUNCTION_END;
}

bool CGuiAdoptCartoonDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case 902: return OnAdoptCartoonPet();
		case 903: return OnReturnCartoonPet();
		case 102:
		case 103:
		case 120:
		case 1000:
		case 1002:
		case 1003:
		case 1004:
		case 1005:
		case 3:
			return OnSwitchToWorldConn( nID );
		default:
			break;
		}
	}
	if( nEvent == EVENT_LISTBOX_SELECTION )
	{
		return OnUpdateButtons();
	}
	if( nEvent == EVENT_DIALOG_COMMAND)
	{
		OnMenuCmdChat(nID);
		return true;
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiAdoptCartoonDialog::OnClose(void)
{
	FUNCTION_BEGIN;
	GetGameGuiManager()->m_guiAdoptCartoonDlg = NULL;

	if(m_ChatMenu)
	{
		m_ChatMenu->Close();
		m_ChatMenu = NULL;
	}

	FUNCTION_END;
}

bool CGuiAdoptCartoonDialog::OnAdoptCartoonPet()
{
	int index = m_guiListBoxCartoon->GetCurItem();
	if( index != -1 )
	{
		DWORD dwCartoonID = (DWORD )( m_guiListBoxCartoon->GetItemData(index) );	
		AdoptCartoon_EnableCtrl( 902,false );
		CCartoonPet::AdoptCartoon( dwCartoonID );	
		m_guiListBoxCartoon->SetCurItem(-1);
	}
	return true;
}

bool CGuiAdoptCartoonDialog::OnReturnCartoonPet()
{
	int index = m_guiListBoxCartoon->GetCurItem();
	if( index != -1 )
	{
		DWORD dwCartoonID = (DWORD )( m_guiListBoxCartoon->GetItemData(index) );
		AdoptCartoon_EnableCtrl( 903,false );	
		CCartoonPet::ReturnCartoon( dwCartoonID );
		m_guiListBoxCartoon->SetCurItem(-1);
	}
	return true;
}

bool CGuiAdoptCartoonDialog::OnSwitchToWorldConn(UINT nID)
{
	GetButton(nID)->SetChecked(true);

	if( GetGameGuiManager()->m_guiWorldConn )
	{
		this->SetVisible(false);
		this->SetEnabled(false);
		POINT pt = this->GetLocation();

		GetGameGuiManager()->m_guiWorldConn->SetLocation(pt.x,pt.y);
		GetGameGuiManager()->m_guiWorldConn->SetVisible(true);
		GetGameGuiManager()->m_guiWorldConn->SetEnabled(true);
		GetGameGuiManager()->m_guiWorldConn->SwitchFromAdoptCartoonDlg(nID);
	}
	return true;
}


void CGuiAdoptCartoonDialog::UpdateCartoonPetList()
{
	m_guiListBoxCartoon->RemoveAllItems();
	CWaitingCartoonPetList::tListWaitingCartoonPet& List = GetScene()->GetWaitCartoonPetList()->GetList();

	for( CWaitingCartoonPetList::tListWaitingCartoonPetIt it = List.begin(); it != List.end(); ++it )
	{
		if( (*it)->state == CARTOON_STATE_ADOPTED || (*it)->state == CARTOON_STATE_WAITING )
		{
			DWORD dwColor = COLOR_ARGB(255,255,255,128);
			if( (*it)->state == CARTOON_STATE_ADOPTED )
			{
				dwColor = COLOR_ARGB(255,0,255,0);
			}
			const char* timestr = Second2String( (*it)->time );
			if( timestr )
			{
				m_guiListBoxCartoon->AddItem("",(void*)((*it)->cartoonID),dwColor);
				int index = m_guiListBoxCartoon->GetItemCount();
				m_guiListBoxCartoon->SetItemText(index-1,0,(*it)->masterName,32);
				if( (*it)->pNpcBase )
				{
					m_guiListBoxCartoon->SetItemText(index-1,1,(*it)->pNpcBase->strName,32);
				}
				else
				{
					m_guiListBoxCartoon->SetItemText(index-1,1,"",32);
				}
				m_guiListBoxCartoon->SetItemText(index-1,2,timestr,32);
			}			
		}
	}

	OnUpdateButtons();
}

bool CGuiAdoptCartoonDialog::OnUpdateButtons()
{
	int index = m_guiListBoxCartoon->GetCurItem();
	if( index != -1 )
	{
		DWORD dwCartoonID = (DWORD )( m_guiListBoxCartoon->GetItemData(index) );
		CWaitingCartoonPetList::t_WaitingCartoonPet* pwc = GetScene()->GetWaitCartoonPetList()->GetCartoonPet(dwCartoonID);
		if( pwc )
		{
			bool bCanAdopt = (pwc->state ==  CARTOON_STATE_WAITING &&  GetScene()->GetWaitCartoonPetList()->IsCanAdoptCartoonPet()) ;
			bool bCanReturn = (pwc->state == CARTOON_STATE_ADOPTED);

			AdoptCartoon_EnableCtrl(902,bCanAdopt);		
			AdoptCartoon_EnableCtrl(903,bCanReturn);	
		}
	}
	return true;
}

void CGuiAdoptCartoonDialog::SwitchFromWorldConn()
{
	if( GetGameGuiManager()->m_guiWorldConn )
	{
		GetGameGuiManager()->m_guiWorldConn->SetVisible(false);
		GetGameGuiManager()->m_guiWorldConn->SetEnabled(false);

		stPointI pt = GetGameGuiManager()->m_guiWorldConn->GetLocation();
		this->SetLocation(pt.x,pt.y);
		this->SetVisible(true);
		this->SetEnabled(true);
		GetButton(901)->SetChecked(TRUE);		
	}
}

void CGuiAdoptCartoonDialog::OnPopupChatMenu()
{
	if( m_ChatMenu )
	{
		SetPopupMenu( m_ChatMenu );
		m_ChatMenu->Popup();
	}	
}

const char* CGuiAdoptCartoonDialog::GetSelectCharacterName()
{
	int index = m_guiListBoxCartoon->GetCurItem();
	if( index != -1 )
	{
		DWORD dwCartoonID = (DWORD )( m_guiListBoxCartoon->GetItemData(index) );
		CWaitingCartoonPetList::t_WaitingCartoonPet* pwc = GetScene()->GetWaitCartoonPetList()->GetCartoonPet(dwCartoonID);
		if( pwc )
		{
			return pwc->masterName;					
		}
	}
	return NULL;
}

void CGuiAdoptCartoonDialog::OnMenuCmdChat(UINT nID)
{
	const char * pszName = GetSelectCharacterName();
	if( NULL != pszName )
	{
		if( nID == AdoptCartoon_ChatCmdID )
		{
			InsertPrivateChat(pszName);
		}
		if( nID == AdoptCartoon_CopyUserName )
		{
			Copy2Clipboard(pszName);
		}
	}
}

bool CGuiAdoptCartoonDialog::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( (GetKeyState(VK_CONTROL) < 0) && (wParam == 'C') )
	{
		const char * pszName = GetSelectCharacterName();
		if( NULL != pszName )
		{            
			Copy2Clipboard(pszName);
			return true;
		}
	}

	return CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);
}

bool CGuiAdoptCartoonDialog::OnContextMenu()
{
	FUNCTION_BEGIN;

	if( m_guiListBoxCartoon->OnLButtonClickOnItem() )
	{
		OnPopupChatMenu();
		return true;
	}
	return false;

	FUNCTION_END;
}