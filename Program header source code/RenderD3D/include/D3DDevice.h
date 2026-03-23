/**
 * \file      D3DDevice.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2005-07-05 12:29:19
 * \brief     D3D图形设备
 * 
 *	      D3D图形设备
 */
 
#pragma once
#include "../../Engine/include/IDevice.h"
#include "../../Engine/include/lookaside_alloc.h"
//#include "./D3DBitmap.h"

#include <vector>
#include <list>
#include <string>
#include <hash_map>

extern const DWORD c_maxRectangleNum;
extern const DWORD c_maxRectangleNum; 
extern const DWORD c_maxVertexNum ;
extern const DWORD c_maxIndexNum ;

#define USE_DYNAMIC_BUFFER
#define USE_INDEX_BUFFER

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param i_pIndex : 描述
 * \param dwIndexSize : 描述
 * \param nStartVertex : 描述
 * \param nRenderNum : 描述
 * \return 返回值的描述
 */
inline void FillIndexBuffer(void* i_pIndex,DWORD dwIndexSize,DWORD nStartVertex,DWORD nRenderNum)
{
	if(dwIndexSize == 4)
	{
		DWORD* pIndex = (DWORD*)i_pIndex;
		DWORD* pEnd = pIndex + nRenderNum * 6;
		for(;pIndex < pEnd ; pIndex += 6 , nStartVertex += 4)
		{
			pIndex[0] = nStartVertex + 0;
			pIndex[1] = nStartVertex + 1;
			pIndex[2] = nStartVertex + 2;

			pIndex[3] = nStartVertex + 2;
			pIndex[4] = nStartVertex + 1;
			pIndex[5] = nStartVertex + 3;
		}
	}
	else
	{
		WORD* pIndex = (WORD*)i_pIndex;
		WORD* pEnd = pIndex + nRenderNum * 6;
		for(;pIndex < pEnd ; pIndex += 6 , nStartVertex += 4)
		{
			pIndex[0] = (WORD)(nStartVertex + 0);
			pIndex[1] = (WORD)(nStartVertex + 1);
			pIndex[2] = (WORD)(nStartVertex + 2);
			pIndex[3] = (WORD)(nStartVertex + 2);
			pIndex[4] = (WORD)(nStartVertex + 1);
			pIndex[5] = (WORD)(nStartVertex + 3);
		}
	}
}


struct stVertexBase
{
	D3DXVECTOR4 v;
	DWORD color;
};

#define D3DFVF_VERTEX_BASE (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

struct stVertexBitmap : public stVertexBase
{
	D3DXVECTOR2 uv;   // The texture coordinates.
};

extern const DWORD D3DFVF_BITMAP;
struct stMultiTexVertexBitmap : public stVertexBitmap
{
	D3DXVECTOR2 uv2;   // The texture coordinates.
};


struct stRenderInfoBase{
	stRectF				rect;
	stRectF				uv;
	DWORD				color;
};

struct stRenderInfo : public stRenderInfoBase{
	BlendType			blend;
	IDirect3DTexture8 * tex;
};

struct stMultiTexRenderInfoBase{
	stRectF				rect;
	stRectF				uv;
	stRectF				uv2;
	DWORD				color;
};

