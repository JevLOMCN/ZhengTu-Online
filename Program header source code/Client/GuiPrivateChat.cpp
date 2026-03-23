/**
*\file		GuiPrivateChat.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    私人聊天对话框
*\	$$$04公司 修改
*/

#include "public.h"
#include "./GameGuiManager.h"
#include "./guiprivatechat.h"
#include "./game.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./GuiChatInvite.h"
#include "./GuiFace.h"
#include "./GuiWorldConn.h"
#include "./GuiChatSelect.h"
#include "./GuiSelectSeptDareType.h"
#include "./GuiChatOutput.h"
#include "./Relation.h"
#include "./Chat.h"
#include "./GuiTeam.h"

#define		Chat_ChatCmdID			1
#define		Chat_CopyUserName		2
#define		Chat_CommTeam			3
#define		Chat_HonorTeam			4

#define USER_LIST_DIALOG_CASE(id,index)	case id:GetTab(2)->SetCurItem(index);GetButton(id)->SetChecked(true);return true;break

//////////////////////////////////////////////////////////////////////////
CGuiChatInvateDialog::CGuiChatInvateDialog(byte state)
:m_eState(state)
{
	m_bSmDialog = true; 
	m_ChatMenu = NULL;
}

void CGuiChatInvateDialog::SetCloseIsHide(bool hide)
{
	m_bCloseIsHide = true;
}

void CGuiChatInvateDialog::SetData(void* pData)
{
	m_pData = pData;
}

void* CGuiChatInvateDialog::GetData()
{
	return m_pData;
}

void CGuiChatInvateDialog::InsertPrivatName(const char* pStr)
{	
	FUNCTION_BEGIN;
	for(int i=0;i<m_pListBox->GetCount();i++)
	{
		if (strcmp(m_pListBox->GetItemText(i),pStr) == 0)
			return;
	}
	if( m_pListBox->GetCount() >= 15 )		//只保存15个
	{
		static int i = 0;
		m_pListBox->SetItemText(i++,0,pStr);
		if( i >= 15 )
		{
			i = 0;
		}
	}
	else
	{
		m_pListBox->InsertItem(0,pStr,NULL);
	}	
}

void CGuiChatInvateDialog::OnChannelClosed(CChatChannel* pChannel)
{
	if( GetGameGuiManager()->m_guiInvite )
	{
		void* pData = GetGameGuiManager()->m_guiInvite->GetData();
		if( (CChatChannel*)pData == pChannel )
		{
			GetGameGuiManager()->m_guiInvite->SetData(NULL);
		}
	}
}

void CGuiChatInvateDialog::AddPrivateChatName(const char* name)
{
	if( GetGameGuiManager()->m_guiInvite )
	{
		GetGameGuiManager()->m_guiInvite->InsertPrivatName(name);
	}
}

void CGuiChatInvateDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	if (m_eState==eChat)
	{
		GetGameGuiManager()->m_guiInvite = NULL;
	}

	if(m_ChatMenu)
	{
		m_ChatMenu->Close();
		m_ChatMenu = NULL;
	}

	FUNCTION_END;
}

void CGuiChatInvateDialog::OnPopupChatMenu()
{
	FUNCTION_BEGIN;
	if( m_ChatMenu )
	{
		SetPopupMenu( m_ChatMenu );
		m_ChatMenu->Popup();
	}	
}

bool CGuiChatInvateDialog::OnMenuCmdChat(UINT nID)
{
	FUNCTION_BEGIN;
	const char * pszName = m_pListBox->GetItemText(m_pListBox->GetCurItem());
	if( NULL != pszName )
	{
		if( nID == Chat_ChatCmdID )
		{
			InsertPrivateChat(pszName);
			return true;
		}
		if( nID == Chat_CopyUserName )
		{
			Copy2Clipboard(pszName);
			return true;
		}
	}
	return false;
}

bool CGuiChatInvateDialog::OnContextMenu()
{
	FUNCTION_BEGIN;

	if( m_pListBox->OnLButtonClickOnItem() )
	{
		OnPopupChatMenu();
		return true;
	}
	return false;

	FUNCTION_END;
}

