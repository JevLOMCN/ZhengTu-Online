#ifndef _LUABIND_INCLUDE_H
#define _LUABIND_INCLUDE_H

#ifdef _DEBUG
#undef new
#endif

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#ifndef LUABIND_NO_RTTI
#define LUABIND_NO_RTTI
#endif
//#define LUABIND_NO_EXCEPTIONS
#include "luabind/luabind.hpp"
#include "luabind/class.hpp"
#include "luabind/function.hpp"
#include "luabind/object.hpp"

#ifdef _DEBUG
#ifndef _GDIPLUS_H
#define new DEBUG_NEW
#endif
#endif

#endif