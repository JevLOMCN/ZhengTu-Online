
#include "public.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include ".\guichangecountry.h"
#include "./Country.h"
#include "./GuiNpcDialog.h"

CGuiChangeCountry::CGuiChangeCountry(void)
:m_pListBox(NULL)
{
}

void CGuiChangeCountry::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiChangeCountry = NULL;
	GetGameGuiManager()->OnNpcActionEnd();
	GetGameGuiManager()->EndNpcDialog();

	FUNCTION_END;
}

void CGuiChangeCountry::OnCreate()
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();

	GetButton(10)->SetEnabled(false);

	m_pListBox = GetListBox(2);
	
	if (m_pListBox)
	{
		m_pListBox->RemoveAllItems();

		const CCountry::COUNTRYLIST  & countrylist = CCountry::Instance()->GetCountryList();
		
		CCountry::COUNTRYLIST::const_iterator it;
		for( it = countrylist.begin(); it != countrylist.end(); ++it )
		{
		        if( 0 == stricmp(it->pstrName,"中立区"))
				continue;
			if(it->id == GetScene()->GetMainCharacter()->GetCountry())
			{
				//if (GetStatic(1)) GetStatic(1)->SetText(it->pstrName);
			}
			else
			{
				m_pListBox->AddItem("",(void*)it->id);
				m_pListBox->SetItemText(m_pListBox->GetCount()-1,0,"  ",2);
				m_pListBox->SetItemText(m_pListBox->GetCount()-1,1,it->pstrName);
			}
		}
	}


	FUNCTION_END;
}

//soke 增加叛国界面文字说明
HRESULT CGuiChangeCountry::OnRender( float fElapsedTime )
{
	HRESULT h = CGuiDialog::OnRender(fElapsedTime);

	POINT pt = GetLocation();
	char msg[] = "叛国将扣除你100锭金子，并且会\n让你失去除了好友外的社会关系，\n请慎重考虑。";
	int iOldFont = GetDevice()->SetFont(1); //soke 改变字体开始

	GetDevice()->DrawString(msg,stPointI(94+pt.x,213+pt.y),COLOR_ARGB(255,230,186,0),FontEffect_Border);
//	GetDevice()->DrawString("移民",stPointI(170+pt.x,12+pt.y),COLOR_ARGB(255,255,255,255));
    GetDevice()->SetFont(iOldFont);          //soke 改变字体结束
	return h;
}
//////////////////////////////////////////////////////////////////////////////

bool CGuiChangeCountry::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	CGuiDialog* pDlg = NULL;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) {
			case 10:
				{
					if (m_pListBox)
					{
						//soke 移民显示价格说明
						if(GetGameGuiManager()->m_guiNpcDialog)
						{
							GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = (void*)GetGameGuiManager()->AddMessageBox(avar("你确定要花100锭金子加入[%s]么？",m_pListBox->GetItemText(m_pListBox->GetCurItem(),1)),eAction_AddCountry,m_pListBox->GetItemData(m_pListBox->GetCurItem()),c_szMBOKCancel);
						//	GetGameGuiManager()->m_guiNpcDialog->m_pMsgDlg = (void*)GetGameGuiManager()->AddMessageBox(avar("你确定要花50两金加入[%s]么？",m_pListBox->GetItemText(m_pListBox->GetCurItem(),1)),eAction_AddCountry,m_pListBox->GetItemData(m_pListBox->GetCurItem()),c_szMBOKCancel);
						}

						/*stApplyCountryCmd cmd;
						cmd.dwToCountryID = (DWORD) m_pListBox->GetItemData(m_pListBox->GetCurItem());
						SEND_USER_CMD(cmd);*/
					}
				}
				break;
			case 11:
				{
					OnGuiEvent(EVENT_BUTTON_CLICKED,-1,NULL);
				}
				break;
			}
		}
		break;

	case EVENT_LISTBOX_SELECTION:
		{
			if (m_pListBox && m_pListBox->GetCurItem()!=-1)
			{
				GetButton(10)->SetEnabled(true);
			}
		}
		break;

	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiChangeCountry::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt( m_pBtnClose->GetLocation() );

		m_pBtnClose->SetLocation(pt.x,pt.y + 24);
	}

	FUNCTION_END;
}