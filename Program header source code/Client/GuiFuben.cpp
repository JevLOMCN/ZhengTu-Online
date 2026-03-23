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
#include "./GuiFuben.h"

///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuiFubenDlg::CGuiFubenDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuiFubenDlg::~CGuiFubenDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

void CGuiFubenDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();

	m_pTab = GetTab(10);
	m_pTab->AddItem("",NULL);
	m_pTab->AddItem("",NULL);
	m_pTab->AddItem("",NULL);	

	m_pTab->AddControl(0,(CGuiControl*)GetButton(101));
	m_pTab->AddControl(0,(CGuiControl*)GetButton(102));
	m_pTab->AddControl(0,(CGuiControl*)GetButton(103));
	m_pTab->AddControl(0,(CGuiControl*)GetButton(104));
	m_pTab->AddControl(0,(CGuiControl*)GetButton(105));
	m_pTab->AddControl(0,(CGuiControl*)GetControl(1001));
	m_pTab->AddControl(0,(CGuiControl*)GetControl(1002));
	m_pTab->AddControl(0,(CGuiControl*)GetControl(1003));
	m_pTab->AddControl(0,(CGuiControl*)GetButton(77));
	m_pTab->AddControl(0,(CGuiControl*)GetButton(88));

	m_pTab->AddControl(1,(CGuiControl*)GetButton(201));
	m_pTab->AddControl(1,(CGuiControl*)GetButton(202));
	m_pTab->AddControl(1,(CGuiControl*)GetButton(203));
	m_pTab->AddControl(1,(CGuiControl*)GetButton(204));
	m_pTab->AddControl(1,(CGuiControl*)GetButton(205));
	m_pTab->AddControl(1,(CGuiControl*)GetControl(2001));
	m_pTab->AddControl(1,(CGuiControl*)GetButton(77));
	m_pTab->AddControl(1,(CGuiControl*)GetButton(99));
	

	m_pTab->AddControl(2,(CGuiControl*)GetButton(301));
	m_pTab->AddControl(2,(CGuiControl*)GetButton(302));
	m_pTab->AddControl(2,(CGuiControl*)GetButton(303));
	m_pTab->AddControl(2,(CGuiControl*)GetButton(304));
	m_pTab->AddControl(2,(CGuiControl*)GetButton(305));
	m_pTab->AddControl(2,(CGuiControl*)GetControl(3001));
	m_pTab->AddControl(2,(CGuiControl*)GetControl(3002));
	m_pTab->AddControl(2,(CGuiControl*)GetControl(3003));
	m_pTab->AddControl(2,(CGuiControl*)GetControl(3004));
	m_pTab->AddControl(2,(CGuiControl*)GetButton(77));
	m_pTab->AddControl(2,(CGuiControl*)GetButton(88));

	
	m_pTab->SetVisible(false);
	m_pTab->SetCurItem(0);
	fbcs1=0;
	fbcs2=0;
	fbcs3=0;
	fbcs4=0;
	fbcs5=0;
	select=1;
	select_fb();
	

	FUNCTION_END;
}


bool CGuiFubenDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
			case 11:
			{
				m_pTab->SetCurItem(0);
				select=1;
				select_fb();
			}	
			break;
			case 12:
			{
				m_pTab->SetCurItem(1);
				select=1;
				select_fb();
			}
			break;
			case 13:
			{
				m_pTab->SetCurItem(2);
				select=1;
				select_fb();
			}
			break;
			case 8://关闭
			{
				Close();
			}
			break;
			case 77:
			{
				stFubenBuyCmd cmd; //sky 请求指令
				cmd.select = select;
				SEND_USER_CMD(cmd);
			}
			break;
			case 88://进入副本
			{
				stFubenInCmd cmd; //sky 请求指令
				cmd.select = select;
				cmd.type = m_pTab->GetCurItem();
				SEND_USER_CMD(cmd);
				Close();
			}
			break;//因为少了个这
			case 99://参加活动
			{
				stFubenInCmd cmd; //sky 请求指令
				cmd.select = select;
				cmd.type = m_pTab->GetCurItem();
				SEND_USER_CMD(cmd);
				Close();
			}
			break;
		}
		if(nID>=101 && nID<=305)
		{
			select = nID%100;	
			select_fb();
		}
		
		
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiFubenDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiFuben = NULL;

	FUNCTION_END;
}