struct stMultiTexRenderInfo : public stMultiTexRenderInfoBase{
	BlendType			blend;
	IDirect3DTexture8 * tex;
	IDirect3DTexture8 * tex2;
	ULONGLONG GetKey()
	{
		DWORD key[2] = { (DWORD)tex,(DWORD)tex2 };
		return *(ULONGLONG*)key;
	}
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param key : 描述
 * \param * : 描述
 * \param * : 描述
 * \return 返回值的描述
 */
inline void GetRenderKeyTex(ULONGLONG key,IDirect3DTexture8 * & pTex,IDirect3DTexture8 * & pTex2)
{
	DWORD * pKey = (DWORD*)&key;
	pTex = (IDirect3DTexture8*)pKey[0];
	pTex2 = (IDirect3DTexture8*)pKey[1];
}

struct stDevRenderInfo {
#ifdef USE_INDEX_BUFFER
	stVertexBitmap vec[4];
#else
	stVertexBitmap vec[6];
#endif
};

struct stDevMultiTexRenderInfo{
#ifdef USE_INDEX_BUFFER
	stMultiTexVertexBitmap vec[4];
#else
	stMultiTexVertexBitmap vec[6];
#endif
};

class CD3DBitmap;
class CD3DBitmapList;
class CTextureManager;
class CCustomTexture;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CD3DDevice :
	public IDevice
{
	friend CTextureManager;
	DWORD m_dwBehaviorFlags;
public:

	enum enumDrawState{
		DrawStateNoTex,
		DrawStateTex,
		DrawStateMultiTex,
	};

	enum{
		eTmpTex4444 = 0,
		eTmpTex565,
		eTmpTex5551,
		eTmpTexDXT1,
		eTmpTexDXT3,
		eTmpTexA8R8G8B8,
		eTmpTexX8R8G8B8,
		eTmpTexA8,
		eTmpTexNum,
	};

	typedef std::vector<stDevRenderInfo> tArrayRenderInfo;
	typedef tArrayRenderInfo::iterator tArrayRenderInfoIt;

	typedef std::vector<stDevMultiTexRenderInfo> tArrayMultiTexRenderInfo;
	typedef tArrayMultiTexRenderInfo::iterator tArrayMultiTexRenderInfoIt;

	struct stRenderInfos
	{
		tArrayRenderInfo array;
		BlendType		 blend;
	};
	
	struct stRenderInfosEx : public stRenderInfos
	{
		IDirect3DTexture8* pTex;
	};

	struct stMultiTexRenderInfos
	{
		tArrayMultiTexRenderInfo array;
		BlendType				 blend;
	};

	struct stMultiTexRenderInfosEx : stMultiTexRenderInfos
	{
		IDirect3DTexture8* pTex2;
	};

	typedef stdext::hash_map< DWORD , stRenderInfos * > tMapRenderInfo;
	typedef tMapRenderInfo::iterator tMapRenderInfoIt;
	typedef std::pair <tMapRenderInfo::iterator, bool> tMapRenderItPair;

	typedef stdext::hash_map<ULONGLONG , stMultiTexRenderInfos * > tMapMultiTexRenderInfo;
	typedef tMapMultiTexRenderInfo::iterator tMapMultiTexRenderInfoIt;
	typedef std::pair <tMapMultiTexRenderInfo::iterator, bool> tMapMultiTexRenderItPair;

	std::lookaside_allocator< stRenderInfos > m_allocRenderInfos;
	std::lookaside_allocator< stMultiTexRenderInfos > m_allocMultiTexRenderInfos;
private:
	tMapRenderInfo	m_mapRenderInfo;
	stRenderInfosEx m_arrayRenderInfo;

	tMapMultiTexRenderInfo	m_mapMultiTexRenderInfo;

	D3DXMATRIXA16	m_Transform;

	bool	m_bUsableA8;
	bool	m_bUsableDXT1;
	bool	m_bUsableDXT3;
	bool	m_bUsable5551;
	bool	m_bTexMinLinear;
	bool	m_bTexMagLinear;
	bool	m_bTransform;
	bool	m_bSystemmemBuffer;

	DWORD m_dwVertexLockFlags[2];


	
	enumDrawState mDrawState;
	BlendType	  mBlendType;

	IDirect3D8 * m_pD3D;
	IDirect3DDevice8 * m_pD3DD;
	D3DDEVTYPE	m_dwDevType;
	D3DPRESENT_PARAMETERS m_d3dpp;

	struct stTempTexture{
		IDirect3DTexture8** pTex;
		int				   iRun;
		D3DFORMAT		   format;
		size_t			   num;
		stTempTexture():pTex(NULL){}
		~stTempTexture()
		{
			delete [] pTex;
		}

		void init(D3DFORMAT i_format,size_t i_num);
		IDirect3DTexture8* getNewTex();
		void release();
	};

	stTempTexture m_tmptextures[eTmpTexNum];
	IDirect3DSurface8* m_tmpsurfaces[eTmpTexNum];

	IDirect3DVertexBuffer8*  m_pVB;
	DWORD					 m_nVBOffset;		// 顶点缓冲区中的领偏移值
	//IDirect3DVertexBuffer8*  m_pMultiTexVB;
	//DWORD					 m_nMultiTexVBOffset;
	IDirect3DIndexBuffer8*	 m_pIB;
	DWORD					 m_nIBOffset;
	DWORD					 m_nVBSize;
	//SimpleList m_bitmaps;

	typedef stdext::hash_map<std::string,CD3DBitmapList*> t_bitmaplists;
	typedef t_bitmaplists::iterator t_bitmaplistsit;
	t_bitmaplists m_bitmaplists;

	int m_frameTriCount;
	
	DWORD m_dwStride;
	DWORD m_dwFVF;

	D3DCAPS8 m_caps;
	DWORD	m_dwD3DIndexSize;
public:
	IDirect3DSurface8 * GetTmpSurface(D3DFORMAT format);
	bool	IsAllowBlendType(BlendType type);
	DWORD	  GetD3DIndexSize() { return 2;}
	D3DFORMAT GetD3DIndexFormat(){ return D3DFMT_INDEX16;}
	int GetMaxTextureBlendStages() { return m_caps.MaxTextureBlendStages; }
	int GetMaxSimultaneousTextures(){ return m_caps.MaxSimultaneousTextures;}
	bool IsDoubleTexture();
	bool IsHasTransform(){ return m_bTransform;}
	void SetMatrix(D3DXMATRIX* pMat,D3DXMATRIX * pOldMat = NULL);
	D3DXMATRIXA16* GetMatrix(){ return &m_Transform;}

	void SetRotation(POINT pt,FLOAT fAngle);
	// 角度是顺时针方向的
	void SetRotation(D3DXVECTOR3 * pV,FLOAT fAngle);
	// 规格化变换矩阵
	void IdentityTransform();

	void InitResourceManager();

	int GetTriCount();

	void EnableLightMap(bool bEnable);

	CD3DDevice(void);
	~CD3DDevice(void);
	IDirect3DSurface8* GetBackSurface();
	void BeginFlush(FlushType type);
	
	HRESULT LockVertexBufferAlign(DWORD dwVertexNum,void ** pData,DWORD & dwStartVertex,size_t stride);
	HRESULT LockVertexBuffer(DWORD dwVertexNum,void** pData,DWORD &);
	HRESULT LockIndexBuffer(DWORD dwIndexNum,void** pData,DWORD &);
	HRESULT LockMultiTexVertexBuffer(DWORD dwVertexNum,void** pData,DWORD &);
	HRESULT _LockVertexBuffer(DWORD dwSize,void** pData,DWORD & dwStartOffset,DWORD dwStride = 0);
	HRESULT UnlockVertexBuffer();
	HRESULT UnlockIndexBuffer();
	HRESULT UnlockMultiTexVertexBuffer();

	HRESULT RenderTriangleStrip(IDirect3DTexture8 * pTex,stDevRenderInfo * pRenderInfo,BlendType blend);
	HRESULT RenderTriangleStrip(IDirect3DTexture8 * pTex,IDirect3DTexture8 * pTex2,stDevMultiTexRenderInfo * pRenderInfo,BlendType blend);
	HRESULT RenderTriangleList(IDirect3DTexture8 * pTex,stDevRenderInfo * pRenderInfo,size_t nRenderNum,BlendType blend);
	//HRESULT RenderTriangleStrip(IDirect3DTexture8 * pTex,stDevRenderInfoStrip& ri,BlendType blend);

	HRESULT RenderTriangleList(IDirect3DTexture8 * pTex,IDirect3DTexture8 * pTex2,stDevMultiTexRenderInfo * pRenderInfo,size_t nRenderNum,BlendType blend);
	//HRESULT RenderTriangleStrip(IDirect3DTexture8 * pTex,IDirect3DTexture8 * pTex2,stDevMultiTexRenderInfoStrip& ri,BlendType blend);
	void FinalFlush();
	void EndFlush();

	IDirect3DVertexBuffer8* GetVB(){ return m_pVB;}
	IDirect3DVertexBuffer8* GetMultiTexVB(){ return m_pVB;}
	IDirect3DIndexBuffer8*	GetIB(){ return m_pIB;}
	
	IDirect3DTexture8 * GetTmpTexture(D3DFORMAT format);
	
	HRESULT	SetStreamSource(DWORD dwStride,DWORD dwFVF);

	bool CheckTexFormat(D3DFORMAT format);

	IBitmapList* FindResource(const char* filename,DWORD dwFlags,TextureUsageType usage);
	IBitmapList* FindResource(const char* filename);
	void DeleteResource(const char* filename);
	void ReleaseResource();
	void ReleaseTextureByTime(DWORD);
	IDirect3DTexture8* CreateTexture(int width,int height,D3DFORMAT format,D3DPOOL pool = D3DPOOL_DEFAULT,DWORD Usage = 0);

	void CheckTexFormat();
	HRESULT CreateDevice();
	HRESULT CreateDevice(DWORD dwBehaviorFlags,D3DDEVTYPE dwDevType);
	bool ResetDevice();
	bool ReleaseAllVideoMemory();

	bool Create(HWND hWnd,DWORD dwWidth,DWORD dwHeight,DWORD dwPixelBits,DWORD dwFlags,enumProgramType eProgramType);
	bool Reset(DWORD dwWidth,DWORD dwHeight,DWORD dwPixelBits,DWORD dwFlags);
	void Destroy();
	bool TestCooperativeLevel();
	bool BeginScene();
	bool EndScene();
	bool Present();
	bool Present(const RECT * prc);
	void Clear( DWORD dwColor );
	
	void Run();
	void SetupMatrix();
	void SetupStates();
	void InvalidateDeviceObjects();
	bool Restore();
	HRESULT _Restore(bool isFullScreen);
private:
	IDirect3DDevice8* GetDevice(){ return m_pD3DD;}
public:
	D3DFORMAT GetBackFormat();
	D3DFORMAT GetSrcFormat(enumPixelFormat e);
	
	/*
	D3DFORMAT GetDstFormat(enumPixelFormat e)
	{
		switch(e)
		{
		case ePixDXT1:
			return m_bUsableDXT1 ? D3DFMT_DXT1 : D3DFMT_R5G6B5;
		case ePixDXT1_5551:
			return m_bUsableDXT1 ? D3DFMT_DXT1 : (m_bUsable5551 ? D3DFMT_A1R5G5B5 : D3DFMT_A4R4G4B4);
		case ePixDXT3:
			return m_bUsableDXT3 ? D3DFMT_DXT3 : D3DFMT_A4R4G4B4;
		case ePixPal8NotAlpha:
		case ePixJpegRGB:
			return m_bUsableDXT1 ? D3DFMT_DXT1 : D3DFMT_R5G6B5;
			return D3DFMT_R5G6B5;
		}
		return m_bUsableDXT3 ? D3DFMT_DXT3 : D3DFMT_A4R4G4B4;
		return D3DFMT_A4R4G4B4;
	}
	//*/

	struct TextureStageStates{
		DWORD states[256];
	};
	DWORD mRenderStates[256];
	TextureStageStates mTextureStageState[2];

	DWORD __GetRenderState(D3DRENDERSTATETYPE state);
	DWORD __GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type);
	void __SetRenderState(D3DRENDERSTATETYPE state,DWORD value);
	void __SetTextureStageState( DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);

