/**
 * \file      D3DDevice.cpp
 * \version   $Id$
 * \author    $$$03π´Àæ
 *            $$$03π´Àæ@###.com
 * \date      2005-07-05 12:29:19
 * \brief     D3DÕº–Œ…Ë±∏
 * 
 *	      D3DÕº–Œ…Ë±∏
 */

#include "../../engine/include/engine.h"

#include "../include/D3DBitmap.h"
#include "../include/d3ddevice.h"
#include "../include/D3DBitmapList.h"
#include "../include/TextureManager.h"
#include "../include/D3DTextout.h"
#include "../include/D3DLightMap.h"
#include "../Include/RenderD3D.h"
#include "../Include/D3DMBitmap.h"
#include "../include/TextureCell.h"
#include "../include/TextureCache.h"
#include "../include/CustomTexture.h"
#include "../../Engine/include/MathEx.h"
#include "../../engine/include/Ini.h"
#include "../include/D3DCursor.h"

const DWORD c_nTimeReleaseTexture = 30000;
const DWORD c_dwFlags = (D3DPRESENTFLAG_LOCKABLE_BACKBUFFER);
const DWORD c_maxRectangleNum = 100; 

#ifdef USE_INDEX_BUFFER

const DWORD c_maxVertexNum = c_maxRectangleNum * 4;

#else	

const DWORD c_maxVertexNum = c_maxRectangleNum * 6;

#endif

const DWORD c_maxIndexNum = c_maxRectangleNum * 6;

#define USE_HARDWARE_VERTEX

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param dwBits : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
D3DFORMAT GetPixelFormat(DWORD dwBits)
{
	switch(dwBits)
	{
	case 24:
		return D3DFMT_R8G8B8;
	case 32:
		return D3DFMT_X8R8G8B8;
	}
	return D3DFMT_R5G6B5;
}

