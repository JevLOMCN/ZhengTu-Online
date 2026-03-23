/**
 * \file	GuiHelp.cpp
 * \version $Id$
 * \author  $$$03¹«Ë¾
 *          xuzhaoh@tom.com
 * \date    13:7:2005   10:42
 * \brief   
 *
 *
 *
 */


#include "public.h"
#include "./GameGuiManager.h"
#include ".\guihelp.h"
#include "./GameAppation.h"
#include "./GuiSystem.h"
#include "./GuiMain.h"

#define IDC_HELP_SWITCH		3

CGuiHelp::CGuiHelp(void)
: m_pTextBox(NULL)
{
	m_bCloseIsHide = (true);
	memset(strFileName,0,MAX_PATH);
}

CGuiHelp::CGuiHelp(const char *pszFileName)
: m_pTextBox(NULL)
{
	memset(strFileName,0,MAX_PATH);
	if(pszFileName)
		strcpy(strFileName,pszFileName);
}



void CGuiHelp::OnClose(void)
{
	GetGameGuiManager()->m_guiHelp = NULL;
}

void CGuiHelp::OnCreate()
{
	CGuiDialog::OnCreate();
	m_pTextBox = GetMLTextBox(1);
	m_pTextBox->SetFont(0);

	m_ptIcon.x = 33;
	m_ptIcon.y = 90;

	bool bCheck = GetGameApplication()->bSystemSetted( USER_SETTING_SHOW_HELPTIP );
	UpdateHelpSwitchCheck(bCheck);
}

void CGuiHelp::UpdateHelpSwitchCheck(bool bCheck)
{
	CGuiCheckBox* p = GetCheckBox(IDC_HELP_SWITCH);
	if(p)	p ->SetChecked(bCheck);
}

void CGuiHelp::LoadFile(const char *pszFileName)
{
	if( pszFileName )
	{
		strcpy(strFileName,pszFileName);
		if(m_pTextBox) m_pTextBox->LoadDoc(pszFileName);
	}
	else
	{
		m_pTextBox->LoadDoc("data\\help\\index.xml");
	}
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
void CGuiHelp::OnSetCloseLocation()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnSetCloseLocation();

	if(m_pBtnClose)
	{
		stPointI pt(m_pBtnClose->GetLocation());

		m_pBtnClose->SetLocation(pt.x ,pt.y + 4);
	}

	FUNCTION_END;
}

bool CGuiHelp::OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl)
{
	FUNCTION_BEGIN;

	if(nEvent == EVENT_CHECKBOX_CHANGED)
	{
		if(nID == IDC_HELP_SWITCH)
		{
			CGuiCheckBox* p = GetCheckBox(IDC_HELP_SWITCH);
			if(p)
			{
				bool bCheck = p->GetChecked();
				if(bCheck) 
					GetGameApplication()->SetSystemSetOnServer(USER_SETTING_SHOW_HELPTIP);
				else 
				{
					GetGameApplication()->ClearSystemSetOnServer(USER_SETTING_SHOW_HELPTIP);	
					if(GetGameGuiManager()->m_guiMain)
						GetGameGuiManager()->m_guiMain->ShowAllTipBtn(false);
				}

				if(GetGameGuiManager()->m_guiSystem)
				{
					GetGameGuiManager()->m_guiSystem->SetServerData(GetGameApplication()->GetServerConfigData());
				}
			}
			
			return true;
		}
	}
	return CGuiDialog::OnGuiEvent(nEvent,nID,pControl);
	FUNCTION_END;
}