bool CGuiChatInvateDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		switch(nID) 
		{
		case 1:
			{
				const char* szText = GetEditBox(3)->GetText();
				if( szText == NULL					|| 
					szText[0] == 0					|| 
					szText[0] == ' '				|| 
					szText[strlen(szText)-1] == ' ' || 
					(0 == strcmp(szText,GetScene()->GetMainCharacter()->GetName())) )
				{
					GameMessageBox("输入不合法！");
					return true;
				}
				if(szText[0])
				{
					switch(m_eState) 
					{
					case eChat:
						{					
							InsertPrivatName(szText);
							GetChatManager().OnChat(szText,(CChatChannel*)m_pData);
						}
						break;
					case eTong:
						{
							stActiveDareCmd  cmd;
							sprintf(cmd.name,GetScene()->GetMainCharacter()->GetName(),sizeof(cmd.name));
							sprintf(cmd.fromRelationName,
								GetGameGuiManager()->m_guiWorldConn->m_TongInfo.unionName,sizeof(cmd.fromRelationName));
							sprintf(cmd.toRelationName,szText,sizeof(cmd.toRelationName));
							cmd.dwDareType = UNION_DARE;

							SEND_USER_CMD(cmd);
						}
						break;
					case eSept:
						{
							stActiveDareCmd  cmd;
							sprintf(cmd.name,GetScene()->GetMainCharacter()->GetName(),sizeof(cmd.name));
							sprintf(cmd.fromRelationName,
								GetGameGuiManager()->m_guiWorldConn->m_SeptInfo.septName,sizeof(cmd.fromRelationName));
							sprintf(cmd.toRelationName,szText,sizeof(cmd.toRelationName));
							cmd.dwDareType = SEPT_DARE;
							cmd.byDareRepute = 0;

							//backup cmd
							GetGameGuiManager()->AddSelectSeptDareTypeDlg();
							if ( GetGameGuiManager()->m_guiSelectSeptDareTypeDlg )
							{
								GetGameGuiManager()->m_guiSelectSeptDareTypeDlg->m_cmdSeptDare = cmd;
							}

							//SEND_USER_CMD(cmd);
						}
						break;
					}
				}
				OnGuiEvent(EVENT_BUTTON_CLICKED,-1,NULL);
			}
			break;
		case 2:
			OnGuiEvent(EVENT_BUTTON_CLICKED,-1,NULL);
			break;
		}
		break;
	case EVENT_LISTBOX_ITEM_DBLCLK:
		{
			if (nID == 4)
			{
				GetEditBox(3)->SetText(m_pListBox->GetItemText(m_pListBox->GetCurItem()));
				this->RequestFocus(GetEditBox(3));
			}
		}
		break;
	case EVENT_DIALOG_COMMAND:
		{
			if ( OnMenuCmdChat(nID) )
			{
				return true;
			}			
		}
	default:
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}

void CGuiChatInvateDialog::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_pListBox	=	GetListBox(4);
	m_ChatMenu = GetGuiManager()->CreateMenu( this );
	if( m_ChatMenu )
	{
		m_ChatMenu->AddMenuItem(Chat_ChatCmdID,"密聊",CGuiMenu::MenuStyle_Command,true);
		m_ChatMenu->AddMenuItem(Chat_CopyUserName,"复制人名",CGuiMenu::MenuStyle_Command,true);		
	}

	FUNCTION_END;
}

//////////////////////////////////////////////////////////////////////////

CGuiChatDialog::CGuiChatDialog()
:m_pEditInput(NULL)
,m_pEditOutput(NULL)
,m_pListUser(NULL)
,m_pChannel(NULL)
{	
}

CGuiChatDialog::~CGuiChatDialog()
{
}

void CGuiChatDialog::OnClose()
{
	if( m_pChannel )
	{
		m_pChannel->SetChatDialog( NULL );
	}
	if(m_ChatMenu)
	{
		m_ChatMenu->Close();
		m_ChatMenu = NULL;
	}
}

void CGuiChatDialog::AddMessage(const char* msg,DWORD color )
{
	m_pEditOutput->AppendParagraph(msg,color);
	ScrollToTail();
}

void CGuiChatDialog::AddMessage(TiXmlElement* page)
{
	m_pEditOutput->AppendParagraph(page,NULL);
	ScrollToTail();
}

void CGuiChatDialog::ScrollToTail()
{
	m_pEditOutput->ScrollToTail();
}

void CGuiChatDialog::LoadChatMsg( const char* pszFileName )
{
	FUNCTION_BEGIN;
	m_pEditOutput->LoadDoc( pszFileName );
}

void CGuiChatDialog::RemoveAllMsg()
{
	FUNCTION_BEGIN;
	m_pEditOutput->SetText("");
}

