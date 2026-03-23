/**
 * \file      engine.cpp
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     ********************************************************************
 * 
 *	      ********************************************************************
 */
 
#include "../include/engine.h"
#include "../include/engine_script.h"

#ifndef _NO_GRAPHICS
#include "../../gui/include/Cursor.h"
#include "../include/Appation.h"

#include "../../RenderD3D/include/RenderD3D.h"

#pragma comment( lib, "d3d8.lib" )
#pragma comment( lib, "D3dx8.lib" )
#pragma comment( lib, "Dxerr8.lib" )

#pragma comment( lib, "d3dxof.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "dsound.lib" )
#pragma comment( lib, "strmiids.lib" )

#pragma comment( lib, "ijl15l.lib" )
#pragma comment( lib, "user32.lib")
#pragma comment( lib, "Comdlg32.lib")
#pragma comment( lib, "Gdi32.lib")
#pragma comment( lib, "ole32.lib")
#pragma comment( lib, "Advapi32.lib")
#pragma comment( lib, "shell32.lib")

static int compareFloatByRandNum (const void *p1, const void *p2) 
{
	int v1 = ((stNumPercent*)p1)->percent;
	int v2 = ((stNumPercent*)p2)->percent;
	if(v2 > v1 ) return 1;
	if(v1 > v2 ) return -1;
	return (rand() % 2) ? 1 : -1;
}

