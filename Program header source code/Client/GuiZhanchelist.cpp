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
#include "./GuiZhanchelist.h"
#include <sys/timeb.h>
#include "./GameScene.h"
#include "Maincharacter.h"
///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuiZhanchelistDlg::CGuiZhanchelistDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;
	

	FUNCTION_END;
}

CGuiZhanchelistDlg::~CGuiZhanchelistDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}
void CGuiZhanchelistDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();
	m_pListBoxZhanche = this->GetListBox(1);
	if (m_pListBoxZhanche)
	{
		m_pListBoxZhanche->RemoveAllItems();
	}



	FUNCTION_END;
}




bool CGuiZhanchelistDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
			case 6:
			{
				SetVisible(false);
			}	
			break;
			case 3://领取家族战车
			{
				int nCurItem = m_pListBoxZhanche->GetCurItem();
				if(nCurItem<20)
				{
					if(zhanche[nCurItem].id!=0)
					{
						if(zhanche[nCurItem].lingyongid!=0)
						{
							GameMessageBox( "此战车已经被他人领用" );
							return true;
						}
						else{
							stZhancheLingyongCmd cmd;
							cmd.id = zhanche[nCurItem].id;
							SEND_USER_CMD(cmd);

						}
					}
				}
				
			}	
			break;
			

		}		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiZhanchelistDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiZhanchelist = NULL;

	FUNCTION_END;
}

void CGuiZhanchelistDlg::shuaxin(void)
{	
	m_pListBoxZhanche->RemoveAllItems();
	int num=0;
	for(int i = 0;i < 20;i++ )
	{
		if(zhanche[i].id!=0)
		{
			char msg[521];
			bool ly = false;//领用状态
			if(zhanche[i].lingyongid != 0 )
			{

				sprintf(msg, "%s   领用中   %s", zhanche[i].name,zhanche[i].lingyongname);
				m_pListBoxZhanche->AddItem(msg, 0);
				m_pListBoxZhanche->SetItemColor(num,0,D3DCOLOR_ARGB(255,255,0,0)); //紫色
				num++;
			}
			else{
				sprintf(msg, "%s   待领用   %s", zhanche[i].name,zhanche[i].lingyongname);
				m_pListBoxZhanche->AddItem(msg, 0);
				m_pListBoxZhanche->SetItemColor(num,0,D3DCOLOR_ARGB(255,0,255,0)); //紫色
				num++;
			}
			
		
		}
		
	}
	

}
