/**
* \file      engine.h
* \version   $Id$
* \author    $$$03公司
*            $$$03公司@###.com
* \date      2006-7-19  21:11:00
* \brief     ********************************************************************
* 
*	      ********************************************************************
*/

#ifndef _XENGINE_H
#define _XENGINE_H


/*
#ifndef _LIB

	#ifdef ENGINE_EXPORTS

	#define ENGINE_API __declspec(dllexport)

	#else

	#define ENGINE_API __declspec(dllimport)

	#endif

#else

	#define ENGINE_API

#endif*/
#define ENGINE_API 
template <typename T>
struct type2type_t
{
	typedef T U;
};

const float PI = 3.14159265358979f;

// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。
//MAKE
#ifndef WINVER
#define _WIN32_WINNT 0x0501
#endif
#ifndef _WIN32_WINNT // Allow use of features specific to Windows NT 4 or later.
 #define _WIN32_WINNT 0x0501
 #endif
 #ifndef _WIN32_WINDOWS // Allow use of features specific to Windows 98 or later.
 #define _WIN32_WINDOWS 0x0501
 #endif
 #ifndef _WIN32_IE // Allow use of features specific to IE 4.0 or later.
 #define _WIN32_IE 0x0601
 #endif

typedef unsigned __int64 QWORD;
typedef short SWORD;

#ifdef ATTENTION
	#error def ATTENTION
#endif

#ifndef DT_UNDERLINE
	#define DT_UNDERLINE 0x00002000
#endif

#define ITOA(NUMBER)  #NUMBER
#define __TEXT_LINE__(LINE) ITOA(LINE)

#ifdef _DEBUG
	#define ATTENTION(MSG) __FILE__"("__TEXT_LINE__(__LINE__)") : ★ATTENTION★ → "#MSG
#else
	#define ATTENTION(MSG) __FILE__"("__TEXT_LINE__(__LINE__)") : ATTENTION!! error: "#MSG
#endif

#include <vector>
#include <string>
#include <map>
#include <hash_map>
#include <list>
#include <fstream>
#include <set>

#include <winsock2.h>
#include <windows.h>
#include <mmsystem.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <mbstring.h>
#include <tchar.h>
#include <malloc.h>


#ifndef _NO_GRAPHICS
	#include <d3d8.h>
	#include <d3dx8.h>
	#include <Dxerr8.h>
	#include <d3dx8math.h>
#endif


#ifdef _DEBUG

#ifndef TRACE
#define TRACE MyOutputDebugString
#endif

//MAKE
#if _MSC_VER <= 1310
	#define AssertFatal(exp,str) _ASSERT_BASE( (exp) ,str)
	#define Assert(expr) _ASSERT_BASE((expr), #expr)
#else
	#define AssertFatal(exp,str) _ASSERT_BASE( (exp) ,L#str)
	#define Assert(expr) _ASSERT_BASE((expr), L#expr)
#endif

#else

#ifndef TRACE
#define TRACE __noop
#endif

#define AssertFatal __noop
#define Assert __noop

#endif

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
	#ifndef DEBUG_NEW
		#define DEBUG_NEW new(_NORMAL_BLOCK,__FILE__, __LINE__)
	#endif
#endif


#define AssertCreateCtrl(ctrl,str) if( NULL == ctrl ) {Assert(0);Engine_WriteLog(str);GetGameGuiManager()->AddMessageBox(str);Close();return;}


