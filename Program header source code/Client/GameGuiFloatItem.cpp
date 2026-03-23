#include "public.h"
#include "./GameGuiManager.h"
#include ".\gameguifloatitem.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGameGuiFloatItem::CGameGuiFloatItem(void)
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
CGameGuiFloatItem::~CGameGuiFloatItem(void)
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
void CGameGuiFloatItem::OnCreate()
{
	FUNCTION_BEGIN;

	CGuiFloatItem::OnCreate();
	m_pTable->m_iTableID = 0;
	m_pTable->m_iTableType = OBJECTCELLTYPE_MOUSE;

	FUNCTION_END;
}