/**
 * \file      GuiElement.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾@###.com
 *           $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUI»æ»­ÔªËØ
 * 
 *	      GUI»æ»­ÔªËØ
 */

#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param defaultColor : ÃèÊö
 * \param disabledColor : ÃèÊö
 * \param hiddenColor : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void GuiBlendColor::Init( DWORD defaultColor, DWORD disabledColor, DWORD hiddenColor )
{
    for( int i=0; i < MAX_CONTROL_STATES; i++ )
    {
        States[ i ] = defaultColor;
    }

    States[ GUI_STATE_DISABLED ] = disabledColor;
    States[ GUI_STATE_HIDDEN ] = hiddenColor;
    Current = hiddenColor;
	defaultColor = defaultColor;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iState : ÃèÊö
 * \param fElapsedTime : ÃèÊö
 * \param fRate : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void GuiBlendColor::Blend( UINT iState, float fElapsedTime, float fRate )
{
	// fRat Ô½Ğ¡ËÙ¶ÈÔ½¿ì
    D3DXCOLOR destColor = States[ iState ];
    D3DXColorLerp( &Current, &Current, &destColor, 1.0f - powf( fRate , 30 * fElapsedTime ) );
}

void GuiBlendColor::Blend( const DWORD& dstColor, float fElapsedTime, bool bUse, float fRate )
{
	D3DXColorLerp( &Current, &Current, &((D3DXCOLOR)dstColor), 1.0f - powf( fRate , 30 * fElapsedTime ) );
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iFont : ÃèÊö
 * \param defaultFontColor : ÃèÊö
 * \param dwTextFormat : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//*
//--------------------------------------------------------------------------------------
void CGuiElement::SetFont( UINT iFont, DWORD defaultFontColor, DWORD dwTextFormat )
{
    this->iFont = iFont;
    this->dwTextFormat = dwTextFormat;

    FontColor.Init( defaultFontColor );
}
//*/
//--------------------------------------------------------------------------------------

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param prl : ÃèÊö
 * \param prcTexture : ÃèÊö
 * \param defaultTextureColor : ÃèÊö
 * \param i_blend : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiElement::SetTexture( const stResourceLocation *prl,const RECT* prcTexture, DWORD defaultTextureColor , BlendType i_blend)
{
	pBmp = GetDevice()->FindBitmaps(prl);
    blend = i_blend;
    if( prcTexture )
	{
        rcTexture = *prcTexture;
	}
    else
	{
		if(pBmp && pBmp->GetFrameCount())
			rcTexture.SetRect(0,0,pBmp->GetBitmap(0)->GetWidth(),pBmp->GetBitmap(0)->GetHeight());
		else
			SetRectEmpty( &rcTexture );
	}
    
    TextureColor.Init( defaultTextureColor );
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void CGuiElement::Refresh()
{
    TextureColor.Current = TextureColor.States[ GUI_STATE_HIDDEN ];
    FontColor.Current = FontColor.States[ GUI_STATE_HIDDEN ];
}







