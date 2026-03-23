/**
 * \file      TextureGroup.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2005-07-05 12:29:20
 * \brief     ÎÆÀíGroup£¬¶àÕÅTextureCacheÎªÒ»×é£¬´æ·ÅÍ¬Ò»ÀàĞÍµÄÎÆÀí
 * 
 *	      ÎÆÀíGroup£¬¶àÕÅTextureCacheÎªÒ»×é£¬´æ·ÅÍ¬Ò»ÀàĞÍµÄÎÆÀí
 */

#include "../../engine/include/engine.h"
#include "../include/texturegroup.h"
#include "../include/texturecache.h"
#include "../include/d3ddevice.h"
#include "../include/D3DBitmapList.h"
#include "../include/TextureAllocator.h"
#include "../include/TextureCell.h"
#include "../include/TextureManager.h"
#include "../Include/RenderD3D.h"
#include "../Include/D3DMBitmap.h"

std::lookaside_allocator<CMBmpTextureCache> CMBmpTextureGroup::mAllocCache;
std::lookaside_allocator<CBmpTextureCache> CBmpTextureGroup::mAllocCache;
std::lookaside_allocator<CFontTextureCache,4> CFontTextureGroup::mAllocCache;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureGroup::CTextureGroup(TextureUsageType usage,D3DFORMAT format,size_t texWidth,size_t texHeight,bool bRowMajor,bool bOnlyAddTail/*,CTextureAllocator* pAllocator*/)
: m_format(format)
, m_texWidth(texWidth)
, m_texHeight(texHeight)
, m_bRowMajor(bRowMajor)
, m_bOnlyAddTail(bOnlyAddTail)
//, m_pAllocator(pAllocator)
, m_dwTime(0)
, m_usage(usage)
{
	m_TextureCaches.reserve(16);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureGroup::~CTextureGroup(void)
{
	Release();
	/*
	for(t_TextureBitmapListsIt it = m_BmpLists.begin(); it != m_BmpLists.end(); ++it)
	{
		(*it)->m_pTexGroup = NULL;
	}
	//*/
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param width : ÃèÊö
 * \param height : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureCell* CTextureGroup::CreateTextureInCache(size_t width, size_t height)
{
	size_t aDiv[] = {4,2,1};
	for (size_t i=0; i<count_of(aDiv); i++)
	{
		size_t diff;
		if(m_bRowMajor) diff = height/aDiv[i];
		else diff = width / aDiv[i];
		for(t_TextureCachesIt it = m_TextureCaches.begin(); it != m_TextureCaches.end();++it)
		{
			CTextureCache* pCache = (*it);
			CTextureCell* p = pCache->PushTexture(width,height,diff);
			if (p) return p;
		}
	}
	return NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param width : ÃèÊö
 * \param height : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureCell* CTextureGroup::CreateTexture(size_t width,size_t height)
{
	//if(m_format == D3DFMT_DXT1 || m_format == D3DFMT_DXT3)
	{
		width = GetWidth4(width);
		height = GetWidth4(height);
		Assert(0==(width & 3));
		Assert(0==(height & 3));
	}

	CTextureCell* pCell = CreateTextureInCache(width,height);
	if(pCell) 
	{
		GetAllocator()->AddTextureCell(pCell);
		return pCell;
	}

	OnRender();

	CTextureCache* pCache = GetAllocator()->AllocTexture(this,width,height);
	if(pCache)
	{
		CTextureCell* p = pCache->PushTexture(width,height,0);
		if(p)
		{
			GetAllocator()->AddTextureCell(p);
			return p;
		}
		else
		{
			//p = pCache->PushTexture(width,height,0);
			Assert(0);
		}
	}
	Assert(0);
	return NULL;
}	

CTextureCell* CTextureGroup::TryCreateTexture(size_t width,size_t height)
{
	//if(m_format == D3DFMT_DXT1 || m_format == D3DFMT_DXT3)
	{
		width = GetWidth4(width);
		height = GetWidth4(height);
	}

	CTextureCell* pCell = CreateTextureInCache(width,height);
	if(pCell) 
	{
		GetAllocator()->AddTextureCell(pCell);
		return pCell;
	}

	OnRender();

	CTextureCache* pCache = GetAllocator()->TryAllocTexture(this,width,height);
	if(pCache)
	{
		CTextureCell* p = pCache->PushTexture(width,height,0);
		if(p)
		{
			GetAllocator()->AddTextureCell(p);
			return p;
		}
		else
		{
			//p = pCache->PushTexture(width,height,0);
			Assert(0);
		}
	}
	return NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CTextureGroup::Release(void)
{
	for(t_TextureCachesIt it = m_TextureCaches.begin(); it != m_TextureCaches.end();++it)
	{
		GetAllocator()->ReleaseATexture((*it)->GetTex());
		DestroyCache(*it);
		//delete (*it);
	}
	m_TextureCaches.resize(0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CTextureGroup::OnRender()
{
	m_dwTime = xtimeGetTime();
	GetAllocator()->UpdateTextureGroup(this);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//------------------------------------------------------
// CMBmpTextureGroup
//------------------------------------------------------
CMBmpTextureGroup::~CMBmpTextureGroup()
{
	Release();
}

CTextureAllocator* CMBmpTextureGroup::GetAllocator()
{
	return GetTextureManager()->GetTextureAllocator(Usage_BlendTerrain);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param * : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureCache* CMBmpTextureGroup::AddTextureCache(IDirect3DTexture8 * pTex)
{
	CTextureCache* pCache = LOOKASIDE_PALLOC5(mAllocCache,pTex,m_texWidth,m_texHeight,m_bRowMajor,m_bOnlyAddTail);
	//CTextureCache* pCache = new CMBmpTextureCache(pTex,m_texWidth,m_texHeight,m_bRowMajor,m_bOnlyAddTail);
	if(pCache) m_TextureCaches.push_back(pCache);
	return pCache;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param p : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CMBmpTextureGroup::OnRender(CMBmpTextureCell* p)
{
	CD3DMBitmap* pBmp = p->GetBitmap();
	Assert(pBmp);
	pBmp->SetLastRenderTime(xtimeGetTime());
	GetAllocator()->UpdateTextureCell(p);
}
void CMBmpTextureGroup::DestroyCache(CTextureCache* pCache)
{
	mAllocCache.freePointer((CMBmpTextureCache*)pCache);
}
//------------------------------------------------------------

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//------------------------------------------------------
// CBmpTextureGroup
//------------------------------------------------------
CBmpTextureGroup::~CBmpTextureGroup()
{
	Release();
}

void CBmpTextureGroup::Release(void)
{
	CTextureGroup::Release();
}

CTextureAllocator* CBmpTextureGroup::GetAllocator()
{
	return GetTextureManager()->GetTextureAllocator(m_BmpLists.front());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param * : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureCache* CBmpTextureGroup::AddTextureCache(IDirect3DTexture8 * pTex)
{
	CTextureCache* pCache = LOOKASIDE_PALLOC5(mAllocCache,pTex,m_texWidth,m_texHeight,m_bRowMajor,m_bOnlyAddTail);
	//CTextureCache* pCache = new CBmpTextureCache(pTex,m_texWidth,m_texHeight,m_bRowMajor,m_bOnlyAddTail);
	if(pCache) m_TextureCaches.push_back(pCache);
	return pCache;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param p : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CBmpTextureGroup::OnRender(CBmpTextureCell* p)
{
	CD3DBitmap* pBmp = p->GetBitmap();

	Assert(pBmp);

	pBmp->SetLastRenderTime(xtimeGetTime());

	CTextureAllocator* pAlloc = GetAllocator();

	pAlloc->UpdateTextureCell(p);
}

void CBmpTextureGroup::DestroyCache(CTextureCache* pCache)
{
	mAllocCache.freePointer((CBmpTextureCache*)pCache);
}
//------------------------------------------------------------


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//------------------------------------------------------------
// CFontTextureGroup
//------------------------------------------------------------
CFontTextureGroup::~CFontTextureGroup()
{
	Release();
}

CTextureAllocator* CFontTextureGroup::GetAllocator()
{
	return GetTextureManager()->GetFontTextureAllocator(m_iFont);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param * : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureCache* CFontTextureGroup::AddTextureCache(IDirect3DTexture8 * pTex)
{
	CTextureCache* pCache = LOOKASIDE_PALLOC5(mAllocCache,pTex,m_texWidth,m_texHeight,m_bRowMajor,m_bOnlyAddTail);
	//CTextureCache* pCache = new CFontTextureCache(pTex,m_texWidth,m_texHeight,m_bRowMajor,m_bOnlyAddTail);
	if(pCache) m_TextureCaches.push_back(pCache);
	return pCache;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pCell : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CFontTextureGroup::OnRender(CFontTextureCell* pCell)
{
	pCell->SetLastTime(xtimeGetTime());
	GetAllocator()->UpdateTextureCell(pCell);
}

void CFontTextureGroup::DestroyCache(CTextureCache* pCache)
{
	mAllocCache.freePointer((CFontTextureCache*)pCache);
}
//--------------------------------------------------------------