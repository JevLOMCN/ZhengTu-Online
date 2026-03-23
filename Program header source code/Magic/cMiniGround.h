#ifdef MAGIC_EDITOR

#pragma once

#include "..\cache\idevice.h"
#include "..\cache\icache.h"
#include "iGround.h"

#include <list>
#include <D3dx8tex.h>
#include "ijl.h"

//#define USE_JPGTEXTURE

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class cMiniGround : public iMiniGround
{
public:
	cMiniGround(void);
	virtual ~cMiniGround(void);
public:
	//draw
	std::vector<stDrawPoint> m_aPoint[PointSize_Num];
	void AddObject(PointSizeEnum size,const stDrawPoint& st) ;
	void ClearObject() ;
	void Draw();

	bool m_bHide;
	void SetHide(bool bHide){m_bHide = bHide;};
	bool IsHide(){return m_bHide;}
	void ToggleHide() {m_bHide = !m_bHide;};

	POINT m_ptScreen;
	SIZE m_size;
	bool MoveTo(POINT ptScreen);
	bool SetSize(SIZE sz);
protected:
	void RenderTerrain() ;
	void RenderObj() ;
	void OnGroundMoveTo(POINT ptPos);

	POINT m_ptGroundCenter;			//地图的中心
	//call by ground..
	//左上角的坐标
	bool CreateMiniGround() ;
	void FinalRelease();
	bool Load(LPCSTR szMap,int width,int height) ;
	HRESULT InvalidateDeviceObjects();	//before call reset alt+tab 
	HRESULT RestoreDeviceObjects();		//after alt+tab

	bool WorldToOffset(POINT pt,POINT& ptOffset);
	bool WorldToScreen(POINT pt,POINT& ptScreen);

#ifdef USE_JPGTEXTURE
	Texture9* m_pJpgTexture;
	D3DXIMAGE_INFO m_info;
#else
	JPEG_CORE_PROPERTIES m_image;
#endif

	SIZE m_szRender;
	Texture9* m_pRenderTexture;		
	POINT m_ptRenderCenter;			//当前的RenderTexture的中心对应Ground的点
	bool IsValid();

	RECT m_rcWorldClip;
	RECT m_rcScreen;
	RECT m_rcTexture;
	bool Validate();
	bool Build();
	bool Update();

	D3DFORMAT m_format;
	int m_nPixelBytes;
	//use two texture..
	//one texture is mem texture..
	//one is managed texture, it is need not invalidate...
	//IBitmap* m_pTexture;
};

static bool ReadJpgTo565(const BYTE* pJPG,int nJpgLen,JPEG_CORE_PROPERTIES& image);

#endif