void CGuiFubenDlg::select_fb(void)
{
	FUNCTION_BEGIN;
	if (m_pTab->GetCurItem() == 0)
	{
		GetStatic(3101)->SetText("");
		GetStatic(3102)->SetText("");
		GetStatic(3103)->SetText("");
		GetStatic(3104)->SetText("");
		GetButton(77)->SetVisible(false);
		if (select == 1)
		{
			GetStatic(1001)->SetVisible(true);
			GetStatic(1002)->SetVisible(false);
			GetStatic(1003)->SetVisible(false);
			GetButton(88)->SetLocation(62, 340);

		
		}
		else if (select == 2)
		{
			GetStatic(1001)->SetVisible(false);
			GetStatic(1002)->SetVisible(true);
			GetStatic(1003)->SetVisible(false);
			GetButton(88)->SetLocation(60+GetButton(88)->GetWidth()+42, 340);

		}
		else if (select == 3)
		{
			GetStatic(1001)->SetVisible(false);
			GetStatic(1002)->SetVisible(false);
			GetStatic(1003)->SetVisible(true);
			GetButton(88)->SetLocation(60+GetButton(88)->GetWidth()*2+84, 340);
		
		
		}
	}
	else if (m_pTab->GetCurItem() == 1)
	{
		GetStatic(3101)->SetText("");
		GetStatic(3102)->SetText("");
		GetStatic(3103)->SetText("");
		GetStatic(3104)->SetText("");
		GetButton(77)->SetVisible(false);
		if (select == 1)
		{
			GetStatic(2001)->SetVisible(true);
			GetButton(99)->SetLocation(62, 340);
		}
	}
	else
	{
		char text[32];
		sprintf(text,"副本次数:%d次",fbcs1);
		GetStatic(3101)->SetText(text);
		sprintf(text,"副本次数:%d次",fbcs2);
		GetStatic(3102)->SetText(text);
		sprintf(text,"副本次数:%d次",fbcs3);
		GetStatic(3103)->SetText(text);
		sprintf(text,"副本次数:%d次",fbcs4);
		GetStatic(3104)->SetText(text);
		GetButton(77)->SetVisible(true);
		GetStatic(3101)->SetLocation(65,315);
		GetStatic(3102)->SetLocation(65+130,315);
		GetStatic(3103)->SetLocation(60+264,315);
		GetStatic(3104)->SetLocation(60+394,315);
		if (select == 1)
		{
			GetStatic(3001)->SetVisible(true);
			GetStatic(3002)->SetVisible(false);
			GetStatic(3003)->SetVisible(false);
			GetStatic(3004)->SetVisible(false);
			GetButton(88)->SetLocation(62, 340);
			GetButton(77)->SetLocation(140,310);
			
		}
		else if (select == 2)
		{
			GetStatic(3001)->SetVisible(false);
			GetStatic(3002)->SetVisible(true);
			GetStatic(3003)->SetVisible(false);
			GetStatic(3004)->SetVisible(false);
			GetButton(88)->SetLocation(60+GetButton(88)->GetWidth()+42, 340);
			GetButton(77)->SetLocation(270,310);
			
		}
		else if (select == 3)
		{
			GetStatic(3001)->SetVisible(false);
			GetStatic(3002)->SetVisible(false);
			GetStatic(3003)->SetVisible(true);
			GetStatic(3004)->SetVisible(false);
			GetButton(88)->SetLocation(60+GetButton(88)->GetWidth()*2+84, 340);
			GetButton(77)->SetLocation(400,310);
			
		}
		else if (select == 4)
		{
			GetStatic(3001)->SetVisible(false);
			GetStatic(3002)->SetVisible(false);
			GetStatic(3003)->SetVisible(false);
			GetStatic(3004)->SetVisible(true);
			GetButton(88)->SetLocation(60+GetButton(88)->GetWidth()*3+126, 340);
			GetButton(77)->SetLocation(530,310);
			
		}
	}

	FUNCTION_END;
}
