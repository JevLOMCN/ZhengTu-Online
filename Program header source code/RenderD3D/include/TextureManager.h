/**
 * \file      TextureManager.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2005-07-05 12:29:19
 * \brief     纹理管理类
 * 
 *	      纹理管理类
 */

#pragma once


#include "./TextureAllocator.h"
#include "D3DBitmap.h"

class CD3DBitmapList;
class CTextureGroup;
class CBmpTextureGroup;
class CFontTextureGroup;
class CD3DFont;
class CMBmpTextureGroup;
class CD3DMBitmapManager;
class CCustomTexture;
class CRenderTargetTexture;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CTextureManager
{
	CTextureAllocator m_allocators[CTextureAllocator::Type_Num];
	typedef std::vector<CCustomTexture*> tListCustomTexture;
	tListCustomTexture m_CustomTextures;
public:
	CTextureAllocator* GetFontTextureAllocator(int iFont){ return &m_allocators[CTextureAllocator::Type_Font + iFont];}

	CCustomTexture* CreateCurstomTexture(int width,int height,D3DFORMAT format,DWORD dwUsage = 0);
	IDirect3DTexture8* CreateTexture(int width,int height,D3DFORMAT format,DWORD dwUsage);
	CTextureAllocator* GetTextureAllocator(CTextureAllocator::TextureType type)
	{
		return & m_allocators[type];
	}

	CTextureGroup* m_texturegroups[Usage_Num];
	void DeleteTextureGroup(CTextureGroup* p)
	{
		for(size_t i = 0;i<Usage_Num;++i)
		{
			if(m_texturegroups[i] == p)
			{
				m_texturegroups[i] = NULL;
				break;
			}
		}
	}
	void DeleteTextureGroup(CTextureAllocator::TextureType type,CTextureGroup* pNoDelete)
	{
		switch(type)
		{
		case CTextureAllocator::Type_Terrain:
			
			if(pNoDelete != m_texturegroups[Usage_Terrain])
				m_texturegroups[Usage_Terrain] = NULL;
			break;

		case CTextureAllocator::Type_Object:

			if(pNoDelete != m_texturegroups[Usage_Building])
				m_texturegroups[Usage_Building] = NULL;

			if(pNoDelete != m_texturegroups[Usage_Interface])
				m_texturegroups[Usage_Interface] = NULL;

			if(pNoDelete != m_texturegroups[Usage_SmallObject])
				m_texturegroups[Usage_SmallObject] = NULL;

			if(pNoDelete != m_texturegroups[Usage_TerrainSmall])
				m_texturegroups[Usage_TerrainSmall] = NULL;

			break;
		case CTextureAllocator::Type_Scene:
			break;
		case CTextureAllocator::Type_NoAlpha:
			if(pNoDelete != m_texturegroups[Usage_NoAlpha])
				m_texturegroups[Usage_NoAlpha] = NULL;
			break;
		}
	}
	D3DFORMAT GetTextureFormat(CD3DBitmapList* bmpList);
	D3DFORMAT GetFontFormat();

	CBmpTextureGroup* CreateTextureGroup(CD3DBitmapList* bmpList);
	CFontTextureGroup* CreateTextureGroup(CD3DFont* font,int iFont);

	CTextureAllocator* GetTextureAllocator(CD3DBitmapList* bmpList);
	SIZE GetTextureSize(CD3DBitmapList* bmpList);
	bool GetIsRowMajor(CD3DBitmapList* bmpList);
	bool GetIsAddTail(CD3DBitmapList* bmpList);

	CTextureAllocator* GetTextureAllocator(TextureUsageType usage);
	D3DFORMAT GetTextureFormat(TextureUsageType usage);
	SIZE GetTextureSize(TextureUsageType usage);
	bool GetIsRowMajor(TextureUsageType usage);
	bool GetIsAddTail(TextureUsageType usage);

	bool Init();
	void ReleaseAllTexture();

	size_t m_totalTextureMem;
	size_t m_useTextureMem;
	size_t m_trueUseTextureMem;
	
	size_t GetTextureMemSize(IDirect3DTexture8 * pTex,size_t width = -1,size_t height = -1)
	{
		D3DSURFACE_DESC desc;
		HRESULT hr = pTex->GetLevelDesc(0,&desc);
		if(width == -1 || height == -1)
			return desc.Size;
		return (width * height) * desc.Size /(desc.Width * desc.Height);
	}

	void UpdateuseTextureMemSize(long addTexMemSize)
	{
		m_useTextureMem = (long)m_useTextureMem + addTexMemSize;
	}
	size_t GetTotalTextureMemSize(){ return m_totalTextureMem;	}
	size_t GetUseTextureMemSize()  { return m_useTextureMem;	}

	void Destroy();
	CMBmpTextureGroup* CreateTextureGroup(CD3DMBitmapManager* pMg);
	
	// 释放所有对纹理的占有
	void ReleaseAll();
	// 释放指定类型的纹理
	void ReleaseTextureType(CTextureAllocator::TextureType texType);

	void ReleaseAllCustomTexture();

	void DestroyAllCustomTexture();
	
	void ResetTextureFormat();

	void DeleteCustomeTexture(CCustomTexture* pTex);
	CRenderTargetTexture* CreateRenderTargetTexture(int width,int height,D3DFORMAT format);
};