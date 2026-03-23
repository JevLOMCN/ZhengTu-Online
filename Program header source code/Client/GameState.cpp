#include "public.h"
#include ".\gamestate.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGameState* GetGameState()
{
	FUNCTION_BEGIN;

	static CGameState gs;
	return &gs;

	FUNCTION_END;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGameState::CGameState(void)
: m_state(eGameState_Null)
, m_stateItem(eGameStateItem_Normal)
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
CGameState::~CGameState(void)
{
	FUNCTION_BEGIN;


	FUNCTION_END;
}
