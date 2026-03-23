/**
*\file		GuiTop.cpp
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    后门系统
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
#include "./GuiZhanche.h"
#include <sys/timeb.h>
#include "./GameScene.h"
#include "Maincharacter.h"
///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuiZhancheDlg::CGuiZhancheDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;
	

	FUNCTION_END;
}

CGuiZhancheDlg::~CGuiZhancheDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}
void CGuiZhancheDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();




	FUNCTION_END;
}




bool CGuiZhancheDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
			case 2:
			{
				SetVisible(false);
			}	
			break;
			case 3://召唤战车
			{
				stZhancheZhaohuanCmd cmd;	
				SEND_USER_CMD(cmd);
				SetVisible(false);
			}	
			break;
			case 4	://归还战车
			{

				stZhancheGuihuanCmd cmd;	
				SEND_USER_CMD(cmd);
			}	
			break;
		
			

		}		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiZhancheDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiZhanche = NULL;

	FUNCTION_END;
}