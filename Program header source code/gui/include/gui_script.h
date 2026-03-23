#ifndef _GUI_SCRIPT_H
#define _GUI_SCRIPT_H

//#include "../../LuaWrapper/lua_State.h"
typedef void* tGuiControlHandle;
typedef void* tGuiDialogHandle;
class CLuaState;
extern void GuiScriptInit(lua_State * lua);
extern void GuiScriptUninit();

#endif