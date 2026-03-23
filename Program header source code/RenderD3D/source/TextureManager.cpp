/**
 * \file      TextureManager.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2005-07-05 12:29:20
 * \brief     ÎÆÀí¹ÜÀíÀà
 * 
 *	      ÎÆÀí¹ÜÀíÀà
 */

#include "../../engine/include/engine.h"
#include "../include/d3ddevice.h"
#include "../include/texturemanager.h"
#include "../include/TextureGroup.h"
#include "../include/TextureCache.h"
#include "../include/D3DFont.h"
#include "../Include/RenderD3D.h"
#include "../Include/D3DMBitmap.h"
#include "../Include/CustomTexture.h"
#include "../include/D3DTextout.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CTextureManager::Init()
{
	{
		for(size_t i=0;i<count_of(m_allocators);++i)
			m_allocators[i].SetType(CTextureAllocator::TextureType(i));
	}

	memset(m_texturegroups,0,sizeof(m_texturegroups));

	CD3DDevice* pDevice = GetD3DDevice();
	
	m_useTextureMem = 0;
	m_trueUseTextureMem = 0;
	m_totalTextureMem = pDevice->GetDevice()->GetAvailableTextureMem();
	Engine_WriteLogF("AvailableTextureMem: %uM\n",m_totalTextureMem/(1024*1024));

	D3DFORMAT format = GetFontFormat();
	
	size_t i;
	size_t textureMem = 0;
	SIZE size = {256,256};
	size_t texNum;

	if(GetD3DDevice()->GetProgramType() != eProgram_ViewGL)
	{
		//--------------------------------------------
		// ×ÖÌåµÄÎÆÀí
		//--------------------------------------------
		int count = CD3DTextout::Font_Count * 2;
		i = 0;
		while( i < count)
		{
			if(c_fontInfo[ i / 2].bNoBorderFont && (i % 2) == 1) 
			{
				count--;
				continue;
			}

			IDirect3DTexture8* p = pDevice->CreateTexture(256,256,format);
			if(p) 
			{
				textureMem += GetTextureMemSize(p);
				if(textureMem > m_totalTextureMem) 
				{
					p->Release();
					break;
				}
				m_allocators[CTextureAllocator::Type_Font + i].AddTexture(p);
			}
			i++;
		}

		if(GetDevice()->IsRenderScene() || GetD3DDevice()->GetProgramType() == eProgram_MapEditor)
		{
			//----------------------------------------
			// µØ±íµÄÎÆÀí
			//----------------------------------------
			size = GetTextureSize(Usage_Terrain);
			format = GetTextureFormat(Usage_Terrain);
			size_t screenWidth = GetD3DDevice()->GetWidth();
			texNum = 16;
			if(screenWidth <= 800)
				texNum = 14;
			else if(screenWidth <= 1024)
				texNum = 16;
			else if(screenWidth <= 1280)
				texNum = 20;
			else
				texNum = 28;

			if(GetD3DDevice()->GetProgramType() == eProgram_MapEditor)
				texNum = 32;

			for( i=0;i<texNum;i++)
			{
				IDirect3DTexture8* p = pDevice->CreateTexture(size.cx,size.cy,format);
				if(p) 
				{
					textureMem += GetTextureMemSize(p);
					if(textureMem > m_totalTextureMem) 
					{
						p->Release();
						break;
					}
					m_allocators[CTextureAllocator::Type_Terrain].AddTexture(p);
				}
				else break;
			}
		}
	}

	//if(GetDevice()->IsRenderScene() || GetD3DDevice()->GetProgramType() != eProgram_Game)
	if(1)
	{
		//-------------------------------------
		// ½¨ÖşµÄÎÆÀí
		//-------------------------------------
		format = GetTextureFormat(Usage_Building);
		for(i=0;i<256;i++)
		{
			IDirect3DTexture8* p = pDevice->CreateTexture(size.cx,size.cy,format);
			if(p) 
			{
				textureMem += GetTextureMemSize(p);
				if(textureMem > m_totalTextureMem) 
				{
					p->Release();
					break;
				}
				m_allocators[CTextureAllocator::Type_Object].AddTexture(p);
			}
			else break;
		}
	}
	else
	{
		UINT nTextureMem = pDevice->GetDevice()->GetAvailableTextureMem();
		UINT nNoAlphaTexSize = nTextureMem * 80/100;
		UINT nAlphaTexSize = nTextureMem - nNoAlphaTexSize;

		int nNoAlphaTexNum,nAlphaTexNum;
		if(GetD3DDevice()->m_bUsableDXT1)
			nNoAlphaTexNum = nNoAlphaTexSize/(256 * 256 /2);
		else
			nNoAlphaTexNum = nNoAlphaTexSize/(256 * 256 * 2);

		if(GetD3DDevice()->m_bUsableDXT3)
			nAlphaTexNum = nAlphaTexSize/(256 * 256);
		else
			nAlphaTexNum = nAlphaTexSize/(256 * 256 * 2);

		if(nNoAlphaTexNum > 64) nNoAlphaTexNum = 64;
		if(nAlphaTexNum > 64) nAlphaTexNum = 64;

		format = GetTextureFormat(Usage_NoAlpha);
		for(i=0;i<nNoAlphaTexNum;i++)
		{
			IDirect3DTexture8* p = pDevice->CreateTexture(size.cx,size.cy,format);
			if(p) 
			{
				textureMem += GetTextureMemSize(p);
				if(textureMem > m_totalTextureMem) 
				{
					p->Release();
					break;
				}
				m_allocators[CTextureAllocator::Type_NoAlpha].AddTexture(p);
			}
			else break;
		}

		format = GetTextureFormat(Usage_Building);
		for(i=0;i<nAlphaTexNum;i++)
		{
			IDirect3DTexture8* p = pDevice->CreateTexture(size.cx,size.cy,format);
			if(p) 
			{
				textureMem += GetTextureMemSize(p);
				if(textureMem > m_totalTextureMem) 
				{
					p->Release();
					break;
				}
				m_allocators[CTextureAllocator::Type_Object].AddTexture(p);
			}
			else break;
		}

	}

	ResetTextureFormat();

	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CTextureManager::Destroy()
{
	//ReleaseAllTexture();
	DestroyAllCustomTexture();
	for(size_t i=0;i<count_of(m_allocators);++i)
	{
		m_allocators[i].Destroy();
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
// ÊÍ·ÅËùÓĞ¶ÔÎÆÀíµÄÕ¼ÓĞ
void CTextureManager::ReleaseAll()
{
	for(size_t i=0;i<count_of(m_allocators);++i)
	{
		m_allocators[i].ReleaseAll();
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CTextureManager::ReleaseAllTexture()
{
	ReleaseAllCustomTexture();
	for(size_t i=0;i<count_of(m_allocators);++i)
	{
		m_allocators[i].ReleaseAll();
		m_allocators[i].ReleaseAllTexture();
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param texType : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
// ÊÍ·ÅÖ¸¶¨ÀàĞÍµÄÎÆÀí
void CTextureManager::ReleaseTextureType(CTextureAllocator::TextureType texType)
{
	m_allocators[texType].ReleaseAll();
	m_allocators[texType].ReleaseAllTexture();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
D3DFORMAT CTextureManager::GetFontFormat()
{
	if(0 != (GetD3DDevice()->GetFlags() & IDevice::DEVICE_DXT))
	{
		return D3DFMT_A4R4G4B4;
	}

	if(GetD3DDevice()->CheckTexFormat(D3DFMT_A8))
		return D3DFMT_A8;
	return D3DFMT_A4R4G4B4;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param font : ÃèÊö
 * \param iFont : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CFontTextureGroup* CTextureManager::CreateTextureGroup(CD3DFont* font,int iFont)
{
	Assert(font->GetTextureGroup() == NULL);
	CFontTextureGroup* pGroup = new CFontTextureGroup(Usage_Num, GetFontFormat(),256,256,true,false,iFont);
	CTextureAllocator* pAllocator = GetFontTextureAllocator(iFont);
	pAllocator->AddTextureGroup(pGroup);
	return pGroup;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pMg : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CMBmpTextureGroup* CTextureManager::CreateTextureGroup(CD3DMBitmapManager* pMg)
{
	Assert(pMg->m_TexGroup == NULL);
	CTextureAllocator* pAllocator = GetTextureAllocator(Usage_BlendTerrain);
	Assert(pAllocator);
	SIZE size = GetTextureSize(Usage_BlendTerrain);
	CMBmpTextureGroup* pGroup = new CMBmpTextureGroup(Usage_BlendTerrain,GetTextureFormat(Usage_BlendTerrain),size.cx,size.cy,true,true);
	pAllocator->AddTextureGroup(pGroup);
	return pGroup;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param bmpList : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CBmpTextureGroup* CTextureManager::CreateTextureGroup(CD3DBitmapList* bmpList)
{
	Assert(bmpList->GetTextureGroup() == NULL);

	CBmpTextureGroup * pGroup = NULL;
	pGroup = (CBmpTextureGroup*)m_texturegroups[bmpList->GetUsage()];

	if(!pGroup)
	{
		CTextureAllocator* pAllocator = GetTextureAllocator(bmpList);
		Assert(pAllocator);

		SIZE size = GetTextureSize(bmpList);
		pGroup = 
			new CBmpTextureGroup(bmpList->GetUsage(),GetTextureFormat(bmpList),size.cx,size.cy,GetIsRowMajor(bmpList),GetIsAddTail(bmpList));
		pAllocator->AddTextureGroup(pGroup);

		switch(bmpList->GetUsage())
		{ 
		case Usage_Terrain:
		case Usage_TerrainSmall:
		case Usage_Building:
		case Usage_SmallObject:
		case Usage_Interface:
			m_texturegroups[bmpList->GetUsage()] = pGroup;
			break;
		case Usage_Magic:
		case Usage_Npc:
		case Usage_Character:
		case Usage_Shadow:
			m_texturegroups[bmpList->GetUsage()] = pGroup;
			break;
		}
	}
	if(pGroup) pGroup->AddBmpLists(bmpList);

	return pGroup;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param usage : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureAllocator* CTextureManager::GetTextureAllocator(TextureUsageType usage)
{
	switch(usage)
	{ 
	case Usage_Terrain:
		return & m_allocators[CTextureAllocator::Type_Terrain];
	case Usage_TerrainSmall:
	case Usage_Building:
	case Usage_SmallObject:
	case Usage_Interface:
		return & m_allocators[CTextureAllocator::Type_Object];
	case Usage_Magic:
	case Usage_Npc:
	case Usage_Character:
	case Usage_BlendTerrain:
	case Usage_Shadow:
		return & m_allocators[CTextureAllocator::Type_Object];
		//return & m_allocators[CTextureAllocator::Type_Scene];
	case Usage_NoAlpha:
		return & m_allocators[CTextureAllocator::Type_NoAlpha];
	}
	return NULL;
}
D3DFORMAT CTextureManager::GetTextureFormat(CD3DBitmapList * bmpList)
{
	return GetTextureFormat(bmpList->GetUsage());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param usage : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
D3DFORMAT CTextureManager::GetTextureFormat(TextureUsageType usage)
{
	CD3DDevice* pDevice = GetD3DDevice();
	switch(usage)
	{ 
	case Usage_Terrain:
	case Usage_NoAlpha:
		return (pDevice->m_bUsableDXT1 ? D3DFMT_DXT1 : D3DFMT_R5G6B5);
	case Usage_TerrainSmall:
	case Usage_Building:
	case Usage_SmallObject:
	case Usage_Interface:
	case Usage_Magic:
	case Usage_Npc:
	case Usage_Character:
	case Usage_BlendTerrain:
	case Usage_Shadow:
		return (pDevice->m_bUsableDXT3 ? D3DFMT_DXT3 : D3DFMT_A4R4G4B4);
	case Usage_Num:
		return GetFontFormat();
	default:
		Assert(0);
		break;
	}
	return D3DFMT_A4R4G4B4;
}

void CTextureManager::ResetTextureFormat()
{
	for(size_t i = 0; i< count_of(m_allocators); ++i)
	{
		m_allocators[i].ResetFormat();
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param usage : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
SIZE CTextureManager::GetTextureSize(TextureUsageType usage)
{
	SIZE size = {256,256};
	return size;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param usage : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CTextureManager::GetIsRowMajor(TextureUsageType usage)
{
	switch(usage)
	{ 
	case Usage_Terrain:
	case Usage_TerrainSmall:
	case Usage_SmallObject:
	case Usage_Interface:
	case Usage_Magic:
	case Usage_Npc:
	case Usage_Character:
	case Usage_Shadow:
		return true;
	case Usage_Building:
		return false;
	}
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param usage : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CTextureManager::GetIsAddTail(TextureUsageType usage)
{
	switch(usage)
	{ 
	case Usage_Terrain:
	case Usage_BlendTerrain:
		return false;
	case Usage_TerrainSmall:
	case Usage_SmallObject:
	case Usage_Interface:
	case Usage_Magic:
	case Usage_Npc:
	case Usage_Character:
	case Usage_Building:
	case Usage_Shadow:
		return true;
	}
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param bmpList : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureAllocator* CTextureManager::GetTextureAllocator(CD3DBitmapList* bmpList)
{
	return GetTextureAllocator(bmpList->GetUsage());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param bmpList : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
SIZE CTextureManager::GetTextureSize(CD3DBitmapList* bmpList)
{
	return GetTextureSize(bmpList->GetUsage());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param bmpList : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CTextureManager::GetIsRowMajor(CD3DBitmapList* bmpList)
{
	return GetIsRowMajor(bmpList->GetUsage());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param bmpList : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CTextureManager::GetIsAddTail(CD3DBitmapList* bmpList)
{
	return GetIsAddTail(bmpList->GetUsage());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param width : ÃèÊö
 * \param height : ÃèÊö
 * \param format : ÃèÊö
 * \param dwUsage : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
IDirect3DTexture8* CTextureManager::CreateTexture(int width,int height,D3DFORMAT format,DWORD dwUsage)
{
	IDirect3DTexture8* pTex;
	HRESULT hr;

	CTextureAllocator::TextureType aType[] = {CTextureAllocator::Type_Object,CTextureAllocator::Type_Scene};
	for( size_t i=0 ; i<count_of(aType) ; ++i )
	{
		hr = GetD3DDevice()->GetDevice()->CreateTexture(width,height,1,dwUsage,format,D3DPOOL_DEFAULT,&pTex);
		if(hr == D3DERR_OUTOFVIDEOMEMORY)
		{
			// ÏÔ´æ²»¹»£¬ÊÍ·ÅÒ»Ğ©ÎÆÀí
			TRACE_DXERROR(hr);
			GetD3DDevice()->FinalFlush();
			ReleaseTextureType(aType[i]);
		}
		else
		{
			break;
		}
	}

	if(FAILED(hr)) 
	{
		TRACE_DXERROR(hr);
		return NULL;
	}

	return pTex;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param width : ÃèÊö
 * \param height : ÃèÊö
 * \param format : ÃèÊö
 * \param dwUsage : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CCustomTexture* CTextureManager::CreateCurstomTexture(int width,int height,D3DFORMAT format,DWORD dwUsage)
{
	CCustomTexture* pCustomTexture = new CCustomTexture(width,height,format,dwUsage);
	m_CustomTextures.push_back(pCustomTexture);
	return pCustomTexture;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param width : ÃèÊö
 * \param height : ÃèÊö
 * \param format : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CRenderTargetTexture* CTextureManager::CreateRenderTargetTexture(int width,int height,D3DFORMAT format)
{
	CRenderTargetTexture* pCustomTexture = new CRenderTargetTexture(width,height,format);
	m_CustomTextures.push_back(pCustomTexture);
	return pCustomTexture;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pTex : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CTextureManager::DeleteCustomeTexture(CCustomTexture* pTex)
{
	for(size_t i=0;i<m_CustomTextures.size();++i)
	{
		if(m_CustomTextures[i] == pTex)
		{
			delete m_CustomTextures[i];
			SimpleRemoveFromArray(m_CustomTextures,i);
			return;
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CTextureManager::ReleaseAllCustomTexture()
{
	for(tListCustomTexture::iterator it = m_CustomTextures.begin(); it != m_CustomTextures.end(); ++it)
	{
		(*it)->Release();
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CTextureManager::DestroyAllCustomTexture()
{
	for(tListCustomTexture::iterator it = m_CustomTextures.begin(); it != m_CustomTextures.end(); ++it)
	{
		delete (*it);
	}
	m_CustomTextures.resize(0);
}