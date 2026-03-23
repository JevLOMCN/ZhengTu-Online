/**
 * \file      TextureCache.cpp
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2005-07-05 12:29:19
 * \brief     纹理Cache，一张大纹理上放多个小纹理
 * 
 *	      纹理Cache，一张大纹理上放多个小纹理
 */

#include "../../engine/include/engine.h"
#include "../include/texturecache.h"
#include "../include/Texturecell.h"
#include "../include/textureline.h"
#include "../Include/RenderD3D.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CTextureCache::CTextureCache(IDirect3DTexture8 * pTex,size_t width,size_t height,bool bRowMajor,bool bOnlyAddTail,size_t minHeight)
: m_pTex(pTex)
, m_width(width)
, m_height(height)
, m_bRowMajor(bRowMajor)
, m_bOnlyAddTail(bOnlyAddTail)
, m_minHeight(minHeight)
{
	m_TextureLines.reserve(16);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CTextureCache::~CTextureCache(void)
{
}

void CTextureCache::Release()
{
	for(t_TextureLinesIt it = m_TextureLines.begin(); it != m_TextureLines.end(); ++it)
	{
		DestroyLine(*it);
		//delete (*it);
	}
}
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param width : 描述
 * \param height : 描述
 * \param diff : 描述
 * \return 返回值的描述
 */
CTextureCell* CTextureCache::PushTexture(size_t width, size_t height, int diff)
{

	Assert( width <= GetWidth() && height <= GetHeight() );

	CTextureCell* pVideo;
	pVideo = PushTextureInLine( width , height , diff );
	if (pVideo)	return pVideo;

	CTextureLine* p;
	
	if(m_bRowMajor) p = NewLine(height < m_minHeight ? m_minHeight : height);
	else			p = NewLine(width < m_minHeight ? m_minHeight : width);

	if (p != NULL)
	{
		pVideo = p->PushTexture(width,height);
		return pVideo;
	}
	else
	{
		//--- 扩充最后一行高度 --------
		if(!m_TextureLines.empty())
		{
			CTextureLine* p = m_TextureLines.back();
			if(m_bRowMajor)
			{
				if(p->GetSize() < height && p->GetBegin() + height <= GetHeight())
				{
					size_t size0 = p->GetSize();
					p->SetSize(height);
					pVideo = p->PushTexture(width,height);
					if(pVideo) return pVideo;
					p->SetSize(size0);
				}
			}
			else
			{
				if(p->GetSize() < width && p->GetBegin() + width <= GetWidth())
				{
					size_t size0 = p->GetSize();
					p->SetSize(width);
					pVideo = p->PushTexture(width,height);
					if(pVideo) return pVideo;
					p->SetSize(size0);
				}
			}
		}
		//------------------------------
	}
	return NULL;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param width : 描述
 * \param height : 描述
 * \param diff : 描述
 * \return 返回值的描述
 */
CTextureCell* CTextureCache::PushTextureInLine(size_t width , size_t height , int diff)
{
	if(m_bRowMajor)
	{
		for(t_TextureLinesIt it = m_TextureLines.begin(); it != m_TextureLines.end(); ++it)
		{
			CTextureLine* pLine = *(it);
			int diffNow = ((int)pLine->GetSize() - (int)height);
			if (diffNow >= 0 && diffNow <= diff)
			{
				CTextureCell* p = pLine->PushTexture(width,height);
				if (p)	return p;
			}
		}
	}
	else
	{
		for(t_TextureLinesIt it = m_TextureLines.begin(); it != m_TextureLines.end(); ++it)
		{
			CTextureLine* pLine = *(it);
			int diffNow = ((int)pLine->GetSize() - (int)width);
			if (diffNow >= 0 && diffNow <= diff)
			{
				CTextureCell* p = pLine->PushTexture(width,height);
				if (p)	return p;
			}
		}
	}
	return NULL;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param size : 描述
 * \return 返回值的描述
 */
CTextureLine* CTextureCache::NewLine(size_t size)
{
	if(m_bRowMajor)
	{
		size_t endHeight = 0;
		if(!m_TextureLines.empty())
			endHeight = m_TextureLines.back()->GetEnd();
		if(GetHeight() - endHeight >= size)
		{
			return CreateLine(endHeight,endHeight + size);
		}
	}
	else
	{
		size_t endWidth = 0;
		if(!m_TextureLines.empty())
			endWidth = m_TextureLines.back()->GetEnd();
		if(GetWidth() - endWidth >= size)
		{
			return CreateLine(endWidth,endWidth + size);
		}
	}
	return NULL;
}

std::lookaside_allocator<CMBmpTextureLine> CMBmpTextureCache::mAllocLine;
std::lookaside_allocator<CBmpTextureLine> CBmpTextureCache::mAllocLine;
std::lookaside_allocator<CFontTextureLine> CFontTextureCache::mAllocLine;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param begin : 描述
 * \param end : 描述
 * \return 返回值的描述
 */
//------------------------------------------------------------------
// CMBmpTextureCache
//------------------------------------------------------------------
CMBmpTextureCache::~CMBmpTextureCache()
{ 
	Release();
}

CTextureLine* CMBmpTextureCache::CreateLine(size_t begin, size_t end)
{
	CMBmpTextureLine* p = LOOKASIDE_PALLOC5(mAllocLine,this,begin,end,m_bRowMajor,m_bOnlyAddTail);
	//CMBmpTextureLine* p = new CMBmpTextureLine(this,begin,end,m_bRowMajor,m_bOnlyAddTail);
	if(p) m_TextureLines.push_back(p);
	return p;
}

void CMBmpTextureCache::DestroyLine(CTextureLine* p)
{
	mAllocLine.freePointer((CMBmpTextureLine*)p);
}
//-------------------------------------------------------------------

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param begin : 描述
 * \param end : 描述
 * \return 返回值的描述
 */
//-------------------------------------------------------------------
// CBmpTextureCache
//-------------------------------------------------------------------
CBmpTextureCache::~CBmpTextureCache()
{ 
	Release();
}

CTextureLine* CBmpTextureCache::CreateLine(size_t begin, size_t end)
{
	CBmpTextureLine* p = LOOKASIDE_PALLOC5(mAllocLine,this,begin,end,m_bRowMajor,m_bOnlyAddTail);
	//CBmpTextureLine* p = new CBmpTextureLine(this,begin,end,m_bRowMajor,m_bOnlyAddTail);
	if(p) m_TextureLines.push_back(p);
	return p;
}
void CBmpTextureCache::DestroyLine(CTextureLine* p)
{
	mAllocLine.freePointer((CBmpTextureLine*)p);
}
//-------------------------------------------------------------------

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param begin : 描述
 * \param end : 描述
 * \return 返回值的描述
 */
//-------------------------------------------------------------------
// CFontTextureCache
//-------------------------------------------------------------------
CFontTextureCache::~CFontTextureCache()
{ 
	Release();
}

CTextureLine* CFontTextureCache::CreateLine(size_t begin, size_t end)
{
	CFontTextureLine* p = LOOKASIDE_PALLOC5(mAllocLine,this,begin,end,m_bRowMajor,m_bOnlyAddTail);
	//CFontTextureLine* p = new CFontTextureLine(this,begin,end,m_bRowMajor,m_bOnlyAddTail);
	if(p) m_TextureLines.push_back(p);
	return p;
}
void CFontTextureCache::DestroyLine(CTextureLine* p)
{
	mAllocLine.freePointer((CFontTextureLine*)p);
}
//-------------------------------------------------------------------