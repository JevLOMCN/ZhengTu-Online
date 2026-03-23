#include "public.h"
#include "./GameGuiManager.h"
#include ".\guishortcutitem.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiShortcutItem::CGuiShortcutItem(void)
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
void CGuiShortcutItem::OnClose(void)
{
	FUNCTION_BEGIN;

	GetGameGuiManager()->m_guiShortcutItem = NULL;

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiShortcutItem::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiDialog::OnCreate();
	m_iScreenWidth = -1;
	OnResetScreenSize();

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiShortcutItem::OnResetScreenSize()
{
	FUNCTION_BEGIN;

	int iScreenWidth = GetDevice()->GetWidth();
	m_iScreenWidth = iScreenWidth;
	int iGroup;

	if(m_iScreenWidth == 800)
	{
		iGroup = 1;
	}
	else
	{
		iGroup = 1;
	}

	stResourceLocation rl;
	rl.group = iGroup;
	rl.frame = 66;
	rl.SetFileName(GetGuiGraphicPackName());
	SetBackImage(rl,NULL);
	UpdateLocation();

	FUNCTION_END;
}