#include "./public.h"

#ifndef LUA_EXTRALIBS
#define LUA_EXTRALIBS /* empty */
#endif

const luaL_reg lualibs[] = 
{
	{"base", luaopen_base},

#ifdef _DEBUG
	{"io", luaopen_io},
	{"debug", luaopen_debug},
#endif

	{"string", luaopen_string},

	{"math", luaopen_math},
	{"table",luaopen_table},
	{"loadlib", luaopen_loadlib},
	/* add your libraries here */
	LUA_EXTRALIBS
	{NULL, NULL}
};

void CLuaState::OpenStdLib()
{
	assert(m_luaState);
	extern const luaL_reg lualibs[];
	const luaL_reg *lib = lualibs;
	for (; lib->func; lib++)
	{
		lib->func(m_luaState);  /* open library */
		lua_settop(m_luaState, 0);  /* discard any results */
	}
}