#pragma once
#include "..\gui\include\guidialog.h"

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CGuiSelectCharDialog :
	public CGuiDialog
{
	std::vector<SelectUserInfo> m_aUserInfo;
public:
	CGuiSelectCharDialog(void);
	void OnClose(void);
	void OnCreate();
	void OnInitScript();
	void SetAttrib(SelectUserInfo * info);
	void UpdateValue();
	void LoginServer();
	void ExitLogin();
};
