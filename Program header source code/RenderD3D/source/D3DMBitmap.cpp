/**
 * \file      D3DMBitmap.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2005-07-05 12:29:19
 * \brief     ********************************************************************
 * 
 *	      ********************************************************************
 */

#include "../../engine/include/engine.h"
#include "../Include/RenderD3D.h"
#include "../include/d3dmbitmap.h"
#include "../Include/TextureCell.h"
#include "../Include/TextureCache.h"
#include "../Include/TextureGroup.h"
#include "../Include/D3DDevice.h"
#include "../Include/TextureManager.h"
#include "../Include/RenderD3D.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CD3DMBitmap::CD3DMBitmap(void)
: m_dwLastRenderTime(0)
, m_pTexture(NULL)
, m_pBmp(NULL)
, m_pBmpAlpha(NULL)
//, m_pMemTex(NULL)
{
}

CD3DMBitmap::~CD3DMBitmap()
{
	//SAFE_RELEASE(m_pMemTex);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pBmp : ÃèÊö
 * \param pAlpha : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CD3DMBitmap::SetBitmap(CD3DBitmap* pBmp,CD3DBitmap* pAlpha)
{
	Assert(pBmp && pAlpha);
	Assert(pBmp->GetBitmapList()->GetUsage() == Usage_Terrain && pAlpha->GetBitmapList()->GetUsage() == Usage_BlendTerrain);

	if(pBmp->GetBitmapList()->GetUsage() != Usage_Terrain || pAlpha->GetBitmapList()->GetUsage() != Usage_BlendTerrain)
		return false;

	Assert(pBmp->GetWidth() == pAlpha->GetWidth() && pBmp->GetHeight() == pAlpha->GetHeight());

	if(pBmp->GetWidth() != pAlpha->GetWidth() || pBmp->GetHeight() != pAlpha->GetHeight())
		return false;

	/*if(pBmp->GetWidth() != pBmp->GetClipWidth() || pBmp->GetHeight() != pBmp->GetClipHeight() || 
		pAlpha->GetClipWidth() != pAlpha->GetClipWidth() || pAlpha->GetHeight() != pAlpha->GetClipHeight())
	{
		Assert(0);
		return false;
	}*/

	if(pBmp->m_block.byGrpType != eGrpTypeTex && pBmp->m_block.byGrpType != eGrpTypeTexRle || (pBmp->m_block.byPixelFormat != ePixDXT1 && pBmp->m_block.byPixelFormat != ePixDXT1_5551))
	{
		Assert(0);
		return false;
	}

	if( (pAlpha->m_block.byGrpType != eGrpTypeTexA4 && pAlpha->m_block.byGrpType != eGrpTypeTexA4Rle) || pAlpha->m_block.byPixelFormat != ePixDXT3)
	{
		Assert(0);
		return false;
	}

	m_pBmp = pBmp;
	m_pBmpAlpha = pAlpha;
	return true;
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
bool CD3DMBitmap::Render(int x,int y,DWORD color,BlendType blend )
{
	if(!m_pTexture && !LoadTexture())
	{
		return false;
	}
	
	POINT ptOffset = m_pBmpAlpha->GetClipOffset();
	int width = m_pBmpAlpha->GetClipWidth();
	int height = m_pBmpAlpha->GetClipHeight();

	x += ptOffset.x;
	y += ptOffset.y;

	stRenderInfo ri;
	ri.color = color;
	ri.blend = blend;
	ri.rect.left = (float)x;
	ri.rect.top = (float)y;
	ri.rect.right = (float)(x + width);
	ri.rect.bottom = (float)(y + height);

	ri.rect &= *GetDevice()->GetRenderRect();
	if(ri.rect.IsRectEmpty()) return true;

	ri.tex = m_pTexture->GetTexCache()->GetTex();

	float nTexL = (float)m_pTexture->GetLeft();
	float nTexT = (float)m_pTexture->GetTop();

	float fTexW = (float)m_pTexture->GetTexCache()->GetWidth();
	float fTexH = (float)m_pTexture->GetTexCache()->GetHeight();

	ri.uv.left = (nTexL + ri.rect.left - x)/fTexW;
	ri.uv.top = (nTexT + ri.rect.top - y)/fTexH;
	ri.uv.right = (nTexL + ri.rect.right - x)/fTexW;
	ri.uv.bottom = (nTexT + ri.rect.bottom - y)/fTexH;

	GetD3DMBitmapManager()->m_TexGroup->OnRender(m_pTexture);

	SetLastRenderTime(xtimeGetTime());
	return SUCCEEDED(GetD3DDevice()->Render(ri));
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param x : ÃèÊö
 * \param y : ÃèÊö
 * \param RECT* : ÃèÊö
 * \param stPointF : ÃèÊö
 * \param color : ÃèÊö
 * \param blend : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CD3DMBitmap::Render(int x,int y,const RECT* rect,const stPointF * scale,DWORD color,BlendType blend )
{
	if(!m_pTexture && !LoadTexture())
	{
		return false;
	}


	stRectI rcAlphaClip = m_pBmpAlpha->GetClipRect();
	stRectI rcClip = rcAlphaClip;
	if(rect) rcClip &= *rect;

	stRenderInfo ri;
	ri.color = color;
	ri.blend = blend;

	if(scale)
	{
		if(rect)
		{
			if(rcAlphaClip.left > rect->left)
				x += (int)(scale->x * (rcAlphaClip.left - rect->left));
			if(rcAlphaClip.top > rect->top)
				y += (int)(scale->y * (rcAlphaClip.top - rect->top));
		}
		else
		{
			x += (int)(scale->x * rcAlphaClip.left);
			y += (int)(scale->y * rcAlphaClip.top);
		}

		ri.rect.left = (float)x;
		ri.rect.top = (float)y;
		ri.rect.right = rcClip.Width() * scale->x + x;
		ri.rect.bottom = rcClip.Height() * scale->y + y;

		if((ri.rect & *GetDevice()->GetRenderRect()).IsRectEmpty())
			return true;
		ri.uv = rcClip;

		if(scale->x != 1.0f){
			ri.uv.left+= 1.0f;
			ri.uv.right-= 1.0f;
		}
		if(scale->y != 1.0f){
			ri.uv.top+= 1.0f;
			ri.uv.bottom-= 1.0f;
		}

	}
	else
	{
		if(rect)
		{
			if(rcAlphaClip.left > rect->left)
				x += (rcAlphaClip.left - rect->left);
			if(rcAlphaClip.top > rect->top)
				y += (rcAlphaClip.top - rect->top);
		}
		else
		{
			x += rcAlphaClip.left;
			y += rcAlphaClip.top;
		}

		ri.rect.left = (float)x;
		ri.rect.top = (float)y;
		ri.rect.right = (float)(rcClip.Width() + x);
		ri.rect.bottom = (float)(rcClip.Height() + y);

		ri.rect &= *GetDevice()->GetRenderRect();
		if(ri.rect.IsRectEmpty()) 
			return true;

		ri.uv.left = ri.rect.left - x + rcClip.left;
		ri.uv.top = ri.rect.top - y + rcClip.top;
		ri.uv.right = ri.rect.right - x + rcClip.left;
		ri.uv.bottom = ri.rect.bottom - y + rcClip.top;
	}

	float fTexW,fTexH;

	long nTexL = m_pTexture->GetLeft();
	long nTexT = m_pTexture->GetTop();

	fTexW = (float)m_pTexture->GetTexCache()->GetWidth();
	fTexH = (float)m_pTexture->GetTexCache()->GetHeight();

	ri.uv.left = (float)(ri.uv.left + nTexL) / fTexW;
	ri.uv.top = (float)(ri.uv.top + nTexT) / fTexH;

	ri.uv.right = (float)(ri.uv.right + nTexL ) / fTexW ;
	ri.uv.bottom = (float)(ri.uv.bottom + nTexT ) / fTexH ;

	ri.tex = m_pTexture->GetTexCache()->GetTex();

	GetD3DMBitmapManager()->m_TexGroup->OnRender(m_pTexture);

	SetLastRenderTime(xtimeGetTime());
	return SUCCEEDED(GetD3DDevice()->Render(ri));
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pTex : ÃèÊö
 * \param dstFormat : ÃèÊö
 * \param ptTex : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CD3DMBitmap::BlendTexture(IDirect3DTexture8* pTex,D3DFORMAT dstFormat,POINT ptTex)
{
	Assert(m_pBmp->GetClipWidth() == 128 && m_pBmp->GetClipHeight() == 128);

	// »ñµÃÄÚ´æµÄÄ¿±êÎÆÀí
	IDirect3DTexture8* pUpdate = GetD3DDevice()->GetTmpTexture(dstFormat);
	Assert(pUpdate);
	if(!pUpdate) return false;

	HRESULT hr;
	int width,height;

	int alphaClipWidth = m_pBmpAlpha->GetClipWidth();
	int bmpClipWidth = m_pBmp->GetClipWidth();

	POINT alphaClipOffset  = m_pBmpAlpha->GetClipOffset();

	width = GetWidth4(m_pBmpAlpha->GetClipWidth());
	height = GetWidth4(m_pBmpAlpha->GetClipHeight());

	stRectI rect( ptTex.x, ptTex.y, ptTex.x + width, ptTex.y + height );

	size_t nBlockRowCount = rect.Height()/4;
	size_t nBLockColCount = rect.Width()/4;
	size_t nBmpBlockColCount = bmpClipWidth/4;
	size_t nTopBlokcRowCount = alphaClipOffset.y/4;
	size_t nLeftBlockColCount = alphaClipOffset.x/4;

	SetFrameAllocator fa;
	BYTE* pDstData;
	int pDstPitch;

	D3DLOCKED_RECT lr;
	if(dstFormat == D3DFMT_DXT3)
	{
		hr = pUpdate->LockRect(0,&lr,&rect,0);
		if(FAILED(hr))
		{
			TRACE_DXERROR(hr);
			return false;
		}
		pDstData = (BYTE*)lr.pBits;
		pDstPitch = lr.Pitch;

	}
	else
	{
		pDstData = (BYTE*)FrameAllocator::alloc(nBlockRowCount * nBLockColCount * 16);
		pDstPitch = nBLockColCount * 16;
	}

	BYTE* pDst = pDstData;

	if( m_pBmpAlpha->m_block.byGrpType == eGrpTypeTexA4Rle && m_pBmp->m_block.byGrpType == eGrpTypeTexRle )
	{
		BYTE* pSrcAlphaRle = m_pBmpAlpha->GetRleBlock(0,0);
		if(pSrcAlphaRle == NULL){
			if(dstFormat == D3DFMT_DXT3)
			{
				hr = pUpdate->UnlockRect(0);
				Assert(SUCCEEDED(hr));
			}
			return false;
		}

		BYTE* pSrcColorRle = m_pBmp->GetRleBlock(0,0);
		
		if(pSrcColorRle == NULL){
			if(dstFormat == D3DFMT_DXT3)
			{
				hr = pUpdate->UnlockRect(0);
				Assert(SUCCEEDED(hr));
			}
			return false;
		}

		SetFrameAllocator fa;
		BYTE* buf = (BYTE*)FrameAllocator::alloc(nBLockColCount * 8);
		BYTE* buf1 = (BYTE*)FrameAllocator::alloc(nBmpBlockColCount * 8);
		
		for(size_t i=0;i<nTopBlokcRowCount;++i)
		{
			RLE_ForwardALine(pSrcColorRle,nBmpBlockColCount*8);
			//RLE_DecodeALine( pSrcColorRle,nBmpBlockColCount*8,buf1,NULL,stRleDecodeByteToByte() );
		}

		for(size_t i=0;i<nBlockRowCount;++i)
		{
			BYTE* pDst0 = pDst;
			RLE_DecodeALine( pSrcAlphaRle,nBLockColCount*8,buf,NULL,stRleDecodeByteToByte() );
			RLE_DecodeALine( pSrcColorRle,nBmpBlockColCount*8,buf1,NULL,stRleDecodeByteToByte() );

			BYTE* pSrcAlpha = buf;
			BYTE* pSrcColor = buf1 + nLeftBlockColCount * 8;
			for(size_t j=0;j<nBLockColCount;++j)
			{
				*(ULONGLONG*)pDst0 = *(ULONGLONG*)pSrcAlpha;
				*(ULONGLONG*)&pDst0[8] = *(ULONGLONG*)pSrcColor;
				pDst0 += 16;
				pSrcAlpha += 8;
				pSrcColor += 8;
			}
			pDst += pDstPitch;
		}
	}
	else if( m_pBmpAlpha->m_block.byGrpType == eGrpTypeTexA4Rle && m_pBmp->m_block.byGrpType == eGrpTypeTex )
	{
		BYTE* pSrcColor1 = m_pBmp->m_pData + nLeftBlockColCount * 8 + nTopBlokcRowCount * bmpClipWidth * 8;
		BYTE* pSrcAlphaRle = m_pBmpAlpha->GetRleBlock(0,0);
		
		if(pSrcAlphaRle == NULL){
			if(dstFormat == D3DFMT_DXT3)
			{
				hr = pUpdate->UnlockRect(0);
				Assert(SUCCEEDED(hr));
			}
			return false;
		}

		SetFrameAllocator fa;
		BYTE* buf = (BYTE*)FrameAllocator::alloc(nBLockColCount * 8);
		for(size_t i=0;i<nBlockRowCount;++i)
		{
			BYTE* pDst0 = pDst;
			RLE_DecodeALine( pSrcAlphaRle,nBLockColCount*8,buf,NULL,stRleDecodeByteToByte() );
			BYTE* pSrcAlpha = buf;
			BYTE* pSrcColor = pSrcColor1;
			for(size_t j=0;j<nBLockColCount;++j)
			{
				*(ULONGLONG*)pDst0 = *(ULONGLONG*)pSrcAlpha;
				*(ULONGLONG*)&pDst0[8] = *(ULONGLONG*)pSrcColor;
				pDst0 += 16;
				pSrcAlpha += 8;
				pSrcColor += 8;
			}
			pSrcColor1 += bmpClipWidth * 8;
			pDst += pDstPitch;
		}
	}
	else if(m_pBmpAlpha->m_block.byGrpType == eGrpTypeTexA4 && m_pBmp->m_block.byGrpType == eGrpTypeTex )
	{
		BYTE* pSrcColor1 = m_pBmp->m_pData + nLeftBlockColCount * 8 + nTopBlokcRowCount * bmpClipWidth * 8;
		BYTE* pSrcAlpha = m_pBmpAlpha->m_pData;
		for(size_t i=0;i<nBlockRowCount;++i)
		{
			BYTE* pDst0 = pDst;
			BYTE* pSrcColor = pSrcColor1;
			for(size_t j=0;j<nBLockColCount;++j)
			{
				*(ULONGLONG*)pDst0 = *(ULONGLONG*)pSrcAlpha;
				*(ULONGLONG*)&pDst0[8] = *(ULONGLONG*)pSrcColor;
				pDst0 += 16;
				pSrcAlpha += 8;
				pSrcColor += 8;
			}
			pDst += pDstPitch;
			pSrcColor1 += bmpClipWidth * 8;
		}
	}

	if(dstFormat == D3DFMT_DXT3)
	{
		hr = pUpdate->UnlockRect(0);
		Assert(SUCCEEDED(hr));
	}
	else
	{
		stRectI srcRect(0,0,width,height);
		IDirect3DSurface8* pDstSur;
		hr = pUpdate->GetSurfaceLevel(0,&pDstSur);
		Assert(SUCCEEDED(hr));
		hr = D3DXLoadSurfaceFromMemory(pDstSur,NULL,&rect,pDstData,D3DFMT_DXT3,pDstPitch,NULL,&srcRect,D3DX_DEFAULT,0);
		Assert(SUCCEEDED(hr));
	}

	//char szFileName[MAX_PATH];
	//static int i=0;
	//sprintf(szFileName,"c:\\%05d.bmp",i++);

	//D3DXSaveTextureToFile(szFileName,D3DXIFF_BMP,pUpdate,NULL);

	hr = GetD3DDevice()->__UpdateTexture(pUpdate,pTex);

	if(FAILED(hr))
	{
		TRACE_DXERROR(hr);
		return false;
	}
	return true;
}

//bool CD3DMBitmap::BlendTexture(IDirect3DTexture8* pTex,D3DFORMAT dstFormat,POINT ptTex)
//{
//	
//	IDirect3DTexture8 * pTmpTex = NULL;
//	HRESULT hr;
//	
//	stRectI dstRect(ptTex.x,ptTex.y,ptTex.x + m_pBmp->GetWidth(),ptTex.y + m_pBmp->GetHeight());
//	if((dstRect.Width() & 0x11) || (dstRect.Height() & 0x11))
//	{
//		Assert(0);
//		return false;
//	}
//	stRectI rect(0,0,m_pBmp->GetWidth(),m_pBmp->GetHeight());
//
//	if( m_pMemTex == NULL)
//	{
//		// »ñµÃÄÚ´æµÄÄ¿±êÎÆÀí
//
//		D3DLOCKED_RECT lr;
//		if(dstFormat != D3DFMT_DXT3)
//		{
//			pTmpTex = GetD3DDevice()->GetTmpTexture(D3DFMT_DXT3);
//			if(!pTmpTex) return false;
//
//			hr = pTmpTex->LockRect(0,&lr,&rect,D3DLOCK_NO_DIRTY_UPDATE);
//			if( FAILED(hr) )
//			{
//				TRACE_DXERROR( hr );
//				return false;
//			}
//		}
//		else
//		{
//			m_pMemTex = GetD3DDevice()->CreateMemTex(dstFormat,rect.Width(),rect.Height());
//			if(!m_pMemTex) return false;
//
//			hr = m_pMemTex->LockRect(0,&lr,&rect,D3DLOCK_NO_DIRTY_UPDATE);
//			
//			if( FAILED(hr) )
//			{
//				TRACE_DXERROR(hr);
//				return false;
//			}
//		}
//
//		BYTE* pDst = (BYTE*)lr.pBits;
//		size_t nBlockRowCount = rect.Height()/4;
//		size_t nBLockColCount = rect.Width()/4;
//
//		if( m_pBmpAlpha->m_block.byGrpType == eGrpTypeTexA4Rle && m_pBmp->m_block.byGrpType == eGrpTypeTexRle )
//		{
//			BYTE* pSrcAlphaRle = m_pBmpAlpha->GetRleBlock(0,0);
//			BYTE* pSrcColorRle = m_pBmp->GetRleBlock(0,0);
//			SetFrameAllocator fa;
//			BYTE* buf = (BYTE*)FrameAllocator::alloc(nBLockColCount * 8);
//			BYTE* buf1 = (BYTE*)FrameAllocator::alloc(nBLockColCount * 8);
//			for(size_t i=0;i<nBlockRowCount;++i)
//			{
//				BYTE* pDst0 = pDst;
//				RLE_DecodeALine( pSrcAlphaRle,nBLockColCount*8,buf,NULL,stRleDecodeByteToByte() );
//				RLE_DecodeALine( pSrcColorRle,nBLockColCount*8,buf1,NULL,stRleDecodeByteToByte() );
//				BYTE* pSrcAlpha = buf;
//				BYTE* pSrcColor = buf1;
//				for(size_t j=0;j<nBLockColCount;++j)
//				{
//					*(ULONGLONG*)pDst0 = *(ULONGLONG*)pSrcAlpha;
//					*(ULONGLONG*)&pDst0[8] = *(ULONGLONG*)pSrcColor;
//					pDst0 += 16;
//					pSrcAlpha += 8;
//					pSrcColor += 8;
//				}
//				pDst += lr.Pitch;
//			}
//		}
//		else if( m_pBmpAlpha->m_block.byGrpType == eGrpTypeTexA4Rle && m_pBmp->m_block.byGrpType == eGrpTypeTex )
//		{
//			BYTE* pSrcColor = m_pBmp->m_pData;
//			BYTE* pSrcAlphaRle = m_pBmpAlpha->GetRleBlock(0,0);
//			SetFrameAllocator fa;
//			BYTE* buf = (BYTE*)FrameAllocator::alloc(nBLockColCount * 8);
//			for(size_t i=0;i<nBlockRowCount;++i)
//			{
//				BYTE* pDst0 = pDst;
//				RLE_DecodeALine( pSrcAlphaRle,nBLockColCount*8,buf,NULL,stRleDecodeByteToByte() );
//				BYTE* pSrcAlpha = buf;
//				for(size_t j=0;j<nBLockColCount;++j)
//				{
//					*(ULONGLONG*)pDst0 = *(ULONGLONG*)pSrcAlpha;
//					*(ULONGLONG*)&pDst0[8] = *(ULONGLONG*)pSrcColor;
//					pDst0 += 16;
//					pSrcAlpha += 8;
//					pSrcColor += 8;
//				}
//				pDst += lr.Pitch;
//			}
//		}
//		else if(m_pBmpAlpha->m_block.byGrpType == eGrpTypeTexA4 && m_pBmp->m_block.byGrpType == eGrpTypeTex )
//		{
//			BYTE* pSrcColor = m_pBmp->m_pData;
//			BYTE* pSrcAlpha = m_pBmpAlpha->m_pData;
//			for(size_t i=0;i<nBlockRowCount;++i)
//			{
//				BYTE* pDst0 = pDst;
//				for(size_t j=0;j<nBLockColCount;++j)
//				{
//					*(ULONGLONG*)pDst0 = *(ULONGLONG*)pSrcAlpha;
//					*(ULONGLONG*)&pDst0[8] = *(ULONGLONG*)pSrcColor;
//					pDst0 += 16;
//					pSrcAlpha += 8;
//					pSrcColor += 8;
//				}
//				pDst += lr.Pitch;
//			}
//		}
//
//		if(dstFormat != D3DFMT_DXT3)
//		{
//			hr = pTmpTex->UnlockRect(0);
//			if( FAILED(hr) )
//			{
//				TRACE_DXERROR(hr);
//				return false;
//			}
//
//			IDirect3DSurface8* pSurSrc,*pSurDst;
//			hr = pTmpTex->GetSurfaceLevel(0,&pSurSrc);
//			if( FAILED(hr) )
//			{
//				TRACE_DXERROR(hr);
//				return false;
//			}
//
//			m_pMemTex = GetD3DDevice()->CreateMemTex(dstFormat,rect.Width(),rect.Height());
//			if(!m_pMemTex) 
//			{
//				pSurSrc->Release();
//				return false;
//			}
//			
//			hr = m_pMemTex->GetSurfaceLevel(0,&pSurDst);
//			if( FAILED(hr) )
//			{
//				pSurSrc->Release();
//				TRACE_DXERROR(hr);
//				return false;
//			}
//
//			hr = D3DXLoadSurfaceFromSurface(pSurDst,NULL,&rect,pSurSrc,NULL,&rect,D3DX_DEFAULT,0);
//
//			pSurSrc->Release();
//			pSurDst->Release();
//
//			if( FAILED(hr) )
//			{
//				m_pMemTex->Release();
//				return false;
//			}
//		}
//		else
//		{
//			hr = m_pMemTex->UnlockRect(0);
//			if(FAILED(hr))
//			{
//				m_pMemTex->Release();
//				TRACE_DXERROR(hr);
//				return false;
//			}
//		}
//	}
//
//	pTmpTex = GetD3DMBitmapManager()->GetMemTex();
//	//pTmpTex = GetD3DDevice()->GetTmpTexture(dstFormat);
//	if(!pTmpTex) return false;
//
//	IDirect3DSurface8* pSurDst,*pSurSrc;
//	hr = pTmpTex->GetSurfaceLevel(0,&pSurDst);
//	if( FAILED(hr) )
//	{
//		TRACE_DXERROR(hr);
//		return false;
//	}
//
//	hr = m_pMemTex->GetSurfaceLevel(0,&pSurSrc);
//	if( FAILED(hr) )
//	{
//		TRACE_DXERROR(hr);
//		pSurDst->Release();
//		return false;
//	}
//
//	hr = D3DXLoadSurfaceFromSurface(pSurDst,NULL,&dstRect,pSurSrc,NULL,NULL,D3DX_DEFAULT,0);
//
//	pSurDst->Release();
//	pSurSrc->Release();
//
//	if( FAILED(hr) )
//	{
//		TRACE_DXERROR(hr);
//		return false;
//	}
//
//	/*char szFilename[MAX_PATH];
//	static int i=0;
//	sprintf(szFilename,"c:\\%04d.dds",i++);
//	D3DXSaveTextureToFile(szFilename,D3DXIFF_DDS,pTmpTex,NULL);*/
//
//	hr = GetD3DDevice()->GetDevice()->UpdateTexture(pTmpTex,pTex);
//
//	if(FAILED(hr))
//	{
//		TRACE_DXERROR(hr);
//		return false;
//	}
//
//	//m_pMemTex->Release();
//	//m_pMemTex = NULL;
//	return true;
//
//}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CD3DMBitmap::ReleaseTexture()
{
	if(m_pTexture)
	{
		GetD3DDevice()->FinalFlush();
		m_pTexture->ReleaseTexture();
		//GetTextureManager()->GetTextureAllocator(Usage_BlendTerrain)->ReleaseATextureCell(m_pTexture);
		m_pTexture = NULL;
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CD3DMBitmap::LoadTexture(void)
{
	Assert(m_pBmp && m_pBmpAlpha);

	if(!m_pBmp->IsLoadData()) 
	{
		if(!m_pBmp->LoadBitmap())
		{
			Assert(0);
			return false;
		}
	}

	if(!m_pBmpAlpha->IsLoadData()) 
	{
		if(!m_pBmpAlpha->LoadBitmap())
		{
			Assert(0);
			return false;
		}
	}

	CMBmpTextureGroup* pTexGroup = GetD3DMBitmapManager()->GetTextureGroup();
	if(!pTexGroup) return false;

	CMBmpTextureCell* pCell = (CMBmpTextureCell*)pTexGroup->CreateTexture(m_pBmpAlpha->GetClipWidth(),m_pBmpAlpha->GetClipHeight());
	if(!pCell) return false;

	POINT ptTex = {pCell->GetLeft(),pCell->GetTop()};
	BlendTexture(pCell->GetTexCache()->GetTex(),pTexGroup->GetFormat(),ptTex);
	m_pTexture = pCell;
	pCell->SetBitmap(this);
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//-----------------------------------------
// CD3DMBitmapManager
//-----------------------------------------
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pBmp : ÃèÊö
 * \param pAlpha : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CD3DMBitmapManager::ClearFreeMBitmap()
{
	DWORD time = xtimeGetTime();
	for(tMBitmaps::iterator it = m_MBitmaps.begin();it != m_MBitmaps.end();)
	{
		if(time - it->second->GetLastRenderTime() > 30000)
		{
			it->second->ReleaseTexture();
			m_allocMBitmap.freePointer(it->second);
			it = m_MBitmaps.erase(it);
		}
		else ++it;
	}
}
enum {MAX_MBITMAP_COUNT = 1024};
CD3DMBitmap* CD3DMBitmapManager::GetMBitmap(CD3DBitmap* pBmp,CD3DBitmap* pAlpha)
{
	tMBitmaps::iterator it = FindMBitmap(pBmp,pAlpha);
	if(it != m_MBitmaps.end()) return it->second;
	if(m_MBitmaps.size() >= MAX_MBITMAP_COUNT)
	{
		ClearFreeMBitmap();
	}
	CD3DMBitmap* pMBmp = LOOKASIDE_ALLOC(m_allocMBitmap);
	if(!pMBmp->SetBitmap(pBmp,pAlpha))
	{
		m_allocMBitmap.freePointer(pMBmp);
		return NULL;
	}
	ULONGLONG key = GetKey(pBmp,pAlpha);
	m_MBitmaps[key] = pMBmp;
	//Engine_WriteLogF("MBitmap count:%d\n",m_MBitmaps.size());
	return pMBmp;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pBmp : ÃèÊö
 * \param pAlpha : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CD3DMBitmapManager::tMBitmaps::iterator CD3DMBitmapManager::FindMBitmap(CD3DBitmap* pBmp,CD3DBitmap* pAlpha)
{
	ULONGLONG key = GetKey(pBmp,pAlpha);
	return m_MBitmaps.find(key);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CD3DMBitmapManager::Release()
{
	for(tMBitmaps::iterator it = m_MBitmaps.begin();it != m_MBitmaps.end();++it)
	{
		m_allocMBitmap.freePointer(it->second);
	}
	m_MBitmaps.clear();
	m_allocMBitmap.freeMemory();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CMBmpTextureGroup* CD3DMBitmapManager::GetTextureGroup()
{
	if(m_TexGroup == NULL) m_TexGroup = GetTextureManager()->CreateTextureGroup(this);
	//if(m_TexGroup) ((CTextureGroup*)m_TexGroup)->OnRender();
	return m_TexGroup;
}

//----------------------------------------------------------------------


void CD3DMBitmapManager::Create()
{
	/*D3DFORMAT format = GetTextureManager()->GetTextureFormat(Usage_BlendTerrain);
	if(GetD3DDevice()->GetMaxTextureBlendStages() <= 1)
		m_pMemTex = GetD3DDevice()->CreateMemTex(format,256,256);*/
}

void CD3DMBitmapManager::Destroy()
{
	//SAFE_RELEASE(m_pMemTex);
}