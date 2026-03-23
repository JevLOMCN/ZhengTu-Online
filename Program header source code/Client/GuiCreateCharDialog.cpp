#include "public.h"
#include "GameGuiManager.h"
#include ".\guicreatechardialog.h"
#include "./game_script.h"
#include "../luabind/public.h"
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiCreateCharDialog::CGuiCreateCharDialog(void)
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiCreateCharDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiCreateCharDialog = NULL;

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
static void CancelCreate()
{
	FUNCTION_BEGIN;

	if(!GetGameGuiManager()->m_guiCreateCharDialog) return;
	CGuiCreateCharDialog* pDlg = GetGameGuiManager()->m_guiCreateCharDialog;
	pDlg->Close();

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
static void Create()
{
	FUNCTION_BEGIN;

	if(!GetGameGuiManager()->m_guiCreateCharDialog) return;
	CGuiCreateCharDialog* pDlg = GetGameGuiManager()->m_guiCreateCharDialog;

	
	CGuiEditBox* pEdtName = pDlg->GetEditBox(2);
	CGuiRadioButton* pRadSex = pDlg->GetRadioButton(3);
	CGuiComboBox* pComboCharType1 = pDlg->GetComboBox(6);
	CGuiComboBox* pComboCharType2 = pDlg->GetComboBox(7);

	if(!pEdtName || !pRadSex || !pComboCharType1 || !pComboCharType2)
		return;

	const char* szName = pEdtName->GetText();
	if(szName == NULL || szName[0] == 0 || szName[0] == ' ' || szName[strlen(szName)-1] == ' ')
	{
		GameMessageBox("Ãû×Ö²»ºÏ·¨£¡");
		return;
	}
	
	stCreateSelectUserCmd cmd;
	strncpy(cmd.strUserName,szName,sizeof(cmd.strUserName));
	cmd.byHairType = rand() & 8;
	cmd.byRGB = GetRandColor24();
	if(pRadSex->GetChecked())
	{
		cmd.charType = (WORD)pComboCharType1->GetItemData(pComboCharType1->GetCurItem());
	}
	else
	{
		cmd.charType = (WORD)pComboCharType2->GetItemData(pComboCharType2->GetCurItem());
	}
	SEND_USER_CMD(cmd);
	pDlg->SetEnabled(false);

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiCreateCharDialog::OnInitScript()
{
	FUNCTION_BEGIN;

	GameScriptInit(GetScriptEngine());
	using namespace luabind;
	module(GetScriptEngine())
	[
		def("CancelCreate",&CancelCreate),
		def("Create",&Create)
	];

	FUNCTION_END;
}