/**
 * \file      IDevice.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     图形设备接口
 * 
 *	      图形设备接口
 */

#pragma once

#include <vector>

struct stDrawPoint
{
	POINT pt;
	DWORD color;
};

enum FlushType
{
	Flush_Null,			//
	Flush_Cover,		// 相互遮挡
	Flush_NoCover,		// 相互没有遮挡
};

enum enumProgramType
{
	eProgram_Game,
	eProgram_MapEditor,
	eProgram_MagicEditor,
	eProgram_ViewGL,
};

enum enumFontEffect{
	FontEffect_None,
	FontEffect_Shadow,
	FontEffect_Border,
};

struct stResourceLocation
{
	char szFileName[MAX_PATH];
	int			group;
	int			frame;
	
	stResourceLocation()
	{
		szFileName[0] = 0;
		group = 0;
		frame = 0;
	}
	
	stResourceLocation(const char* pszPack,int i_group,int i_frame)
	{
		strncpy(szFileName,pszPack,sizeof(szFileName));
		szFileName[sizeof(szFileName)-1] = 0;
		group = i_group;
		frame = i_frame;
	}
	
	stResourceLocation(const char* pszPath)
	{
		SetImagePath(pszPath);
	}

	void SetFileName(const char* pszFileName)
	{
		strncpy(szFileName,pszFileName,sizeof(szFileName));
		szFileName[sizeof(szFileName)-1] = 0;
	}

	void SetImagePath(const char* path)
	{
		group = -1;
		frame = -1;
		szFileName[0] = 0;

		char szTmp[MAX_PATH];
		strcpy(szTmp,path);
		char* p = strrchr( szTmp,'\\' );
		frame = atol(p+1);
		if(szTmp < p) 
		{
			*p = 0;
			p = strrchr( szTmp,'\\' );
			group = atol( p+1 );
			if( szTmp < p ) 
			{
				*p = 0;
				strcpy(szFileName,szTmp);
				strcat(szFileName,".gl");
			}
		}
	}

	bool IsEmpty()
	{
		return szFileName[0] == 0 || group < 0 || frame < 0;
	}
};

enum enumImageScaleMode{
	ImageScaleMode_None,
	ImageScaleMode_Half,
};

#include "IBitmapList.h"

struct stPicCharInfo{
	IBitmaps* pBmps;
	SIZE size;
	stPointF scale;
};

