#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GuiRecoSys_A_Query_TakeDlg.h"
#include "./RoleItem.h"

#define  IDB_RAT_OK		7
#define  IDB_RAT_CANCLE 8

#define  IDS_RAT_YOURA_LIST		10
#define  IDS_RAT_GETLVL_LIST	11
#define  IDS_RAT_LEIJI_LIST		12
#define  IDS_RAT_KETI_LIST		13

#define  IDS_RAT_SUMBOUNT		14
#define  IDS_RAT_CURBOUNT		15

CGuiRecoSys_A_Query_TakeDlg::CGuiRecoSys_A_Query_TakeDlg(void)
{
	m_bSmDialog = true;
}

CGuiRecoSys_A_Query_TakeDlg::~CGuiRecoSys_A_Query_TakeDlg(void)
{
}

void CGuiRecoSys_A_Query_TakeDlg::OnCreate(void)
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();
	if (GetStatic(IDS_RAT_YOURA_LIST))
	{
		//GetStatic(IDS_RAT_YOURA_LIST)->SetVisible(false);
	}
	if (GetStatic(IDS_RAT_GETLVL_LIST))
	{
		//GetStatic(IDS_RAT_GETLVL_LIST)->SetVisible(false);
	}
	if (GetStatic(IDS_RAT_LEIJI_LIST))
	{
		//GetStatic(IDS_RAT_LEIJI_LIST)->SetVisible(false);
	}
	if (GetStatic(IDS_RAT_KETI_LIST))
	{
		//GetStatic(IDS_RAT_KETI_LIST)->SetVisible(false);
	}
	FUNCTION_END;
}

bool CGuiRecoSys_A_Query_TakeDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case IDB_RAT_OK : 
			{
				OnOk();
			}
			break;
		case IDB_RAT_CANCLE :
			{
				Close();
			}
			break;
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

HRESULT CGuiRecoSys_A_Query_TakeDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	return hr;
	FUNCTION_END;
}

void CGuiRecoSys_A_Query_TakeDlg::OnClose(void)
{
	FUNCTION_BEGIN;
	GetGameGuiManager()->m_guiRecoSys_A_Query_TakeDlg = NULL;
	CGuiDialog::OnClose();
	FUNCTION_END;
}

void CGuiRecoSys_A_Query_TakeDlg::OnOk()
{
	FUNCTION_BEGIN;

	TRACE("@@@@@@---请求提取推荐人奖励---wyc");
	stReqRecommendBountyUserCmd cmd;
	cmd.byState = GET_BOUNTY;
	SEND_USER_CMD(cmd);

	Close();
	FUNCTION_END;
}

void CGuiRecoSys_A_Query_TakeDlg::OnRefreshUIData(stRtnRecommendBountyUserCmd* pCmd)
{
	char temp[256] = {""};
	if (GetListBox(20))
	{
		GetListBox(20)->RemoveAllItems();
		for(int i=0;i<pCmd->dwSize;i++)
		{
			GetListBox(20)->AddItem("",NULL);
			GetListBox(20)->SetItemText(i,0,pCmd->data[i].name,17,GuiListBoxColFmt_Left) ; 
			sprintf(temp,"%d",pCmd->data[i].dwLevel);
			GetListBox(20)->SetItemText(i,1,temp,6,GuiListBoxColFmt_Left) ; 
			GetListBox(20)->SetItemText(i,2,Money2String(pCmd->data[i].dwTotal),17,GuiListBoxColFmt_Center) ;
		}
		
	}
	if (GetStatic(IDS_RAT_SUMBOUNT)) 
	{
		sprintf(temp,"累计奖励银子:%s",Money2String(pCmd->dwTotal));
		GetStatic(IDS_RAT_SUMBOUNT)->SetText(temp);
	}
	if (GetStatic(IDS_RAT_CURBOUNT)) 
	{
		sprintf(temp,"本次可提取银子:%s",Money2String(pCmd->dwBalance));
		GetStatic(IDS_RAT_CURBOUNT)->SetText(temp);
	}
}
