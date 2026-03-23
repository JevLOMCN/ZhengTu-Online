#pragma once
#include "..\gui\include\guidialog.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CGuiModifyPasswd :
	public CGuiDialog
{
public:
	CGuiModifyPasswd(void);
	void OnClose(void);
	void OnInitScript();
};
