#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include ".\guichatselect.h"
#include ".\guiprivatechatselect.h"

//////////////////////////////////////////////////////////////////////////
// 这个不是 私聊 而是密聊！！！！
//
//////////////////////////////////////////////////////////////////////////


CGuiPrivateChatSelect::CGuiPrivateChatSelect(void)
{
	m_bSmDialog = true;

}


void CGuiPrivateChatSelect::BeginPopup()
{
	FUNCTION_BEGIN;

	if (InitData()==false)
		return;

	CGuiDialog::BeginPopup();

	stGuiAnimationInfo st;
	st.dwColorDst = GetColor();
	st.ptDst = GetLocation();
	st.fTime = 0.3f;
	st.eAniComplete = GuiAniEvent_None;
	BeginAnimation( &st );

	

	FUNCTION_END;
}

int CGuiPrivateChatSelect::GetPrivateChatNameNum()
{
	return m_chatNameList.size();
}

void CGuiPrivateChatSelect::EndPopup()
{
	FUNCTION_BEGIN;

	CGuiDialog::EndPopup();

	stGuiAnimationInfo st;
	st.dwColorDst = GetColor();
	st.ptDst = GetLocation();
	st.ptDst.y += 100;
	st.fTime = 0.3f;
	st.eAniComplete = GuiAniEvent_Hide;
	BeginAnimation( &st );

	FUNCTION_END;
}

void CGuiPrivateChatSelect::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_pListBox =GetListBox(1);

	m_chatNameList.clear();

	FUNCTION_END;
}

void CGuiPrivateChatSelect::RenderBackground(float fElapsedTime)
{
	stRectI rect(0,0,108,90);
	rect.OffsetRect(m_x+10,m_y+10);

	GetDevice()->FillRect(&rect,COLOR_ARGB(128,0,0,0));
}

bool CGuiPrivateChatSelect::InitData()
{
	if (m_pListBox)
	{
		m_pListBox->RemoveAllItems();
		for (int i=0;i<m_chatNameList.size();i++)
		{
			m_pListBox->AddItem(m_chatNameList[i].c_str(),NULL);
		}
		if (m_pListBox->GetCount()>0)
			m_pListBox->SetCurItem(0);
		else
			return false;
	}
	return true;
}

void CGuiPrivateChatSelect::AddWhisperChatName(const char* name)
{
	std::string strName = name;
	
	for (std::vector<std::string>::iterator pointer=m_chatNameList.begin();
		pointer != m_chatNameList.end();pointer++)
	{
		if (strName == (*pointer))
		{
			m_chatNameList.erase(pointer);
			break;
		}
	}
	
	if (m_chatNameList.size()>=MAXCHATNUM)
		m_chatNameList.pop_back();

	m_chatNameList.insert(m_chatNameList.begin(),strName);

}

bool CGuiPrivateChatSelect::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	FUNCTION_BEGIN;

	switch(uMsg)
	{
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_RETURN:
			{
				OnGuiEvent(EVENT_LISTBOX_ITEM_CLICK,1,NULL);
				return true;
			}
			break;
		case VK_UP:
		case VK_DOWN:
			break;
		default:
			if (GetGameGuiManager()->m_guiChatSelect)
			{
				GetGameGuiManager()->m_guiChatSelect->MsgProc(hWnd,uMsg,wParam,lParam);
				GetGameGuiManager()->m_guiChatSelect->ChatEditSetFocus();

			}
			EndPopup();
			return true;
		}
		break;
	}

	return CGuiDialog::MsgProc( hWnd, uMsg, wParam, lParam );

	FUNCTION_END;
}

bool CGuiPrivateChatSelect::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	if ((nEvent==EVENT_LISTBOX_ITEM_CLICK)&&(nID==1))
	{
		if (GetGameGuiManager()->m_guiChatSelect)
		{
			GetGameGuiManager()->m_guiChatSelect->InsertPrivateChat(
				m_pListBox->GetItemText(m_pListBox->GetCurItem()));
			GetGameGuiManager()->m_guiChatSelect->ChatEditSetFocus();
		}
		EndPopup();
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl); 
}

void CGuiPrivateChatSelect::AddWhisperChatUser(const char* user)//static
{
	if (GetGameGuiManager()->m_guiPrivateChatSelect)
	{
		GetGameGuiManager()->m_guiPrivateChatSelect->AddWhisperChatName(user);
	}	
}