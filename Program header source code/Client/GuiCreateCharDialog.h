#pragma once
#include "..\gui\include\guidialog.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CGuiCreateCharDialog :
	public CGuiDialog
{
public:
	CGuiCreateCharDialog(void);
	void OnClose(void);
	void OnInitScript();
};
