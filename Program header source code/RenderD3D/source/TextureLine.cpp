/**
 * \file      TextureLine.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2005-07-05 12:29:20
 * \brief     ÎÆÀíLine£¬TextureCache ÉÏ»®³öµÄÒ»ĞĞ
 * 
 *	      ÎÆÀíLine£¬TextureCache ÉÏ»®³öµÄÒ»ĞĞ
 */

#include "../../engine/include/engine.h"
#include "../include/texturecell.h"
#include "../include/textureline.h"
#include "../include/texturecache.h" 
 
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureLine::CTextureLine(CTextureCache* pTexCache,size_t begin,size_t end,bool bIsRow,bool bOnlyAddTail )
: m_pTexCache(pTexCache)
, m_begin(begin)
, m_end(end)
, m_bIsRow(bIsRow)
, m_bOnlyAddTail(bOnlyAddTail)
, m_bFree(true)
{
	m_TextureCells.reserve(16);
	Assert(end <= 256);
	Assert(begin <= 256);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureLine::~CTextureLine(void)
{
}

void	CTextureLine::Release()
{
	for(size_t i=0;i<m_TextureCells.size();++i)
	{
		DestroyCell(m_TextureCells[i]);
		//delete m_TextureCells[i];
	}
	m_TextureCells.clear();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pTexCell : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CTextureLine::PopTexture(CTextureCell* pTexCell)
{
	t_TexureCellsIt it = FindTexture(pTexCell);
	Assert(it != m_TextureCells.end());
	if(it == m_TextureCells.end()) return false;
	
	t_TexureCellsIt it0,it1;
	
	it1 = it;
	++it1;
	
	CTextureCell* p0 = NULL;
	
	if(it1 != m_TextureCells.end())
	{
		if((*it1)->IsFree())
		{
			p0 = (*it1);
			p0->SetBegin((*it)->GetBegin());
			DestroyCell(*it);
			//delete (*it);
			it = m_TextureCells.erase(it);
		}
	}

	if(p0 == NULL)
	{
		(*it)->SetFree(true);
		p0 = (*it);
	}

	if(it != m_TextureCells.begin())
	{
		it0 = --it;
	}
	else
	{
		it0 = m_TextureCells.end();
	}

	if(it0 != m_TextureCells.end())
	{
		if((*it0)->IsFree())
		{
			p0->SetBegin((*it0)->GetBegin());
			//delete (*it0);
			DestroyCell(*it0);
			m_TextureCells.erase(it0);
		}
	}

	return true;
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
CTextureCell * CTextureLine::PushTexture(size_t width,size_t height)
{
	t_TexureCellsIt it = PrepareTexture(width,height);
	if(it == m_TextureCells.end()) return NULL;

	CTextureCell* p = (*it);
	p->SetFree(false);
	if(m_bIsRow)
	{
		p->SetBottom(GetBegin() + height);
		size_t freeSize = p->GetWidth() - width;
		if(freeSize)
		{
			p->SetRight(p->GetLeft() + width);

			stRectI rc(p->GetRight(),GetBegin(),p->GetRight() + freeSize,GetEnd());
			CTextureCell* pFree = CreateCell(&rc);

			t_TexureCellsIt it0 = ++it;
			m_TextureCells.insert(it0,pFree);
		}
	}
	else
	{
		p->SetRight(GetBegin() + width);
		size_t freeSize = p->GetHeight() - height;
		if(freeSize)
		{
			p->SetBottom(p->GetTop() + height);

			stRectI rc(GetBegin(),p->GetBottom(),GetEnd(),p->GetBottom() + freeSize);
			CTextureCell* pFree = CreateCell(&rc);

			t_TexureCellsIt it0 = ++it;
			m_TextureCells.insert(it0,pFree);
		}
	}
	return p;
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
CTextureLine::t_TexureCellsIt CTextureLine::PrepareTexture(size_t width,size_t height)
{
	Assert(!m_TextureCells.empty());
	if(m_TextureCells.empty()) return m_TextureCells.end();

	if(m_bIsRow)
	{
		if (GetSize() < height)
			return m_TextureCells.end();
		
		t_TexureCellsIt it;
		it = (m_bOnlyAddTail ? (--m_TextureCells.end()) : m_TextureCells.begin());

		for(;it != m_TextureCells.end();++it)
		{
			CTextureCell* p = (*it);
			if(!p->IsFree()) continue;
			if((size_t)p->GetWidth() >= width)
				return it;
		}
	}
	else
	{
		if (GetSize() < width)
			return m_TextureCells.end();
		
		t_TexureCellsIt it;
		it = (m_bOnlyAddTail ? (--m_TextureCells.end()) : m_TextureCells.begin());

		for(;it != m_TextureCells.end();++it)
		{
			CTextureCell* p = (*it);
			if(!p->IsFree()) continue;
			if((size_t)p->GetHeight() >= height)
				return it;
		}
	}

	return m_TextureCells.end();
}

std::lookaside_allocator<CMBmpTextureCell> CMBmpTextureLine::mAllocTexCell;
std::lookaside_allocator<CBmpTextureCell> CBmpTextureLine::mAllocTexCell;
std::lookaside_allocator<CFontTextureCell> CFontTextureLine::mAllocTexCell;
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//----------------------------------------------------------
// CMBmpTextureLine
//----------------------------------------------------------
CMBmpTextureLine::CMBmpTextureLine(CTextureCache* pTexCache,size_t begin,size_t end,bool bIsRow,bool bOnlyAddTail)
: CTextureLine(pTexCache, begin, end, bIsRow,bOnlyAddTail)
{
	if(bIsRow)
	{
		stRectI rc(0,begin,pTexCache->GetWidth(),end);
		m_TextureCells.push_back(CreateCell(&rc));
	}
	else
	{
		stRectI rc(begin,0,end,pTexCache->GetHeight());
		m_TextureCells.push_back(CreateCell(&rc));
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param stRectI* : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureCell* CMBmpTextureLine::CreateCell(const stRectI* prc)
{
	return LOOKASIDE_PALLOC2(mAllocTexCell,this,prc);
	//return new CMBmpTextureCell(this,prc);
}

void CMBmpTextureLine::DestroyCell(CTextureCell* pCell)
{
	mAllocTexCell.freePointer((CMBmpTextureCell*)pCell);
}
//---------------------------------------------------------------


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//----------------------------------------------------------
// CBmpTextureLine
//----------------------------------------------------------
CBmpTextureLine::CBmpTextureLine(CTextureCache* pTexCache,size_t begin,size_t end,bool bIsRow,bool bOnlyAddTail)
		: CTextureLine(pTexCache, begin, end, bIsRow,bOnlyAddTail)
{
	if(bIsRow)
	{
		stRectI rc(0,begin,pTexCache->GetWidth(),end);
		m_TextureCells.push_back(CreateCell(&rc));
	}
	else
	{
		stRectI rc(begin,0,end,pTexCache->GetHeight());
		m_TextureCells.push_back(CreateCell(&rc));
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param stRectI* : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureCell* CBmpTextureLine::CreateCell(const stRectI* prc)
{
	return LOOKASIDE_PALLOC2(mAllocTexCell,this,prc);
	//return new CBmpTextureCell(this,prc);
}

void CBmpTextureLine::DestroyCell(CTextureCell* pCell)
{
	mAllocTexCell.freePointer((CBmpTextureCell*)pCell);
}
//---------------------------------------------------------------

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//---------------------------------------------------------------
// CFontTextureLine
//---------------------------------------------------------------
CFontTextureLine::CFontTextureLine(CTextureCache* pTexCache,size_t begin,size_t end,bool bIsRow,bool bOnlyAddTail)
		: CTextureLine(pTexCache, begin, end, bIsRow,bOnlyAddTail)
{
	if(bIsRow)
	{
		stRectI rc(0,begin,pTexCache->GetWidth(),end);
		m_TextureCells.push_back(CreateCell(&rc));
	}
	else
	{
		stRectI rc(begin,0,end,pTexCache->GetHeight());
		m_TextureCells.push_back(CreateCell(&rc));
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param stRectI* : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureCell* CFontTextureLine::CreateCell(const stRectI* prc)
{
	return LOOKASIDE_PALLOC2(mAllocTexCell,this,prc);
	//return new CFontTextureCell(this,prc);
}

void CFontTextureLine::DestroyCell(CTextureCell* pCell)
{
	mAllocTexCell.freePointer((CFontTextureCell*)pCell);
}
//----------------------------------------------------------------
//*/