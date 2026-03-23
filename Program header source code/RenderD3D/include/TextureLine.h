/**
 * \file      TextureLine.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2005-07-05 12:29:19
 * \brief     纹理Line，TextureCache 上划出的一行
 * 
 *	      纹理Line，TextureCache 上划出的一行
 */

#pragma once

#include <vector>

#include ".\texturecell.h"

class CTextureCache;
class CTextureCell;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CTextureLine
{
public:
	CTextureLine(CTextureCache* pTexCache,size_t begin,size_t end,bool bIsRow = true,bool bOnlyAddTail = true );
	virtual ~CTextureLine(void);
public:
	typedef std::vector<CTextureCell*> t_TexureCells;
	typedef t_TexureCells::iterator t_TexureCellsIt;
protected:
	CTextureCache* m_pTexCache;
	t_TexureCells  m_TextureCells;
	size_t			m_begin;
	size_t			m_end;
	bool			m_bOnlyAddTail;
	bool			m_bIsRow;
	bool			m_bFree;
	t_TexureCellsIt PrepareTexture(size_t width,size_t height);
	void	Release();
public:
	
	virtual CTextureCell* CreateCell(const stRectI* prc) = 0;
	virtual void DestroyCell(CTextureCell* pCell) = 0;

	bool PopTexture(CTextureCell* pTexCell);

	bool GetFree(){ return m_bFree;}
	void SetFree(bool b) { m_bFree = b; }
	bool IsRow(){ return m_bIsRow; }
	CTextureCell * PushTexture(size_t width,size_t height);
	t_TexureCellsIt FindTexture(CTextureCell* pTexCell)
	{
		for(t_TexureCellsIt it = m_TextureCells.begin(); it != m_TextureCells.end(); ++it)
		{
			if((*it) == pTexCell) return it;
		}
		return m_TextureCells.end();
	}

	size_t			GetSize() { return m_end - m_begin;}
	void			SetSize(size_t height){ m_end = m_begin + height;}

	size_t			GetEnd() { return m_end;}
	size_t			GetBegin()	{ return m_begin;}

	CTextureCache*	GetTexCache(){ return m_pTexCache;}
	
};

class CMBmpTextureCell;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CMBmpTextureLine : public CTextureLine
{
	static std::lookaside_allocator<CMBmpTextureCell> mAllocTexCell;
public:
	CMBmpTextureLine(CTextureCache* pTexCache,size_t begin,size_t end,bool bIsRow = true,bool bOnlyAddTail = true );
	CTextureCell* CreateCell(const stRectI* prc);
	void DestroyCell(CTextureCell* pCell);
	~CMBmpTextureLine(){ Release();}
};

class CBmpTextureCell;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CBmpTextureLine : public CTextureLine
{
	static std::lookaside_allocator<CBmpTextureCell> mAllocTexCell;
public:
	CBmpTextureLine(CTextureCache* pTexCache,size_t begin,size_t end,bool bIsRow = true,bool bOnlyAddTail = true );
	CTextureCell* CreateCell(const stRectI* prc);
	void DestroyCell(CTextureCell* pCell);
	~CBmpTextureLine(){ Release();}
};

class CFontTextureCell;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CFontTextureLine : public CTextureLine
{
	static std::lookaside_allocator<CFontTextureCell> mAllocTexCell;
public:
	CFontTextureLine(CTextureCache* pTexCache,size_t begin,size_t end,bool bIsRow = true,bool bOnlyAddTail = true );
	CTextureCell* CreateCell(const stRectI* prc);
	void DestroyCell(CTextureCell* pCell);
	~CFontTextureLine(){ Release();}
};