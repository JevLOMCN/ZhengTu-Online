#include "public.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include ".\guichangewarfield.h"
#include "./Country.h"
#include "GuiNpcDialog.h"

CGuiChangeWarField::CGuiChangeWarField(BYTE state)
:m_pListBox(NULL)
,m_eState(state)
{
  
}


void CGuiChangeWarField::RefreshWarField(stReturnDareCountryCmd* pCmd)
{	
	if (m_pListBox == NULL)
		return;

	m_pListBox->RemoveAllItems();

	countryList.clear();
	for (int i=0;i<pCmd->dwSize;i++)
	{
		countryList.push_back(pCmd->country_list[i]);
	}

	for (int i=0;i<countryList.size();i++)
	{
		char szCountryName[MAX_NAMESIZE];
		if( CCountry::Instance()->GetCountryName(countryList[i].dwID,szCountryName))
		{
			m_pListBox->AddItem("",(void*)&countryList[i]);
			m_pListBox->SetItemText(m_pListBox->GetCount()-1,0,"  ",2);
			m_pListBox->SetItemText(m_pListBox->GetCount()-1,1,szCountryName);
		}
	}
}

void CGuiChangeWarField::OnClose(void)
{
	FUNCTION_BEGIN;

	if (m_eState==CGuiNpcDialog::eChangeWarField)
		GetGameGuiManager()->m_guiChangeWarField = NULL;
	else
		GetGameGuiManager()->m_guiChoiceWarField = NULL;
	
	
	GetGameGuiManager()->OnNpcActionEnd();
	GetGameGuiManager()->EndNpcDialog();

	FUNCTION_END;
}

void CGuiChangeWarField::OnCreate()
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();

	countryList.clear();

	GetButton(10)->SetEnabled(false);

	m_pListBox = GetListBox(2);

	FUNCTION_END;
}

bool CGuiChangeWarField::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
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
						stCountryInfo* pData = (stCountryInfo*) m_pListBox->GetItemData(m_pListBox->GetCurItem());
						if (pData)
						{
							if (m_eState==CGuiNpcDialog::eChangeWarField)
							{
								stTransDareCountryCmd cmd;
								cmd.dwCountryID = pData->dwID ;
								SEND_USER_CMD(cmd);
							}
							else
							{
								// 挑国战
								if( !CCountry::Instance()->IsAllyCountry(pData->dwID) )
								{
									stDareCountryFormalCmd cmd;
									cmd.dwCountryID = pData->dwID ;
									SEND_USER_CMD(cmd);
								}		
								else 
								{
									GetGameGuiManager()->AddClientSystemMessage("不能挑战盟国！");
								}
							}
						}
					}
				}
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
			if (m_eState==CGuiNpcDialog::eChangeWarField)
			{
				if (GetStatic(1))
				{
					stCountryInfo* pData = (stCountryInfo*) m_pListBox->GetItemData(m_pListBox->GetCurItem());
					if (pData)
					{
						if (pData->byType==0)
							GetStatic(1)->SetText("0");
						else
							GetStatic(1)->SetText("1");
					}
				}
			}
		}
		break;

	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}

void CGuiChangeWarField::OnSetCloseLocation()
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
