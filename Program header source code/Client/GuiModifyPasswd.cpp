#include "./public.h"
#include "./GameGuiManager.h"
#include "./guimodifypasswd.h"
#include "./game_script.h"

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
CGuiModifyPasswd::CGuiModifyPasswd(void)
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CGuiModifyPasswd::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiModifyPasswd = NULL;

	FUNCTION_END;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
static void ModifyPasswd()
{
	FUNCTION_BEGIN;

	if(!GetGameGuiManager()->m_guiModifyPasswd) return;
	CGuiDialog* pDlg = GetGameGuiManager()->m_guiModifyPasswd;
	stPasswordLogonUserCmd cmd;

	const char* pszAccount = get_lua_globalvalue<const char*>(*pDlg->GetScriptEngine(),"Account","");
	const char* pszPasswd = get_lua_globalvalue<const char*>(*pDlg->GetScriptEngine(),"Passwd","");
	const char* pszNewPasswd = get_lua_globalvalue<const char*>(*pDlg->GetScriptEngine(),"NewPasswd","");
	const char* pszNewRePasswd = get_lua_globalvalue<const char*>(*pDlg->GetScriptEngine(),"NewRePasswd","");

	if(!pszAccount || !pszPasswd || !pszNewPasswd || !pszNewRePasswd) return;

	if(pszAccount == NULL || pszAccount[0] == 0 || pszAccount[0] == ' ' || pszAccount[strlen(pszAccount)-1] == ' ')
	{
		GameMessageBox("’ ∫≈√˚≤ª∫œ∑®£°");
		return;
	}

	if(strcmp(pszNewPasswd ,pszNewRePasswd))
	{
		ShowMessage("¡Ω¥Œ ‰»Îµƒ√‹¬Î≤ª“ª—˘£°");
		return;
	}

	strncpy(cmd.strName,pszAccount,sizeof(cmd.strName));
	strncpy(cmd.strPassword,pszPasswd,sizeof(cmd.strPassword));
	strncpy(cmd.strNewPassword,pszNewPasswd,sizeof(cmd.strNewPassword));

	SEND_USER_CMD(cmd);
	pDlg->SetEnabled(false);
	

	FUNCTION_END;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
static void CancelModify()
{
	FUNCTION_BEGIN;

	if(GetGameGuiManager()->m_guiModifyPasswd)
		GetGameGuiManager()->m_guiModifyPasswd->Close();
	GetGameGuiManager()->AddGuiLogin();

	FUNCTION_END;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CGuiModifyPasswd::OnInitScript()
{
	FUNCTION_BEGIN;

	GameScriptInit(GetScriptEngine());
	lua_register_directclosure(*GetScriptEngine(),ModifyPasswd);
	lua_register_directclosure(*GetScriptEngine(),CancelModify);

	FUNCTION_END;
}