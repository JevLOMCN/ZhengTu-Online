#include "../engine/include/engine.h"
#include ".\movie.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
#ifndef CHECK_HR
#define CHECK_HR(expr) { if (FAILED(expr)) __leave; };
#endif

const DWORD bits555[] = {0x007C00,0x0003E0,0x00001F};
const DWORD bits565[] = {0x00F800,0x0007E0,0x00001F};
const DWORD bits888[] = {0xFF0000,0x00FF00,0x0000FF};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dd : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
template <typename T>
inline void INITDDSTRUCT(T& dd)
{
	ZeroMemory(&dd, sizeof(dd));
	dd.dwSize = sizeof(dd);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CMovie::CMovie(void)
: m_pDDTexture(NULL)
, m_pDDDecode(NULL)
, m_dwFrameCount(0)
, m_bPresented(false)
, m_bDirectedFlips(false)
{
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CMovie::~CMovie(void)
{
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CMovie::Init()
{
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
RECT CMovie::GetRect()
{
	return *GetDevice()->GetRenderRect();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CMovieAllocator::CMovieAllocator(CMovie* pMovie,HWND hwndApplication, HRESULT *phr)\
: m_lpDDObj(NULL)
, m_pMovie(pMovie)
, m_pWC(NULL)
, m_pAlloc(NULL)
, m_pPresenter(NULL)
, m_lpBackBuffer(NULL)
, m_hwndApp(hwndApplication)
, m_bErrorMessage(false)
, m_dwFrameNum(0)
, m_hMonitor(NULL)
{
	*phr = S_OK;
	
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CMovieAllocator::~CMovieAllocator()
{
	SAFE_RELEASE(m_lpDDObj);    
	SAFE_RELEASE(m_pWC);
	SAFE_RELEASE(m_pPresenter);
	SAFE_RELEASE(m_lpBackBuffer);

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
	if (m_pAlloc) 
	{
		m_pAlloc->FreeSurface(0);
		m_pAlloc = NULL;
	}   
}
//-------------------------------------------------------------------------
//  Initialize
//  configures D3D environment and creates composing thread
//-------------------------------------------------------------------------
HRESULT CMovieAllocator::Initialize()
{
	HRESULT hr;
	hr = InitializeEnvironment();
	if (SUCCEEDED(hr)) 
	{   
		//m_hThread = CreateThread(NULL, 0, ComposeThreadProc, this, 0, &m_dwThreadID);
		//if (!m_hThread)
		//	hr = E_FAIL;
	}

	return hr;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//-------------------------------------------------------------------------
//  CreateDefaultAllocatorPresenter
//
//  Why do we need default AP? In this sample, we use the same DirectDrawObject
//  for each VMR invovled in presenation. 
//-------------------------------------------------------------------------
HRESULT
CMovieAllocator::CreateDefaultAllocatorPresenter()
{
	HRESULT hr = S_OK;
	IVMRMonitorConfig * pMonConf = NULL;

	__try 
	{
		CHECK_HR(hr = CoCreateInstance(CLSID_AllocPresenter, NULL,
			CLSCTX_INPROC_SERVER,
			IID_IVMRSurfaceAllocator,
			(LPVOID*)&m_pAlloc));

		CHECK_HR(hr = m_pAlloc->QueryInterface(IID_IVMRImagePresenter,
			(LPVOID*)&m_pPresenter));

		CHECK_HR(hr = m_pAlloc->QueryInterface(IID_IVMRWindowlessControl,
			(LPVOID*)&m_pWC));

		// Important! At this point, we advised out custom window AND now we
		// have access to DDrawObject related to it.

		CHECK_HR(hr = m_pWC->SetVideoClippingWindow(m_hwndApp));

		CHECK_HR(hr = m_pAlloc->QueryInterface(IID_IVMRMonitorConfig,
			(LPVOID*)&pMonConf));
		VMRMONITORINFO vmrMonInf;
		ZeroMemory( &vmrMonInf, sizeof(VMRMONITORINFO));
		DWORD nDevices = 0;

		CHECK_HR(hr = pMonConf->GetAvailableMonitors( &vmrMonInf, 1, &nDevices));
		m_hMonitor = vmrMonInf.hMon;
	}
	__finally 
	{        
		if (FAILED(hr)) 
		{
			SAFE_RELEASE( pMonConf );
			SAFE_RELEASE(m_pWC)
			SAFE_RELEASE(m_pPresenter);
			SAFE_RELEASE(m_pAlloc);
		}
	}

	SAFE_RELEASE( pMonConf );

	return hr;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//-------------------------------------------------------------------------
//  InitializeEnvironment
//  creates default allocator-presenter and sets D3D environment
//-------------------------------------------------------------------------
HRESULT CMovieAllocator::InitializeEnvironment()
{
	HRESULT hr;
	//    m_hMonitor = MonitorFromWindow(m_hwndApp, MONITOR_DEFAULTTOPRIMARY);

	hr = CreateDefaultAllocatorPresenter();
	if (hr != S_OK)
		return hr;

	BITMAPINFOHEADER  bi = 
	{
		sizeof(BITMAPINFOHEADER), // biSize
			640,                      // biWidth
			480,                      // biHeight
			1,                        // biPlanes
			0,                        // biBitCount
			BI_RGB,                   // biCompression
			0,                        // biSizeImage,
			0,                        // biXpelsPerMeter
			0,                        // biYPelsPerMeter
			0,                        // biClrUsed
			0                         // biClrImportant
	};
	VMRALLOCATIONINFO ai = 
	{
		AMAP_3D_TARGET,             // dwFlags
			&bi,                        // lpHdr
			NULL,                       // lpPicFmt
		{4, 3},                     // szAspectRatio
		1,                          // dwMinBuffers
		1,                          // dwMaxBuffers
		0,                          // dwInterlaceFlags
		{640, 480}                  // szNativeSize
	};

	DWORD dwBuffers = 0;
	LPDIRECTDRAWSURFACE7 lpDDSurf;
	hr = m_pAlloc->AllocateSurface(0, &ai, &dwBuffers, &lpDDSurf);
	if (hr != DD_OK)
		return hr;

	DDSURFACEDESC2 ddsd = {sizeof(DDSURFACEDESC2)};
	hr = lpDDSurf->GetSurfaceDesc(&ddsd);
	if (hr != DD_OK) {
		return hr;
	}

	//
	// Overlay surfaces have these flags set, we need to remove
	// these flags prior to calling GetAttachedSurface
	//
	ddsd.ddsCaps.dwCaps &= ~(DDSCAPS_FRONTBUFFER | DDSCAPS_VISIBLE);

	hr = lpDDSurf->GetAttachedSurface(&ddsd.ddsCaps, &m_lpBackBuffer);


	m_lpBackBuffer->GetDDInterface((LPVOID *)&m_lpDDObj);

	//
	// get the h/w caps for this device
	//
	INITDDSTRUCT(m_ddHWCaps);
	m_lpDDObj->GetCaps(&m_ddHWCaps, NULL); 

	return hr;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pdwUserID : ÃèÊö
 * \param w : ÃèÊö
 * \param lpdwBuffer : ÃèÊö
 * \param lplpSurface						 : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
STDMETHODIMP
CMovieAllocator::AllocateSurface(
						   DWORD_PTR pdwUserID,
						   VMRALLOCATIONINFO *w,
						   DWORD* lpdwBuffer,
						   LPDIRECTDRAWSURFACE7* lplpSurface
						   )
{
	// before doing anything, let's check if pdwUserID is valid

	HRESULT hr = S_OK;

	if (!w) 
	{
		return E_POINTER;
	}

	DWORD dwFlags               = w->dwFlags;
	LPBITMAPINFOHEADER lpHdr    = w->lpHdr;
	LPDDPIXELFORMAT lpPixFmt    = w->lpPixFmt;
	LPSIZE lpAspectRatio        = &w->szAspectRatio;
	DWORD dwMinBuffers          = w->dwMinBuffers;
	DWORD dwMaxBuffers          = w->dwMaxBuffers;
	VIDEOINFO vi;
	DDSURFACEDESC2 ddsdDisplay;

	if (!lpHdr) 
	{
		return E_POINTER;
	}

	const DWORD AMAP_INVALID_FLAGS = (DWORD) ~(AMAP_PIXELFORMAT_VALID | AMAP_3D_TARGET |
		AMAP_ALLOW_SYSMEM | AMAP_FORCE_SYSMEM |
		AMAP_DIRECTED_FLIP | AMAP_DXVA_TARGET);

	if (dwFlags & AMAP_INVALID_FLAGS) 
	{
		return E_INVALIDARG;
	}

	const DWORD AMAP_SYSMEM_FLAGS = (AMAP_ALLOW_SYSMEM | AMAP_FORCE_SYSMEM);
	if (AMAP_SYSMEM_FLAGS == (dwFlags & AMAP_SYSMEM_FLAGS)) 
	{
		return E_INVALIDARG;
	}

	if (!lpAspectRatio) 
	{
		return E_POINTER;
	}

	if (!lplpSurface) 
	{
		return E_POINTER;
	}

	*lplpSurface = NULL;

	if (!lpdwBuffer) 
	{
		return E_POINTER;
	}

	if (dwMinBuffers == 0 || dwMaxBuffers == 0) 
	{
		return E_INVALIDARG;
	}

	if (dwMinBuffers > dwMaxBuffers) 
	{
		return E_INVALIDARG;
	}

	if (dwMaxBuffers > 16) 
	{
		return E_INVALIDARG;
	}

	if (dwFlags & AMAP_PIXELFORMAT_VALID) 
	{
		if (!lpPixFmt) 
		{
			return E_POINTER;
		}
	}
	else 
	{
		lpPixFmt = NULL;
	}

	if (lpAspectRatio->cx < 1 || lpAspectRatio->cy < 1) 
	{
		return E_INVALIDARG;
	}

	if (dwFlags & AMAP_3D_TARGET) 
	{

		CopyMemory(&vi.bmiHeader, lpHdr, lpHdr->biSize);
		lpHdr = &vi.bmiHeader;

		if (dwFlags & (AMAP_FORCE_SYSMEM | AMAP_ALLOW_SYSMEM)) 
		{
			return E_INVALIDARG;
		}

		if (lpHdr->biCompression == BI_RGB && lpHdr->biBitCount == 0) 
		{
			ZeroMemory( &ddsdDisplay, sizeof(DDSURFACEDESC2));
			ddsdDisplay.dwSize = sizeof(DDSURFACEDESC2);

			m_lpDDObj->GetDisplayMode( &ddsdDisplay );

			lpHdr->biBitCount = (unsigned short)(ddsdDisplay.ddpfPixelFormat.dwRGBBitCount);
			lpHdr->biCompression = ddsdDisplay.ddpfPixelFormat.dwFourCC;

			/*
			if (lpHdr->biCompression == BI_BITFIELDS) 
			{
			ddsdDisplay.
			const DWORD *pMonMasks = NULL;
			// GetBitMasks(&m_lpCurrMon->DispInfo.bmiHeader);

			DWORD *pBitMasks = (DWORD *)((LPBYTE)lpHdr + lpHdr->biSize);
			pBitMasks[0] = pMonMasks[0];
			pBitMasks[1] = pMonMasks[1];
			pBitMasks[2] = pMonMasks[2];
			}
			*/
		}
	}

	hr = AllocateSurfaceWorker(
		dwFlags, 
		lpHdr, 
		lpPixFmt, 
		lpAspectRatio,
		dwMinBuffers, 
		dwMaxBuffers,
		lpdwBuffer, 
		lplpSurface,
		&ddsdDisplay);

	if( SUCCEEDED(hr))
	{
		
		DDSURFACEDESC2 ddsdTexture;
		ZeroMemory( &ddsdTexture, sizeof(DDSURFACEDESC2));
		ddsdTexture.dwSize = sizeof(DDSURFACEDESC2);

		ddsdTexture.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;


		// Adjust width and height, if the driver requires it
		/*if (m_pD3DHelper->IsPower2()) 
		{

			for (ddsdTexture.dwWidth = 1; 
				(DWORD)abs(lpHdr->biWidth) > ddsdTexture.dwWidth;
				ddsdTexture.dwWidth <<= 1);

			for (ddsdTexture.dwHeight = 1; 
				(DWORD)abs(lpHdr->biHeight) > ddsdTexture.dwHeight;
				ddsdTexture.dwHeight <<= 1);        
		}
		else*/
		{
			ddsdTexture.dwWidth = (DWORD)abs(lpHdr->biWidth);
			ddsdTexture.dwHeight = (DWORD)abs(lpHdr->biHeight);
		}

		ddsdTexture.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE; 

		ddsdTexture.ddpfPixelFormat.dwFlags    = DDPF_RGB;
		ddsdTexture.ddpfPixelFormat.dwFourCC   = 0;
		ddsdTexture.ddpfPixelFormat.dwRGBBitCount = 32;
		ddsdTexture.ddpfPixelFormat.dwRBitMask = 0xff0000;
		ddsdTexture.ddpfPixelFormat.dwGBitMask = 0x00ff00;
		ddsdTexture.ddpfPixelFormat.dwBBitMask = 0x0000ff;

		hr = m_lpDDObj->CreateSurface(&ddsdTexture, &(m_pMovie->m_pDDTexture), NULL);
		if( FAILED(hr))
		{
			m_bErrorMessage = true;
			SAFE_RELEASE(m_pMovie->m_pDDDecode);
		}
		else
		{
			m_pMovie->m_VideoAR = *lpAspectRatio;
			m_pMovie->m_VideoSize.cx = abs(lpHdr->biWidth);
			m_pMovie->m_VideoSize.cy = abs(lpHdr->biHeight);

			DDBLTFX ddFX;
			INITDDSTRUCT(ddFX);
			m_pMovie->m_pDDTexture->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &ddFX);

			//*lplpSurface = m_pMovie->m_pDDDecode;
			//*lpdwBuffer = dwMaxBuffers;       
			m_pMovie->m_bDirectedFlips = (AMAP_DIRECTED_FLIP == (dwFlags & AMAP_DIRECTED_FLIP));
		}
	}

	return hr;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dwFlags : ÃèÊö
 * \param lpHdr : ÃèÊö
 * \param lpPixFmt : ÃèÊö
 * \param lpAspectRatio : ÃèÊö
 * \param dwMinBackBuffers : ÃèÊö
 * \param dwMaxBackBuffers : ÃèÊö
 * \param lpdwBackBuffer : ÃèÊö
 * \param lplpSurface : ÃèÊö
 * \param pddsdDisplay : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT
CMovieAllocator::AllocateSurfaceWorker(
								 DWORD dwFlags,
								 LPBITMAPINFOHEADER lpHdr,
								 LPDDPIXELFORMAT lpPixFmt,
								 LPSIZE lpAspectRatio,
								 DWORD dwMinBackBuffers,
								 DWORD dwMaxBackBuffers,
								 DWORD* lpdwBackBuffer,
								 LPDIRECTDRAWSURFACE7* lplpSurface,
								 DDSURFACEDESC2* pddsdDisplay    )
{

	if (!lpHdr) 
	{
		return E_INVALIDARG;
	}

	if( !pddsdDisplay )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = E_FAIL;
	LPDIRECTDRAWSURFACE7 lpSurface7 = NULL;

	DDSURFACEDESC2 ddsd;
	INITDDSTRUCT(ddsd);

	DDCAPS_DX7 hwCaps;
	ZeroMemory( &hwCaps, sizeof(DDCAPS_DX7));
	hwCaps.dwSize = sizeof(DDCAPS_DX7);
	m_lpDDObj->GetCaps( &hwCaps, NULL );

	bool bCanBltFourCCSysMem = false;
	BOOL bStretchCapsOk = TRUE;

	if( hwCaps.dwSVBCaps & DDCAPS_BLTFOURCC )
	{
		if (hwCaps.dwSVBCaps & DDCAPS_BLTSTRETCH)
		{
			const DWORD caps = DDFXCAPS_BLTSHRINKX | 
				DDFXCAPS_BLTSHRINKX  |
				DDFXCAPS_BLTSTRETCHX | 
				DDFXCAPS_BLTSTRETCHY;
			if( (hwCaps.dwSVBFXCaps & caps) == caps )
			{
				bCanBltFourCCSysMem = true;
			}
		}
	}


	ddsd.dwWidth = abs(lpHdr->biWidth);
	ddsd.dwHeight = abs(lpHdr->biHeight);

	if (lpPixFmt) 
	{
		ddsd.ddpfPixelFormat = *lpPixFmt;
	}
	else 
	{
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

		if (lpHdr->biCompression <= BI_BITFIELDS &&
			pddsdDisplay->ddpfPixelFormat.dwRGBBitCount <= lpHdr->biBitCount)
		{
			ddsd.ddpfPixelFormat.dwFourCC = BI_RGB;
			ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
			ddsd.ddpfPixelFormat.dwRGBBitCount = lpHdr->biBitCount;

			if (dwFlags & AMAP_3D_TARGET) 
			{
				ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE;
			}
			// Store the masks in the DDSURFACEDESC
			const DWORD *pBitMasks = NULL;
			if( lpHdr->biCompression == BI_RGB )
			{
				if( 15 == lpHdr->biBitCount )
					pBitMasks = bits555;
				else
					pBitMasks = bits888;
			}
			if( !pBitMasks )
				return E_FAIL;

			ddsd.ddpfPixelFormat.dwRBitMask = pBitMasks[0];
			ddsd.ddpfPixelFormat.dwGBitMask = pBitMasks[1];
			ddsd.ddpfPixelFormat.dwBBitMask = pBitMasks[2];
		}
		else if (lpHdr->biCompression > BI_BITFIELDS)
		{
			ddsd.ddpfPixelFormat.dwFourCC = lpHdr->biCompression;
			ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			ddsd.ddpfPixelFormat.dwYUVBitCount = lpHdr->biBitCount;
		}
		else
		{
			return E_FAIL;
		}
	}

	if (dwFlags & AMAP_FORCE_SYSMEM) 
	{

		if (lpHdr->biCompression > BI_BITFIELDS && (false == bCanBltFourCCSysMem) ) 
		{
			return VFW_E_DDRAW_CAPS_NOT_SUITABLE;
		}

		if (lpHdr->biCompression <= BI_BITFIELDS &&
			pddsdDisplay->ddpfPixelFormat.dwRGBBitCount != lpHdr->biBitCount) 
		{
			return DDERR_INCOMPATIBLEPRIMARY;
		}

		hr = AllocateOffscreenSurface(  &(m_pMovie->m_pDDDecode), 
			AMAP_FORCE_SYSMEM, 
			&ddsd,
			dwMinBackBuffers, 
			dwMaxBackBuffers,
			lpdwBackBuffer, 
			FALSE);
	}
	else 
	{
		// figure out if we can stretchBlt
		{
			DWORD caps = 0;
			const DWORD dwFXCaps =  DDFXCAPS_BLTSHRINKX | 
				DDFXCAPS_BLTSHRINKX  |
				DDFXCAPS_BLTSTRETCHX | 
				DDFXCAPS_BLTSTRETCHY;
			if( lpHdr->biCompression <= BI_BITFIELDS )
			{
				caps = DDCAPS_BLTSTRETCH;
			}
			else
			{
				caps = DDCAPS_BLTFOURCC | DDCAPS_BLTSTRETCH;
			}
			bStretchCapsOk &= ((caps & hwCaps.dwCaps) == caps);
			bStretchCapsOk &= ((dwFXCaps & hwCaps.dwFXCaps) == dwFXCaps);
		}


		hr = AllocateOverlaySurface(    &(m_pMovie->m_pDDDecode), 
			dwFlags, 
			&ddsd,
			dwMinBackBuffers, 
			dwMaxBackBuffers, 
			lpdwBackBuffer);
		if (hr != DD_OK) 
		{

			if (lpHdr->biCompression > BI_BITFIELDS) 
			{
				if( !bStretchCapsOk ) 
				{
					return VFW_E_DDRAW_CAPS_NOT_SUITABLE;
				}
			}
			else 
			{
				//LPBITMAPINFOHEADER lpMon = &m_lpCurrMon->DispInfo.bmiHeader;
				if (lpHdr->biBitCount != pddsdDisplay->ddpfPixelFormat.dwRGBBitCount) 
				{
					return DDERR_INCOMPATIBLEPRIMARY;
				}

				if (lpHdr->biCompression != pddsdDisplay->ddpfPixelFormat.dwFourCC) 
				{
					if (lpHdr->biBitCount != 32) 
					{
						return DDERR_INCOMPATIBLEPRIMARY;
					}
					else 
					{
						OutputDebugString( TEXT("RGB32 should have BI_BITFIELDS set") );
					}
				}
			}


			if( bStretchCapsOk ) 
			{
				hr = AllocateOffscreenSurface(  &(m_pMovie->m_pDDDecode), 
					dwFlags, 
					&ddsd,
					dwMinBackBuffers, 
					dwMaxBackBuffers,
					lpdwBackBuffer, 
					TRUE);
			}
			else 
			{
				hr = VFW_E_DDRAW_CAPS_NOT_SUITABLE;
			}
		}

		if ( (hr != DD_OK) && (dwFlags & AMAP_ALLOW_SYSMEM) ) 
		{
			if( lpHdr->biCompression <= BI_BITFIELDS &&
				pddsdDisplay->ddpfPixelFormat.dwRGBBitCount == lpHdr->biBitCount) 
			{

				hr = AllocateOffscreenSurface(  &(m_pMovie->m_pDDDecode), 
					AMAP_FORCE_SYSMEM, 
					&ddsd,
					dwMinBackBuffers, 
					dwMaxBackBuffers,
					lpdwBackBuffer, 
					TRUE);
			}
			else 
			{
				hr = VFW_E_DDRAW_CAPS_NOT_SUITABLE;
			}
		}
	}

	*lplpSurface = m_pMovie->m_pDDDecode;
	return hr;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param lplpSurf : ÃèÊö
 * \param dwFlags : ÃèÊö
 * \param pddsd : ÃèÊö
 * \param dwMinBuffers : ÃèÊö
 * \param dwMaxBuffers : ÃèÊö
 * \param lpdwBuffer								 : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT
CMovieAllocator::AllocateOverlaySurface(
								  LPDIRECTDRAWSURFACE7* lplpSurf,
								  DWORD dwFlags,
								  DDSURFACEDESC2* pddsd,
								  DWORD dwMinBuffers,
								  DWORD dwMaxBuffers,
								  DWORD* lpdwBuffer
								  )
{
	HRESULT hr = S_OK;
	LPDIRECTDRAWSURFACE7 lpSurface7 = NULL;

	pddsd->dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT |
		DDSD_PIXELFORMAT | DDSD_BACKBUFFERCOUNT;

	pddsd->ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM |
		DDSCAPS_OVERLAY | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

	if (dwFlags & AMAP_3D_TARGET) 
	{
		pddsd->ddsCaps.dwCaps |= DDSCAPS_3DDEVICE;
	}

	DWORD dwMinBuff = dwMinBuffers;
	DWORD dwMaxBuff = dwMaxBuffers;

	for(    DWORD dwTotalBufferCount =  dwMaxBuff;
		dwTotalBufferCount >= dwMinBuff; 
		dwTotalBufferCount--) 
	{
		// CleanUp stuff from the last loop
		SAFE_RELEASE(lpSurface7);

		pddsd->dwBackBufferCount = dwTotalBufferCount - 1;
		if(dwTotalBufferCount == 1) 
		{
			pddsd->dwFlags &= ~DDSD_BACKBUFFERCOUNT;
			pddsd->ddsCaps.dwCaps &= ~(DDSCAPS_FLIP | DDSCAPS_COMPLEX);
		}

		hr = m_lpDDObj->CreateSurface(pddsd, &lpSurface7, NULL);

		if (hr == DD_OK) 
		{

			// hr = CheckOverlayAvailable(lpSurface7);
			*lpdwBuffer = dwTotalBufferCount;
			break;
		}
	} // for 

	*lplpSurf = lpSurface7;
	return hr;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param lplpSurf : ÃèÊö
 * \param dwFlags : ÃèÊö
 * \param pddsd : ÃèÊö
 * \param dwMinBuffers : ÃèÊö
 * \param dwMaxBuffers : ÃèÊö
 * \param lpdwBuffer : ÃèÊö
 * \param fAllowBackBuffer									 : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT
CMovieAllocator::AllocateOffscreenSurface(
									LPDIRECTDRAWSURFACE7* lplpSurf,
									DWORD dwFlags,
									DDSURFACEDESC2* pddsd,
									DWORD dwMinBuffers,
									DWORD dwMaxBuffers,
									DWORD* lpdwBuffer,
									BOOL fAllowBackBuffer
									)
{
	HRESULT hr = S_OK;
	LPDIRECTDRAWSURFACE7 lpSurf7FB = NULL;
	DWORD dwTotalBufferCount = 0;


	*lpdwBuffer = 0;
	pddsd->dwBackBufferCount = 0;
	pddsd->dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;

	if (dwFlags & AMAP_FORCE_SYSMEM) 
	{
		pddsd->ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	}
	else 
	{
		pddsd->ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM |
			DDSCAPS_OFFSCREENPLAIN;
	}

	if (dwFlags & AMAP_3D_TARGET) 
	{
		pddsd->ddsCaps.dwCaps |= DDSCAPS_3DDEVICE;
	}

	hr = m_lpDDObj->CreateSurface(pddsd, &lpSurf7FB, NULL);

	if (hr != DD_OK) 
	{
		return hr;
	}

	DWORD dwMinBuff;
	DWORD dwMaxBuff;

	if (fAllowBackBuffer) 
	{
		dwMinBuff = dwMinBuffers + 1;
		dwMaxBuff = dwMaxBuffers + 1;

		if (dwMinBuffers <= 2) 
		{
			dwMinBuff = dwMinBuffers + 1;
		}

		if (dwMaxBuffers <= 2) 
		{
			dwMaxBuff = dwMaxBuffers + 1;
		}
	}
	else 
	{

		dwMinBuff = dwMinBuffers;
		dwMaxBuff = dwMaxBuffers;
	}

	dwTotalBufferCount = 1;

	__try {

		LPDIRECTDRAWSURFACE7 lpSurf7 = lpSurf7FB;

		for ( ; dwTotalBufferCount < dwMaxBuff; dwTotalBufferCount++) 
		{
			LPDIRECTDRAWSURFACE7 lpSurf7_2 = NULL;

			hr = m_lpDDObj->CreateSurface(pddsd, &lpSurf7_2, NULL);
			if (hr != DD_OK)
				__leave;

			LPDIRECTDRAWSURFACE4 lp4FB;
			lpSurf7->QueryInterface(IID_IDirectDrawSurface4, (LPVOID*)&lp4FB);

			LPDIRECTDRAWSURFACE4 lp4BB;
			lpSurf7_2->QueryInterface(IID_IDirectDrawSurface4, (LPVOID*)&lp4BB);

			hr = lp4FB->AddAttachedSurface(lp4BB);

			SAFE_RELEASE(lp4FB);
			SAFE_RELEASE(lp4BB);

			lpSurf7 = lpSurf7_2;
			SAFE_RELEASE(lpSurf7_2);

			if (hr != DD_OK)
				__leave;

		}
	}
	__finally 
	{
		if (hr != DD_OK) 
		{
			if (dwTotalBufferCount >= dwMinBuff) 
			{
				hr = DD_OK;
			}
			else 
			{
				dwTotalBufferCount = 0;
				SAFE_RELEASE(lpSurf7FB);
			}
		}

		if (hr == DD_OK) 
		{
			*lpdwBuffer = dwTotalBufferCount;
		}
	}

	*lplpSurf = lpSurf7FB;
	return hr;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pdwUserID : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
STDMETHODIMP
CMovieAllocator::FreeSurface(DWORD_PTR pdwUserID)
{

	if(m_pMovie->m_pDDTexture)
	{
		SAFE_RELEASE(m_pMovie->m_pDDTexture);
	}

	return S_OK;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param w : ÃèÊö
 * \param lplpSurface : ÃèÊö
 * \param dwSurfaceFlags						 : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
STDMETHODIMP
CMovieAllocator::PrepareSurface(
						  DWORD_PTR w,
						  LPDIRECTDRAWSURFACE7 lplpSurface,
						  DWORD dwSurfaceFlags
						  )
{
	return S_OK;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param lpIVMRSurfAllocNotify : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
STDMETHODIMP
CMovieAllocator::AdviseNotify(IVMRSurfaceAllocatorNotify*  lpIVMRSurfAllocNotify)
{
	return E_NOTIMPL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pdwUserID : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
STDMETHODIMP
CMovieAllocator::StartPresenting(DWORD_PTR pdwUserID)
{
	m_pMovie->m_dwFrameCount = 0;
	return S_OK;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pdwUserID : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
STDMETHODIMP
CMovieAllocator::StopPresenting(DWORD_PTR pdwUserID)
{
	return S_OK;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pdwUserID : ÃèÊö
 * \param p						 : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
STDMETHODIMP
CMovieAllocator::PresentImage(
						DWORD_PTR pdwUserID,
						VMRPRESENTATIONINFO* p
						)
{
	HRESULT hr = S_OK;
	CSafeLock lock(&m_pMovie->m_lock);

	hr = m_pMovie->m_pDDTexture->Blt(&m_pMovie->GetRect(), p->lpSurf, 
		NULL, DDBLT_WAIT, NULL);
	m_pMovie->m_dwFrameCount++;
	m_pMovie->m_bPresented = true;

	return S_OK;
}

