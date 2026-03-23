/**
 * \file      TextureAllocator.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2005-07-05 12:29:19
 * \brief     ÎÆÀí·ÖÅäÆ÷
 * 
 *	      ÎÆÀí·ÖÅäÆ÷
 */

#include "../../engine/include/engine.h"
#include "../include/textureallocator.h"
#include "../include/texturegroup.h"
#include "../include/texturecell.h"
#include "../include/texturecache.h"
#include "../include/texturemanager.h"
#include "../include/D3DDevice.h"
#include "../Include/RenderD3D.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureAllocator::CTextureAllocator(void)
{
	m_UseTextures.reserve(16);
	m_FreeTextures.reserve(16);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureAllocator::~CTextureAllocator(void)
{
	Destroy();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CTextureAllocator::Destroy()
{
	ReleaseAll();
	ReleaseAllTexture();
	m_FreeTextureGroups.clear(type2type_t<CTextureGroup>());
	m_TextureGroups.clear(type2type_t<CTextureGroup>());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pGroup : ÃèÊö
 * \param width : ÃèÊö
 * \param height : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureCache* CTextureAllocator::AllocTexture(CTextureGroup* pGroup,size_t width,size_t height)
{
	
	IDirect3DTexture8* pTex = UseATexture(pGroup->GetFormat());
	if(pTex)
	{
		return pGroup->AddTextureCache(pTex);
	}
	
	GetD3DDevice()->FinalFlush();
	const DWORD max_time = 30000;

	switch(m_texType)
	{
	case Type_Terrain:
	case Type_Font:
	case Type_Font2:
	case Type_Font3:
	case Type_Font4:
	case Type_Font5:
		return ReleaseCellByTime(max_time,width,height);
	case Type_Object:
	case Type_NoAlpha:
		ReleaseAll(pGroup);
		break;
	case Type_Scene:
		ReleaseGroupByTime(max_time,pGroup);
		break;
	}

	pTex = UseATexture(pGroup->GetFormat());
	if(pTex)
	{
		return pGroup->AddTextureCache(pTex);
	}
	return NULL;
}

CTextureCache* CTextureAllocator::TryAllocTexture(CTextureGroup* pGroup,size_t width,size_t height)
{
	IDirect3DTexture8* pTex = UseATexture(pGroup->GetFormat());
	if(pTex)
	{
		return pGroup->AddTextureCache(pTex);
	}
	return NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dwTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureCache* CTextureAllocator::ReleaseCellByTime(DWORD dwTime,size_t width,size_t height)
{
	CTextureCache* pRet = NULL;
	DWORD time = xtimeGetTime();
	for(SimpleList::reverse_iterator it = m_TextureCells.rbegin(); it != m_TextureCells.rend();)
	{
		CTextureCell & cell = static_cast<CTextureCell&>(*it);
		if(time - cell.GetLastTime() > dwTime)
		{
			if(pRet == NULL && cell.GetWidth() >= width && cell.GetHeight() >= height) pRet = cell.GetTexCache();

			// ÏÂÃæË³Ğò²»ÄÜĞ´·´
			++it;
			ReleaseATextureCell(&cell);
		}
		else
		{
			if(pRet) break;

			if(cell.GetWidth() >= width && cell.GetHeight() >= height) 
			{
				pRet = cell.GetTexCache();

				// ÏÂÃæË³Ğò²»ÄÜĞ´·´
				++it;
				ReleaseATextureCell(&cell);
				break;
			}
			++it;
		}
	}

	return pRet;

	//CTextureCache* pRet = NULL;
	//CTextureCell* pCell = (CTextureCell*)m_TextureCells._End();
	//DWORD time = xtimeGetTime();
	//for(; pCell ;)
	//{
	//	if(time - pCell->GetLastTime() > dwTime)
	//	{
	//		if(pRet == NULL) pRet = pCell->GetTexCache();
	//		
	//		// ÏÂÃæË³Ğò²»ÄÜĞ´·´
	//		CTextureCell* next = (CTextureCell*)pCell->_Prev();
	//		ReleaseATextureCell(pCell);
	//		pCell  = next;
	//	}
	//	else
	//	{
	//		break;
	//	}
	//}

	//if(pRet == NULL)
	//{
	//	pCell = (CTextureCell*)m_TextureCells._End();
	//	if(pCell)
	//	{
	//		// ÏÂÃæË³Ğò²»ÄÜĞ´·´
	//		pRet = pCell->GetTexCache();
	//		ReleaseATextureCell(pCell);
	//	}
	//}
	//return pRet;
} 

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param dwTime : ÃèÊö
 * \param pNoDelete : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CTextureAllocator::ReleaseGroupByTime(DWORD dwTime,CTextureGroup* pNoDelete)
{
	bool bRelease = false;
	DWORD time = xtimeGetTime();
	for(SimpleList::reverse_iterator it = m_TextureGroups.rbegin(); it != m_TextureGroups.rend() ;)
	{
		CTextureGroup& cGroup = static_cast<CTextureGroup&>(*it);
		if(time - cGroup.GetLastTime() > dwTime)
		{
			cGroup.Release();
			if(&cGroup == pNoDelete)
			{
				it = m_TextureGroups.erase(it);
				m_FreeTextureGroups.push_back(&cGroup);
			}
			else
			{
				++it;
			}
			bRelease = true;
		}
		else
		{
			break;
		}
	}

	if(!bRelease)
	{
		if(!m_TextureGroups.empty())
		{
			CTextureGroup & cGroup = static_cast<CTextureGroup&>(*m_TextureGroups.back());
			cGroup.Release();
			if(&cGroup != pNoDelete)
			{
				m_TextureGroups.pop_back();
				m_FreeTextureGroups.push_back(&cGroup);
			}
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pNoDelete : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//-------------------------------------------------
// ÊÍ·ÅËùÓĞ¶ÔÎÆÀíµÄÕ¼ÓĞ
//-------------------------------------------------
void CTextureAllocator::ReleaseAll(CTextureGroup* pNoDelete)
{
	m_TextureCells.clear();

	for(t_TexturesIt it = m_UseTextures.begin(); it != m_UseTextures.end(); ++ it)
	{
		GetTextureManager()->UpdateuseTextureMemSize( -(long)(GetTextureManager()->GetTextureMemSize(*it)) );
		m_FreeTextures.push_back(*it);
	}

	m_UseTextures.resize(0);

	DWORD time = xtimeGetTime();
	for(SimpleList::reverse_iterator it = m_TextureGroups.rbegin(); it != m_TextureGroups.rend() ;)
	{
		CTextureGroup& cGroup = static_cast<CTextureGroup&>(*it);
		cGroup.Release();

		if(&cGroup != pNoDelete)
		{
			it = m_TextureGroups.erase(it);
			m_FreeTextureGroups.push_back(&cGroup);
		}
		else
		{
			++it;
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param * : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CTextureAllocator::UpdateTextureGroup(CTextureGroup * p)
{
	if(m_TextureGroups.empty() || m_TextureGroups.front() != p)
	{
		p->unlink();// ´Óµ±Ç°Á´±íÖĞÉ¾³ı(¿ÉÄÜÔÚm_FreeTextureGroups »ò m_TextureGroups ÖĞ)
		m_TextureGroups.push_front(p);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pGroup : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CTextureAllocator::AddTextureGroup(CTextureGroup* pGroup)
{
	pGroup->UpdateTime();
	UpdateTextureGroup(pGroup);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param p : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CTextureAllocator::AddTextureCell(CTextureCell* p)
{
	if(m_texType == Type_Terrain || (m_texType >= Type_Font && m_texType <= Type_Font5))
	{
		Assert(p->IsFree() == false);
		m_TextureCells.push_front(p);
		//m_TextureCells._AddToBegin(p);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param p : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CTextureAllocator::UpdateTextureCell(CTextureCell* p)
{
	if(m_texType == Type_Terrain || (m_texType >= Type_Font && m_texType <= Type_Font5))
	{
		Assert(p->IsFree() == false);
		/*if( p != m_TextureCells._Begin())
		{
			m_TextureCells._Delete(p);
			m_TextureCells._AddToBegin(p);
		}*/
		if( m_TextureCells.empty() || p != m_TextureCells.front())
		{
			p->unlink();
			m_TextureCells.push_front(p);
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pCell : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CTextureAllocator::ReleaseATextureCell(CTextureCell* pCell)
{
	pCell->unlink();
	pCell->ReleaseTexture();
	/*m_TextureCells._Delete(pCell);
	pCell->ReleaseTexture();*/
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pTex : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CTextureAllocator::ReleaseATexture(IDirect3DTexture8* pTex)
{
	for(size_t i = 0; i < m_UseTextures.size(); ++i)
	{
		if(m_UseTextures[i] == pTex)
		{
			DeleteATexture(m_UseTextures,i);
			m_FreeTextures.push_back(pTex);
			GetTextureManager()->UpdateuseTextureMemSize( - (long)GetTextureManager()->GetTextureMemSize(pTex) );
			break;
		}
	}
	//Assert(0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param format : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline IDirect3DTexture8* CTextureAllocator::UseATexture(DWORD format)
{
	if(m_FreeTextures.empty()) 
	{
		if(m_texType == Type_Object || m_texType == Type_Scene || m_texType == Type_NoAlpha)
		{
			IDirect3DTexture8* pTex = GetD3DDevice()->CreateTexture(256,256,(D3DFORMAT)format);
			if(!pTex) return NULL;
			AddTexture(pTex);
		}
		else 
		{
			return NULL;
		}
	}

	IDirect3DTexture8* pTex = DeleteATexture(m_FreeTextures,m_FreeTextures.size()-1);
	m_UseTextures.push_back(pTex);
	GetTextureManager()->UpdateuseTextureMemSize( GetTextureManager()->GetTextureMemSize(pTex));
	return pTex;
}

void CTextureAllocator::ResetFormat()
{
	for(SimpleList::iterator it = m_TextureGroups.begin(); 
		it != m_TextureGroups.end(); ++it){
		CTextureGroup& group = static_cast<CTextureGroup&>(*it);
		group.ResetFormat(GetTextureManager()->GetTextureFormat(group.GetUsage()));
	}

	for(SimpleList::iterator it = m_FreeTextureGroups.begin(); 
		it != m_FreeTextureGroups.end(); ++it){
		CTextureGroup& group = static_cast<CTextureGroup&>(*it);
		group.ResetFormat(GetTextureManager()->GetTextureFormat(group.GetUsage()));
	}
}