void CGuiChatDialog::UpdateDialogTitle()
{
}

void CGuiChatDialog::AddChatUser(const char* name,DWORD color)
{
	FUNCTION_BEGIN;
	m_pListUser->AddItem(name,NULL);
	m_pListUser->SetItemColor(m_pListUser->GetItemCount()-1,0,color);
}

void CGuiChatDialog::RemoveAllUser()
{
	FUNCTION_BEGIN;
	m_pListUser->RemoveAllItems();
}

void CGuiChatDialog::OnCreate()
{
	m_pEditInput	=	GetEditBox(2);
	m_pEditOutput	=	GetMLTextBox(1);
	m_pListUser		=	GetListBox(4);

	this->SetFocus((CGuiControl*)m_pEditInput);


	m_pEditInput->SetUsage(CGuiEditBox::USAGE_CHAT);

	UpdateDialogTitle();

	m_ChatMenu = GetGuiManager()->CreateMenu( this );
	if( m_ChatMenu )
	{
		m_ChatMenu->AddMenuItem(Chat_ChatCmdID,"密聊",CGuiMenu::MenuStyle_Command,true);
		m_ChatMenu->AddMenuItem(Chat_CopyUserName,"复制人名",CGuiMenu::MenuStyle_Command,true);		
	}

	CGuiDialog::OnCreate();
}

bool CGuiChatDialog::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	FUNCTION_BEGIN;

	if( uMsg == WM_KEYDOWN && wParam == VK_RETURN )
	{
		if( m_pEditInput && m_pEditInput->IsFocus() )
		{
			SendChat();
			return true;
		}
	}
	if ( (GetKeyState(VK_CONTROL) < 0) && (wParam == 'C') )
	{
		if( m_pListUser )
		{
			int cur = m_pListUser->GetCurItem();
			if( cur != -1 )
			{
				const char* psz = m_pListUser->GetItemText(cur);
				Copy2Clipboard( psz );
				return true;
			}
		}
	}
	return CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);
}

bool CGuiChatDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	switch( nEvent )
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch( nID )
			{
			case 102:	// 聊天表情对话框
				{
					OnOffChatFaceSelect(m_pEditInput);
					return true;
				}
				break;
			case -1:
			case 104:
				{
					GetChatManager().OnHideChatDialog( m_pChannel );
					return true;
				}
				break;
			case 106:	//退出群聊
				{
					if( NULL != m_pChannel )
					{ 
						m_pChannel->CloseChannel();
					}
					Close();
					return true;
				}
				break;
			case 107:
				{
					SendChat();
					return true;
				}
				break;
			default:
				break;
			}
		}
		break;
	case EVENT_DIALOG_COMMAND:
		{
			if ( OnMenuCmdChat(nID) )
			{
				return true;
			}			
		}
		break;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}

void CGuiChatDialog::OnPopupChatMenu()
{
	if( m_ChatMenu )
	{
		SetPopupMenu( m_ChatMenu );
		m_ChatMenu->Popup();
	}	
}

bool CGuiChatDialog::OnMenuCmdChat(UINT nID)
{
	const char * pszName = m_pListUser->GetItemText(m_pListUser->GetCurItem());
	if( NULL != pszName )
	{
		if( nID == Chat_ChatCmdID )
		{
			InsertPrivateChat(pszName);
			return true;
		}
		if( nID == Chat_CopyUserName )
		{
			Copy2Clipboard(pszName);
			return true;
		}
	}
	return false;
}

bool CGuiChatDialog::OnContextMenu()
{
	FUNCTION_BEGIN;

	if( m_pListUser && m_pListUser->OnLButtonClickOnItem() )
	{
		OnPopupChatMenu();
		return true;
	}
	return false;

	FUNCTION_END;
}


void CGuiChatDialog::SendChat()
{
	FUNCTION_BEGIN;

	if( m_pEditInput )
	{
		const char* szText = m_pEditInput->GetText();
		if(szText[0])
		{
			if( m_pChannel )
			{
				//发送消息
				m_pChannel->SendChatMsg( m_pEditInput );
			}
			m_pEditInput->SetText("");
		}
	}

	FUNCTION_END;
}



//////////////////////////////////////////////////////////////////////////
CGuiPrivateChat::CGuiPrivateChat(CChatChannel* pChannel)
{
	FUNCTION_BEGIN;

	m_nCapOffsetY = 7;
	m_nSBWidth = 18;

	m_pChannel = pChannel;

	FUNCTION_END;
}

