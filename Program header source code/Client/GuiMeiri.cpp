/**
*\file		GuiTop.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    经脉对话框
*
* 
*
*/
#include "public.h"
#include "../gui/include/GuiManager.h"	
#include "./GameGuiManager.h"
#include "./Game.h"
#include "./table.h"
#include "./RoleItem.h"
#include "./GuiMeiri.h"
#include <sys/timeb.h>
#include "./GameScene.h"
#include "Maincharacter.h"
///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuiMeiriDlg::CGuiMeiriDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuiMeiriDlg::~CGuiMeiriDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}
void CGuiMeiriDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();

	



	FUNCTION_END;
}


bool CGuiMeiriDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
			case 1:
			{
				SetVisible(false);
			}	
			break;
			case 501:
			case 502:
			case 503:
			case 504:
			case 505:
			case 506:
			case 507:
			case 508:
			{
				int num = nID - 500;
				CMainCharacter* pMainRole = (CMainCharacter*)GetScene()->GetMainCharacter();
				stMeiRiRenWuChuanSongUserCmd cmd1;
				if (pMainRole)
					cmd1.dwOldTempID = pMainRole->GetProperty()->userid;
				else
					cmd1.dwOldTempID = 0;
				cmd1.num = num;
				SEND_USER_CMD(cmd1);
			}
			break;
		}		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiMeiriDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiMeiri = NULL;

	FUNCTION_END;
}
