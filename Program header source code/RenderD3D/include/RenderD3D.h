#ifndef _RENDERD3D_H
#define _RENDERD3D_H

class CD3DDevice;
class CTextureManager;
class CD3DLightMap;
class CD3DTextout;
class CD3DMBitmapManager;
class CD3DCursor;


inline CD3DDevice* GetD3DDevice()
{ 
	extern CD3DDevice* g_pD3DDevice;
	return g_pD3DDevice;
}

inline CTextureManager* GetTextureManager()
{
	extern CTextureManager* g_TextureManager;
	return g_TextureManager;
}

inline CD3DLightMap* GetD3DLightMap()
{
	extern CD3DLightMap* g_D3DLightMap;
	return g_D3DLightMap;
}

inline CD3DTextout* GetD3DTextout()
{
	extern CD3DTextout* g_pD3DTextout;
	return g_pD3DTextout;
}

inline CD3DMBitmapManager* GetD3DMBitmapManager()
{
	extern CD3DMBitmapManager * g_pD3DMBitmapManager;
	return g_pD3DMBitmapManager;
}

inline CD3DCursor* GetD3DCursor()
{
	extern CD3DCursor * g_pD3DCursor;
	return g_pD3DCursor;
}

#endif