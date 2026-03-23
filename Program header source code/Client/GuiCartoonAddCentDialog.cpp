#include "public.h"
#include "GameGuiManager.h"
#include "./GameScene.h"
#include "./MainCharacter.h"
#include ".\GuiCartoonAddCentDialog.h"
#include "./CartoonPet.h"
#include "./RoleItem.h"

#define CartoonAddCent_SetLabelTxt(id,txt)	{CGuiStatic* pLabel = GetStatic(id); if( pLabel ) {pLabel->SetText(txt);}}
#define CartoonAddCent_SetLabelNum(id,num)	{char szTemp[64];if(num>=0){_snprintf(szTemp,64,"%s",Money2String(num));szTemp[63] = 0;}else{_snprintf(szTemp,64,"金子不够");szTemp[63] = 0;}CartoonAddCent_SetLabelTxt(id,szTemp);}


CGuiCartoonAddCentDialog::CGuiCartoonAddCentDialog(DWORD dwCartoonID)
{
	FUNCTION_BEGIN;
	m_pEditBox = NULL;
	m_bSmDialog = true;
	m_dwCartoonID = dwCartoonID;
	FUNCTION_END;
}

CGuiCartoonAddCentDialog::~CGuiCartoonAddCentDialog(void)
{
}

bool CGuiCartoonAddCentDialog::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		if( nID == 5 )
		{
			OnIDOK();
			return true;
		}
		if( nID == 6 )
		{
			Close();
			return true;
		}
	}
	if( nEvent == EVENT_EDITBOX_CHANGE )
	{
		int gold = GetInputGoldNum();
		if( gold > 0 )
		{
			long time = gold * 144;
			const char* timestr = Second2String(time);
			CartoonAddCent_SetLabelTxt(9,timestr);

			CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
			if( pMainRole )
			{
				int lastgold = pMainRole->GetGoldNum() - gold;				
				CartoonAddCent_SetLabelNum(10,lastgold);	//剩余金子
			}
		}
		
		return true;
	}
	return false;

	FUNCTION_END;
}

int  CGuiCartoonAddCentDialog::GetInputGoldNum()
{
	FUNCTION_BEGIN;

	char szTxt[32]={0};
	sprintf(szTxt,"%d%s%d%s%d%s",strtoul(GetEditBox(11)->GetText(),NULL,10),INGOT_TXT,
		strtoul(GetEditBox(12)->GetText(),NULL,10),TAEL_TXT,
		strtoul(GetEditBox(13)->GetText(),NULL,10),CENT_TXT);

	int nMoney= String2Money(szTxt);

	return nMoney;

	FUNCTION_END;
}

void CGuiCartoonAddCentDialog::OnClose(void)
{
	FUNCTION_BEGIN;
	GetGameGuiManager()->m_guiCartoonAddCentDlg = NULL;
	FUNCTION_END;
}

void CGuiCartoonAddCentDialog::OnCreate()
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();

	UpdateGoldNum();

	FUNCTION_END;
}

void CGuiCartoonAddCentDialog::OnIDOK()
{
	FUNCTION_BEGIN;
	int gold = GetInputGoldNum();
	if( gold > 0 && GetScene()->GetMainCharacter()->GetGoldNum() >= gold )
	{
		int time = gold * 144;
		CCartoonPet::AddCent(m_dwCartoonID,time);
		Close();
	}

	FUNCTION_END;
}


void CGuiCartoonAddCentDialog::UpdateGoldNum()
{
	FUNCTION_BEGIN;

	CMainCharacter* pMainRole = GetScene()->GetMainCharacter();
	if( pMainRole )
	{
		int gold = pMainRole->GetGoldNum();
		CartoonAddCent_SetLabelNum(10,gold);
	}	

	FUNCTION_END;
}