	void SetDeviceDrawState(enumDrawState drawState,BlendType blend);
	void SetDeviceStateNoTex();
	void SetDeviceState(BlendType blend);
	void SetDeviceStateForMultiTex(BlendType blend);
	HRESULT Render(stRenderInfo& ri);
	HRESULT Render(stMultiTexRenderInfo& ri);
	bool FillRect(const RECT * lprc,DWORD colorLeftTop,DWORD colorRightTop,DWORD colorLeftBottom,DWORD colorRightBottom);
	bool FillRect(const RECT * lprc,DWORD color);
	bool DrawRectangle(const RECT * lprc,DWORD color);
	bool DrawPolyLine(const POINT* points,size_t pointCount,DWORD color);
	bool DrawLine(const POINT * pt1,const POINT *pt2,DWORD color);
	bool DrawLine( int x1,int y1,int x2,int y2,DWORD color);  //Add By David For 2006-3-9
	bool DrawTriangle(POINT pt1,POINT pt2,POINT pt3,DWORD color1,DWORD color2,DWORD color3);
	bool CD3DDevice::DrawTriangleFan(const POINT * pt,DWORD triangleNum,DWORD color);
	// 方向逆时针
	bool DrawRectPie(const RECT* prect,float fStartAngle,float fEndAngle,DWORD color);
	int SetFont(int iFont);
	int IsValidFont(int iFont);