/**
* \brief 简短描述
* 
* 详细描述
* 
* \param _Ptr : 描述
* \return 返回值的描述
*/
template<class T> 
inline	void constructInPlace(T  *_Ptr)
{	// construct object at _Ptr with value _Val
	new ((void  *)_Ptr) T( T() );
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param _Ptr : 描述
* \param param : 描述
* \return 返回值的描述
*/
template<class _Ty,class _TParam> 
inline	void constructInPlace(_Ty  *_Ptr,_TParam param)
{	// construct object at _Ptr with value _Val
	new ((void  *)_Ptr) _Ty( param );
}

template<class _Ty,class _TParam1,class _TParam2> 
inline	void constructInPlace(_Ty  *_Ptr,_TParam1 param1,_TParam2 param2)
{	// construct object at _Ptr with value _Val
	new ((void  *)_Ptr) _Ty( param1,param2 );
}

template<class _Ty,class _TParam1,class _TParam2,class _TParam3> 
inline	void constructInPlace(_Ty  *_Ptr,_TParam1 param1,_TParam2 param2,_TParam3 param3)
{	// construct object at _Ptr with value _Val
	new ((void  *)_Ptr) _Ty( param1,param2 ,param3);
}

template<class _Ty,class _TParam1,class _TParam2,class _TParam3,class _TParam4> 
inline	void constructInPlace(_Ty  *_Ptr,_TParam1 param1,_TParam2 param2,_TParam3 param3,_TParam4 param4)
{	// construct object at _Ptr with value _Val
	new ((void  *)_Ptr) _Ty( param1,param2 ,param3,param4);
}

template<class _Ty,class _TParam1,class _TParam2,class _TParam3,class _TParam4,class _TParam5> 
inline	void constructInPlace(_Ty  *_Ptr,_TParam1 param1,_TParam2 param2,_TParam3 param3,_TParam4 param4,_TParam5 param5)
{	// construct object at _Ptr with value _Val
	new ((void  *)_Ptr) _Ty( param1,param2 ,param3,param4,param5);
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param p : 描述
* \return 返回值的描述
*/
template <class T>
inline void destructInPlace(T* p)
{
	p->~T();
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param _Ptr : 描述
* \param count : 描述
* \return 返回值的描述
*/
template<class _T> 
inline	void constructInPlaceCount(_T  *_Ptr,size_t count)
{	// construct object at _Ptr with value _Val
	for(size_t i=0;i<count;++i)
	{
		constructInPlace(_Ptr++);
	}
}


/**
* \brief 简短描述
* 
* 详细描述
* 
* \param p : 描述
* \param count : 描述
* \return 返回值的描述
*/
template <class T>
inline void destructInPlaceCount(T* p,size_t count)
{
	for(size_t i=0;i<count;++i)
	{
		destructInPlace(p++);
	}
}

#include "lookaside_alloc.h"
#include "stack_alloc.h"


typedef std::string String;

#ifdef _DEBUG
#ifndef _GDIPLUS_H
#define new DEBUG_NEW
#endif
#endif

#define BIT(i) (1 << (i))

#define count_of(X) (sizeof(X)/sizeof((X)[0]))

#ifndef MAKEFOURCC
	#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
		((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
		((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
#define SAFE_RELEASE(p)  if(p) { (p)->Release(); (p) = NULL; }
#define SAFE_DELETE(p) { delete (p); (p) = NULL; }
#define SAFE_DELETEARRAY(p) { delete [] (p); (p) = NULL; }
#define SAFE_FREE(p) { free(p); (p) = NULL; }

#define _file_line_ __FILE__,__LINE__
#define Debug_Bool(b) {if(b) TRACE("%s(%d)\t%s\ttrue\n",_file_line_,#b);else TRACE("%s(%d)\t%s\tfalse\n",_file_line_,#b);}
#define Debug_Int(i) {TRACE("%s(%d)\t%s\t%d\n",_file_line_,#i,i);}
#define Debug_Float(f) {TRACE("%s(%d)\t%s\t%f\n",_file_line_,#f,f);}
#define Debug_String(str) {TRACE("%s(%d)\t%s:[%s]\n",_file_line_,#str,str);}
#define Debug_Color(c) {TRACE("%s (%d)\t%s\t%d,%d,%d,%d\n",__FILE__,__LINE__,#c,COLOR_GET_ALPHA(c),COLOR_GET_R(c),COLOR_GET_G(c),COLOR_GET_B(c));}

/**
* \brief 释放一个指针列表
* 
* 彻底释放一个指针列表
*
* \param TList : 代表一个vecotr类型
* \param list : 代表这个vector的一个实例列表
* \return 返回值的描述
*/
template< class TList >
inline void FreePointerList(TList& list)
{
	for(TList::iterator it = list.begin();it != list.end();++it)
	{
		SAFE_DELETE((*it));
	}
	list.clear();
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param data : 描述
* \param size : 描述
* \return 返回值的描述
*/
inline void* bzero(void* data,size_t size)
{
	return memset(data,0,size);
}

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lParam)	((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lParam)	((int)(short)HIWORD(lParam))
#endif

#define ROUNDNUM2(value,num) ( ((value) + (num-1)) & (~(num-1)) )

extern const int c_nTabSize;

extern bool Copy2Clipboard(const char* pstr);

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param v1 : 描述
* \param v2 : 描述
* \return 返回值的描述
*/
template <class _T>
_T inline getMin(_T v1,_T v2)
{
	return (v1 < v2 ? v1 : v2);
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param v1 : 描述
* \param v2 : 描述
* \return 返回值的描述
*/
template <class _T>
_T inline getMax(_T v1,_T v2)
{
	return (v1 > v2 ? v1 : v2);
}


extern void MyOutputDebugString(const char* pszFmt,...);

extern void TraceDXError(HRESULT hr,const char* file,int line);

#define TRACE_DXERROR(hr) TraceDXError(hr,__FILE__,__LINE__)

const char* avar(const char* pszFmt,...);

extern bool g_bUseD3DRender;

#include <time.h>

#include "stringex.h"
#include "engine_color.h"
#include "engine_core.h"
#include "SimpleList.h"
#include "./streamtypes.h"
#include "./FilePackHelper.h"
#include "timer.h"

#ifndef _NO_GRAPHICS
	#include "idevice.h"
	#include "ibitmap.h"
	#include "ibitmaps.h"
	#include "ibitmaplist.h"
	#include "ILightMap.h"
	#include "Appation.h"
#include "../include/graphic.h"
#endif

#include "enginestate.h"
#include "frameallocator.h"
#include "Ini.h"
#include "Lock.h"
#include "Log.h"
#include "../../xml_parse/tinyxml.h"

#ifndef _NO_GRAPHICS
	class IDevice;
	/**
	* \brief 简短描述
	* 
	* 详细描述
	* 
	*/
	class IGraphicCursor;
	inline IGraphicCursor* GetGraphicCursor(){
		extern IGraphicCursor* g_pGCursor;
		return g_pGCursor;
	}
	class ILightMap;
	inline IDevice* GetDevice()
	{
		extern IDevice* g_pDevice;
		return g_pDevice;
	}

	inline ILightMap* GetLightMap()
	{
		extern ILightMap* g_pLightMap;
		return g_pLightMap;
	}

	inline HWND	  Engine_GetHWnd(){ return GetDevice()->GetHWnd();}

	extern void WStringCPtoX(const WCHAR * pwszString,int nCP,bool bTrail,int *pX,int iFont);
	extern void WStringXtoCP(const WCHAR * pwszString,int nX, int * pCP,int *pnTrail,int iFont);
	extern int WStringPriorItemPos(const WCHAR* pwszString,int nCP);
	extern int WStringNextItemPos(const WCHAR* pwszString,int nCP);

	extern int WCharWidth(WCHAR ch,int iFont);
	extern void WStringNXtoCP(const WCHAR * pwszString,int count,int nX, int * pCP,int *pnTrail,int iFont);
#endif

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
inline double Engine_GetTime(){ return Engine_GetState()->GetTime();}
inline float  Engine_GetElapsedTime(){ return Engine_GetState()->GetElapsedTime(); }
inline float  Engine_GetDrawElapsedTime() { return Engine_GetState()->GetDrawElapsedTime();}

extern void EncryptData(BYTE* inData,size_t len,const BYTE* key1,const BYTE * key2,const BYTE* key3);
extern void DecryptData(BYTE* inData,size_t len,const BYTE* key1,const BYTE * key2,const BYTE* key3);
extern void PasswordToDesKey(const char* pszPassword,BYTE* key1,BYTE * key2,BYTE* key3);

extern iLog & Engine_GetFileLog();

#define appDump Engine_GetFileLog()


/**
* \brief 简短描述
* 
* 详细描述
* 
* \param szString : 描述
* \return 返回值的描述
*/
inline void Engine_WriteLog(const char* szString)
{
#if defined _DEBUG || defined RELEASE_TEST
	time_t ti = time(NULL);
	tm* t = localtime(&ti);
	char szTime[MAX_PATH];
	strftime(szTime,MAX_PATH,"[%y-%m-%d %H:%M:%S] ",t);

	appDump.writeString(szTime);
	appDump.writeString(szString);
	std::cout << szTime << szString;
#endif
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param szFmt : 描述
* \param argptr : 描述
* \return 返回值的描述
*/
inline void Engine_WriteLogV(const char* szFmt,va_list argptr )
{
	char szString[1024];
	_vsnprintf(szString,1024,szFmt,argptr);
	szString[1023] = 0;
	Engine_WriteLog(szString);
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param szFmt : 描述
* \return 返回值的描述
*/
inline void Engine_WriteLogF(const char* szFmt,...)
{
	va_list args;
	va_start( args, szFmt );
	Engine_WriteLogV(szFmt,args);
	va_end(args);
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param p : 描述
* \param charnum : 描述
* \return 返回值的描述
*/
inline bool IsNewLine(const char* p,size_t &charnum)
{
	if((*p) == '\r')
	{
		if(*(p + 1) == '\n')
			charnum = 2;
		else
			charnum = 1;
		return true;
	}
	if((*p) == '\n')
	{
		charnum = 1;
		return true;
	}
	return false;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param p : 描述
* \param charnum : 描述
* \return 返回值的描述
*/
inline bool IsNewLineW(const WCHAR* p,size_t &charnum)
{
	if((*p) == L'\r')
	{
		if(*(p + 1) == L'\n')
			charnum = 2;
		else
			charnum = 1;
		return true;
	}
	if((*p) == L'\n')
	{
		charnum = 1;
		return true;
	}
	return false;
}

extern bool Engine_Init();
extern void Engine_Uninit();

#ifndef _NO_GRAPHICS
	class CCursor;
	class CAppation;
	extern CCursor* Engine_GetCursor();

	/**
	* \brief 简短描述
	* 
	* 详细描述
	* 
	* \return 返回值的描述
	*/
	inline CAppation* Engine_GetAppation()
	{
		extern CAppation * g_pCurrentApp;
		return g_pCurrentApp;
	}

#endif

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param p : 描述
* \return 返回值的描述
*/
inline void Engine_SetCallBack(IEngineCallBack* p)
{
	extern IEngineCallBack* g_EngineCallBack;
	g_EngineCallBack = p;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \return 返回值的描述
*/
inline IEngineCallBack* Engine_GetCallBack()
{
	extern IEngineCallBack* g_EngineCallBack;
	return g_EngineCallBack;
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param vec : 描述
* \param index : 描述
* \return 返回值的描述
*/
template < class tVec>
inline void SimpleRemoveFromArray(tVec & vec,int index)
{
	vec[index] = vec[vec.size()-1];
	vec.resize(vec.size()-1);
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param vec : 描述
* \param v : 描述
* \return 返回值的描述
*/
template < class tVec ,class _Type>
inline void SortInsertToArray(tVec & vec,const _Type & v)
{
	int i = vec.size() - 1;
	vec.resize(vec.size() + 1);
	while( i >= 0 && vec[i] > v)
	{
		vec[i+1] = vec[i];
		--i;
	}
	vec[i+1] = v;
}

extern BOOL GotoURL( const char* url, int showcmd );

#include <hash_map>
template<class T1,class T2>
class CClassFunMap
{
private:
	typedef T2 (T1::* tFun)();
	typedef stdext::hash_map<std::string,tFun> tFunMap;
public:
	CClassFunMap()
	{
		m_pClass = NULL;
	}

	void Register(const char* funName,tFun tfun,T1* pT1 = NULL)
	{		
		_Register(m_funMap,funName,tfun,pT1);
	}
	//调用函数
	virtual T2 operator()(const char* funName)
	{
		tFun& f = m_funMap[funName];
		if( m_pClass && f ) return(m_pClass->*f)();
		return (T2)0;
	}
	size_t size() { return _size<tFunMap>();}
protected:
	//注册函数,在类的内部定义
	template<class T,class TMap>
	void _Register(TMap& map,const char* funName,T t,T1* pT1 = NULL)
	{
		T& f = map[funName];
		f = t;
		m_pClass = pT1;
	}
	template<class T>
	size_t _size()
	{
		return m_funMap.size();
	}
protected:
	tFunMap m_funMap;
	T1*     m_pClass;
};

#endif