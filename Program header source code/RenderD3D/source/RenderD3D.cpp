#include "../../engine/include/engine.h"
#include "../Include/RenderD3D.h"
#include "../Include/D3DDevice.h"
#include "../Include/TextureManager.h"
#include "../Include/D3DLightMap.h"
#include "../Include/D3DTextout.h"
#include "../Include/D3DMBitmap.h"
#include "../include/D3DCursor.h"

CD3DCursor s_D3DCursor;
CD3DCursor * g_pD3DCursor = NULL;
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CD3DDevice s_D3DDevice;
CD3DDevice * g_pD3DDevice = NULL;
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureManager s_TextureManager;
CTextureManager * g_TextureManager = NULL;
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CD3DLightMap s_D3DLightMap;
CD3DLightMap* g_D3DLightMap = NULL;
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CD3DTextout s_D3DTextout;
CD3DTextout * g_pD3DTextout = NULL;
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CD3DMBitmapManager s_D3DMBitmapManager;
CD3DMBitmapManager * g_pD3DMBitmapManager = NULL;

void InitRenderD3D()
{
	g_pD3DDevice = &s_D3DDevice;
	g_TextureManager = &s_TextureManager;
	g_D3DLightMap = &s_D3DLightMap;
	g_pD3DTextout = &s_D3DTextout;
	g_pD3DMBitmapManager = &s_D3DMBitmapManager;
}

//BOOL WINAPI DllMain(
//					HINSTANCE hinstDLL,
//					DWORD fdwReason,
//					LPVOID lpvReserved
//					)
//{
//	return TRUE;
//}