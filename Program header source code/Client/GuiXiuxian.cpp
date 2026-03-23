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
#include "./GuiXiuxian.h"
#include <sys/timeb.h>
#include "./GameScene.h"
#include "Maincharacter.h"
///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuiXiuxianDlg::CGuiXiuxianDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuiXiuxianDlg::~CGuiXiuxianDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}
void CGuiXiuxianDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();

	



	FUNCTION_END;
}


bool CGuiXiuxianDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
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
			case 1800:
			{
				
			}
			break;
		}		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiXiuxianDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiXiuxian = NULL;

	FUNCTION_END;
}