/**
* \brief  按照出现机率（百分比）获取随机值
*
*	按照出现机率（百分比）获取随机值
*
* \param aNum : 随机值和百分比
* \param count : 个数
* \param ret : 返回的随机值
*
* \return  是否获取成功
*
*/
bool GetRandNumByPercent(stNumPercent aNum[],size_t count,int & ret)
{
	qsort(aNum,count,sizeof(stNumPercent),compareFloatByRandNum);
	int num = rand() % 100 + 1;
	for(size_t i=0;i<count;++i)
	{
		if(num <= aNum[i].percent)
		{
			ret = aNum[i].num;
			return true;
		}
		num -= aNum[i].percent;
	}
	return false;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param hr : 描述
 * \param file : 描述
 * \param line : 描述
 * \return 返回值的描述
 */
void TraceDXError(HRESULT hr,const char* file,int line)
{
	const char* p = DXGetErrorDescription8(hr);
	TRACE("%s(%u): DXError:%s\n",file,line,p);

#if defined _DEBUG || defined RELEASE_TEST
	Engine_WriteLogF("%s(%u): DXError:%s\n",file,line,p);
#endif

}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pwszString : 描述
 * \param nCP : 描述
 * \param bTrail : 描述
 * \param pX : 描述
 * \param iFont : 描述
 * \return 返回值的描述
 */
void WStringCPtoX(const WCHAR * pwszString,int nCP,bool bTrail,int *pX,int iFont)
{
	const WCHAR* p = pwszString;
	const WCHAR* end = p + nCP;
	if(bTrail) end ++;
	*pX = 0;
	int iWcharWidth = GetDevice()->GetFontWcharWidth(iFont);
	int iCharWidth = GetDevice()->GetFontWidth(iFont);
	while(p < end && (*p))
	{
		int w;
		if((*p) & 0xff00)
		{
			WORD ch = my_wcstombs(*p);
			if(ch >= c_nPicCharBase)
				w = GetDevice()->GetAFontWidth(ch);
			else w = iWcharWidth;
		}
		else
		{
			if((*p) == '\t')
				w = c_nTabSize * iCharWidth;
			else
				w = iCharWidth;
		}
		*pX += w;
		p++;
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pwszString : 描述
 * \param nX : 描述
 * \param pCP : 描述
 * \param pnTrail : 描述
 * \param iFont : 描述
 * \return 返回值的描述
 */
void WStringXtoCP(const WCHAR * pwszString,int nX, int * pCP,int *pnTrail,int iFont)
{
	WStringNXtoCP(pwszString,wcslen(pwszString),nX,pCP,pnTrail,iFont);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pwszString : 描述
 * \param count : 描述
 * \param nX : 描述
 * \param pCP : 描述
 * \param pnTrail : 描述
 * \param iFont : 描述
 * \return 返回值的描述
 */
void WStringNXtoCP(const WCHAR * pwszString,int count,int nX, int * pCP,int *pnTrail,int iFont)
{
	const WCHAR* p = pwszString;
	if(nX < 0) nX = 0;
	*pnTrail = false;
	const WCHAR * end = p + count;
	int iWcharWidth = GetDevice()->GetFontWcharWidth(iFont);
	int iCharWidth = GetDevice()->GetFontWidth(iFont);
	while(p < end)
	{
		int w;
		if((*p) & 0xff00)
		{
			WORD ch = my_wcstombs(*p);
			if(ch >= c_nPicCharBase)
				w = GetDevice()->GetAFontWidth(ch);
			else w = iWcharWidth;
		}
		else
		{
			if((*p) == '\t')
				w = c_nTabSize * iCharWidth;
			else
				w = iCharWidth;
		}
		if(nX < w)
		{
			*pnTrail = ( nX * 2 > w);
			break;
		}
		nX -= w;
		p++;
	}
	*pCP = p - pwszString;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pwszString : 描述
 * \param nCP : 描述
 * \return 返回值的描述
 */
int WStringPriorItemPos(const WCHAR* pwszString,int nCP)
{
	// 过滤空格
	int j,i;
	for(j= nCP-1; j>=0;--j)
	{
		if(!iswspace(pwszString[j])) break;
	}

	for(i= j; i>= 0;--i)
	{
		if(iswspace(pwszString[i]))
		{
			return i+1;
		}
		else
		{
			if(!iswascii(pwszString[i]))
			{
				return i;
			}
		}
	}
	return 0;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pwszString : 描述
 * \param nCP : 描述
 * \return 返回值的描述
 */
int WStringNextItemPos(const WCHAR* pwszString,int nCP)
{
	int j,i;
	if(pwszString[nCP] == 0) return nCP;
	for(i= nCP+1; pwszString[i];++i)
	{
		if(iswspace(pwszString[i]))
		{
			break;
		}
		else
		{
			if(!iswascii(pwszString[i]))
			{
				return i;
			}
		}
	}

	// 过滤空格
	for(j = i; pwszString[j]; ++j)
	{
		if(!iswspace(pwszString[j])) break;
	}

	return j;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param ch : 描述
 * \param iFont : 描述
 * \return 返回值的描述
 */
int WCharWidth(WCHAR ch,int iFont)
{
	int ichar = my_wcstombs(ch);
	//WideCharToMultiByte(CP_ACP,0,&ch,1,(LPSTR)&ichar,4,NULL,NULL);

	return GetDevice()->GetAFontWidth(ichar,iFont);
}



#endif

#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "ws2_32.lib" )

const int c_nTabSize = 4;


void MyOutputDebugString(const char* pszFmt,...)
{
	va_list ap;
	va_start( ap, pszFmt );

	std::vector<char> str;
	str.resize(64);
	while( _vsnprintf(&str[0],str.size(),pszFmt,ap) < 0 )
	{
		str.resize(str.size() * 2);
		if(str.size() > 8192) 
		{
			str.back() = 0;
			break;
		}
	}
	str[str.size()-1] = 0;
	va_end  ( ap );
	OutputDebugString(&str[0]);

}

IEngineCallBack* g_EngineCallBack = NULL;


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pszFmt : 描述
 * \return 返回值的描述
 */
const char* avar(const char* pszFmt,...)
{
	__declspec( thread ) static char str[1024];

	va_list ap;
	va_start( ap, pszFmt );

	_vsnprintf(&str[0],count_of(str),pszFmt,ap);
	str[count_of(str)-1] = 0;

	va_end  ( ap );
	return &str[0];
}

#ifndef _NO_GRAPHICS
static bool g_bUseD3DRender = true;

IDevice* g_pDevice = NULL;

ILightMap* g_pLightMap = NULL;

IGraphicCursor* g_pGCursor = NULL;
#endif

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
bool Engine_Init()
{
	if(!InitFilePackHelper())
		return false;

#ifndef _NO_GRAPHICS
	FrameAllocator::init(3 * 1024 * 1024);
	extern void InitRenderD3D();
	InitRenderD3D();

	g_pDevice = (IDevice*)GetD3DDevice();
	g_pLightMap = (ILightMap*)GetD3DLightMap();
	g_pGCursor = (IGraphicCursor*)GetD3DCursor();
	//GetDevice = (IDevice * (*)())GetD3DDevice;
	//GetLightMap = (ILightMap * (*)())GetD3DLightMap;
	g_pDevice->InitResourceManager();
	Engine_ScriptInit();
#endif
	return true;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void Engine_Uninit()
{
	UninitFilePackHelper();

#ifndef _NO_GRAPHICS
	FrameAllocator::destroy();
#endif

}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
iLog & Engine_GetFileLog()
{
	static CFileLog log;
	return log;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param url : 描述
 * \param showcmd : 描述
 * \return 返回值的描述
 */
#include <shellapi.h>
BOOL GotoURL( const char* url, int showcmd )
{
	static bool sHaveKey = false;
	static char sWebKey[512];

	if ( !sHaveKey )
	{
		HKEY regKey;
		DWORD size = sizeof( sWebKey );

		if ( RegOpenKeyEx( HKEY_CLASSES_ROOT, "\\http\\shell\\open\\command", 0, KEY_QUERY_VALUE, &regKey ) != ERROR_SUCCESS )
		{
			TRACE( "Failed to open the HKCR\\http registry key!!!\n");
			return( false );
		}

		size = sizeof( sWebKey );
		if ( RegQueryValueEx( regKey, "", NULL, NULL, (unsigned char*)sWebKey, &size ) != ERROR_SUCCESS ) 
		{
			TRACE( "Failed to query the open command registry key!!!\n" );
			return( false );
		}

		RegCloseKey( regKey );
		sHaveKey = true;

		char *p = strstr(sWebKey, "\"%1\"");
		if (p) *p = 0;
	}

	STARTUPINFO si;
	memset( &si, 0, sizeof( si ) );
	si.cb = sizeof( si );

	char buf[1024];
	_snprintf( buf, sizeof( buf ), "%s %s", (const char*) sWebKey, url );
	buf[1023] = 0;

	//Con::errorf( ConsoleLogEntry::General, "** Web browser command = %s **", buf );

	PROCESS_INFORMATION pi;
	memset( &pi, 0, sizeof( pi ) );
	if(!CreateProcess( NULL,
		buf, 
		NULL,
		NULL, 
		false, 
		CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP, 
		NULL, 
		NULL, 
		&si, 
		&pi ))
		return false;

	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	return true;
}

bool Copy2Clipboard(const char* pstr)
{
	if( OpenClipboard( NULL ) )
	{
		EmptyClipboard();
		HGLOBAL hBlock = GlobalAlloc( GMEM_MOVEABLE, sizeof(char) * (strlen(pstr)+1) );
		if( hBlock )
		{
			char *pszText  = (char*)GlobalLock( hBlock );
			if( pszText )
			{
				strcpy(pszText,pstr);
				GlobalUnlock( hBlock );
			}
			SetClipboardData( CF_TEXT, hBlock );
		}
		CloseClipboard();
		if( hBlock )
		{
			GlobalFree( hBlock );
		}
		return true;
	}
	return false;
}