	SIZE GetStringSize(const char* lpszString, size_t count,size_t maxWidth,bool bSingleLine = false,enumFontEffect fontEffect = FontEffect_None);
	void GetStringRect(const char* lpszString, size_t count,RECT & layoutRect,DWORD dwTextFormat = DT_TOP | DT_LEFT );
	void GetStringRect2(const char* lpszString, size_t count,RECT & layoutRect,RECT & totalRect,DWORD dwTextFormat = DT_TOP | DT_LEFT );

	int	SetFontHSpace(int n);
	int	SetFontVSpace(int n);
	int	SetFontTabSize(int n);

	void DrawString(const char* lpszString , size_t count,const RECT & layoutRect ,DWORD clrText = 0xffffffff,DWORD dwTextFormat = DT_TOP | DT_LEFT,enumFontEffect fontEffect = FontEffect_None,DWORD clrShadow = c_dwClrDefaultShadow,DWORD* pData = NULL);
	void DrawString(const WCHAR* lpszString , size_t count,const RECT & layoutRect ,DWORD clrText = 0xffffffff,DWORD dwTextFormat = DT_TOP | DT_LEFT,enumFontEffect fontEffect = FontEffect_None,DWORD clrShadow = c_dwClrDefaultShadow,DWORD* pData = NULL);
	void AddMapRenderInfo(stRenderInfo* pRenderInfo);
	void AddMapRenderInfo(stMultiTexRenderInfo* pRenderInfo);