const char* GetPixelFormatDesc(D3DFORMAT fmt)
{
	static char szDesc[64];
	switch(fmt)
	{
	case D3DFMT_DXT3:
		strcpy(szDesc,"D3DFMT_DXT3");
		return szDesc;
	case D3DFMT_DXT1:
		strcpy(szDesc,"D3DFMT_DXT1");
		return szDesc;
	case D3DFMT_R5G6B5:
		strcpy(szDesc,"D3DFMT_R5G6B5");
		return szDesc;
	case D3DFMT_A4R4G4B4:
		strcpy(szDesc,"D3DFMT_A4R4G4B4");
		return szDesc;
	default:
		sprintf(szDesc,"%d",fmt);
		return szDesc;
	}
}
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param dwFmt : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
DWORD GetPixelBits(D3DFORMAT dwFmt)
{
	switch(dwFmt)
	{
	case D3DFMT_X8R8G8B8:
		return 32;
	case D3DFMT_R8G8B8:
		return 24;
	}
	return 16;
}
DWORD GetDesktopBitsPixel()
{
	HDC hDC = GetDC(NULL);
	DWORD dwBitsPixel = GetDeviceCaps(hDC,BITSPIXEL);
	ReleaseDC(NULL,hDC);
	return dwBitsPixel;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
CD3DDevice::CD3DDevice(void)
: m_pD3D(NULL)
, m_pD3DD(NULL)
, m_dwDevType(D3DDEVTYPE_HAL)
, m_bUsableDXT1(false)
, m_bUsableDXT3(false)
, m_bUsable5551(false)
, m_bUsableA8(false)
, m_bTexMinLinear(false)
, m_bTexMagLinear(false)
, m_pVB(NULL)
//, m_pMultiTexVB(NULL)
, m_pIB(NULL)
, m_nVBOffset(0)
//, m_nMultiTexVBOffset(0)
, m_nIBOffset(0)
, m_frameTriCount(0)
, m_bTransform(false)
, m_dwD3DIndexSize(2)
, m_bSystemmemBuffer(false)
, m_nVBSize(0)
, m_dwStride(0)
, m_dwFVF(0)
{
	memset(&m_d3dpp,0,sizeof(m_d3dpp));
	memset(&m_tmptextures ,0,sizeof(m_tmptextures));
	memset(&m_tmpsurfaces ,0,sizeof(m_tmpsurfaces));
	m_arrayRenderInfo.array.reserve(256);
	SetBufferLockFlags(false);

	//m_bSystemmemBuffer = true;
}

void CD3DDevice::SetBufferLockFlags(bool bSecond)
{
#ifdef USE_DYNAMIC_BUFFER
	//if(bSecond)
	//{
	//	m_dwVertexLockFlags[0] = 0;//D3DLOCK_DISCARD;
	//	m_dwVertexLockFlags[1] = 0;//D3DLOCK_DISCARD;
	//}
	//else
	{
		m_dwVertexLockFlags[0] = D3DLOCK_NOOVERWRITE;
		m_dwVertexLockFlags[1] = D3DLOCK_DISCARD;
	}
#else
	m_dwVertexLockFlags[0] = 0;
	m_dwVertexLockFlags[1] = 0;
#endif
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
CD3DDevice::~CD3DDevice(void)
{
	//Destroy();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
HRESULT CD3DDevice::CreateDevice()
{
	HRESULT hResult ;
	m_dwDevType = D3DDEVTYPE_FORCE_DWORD;
	struct stCreateDevice{
		D3DDEVTYPE	dev;
		DWORD		flags;
	};
	stCreateDevice aCreate[]={
#ifdef USE_HARDWARE_VERTEX
		{D3DDEVTYPE_HAL,D3DCREATE_HARDWARE_VERTEXPROCESSING},
#endif
		{D3DDEVTYPE_HAL,D3DCREATE_SOFTWARE_VERTEXPROCESSING},
		{D3DDEVTYPE_REF,D3DCREATE_SOFTWARE_VERTEXPROCESSING},
	};


	for(size_t i=0; i < count_of(aCreate); ++i){
		hResult	= CreateDevice(aCreate[i].flags,aCreate[i].dev);
		if(FAILED(hResult)){
			TRACE_DXERROR(hResult);
		}else{
			m_dwDevType = aCreate[i].dev;
			m_dwBehaviorFlags = aCreate[i].flags;
			break;
		}
	}

	const char* szDevType[] = {
		"NULL",
		"D3DDEVTYPE_HAL",
		"D3DDEVTYPE_REF",
		"D3DDEVTYPE_SW",
	};

	Engine_WriteLogF("create device succeeded. %s : %s\n",szDevType[m_dwDevType],
		((m_dwBehaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING) ? "D3DCREATE_SOFTWARE_VERTEXPROCESSING" : 
		((m_dwBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) ? "D3DCREATE_MIXED_VERTEXPROCESSING" : "D3DCREATE_HARDWARE_VERTEXPROCESSING") ) );

	return S_OK;;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param dwBehaviorFlags : √Ë ˆ
 * \param dwDevType : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
HRESULT CD3DDevice::CreateDevice(DWORD dwBehaviorFlags,D3DDEVTYPE dwDevType)
{
	HRESULT hResult;

	if( (dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) && D3DDEVTYPE_HAL == dwDevType )
		dwBehaviorFlags |= (D3DCREATE_PUREDEVICE);
	//dwBehaviorFlags |= D3DCREATE_DISABLE_DRIVER_MANAGEMENT;
	hResult	=	m_pD3D->CreateDevice(	D3DADAPTER_DEFAULT,
		dwDevType,
		m_hWnd,
		dwBehaviorFlags,
		&m_d3dpp,
		&m_pD3DD );

	if(FAILED(hResult))
	{
		TRACE_DXERROR(hResult);

		if((m_dwFlags & DEVICE_FULLSCREEN) == 0 && hResult == D3DERR_OUTOFVIDEOMEMORY)
		{
			TRACE("¥¥Ω®…Ë±∏ ß∞‹£°Ω¯»Î“ªœ¬»´∆¡£¨ Õ∑≈œ‘¥Ê£°\n");
			BeginFullScreen(&m_dm);
			EndFullScreen();
			hResult	=	m_pD3D->CreateDevice(	D3DADAPTER_DEFAULT,
				dwDevType,
				m_hWnd,
				dwBehaviorFlags,
				&m_d3dpp,
				&m_pD3DD );
		}

	}

	return hResult;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::Destroy()
{
	EndFullScreen();
	GetD3DCursor()->release();
	GetD3DMBitmapManager()->Destroy();
	for(tMapRenderInfoIt it = m_mapRenderInfo.begin(); it != m_mapRenderInfo.end(); ++it)
	{
		m_allocRenderInfos.freePointer(it->second);
	}
	m_mapRenderInfo.clear();

	for(tMapMultiTexRenderInfoIt it = m_mapMultiTexRenderInfo.begin(); it != m_mapMultiTexRenderInfo.end(); ++it)
	{
		m_allocMultiTexRenderInfos.freePointer(it->second);
	}
	m_mapMultiTexRenderInfo.clear();

	CD3DBitmapList::DeleteResThread();
	if(GetDevice())
	{
		InvalidateDeviceObjects();
		GetD3DLightMap()->Release();
	}
	ReleaseResource();
	for(size_t i=0;i<eTmpTexNum;++i)
	{
		m_tmptextures[i].release();
	}
	for(size_t i=0;i<eTmpTexNum;++i)
	{
		SAFE_RELEASE(m_tmpsurfaces[i]);
	}
	GetD3DMBitmapManager()->Release();
	GetTextureManager()->Destroy();
	SAFE_RELEASE(m_pD3DD);
	SAFE_RELEASE(m_pD3D);
	ResetScreenPixelBits();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::ReleaseResource()
{
	for(t_bitmaplistsit it = m_bitmaplists.begin(); it != m_bitmaplists.end(); ++it)
	{
		delete it->second;
	}
	m_bitmaplists.clear();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param format : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DDevice::CheckTexFormat(D3DFORMAT format)
{
	return SUCCEEDED(m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,m_dwDevType,m_d3dpp.BackBufferFormat,0,D3DRTYPE_TEXTURE,format));
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::CheckTexFormat()
{
	m_bUsableDXT1 = SUCCEEDED(m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,m_dwDevType,m_d3dpp.BackBufferFormat,0,D3DRTYPE_TEXTURE,D3DFMT_DXT1));
	m_bUsableDXT3 = SUCCEEDED(m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,m_dwDevType,m_d3dpp.BackBufferFormat,0,D3DRTYPE_TEXTURE,D3DFMT_DXT3));
	m_bUsable5551 = SUCCEEDED(m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,m_dwDevType,m_d3dpp.BackBufferFormat,0,D3DRTYPE_TEXTURE,D3DFMT_A1R5G5B5));
	m_bUsableA8 = SUCCEEDED(m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,m_dwDevType,m_d3dpp.BackBufferFormat,0,D3DRTYPE_TEXTURE,D3DFMT_A8));

	//if(0 == (GetFlags() & DEVICE_DXT)){
	//	m_bUsableA8 = m_bUsableDXT3 = m_bUsableDXT1 = false;
	//}

}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param bEnable : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::EnableLightMap(bool bEnable)
{
	GetD3DLightMap()->SetEnabled(bEnable);
}

bool CD3DDevice::IsDoubleTexture()
{ 
	return GetMaxTextureBlendStages() > 1 && GetMaxSimultaneousTextures() > 1;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ 
 */
bool CD3DDevice::ResetDevice()
{
	//Engine_WriteLogF("µ±«∞œ‘ æƒ£ Ω %d x %d %d Œª—’…´\n",m_d3dpp.BackBufferWidth,m_d3dpp.BackBufferHeight,::GetPixelBits(m_d3dpp.BackBufferFormat));
	//GetDevice()->ShowCursor(FALSE);


	DWORD dwFVF = 0;
	D3DPOOL pool = D3DPOOL_DEFAULT;
	HRESULT hr;
	m_dwStride = 0;
	m_dwFVF = 0;
	m_nIBOffset = 0;
	m_nVBOffset = 0;

	GetDevice()->GetDeviceCaps(&m_caps);
	D3DDISPLAYMODE displayMode;
	GetDevice()->GetDisplayMode(&displayMode);
	
	std::string strMsg;

	strMsg = ("\nDevice capabilities\n");
	strMsg += ("--------------------------------------------------\n");
	strMsg += avar("Current Mode %d x %d, %d bits, %d hz\n",displayMode.Width,displayMode.Height,::GetPixelBits(displayMode.Format),displayMode.RefreshRate);
	strMsg += avar("Max texture size        : %d x %d\n",m_caps.MaxTextureWidth,m_caps.MaxTextureHeight);
	strMsg += avar("MaxSimultaneousTextures : %d\n",m_caps.MaxSimultaneousTextures);
	strMsg += avar("MaxTextureBlendStages   : %d\n",m_caps.MaxTextureBlendStages);
	strMsg += avar("MaxPrimitiveCount       : %d\n",m_caps.MaxPrimitiveCount);
	strMsg += avar("MaxVertexIndex          : %d\n",m_caps.MaxVertexIndex);
	strMsg += avar("CursorCaps              : 0x%x\n",m_caps.CursorCaps);
	Engine_WriteLog(strMsg.c_str());

	strMsg = "\nTexture formats\n";
	strMsg += ("--------------------------------------------------\n");
	strMsg += avar("A8R8G8B8     : %d\n",CheckTexFormat(D3DFMT_A8R8G8B8));
	strMsg += avar("X8R8G8B8     : %d\n",CheckTexFormat(D3DFMT_X8R8G8B8));
	strMsg += avar("A4R4G4B4     : %d\n",CheckTexFormat(D3DFMT_A4R4G4B4));
	strMsg += avar("R5G6B5       : %d\n",CheckTexFormat(D3DFMT_R5G6B5));
	strMsg += avar("A1R5G5B5     : %d\n",CheckTexFormat(D3DFMT_A1R5G5B5));
	strMsg += avar("A8           : %d\n",CheckTexFormat(D3DFMT_A8));
	strMsg += avar("DXT1         : %d\n",CheckTexFormat(D3DFMT_DXT1));
	strMsg += avar("DXT3         : %d\n",CheckTexFormat(D3DFMT_DXT3));
	Engine_WriteLog(strMsg.c_str());

	for(size_t i=0; i<count_of(mTextureStageState);++i){
		for(size_t j=0;j<count_of(mTextureStageState[i].states);++j)
			mTextureStageState[i].states[j] = D3DTSS_FORCE_DWORD;
	}
	
	for(size_t i=0; i<count_of(mRenderStates);++i){
		mRenderStates[i] = D3DRS_FORCE_DWORD;
	}

	GetD3DCursor()->init();

	DWORD dwBufSize = (IsDoubleTexture() ? sizeof(stMultiTexVertexBitmap) * c_maxVertexNum : sizeof(stVertexBitmap) * c_maxVertexNum);

	m_nVBSize = dwBufSize;
	DWORD dwUsageOther = D3DUSAGE_WRITEONLY /*| D3DUSAGE_DONOTCLIP*/;
	//D3DUSAGE_SOFTWAREPROCESSING
	if(m_dwBehaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING){
		dwUsageOther |= D3DUSAGE_SOFTWAREPROCESSING;
	}
	if(m_bSystemmemBuffer)
	{
		pool = D3DPOOL_SYSTEMMEM;

		hr = GetDevice()->CreateVertexBuffer(dwBufSize, dwUsageOther,dwFVF,pool,&m_pVB);
		if(FAILED(hr))
		{
			TRACE_DXERROR(hr);
			return false;
		}
#ifdef USE_INDEX_BUFFER
		pool = D3DPOOL_SYSTEMMEM;
		hr = GetDevice()->CreateIndexBuffer(GetD3DIndexSize() * c_maxIndexNum,dwUsageOther,GetD3DIndexFormat() ,pool,&m_pIB);
		if(FAILED(hr))
		{
			TRACE_DXERROR(hr);
			return false;
		}
#endif
	}
	else
	{

#ifdef USE_DYNAMIC_BUFFER
		DWORD dwFlagDynamic = D3DUSAGE_DYNAMIC;
#else
		DWORD dwFlagDynamic = 0;
#endif	

		pool = D3DPOOL_DEFAULT;
		hr = GetDevice()->CreateVertexBuffer(dwBufSize,dwFlagDynamic |  dwUsageOther,dwFVF,pool,&m_pVB);
		if(FAILED(hr))
		{
			TRACE_DXERROR(hr);
			pool = D3DPOOL_SYSTEMMEM;
			hr = GetDevice()->CreateVertexBuffer(dwBufSize, dwUsageOther,dwFVF,pool,&m_pVB);
			if(FAILED(hr))
			{
				TRACE_DXERROR(hr);
				return false;
			}
		}
#ifdef USE_INDEX_BUFFER
		pool = D3DPOOL_DEFAULT;
		hr = GetDevice()->CreateIndexBuffer(GetD3DIndexSize() * c_maxIndexNum,dwFlagDynamic |  dwUsageOther,GetD3DIndexFormat() ,pool,&m_pIB);
		if(FAILED(hr))
		{
			TRACE_DXERROR(hr);
			pool = D3DPOOL_SYSTEMMEM;
			hr = GetDevice()->CreateIndexBuffer(GetD3DIndexSize() * c_maxIndexNum, dwUsageOther,GetD3DIndexFormat() ,pool,&m_pIB);
			if(FAILED(hr))
			{
				TRACE_DXERROR(hr);
				return false;
			}
		}
#endif
	}

	UpdateWndSize();
	if(IsDoubleTexture())
	{
		GetD3DMBitmapManager()->Release();
	}

	GetD3DLightMap()->Init();
	GetTextureManager()->Init();

	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param hWnd : √Ë ˆ
 * \param iWidth : √Ë ˆ
 * \param iHeight : √Ë ˆ
 * \param dwPixelBits : √Ë ˆ
 * \param dwFlags : √Ë ˆ
 * \param eProgramType : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
const int REC_W = 1024;
const int REC_H = 768;
bool CD3DDevice::Create(HWND hWnd,DWORD iWidth,DWORD iHeight,DWORD dwPixelBits,DWORD dwFlags,enumProgramType eProgramType)
{
	POINT pos = {0,0};
	ClientToScreen(hWnd,&pos);

	m_eProgramType = eProgramType;
	HRESULT	hResult;

	SAFE_RELEASE(m_pD3DD);
	SAFE_RELEASE(m_pD3D);

	m_pD3D	=	Direct3DCreate8( D3D_SDK_VERSION );

	if ( m_pD3D == NULL )
	{
		return	false;
	}

	D3DADAPTER_IDENTIFIER8 identifiet;
	size_t acount = m_pD3D->GetAdapterCount();
	Engine_WriteLogF("Rendering device count %d\n",acount);
	for(size_t i=0;i < acount; ++i){
		hResult = m_pD3D->GetAdapterIdentifier(i,D3DENUM_NO_WHQL_LEVEL,&identifiet);
		if(SUCCEEDED(hResult)){
			DWORD highPart = ((DWORD*)&identifiet.DriverVersion.QuadPart)[1];
			DWORD lowPart = ((DWORD*)&identifiet.DriverVersion.QuadPart)[0];
			WORD Product = HIWORD(highPart);
			WORD Version = LOWORD(highPart);
			WORD SubVersion = HIWORD(lowPart);
			WORD Build = LOWORD(lowPart);
			Engine_WriteLogF("Rendering device %d: %s [ %s ] version %d.%d.%d.%d\n",i,identifiet.Driver,identifiet.Description,Product,Version,SubVersion,Build);
		}
	}

	CheckDisplayMode();

	DEVMODE dm;
	if( !GetDisplayMode(&dm,iWidth,iHeight,dwPixelBits) )
	{
		TRACE( "√ª”–ø…”√µƒœ‘ æƒ£ Ω!" );
		return	false;
	}


	iWidth = dm.dmPelsWidth;
	iHeight = dm.dmPelsHeight;
	dwPixelBits = dm.dmBitsPerPel;

	m_iWidth	=	iWidth;
	m_iHeight	=	iHeight;
	m_dwFlags	=	dwFlags;
	m_hWnd		=	hWnd;
	m_iPixelBits = dwPixelBits;

	ZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );

	m_d3dpp.hDeviceWindow		=	hWnd;
	m_d3dpp.BackBufferWidth		=	m_iWidth;
	m_d3dpp.BackBufferHeight	=	m_iHeight;

	m_d3dpp.BackBufferCount	=	1;
	m_d3dpp.SwapEffect =	D3DSWAPEFFECT_DISCARD;
	m_d3dpp.BackBufferFormat = GetPixelFormat(m_iPixelBits);
	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	m_dm = dm;

	if(m_dwFlags & DEVICE_FULLSCREEN)
	{
		if(!BeginFullScreen(&dm))
			return false;
	}
	else{
		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);

		if(m_d3dpp.BackBufferWidth > cx || m_d3dpp.BackBufferHeight > cy) 
		{
			// ◊¿√Ê∑÷±Á¬ ≤ª÷ß≥÷◊™»´∆¡
			if( !BeginFullScreen(&dm) )
			{
				EndFullScreen();
				DWORD dwPixel = GetDesktopBitsPixel();
				SetScreenPixelBits(m_iPixelBits);
				m_d3dpp.BackBufferWidth = cx;
				m_d3dpp.BackBufferHeight = cy;
				m_iWidth = cx;
				m_iHeight = cy;
			}
		}else{
			DWORD dwPixel = GetDesktopBitsPixel();
			SetScreenPixelBits(m_iPixelBits);
		}
	}

	ResetWindowPos();

	m_d3dpp.Windowed			=	true;
	m_d3dpp.Flags				=	c_dwFlags;
	if(m_dwFlags & DEVICE_VSYN) m_d3dpp.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
	//m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	//if(m_dwFlags & DEVICE_VSYN)	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	//else m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	m_d3dpp.EnableAutoDepthStencil	= false;
	m_d3dpp.AutoDepthStencilFormat	= D3DFMT_D16;

	m_d3dpp.BackBufferFormat = GetPixelFormat(GetDesktopBitsPixel());

	hResult = CreateDevice();

	if(FAILED(hResult))
	{
		TRACE_DXERROR(hResult);
		return false;
	}

	m_iWidth = m_d3dpp.BackBufferWidth;
	m_iHeight = m_d3dpp.BackBufferHeight;


	m_iPixelBits = ::GetPixelBits(m_d3dpp.BackBufferFormat);

	GetDevice()->GetDeviceCaps(&m_caps);
	CheckTexFormat();
 
	m_tmptextures[eTmpTex4444].init(D3DFMT_A4R4G4B4,1);
	m_tmptextures[eTmpTex565].init(D3DFMT_R5G6B5,1);
	m_tmptextures[eTmpTex5551].init(D3DFMT_A1R5G5B5,1);
	m_tmptextures[eTmpTexDXT1].init(D3DFMT_DXT1,1);
	m_tmptextures[eTmpTexDXT3].init(D3DFMT_DXT3,1);
	m_tmptextures[eTmpTexA8R8G8B8].init(D3DFMT_A8R8G8B8,1);
	m_tmptextures[eTmpTexX8R8G8B8].init(D3DFMT_X8R8G8B8,1);
	m_tmptextures[eTmpTexA8].init(D3DFMT_A8,1);
	m_dwD3DIndexSize = 2;

	ResetDevice();

	SetupMatrix();

	SetupStates();

	m_rcRender.left = 0;
	m_rcRender.top = 0;
	m_rcRender.right = GetWidth();
	m_rcRender.bottom = GetHeight();

	GetD3DTextout()->Init();

	GetD3DMBitmapManager()->Create();
	m_bCreated = true;

	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::InitResourceManager()
{
	CD3DBitmapList::InitResThread();
}

void CD3DDevice::SetupMatrix( )	// …Ë÷√ ¿ΩÁæÿ’Û
{
	if ( m_pD3DD == NULL )
		return ;

	if ( m_iWidth == 0 || m_iHeight == 0 )
		return	;

	D3DVIEWPORT8 vp;
	vp.X      = 0;
	vp.Y      = 0;
	vp.Width  = m_iWidth;
	vp.Height = m_iHeight;
	vp.MinZ   = 0.0f;
	vp.MaxZ   = 1.0f;

	m_pD3DD->SetViewport(&vp);

	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 d3dMatrixView;
	D3DXMATRIXA16 d3dMatrixProj;

	D3DXMatrixIdentity( &matWorld );
	m_pD3DD->SetTransform( D3DTS_WORLD,      &matWorld );

	D3DXMatrixTranslation(&d3dMatrixView,(float)(-((int)m_iWidth/2)),(float)(-((int)m_iHeight/2)) ,0.0f);
	m_pD3DD	->	SetTransform( D3DTS_VIEW, &d3dMatrixView );

	D3DXMatrixOrthoLH(&d3dMatrixProj,(float)m_iWidth,(float)m_iHeight,0.0f,1.0f);
	m_pD3DD	->	SetTransform( D3DTS_PROJECTION, &d3dMatrixProj );

}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param dwColor : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::Clear( DWORD dwColor )
{
	if ( m_pD3DD == NULL )
		return ;
	D3DRECT rect = {0,0,GetWidth(),GetHeight()};
	m_pD3DD	->	Clear	(	1, 
		&rect,
		D3DCLEAR_TARGET,
		dwColor,
		1.0f, 
		0L );
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param bMagLinear : √Ë ˆ
 * \param bMinLinear : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::SetTexFilter(bool bMagLinear,bool bMinLinear)
{
	if(bMagLinear == m_bTexMagLinear && bMinLinear == m_bTexMinLinear)
		return;

	FinalFlush();

	__SetTextureStageState	( 0, D3DTSS_MAGFILTER, (bMagLinear ? D3DTEXF_LINEAR : D3DTEXF_POINT) );
	__SetTextureStageState	( 0, D3DTSS_MINFILTER, (bMinLinear ? D3DTEXF_LINEAR : D3DTEXF_POINT) );

	m_bTexMagLinear = bMagLinear;
	m_bTexMinLinear = bMinLinear;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DDevice::TestCooperativeLevel()
{
	HRESULT hr;
	// Test the cooperative level to see if it's okay to render
	if( FAILED( hr = m_pD3DD->TestCooperativeLevel() ) )
	{
		// If the device was lost, do not render until we get it back
		if( D3DERR_DEVICELOST == hr )
		{
			//InvalidateDeviceObjects();
			return SUCCEEDED(hr);
		}

		// Check if the device needs to be resized.
		if( D3DERR_DEVICENOTRESET == hr )
		{
			TRACE("Device lost but can be reset...\n");

			if(Restore())
				return true;
			return false;
		}
		return SUCCEEDED(hr);
	}
	return SUCCEEDED(hr);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DDevice::BeginScene()
{
	HRESULT hr;
	
	if( FAILED(hr = m_pD3DD->BeginScene()) ) 
	{
		TRACE_DXERROR(hr);
		return false;
	}
	m_frameTriCount = 0;
	m_FlushType = Flush_Cover;

	m_bMonochrome = false;
	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DDevice::EndScene()
{
	EndFlush();

	return SUCCEEDED(m_pD3DD->EndScene());
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DDevice::Present()
{
	RECT rc = {0,0,GetWidth(),GetHeight()};
	if(GetFlags() & DEVICE_FULLSCREEN)
		return SUCCEEDED(m_pD3DD->Present(&rc,&rc, NULL, NULL ));
	else
	{
		RECT rcDst = {0,0,GetWndWidth(),GetWndHeight()};
		return SUCCEEDED(m_pD3DD->Present(&rc,&rcDst, NULL, NULL ));
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param RECT : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DDevice::Present(const RECT * prc)
{
	HRESULT hr = (m_pD3DD->Present(prc,prc, NULL, NULL ));
	if( D3DERR_DEVICELOST == hr )
	{
		InvalidateDeviceObjects();
	}
	return SUCCEEDED(hr);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param RECT : √Ë ˆ
 * \param RECT : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DDevice::Present(const RECT * prcDst,const RECT * prcSrc)
{
	HRESULT hr = m_pD3DD->Present(prcSrc,prcDst, NULL, NULL );
	if( D3DERR_DEVICELOST == hr )
	{
		InvalidateDeviceObjects();
	}
	return SUCCEEDED(hr);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::InvalidateDeviceObjects()
{
	for(size_t i=0;i<eTmpTexNum;++i)
	{
		m_tmptextures[i].release();
	}
	for(size_t i=0;i<eTmpTexNum;++i)
	{
		SAFE_RELEASE(m_tmpsurfaces[i]);
	}

	GetD3DCursor()->invalidateDevice();

	m_pD3DD->SetStreamSource(0,NULL,0);
	m_pD3DD->SetIndices(NULL,0);
	SAFE_RELEASE( m_pVB );
	//SAFE_RELEASE( m_pMultiTexVB );
	SAFE_RELEASE( m_pIB );
	ReleaseAllVideoMemory();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
HRESULT CD3DDevice::_Restore(bool isFullScreen)
{
	HRESULT hResult;

	D3DPRESENT_PARAMETERS d3dpp1 = m_d3dpp;

	DEVMODE dm;
	if (isFullScreen)
	{
		if( !GetDisplayMode(&dm,m_iWidth,m_iHeight,m_iPixelBits) )
		{
			TRACE( "√ª”–ø…”√µƒœ‘ æƒ£ Ω!\n" );
			return	E_FAIL;
		}

		m_iWidth = dm.dmPelsWidth;
		m_iHeight = dm.dmPelsHeight;
		m_iPixelBits = dm.dmBitsPerPel;
	}
	else
	{
		dm.dmPelsWidth = m_iWidth;
		dm.dmPelsHeight = m_iHeight;
		dm.dmBitsPerPel = m_iPixelBits;
	}

	ZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );

	m_d3dpp.hDeviceWindow		=	m_hWnd;
	m_d3dpp.BackBufferWidth		=	m_iWidth;
	m_d3dpp.BackBufferHeight	=	m_iHeight;

	m_d3dpp.BackBufferCount	=	1;
	m_d3dpp.SwapEffect =	D3DSWAPEFFECT_DISCARD;

	m_dm = dm;

	if ( m_dwFlags & DEVICE_FULLSCREEN )
	{
		if(!BeginFullScreen(&dm))
		{
			EndFullScreen();
			m_dwFlags &= (~DEVICE_FULLSCREEN);
		}
	}
	else
	{
		EndFullScreen();
		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);

		if(m_d3dpp.BackBufferWidth > cx || m_d3dpp.BackBufferHeight > cy) 
		{
			// ◊¿√Ê∑÷±Á¬ ≤ª÷ß≥÷◊™»´∆¡
			if( !BeginFullScreen(&dm) )
			{
				EndFullScreen();
				DWORD dwPixel = GetDesktopBitsPixel();
				SetScreenPixelBits(m_iPixelBits);
				m_d3dpp.BackBufferWidth = cx;
				m_d3dpp.BackBufferHeight = cy;
				m_iWidth = cx;
				m_iHeight = cy;
			}
		}else{
			DWORD dwPixel = GetDesktopBitsPixel();
			SetScreenPixelBits(m_iPixelBits);
		}
	}

	m_iPixelBits = GetDesktopBitsPixel();

	m_d3dpp.Windowed			=	true;	//  µº ∂º «¥∞ø⁄ƒ£ Ω£®ºŸ»´∆¡£©
	m_d3dpp.Flags				=	c_dwFlags;
	if(m_dwFlags & DEVICE_VSYN) 
		m_d3dpp.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;

	m_d3dpp.BackBufferFormat = GetPixelFormat(m_iPixelBits);

	m_d3dpp.EnableAutoDepthStencil	= false;
	m_d3dpp.AutoDepthStencilFormat	= D3DFMT_D16;

	hResult = m_pD3DD->Reset(&m_d3dpp);

	if(FAILED(hResult))
	{
		if((m_dwFlags & DEVICE_FULLSCREEN) == 0)
		{
			TRACE("÷ÿ…Ë…Ë±∏ ß∞‹£°Ω¯»Î“ªœ¬»´∆¡£¨ Õ∑≈œ‘¥Ê£°\n");
			BeginFullScreen(&dm);
			EndFullScreen();
			hResult = m_pD3DD->Reset(&m_d3dpp);
		}
	}

	m_iWidth = m_d3dpp.BackBufferWidth;
	m_iHeight = m_d3dpp.BackBufferHeight;

	m_d3dpp.BackBufferFormat = GetPixelFormat(m_iPixelBits);

	ResetWindowPos();

	m_rcRender.left = 0;
	m_rcRender.top = 0;
	m_rcRender.right = m_iWidth;
	m_rcRender.bottom = m_iHeight;

	return hResult;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DDevice::Restore()
{
	return Reset(m_iWidth,m_iHeight,m_iPixelBits,m_dwFlags);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DDevice::ReleaseAllVideoMemory()
{
	GetDevice()->SetTexture(0,NULL);
	GetTextureManager()->ReleaseAllTexture();
	/*
	DWORD dw1 = xtimeGetTime();
	GetDevice()->SetTexture(0,NULL);
	int i =0;
	for(CD3DBitmap* pBmp = (CD3DBitmap*)m_bitmaps._Begin(); pBmp ; )
	{
	pBmp->ReleaseTexture();
	CD3DBitmap* pCurBmp = pBmp;
	pBmp = (CD3DBitmap*)pBmp->_Next();

	m_bitmaps._Delete(pCurBmp);
	++i;
	}
	TRACE("ReleaseAllVideoMemory time :%u,count:%u\n",xtimeGetTime() - dw1,i);
	//*/
	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param width : √Ë ˆ
 * \param height : √Ë ˆ
 * \param format : √Ë ˆ
 * \param pool : √Ë ˆ
 * \param Usage : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
IDirect3DTexture8* CD3DDevice::CreateTexture(int width,int height,D3DFORMAT format,D3DPOOL pool,DWORD Usage)
{
	HRESULT hr;
	IDirect3DTexture8 * p = NULL;
	/*hr = m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,m_dwDevType,m_d3dpp.BackBufferFormat,Usage,D3DRTYPE_TEXTURE,format);
	if(FAILED(hr))
	{
#if defined _DEBUG || defined RELEASE_TEST
		const char* p = DXGetErrorDescription8(hr);
		Engine_WriteLogF("CreateTexture Failed: %dx%d , format %s, pool %d, Usage %d ,%s\n",width,height,GetPixelFormatDesc(format),pool,Usage,p ? p : "");
#endif
		return NULL;
	}*/

	hr = m_pD3DD->CreateTexture(width,height,1,Usage,format,pool,&p);
	//hr = m_pD3DD->CreateTexture(width,height,1,Usage,format,D3DPOOL_MANAGED,&p);
	if(FAILED(hr))
	{
#if defined _DEBUG || defined RELEASE_TEST
		const char* p = DXGetErrorDescription8(hr);
		Engine_WriteLogF("CreateTexture Failed: %dx%d , format %s, pool %d, Usage %d ,%s\n",width,height,GetPixelFormatDesc(format),pool,Usage,p ? p : "");
		//TRACE_DXERROR(hr);
#endif
		return NULL;
	}
	else
	{
		return p;
	}
	/*
	IDirect3DTexture8* pTex = NULL;
	HRESULT hr;
	hr = m_pD3DD->CreateTexture(width,height,1,0,format,D3DPOOL_DEFAULT,&pTex);
	if(SUCCEEDED(hr)) return pTex;

	if(hr == D3DERR_OUTOFVIDEOMEMORY)
	{
	//ReleaseTextureByTime(c_nTimeReleaseTexture);
	ReleaseAllVideoMemory();
	for(;;)
	{
	hr = m_pD3DD->CreateTexture(width,height,1,0,format,D3DPOOL_DEFAULT,&pTex);
	if(SUCCEEDED(hr)) return pTex;
	if(hr == D3DERR_OUTOFVIDEOMEMORY)
	{
	if(!ReleaseLastTexture()) return NULL;
	}
	else
	{
	TRACE_DXERROR(hr);
	Assert(0);
	return NULL;
	}

	}
	}
	TRACE_DXERROR(hr);
	Assert(0);
	return NULL;
	//*/
}



/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param char* : √Ë ˆ
 * \param dwFlags : √Ë ˆ
 * \param usage : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
CD3DBitmapList * CreateResource(const char* pszFile,DWORD dwFlags,TextureUsageType usage)
{
	CD3DBitmapList* p = NULL;
	static DWORD s_dwResourceID = 1;
	p = new CD3DBitmapList(pszFile,dwFlags,s_dwResourceID,usage);
	if(!p) return NULL;

	if(p->Open())
	{
		TRACE("*********New Resource ID:%d %s,Flg:0x%x,Usage:%d***************\n",s_dwResourceID,pszFile,dwFlags,usage);
		s_dwResourceID++;
		return p;
	}

	delete p;

	return NULL;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param char* : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::DeleteResource(const char* filename)
{
	char strFileName[MAX_PATH];
	strcpy(strFileName,filename);
	strlwr(strFileName);
	char * p = strrchr(strFileName,'.');
	strcpy(p,".pak");

	t_bitmaplistsit it = m_bitmaplists.find(strFileName);
	if(it == m_bitmaplists.end())
		return;

	FinalFlush();
	GetTextureManager()->ReleaseAll();
	it->second->ReleaseTexture();
	CD3DBitmapList::DeleteLoadResource(it->second);
	delete it->second;

	TRACE("********* Delete Resource %s ***************\n",filename);

	m_bitmaplists.erase(it);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param char* : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
IBitmapList* CD3DDevice::FindResource(const char* filename)
{
	char strFileName[MAX_PATH];
	strcpy(strFileName,filename);
	strlwr(strFileName);
	char * p = strrchr(strFileName,'.');
	if(!p) 
		return NULL;
	strcpy(p,".pak");

	t_bitmaplistsit it = m_bitmaplists.find(strFileName);
	if(it == m_bitmaplists.end())
	{
		return NULL;
	}
	return it->second;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param char* : √Ë ˆ
 * \param dwFlags : √Ë ˆ
 * \param usage : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
IBitmapList* CD3DDevice::FindResource(const char* filename,DWORD dwFlags,TextureUsageType usage)
{
	char strFileName[MAX_PATH];
	strcpy(strFileName,filename);
	strlwr(strFileName);
	char * p = strrchr(strFileName,'.');
	if(!p) 
		return NULL;
	strcpy(p,".pak");

	t_bitmaplistsit it = m_bitmaplists.find(strFileName);
	if(it == m_bitmaplists.end())
	{
		CD3DBitmapList* p = CreateResource(strFileName,dwFlags,usage);
		if(p)
		{
			m_bitmaplists[strFileName] = p;
			return p;
		}
		return NULL;
	}
	return it->second;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::Run()
{
	//*
	const int c_nTimeClear = 3*60*1000;
	static DWORD time = xtimeGetTime();
	if (xtimeGetTime() - time >= c_nTimeClear)
	{
		time = xtimeGetTime();
		CD3DBitmapList::ClearResMemory();
	}

	/*
	static DWORD time1 = xtimeGetTime();
#ifdef _DEBUG
	const int c_nTimeUnmapView = 10*60*1000;
#else
	const int c_nTimeUnmapView = 10*60*1000;
#endif
	if ( xtimeGetTime() - time1 >= c_nTimeUnmapView )
	{
		time1 = xtimeGetTime();
		EnterCriticalSection(&CD3DBitmapList::g_csloaded2);
		for( t_bitmaplistsit it = m_bitmaplists.begin(); it != m_bitmaplists.end(); ++it )
		{
			it->second->UnmapFileGroupByTime( c_nTimeUnmapView );
		}
		LeaveCriticalSection(&CD3DBitmapList::g_csloaded2);
	}
	//*/
	/*
	//const int c_nTimeReleaseTexture = 30000;
	//static DWORD time2 = xtimeGetTime();
	//if(xtimeGetTime() - time2 >= c_nTimeReleaseTexture)

	size_t i=1;
	while(GetDevice()->GetAvailableTextureMem() < 2)
	{
	DWORD dw1 = xtimeGetTime();
	this->ReleaseTextureByTime(c_nTimeReleaseTexture/i);
	DWORD time2 = xtimeGetTime();
	TRACE("run ReleaseTexture Time : %u\n",time2  - dw1);
	i++;
	}
	//*/
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param char* : √Ë ˆ
 * \param count : √Ë ˆ
 * \param & : √Ë ˆ
 * \param dwTextFormat : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::GetStringRect(const char* lpszString, size_t count,RECT & layoutRect,DWORD dwTextFormat)
{
	GetD3DTextout()->GetStringRect(lpszString,count,layoutRect,dwTextFormat);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param char* : √Ë ˆ
 * \param count : √Ë ˆ
 * \param & : √Ë ˆ
 * \param & : √Ë ˆ
 * \param dwTextFormat : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::GetStringRect2(const char* lpszString, size_t count,RECT & layoutRect,RECT & totalRect,DWORD dwTextFormat)
{
	GetD3DTextout()->GetStringRect2(lpszString,count,layoutRect,totalRect,dwTextFormat);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param char* : √Ë ˆ
 * \param count : √Ë ˆ
 * \param RECT : √Ë ˆ
 * \param clrText : √Ë ˆ
 * \param dwTextFormat : √Ë ˆ
 * \param fontEffect : √Ë ˆ
 * \param clrShadow : √Ë ˆ
 * \param pData : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::DrawString(const char* lpszString , size_t count,const RECT & layoutRect ,DWORD clrText,DWORD dwTextFormat,enumFontEffect fontEffect,DWORD clrShadow,DWORD* pData)
{
	GetD3DTextout()->DrawString(lpszString , count,layoutRect, clrText, dwTextFormat, fontEffect, clrShadow,pData);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param WCHAR* : √Ë ˆ
 * \param count : √Ë ˆ
 * \param RECT : √Ë ˆ
 * \param clrText : √Ë ˆ
 * \param dwTextFormat : √Ë ˆ
 * \param fontEffect : √Ë ˆ
 * \param clrShadow : √Ë ˆ
 * \param pData : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::DrawString(const WCHAR* lpszString , size_t count,const RECT & layoutRect ,DWORD clrText,DWORD dwTextFormat,enumFontEffect fontEffect,DWORD clrShadow,DWORD* pData)
{
	GetD3DTextout()->DrawString(lpszString , count,layoutRect, clrText, dwTextFormat, fontEffect, clrShadow,pData);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param n : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
int	CD3DDevice::SetFontHSpace(int n)
{
	return GetD3DTextout()->SetHSpace(n);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param n : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
int	CD3DDevice::SetFontVSpace(int n)
{
	return GetD3DTextout()->SetVSpace(n);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param n : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
int	CD3DDevice::SetFontTabSize(int n)
{
	return GetD3DTextout()->SetTabSize(n);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param iFont : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
int CD3DDevice::GetFontHeight(int iFont)
{
	return (int)GetD3DTextout()->GetFontHeight(iFont);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param iFont : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
int CD3DDevice::GetFontWidth(int iFont)
{
	return (int)GetD3DTextout()->GetFontWidth(iFont);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param iFont : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
int CD3DDevice::GetFontWcharWidth(int iFont)
{
	return (int)GetD3DTextout()->GetFontWcharWidth(iFont);
}




/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param type : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::BeginFlush(FlushType type)
{
	FinalFlush();
	m_FlushType = type;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::FinalFlush()
{
	switch(m_FlushType)
	{
	case Flush_Cover:
		{
			if(!m_arrayRenderInfo.array.empty())
			{
				RenderTriangleList(m_arrayRenderInfo.pTex,&m_arrayRenderInfo.array[0],m_arrayRenderInfo.array.size(),m_arrayRenderInfo.blend);
				m_arrayRenderInfo.array.resize(0);
			}
		}
		break;

	case Flush_NoCover:

		if(!m_mapRenderInfo.empty())
		{
			for(tMapRenderInfoIt it = m_mapRenderInfo.begin(); it != m_mapRenderInfo.end(); ++it)
			{
				RenderTriangleList((IDirect3DTexture8*)it->first,&it->second->array[0],it->second->array.size(),it->second->blend);
				m_allocRenderInfos.freePointer(it->second);
			}
			m_mapRenderInfo.clear();
		}

		if(!m_mapMultiTexRenderInfo.empty())
		{
			for(tMapMultiTexRenderInfoIt it = m_mapMultiTexRenderInfo.begin(); it != m_mapMultiTexRenderInfo.end(); ++it)
			{
				IDirect3DTexture8* tex,*tex2;
				GetRenderKeyTex(it->first,tex,tex2);
				RenderTriangleList(tex,tex2,&it->second->array[0],it->second->array.size(),it->second->blend);
				m_allocMultiTexRenderInfos.freePointer(it->second);
			}
			m_mapMultiTexRenderInfo.clear();
		}

		break;
	}
	GetDevice()->SetTexture(0,NULL);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::EndFlush()
{
	FinalFlush();
	m_FlushType = Flush_Cover;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
int CD3DDevice::GetTriCount()
{
	return m_frameTriCount;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
IDirect3DSurface8* CD3DDevice::GetBackSurface()
{
	IDirect3DSurface8* pSur;
	HRESULT hr;
	hr = GetDevice()->GetRenderTarget(&pSur);
	if(SUCCEEDED(hr))
	{
		return pSur;
	}
	hr = GetDevice()->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&pSur);
	if(SUCCEEDED(hr))
	{
		return pSur;
	}
	return NULL;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
D3DFORMAT CD3DDevice::GetBackFormat()
{
	D3DDISPLAYMODE dispalyMode;
	HRESULT hr;
	hr = GetDevice()->GetDisplayMode(&dispalyMode);
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return D3DFMT_UNKNOWN;
	}
	return dispalyMode.Format;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
IDirect3DSurface8* CD3DDevice::GetRenderTarget()
{
	IDirect3DSurface8* pSurface;
	HRESULT hr;
	hr = GetDevice()->GetRenderTarget(&pSurface);
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		hr = GetDevice()->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&pSurface);
		if(FAILED(hr))
		{
			TRACE_DXERROR(hr);
			return NULL;
		}
	}
	return pSurface;
}



/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
size_t CD3DDevice::GetTotalTextureMemSize()
{
	return GetTextureManager()->GetTotalTextureMemSize();	
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
size_t CD3DDevice::GetUseTextureMemSize()  
{ 
	return GetTextureManager()->GetUseTextureMemSize();	
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
size_t CD3DDevice::GetUseResMemorySize()
{
	return CD3DBitmapList::s_dwMemorySize;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param dwWidth : √Ë ˆ
 * \param dwHeight : √Ë ˆ
 * \param dwPixelBits : √Ë ˆ
 * \param dwFlags : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DDevice::Reset(DWORD dwWidth,DWORD dwHeight,DWORD dwPixelBits,DWORD dwFlags)
{
	HRESULT hr;
	/*if (Engine_GetCallBack())
	{
		Engine_GetCallBack()->InvalidateDeviceObjects();
	}*/

	InvalidateDeviceObjects();

	m_iWidth = dwWidth;
	m_iHeight = dwHeight;
	m_dwFlags = dwFlags;
	m_iPixelBits = dwPixelBits;

	bool isFullScreen = false;
	if((m_dwFlags & DEVICE_FULLSCREEN) != 0)
	{
		isFullScreen = true;
	}
	
	if( FAILED(hr = _Restore(isFullScreen)))
	{
		TRACE("Device reset failed.\n\n");
		return false;
	}

	TRACE("Device reset success.\n");

	CheckTexFormat();

	ResetDevice();


	SetupMatrix();

	SetupStates();

	EnableLightMap( (dwFlags & DEVICE_LIGHTMAP) != 0 );
	
	if (Engine_GetCallBack())
	{
		//Engine_GetCallBack()->RestoreDeviceObjects();
		Engine_GetCallBack()->OnDisplayModeChange();
	}

	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param char* : √Ë ˆ
 * \param count : √Ë ˆ
 * \param maxWidth : √Ë ˆ
 * \param bSingleLine : √Ë ˆ
 * \param fontEffect : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
SIZE CD3DDevice::GetStringSize(const char* lpszString, size_t count,size_t maxWidth,bool bSingleLine,enumFontEffect fontEffect)
{
	return GetD3DTextout()->GetStringSize(lpszString,count,maxWidth,bSingleLine,fontEffect);
}

IDirect3DTexture8 * CD3DDevice::CreateMemTex(D3DFORMAT format,int width,int height)
{
	IDirect3DTexture8* pTex;
	HRESULT hr = m_pD3DD->CreateTexture(width,height,1,0,format,D3DPOOL_SYSTEMMEM,&pTex);
	if(FAILED(hr)) 
	{
		TRACE_DXERROR(hr);
		return NULL;
	}
	return pTex;
}

IDirect3DSurface8 * CD3DDevice::CreateMemSurface(D3DFORMAT format, int width,int height)
{
	IDirect3DSurface8* pSur;
	HRESULT hr = m_pD3DD->CreateImageSurface(width,height,format,&pSur);
	if(FAILED(hr)) 
	{
		TRACE_DXERROR(hr);
		return NULL;
	}
	return pSur;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param format : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
IDirect3DTexture8 * CD3DDevice::GetTmpTexture(D3DFORMAT format)
{
	switch(format)
	{
	case D3DFMT_A4R4G4B4:
		return m_tmptextures[eTmpTex4444].getNewTex();
	case D3DFMT_R5G6B5:
		return m_tmptextures[eTmpTex565].getNewTex();
	case D3DFMT_DXT1:
		return m_tmptextures[eTmpTexDXT1].getNewTex();
	case D3DFMT_DXT3:
		return m_tmptextures[eTmpTexDXT3].getNewTex();
	case D3DFMT_A1R5G5B5:
		return m_tmptextures[eTmpTex5551].getNewTex();
	case D3DFMT_A8R8G8B8:
		return m_tmptextures[eTmpTexA8R8G8B8].getNewTex();
		break;
	case D3DFMT_X8R8G8B8:
		return m_tmptextures[eTmpTexX8R8G8B8].getNewTex();
		break;
	case D3DFMT_A8:
		return m_tmptextures[eTmpTexA8].getNewTex();
		break;

	}
	return NULL;
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param format : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
IDirect3DSurface8 * CD3DDevice::GetTmpSurface(D3DFORMAT format)
{
	switch(format)
	{
	case D3DFMT_A4R4G4B4:
		if(m_tmpsurfaces[eTmpTex4444] == NULL)
		{
			m_pD3DD->CreateImageSurface(256,256,D3DFMT_A4R4G4B4,&m_tmpsurfaces[eTmpTex4444]);
		}
		return m_tmpsurfaces[eTmpTex4444];
	case D3DFMT_R5G6B5:
		if(m_tmpsurfaces[eTmpTex565] == NULL)
		{
			m_pD3DD->CreateImageSurface(256,256,D3DFMT_R5G6B5,&m_tmpsurfaces[eTmpTex565]);
		}
		return m_tmpsurfaces[eTmpTex565];
	case D3DFMT_DXT1:
		if(m_tmpsurfaces[eTmpTexDXT1] == NULL)
		{
			m_pD3DD->CreateImageSurface(256,256,D3DFMT_DXT1,&m_tmpsurfaces[eTmpTexDXT1]);
		}
		return m_tmpsurfaces[eTmpTexDXT1];
	case D3DFMT_DXT3:
		if(m_tmpsurfaces[eTmpTexDXT3] == NULL)
		{
			m_pD3DD->CreateImageSurface(256,256,D3DFMT_DXT3,&m_tmpsurfaces[eTmpTexDXT3]);
		}
		return m_tmpsurfaces[eTmpTexDXT3];
	case D3DFMT_A1R5G5B5:
		if(m_tmpsurfaces[eTmpTex5551] == NULL)
		{
			m_pD3DD->CreateImageSurface(256,256,D3DFMT_A1R5G5B5,&m_tmpsurfaces[eTmpTex5551]);
		}
		return m_tmpsurfaces[eTmpTex5551];
	case D3DFMT_A8R8G8B8:
		if(m_tmpsurfaces[eTmpTexA8R8G8B8] == NULL)
		{
			m_pD3DD->CreateImageSurface(256,256,D3DFMT_A8R8G8B8,&m_tmpsurfaces[eTmpTexA8R8G8B8]);
		}
		return m_tmpsurfaces[eTmpTexA8R8G8B8];
		break;
	case D3DFMT_X8R8G8B8:
		if(m_tmpsurfaces[eTmpTexX8R8G8B8] == NULL)
		{
			m_pD3DD->CreateImageSurface(256,256,D3DFMT_X8R8G8B8,&m_tmpsurfaces[eTmpTexX8R8G8B8]);
		}
		return m_tmpsurfaces[eTmpTexX8R8G8B8];
		break;
	case D3DFMT_A8:
		if(m_tmpsurfaces[eTmpTexA8] == NULL)
		{
			m_pD3DD->CreateImageSurface(256,256,D3DFMT_A8,&m_tmpsurfaces[eTmpTexA8]);
		}
		return m_tmpsurfaces[eTmpTexA8];
		break;

	}
	return NULL;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param e : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
D3DFORMAT CD3DDevice::GetSrcFormat(enumPixelFormat e)
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
		return m_bUsableDXT1 ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;
		//return D3DFMT_R5G6B5;
	}
	return m_bUsableDXT3 ? D3DFMT_A8R8G8B8 : D3DFMT_A4R4G4B4;
	return D3DFMT_A4R4G4B4;
}



/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param cx : √Ë ˆ
 * \param cy : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::SetFontScale(float cx,float cy)
{
	GetD3DTextout()->SetScale(cx,cy);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
stPointF CD3DDevice::GetFontScale()
{
	return GetD3DTextout()->GetScale();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param width : √Ë ˆ
 * \param height : √Ë ˆ
 * \param formt : √Ë ˆ
 * \param usage : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
CCustomTexture* CD3DDevice::CreateCustomTexture(int width,int height,D3DFORMAT formt,DWORD usage )
{
	return GetTextureManager()->CreateCurstomTexture(width,height,formt,usage);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param ppPicCharInfo : √Ë ˆ
 * \param * : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::GetPicCharInfo( stPicCharInfo** ppPicCharInfo,size_t * count)
{
	return GetD3DTextout()->GetPicCharInfo(ppPicCharInfo,count);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param p : √Ë ˆ
 * \param nPicCharInfoCount : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::SetPicCharInfo( stPicCharInfo* p , size_t nPicCharInfoCount )
{
	GetD3DTextout()->SetPicCharInfo(p,nPicCharInfoCount);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param ch : √Ë ˆ
 * \param iFont : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
int CD3DDevice::GetAFontWidth( int ch,int iFont )
{
	return (int)GetD3DTextout()->GetAFontWidth(ch,iFont);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param ichar : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
stPicCharInfo* CD3DDevice::GetPicCharInfo(int ichar)
{
	return GetD3DTextout()->GetPicCharInfo(ichar);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pMat : √Ë ˆ
 * \param * : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::SetMatrix(D3DXMATRIX* pMat,D3DXMATRIX * pOldMat )
{
	if(pOldMat) memcpy(pOldMat,&m_Transform,sizeof(D3DXMATRIX));
	m_Transform = *pMat;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param * : √Ë ˆ
 * \param fAngle : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::SetRotation(D3DXVECTOR3 * pV,FLOAT fAngle)
{
	D3DXMATRIXA16 mr,mt2;

	D3DXMatrixTranslation(&m_Transform,-pV->x,-pV->y,-pV->z);

	D3DXMatrixRotationZ(&mr,fAngle);
	m_bTransform = !D3DXMatrixIsIdentity(&mr);

	D3DXMatrixTranslation(&mt2,pV->x,pV->y,pV->z);

	D3DXMatrixMultiply(&m_Transform,&m_Transform,&mr);
	D3DXMatrixMultiply(&m_Transform,&m_Transform,&mt2);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pt : √Ë ˆ
 * \param fAngle : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::SetRotation(POINT pt,FLOAT fAngle)
{
	D3DXVECTOR3 vec((float)pt.x,(float)pt.y,0.0f);
	SetRotation(&vec,fAngle);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::IdentityTransform()
{
	D3DXMatrixIdentity(&m_Transform);
	m_bTransform = false;
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param type : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool	CD3DDevice::IsAllowBlendType(BlendType type)
{
	switch(type)
	{
	case Blend_Null:
		return (m_caps.SrcBlendCaps & D3DPBLENDCAPS_SRCALPHA) != 0 && (m_caps.DestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA ) != 0;
	case Blend_Add:
		return (m_caps.SrcBlendCaps & D3DPBLENDCAPS_ONE) != 0 && (m_caps.DestBlendCaps & D3DPBLENDCAPS_INVSRCCOLOR ) != 0;
		break;
	case Blend_Add2:
		return (m_caps.SrcBlendCaps & D3DPBLENDCAPS_ONE) != 0 && (m_caps.DestBlendCaps & D3DPBLENDCAPS_ONE ) != 0;
		break;
	case Blend_AlphaAdd:
		return (m_caps.SrcBlendCaps & D3DPBLENDCAPS_SRCALPHA) != 0 && (m_caps.DestBlendCaps & D3DPBLENDCAPS_ONE ) != 0;
		break;
	case Blend_AlphaAdd2:
		return (m_caps.SrcBlendCaps & D3DPBLENDCAPS_ONE) != 0 && (m_caps.DestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA ) != 0;
		break;
	case Blend_AlphaAdd3:
		return (m_caps.SrcBlendCaps & D3DPBLENDCAPS_SRCCOLOR) != 0 && (m_caps.DestBlendCaps & D3DPBLENDCAPS_INVSRCCOLOR ) != 0;
		break;
	case Blend_MulAlpha:
		return (m_caps.SrcBlendCaps & D3DPBLENDCAPS_ZERO) != 0 && (m_caps.DestBlendCaps & D3DPBLENDCAPS_SRCALPHA ) != 0;
		break;
	case Blend_MulInvAlpha:
		return (m_caps.SrcBlendCaps & D3DPBLENDCAPS_ZERO) != 0 && (m_caps.DestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA ) != 0;
		break;
	case Blend_MulInvColor:
		return (m_caps.SrcBlendCaps & D3DPBLENDCAPS_ZERO) != 0 && (m_caps.DestBlendCaps & D3DPBLENDCAPS_INVSRCCOLOR) != 0;
		break;
	case Blend_Bright:
		return (m_caps.TextureOpCaps & D3DTEXOPCAPS_MODULATE2X) != 0;
		break;
	case Blend_IgnoreTexelColor:	// ≤ªº∆À„Ã˘Õºµƒ—’…´
		return (m_caps.TextureOpCaps & D3DTEXOPCAPS_SELECTARG2) != 0;
		break;
	case Blend_ColorAdd:
		return (m_caps.TextureOpCaps & D3DTEXOPCAPS_ADD) != 0;
		break;
	case Blend_AlphaAdd4:
		return (m_caps.TextureOpCaps & D3DTEXOPCAPS_ADD) != 0;
		break;
	case Blend_AlphaSub:
		return (m_caps.TextureOpCaps & D3DTEXOPCAPS_SUBTRACT) != 0;
		break;
	case Blend_Monochrome:	// ª“∂»Õº
		return (m_caps.TextureOpCaps & D3DTEXOPCAPS_DOTPRODUCT3) != 0;
	case Blend_Add3:
		return (m_caps.SrcBlendCaps & D3DPBLENDCAPS_ONE) != 0 && (m_caps.DestBlendCaps & D3DPBLENDCAPS_SRCCOLOR) != 0;
	}
	return false;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CD3DDevice::IsLoadQueueLowerEmpty()
{
	return CD3DBitmapList::s_bQueueLowerEmpty;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::LockResLoad()
{
	CD3DBitmapList::LockResLoad();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CD3DDevice::UnlockResLoad()
{
	CD3DBitmapList::UnlockResLoad();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pDestFile : √Ë ˆ
 * \param DestFormat : √Ë ˆ
 * \param RECT* : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
HRESULT CD3DDevice::SaveBackSurfaceToFile(LPCTSTR  pDestFile,D3DXIMAGE_FILEFORMAT DestFormat,CONST RECT* pSrcRect) 
{
	IDirect3DSurface8* pSur;
	HRESULT hr;
	hr = GetDevice()->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&pSur);
	if(SUCCEEDED(hr))
	{
		hr=D3DXSaveSurfaceToFile(pDestFile,DestFormat,pSur,NULL,pSrcRect);
		pSur->Release();
		return hr;
	}
	return hr;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param iFont : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
int CD3DDevice::SetFont(int iFont) 
{
	return GetD3DTextout()->SetFont(iFont);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param iFont : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
int CD3DDevice::IsValidFont(int iFont)
{
	return iFont >=0 && iFont < CD3DTextout::Font_Count;
}

bool CD3DDevice::SetScreenPixelBits(int bits)
{

	DEVMODE modReg;
	memset(&modReg,0,sizeof(modReg));
	modReg.dmSize = sizeof(modReg);
	EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&modReg);
	if(modReg.dmBitsPerPel == bits) return true;

	modReg.dmBitsPerPel = bits;
	modReg.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT ;

	DWORD dw = ::ChangeDisplaySettings(&modReg,CDS_FULLSCREEN);
	return (DISP_CHANGE_SUCCESSFUL == dw);
}

void CD3DDevice::ResetScreenPixelBits()
{
	::ChangeDisplaySettings(NULL,0);
}

void CD3DDevice::stTempTexture::init(D3DFORMAT i_format,size_t i_num)
{
	format = i_format;
	num = i_num;
	iRun = -1;
	pTex = new IDirect3DTexture8*[num];
	memset(pTex,0,sizeof(IDirect3DTexture8*)*num);
}

IDirect3DTexture8* CD3DDevice::stTempTexture::getNewTex()
{
	iRun++;
	if(iRun == num){
		iRun = 0;
	}
	if(pTex[iRun] == NULL){
		HRESULT hr = GetD3DDevice()->GetDevice()->CreateTexture(256,256,1,0,format,D3DPOOL_SYSTEMMEM,&pTex[iRun]);
		if(FAILED(hr)){
			iRun--;
			if(iRun == -1) iRun = num - 1;
			return NULL;
		}
	}
	return pTex[iRun];
}

void CD3DDevice::stTempTexture::release()
{
	if(pTex){
		for(size_t i=0; i<count_of(pTex);++i){
			SAFE_RELEASE(pTex[i]);
		}
	}
	iRun = -1;
}

bool CD3DDevice::IsSupportFlag(DWORD flags)
{
	switch(flags){
	case DEVICE_MULTITEX:
		return GetMaxTextureBlendStages() > 1 && GetMaxSimultaneousTextures() > 1;
		break;
	case DEVICE_DXT:
		return CheckTexFormat(D3DFMT_DXT3);
		break;
	default:
		Assert(0);
		return true;
	}
}

void CD3DDevice::UnmapAllBitmapData()
{
	CD3DBitmapList::ClearLoadQueue();
	for( t_bitmaplistsit it = m_bitmaplists.begin(); it != m_bitmaplists.end(); ++it )
	{
		it->second->UnmapAllBitmapData();
	}
}

bool CD3DDevice::IsSupportHardwareCursor()
{
	return (0 != (m_caps.CursorCaps & D3DCURSORCAPS_COLOR)) || (0 != (m_caps.CursorCaps & D3DCURSORCAPS_LOWRES ));
}