#pragma once
#include "..\gui\include\guidialog.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CGuiShortcutItem :
	public CGuiDialog
{
public:
	CGuiShortcutItem(void);
	void OnClose(void);
	void OnCreate();
	void OnResetScreenSize();
	int m_iScreenWidth;
};
