#ifndef _LUABIND_PUBLIC_H
#define _LUABIND_PUBLIC_H

#include "./luabind_include.h"
#include "./LuaVariant.h"
///*
class CLuaState{

	lua_State *m_luaState;
	int m_ErrFn;
	int m_nParseStatus;
private:

public:

	CLuaState(bool bOpenStdLib = true)
		: m_ErrFn(0)
		, m_nParseStatus(-1)
	{
		m_luaState = lua_open();
		if(bOpenStdLib) OpenStdLib();
		luabind::open(m_luaState);
	}

	~CLuaState()
	{
		//lua_setgcthreshold(m_luaState, 0);
		lua_close(m_luaState);
	}


	void OpenStdLib();

	lua_State* GetHandle()
	{
		return m_luaState;
	}

	operator lua_State*()
	{
		return m_luaState;
	}

	int ErrorFn()
	{
		return m_ErrFn;
	}

	bool Do()
	{
		if (m_nParseStatus == 0) {  /* parse OK? */
			m_nParseStatus = lua_pcall(m_luaState, 0, LUA_MULTRET, 0);  /* call main */
		}
		if (m_nParseStatus != 0) {
			lua_getglobal(m_luaState, "_ALERT");
			if (lua_isfunction(m_luaState, -1)) {
				lua_insert(m_luaState, -2);
				lua_call(m_luaState, 1, 0);
			}
			else {  /* no _ALERT function; print it on stderr */
				fprintf(stderr, "%s\n", lua_tostring(m_luaState, -2));
				lua_pop(m_luaState, 2);  /* remove error message and _ALERT */
			}
		}
		return m_nParseStatus == 0;
	}

	bool LoadString(const char* buffer)
	{
		return (m_nParseStatus = luaL_loadbuffer(m_luaState,buffer,strlen(buffer),"LuaWrap")) == 0;
	}

	bool DoFile(const char* filename)
	{
		return (m_nParseStatus = lua_dofile(m_luaState, filename)) == 0;
	}
	bool DoString(const char* buffer)
	{
		return DoBuffer(buffer, strlen(buffer));
	}
	bool DoBuffer(const char* buffer, size_t size)
	{
		return (m_nParseStatus = lua_dobuffer(m_luaState, buffer, size, "LuaWrap")) == 0;
	}

	template<typename R>
		R GetGlobal(const char* name)
	{
		luabind::object g = luabind::globals(m_luaState);
		return g[name];
	}

	template<typename R>
		void SetGlobal(const char* name, R value)
	{
		luabind::object g = luabind::globals(m_luaState);
		g[name] = value;
	}


	bool IsExistFunction(const char* name)
	{
		lua_getglobal(m_luaState, name);
		if(lua_type(m_luaState,-1) == LUA_TFUNCTION)
			return true;
		return false;
	}

	// Call Lua function
	//   func:	Lua function name
	//   R:		Return type. (void, float, double, int, long, bool, const char*, std::string)
	// Sample:	double f = lua.Call<double>("test0", 1.0, 3, "param");
	template<typename R>
		R Call(const char* func,R defVal)
	{
		if(IsExistFunction(func)){
			try{
				return luabind::call_function<R>(m_luaState,func);
			}catch(...){
				return defVal;
			}
		}
		return defVal;
	}

	template<typename R, typename P1>
		R Call(const char* func,R defVal, P1 p1)
	{
		if(IsExistFunction(func)){
			try{
				return luabind::call_function<R>(m_luaState,func,p1);
			}catch(...){
				return defVal;
			}
		}
		return defVal;
	}

	template<typename R, typename P1, typename P2>
		R Call(const char* func,R defVal, P1 p1, P2 p2)
	{
		if(IsExistFunction(func)){
			try{
				return luabind::call_function<R>(m_luaState,func,p1,p2);
			}catch(...){
				return defVal;
			}
		}
		return defVal;
	}

	template<typename R, typename P1, typename P2, typename P3>
		R Call(const char* func,R defVal, P1 p1, P2 p2, P3 p3)
	{
		if(IsExistFunction(func)){
			try{
				return luabind::call_function<R>(m_luaState,func,p1,p2,p3);
			}catch(...){
				return defVal;
			}
		}
			
		return defVal;
	}

	template<typename R, typename P1, typename P2, typename P3, typename P4>
		R Call(const char* func,R defVal, P1 p1, P2 p2, P3 p3, P4 p4)
	{
		if(IsExistFunction(func)){
			try{
				return luabind::call_function<R>(m_luaState,func,p1,p2,p3,p4);
			}catch(...){
				return defVal;
			}
		}
		return defVal;
	}

