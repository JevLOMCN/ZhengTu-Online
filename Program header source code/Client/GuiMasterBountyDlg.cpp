#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GuiMasterBountyDlg.h"
#include "./RoleItem.h"

#define  IDB_MAB_OK		7

#define  IDS_MAB_SUMBOUNT		10
#define  IDS_MAB_CURBOUNT		11

CGuiMasterBountyDlg::CGuiMasterBountyDlg(void)
{
	m_bSmDialog = true;
}

CGuiMasterBountyDlg::~CGuiMasterBountyDlg(void)
{
}

void CGuiMasterBountyDlg::OnCreate(void)
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();
	FUNCTION_END;
}

bool CGuiMasterBountyDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case IDB_MAB_OK : 
			{
				OnOk();
			}
			break;
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

HRESULT CGuiMasterBountyDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	return hr;
	FUNCTION_END;
}

void CGuiMasterBountyDlg::OnClose(void)
{
	FUNCTION_BEGIN;
	GetGameGuiManager()->m_guiMasterBountyDlg = NULL;
	CGuiDialog::OnClose();
	FUNCTION_END;
}

void CGuiMasterBountyDlg::OnOk()
{
	FUNCTION_BEGIN;

	TRACE("@@@@@@---请求提取师徒系统--师傅的--奖励---wyc");
	stReqMasterBountyUserCmd cmd;
	cmd.byState = GET_SCHOOL_BOUNTY;
	SEND_USER_CMD(cmd);

	Close();
	FUNCTION_END;
}

void CGuiMasterBountyDlg::OnRefreshUIData(stRtnMasterBountyUserCmd* pCmd)
{
	char temp[256] = {""};
	if (GetStatic(IDS_MAB_SUMBOUNT)) 
	{
		sprintf(temp,"累计进贡:%s",Money2String(pCmd->dwTotal));
		GetStatic(IDS_MAB_SUMBOUNT)->SetText(temp);
	}
	if (GetStatic(IDS_MAB_CURBOUNT)) 
	{
		sprintf(temp,"本次可提取:%s",Money2String(pCmd->dwBalance));
		GetStatic(IDS_MAB_CURBOUNT)->SetText(temp);
	}
}