extern const DWORD c_dwClrDefaultShadow;
extern const int c_nPicCharBase;// 不是UNICODE
//class CCustomTexture;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class IDevice
{
protected:
	bool	m_bActive;
	bool	m_bCreated;
	bool	m_bFullScreen;
	bool	m_bRenderScene;
	bool	m_bMonochrome;
	HWND	m_hWnd;
	DWORD	m_dwFlags;
	int		m_iWidth;
	int		m_iHeight;
	int		m_iWndX;
	int		m_iWndY;
	int		m_iWndWidth;
	int		m_iWndHeight;
	int		m_iPixelBits;
	stRectI	m_rcRender;
	DEVMODE	m_dm;
	std::vector<DEVMODE> m_aDisplayMode;
	FlushType m_FlushType;
	enumProgramType m_eProgramType;
	enumImageScaleMode m_eImageScaleMode;

protected:
	bool GetDisplayMode(DEVMODE *pRet,DWORD nWidth,DWORD nHeight,DWORD dwPixelBits);
	void CheckDisplayMode();
	bool ResetWindowPos();
	bool BeginFullScreen(DEVMODE* pMode);
	bool EndFullScreen();
public:
	bool IsRenderScene(){ return  m_bRenderScene;}
	void SetRenderScene(bool b) { m_bRenderScene = b;}
	int GetPixelBits(){ return m_iPixelBits;}
	DEVMODE* GetDisplayModeList(){ return &m_aDisplayMode[0];}
	size_t GetDisplayModeCount(){ return m_aDisplayMode.size();}
	int GetWindowSizeList(SIZE* pBuf,size_t count);
	int GetPixelBitsList(DWORD *pdwBits,size_t count);
	void UpdateWndSize();
	int GetWndX(){ return m_iWndX;}
	int GetWndY(){ return m_iWndY;}
	int GetWndWidth() { return m_iWndWidth;}
	int GetWndHeight() { return m_iWndHeight;}

	virtual void GetPicCharInfo( stPicCharInfo** ppPicCharInfo,size_t * count) = 0;
	virtual void SetPicCharInfo( stPicCharInfo* p,size_t nPicCharInfoCount)  = 0;
	virtual stPicCharInfo* GetPicCharInfo(int ichar) = 0;
	enumImageScaleMode GetImageScaleMode(){ return m_eImageScaleMode;}
	void SetImageScaleMode(enumImageScaleMode e) { m_eImageScaleMode = e;}

	bool IsActive(){ return m_bActive;}
	bool IsCreated(){ return m_bCreated; }
	enumProgramType GetProgramType(){ return m_eProgramType;}
	//void SetProgramType(enumProgramType type) { m_eProgramType = type; }

	virtual void InitResourceManager() = 0;
	virtual void BeginFlush(FlushType type) = 0;
	virtual void EndFlush() = 0;

	virtual bool MsgProc( UINT Message, WPARAM wParam, LPARAM lParam );
	virtual void Run() = 0;
	const RECT * GetRenderRect(){ return &m_rcRender;}
	virtual void DeleteResource(const char* filename) = 0;
public:
	enum{
		//DEVICE_32BPP = BIT(1),
		DEVICE_FULLSCREEN = BIT(2),
		DEVICE_VSYN	= BIT(3),
		DEVICE_LIGHTMAP = BIT(4),
		DEVICE_MULTITEX = BIT(5),
		DEVICE_RENDERTARGET = BIT(6),
		DEVICE_DXT	= BIT(7),
	};

	void ConverMousePoint(POINT & pt);
	int GetWidth(){ return m_iWidth;}
	int GetHeight(){ return m_iHeight;}

	bool GetNextLevelPixelBits(int width,int height,DWORD dwPixelBits,DWORD & dwResult);

	bool GetNextLevelWindowSize(int width,int height,SIZE& szResult);

	IDevice();
	virtual ~IDevice(void){};
	virtual int GetTriCount() = 0;
	virtual bool IsSupportFlag(DWORD flags) = 0;
	virtual bool Create(HWND hWnd,DWORD dwWidth,DWORD dwHeight,DWORD dwPixelBits,DWORD dwFlags,enumProgramType eProgramType = eProgram_Game) = 0;
	virtual bool Reset(DWORD dwWidth,DWORD dwHeight,DWORD dwPixelBits,DWORD dwFlags) = 0;
	virtual void Destroy() = 0;
	virtual bool TestCooperativeLevel() = 0;
	virtual bool BeginScene() = 0;
	virtual bool EndScene() = 0;
	virtual bool Present() = 0;
	virtual bool Present(const RECT * prc) = 0;
	virtual void Clear(DWORD dwColor) = 0;

	virtual void EnableLightMap(bool bEnable) = 0;
	bool IsEnableLightMap() { return (m_dwFlags & DEVICE_LIGHTMAP)!=0;}

	// Find and create
	virtual IBitmapList* FindResource(const char* filename,DWORD dwFlags,TextureUsageType usage) = 0;

	// Find
	virtual IBitmapList* FindResource(const char* filename) = 0;

	IBitmaps* FindBitmaps(const stResourceLocation* p)
	{
		IBitmapList* pBmpList = FindResource(p->szFileName);
		if(!pBmpList) return NULL;
		return pBmpList->GetBitmaps(p->group,p->frame);
	}
	
	IBitmap* FindBitmap(const stResourceLocation* p)
	{
		IBitmaps * pBmps = FindBitmaps(p);
		if(!pBmps)
		{
			return NULL;
		}
		return pBmps->GetBitmap(0);
	}

	void BeginMonochrome(){
		m_bMonochrome = true;
	}
	void EndMonochrome(){
		m_bMonochrome = false;
	}

	virtual bool ReleaseAllVideoMemory() = 0;

	virtual bool FillRect(const RECT * lprc,DWORD colorLeftTop,DWORD colorRightTop,DWORD colorLeftBottom,DWORD colorRightBottom) = 0;
	virtual bool FillRect(const RECT * lprc,DWORD color) = 0;
	virtual bool DrawRectangle(const RECT * lprc,DWORD color) = 0;
	virtual bool DrawPolyLine(const POINT* points,size_t pointCount,DWORD color) = 0;
	virtual bool DrawLine(const POINT * pt1,const POINT *pt2,DWORD color) = 0;
	virtual bool DrawLine( int x1,int y1,int x2,int y2,DWORD color) = 0;			//Add By David For 2006-3-9

	virtual int SetFont(int iFont) = 0;

	virtual SIZE GetStringSize(const char* lpszString, size_t count,size_t maxWidth,bool bSingleLine = false,enumFontEffect fontEffect = FontEffect_None) = 0;
	virtual void GetStringRect(const char* lpszString, size_t count,RECT & layoutRect,DWORD dwTextFormat = DT_TOP | DT_LEFT ) = 0;
	virtual void GetStringRect2(const char* lpszString, size_t count,RECT & layoutRect,RECT & totalRect,DWORD dwTextFormat = DT_TOP | DT_LEFT ) = 0;

	virtual void DrawString(const char* lpszString , size_t count,const RECT & layoutRect ,DWORD clrText = 0xffffffff,DWORD dwTextFormat = DT_TOP | DT_LEFT,enumFontEffect fontEffect = FontEffect_None,DWORD clrShadow = c_dwClrDefaultShadow,DWORD* pData = NULL) = 0;
	virtual void DrawString(const WCHAR* lpszString , size_t count,const RECT & layoutRect ,DWORD clrText = 0xffffffff,DWORD dwTextFormat = DT_TOP | DT_LEFT,enumFontEffect fontEffect = FontEffect_None,DWORD clrShadow = c_dwClrDefaultShadow,DWORD* pData = NULL) = 0;

	// 设置字体水平间距
	virtual int	SetFontHSpace(int n) = 0;
	// 设置字体垂直间距
	virtual int	SetFontVSpace(int n) = 0;
	// 设置字体TAB字符宽度
	virtual int	SetFontTabSize(int n) = 0;

	virtual size_t GetTotalTextureMemSize() = 0;
	virtual size_t GetUseTextureMemSize() = 0;
	
	virtual size_t GetUseResMemorySize() = 0;

	void DrawString(const char* lpszString , size_t count,POINT  pt,DWORD clrText = 0xffffffff,enumFontEffect fontEffect = FontEffect_None,DWORD clrShadow = c_dwClrDefaultShadow,DWORD* pData = NULL)
	{
		RECT layoutRect = {pt.x,pt.y,0xffff,0xffff};
		DrawString(lpszString,count,layoutRect,clrText,DT_TOP | DT_LEFT,fontEffect,clrShadow,pData);
	}

	void DrawString(const char* lpszString , const RECT & layoutRect ,DWORD clrText = 0xffffffff,DWORD dwTextFormat = DT_TOP | DT_LEFT,enumFontEffect fontEffect = FontEffect_None,DWORD clrShadow = c_dwClrDefaultShadow,DWORD* pData = NULL)
	{
		DrawString(lpszString,-1,layoutRect,clrText,dwTextFormat,fontEffect,clrShadow,pData);
	}
	
	void DrawString(const char* lpszString , POINT pt,DWORD clrText = 0xffffffff,enumFontEffect fontEffect = FontEffect_None,DWORD clrShadow = c_dwClrDefaultShadow,DWORD* pData = NULL)
	{
		DrawString(lpszString,-1,pt,clrText,fontEffect,clrShadow,pData);
	}

	void DrawString(const WCHAR* lpszString , size_t count,POINT  pt,DWORD clrText = 0xffffffff,enumFontEffect fontEffect = FontEffect_None,DWORD clrShadow = c_dwClrDefaultShadow,DWORD* pData = NULL)
	{
		RECT layoutRect = {pt.x,pt.y,0xffff,0xffff};
		DrawString(lpszString,count,layoutRect,clrText,DT_TOP | DT_LEFT,fontEffect,clrShadow,pData);
	}

	void DrawString(const WCHAR* lpszString , const RECT & layoutRect ,DWORD clrText = 0xffffffff,DWORD dwTextFormat = DT_TOP | DT_LEFT,enumFontEffect fontEffect = FontEffect_None,DWORD clrShadow = 0xa0101010,DWORD* pData = NULL)
	{
		DrawString(lpszString,-1,layoutRect,clrText,dwTextFormat,fontEffect,clrShadow,pData);
	}

	void DrawString(const WCHAR* lpszString , POINT pt,DWORD clrText = 0xffffffff,enumFontEffect fontEffect = FontEffect_None,DWORD clrShadow = c_dwClrDefaultShadow,DWORD* pData = NULL)
	{
		DrawString(lpszString,-1,pt,clrText,fontEffect,clrShadow,pData);
	}

	RECT SetClipRect(const RECT* prc);

	HWND GetHWnd(){ return m_hWnd; }

	virtual int GetFontHeight(int iFont = -1)  =  0;
	virtual int GetFontWidth(int iFont = -1)  =  0;
	virtual int GetFontWcharWidth(int iFont = -1) = 0;
	virtual int GetAFontWidth(int ch,int iFont = -1) = 0;

	DWORD GetFlags(){ return m_dwFlags;}
	//virtual bool RenderBlendTexture(CCustomTexture* pTexture,CCustomTexture* pAlpha,const RECT* pRect, const RECT* pRectTexture1,const RECT* pRectTexture2,DWORD color) = 0;
	virtual bool RenderBlendBitmap(IBitmap* pBmp,IBitmap* pAlpha,int x,int y,DWORD color = 0xffffffff,BlendType blend = Blend_Null) = 0;
	virtual bool RenderBlendBitmap(IBitmap* pBmp,IBitmap* pAlpha,int x,int y,const RECT* rect = NULL,const stPointF * scale = NULL,DWORD color = 0xffffffff,BlendType blend = Blend_Null) = 0;

	virtual void SetFontScale(float cx,float cy) = 0;
	virtual stPointF GetFontScale() = 0;
	virtual bool Present(const RECT * prcDst,const RECT * prcSrc) = 0;

	// 使后面渲染的图绕一个点旋转，角度是顺时针方向的
	virtual void SetRotation(POINT pt,FLOAT fAngle) = 0;
	// 规格化变换矩阵(不旋转)
	virtual void IdentityTransform() = 0;

	virtual bool IsLoadQueueLowerEmpty() = 0;
	virtual void LockResLoad() = 0;
	virtual void UnlockResLoad() = 0;

	virtual HRESULT SaveBackSurfaceToFile(LPCTSTR  pDestFile,D3DXIMAGE_FILEFORMAT DestFormat,CONST RECT* pSrcRect) = 0;
	// 方向逆时针
	virtual bool DrawRectPie( const RECT* prect,float fStartAngle,float fEndAngle,DWORD color ) = 0;
	virtual bool DrawTriangle(POINT pt1,POINT pt2,POINT pt3,DWORD color1,DWORD color2,DWORD color3) = 0;
	virtual int IsValidFont(int iFont) = 0;

	virtual void UnmapAllBitmapData() = 0;

	virtual bool IsSupportHardwareCursor() = 0;
};



