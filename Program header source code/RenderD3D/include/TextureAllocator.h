/**
 * \file      TextureAllocator.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2005-07-05 12:29:19
 * \brief     纹理分配器
 * 
 *	      纹理分配器
 */

#pragma once

enum TextureManagerFlag{
	TextureFlag_OnlyAddTail =  BIT(0),
	TextureFlag_ColMajor =  BIT(1),
	TextureFlag_GroupRelease = BIT(2),
};



class CTextureCache;
class CTextureCell;
class CTextureGroup;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CTextureAllocator
{
public:
	typedef std::vector<IDirect3DTexture8*> t_Textures;
	typedef t_Textures::iterator t_TexturesIt;

	enum TextureType{
		Type_Terrain = 0,
		Type_Object,
		Type_Scene,
		Type_Font,
		Type_Font2,
		Type_Font3,
		Type_Font4,
		Type_Font5,
		Type_NoAlpha,
		Type_Num,
	};

public:
	CTextureAllocator(void);
	~CTextureAllocator(void);
	
protected:
	t_Textures m_UseTextures;
	t_Textures m_FreeTextures;

	SimpleList m_TextureGroups;
	SimpleList m_FreeTextureGroups;
	SimpleList m_TextureCells;

	DWORD		m_dwFlags;
	TextureType m_texType;

	IDirect3DTexture8* DeleteATexture(t_Textures & v,size_t index)
	{
		if(v.empty()) return NULL;
		IDirect3DTexture8* pTex = v[index];
		if(index != v.size() - 1)
		{
			v[index] = v.back();
		}
		v.erase(--v.end());
		return pTex;
	}

	IDirect3DTexture8* UseATexture(DWORD format);

public:
	void ResetFormat();

	void CheckTexture(IDirect3DTexture8* pTex)
	{
		for(size_t i = 0; i < m_UseTextures.size(); ++i)
		{
			if(m_UseTextures[i] == pTex)
			{
				return;
			}
		}
		Assert(0);
	}
	void ReverseCheckTexture(IDirect3DTexture8* pTex)
	{
		for(size_t i = 0; i < m_UseTextures.size(); ++i)
		{
			if(m_UseTextures[i] == pTex)
			{
				Assert(0);
			}
		}
		for(size_t i = 0; i < m_FreeTextures.size(); ++i)
		{
			if(m_FreeTextures[i] == pTex)
			{
				Assert(0);
			}
		}
	}
	void SetType(TextureType type){ m_texType = type;}
	CTextureCache* AllocTexture(CTextureGroup* pGroup,size_t width,size_t height);
	CTextureCache* ReleaseCellByTime(DWORD dwTime,size_t width,size_t height);
	void ReleaseGroupByTime(DWORD dwTime,CTextureGroup* pNoDelete = NULL);
	void ReleaseAll(CTextureGroup* pNoDelete = NULL);
	
	void AddTextureGroup(CTextureGroup*);
	void AddTextureCell(CTextureCell*);
	void UpdateTextureCell(CTextureCell*);
	void AddTexture(IDirect3DTexture8 * pTex)
	{
		m_FreeTextures.push_back(pTex);
	}
	void UpdateTextureGroup(CTextureGroup * p);
	void ReleaseATexture(IDirect3DTexture8* pTex);
	void ReleaseAllTexture()
	{
		for(size_t i = 0; i < m_UseTextures.size(); ++i)
		{
			SAFE_RELEASE(m_UseTextures[i]);
		}
		m_UseTextures.resize(0);
		for(size_t i = 0; i < m_FreeTextures.size(); ++i)
		{
			SAFE_RELEASE(m_FreeTextures[i]);
		}
		m_FreeTextures.resize(0);
	}

	void Destroy();
	void ReleaseATextureCell(CTextureCell* pCell);
	CTextureCache* TryAllocTexture(CTextureGroup* pGroup,size_t width,size_t height);
};