#include "public.h"
#include "./GameGuiManager.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include ".\guicountrywarquery.h"
#include "./Country.h"


CGuiCountryWarQuery::CGuiCountryWarQuery(void)
{
	
}

void CGuiCountryWarQuery::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiCountryWarQuery = NULL;

	GetGameGuiManager()->OnNpcActionEnd();
	GetGameGuiManager()->EndNpcDialog();

	FUNCTION_END;
}

void CGuiCountryWarQuery::OnCreate()
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();

	GetButton(1000)->SetButtonGroup(1);
	GetButton(1001)->SetButtonGroup(1);
	GetButton(1002)->SetButtonGroup(1);

	m_pTab = GetTab(1);
	m_pTab->AddItem("",NULL);
	m_pTab->AddItem("",NULL);
	m_pTab->AddItem("",NULL);

	m_pTab->AddControl(0,(CGuiControl*)GetListBox(100));
	m_pTab->AddControl(1,(CGuiControl*)GetListBox(200));
	m_pTab->AddControl(2,(CGuiControl*)GetListBox(300));

	m_pTab->SetVisible(false);

	OnGuiEvent(EVENT_BUTTON_CLICKED,1000,GetButton(1000));

	FUNCTION_END;
}

bool CGuiCountryWarQuery::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	CGuiDialog* pDlg = NULL;

	switch(nEvent)
	{
	case EVENT_BUTTON_CLICKED:
		{
			switch(nID) {
			case 1000:
				{
					m_pTab->SetCurItem(0);
					GetButton(1000)->SetChecked(true);
					SetBackImage(stResourceLocation("data\\interfaces.gl",53,51));
				}
				break;
			case 1001:
				{
					m_pTab->SetCurItem(1);
					GetButton(1001)->SetChecked(true);
					SetBackImage(stResourceLocation("data\\interfaces.gl",53,50));
				}
				break;
			case 1002:
				{
					m_pTab->SetCurItem(2);
					GetButton(1002)->SetChecked(true);
					SetBackImage(stResourceLocation("data\\interfaces.gl",53,52));
				}
				break;
			case 2:
				{
					OnGuiEvent(EVENT_BUTTON_CLICKED,-1,NULL);
				}
				break;
			}
		}	
		break;

	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);

	FUNCTION_END;
}


char*  CGuiCountryWarQuery::GetCountryName(DWORD dwID)
{
	static char name[MAX_NAMESIZE]={0};
	
	name[0]=0;

	CCountry::Instance()->GetCountryName(dwID,name);

	return name;

}

void CGuiCountryWarQuery::RefreshWarList(stReturnDareRecordStatCmd* pCmd)
{
	if (GetListBox(300)==NULL)
		return;

	GetListBox(100)->RemoveAllItems();
	for (int i=0;i<pCmd->dwSize;i++)
	{
		char szText[128]={0};

		GetListBox(300)->AddItem("",NULL);

		
		GetListBox(300)->SetItemText(GetListBox(300)->GetCount()-1,0,GetCountryName(pCmd->dare_stat[i].dwCountry),18,GuiListBoxColFmt_Center);
		sprintf(szText,"%d次",pCmd->dare_stat[i].dwFormalWin);
		GetListBox(300)->SetItemText(GetListBox(300)->GetCount()-1,1,szText,18,GuiListBoxColFmt_Center);
		sprintf(szText,"%d次",pCmd->dare_stat[i].dwFormalFail);
		GetListBox(300)->SetItemText(GetListBox(300)->GetCount()-1,2,szText,18,GuiListBoxColFmt_Center);
		sprintf(szText,"%d次",pCmd->dare_stat[i].dwAnnoyWin);
		GetListBox(300)->SetItemText(GetListBox(300)->GetCount()-1,3,szText,20,GuiListBoxColFmt_Center);
		sprintf(szText,"%d次",pCmd->dare_stat[i].dwAnnoyFail);
		GetListBox(300)->SetItemText(GetListBox(300)->GetCount()-1,4,szText,18,GuiListBoxColFmt_Center);
	}
}


#define   DATATIMESTR             "%d年%d月%d日20点"

void CGuiCountryWarQuery::RefreshWarPlanList(stReturnDareRecordPlanCmd* pCmd)
{
	if (GetListBox(100)==NULL)
		return;

	GetListBox(100)->RemoveAllItems();
	for (int i=0;i<pCmd->dwSize;i++)
	{
		
		char szText[128]={0};
		tm* chattime=NULL;
		DWORD times = pCmd->dare_plan[i].planTime + 86400;
		chattime=localtime((const time_t*)&times);

		sprintf(szText,DATATIMESTR,chattime->tm_year+1900,
			chattime->tm_mon+1,chattime->tm_mday);

		GetListBox(100)->AddItem("",NULL);
		GetListBox(100)->SetItemText(GetListBox(100)->GetCount()-1,0,szText,25,GuiListBoxColFmt_Center);
		GetListBox(100)->SetItemText(GetListBox(100)->GetCount()-1,1,GetCountryName(pCmd->dare_plan[i].attCountry),35,GuiListBoxColFmt_Center);
		GetListBox(100)->SetItemText(GetListBox(100)->GetCount()-1,2,GetCountryName(pCmd->dare_plan[i].defCountry),30,GuiListBoxColFmt_Center);
	}
}



#define   DATASTR             "%d年%d月%d日"

void CGuiCountryWarQuery::RefreshWarResultList(stReturnDareRecordResultCmd* pCmd)
{
	if (GetListBox(200)==NULL)
		return;

	GetListBox(200)->RemoveAllItems();
	for (int i=0;i<pCmd->dwSize;i++)
	{
		char szText[128]={0};
		tm* chattime=NULL;
		chattime=localtime((const time_t*)&(pCmd->dare_result[i].dareTime));

		sprintf(szText,DATASTR,chattime->tm_year+1900,
			chattime->tm_mon+1,chattime->tm_mday);

		GetListBox(200)->AddItem("",NULL);
		GetListBox(200)->SetItemText(GetListBox(200)->GetCount()-1,0,szText,18,GuiListBoxColFmt_Center);
		sprintf(szText,"%s %s",GetCountryName(pCmd->dare_result[i].attCountry),pCmd->dare_result[i].attKingName);
		GetListBox(200)->SetItemText(GetListBox(200)->GetCount()-1,1,szText,24,GuiListBoxColFmt_Center);
		sprintf(szText,"%s %s",GetCountryName(pCmd->dare_result[i].defCountry),pCmd->dare_result[i].defKingName);
		GetListBox(200)->SetItemText(GetListBox(200)->GetCount()-1,2,szText,23,GuiListBoxColFmt_Center);
		switch(pCmd->dare_result[i].byResult) {
		case 0:
			sprintf(szText,"%s攻城成功",GetCountryName(pCmd->dare_result[i].attCountry));
			break;
		case 1:
			sprintf(szText,"%s防御成功",GetCountryName(pCmd->dare_result[i].defCountry));
			break;
		case 2:
			sprintf(szText,"%s反攻成功",GetCountryName(pCmd->dare_result[i].defCountry));
			break;
		default:
			szText[0]=0;
		}
		GetListBox(200)->SetItemText(GetListBox(200)->GetCount()-1,3,szText,26,GuiListBoxColFmt_Center);
	}
}

