#pragma once
#include "..\gui\include\guidialog.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CGuiRegisterDialog :
	public CGuiDialog
{
public:
	CGuiRegisterDialog(void);
	void OnClose(void);
	void OnInitScript();
};
