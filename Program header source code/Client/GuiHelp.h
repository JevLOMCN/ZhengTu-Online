/**
 * \file	GuiHelp.h
 * \version $Id$
 * \author  $$$03¹«Ë¾
 *          xuzhaoh@tom.com
 * \date    13:7:2005   10:42
 * \brief   
 *
 *
 *
 */


#pragma once
#include "..\gui\include\guidialog.h"

class CGuiHelp :
	public CGuiDialog
{
public:
	CGuiHelp(void);
	CGuiHelp(const char *pszFileName);
	void OnClose(void);
	void OnCreate();
	void OnSetCloseLocation();
	void UpdateHelpSwitchCheck(bool bCheck);
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);

	CGuiMLTextBox* m_pTextBox;

	void LoadFile(const char *pszFileName);
private:
	char strFileName[MAX_PATH];
};
