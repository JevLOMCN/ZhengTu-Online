/**
 * \file      GuiElement.h
 * \version   $Id$
 * \author    $$$03¹«Ë¾@###.com
 *           $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUI»æ»­ÔªËØ
 * 
 *	      GUI»æ»­ÔªËØ
 */

#pragma once

struct GuiBlendColor
{
    void Init( DWORD defaultColor, DWORD disabledColor = D3DCOLOR_ARGB(200, 128, 128, 128), DWORD hiddenColor = 0 );
    void Blend( UINT iState, float fElapsedTime, float fRate = 0.7f );
	void Blend( const DWORD& dstColor, float fElapsedTime, bool bUse, float fRate = 0.7f );
    
    DWORD  States[ MAX_CONTROL_STATES ]; // Modulate colors for all possible control states
    D3DXCOLOR Current;
	DWORD	defaultColor;
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CGuiElement
{
public:
    void SetTexture( const stResourceLocation *prl,const RECT* prcTexture = NULL, DWORD defaultTextureColor = COLOR_ARGB(255, 255, 255, 255),BlendType i_blend = Blend_Null);
    void SetFont( UINT iFont, DWORD defaultFontColor = D3DCOLOR_ARGB(255, 255, 255, 255), DWORD dwTextFormat = DT_CENTER | DT_VCENTER );
    
    void Refresh();
    
	CGuiElement() : pBmp(NULL) ,blend(Blend_Null),dwTextFormat(DT_CENTER | DT_VCENTER) , iFont(0){}

	IBitmaps* pBmp;
    UINT iFont;             // Index of the font for this Element
	BlendType blend;
    DWORD dwTextFormat;     // The format argument to DrawText 

    stRectI rcTexture;      // Bounding rect of this element on the composite texture
    
    GuiBlendColor TextureColor;
    GuiBlendColor FontColor;
};

//-----------------------------------------------------------------------------
// Contains all the display information for a given control type
//-----------------------------------------------------------------------------
struct GuiElementHolder
{
    int nControlType;
    UINT iElement;

    CGuiElement Element;
};