/**
 * \file      TextureCache.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2005-07-05 12:29:19
 * \brief     纹理Cache，一张大纹理上放多个小纹理
 * 
 *	      纹理Cache，一张大纹理上放多个小纹理
 */

#pragma once

#include <vector>

class CTextureCell;
class CTextureLine;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CTextureCache
{
public:
	CTextureCache(IDirect3DTexture8 * pTex,size_t width,size_t height,bool bRowMajor,bool bOnlyAddTail,size_t minHeight = 0);
	virtual ~CTextureCache(void);

	typedef std::vector<CTextureLine*> t_TextureLines;
	typedef t_TextureLines::iterator t_TextureLinesIt;
protected:
	IDirect3DTexture8*	m_pTex;
	size_t m_width;
	size_t m_height;
	size_t m_minHeight;
	bool	m_bOnlyAddTail;
	bool	m_bRowMajor;
	t_TextureLines m_TextureLines;
	virtual CTextureLine* CreateLine(size_t begin, size_t end)  = 0;
	virtual	void DestroyLine(CTextureLine* p) = 0;
	void Release();
public:
	IDirect3DTexture8* GetTex(){ return m_pTex; }
	int GetWidth(){ return m_width;}
	int GetHeight(){ return m_height;}
	CTextureCell* PushTexture(size_t width, size_t height, int diff);
	CTextureCell* PushTextureInLine(size_t width , size_t height , int diff);
	CTextureLine* NewLine(size_t size);
	
};

class CMBmpTextureLine;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CMBmpTextureCache : public CTextureCache
{
	static std::lookaside_allocator<CMBmpTextureLine> mAllocLine;
public:
	CMBmpTextureCache(IDirect3DTexture8 * pTex,size_t width,size_t height,bool bRowMajor,bool bOnlyAddTail,size_t minHeight = 0)
		: CTextureCache(pTex,width,height,bRowMajor,bOnlyAddTail,minHeight)
	{}
	~CMBmpTextureCache();
	CTextureLine* CreateLine(size_t begin, size_t end);
	void DestroyLine(CTextureLine* p);
};

class CBmpTextureLine;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CBmpTextureCache : public CTextureCache
{
	static std::lookaside_allocator<CBmpTextureLine> mAllocLine;
public:
	CBmpTextureCache(IDirect3DTexture8 * pTex,size_t width,size_t height,bool bRowMajor,bool bOnlyAddTail,size_t minHeight = 0)
		: CTextureCache(pTex,width,height,bRowMajor,bOnlyAddTail,minHeight)
		{}
	~CBmpTextureCache();
	CTextureLine* CreateLine(size_t begin, size_t end);
	void DestroyLine(CTextureLine* p);
};

class CFontTextureLine;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CFontTextureCache : public CTextureCache
{
	static std::lookaside_allocator<CFontTextureLine> mAllocLine;
public:
	CFontTextureCache(IDirect3DTexture8 * pTex,size_t width,size_t height,bool bRowMajor,bool bOnlyAddTail,size_t minHeight = 0)
		: CTextureCache(pTex,width,height,bRowMajor,bOnlyAddTail,minHeight)
		{}
	~CFontTextureCache();
	CTextureLine* CreateLine(size_t begin, size_t end);
	void DestroyLine(CTextureLine* p);
};