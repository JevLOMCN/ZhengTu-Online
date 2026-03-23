#include "public.h"
#include "../gui/include/guiTypes.h"
#include ".\gameguimanager.h"
#include "Game.h"
#include ".\GuiSaveBoxContinueDialog.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include "./roleitem.h"

#define IDC_ID_OK		11
#define IDC_ID_CANCEL	12
#define IDC_STATIC_COST	2
#define IDC_EDIT_DAYS	3

CGuiSaveBoxContinueDialog::CGuiSaveBoxContinueDialog(void)
{
	FUNCTION_BEGIN;
	
	m_byBoxId = 0;
	m_price = 1;

	FUNCTION_END;
}

CGuiSaveBoxContinueDialog::~CGuiSaveBoxContinueDialog(void)
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

void CGuiSaveBoxContinueDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiSaveBoxContinueDlg = NULL;

	FUNCTION_END;
}

void CGuiSaveBoxContinueDialog::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();	

	FUNCTION_END;
}

bool CGuiSaveBoxContinueDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	switch( nEvent )
	{
	case EVENT_BUTTON_CLICKED: if( OnBtnClick(nID) ) return true;		break;
	case EVENT_EDITBOX_CHANGE: return OnEditDaysChange();	break;
	case EVENT_EDITBOX_STRING: return OnGuiEvent(EVENT_BUTTON_CLICKED,IDC_ID_OK,GetButton(IDC_ID_OK)); break;
	default:	break;
	}
	return true;

	FUNCTION_END;
}

bool CGuiSaveBoxContinueDialog::OnBtnClick(int id)
{
	switch(id)
	{
	case IDC_ID_OK:
		{
			char cost[MAX_PATH];
			sprintf(cost,"%s",GetEditBox(IDC_EDIT_DAYS)->GetText());
			int days = 0;
			sscanf(cost,"%d",&days);
			//send days

			stStoreInfoNpcTradeUserCmd cmd;
			cmd.page = m_byBoxId ;
			cmd.day[0] = days;
			Client_SendCommand(&cmd,sizeof(stTradeUserCmd) + 2 * sizeof(BYTE),false);

			Close();//close the box
			return true;
		}

		break;
	case IDC_ID_CANCEL:
		Close();//close the box
		return true;
		break;
	}
	

	return false;
}

bool CGuiSaveBoxContinueDialog::OnEditDaysChange()
{
	//int days = GetEditBox(IDC_EDIT_DAYS)->Get
	char cost[MAX_PATH];
	sprintf(cost,"%s",GetEditBox(IDC_EDIT_DAYS)->GetText());
	int days = 0;
	sscanf(cost,"%d",&days);
	int iCose = days * m_price;
	sprintf(cost,"%d",iCose);
	GetStatic(IDC_STATIC_COST)->SetText(cost);
	return true;
}

 void CGuiSaveBoxContinueDialog::SetBoxID(BYTE box)
 {
	 m_byBoxId = box; 

	 CMainCharacter *pMainChar = GetScene()->GetMainCharacter();
	 if(pMainChar)
	 {
		 CRoleItem *pItem = pMainChar->GetSavingBoxItem();
		 if(pItem)
		 {
			m_price =  pItem->GetPrice();
		 }
	 }
 }