CGuiPrivateChat::~CGuiPrivateChat()
{
}

void CGuiPrivateChat::UpdateDialogTitle()
{
	if( m_pChannel )
	{
		GetStatic(5)->SetText("私人聊天[普通]");
		bool bEnbale = CanInvite();
//#ifdef _DEBUG
//		Engine_WriteLogF("Channel:%u bEnable %s\n",m_pChannel->GetChannelID(),bEnbale?"true":"false");
//#endif

		GetButton(100)->SetEnabled(bEnbale);
		//GetButton(101)->SetEnabled(bEnbale);
		GetButton(105)->SetEnabled(bEnbale);
	}
}

bool CGuiPrivateChat::CanInvite()
{
	if( NULL != m_pChannel )
	{
		//如果是自己创建的频道且人数小于20人 或者只是对话 则可以添加成员
		bool bRes = ( ( m_pChannel->IsMine() && m_pChannel->GetMemberCount() < 20 ) || 
			(m_pChannel->GetChannelID() == 0));
		return bRes;
	}
	return true;
}

bool CGuiPrivateChat::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID)
			{
			case 100:	// 邀请社会关系	
				{
					GetGameGuiManager()->AddUserListDialog(m_pChannel);
				}
				break;
			//case 101:	// 邀请附近玩家
			//	{
			//		GetGuiManager()->RequestFocus((CGuiDialog*) GetGameGuiManager()->AddNearby(m_pChannel) );
			//		GetGameGuiManager()->m_guiNearby->OnGuiEvent(EVENT_BUTTON_CLICKED,NEARBYPAGE,NULL);
			//		return true;
			//	}
			//	break;			
			case 105:	// 邀请玩家
				{
					GetGameGuiManager()->AddChatInvate(m_pChannel);
					return true;
				}
				break;
			default:
				break;
			}
		}
		break;
	}
	return CGuiChatDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

CGuiFUChatDialog::CGuiFUChatDialog(DWORD chatType,CChatChannel* pChannel)
{
	m_dwChatType = chatType;
	m_pChannel = pChannel;
}

bool CGuiFUChatDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{	
	//if( nEvent == EVENT_CHECKBOX_CHANGED )
	//{		
	//	if( m_pChannel )
	//	{
	//		bool bChecked = GetCheckBox(108)->GetChecked();
	//		m_pChannel->SetForbid(bChecked);
	//	}		
	//}
	return CGuiChatDialog::OnGuiEvent(nEvent,nID,pControl);
}

void CGuiFUChatDialog::UpdateDialogTitle()
{
	FUNCTION_BEGIN;
	switch( m_dwChatType )
	{
	case CHAT_TYPE_FAMILY:
		GetStatic(5)->SetText("家族聊天");
		break;
	case CHAT_TYPE_UNION:
		GetStatic(5)->SetText("帮会聊天");
		break;
	default:
		Assert(0);
		break;
	}
	//if( m_pChannel )
	//{
	//	bool bForbid = m_pChannel->GetForbidden();
	//	GetCheckBox(108)->SetChecked(bForbid);
	//}
}
//////////////////////////////////////////////////////////////////////////

CGuiUserListDialog::CGuiUserListDialog()
{
	m_pMenu = NULL;
}

void CGuiUserListDialog::OnCreate()
{
	CGuiDialog::OnCreate();

	GetTab(2)->SetCurItem(0);
	GetTab(2)->SetVisible(false);

	m_pMenu = GetGuiManager()->CreateMenu( this );
	if( m_pMenu )
	{
		m_pMenu->AddMenuItem(Chat_ChatCmdID,"密聊",CGuiMenu::MenuStyle_Command,true);
		m_pMenu->AddMenuItem(Chat_CopyUserName,"复制人名",CGuiMenu::MenuStyle_Command,true);
		m_pMenu->AddMenuItem(Chat_CommTeam,"普通组队",CGuiMenu::MenuStyle_Command,true);
		m_pMenu->AddMenuItem(Chat_HonorTeam,"荣誉组队",CGuiMenu::MenuStyle_Command,true);
	}

	for( int i = 0; i < 4; i++ )
	{
		m_pUserList[i] = GetListBox( i*100 + 101 );
		Assert(m_pUserList[i]);
	}

	UpdateUserList();

	UpdateNearCharacterList();
}