	int GetFontHeight(int iFont = -1);
	int GetFontWidth(int iFont = -1);
	int GetFontWcharWidth(int iFont = -1);

	IDirect3DSurface8* GetRenderTarget();
	bool DrawTexture(IDirect3DTexture8* pTexture, const RECT* pRect = NULL, const RECT* pRectTexture = NULL, DWORD dwColor = 0xffffffff,BlendType blend = Blend_Null);

	size_t GetTotalTextureMemSize();
	size_t GetUseTextureMemSize();

	bool RenderBlendTexture(CCustomTexture* pTexture,CCustomTexture* pAlpha,const stRectF* pRect, const stRectF* pRectTexture1,const stRectF* pRectTexture2,DWORD color);
	bool RenderBlendTexture(IDirect3DTexture8* pTexture,IDirect3DTexture8* pAlpha,const stRectF* pRect, const stRectF* pRectTexture1,const stRectF* pRectTexture2,DWORD color);
	bool RenderBlendBitmap(IBitmap* pBmp,IBitmap* pAlpha,int x,int y,DWORD color = 0xffffffff,BlendType blend = Blend_Null);
	bool RenderBlendBitmap(IBitmap* pBmp,IBitmap* pAlpha,int x,int y,const RECT* rect = NULL,const stPointF * scale = NULL,DWORD color = 0xffffffff,BlendType blend = Blend_Null);

	void SetFontScale(float cx,float cy);
	stPointF GetFontScale();
	void SetTexFilter(bool bMagLinear,bool bMinLinear);
	void GetTexFilter(bool & bMagLinear,bool& bMinLinear)
	{
		bMagLinear = m_bTexMagLinear;
		bMinLinear = m_bTexMinLinear;
	}

	bool Present(const RECT * prcDst,const RECT * prcSrc);

	CCustomTexture* CreateCustomTexture(int width,int height,D3DFORMAT formt,DWORD usage = 0);

	void GetPicCharInfo( stPicCharInfo** ppPicCharInfo,size_t * count);
	void SetPicCharInfo( stPicCharInfo* p,size_t nPicCharInfoCount);

	int GetAFontWidth(int ch,int iFont = -1);
	stPicCharInfo* GetPicCharInfo(int ichar);

	size_t GetUseResMemorySize();

