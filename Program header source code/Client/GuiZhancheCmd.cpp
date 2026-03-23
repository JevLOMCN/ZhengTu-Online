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
#include "./GuiZhancheCmd.h"
#include <sys/timeb.h>
#include ".\guimain.h"
#include "./GameScene.h"
#include "Maincharacter.h"
///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuiZhancheCmdDlg::CGuiZhancheCmdDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;
	

	FUNCTION_END;
}

CGuiZhancheCmdDlg::~CGuiZhancheCmdDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}
void CGuiZhancheCmdDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();

	
	OnResetScreenSize();
	num = 0; 

	FUNCTION_END;
}


HRESULT CGuiZhancheCmdDlg::OnRender(float fElapsedTime)
{
	FUNCTION_BEGIN;

	
	HRESULT hr = CGuiDialog::OnRender(fElapsedTime);

	std::vector< CCharacter*, std::stack_allocator<CCharacter*> > aCharacter;
	GetScene()->GetObjectOrderByDistance(aCharacter,type2type_t<CCharacter>());
	if(IsVisible())
	{
		// if(num != aCharacter.size())
		// {
		// 	num = aCharacter.size();
			
			stZhancheSkillCmd cmd;
			for(int i=0;i<aCharacter.size();i++)
			{
				if(i>=100)
				{
					break;
				}
				CCharacter* pChar = aCharacter[i];
				cmd.playerid[i] = pChar->GetID();
			}
			cmd.type = GetScene()->GetMainCharacter()->GetProperty()->zhanchetype;;
			SEND_USER_CMD(cmd);
		// }
	}
	

	
	return hr;

	FUNCTION_END;
}

bool CGuiZhancheCmdDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
			
			case 18	://脱离战车
			{
				stZhancheTuoliCmd cmd;	
				SEND_USER_CMD(cmd);
				SetVisible(false);	

			}	
			break;
		
			

		}		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiZhancheCmdDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiZhancheCmd = NULL;

	FUNCTION_END;
}

/**
 * \brief 重设对话框尺寸
 * 
 * 
 * 
 * \return 
 */
void CGuiZhancheCmdDlg::OnResetScreenSize()
{
	
	//Set pos follow chat select dlg
	if ( GetGameGuiManager()->m_guiMain )
	{
		//m_x = GetGameGuiManager()->m_guiChatSelect->GetLocation().x + 422;
		m_x = GetGameGuiManager()->m_guiMain->GetLocation().x + 22;  //修改新增快捷栏x位置
		m_y = GetGameGuiManager()->m_guiMain->GetLocation().y + 717 - GetHeight();
	}
	else
	{
			//m_x = 422;
		m_x = 0;
		m_y = 684 - GetHeight();
	}

	//ResetDlgLocation();

	BerthHide();
}
