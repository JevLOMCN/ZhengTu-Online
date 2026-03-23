#include "public.h"
#include "GameGuiManager.h"
#include ".\guiregisterdialog.h"
#include "./GuiLogin.h"
#include "./Game.h"
#include "./game_script.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CGuiRegisterDialog::CGuiRegisterDialog(void)
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiRegisterDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiRegisterDialog = NULL;

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
static void Register()
{
	FUNCTION_BEGIN;

	if(!GetGameGuiManager()->m_guiRegisterDialog)
		return;
	CGuiRegisterDialog* pDlg = GetGameGuiManager()->m_guiRegisterDialog;
	const char* pszAccount = get_lua_globalvalue<const char*>(*pDlg->GetScriptEngine(),"Account","");
	const char* pszPasswd = get_lua_globalvalue<const char*>(*pDlg->GetScriptEngine(),"Passwd","");
	const char* pszRePasswd = get_lua_globalvalue<const char*>(*pDlg->GetScriptEngine(),"RePasswd","");
	
	if(pszAccount == NULL || pszAccount[0] == 0 || pszAccount[0] == ' ' || pszAccount[strlen(pszAccount)-1] == ' ')
	{
		GameMessageBox("帐号名不合法！");
		return;
	}
	if(pszPasswd == NULL || pszPasswd[0] == 0 || pszPasswd[0] == ' ' || pszPasswd[strlen(pszPasswd)-1] == ' ')
	{
		GameMessageBox("密码不合法！");
		return;
	}
	if(strcmp(pszPasswd,pszRePasswd))
	{
		GameMessageBox("两次输入密码不相同！");
		return;
	}
	if(!ConnectionLoginServer())
	{
		GameMessageBox("连接服务器失败！");
		return;
	}

	stAccountLogonUserCmd cmd;
	strncpy(cmd.strName,pszAccount,sizeof(cmd.strName));
	strncpy(cmd.strPassword,pszPasswd,sizeof(cmd.strPassword));

	SEND_USER_CMD(cmd);

	pDlg->SetEnabled(false);

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
static void CancelRegister()
{
	FUNCTION_BEGIN;

	
	GetGameGuiManager()->m_guiRegisterDialog->Close();
	GetGameGuiManager()->AddGuiLogin();

	FUNCTION_END;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CGuiRegisterDialog::OnInitScript()
{
	FUNCTION_BEGIN;

	GameScriptInit(GetScriptEngine());
	lua_register_directclosure(*GetScriptEngine(),CancelRegister);
	lua_register_directclosure(*GetScriptEngine(),Register);

	FUNCTION_END;
}