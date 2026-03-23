/**
 * \file      TextureGroup.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2005-07-05 12:29:19
 * \brief     纹理Group，多张TextureCache为一组，存放同一类型(尺寸相近)的纹理
 * 
 *	      纹理Group，多张TextureCache为一组，存放同一类型(尺寸相近)的纹理
 */

#pragma once

#include <vector>
#include "D3DBitmapList.h"

class CTextureCache;
class CTextureCell;
class CTextureAllocator;


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CTextureGroup: public SimpleNode
{
public:
	CTextureGroup(TextureUsageType usage,D3DFORMAT format,size_t texWidth,size_t texHeight,bool bRowMajor,bool bOnlyAddTail/*,CTextureAllocator* pAllocator*/);
	virtual ~CTextureGroup(void);
public:
	typedef std::vector<CTextureCache*> t_TextureCaches;
	typedef t_TextureCaches::iterator  t_TextureCachesIt;
protected:
	t_TextureCaches m_TextureCaches;
	D3DFORMAT	m_format;
	size_t		m_texWidth;
	size_t		m_texHeight;
	bool		m_bRowMajor;
	bool		m_bOnlyAddTail;
	DWORD		m_dwTime;
	TextureUsageType m_usage;
	
public:
	TextureUsageType GetUsage(){
		return m_usage;
	}

	void ResetFormat(D3DFORMAT fmt){ m_format = fmt; }

	virtual CTextureAllocator* GetAllocator() = 0;

	void CheckTexCache(CTextureCache* p)
	{
		for(t_TextureCachesIt it = m_TextureCaches.begin(); it != m_TextureCaches.end(); ++it)
		{
			if((*it) == p) return;
		}
		Assert(0);
	}
	D3DFORMAT	GetFormat(){ return m_format;}
	bool		IsRowMajor() { return m_bRowMajor;}
	bool		IsOnlyAddTail(){return m_bOnlyAddTail;}
	DWORD		GetLastTime(){ return m_dwTime; }
	void		UpdateTime(){ m_dwTime = xtimeGetTime();}
	void		OnRender();
	CTextureCell* CreateTextureInCache(size_t width, size_t height);
	CTextureCell* CreateTexture(size_t width,size_t height);
	CTextureCell* TryCreateTexture(size_t width,size_t height);
	virtual void Release(void);
	virtual CTextureCache* AddTextureCache(IDirect3DTexture8 * pTex) = 0;
	virtual void DestroyCache(CTextureCache* pCache) = 0;
};

class CMBmpTextureCell;
class CMBmpTextureCache;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CMBmpTextureGroup : public CTextureGroup
{
	static std::lookaside_allocator<CMBmpTextureCache> mAllocCache;
public:
	CMBmpTextureGroup(TextureUsageType usage,D3DFORMAT format,size_t texWidth,size_t texHeight,bool bRowMajor,bool bOnlyAddTail/*,CTextureAllocator* pAllocator*/)
		: CTextureGroup(usage,format,texWidth,texHeight,bRowMajor,bOnlyAddTail/*,CTextureAllocator* pAllocator*/)
	{}
	~CMBmpTextureGroup();
	CTextureAllocator* GetAllocator();
	CTextureCache* AddTextureCache(IDirect3DTexture8 * pTex);
	void OnRender(CMBmpTextureCell* pCell);
	void DestroyCache(CTextureCache* pCache);
};

class CBmpTextureCache;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CBmpTextureGroup : public CTextureGroup
{
	static std::lookaside_allocator<CBmpTextureCache> mAllocCache;
public:
	typedef std::vector<CD3DBitmapList*> t_TextureBitmapLists;
	typedef t_TextureBitmapLists::iterator  t_TextureBitmapListsIt;
protected:
	t_TextureBitmapLists m_BmpLists;
public:
	CBmpTextureGroup(TextureUsageType usage,D3DFORMAT format,size_t texWidth,size_t texHeight,bool bRowMajor,bool bOnlyAddTail/*,CTextureAllocator* pAllocator*/)
		: CTextureGroup(usage,format,texWidth,texHeight,bRowMajor,bOnlyAddTail/*,CTextureAllocator* pAllocator*/)
	{
		m_BmpLists.reserve(16);
	}
	~CBmpTextureGroup();

	void AddBmpLists(CD3DBitmapList* p)
	{
		Assert(p->m_pTexGroup == NULL);
		Assert(FindBmpList(p) == m_BmpLists.end());
		m_BmpLists.push_back(p);
	}

	void DeleteBmpLists(CD3DBitmapList* p)
	{
		for(size_t i=0;i<m_BmpLists.size();++i)
		{
			if(m_BmpLists[i] == p)
			{
				SimpleRemoveFromArray(m_BmpLists,i);
				return;
			}
		}
	}

	t_TextureBitmapListsIt FindBmpList(CD3DBitmapList* p)
	{
		for(t_TextureBitmapListsIt it = m_BmpLists.begin(); it != m_BmpLists.end(); ++it)
		{
			if((*it) == p) return it;
		}
		return m_BmpLists.end();
	}

	void CheckUsage(TextureUsageType usage)
	{
		for(t_TextureBitmapListsIt it = m_BmpLists.begin(); it != m_BmpLists.end(); ++it)
		{
			CD3DBitmapList* p = *it;
			Assert(usage == p->GetUsage());
		}
	}

	CTextureAllocator* GetAllocator();

	CTextureCache* AddTextureCache(IDirect3DTexture8 * pTex);
	void DestroyCache(CTextureCache* pCache);
	void OnRender(CBmpTextureCell* p);
	void Release(void);
};

class CFontTextureCell;
class CFontTextureCache;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CFontTextureGroup : public CTextureGroup
{
	static std::lookaside_allocator<CFontTextureCache,4> mAllocCache;
	int m_iFont;
public:
	CFontTextureGroup(TextureUsageType usage,D3DFORMAT format,size_t texWidth,size_t texHeight,bool bRowMajor,bool bOnlyAddTail/*,CTextureAllocator* pAllocator*/,int iFont)
		: CTextureGroup(usage,format,texWidth,texHeight,bRowMajor,bOnlyAddTail/*,CTextureAllocator* pAllocator*/)
		, m_iFont(iFont)
	{}
	~CFontTextureGroup();

	CTextureAllocator* GetAllocator();
	CTextureCache* AddTextureCache(IDirect3DTexture8 * pTex);
	void DestroyCache(CTextureCache* pCache);
	void OnRender(CFontTextureCell* pCell);
};