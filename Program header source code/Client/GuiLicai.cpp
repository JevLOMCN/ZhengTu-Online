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
#include "./GuiLicai.h"

///////////////////////////////////////////////////////////////////////////////
 
#define ID_BUTTON_CLOSE       10000   //关闭
///////////////////////////////////////////////////////////////////////////////

CGuiLicaiDlg::CGuiLicaiDlg()
{
	FUNCTION_BEGIN;

	m_bSmDialog = true;

	FUNCTION_END;
}

CGuiLicaiDlg::~CGuiLicaiDlg()
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

void CGuiLicaiDlg::OnCreate()
{
	FUNCTION_BEGIN;
	
	CGuiDialog::OnCreate();
	m_pTab = GetTab(7);
	m_pTab->AddItem("",NULL); //0  
	m_pTab->AddItem("",NULL); //1 
	m_pTab->AddItem("",NULL); //2  

    // bright moon 百兽图鉴
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(301));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(302));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(303));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(350));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(304));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(380));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(305));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(306));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(307));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(308));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(309));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(310));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(311));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(312));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(313));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(314));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(315));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(315));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(316));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(317));
	m_pTab->AddControl(0,(CGuiControl*)GetStatic(384));
	m_pTab->AddControl(0,(CGuiControl*)GetButton(23));
	m_pTab->AddControl(0,(CGuiControl*)GetEditBox(200));
	
	m_pTab->AddControl(1,(CGuiControl*)GetStatic(318));
	m_pTab->AddControl(1,(CGuiControl*)GetStatic(319));
	m_pTab->AddControl(1,(CGuiControl*)GetStatic(381));
	// m_pTab->AddControl(1,(CGuiControl*)GetStatic(382));
	m_pTab->AddControl(1,(CGuiControl*)GetStatic(320));
	m_pTab->AddControl(1,(CGuiControl*)GetStatic(321));
	m_pTab->AddControl(1,(CGuiControl*)GetStatic(322));
	m_pTab->AddControl(1,(CGuiControl*)GetStatic(323));
	m_pTab->AddControl(1,(CGuiControl*)GetStatic(324));
	m_pTab->AddControl(1,(CGuiControl*)GetStatic(325));
	m_pTab->AddControl(1,(CGuiControl*)GetStatic(326));
	m_pTab->AddControl(1,(CGuiControl*)GetStatic(327));
	m_pTab->AddControl(1,(CGuiControl*)GetStatic(328));
	m_pTab->AddControl(1,(CGuiControl*)GetStatic(329));
	m_pTab->AddControl(1,(CGuiControl*)GetStatic(330));
	m_pTab->AddControl(1,(CGuiControl*)GetStatic(331));
	m_pTab->AddControl(1,(CGuiControl*)GetStatic(332));
	m_pTab->AddControl(1,(CGuiControl*)GetStatic(383));
	m_pTab->AddControl(1,(CGuiControl*)GetButton(24));

	m_pTab->AddControl(2,(CGuiControl*)GetStatic(333));
	m_pTab->AddControl(2,(CGuiControl*)GetStatic(334));
	m_pTab->AddControl(2,(CGuiControl*)GetStatic(336));
	m_pTab->AddControl(2,(CGuiControl*)GetStatic(335));
	m_pTab->AddControl(2,(CGuiControl*)GetButton(25));


	m_pTab->SetCurItem(0);

	m_pTab->SetVisible(false);

	m_pEditBox = GetEditBox(200);
	m_pEditBox->SetStyle(CGuiEditBox::GUI_ES_NUMBER);
	FUNCTION_END;
}



bool CGuiLicaiDlg::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{		
			case 20://我要投保
			{
				m_pTab->SetCurItem(0);
			}
			break;
			case 21://我的账目
			{
				m_pTab->SetCurItem(1);
			}
			break;
			case 22://我的金库
			{
				m_pTab->SetCurItem(2);
			}
			break;
			case 99://关闭
			{
				SetVisible(false);
			}
			break;
		}
		
		return true;
	}
	if( nEvent == EVENT_EDITBOX_CHANGE && nID == 200 )
	{
		const char* pszText = m_pEditBox->GetText();
		int m_iInputPointNum = strtoul(pszText,NULL,10);
		point = m_iInputPointNum*1000;
		char szTemp[64];
		sprintf(szTemp,"%d锭%d两",point/10000, (point%10000)/100);
		GetStatic(380)->SetText(szTemp);
		
		//即刻返还
		sprintf(szTemp,"即刻返还：%d锭%d两",(int)(point*0.2)/10000, ((int)(point*0.2)%10000)/100);
		GetStatic(305)->SetText(szTemp);
		//60
		sprintf(szTemp,"60级：%d锭%d两",(int)(point*0.2)/10000, ((int)(point*0.2)%10000)/100);
		GetStatic(306)->SetText(szTemp);
		//70
		sprintf(szTemp,"70级：%d锭%d两",(int)(point*0.2)/10000, ((int)(point*0.2)%10000)/100);
		GetStatic(307)->SetText(szTemp);
		//80
		sprintf(szTemp,"80级：%d锭%d两",(int)(point*0.2)/10000, ((int)(point*0.2)%10000)/100);
		GetStatic(308)->SetText(szTemp);
		//90
		sprintf(szTemp,"90级：%d锭%d两",(int)(point*0.2)/10000, ((int)(point*0.2)%10000)/100);
		GetStatic(309)->SetText(szTemp);
		//100
		sprintf(szTemp,"100级：%d锭%d两",(int)(point*0.2)/10000, ((int)(point*0.2)%10000)/100);
		GetStatic(310)->SetText(szTemp);
		//110
		sprintf(szTemp,"110级：%d锭%d两",(int)(point*0.2)/10000, ((int)(point*0.2)%10000)/100);
		GetStatic(311)->SetText(szTemp);
		//120
		sprintf(szTemp,"120级：%d锭%d两",(int)(point*0.3)/10000, ((int)(point*0.3)%10000)/100);
		GetStatic(312)->SetText(szTemp);
		//130
		sprintf(szTemp,"130级：%d锭%d两",(int)(point*0.3)/10000, ((int)(point*0.3)%10000)/100);
		GetStatic(313)->SetText(szTemp);
		//140
		sprintf(szTemp,"140级：%d锭%d两",(int)(point*0.5)/10000, ((int)(point*0.5)%10000)/100);
		GetStatic(314)->SetText(szTemp);
		//150
		sprintf(szTemp,"150级：%d锭%d两",point/10000, (point%10000)/100);
		GetStatic(315)->SetText(szTemp);
		//160
		sprintf(szTemp,"160级：%d锭%d两",(int)(point*1.5)/10000, ((int)(point*1.5)%10000)/100);
		GetStatic(316)->SetText(szTemp);

		int allpoint = 0;
		allpoint = (int)(point*0.2)*7 + (int)(point*0.3)*2 +(int)(point*0.5) + point +  (int)point*1.5;
		//总计
		sprintf(szTemp,"%d锭%d两",allpoint/10000, (allpoint%10000)/100);
		GetStatic(384)->SetText(szTemp);
		return true;
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}

void CGuiLicaiDlg::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guilicai = NULL;

	FUNCTION_END;
}
