/**
 * \file      TextureCell.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2005-07-05 12:29:19
 * \brief     纹理Cell，一张大纹理上的小纹理对象
 * 
 *	      纹理Cell，一张大纹理上的小纹理对象
 */

#pragma once

#include "TextureLine.h"
#include "D3DBitmap.h"
#include "TextureCache.h"
#include "D3DMBitmap.h"
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CTextureCell : public SimpleNode
{
	friend class CTextureLine;
public:
	CTextureCell(CTextureLine* pTexLine,const stRectI* prc);
protected:
	stRectI m_rcTex;
	CTextureLine * m_pTexLine;
	bool	m_bFree;
	virtual ~CTextureCell(void);

public:

	virtual DWORD	GetLastTime() = 0;

	virtual void ReleaseTexture();

	CTextureLine* GetTexLine(){ return m_pTexLine; }
	CTextureCache* GetTexCache(){ return m_pTexLine->GetTexCache();}

	void SetFree(bool b) {m_bFree = b;}
	bool IsFree(){ return m_bFree;}
	const stRectI & GetTexRect(){ return m_rcTex;}

	int GetWidth(){ return m_rcTex.Width();}
	int GetHeight(){ return m_rcTex.Height();}

	void SetLeft(int l) { m_rcTex.left = l; }
	void SetTop(int t) { m_rcTex.top = t; }
	void SetRight(int r) { m_rcTex.right = r; }
	void SetBottom(int b){ m_rcTex.bottom = b; }
	
	int GetLeft(){ return m_rcTex.left;}
	int GetTop(){ return m_rcTex.top;}
	int GetRight(){ return m_rcTex.right;}
	int GetBottom(){ return m_rcTex.bottom;}

	int GetBegin(){ return (m_pTexLine->IsRow() ? m_rcTex.left : m_rcTex.top); }
	int GetEnd(){ return (m_pTexLine->IsRow() ? m_rcTex.right : m_rcTex.bottom); }

	void SetBegin(int begin)
	{
		if(m_pTexLine->IsRow()) m_rcTex.left = begin; 
		else m_rcTex.top = begin;
	}

	void SetEnd(int end) 
	{ 
		if(m_pTexLine->IsRow()) m_rcTex.right = end; 
		else m_rcTex.bottom = end;
	}

	int GetSize() { return (m_pTexLine->IsRow() ? m_rcTex.Width() : m_rcTex.Height()); }
};
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CMBmpTextureCell : public CTextureCell
{
protected:
	CD3DMBitmap* m_pBmp;
public:
	CMBmpTextureCell(CTextureLine* pTexLine,const stRectI* prc)
		: CTextureCell(pTexLine,prc)
		, m_pBmp(NULL)
	{}
	~CMBmpTextureCell();

	void SetBitmap(CD3DMBitmap* p)
	{
		m_pBmp = p;
	}

	CD3DMBitmap* GetBitmap(){ return m_pBmp; }

	DWORD	GetLastTime()
	{
		Assert(m_pBmp);
		return m_pBmp->GetLastRenderTime();
	}

	void ReleaseTexture();
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CBmpTextureCell : public CTextureCell
{
protected:
	CD3DBitmap* m_pBmp;
	DWORD	m_dwBlock;
public:
	CBmpTextureCell(CTextureLine* pTexLine,const stRectI* prc)
		: CTextureCell(pTexLine,prc)
		, m_pBmp(NULL)
	{}

	~CBmpTextureCell();

	void SetBitmap(CD3DBitmap* p,DWORD dwBlock)
	{
		m_pBmp = p;
		m_dwBlock = dwBlock;
	}

	CD3DBitmap* GetBitmap(){ return m_pBmp; }

	DWORD	GetLastTime()
	{
		Assert(m_pBmp);
		return m_pBmp->GetLastRenderTime();
	}

	void ReleaseTexture();
};

class CD3DFont;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CFontTextureCell : public CTextureCell
{
protected:
	DWORD m_dwLastTime;
	WORD  m_char;
	CD3DFont* m_pFont;
public:
	CFontTextureCell(CTextureLine* pTexLine,const stRectI* prc)
		: CTextureCell(pTexLine,prc)
		, m_dwLastTime(0)
		, m_pFont(NULL)
	{}
	
	~CFontTextureCell();

	DWORD	GetLastTime()
	{
		return m_dwLastTime;
	}

	void	SetLastTime(DWORD time)
	{
		m_dwLastTime = time;
	}

	void SetFontData(CD3DFont* pFont,WORD c)
	{
		m_pFont = pFont;
		m_char = c;
	}

	void ReleaseTexture();
};