	template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
		R Call(const char* func,R defVal, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
	{
		if(IsExistFunction(func)){
			try{
				return luabind::call_function<R>(m_luaState,func,p1,p2,p3,p4,p5);
			}catch(...){
				return defVal;
			}
		}
			
		return defVal;
	}

	template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		R Call(const char* func,R defVal, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
	{
		if(IsExistFunction(func)){
			try{
				return luabind::call_function<R>(m_luaState,func,p1,p2,p3,p4,p5,p6);
			}catch(...){
				return defVal;
			}
		}
			
		return defVal;
	}

	template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		R Call(const char* func,R defVal, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
	{
		if(IsExistFunction(func)){
			try{
				return luabind::call_function<R>(m_luaState,func,p1,p2,p3,p4,p5,p6,p7);
			}catch(...){
				return defVal;
			}
		}
			
		return defVal;
	}

	template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		R Call(const char* func,R defVal, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
	{
		if(IsExistFunction(func)){
			try{
				return luabind::call_function<R>(m_luaState,func,p1,p2,p3,p4,p5,p6,p7,p8);
			}catch(...){
				return defVal;
			}
		}
		return defVal;
	}

	template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		R Call(const char* func,R defVal, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
	{
		if(IsExistFunction(func)){
			try{
				return luabind::call_function<R>(m_luaState,func,p1,p2,p3,p4,p5,p6,p7,p8,p9);
			}catch(...){
				return defVal;
			}
		}
			
		return defVal;
	}

	template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
		R Call(const char* func,R defVal, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
	{
		if(IsExistFunction(func)){
			try{
				return luabind::call_function<R>(m_luaState,func,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10);
			}catch(...){
				return defVal;
			}
		}
			
		return defVal;
	}


	void CallRTVoid(const char* func)
	{
		if(IsExistFunction(func))
			luabind::call_function<void>(m_luaState,func);
	}

	template< typename P1>
		void CallRTVoid(const char* func, P1 p1)
	{
		if(IsExistFunction(func))
			luabind::call_function<void>(m_luaState,func,p1);
	}

	template< typename P1, typename P2>
		void CallRTVoid(const char* func, P1 p1, P2 p2)
	{
		if(IsExistFunction(func))
			luabind::call_function<void>(m_luaState,func,p1,p2);
	}

	template< typename P1, typename P2, typename P3>
		void CallRTVoid(const char* func, P1 p1, P2 p2, P3 p3)
	{
		if(IsExistFunction(func))
			luabind::call_function<void>(m_luaState,func,p1,p2,p3);
	}

	template< typename P1, typename P2, typename P3, typename P4>
		void CallRTVoid(const char* func, P1 p1, P2 p2, P3 p3, P4 p4)
	{
		if(IsExistFunction(func))
			luabind::call_function<void>(m_luaState,func,p1,p2,p3,p4);
	}

	template< typename P1, typename P2, typename P3, typename P4, typename P5>
		void CallRTVoid(const char* func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
	{
		if(IsExistFunction(func))
			luabind::call_function<void>(m_luaState,func,p1,p2,p3,p4,p5);
	}

	template< typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		void CallRTVoid(const char* func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
	{
		if(IsExistFunction(func))
			luabind::call_function<void>(m_luaState,func,p1,p2,p3,p4,p5,p6);
	}

	template< typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		void CallRTVoid(const char* func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
	{
		if(IsExistFunction(func))
			luabind::call_function<void>(m_luaState,func,p1,p2,p3,p4,p5,p6,p7);
	}

	template< typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		void CallRTVoid(const char* func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
	{
		if(IsExistFunction(func))
			luabind::call_function<void>(m_luaState,func,p1,p2,p3,p4,p5,p6,p7,p8);
	}

	template< typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		void CallRTVoid(const char* func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
	{
		if(IsExistFunction(func))
			luabind::call_function<void>(m_luaState,func,p1,p2,p3,p4,p5,p6,p7,p8,p9);
	}

	template< typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
		void CallRTVoid(const char* func, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
	{
		if(IsExistFunction(func))
			luabind::call_function<void>(m_luaState,func,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10);
	}
};//*/

inline bool lua_exist_func(lua_State* L,const char* name)
{
	lua_getglobal(L, name);
	if(lua_type(L,-1) == LUA_TFUNCTION)
		return true;
	return false;
}

#endif