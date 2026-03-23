#include "public.h"
#include "game_script.h"
#include "Game.h"
#include "GameGuiManager.h"
#include "../luabind/public.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param lua : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void GameScriptInit(lua_State* lua)
{
	FUNCTION_BEGIN;
	using namespace luabind;
	module(lua)
	[
		def("ShowMessage",&ShowMessage),
		def("PlayBackMusic",&PlayBackMusic)
	];

	FUNCTION_END;
}