#pragma once

#include <strmif.h>
#include <uuids.h>
#include <vfwmsgs.h>
#include <Amvideo.h>
class CMovie;
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CMovieAllocator : public  IVMRSurfaceAllocator, 
								IVMRImagePresenter
{
	HWND m_hwndApp;
	CMovie* m_pMovie;
	IDirectDraw7* m_lpDDObj;
	IVMRWindowlessControl*  m_pWC;          // windowless control
	IVMRSurfaceAllocator*   m_pAlloc;       // DEFAULT allocator presenter
	IVMRImagePresenter*     m_pPresenter;   // image presenter
	IDirectDrawSurface7*    m_lpBackBuffer; // back buffer

	HMONITOR                    m_hMonitor;     // monitor handle
	DWORD                       m_dwFrameNum;   // number of frames drawn

	bool                        m_bErrorMessage;

	// DirectShow specific members
	DDCAPS                          m_ddHWCaps;     // hardware capabilities

public:
	CMovieAllocator(CMovie* pMovie,HWND hwndApplication, HRESULT *phr);
	~CMovieAllocator();
	HRESULT Initialize();

	//-------------------------------
	// IVMRSurfaceAllocator Ω”ø⁄
	//-------------------------------
	STDMETHODIMP AdviseNotify(IVMRSurfaceAllocatorNotify*  lpIVMRSurfAllocNotify);
	STDMETHODIMP PrepareSurface(DWORD_PTR w,LPDIRECTDRAWSURFACE7 lplpSurface,DWORD dwSurfaceFlags);
	STDMETHODIMP FreeSurface(DWORD_PTR pdwUserID);
	STDMETHODIMP AllocateSurface(DWORD_PTR pdwUserID,VMRALLOCATIONINFO *w,DWORD* lpdwBuffer,LPDIRECTDRAWSURFACE7* lplpSurface);
	//--------------------------------------------------------------------

	//-------------------------------
	// IVMRImagePresenter Ω”ø⁄
	//-------------------------------
	STDMETHODIMP StartPresenting(DWORD_PTR pdwUserID);
	STDMETHODIMP StopPresenting(DWORD_PTR pdwUserID);
	STDMETHODIMP PresentImage(DWORD_PTR pdwUserID,VMRPRESENTATIONINFO* p);
	//--------------------------------------------------------------------


	HRESULT AllocateOffscreenSurface(
		LPDIRECTDRAWSURFACE7* lplpSurf,
		DWORD dwFlags,
		DDSURFACEDESC2* pddsd,
		DWORD dwMinBuffers,
		DWORD dwMaxBuffers,
		DWORD* lpdwBuffer,
		BOOL fAllowBackBuffer
		);
	HRESULT AllocateOverlaySurface(
		LPDIRECTDRAWSURFACE7* lplpSurf,
		DWORD dwFlags,
		DDSURFACEDESC2* pddsd,
		DWORD dwMinBuffers,
		DWORD dwMaxBuffers,
		DWORD* lpdwBuffer
		);
	HRESULT AllocateSurfaceWorker(
		DWORD dwFlags,
		LPBITMAPINFOHEADER lpHdr,
		LPDDPIXELFORMAT lpPixFmt,
		LPSIZE lpAspectRatio,
		DWORD dwMinBackBuffers,
		DWORD dwMaxBackBuffers,
		DWORD* lpdwBackBuffer,
		LPDIRECTDRAWSURFACE7* lplpSurface,
		DDSURFACEDESC2* pddsdDisplay    );

	HRESULT CreateDefaultAllocatorPresenter();
	HRESULT InitializeEnvironment();
};

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CMovie
{
	friend class CMovieAllocator;
	CLock		m_lock;
	IDirectDrawSurface7*    m_pDDDecode;	// Front buffer
	IDirectDrawSurface7*	m_pDDTexture;	// Back buffer
	DWORD		m_dwFrameCount;
	SIZE		m_VideoAR;
	SIZE		m_VideoSize;
	bool		m_bPresented;
	bool		m_bDirectedFlips;
public:
	CMovie(void);
	~CMovie(void);
	bool Init();
	RECT GetRect();
};
