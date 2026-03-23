#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GuiPrenticeBountyDlg.h"
#include "./RoleItem.h"

#define  IDB_PRB_OK				7

#define  IDS_PRB_YOUR_MASTER	10
#define  IDS_PRB_PRE_LEVEL		11
#define  IDS_PRB_CURBOUNT		12
#define  IDS_PRB_SUMBOUNT		13

CGuiPrenticeBountyDlg::CGuiPrenticeBountyDlg(void)
{
	m_bSmDialog = true;
}

CGuiPrenticeBountyDlg::~CGuiPrenticeBountyDlg(void)
{
}

void CGuiPrenticeBountyDlg::OnCreate(void)
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();
	FUNCTION_END;
}

bool CGuiPrenticeBountyDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case IDB_PRB_OK : 
			{
				OnOk();
			}
			break;
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

HRESULT CGuiPrenticeBountyDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	return hr;
	FUNCTION_END;
}

void CGuiPrenticeBountyDlg::OnClose(void)
{
	FUNCTION_BEGIN;
	GetGameGuiManager()->m_guiPrenticeBountyDlg = NULL;
	CGuiDialog::OnClose();
	FUNCTION_END;
}

void CGuiPrenticeBountyDlg::OnOk()
{
	FUNCTION_BEGIN;
	Close();
	FUNCTION_END;
}

void CGuiPrenticeBountyDlg::OnRefreshUIData(stRtnPrenticeBountyUserCmd* pCmd)
{
	char temp[256] = {""};
	if (GetStatic(IDS_PRB_YOUR_MASTER)) 
	{
		sprintf(temp,"你的师傅:%s",pCmd->name);
		GetStatic(IDS_PRB_YOUR_MASTER)->SetText(temp);
	}
	if (GetStatic(IDS_PRB_PRE_LEVEL)) 
	{
		sprintf(temp,"上次进贡等级:%d",pCmd->dwLastLevel);
		GetStatic(IDS_PRB_PRE_LEVEL)->SetText(temp);
	}
	if (GetStatic(IDS_PRB_SUMBOUNT)) 
	{
		sprintf(temp,"累计进贡:%s",Money2String(pCmd->dwTotal));
		GetStatic(IDS_PRB_SUMBOUNT)->SetText(temp);
	}
	if (GetStatic(IDS_PRB_CURBOUNT)) 
	{
		sprintf(temp,"本次可提取:%s",Money2String(pCmd->dwBalance));
		GetStatic(IDS_PRB_CURBOUNT)->SetText(temp);
	}
}