bool CGuiUserListDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	//处理按钮按键消息
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
			USER_LIST_DIALOG_CASE(11,0);
			USER_LIST_DIALOG_CASE(12,1);
			USER_LIST_DIALOG_CASE(13,2);
			USER_LIST_DIALOG_CASE(14,3);
		case 1:
			Close();
			return true;
			break;
		default:break;
		}
	}

	//处理双击列表框控件消息
	if( nEvent == EVENT_LISTBOX_ITEM_DBLCLK )
	{
		CGuiListBox* pListBox = (CGuiListBox*)pControl;
		if( pListBox )
		{
			int cur = pListBox->GetCurItem();
			if( cur != -1 && m_pChannel )
			{
				const char* pszName = pListBox->GetItemText(cur,0);
				m_pChannel->InviteUser(pszName);
			}
		}
	}

	//处理菜单消息
	if( nEvent == EVENT_DIALOG_COMMAND )
	{
		int cur = GetTab(2)->GetCurItem();
		if( cur < 4 && cur > -1 )
		{
			const char * pszName = m_pUserList[cur]->GetItemText(m_pUserList[cur]->GetCurItem());
			if( NULL != pszName )
			{
				switch ( nID )
				{
				case Chat_ChatCmdID:		InsertPrivateChat(pszName);return true;break;
				case Chat_CopyUserName:		Copy2Clipboard(pszName);return true;break;
				case Chat_CommTeam:			CGuiTeam::AddMember(pszName,TEAM_NORMAL);return true;break;
				case Chat_HonorTeam:		CGuiTeam::AddMember(pszName,TEAM_HONOR);return true;break;
				default:break;
				}
			}
		}		
	}

	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}

void CGuiUserListDialog::OnClose()
{
	GetGameGuiManager()->m_pUserListDlg = NULL;
}

bool CGuiUserListDialog::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{

	return CGuiDialog::MsgProc(hWnd,uMsg,wParam,lParam);
}

HRESULT CGuiUserListDialog::OnRender( float fElapsedTime )
{
	static DWORD m_dwRefreshTime=0;
	if(xtimeGetTime() - m_dwRefreshTime > 5000)
	{
		m_dwRefreshTime = xtimeGetTime();
		UpdateNearCharacterList();
	}

	return CGuiDialog::OnRender(fElapsedTime);
}

void CGuiUserListDialog::UpdateUserList()
{
	FUNCTION_BEGIN;
	for( int i = 0 ; i < 3; i++ )
	{
		GetRelation().OnUpdate(m_pUserList[i],(CRelation::RelationShip)i);
	}
}

void CGuiUserListDialog::OnChannelClosed(CChatChannel* pChannel)
{
	if( GetGameGuiManager()->m_pUserListDlg )
	{
		CChatChannel* p = GetGameGuiManager()->m_pUserListDlg->m_pChannel;
		if( p == pChannel )
		{
			GetGameGuiManager()->m_pUserListDlg->SetChannel( NULL );
		}
	}	
}

void CGuiUserListDialog::OnRelationUpdate()
{
	if( GetGameGuiManager()->m_pUserListDlg )
	{
		GetGameGuiManager()->m_pUserListDlg->UpdateUserList();
	}	
}

bool CGuiUserListDialog::OnContextMenu()
{
	FUNCTION_BEGIN;

	int cur = GetTab(2)->GetCurItem();
	if( cur < 4 && cur > -1 )
	{
		if( m_pUserList[cur]->OnLButtonClickOnItem() )
		{
			if( m_pMenu )
			{
				SetPopupMenu( m_pMenu );
				m_pMenu->Popup();
				return true;
			}		
		}
	}

	return false;

	FUNCTION_END;
}

void CGuiUserListDialog::UpdateNearCharacterList()
{
	if( m_pUserList[3] == NULL ) return ;

	m_pUserList[3]->RemoveAllItems();
	std::vector< CCharacter*, std::stack_allocator<CCharacter*> > aCharacter;
	GetScene()->GetObjectOrderByDistance(aCharacter,type2type_t<CCharacter>());
	int num = getMin(100,(int)aCharacter.size());
	for( size_t i = 0; i < num; ++i )
	{
		CCharacter* pChar = aCharacter[i];
		m_pUserList[3]->AddItem("",NULL);
		m_pUserList[3]->SetItemText(i,0,pChar->GetName(),22,GuiListBoxColFmt_Center);
		m_pUserList[3]->SetItemText(i,1,pChar->GetCountryName(),16,GuiListBoxColFmt_Center);
	}
}