	bool IsLoadQueueLowerEmpty();
	void LockResLoad();
	void UnlockResLoad();

	HRESULT SaveBackSurfaceToFile(LPCTSTR  pDestFile,D3DXIMAGE_FILEFORMAT DestFormat,CONST RECT* pSrcRect) ;

	void SetBufferLockFlags(bool bSecond);

	IDirect3DTexture8 * CreateMemTex(D3DFORMAT format,int width,int height);
	IDirect3DSurface8 * CreateMemSurface(D3DFORMAT format, int width,int height);

	bool SetScreenPixelBits(int bits);

	void ResetScreenPixelBits();

	bool IsSupportFlag(DWORD flags);

	void UnmapAllBitmapData();
	bool IsSupportHardwareCursor();

	HRESULT __CopyRects(
		IDirect3DSurface8* pSourceSurface,
		CONST RECT* pSourceRectsArray,
		UINT cRects,
		IDirect3DSurface8* pDestinationSurface,
		CONST POINT* pDestPointsArray
		)
	{
		return m_pD3DD->CopyRects(pSourceSurface,pSourceRectsArray,cRects,pDestinationSurface,pDestPointsArray);
	}

	HRESULT __Clear(
		DWORD Count,
		CONST D3DRECT* pRects,
		DWORD Flags,
		D3DCOLOR Color,
		float Z,
		DWORD Stencil
		)
	{
		return m_pD3DD->Clear(Count,pRects,Flags,Color,Z,Stencil);
	}

	HRESULT __SetIndices(
		IDirect3DIndexBuffer8* pIndexData,
		UINT BaseVertexIndex
		)
	{
		return m_pD3DD->SetIndices(pIndexData,BaseVertexIndex);
	}

	HRESULT __SetTexture(
		DWORD Stage,
		IDirect3DBaseTexture8* pTexture
		)
	{
		return m_pD3DD->SetTexture(Stage,pTexture);
	}

	HRESULT __DrawIndexedPrimitive(
		D3DPRIMITIVETYPE Type,
		UINT MinIndex,
		UINT NumVertices,
		UINT StartIndex,
		UINT PrimitiveCount
		)
	{
		return m_pD3DD->DrawIndexedPrimitive(Type,MinIndex,NumVertices,StartIndex,PrimitiveCount);
	}

	HRESULT __DrawPrimitive(
		D3DPRIMITIVETYPE PrimitiveType,
		UINT StartVertex,
		UINT PrimitiveCount
		)
	{
		return m_pD3DD->DrawPrimitive(PrimitiveType,StartVertex,PrimitiveCount);
	}

	HRESULT __UpdateTexture(
		IDirect3DBaseTexture8* pSourceTexture,
		IDirect3DBaseTexture8* pDestinationTexture
		)
	{
		return m_pD3DD->UpdateTexture(pSourceTexture,pDestinationTexture);
	}

	HRESULT __SetCursorProperties(
		UINT XHotSpot,
		UINT YHotSpot,
		IDirect3DSurface8* pCursorBitmap
		)
	{
		return m_pD3DD->SetCursorProperties(XHotSpot,YHotSpot,pCursorBitmap);
	}

	HRESULT __CreateImageSurface(
		UINT Width,
		UINT Height,
		D3DFORMAT Format,
		IDirect3DSurface8** ppSurface
		)
	{
		return m_pD3DD->CreateImageSurface(Width,Height,Format,ppSurface);
	}

	BOOL __ShowCursor(
		BOOL bShow
		)
	{
		return m_pD3DD->ShowCursor(bShow);
	}

	HRESULT __GetRenderTarget(
		IDirect3DSurface8** ppRenderTarget
		)
	{
		return m_pD3DD->GetRenderTarget(ppRenderTarget);
	}

	HRESULT __SetRenderTarget(
		IDirect3DSurface8* pRenderTarget,
		IDirect3DSurface8* pNewZStencil
		)
	{
		return m_pD3DD->SetRenderTarget(pRenderTarget,pNewZStencil);
	}

	HRESULT __GetDisplayMode(
		D3DDISPLAYMODE* pMode
		)
	{
		return m_pD3DD->GetDisplayMode(pMode);
	}

};

//*/
