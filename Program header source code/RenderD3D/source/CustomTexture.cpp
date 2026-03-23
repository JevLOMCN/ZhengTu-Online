#include "../../engine/include/engine.h"
#include "../Include/RenderD3D.h"
#include "../Include/D3DDevice.h"
#include "..\include\customtexture.h"
#include "../Include/TextureManager.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pSrcSur : ÃèÊö
 * \param pDstTex : ÃèÊö
 * \param RECT* : ÃèÊö
 * \param POINT : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CopySurfaceToTexture(IDirect3DSurface8* pSrcSur,IDirect3DTexture8* pTmpTex,IDirect3DTexture8* pDstTex,const RECT* rcSrc,const POINT * ptDst)
{
	HRESULT hr;
	stRectI rect;
	stPointI pt;
	D3DSURFACE_DESC desc;
	pSrcSur->GetDesc(&desc);
	if(rcSrc == NULL)
	{
		rect = stRectI(0,0,desc.Width,desc.Height);
	}
	else rect = *rcSrc;

	if(ptDst == NULL) pt = stPointI(0,0);
	else pt = *ptDst;

	stRectI rectDst( pt.x,pt.y,pt.x + rect.Width(),pt.y + rect.Height() );

	IDirect3DSurface8* pTmpSur;
	hr = pTmpTex->GetSurfaceLevel(0,&pTmpSur);
	hr = D3DXLoadSurfaceFromSurface(pTmpSur,NULL,&rectDst,pSrcSur,NULL,&rect,D3DX_DEFAULT,0);
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
	}
	pTmpSur->Release();
	hr = GetD3DDevice()->__UpdateTexture(pTmpTex,pDstTex);

	return hr;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param * : ÃèÊö
 * \param rcClip : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool GetSurfaceClipRect(IDirect3DSurface8 * pSur,RECT* rcClip)
{
	HRESULT hr;
	D3DSURFACE_DESC desc;
	hr = pSur->GetDesc(&desc);
	if(FAILED(hr)) return false;

	D3DLOCKED_RECT lr;
	hr = pSur->LockRect(&lr,NULL,D3DLOCK_NO_DIRTY_UPDATE);
	if(FAILED(hr)) return false;

	switch(desc.Format)
	{
	case D3DFMT_A8R8G8B8:
		{
			BYTE* pData = (BYTE*)lr.pBits;
			for( size_t i=0;i<desc.Height;++i )
			{
				DWORD* p = (DWORD*)pData;
				for( size_t j = 0; j<desc.Width;++j )
				{
					if( p[j] & 0xf0000000 )
					{
						rcClip->top = i;
						goto _bak1;
					}
				}
				pData += lr.Pitch;
			}
_bak1:
			pData = (BYTE*)lr.pBits;
			for(int i=desc.Height -  1;i>=0;--i)
			{
				DWORD* p = (DWORD*)(pData + i * lr.Pitch);
				for(size_t j = 0; j<desc.Width;++j)
				{
					if(p[j] & 0xf0000000)
					{
						rcClip->bottom = i + 1;
						goto _bak2;
					}
				}
			}
_bak2:
			pData = (BYTE*)lr.pBits;
			for( size_t j=0 ; j<desc.Width; ++j )
			{
				DWORD* p = (DWORD*)pData + j;
				for( size_t i=0; i< desc.Height;++i )
				{
					if(p[0] & 0xf0000000)
					{
						rcClip->left = j;
						goto _bak3;
					}
					p = (DWORD*)((BYTE*)p + lr.Pitch);
				}
			}
_bak3:
			pData = (BYTE*)lr.pBits;
			for( int j=desc.Width-1; j >= 0 ; --j )
			{
				DWORD* p = (DWORD*)pData + j;
				for( size_t i=0; i< desc.Height;++i )
				{
					if(p[0] & 0xf0000000)
					{
						rcClip->right = j + 1;
						goto _bak4;
					}
					p = (DWORD*)((BYTE*)p + lr.Pitch);
				}
			}
		}
_bak4:
		pSur->UnlockRect();
		return true;
		break;
	case D3DFMT_A4R4G4B4:
		{
			BYTE* pData = (BYTE*)lr.pBits;
			for( size_t i=0;i<desc.Height;++i )
			{
				WORD* p = (WORD*)pData;
				for( size_t j = 0; j<desc.Width;++j )
				{
					if( p[j] & 0xf000 )
					{
						rcClip->top = i;
						goto _bak5;
					}
				}
				pData += lr.Pitch;
			}
_bak5:
			pData = (BYTE*)lr.pBits;
			for(int i=desc.Height -  1;i>=0;--i)
			{
				WORD* p = (WORD*)(pData + i * lr.Pitch);
				for(size_t j = 0; j<desc.Width;++j)
				{
					if( p[j] & 0xf000 )
					{
						rcClip->bottom = i + 1;
						goto _bak6;
					}
				}
			}
_bak6:
			pData = (BYTE*)lr.pBits;
			for( size_t j=0 ; j<desc.Width; ++j )
			{
				WORD* p = (WORD*)pData + j;
				for( size_t i=0; i< desc.Height;++i )
				{
					if( p[0] & 0xf000 )
					{
						rcClip->left = j;
						goto _bak7;
					}
					p = (WORD*)((BYTE*)p + lr.Pitch);
				}
			}
_bak7:
			pData = (BYTE*)lr.pBits;
			for( int j=desc.Width-1; j >= 0 ; --j )
			{
				WORD* p = (WORD*)pData + j;
				for( size_t i=0; i< desc.Height;++i )
				{
					if( p[0] & 0xf000 )
					{
						rcClip->right = j + 1;
						goto _bak8;
					}
					p = (WORD*)((BYTE*)p + lr.Pitch);
				}
			}
		}
_bak8:
		pSur->UnlockRect();
		return true;
		break;
	}
	pSur->UnlockRect();
	return false;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CCustomTexture::CCustomTexture(int width,int height,D3DFORMAT format,DWORD usage)
: m_width(width)
, m_height(height)
, m_format(format)
, m_usage(usage)
, m_pTex(NULL)
{
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CCustomTexture::~CCustomTexture(void)
{
	SAFE_RELEASE(m_pTex);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CCustomTexture::Init()
{
	if(m_pTex) return true;
	m_pTex = GetTextureManager()->CreateTexture(m_width,m_height,m_format,m_usage);
	return m_pTex != NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CCustomTexture::Release()
{
	SAFE_RELEASE(m_pTex);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pStream : ÃèÊö
 * \param pClipRect : ÃèÊö
 * \param RECT* : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CCustomTexture::LoadFromJpegStream(Stream* pStream,RECT* pClipRect,const RECT* pRect)
{
	if(!Init()) return false;
	SetFrameAllocator fa;
	BYTE* pData = (BYTE*)FrameAllocator::alloc(pStream->getSize());
	pStream->read(pData,pStream->getSize());

	IDirect3DTexture8* pMemTex = NULL;
	HRESULT hr;

	if( m_width == 256 && m_height == 256 )
	{
		pMemTex = GetD3DDevice()->GetTmpTexture(m_format);
		if(pMemTex) pMemTex->AddRef();
	}

	if(pMemTex == NULL)
	{
		pMemTex = GetD3DDevice()->CreateTexture(m_width,m_height,m_format,D3DPOOL_SYSTEMMEM);
		if(pMemTex == NULL) 
		{
			return false;
		}
	}

	IDirect3DSurface8* pSur;
	hr = pMemTex->GetSurfaceLevel(0,&pSur);
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		SAFE_RELEASE(pMemTex);
		return false;
	}

	hr = D3DXLoadSurfaceFromFileInMemory(pSur,NULL,pRect,pData,pStream->getSize(),pRect,D3DX_DEFAULT,0,NULL);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		SAFE_RELEASE(pSur);
		SAFE_RELEASE(pMemTex);
		return false;
	}
	
	if(pClipRect)
	{
		if(!GetSurfaceClipRect(pSur,pClipRect))
		{
			*pClipRect = stRectI(0,0,m_width,m_height);
		}
	}
	
	SAFE_RELEASE(pSur);
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		SAFE_RELEASE(pMemTex);
		return false;
	}

	//D3DXSaveTextureToFile("c:\\111.bmp",D3DXIFF_BMP,pMemTex,NULL);
	hr = GetD3DDevice()->__UpdateTexture(pMemTex,m_pTex);

	SAFE_RELEASE(pMemTex);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}

	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pStream : ÃèÊö
 * \param pClipRect : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CCustomTexture::LoadFromTgaStream(Stream* pStream,RECT* pClipRect)
{
	if(!Init()) return false;
	SetFrameAllocator fa;
	BYTE* pData = (BYTE*)FrameAllocator::alloc(pStream->getSize());
	pStream->read(pData,pStream->getSize());

	IDirect3DTexture8* pMemTex = NULL;
	HRESULT hr;
	
	if(m_width == 256 && m_height == 256)
	{
		pMemTex = GetD3DDevice()->GetTmpTexture(m_format);
		if(pMemTex) pMemTex->AddRef();
	}

	if(pMemTex == NULL)
	{
		pMemTex = GetD3DDevice()->CreateTexture(m_width,m_height,m_format,D3DPOOL_SYSTEMMEM);
		if(NULL == pMemTex) 
		{
			return false;
		}
	}

	IDirect3DSurface8* pSur;
	hr = pMemTex->GetSurfaceLevel(0,&pSur);
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		SAFE_RELEASE(pMemTex);
		return false;
	}

	hr = D3DXLoadSurfaceFromFileInMemory(pSur,NULL,NULL,pData,pStream->getSize(),NULL,D3DX_DEFAULT,0,NULL);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		SAFE_RELEASE(pSur);
		SAFE_RELEASE(pMemTex);
		return false;
	}

	if(pClipRect)
	{
		if(!GetSurfaceClipRect(pSur,pClipRect))
		{
			*pClipRect = stRectI(0,0,m_width,m_height);
		}
	}

	SAFE_RELEASE(pSur);
	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		SAFE_RELEASE(pMemTex);
		return false;
	}

	//D3DXSaveTextureToFile("c:\\111.bmp",D3DXIFF_BMP,pMemTex,NULL);
	hr = GetD3DDevice()->__UpdateTexture(pMemTex,m_pTex);

	SAFE_RELEASE(pMemTex);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}
	
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param char* : ÃèÊö
 * \param pClipRect : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CCustomTexture::LoadFromTgaFile(const char* pszFile,RECT* pClipRect)
{
	FileStream stream;
	stream.open(pszFile,FileStream::Read);
	return LoadFromTgaStream(&stream,pClipRect);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param char* : ÃèÊö
 * \param pClipRect : ÃèÊö
 * \param RECT* : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CCustomTexture::LoadFromJpegFile(const char* pszFile,RECT* pClipRect,const RECT* pRect)
{
	FileStream stream;
	stream.open(pszFile,FileStream::Read);
	return LoadFromJpegStream(&stream,pClipRect,pRect);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param x : ÃèÊö
 * \param y : ÃèÊö
 * \param color : ÃèÊö
 * \param blend : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CCustomTexture::Render(float x,float y,DWORD color ,BlendType blend )
{
	if(m_pTex == NULL) return false;
	
	stRenderInfo ri;
	ri.blend = blend;
	ri.color = color;
	ri.tex = m_pTex;
	ri.uv = stRectF(0,0,1,1);
	ri.rect = stRectF(x,y,x + m_width,y + m_height);

	return SUCCEEDED(GetD3DDevice()->Render(ri));
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param x : ÃèÊö
 * \param y : ÃèÊö
 * \param stRectF* : ÃèÊö
 * \param stPointF : ÃèÊö
 * \param color : ÃèÊö
 * \param blend : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CCustomTexture::Render(float x,float y,const stRectF* rect,const stPointF * scale,DWORD color,BlendType blend )
{
	if(m_pTex == NULL) return false;

	stRenderInfo ri;
	ri.blend = blend;
	ri.color = color;
	ri.tex = m_pTex;
	
	int cx,cy;

	if(rect)
	{
		ri.uv.SetRect(	(float)rect->left/(float)m_width, 
						(float)rect->top/(float)m_height,
						(float)rect->right/(float)m_width,
						(float)rect->bottom/(float)m_height ) ;

		cx = (int)(rect->right - rect->left);
		cy = (int)(rect->bottom - rect->top);
	}
	else
	{
		ri.uv.SetRect(0,0,1,1);
		cx = m_width;
		cy = m_height;
	}

	if(scale)
		ri.rect.SetRect(x,y,x + cx * scale->x,y + cy * scale->y);
	else
		ri.rect.SetRect(x,y,x + cx,y + cy);

	return SUCCEEDED(GetD3DDevice()->Render(ri));
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CRenderTargetTexture::Init()
{
	m_usage = D3DUSAGE_RENDERTARGET;
	if(CCustomTexture::Init())
	{
		if(m_usage == 0)
			m_pTmpTex = GetD3DDevice()->CreateTexture(m_width,m_height,m_format,D3DPOOL_SYSTEMMEM);
		return true;
	}
	else
	{
		m_usage = 0;
		m_pTmpTex = GetD3DDevice()->CreateTexture(m_width,m_height,m_format,D3DPOOL_SYSTEMMEM);
		return CCustomTexture::Init();
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CRenderTargetTexture::BeginRenderTarget()
{
	if(!m_pTex) return false;
	HRESULT hr;
	if(m_usage == D3DUSAGE_RENDERTARGET)
	{
		hr = GetD3DDevice()->__GetRenderTarget(&m_pOldRenderTarget);
		if(FAILED(hr))
		{
			TRACE_DXERROR(hr);
			return false;
		}

		IDirect3DSurface8 * pSur;
		hr = m_pTex->GetSurfaceLevel(0,&pSur);
		if(FAILED(hr))
		{
			TRACE_DXERROR(hr);
			SAFE_RELEASE(m_pOldRenderTarget);
			return false;
		}

		hr = GetD3DDevice()->__SetRenderTarget(pSur,NULL);

		pSur->Release();
		if(FAILED(hr))
		{
			TRACE_DXERROR(hr);
			SAFE_RELEASE(m_pOldRenderTarget);
			return false;
		}

		GetD3DDevice()->SetBufferLockFlags(true);
	}
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param RECT* : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CRenderTargetTexture::UpdateRenderTarget(const RECT* pRect)
{
	if(!m_pTex) return false;
	HRESULT hr;
	if(m_usage == D3DUSAGE_RENDERTARGET)
	{
		if(!m_pOldRenderTarget) return false;
		hr = GetD3DDevice()->__SetRenderTarget(m_pOldRenderTarget,NULL);
		SAFE_RELEASE(m_pOldRenderTarget);
		if(FAILED(hr))
		{
			TRACE_DXERROR(hr);
			return false;
		}
	}
	else
	{
		if(!m_pTmpTex) return false;

		IDirect3DSurface8 * pSrcSur;
		hr = GetD3DDevice()->__GetRenderTarget(&pSrcSur);
		if(FAILED(hr))
		{
			TRACE_DXERROR(hr);
			return false;
		}

		stRectI rect;
		if(!pRect)
			rect.SetRect(0,0,m_width,m_height);
		else
			rect = *pRect;

		hr = CopySurfaceToTexture(pSrcSur,m_pTmpTex,m_pTex,&rect,NULL);
		pSrcSur->Release();
		if(FAILED(hr))
		{
			TRACE_DXERROR(hr);
			return false;
		}
	}
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CRenderTargetTexture::EndRenderTarget()
{
	if(!m_pTex) return false;
	HRESULT hr;
	if(m_usage == D3DUSAGE_RENDERTARGET)
	{
		if(!m_pOldRenderTarget) return false;

		hr = GetD3DDevice()->__SetRenderTarget(m_pOldRenderTarget,NULL);
		SAFE_RELEASE(m_pOldRenderTarget);

		if(FAILED(hr))
		{
			TRACE_DXERROR(hr);
			return false;
		}
	}
	GetD3DDevice()->SetBufferLockFlags(false);
	return true;
}

void CRenderTargetTexture::Release()
{
	if(m_pOldRenderTarget)
	{
		EndRenderTarget();
	}
	//SAFE_RELEASE(m_pRenderTarget);
	CCustomTexture::Release();
}