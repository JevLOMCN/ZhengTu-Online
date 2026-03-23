/**
 * \file      IBitmaps.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     Í¼Æ¬£¨¶àÖ¡£©²Ù×÷½Ó¿Ú
 * 
 *	      Í¼Æ¬£¨¶àÖ¡£©²Ù×÷½Ó¿Ú
 */

#include "../include/engine.h"
#include "../include/ibitmaps.h"
#include "../include/IDevice.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
IBitmaps::IBitmaps(void)
//: m_dwMarkTime(0)
//, eLoad(Load_NotLoad)
: m_pBmpList(NULL)
, m_pFrameIndex(NULL)
, m_dwGroup(-1)
, m_dwFrame(-1)
, m_dwDataOffset(0)
, m_dwDataSize(0)
{
	m_aniInfo.wdBitmapCount = 0;
	m_aniInfo.wdFrameCount = 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
const char* IBitmaps::GetPackName()
{
	return m_pBmpList->GetFileName();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pBmps : ÃèÊö
 * \param pAlphas : ÃèÊö
 * \param x : ÃèÊö
 * \param y : ÃèÊö
 * \param dwTime : ÃèÊö
 * \param rClip : ÃèÊö
 * \param scale : ÃèÊö
 * \param dwColor : ÃèÊö
 * \param blend : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void IBitmaps::RenderBlendAni(IBitmaps* pBmps,IBitmaps* pAlphas,int x , int y , DWORD dwTime , const RECT* rClip, const stPointF * scale , DWORD dwColor, BlendType blend)
{
	if( pBmps->GetFrameCount() == 0 || pAlphas->GetFrameCount() == 0 ) return;
	size_t frameBmp,frameAlpha;

	if(pBmps->GetFrameCount() == 1 ) frameBmp = 0;
	else frameBmp = ( dwTime / pBmps->m_aniInfo.wdAniSpeed ) % pBmps->GetFrameCount();

	if(pAlphas->GetFrameCount() == 1 ) frameAlpha = 0;
	else frameAlpha = ( dwTime / pAlphas->m_aniInfo.wdAniSpeed ) % pAlphas->GetFrameCount();

	IBitmap* pBmp,*pAlpha;
	pBmp = pBmps->GetBitmap( frameBmp );
	pAlpha = pAlphas->GetBitmap( frameAlpha );

	//TRACE("frameBmp %d\n",frameBmp);
	GetDevice()->RenderBlendBitmap(pBmp,pAlpha,x,y,rClip,scale,dwColor,blend);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pBmps : ÃèÊö
 * \param pAlphas : ÃèÊö
 * \param x : ÃèÊö
 * \param y : ÃèÊö
 * \param dwTime : ÃèÊö
 * \param dwColor : ÃèÊö
 * \param blend : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void IBitmaps::RenderBlendAni(IBitmaps* pBmps,IBitmaps* pAlphas,int x , int y , DWORD dwTime ,DWORD dwColor,BlendType blend)
{
	if( pBmps->GetFrameCount() == 0 || pAlphas->GetFrameCount() == 0 ) return;
	size_t frameBmp,frameAlpha;

	if( pBmps->GetFrameCount() == 1 ) frameBmp = 0;
	else frameBmp = (dwTime / pBmps->m_aniInfo.wdAniSpeed) % pBmps->GetFrameCount();

	if( pAlphas->GetFrameCount() == 1 ) frameAlpha = 0;
	else frameAlpha = (dwTime / pAlphas->m_aniInfo.wdAniSpeed) % pAlphas->GetFrameCount();

	IBitmap* pBmp,*pAlpha;
	pBmp = pBmps->GetBitmap(frameBmp);
	pAlpha = pAlphas->GetBitmap(frameAlpha);
	GetDevice()->RenderBlendBitmap(pBmp,pAlpha,x,y,dwColor,blend);
}