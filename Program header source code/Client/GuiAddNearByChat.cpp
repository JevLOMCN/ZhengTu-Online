
#include "public.h"
#include "GameGuiManager.h"
#include ".\guiaddnearbychat.h"

CGuiAddNearByChat::CGuiAddNearByChat(const char* pszText,enumGuiActionType type,void* pData)
: m_strText(pszText)
, m_action(type)
, m_pData(pData)
{
}


/**
* \brief 关闭对话框
* 
* 关闭对话框
* 
* \return 无
*/
void CGuiAddNearByChat::OnClose(void)
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}

/**
* \brief 对话框创建
* 
* 对话框创建
* 
* \return 无
*/
void CGuiAddNearByChat::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	CGuiStatic* pText = GetStatic(1);
	Assert(pText);
	pText->SetText(m_strText.c_str());


	FUNCTION_END;
}
