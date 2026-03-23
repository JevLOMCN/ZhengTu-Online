

#include "public.h"
#include "./GameGuiManager.h"

#include "./GameScene.h"
#include "./character.h"
#include "./MainCharacter.h"
#include "./CreateRole.h"
#include "./GuiWorldConn.h"
#include "./GuiItem.h"
#include "./RoleItem.h"
#include "./GameGuiFloatItem.h"
#include "./Country.h"

#include "CGuiRightFadeNotifyDialog.h"

void CGuiRightFadeNotifyDialog::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();


	FUNCTION_END;
}

void CGuiRightFadeNotifyDialog::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();

	m_tab = GetTab(12);

	m_tab->AddItem("0", NULL);
	m_tab->AddItem("1", NULL);
	m_tab->AddItem("2", NULL);
	m_tab->AddItem("3", NULL);

	AddControl2Tab(m_tab, 0, 2);
	AddControl2Tab(m_tab, 1, 3);
	AddControl2Tab(m_tab, 2, 4);
	AddControl2Tab(m_tab, 3, 5);

	m_tab->SetVisible(false);

	m_multiTextBox[0] = GetMLTextBox(2);
	m_multiTextBox[1] = GetMLTextBox(3);
	m_multiTextBox[2] = GetMLTextBox(4);
	m_multiTextBox[3] = GetMLTextBox(5);
	m_multiTextBox[4] = GetMLTextBox(3);

	GetButton(8)->SetButtonGroup(1);
	GetButton(9)->SetButtonGroup(1);
	GetButton(10)->SetButtonGroup(1);
	GetButton(11)->SetButtonGroup(1);

	OnGuiEvent(EVENT_BUTTON_CLICKED, 8, GetButton(8));

	m_lastCreateTime = time(NULL);

	FUNCTION_END;
}

void CGuiRightFadeNotifyDialog::OnClose()
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiRightFadeNotifyDialog = NULL;

	FUNCTION_END;
}

void CGuiRightFadeNotifyDialog::OnResetScreenSize()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnResetScreenSize();

	m_x = ( GetDevice()->GetWidth() - GetImage( 0 )->GetWidth() );
	m_y =  GetDevice()->GetHeight() - GetImage( 0 )->GetHeight() - 10;

	FUNCTION_END;
}
HRESULT CGuiRightFadeNotifyDialog::OnRender( float fElapsedTime )
{
	if(m_x > GetDevice()->GetWidth() + 20 - GetImage( 0 )->GetWidth() ) //调整X
		m_x -= 13; //移动速度 越大越快

	//if(time(NULL) > m_lastCreateTime + 5 )
	//	Hide();

	return CGuiDialog::OnRender(fElapsedTime);
}

bool CGuiRightFadeNotifyDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch(nEvent)
	{
	case EVENT_DIALOG_SHOW:
		{
			m_x = GetDevice()->GetWidth()+ 125; //微调X125
			m_y =  GetDevice()->GetHeight() - GetImage( 0 )->GetHeight() - 10; //调整Y上下
		}
		break;

	case EVENT_BUTTON_CLICKED:
		{
			switch(nID)
			{
			case 7:
				{
					Hide();
					break;
				}
			case 8:
			case 9:
			case 10:
			case 11:
				{
					m_tab->SetCurItem(nID - 8);
					GetButton(nID)->SetChecked(true);
					break;
				}
			}
			break;
		}
	}
	
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void  CGuiRightFadeNotifyDialog::SetInfo(int index, char* c,DWORD color)
{
	if(index < 1) return;

	char szBuf[0x100];
	sprintf(szBuf, "%s %s", c, GetGameTime()->GetZSTimeString());
	
	m_multiTextBox[0]->AppendParagraph(szBuf,color);
	m_multiTextBox[index]->AppendParagraph(szBuf,color);
}

