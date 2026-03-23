#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GuiRecoSys_B_Query_TakeDlg.h"
#include "./RoleItem.h"
#include  "./GameScene.h"
#include "./MainCharacter.h"

#define  IDB_RBT_OK		7
#define  IDB_RBT_CANCLE 8

#define  IDS_RBT_YOURA    10
#define  IDS_RBT_PRELVL   11
#define  IDS_RBT_CURLVL   12
#define  IDS_RBT_SUMBOUNT 13
#define  IDS_RBT_CURBOUNT 14

CGuiRecoSys_B_Query_TakeDlg::CGuiRecoSys_B_Query_TakeDlg(void)
{
	m_bSmDialog = true;
}

CGuiRecoSys_B_Query_TakeDlg::~CGuiRecoSys_B_Query_TakeDlg(void)
{
}

void CGuiRecoSys_B_Query_TakeDlg::OnCreate(void)
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();
	FUNCTION_END;
}

bool CGuiRecoSys_B_Query_TakeDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case IDB_RBT_OK : 
			{
				OnOk();
			}
			break;
		case IDB_RBT_CANCLE :
			{
				Close();
			}
			break;
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

HRESULT CGuiRecoSys_B_Query_TakeDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	return hr;
	FUNCTION_END;
}

void CGuiRecoSys_B_Query_TakeDlg::OnClose(void)
{
	FUNCTION_BEGIN;
	GetGameGuiManager()->m_guiRecoSys_B_Query_TakeDlg = NULL;
	CGuiDialog::OnClose();
	FUNCTION_END;
}

void CGuiRecoSys_B_Query_TakeDlg::OnOk()
{
	FUNCTION_BEGIN;

	TRACE("@@@@@@---请求提取[被]推荐人奖励---wyc");
	stReqRecommendedBountyUserCmd cmd;
	cmd.byState = GET_BOUNTY;
	SEND_USER_CMD(cmd);

	Close();
	FUNCTION_END;
}

void CGuiRecoSys_B_Query_TakeDlg::OnRefreshUIData(stRtnRecommendedBountyUserCmd* pCmd)
{
	char temp[256] = {""};
	if (GetStatic(IDS_RBT_YOURA)) 
	{
		sprintf(temp,"你的推荐人:%s",
			pCmd->name);
		GetStatic(IDS_RBT_YOURA)->SetText(temp);
	}
	if (GetStatic(IDS_RBT_PRELVL)) 
	{
		sprintf(temp,"上次提取等级:%d级",
			pCmd->dwLastLevel);
		GetStatic(IDS_RBT_PRELVL)->SetText(temp);
	}
	if (GetStatic(IDS_RBT_SUMBOUNT)) 
	{
		
		sprintf(temp,"累计奖励银子:%s",Money2String(pCmd->dwTotal));
		GetStatic(IDS_RBT_SUMBOUNT)->SetText(temp);
	}
	if (GetStatic(IDS_RBT_CURBOUNT)) 
	{
		sprintf(temp,"本次可提取银子:%s",Money2String(pCmd->dwBalance));
		GetStatic(IDS_RBT_CURBOUNT)->SetText(temp);
	}
	if (GetStatic(IDS_RBT_CURLVL)) 
	{
		int curLevel = GetScene()->GetMainCharacter()->GetLevel();
		sprintf(temp,"当前等级:%d级",
			curLevel);
		GetStatic(IDS_RBT_CURLVL)->SetText(temp);
	}
}
