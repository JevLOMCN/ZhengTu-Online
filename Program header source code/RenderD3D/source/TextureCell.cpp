/**
 * \file      TextureCell.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2005-07-05 12:29:20
 * \brief     ÎÆÀíCell£¬Ò»ÕÅ´óÎÆÀíÉÏµÄÐ¡ÎÆÀí¶ÔÏó
 * 
 *	      ÎÆÀíCell£¬Ò»ÕÅ´óÎÆÀíÉÏµÄÐ¡ÎÆÀí¶ÔÏó
 */

#include "../../engine/include/engine.h"
#include "../include/texturecell.h"
#include "../include/D3DBitmap.h"
#include "../include/D3DFont.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureCell::CTextureCell(CTextureLine* pTexLine,const stRectI* prc)
: m_pTexLine(pTexLine)
, m_rcTex(*prc)
, m_bFree(true)
{
	Assert(prc->right > prc->left);
	Assert(prc->bottom > prc->top);
	Assert(prc->right <= 256);
	Assert(prc->bottom <= 256);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CTextureCell::~CTextureCell(void)
{
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CTextureCell::ReleaseTexture()
{
	// ÏÂÃæË³Ðò²»ÄÜÐ´·´
	/*if(m_pBmp)
	{
		m_pBmp->m_textures[m_dwBlock] = NULL;
		m_pBmp = NULL;
	}*/
	if(!m_pTexLine->PopTexture(this))
	{
		Assert(0);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------
// CMBmpTextureCell
//--------------------------------
CMBmpTextureCell::~CMBmpTextureCell()
{
	if(m_pBmp)
	{
		m_pBmp->m_pTexture = NULL;
		m_pBmp = NULL;
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CMBmpTextureCell::ReleaseTexture()
{
	// ÏÂÃæË³Ðò²»ÄÜÐ´·´
	if(m_pBmp)
	{
		m_pBmp->m_pTexture = NULL;
		m_pBmp = NULL;
	}
	CTextureCell::ReleaseTexture();
}
//----------------------------------


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//----------------------------------
// CBmpTextureCell
//----------------------------------
CBmpTextureCell::~CBmpTextureCell()
{
	if(m_pBmp)
	{
		m_pBmp->m_textures[m_dwBlock].pTex = NULL;
		m_pBmp = NULL;
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CBmpTextureCell::ReleaseTexture()
{
	// ÏÂÃæË³Ðò²»ÄÜÐ´·´
	if(m_pBmp)
	{
		m_pBmp->m_textures[m_dwBlock].pTex = NULL;
		m_pBmp = NULL;
	}
	CTextureCell::ReleaseTexture();
}
//-----------------------------------


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CFontTextureCell::ReleaseTexture()
{
	if(m_pFont) 
	{
		m_pFont->DeleteCharTex(m_char);
		m_pFont = NULL;
	}
	CTextureCell::ReleaseTexture();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CFontTextureCell::~CFontTextureCell()
{
	if(m_pFont) 
	{
		m_pFont->DeleteCharTex(m_char);
		m_pFont = NULL;
	}
}