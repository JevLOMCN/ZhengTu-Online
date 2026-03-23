#include "public.h"
#include "./Game.h"
#include "./GameGuiManager.h"
#include "./GuiRecoSys_A_InputDlg.h"

#define  IDB_RAI_OK		7
#define  IDB_RAI_CANCLE 8

#define  IDE_RAI_NAME   10

CGuiRecoSys_A_InputDlg::CGuiRecoSys_A_InputDlg(void)
{
	m_bSmDialog = true;
}

CGuiRecoSys_A_InputDlg::~CGuiRecoSys_A_InputDlg(void)
{
}

void CGuiRecoSys_A_InputDlg::OnCreate(void)
{
	FUNCTION_BEGIN;
	CGuiDialog::OnCreate();
	if (GetEditBox(IDE_RAI_NAME))
	{
		GetEditBox(IDE_RAI_NAME)->SetMaxCharCount(MAX_NAMESIZE);
	}
	FUNCTION_END;
}

bool CGuiRecoSys_A_InputDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;
	if( nEvent == EVENT_BUTTON_CLICKED )
	{
		switch( nID )
		{
		case IDB_RAI_OK : 
			{
				OnOk();
			}
			break;
		case IDB_RAI_CANCLE :
			{
				Close();
			}
			break;
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

HRESULT CGuiRecoSys_A_InputDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);
	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	return hr;
	FUNCTION_END;
}

void CGuiRecoSys_A_InputDlg::OnClose(void)
{
	FUNCTION_BEGIN;
	GetGameGuiManager()->m_guiRecoSys_A_InputDlg = NULL;
	CGuiDialog::OnClose();
	FUNCTION_END;
}

void CGuiRecoSys_A_InputDlg::OnOk()
{
	FUNCTION_BEGIN;
	const char* szName = GetEditBox(IDE_RAI_NAME)->GetText();
	if(szName == NULL || szName[0] == 0 || szName[0] == ' ' || szName[strlen(szName)-1] == ' '
		|| (strchr(szName, '\'') || strchr(szName, ';') || strchr(szName, '\"')))
	{
		GameMessageBox("名字不合法！");
		return;
	}

	TRACE("@@@@@@---请求设置推荐人---wyc");
	stSetRecommendUserCmd cmd;
	strncpy(cmd.name,szName,sizeof(cmd.name));
	SEND_USER_CMD(cmd);

	Close();

	FUNCTION_END;
}
