#include "public.h"
#include "./GameGuiManager.h"
#include ".\GuiAddMemberDialog.h"
#include "./GuiWorldConn.h"
#include "./Character.h"

CGuiAddMemberDialog::CGuiAddMemberDialog(enumGuiActionType type,const char* pszTitle)
{
	m_type = type;
	//m_bSmDialog = true;
	m_title = pszTitle;
}

CGuiAddMemberDialog::~CGuiAddMemberDialog(void)
{
}


void CGuiAddMemberDialog::OnClose(void)
{
	GetGameGuiManager()->m_guiAddMemberDlg = NULL;
}

void CGuiAddMemberDialog::OnCreate()
{
	CGuiDialog::OnCreate();
	m_pEditBox = GetEditBox(5);
	m_pListBox = GetListBox(2);

	AssertCreateCtrl(m_pEditBox,"添加成员对话框脚本错误");
	AssertCreateCtrl(m_pListBox,"添加成员对话框脚本错误");

	SetTitle(m_title.c_str());

	UpdateNearCharacterList();
}

void CGuiAddMemberDialog::OnInputBoxEnter(bool bOk)
{
	if(bOk)
	{
		const char* szText = m_pEditBox->GetText();
		if(szText == NULL || szText[0] == 0 || szText[0] == ' ' || szText[strlen(szText)-1] == ' ')
		{
			GameMessageBox("输入不合法！");
			return;
		}

		switch(m_type)
		{
		case eAction_AddFriendMember:
			{
				AddFriend(szText);
			}
			break;
		case eAction_AddSeptMember:
			{
				AddSept(szText);
			}
			break;
		case eAction_AddTongMember:
			{
				AddTong(szText);
			}
			break;
		case eAction_AddSchoolMember:
			{
				AddPrentice(szText);
			}
			break;
		default:
			break;
		}
	}
}

bool CGuiAddMemberDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		OnInputBoxEnter( nID == 3 );
		Close();
		return true;
	}
	if( nEvent == EVENT_LISTBOX_ITEM_DBLCLK &&
		m_pListBox && m_pEditBox )
	{
		int iCur = m_pListBox->GetCurItem();
		if( iCur >= 0 )
		{
			const char* pszName = m_pListBox->GetItemText(iCur);
			m_pEditBox->SetText(pszName);
		}
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
}

void CGuiAddMemberDialog::SetTitle(const char* title)
{
	GetStatic(1)->SetText(title);
}

void CGuiAddMemberDialog::UpdateNearCharacterList()
{
	if ( m_pListBox )
	{
		std::vector< CCharacter*, std::stack_allocator<CCharacter*> > aCharacter;
		GetScene()->GetObjectOrderByDistance(aCharacter,type2type_t<CCharacter>());
		m_pListBox->RemoveAllItems();
		for( size_t i = 0; i < aCharacter.size(); ++i )
		{
			m_pListBox->AddItem(aCharacter[i]->GetName(),NULL);
		}
	}
}

HRESULT CGuiAddMemberDialog::OnRender(float fElapsedTime)
{
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	static DWORD dwRefreshTime = 0;
	if( xtimeGetTime() - dwRefreshTime > 5000)
	{
		dwRefreshTime = xtimeGetTime();
		UpdateNearCharacterList();
	}
	return hr;
}