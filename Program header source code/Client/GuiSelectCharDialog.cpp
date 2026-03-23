#include "public.h"
#include "./GameGuiManager.h"
#include ".\guiselectchardialog.h"
#include "./game_script.h"
#include "../luabind/public.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiSelectCharDialog::CGuiSelectCharDialog(void)
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
void CGuiSelectCharDialog::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiSelectChar = NULL;

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiSelectCharDialog::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
static void LoginServer()
{
	FUNCTION_BEGIN;

	if(GetGameGuiManager()->m_guiSelectChar)
		GetGameGuiManager()->m_guiSelectChar->LoginServer();

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
static void UpdateValue()
{
	FUNCTION_BEGIN;

	if(GetGameGuiManager()->m_guiSelectChar)
		GetGameGuiManager()->m_guiSelectChar->UpdateValue();

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
static void ExitLogin()
{
	FUNCTION_BEGIN;

	if(GetGameGuiManager()->m_guiSelectChar)
		GetGameGuiManager()->m_guiSelectChar->ExitLogin();

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
static void CreateChar()
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->AddCreateCharDialog();

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiSelectCharDialog::OnInitScript()
{
	FUNCTION_BEGIN;

	GameScriptInit(GetScriptEngine());
	using namespace luabind;
	module(GetScriptEngine())
	[
		def("LoginServer",&::LoginServer),
		def("UpdateValue",&::UpdateValue),
		def("ExitLogin",&::ExitLogin),
		def("CreateChar",&::CreateChar)
	];
	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param info : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiSelectCharDialog::SetAttrib(SelectUserInfo * info)
{
	FUNCTION_BEGIN;

	m_aUserInfo.reserve(MAX_CHARINFO);
	m_aUserInfo.resize(0);
	CGuiComboBox* pListName = GetComboBox(1);
	Assert(pListName);
	if(pListName) pListName->RemoveAllItems();
	for(size_t i=0;i<MAX_CHARINFO;++i)
	{
		if(info[i].id == 0) break;
		m_aUserInfo.push_back(info[i]);
		if(pListName) pListName->AddItem(info[i].name,NULL);
	}
	if(pListName)
	{
		if(pListName->GetItemCount())
			pListName->SetCurItem(0);
	}
	UpdateValue();

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiSelectCharDialog::UpdateValue()
{
	FUNCTION_BEGIN;

	CGuiComboBox* pListName = GetComboBox(1);
	if(!pListName) return ;
	int ix = pListName->GetCurItem();
	CGuiStatic * pCharType = GetStatic(3);
	CGuiStatic * pLevel = GetStatic(5);
	CGuiStatic * pMapName = GetStatic(7);
	if((size_t)ix >= m_aUserInfo.size())
	{
		if(pCharType) pCharType->SetText("");
		if(pLevel) pLevel->SetText("");
		if(pMapName) pMapName->SetText("");
	}
	else
	{
		SelectUserInfo & info = m_aUserInfo[ix];
		pCharType->SetText(GetCharTypeInfo(info.type).name);

		if(pLevel) pLevel->SetText(avar("%u",info.level));
		if(pMapName) pMapName->SetText(info.mapName);
	}

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiSelectCharDialog::LoginServer()
{
	FUNCTION_BEGIN;

	CGuiComboBox* pListName = GetComboBox(1);
	size_t ix;
	if(pListName && (ix = pListName->GetCurItem()) != -1 && ix < m_aUserInfo.size())
	{
		SelectUserInfo & info = m_aUserInfo[ix];
		stLoginSelectUserCmd cmd;
		cmd.charNo = ix;
		SEND_USER_CMD(cmd);
		SetEnabled(false);	
	}

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiSelectCharDialog::ExitLogin()
{
	FUNCTION_BEGIN;

	Close();
	GetClient()->Destroy();
	GetClient()->ResetState();
	GetGameGuiManager()->AddGuiLogin();

	FUNCTION_END;
}