/**
 * \file      GuiCheckBox.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾@###.com
 *           $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUI¼ì²é¿ò ¸´Ñ¡¿ò
 * 
 *	      GUI¼ì²é¿ò
 */

#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"

//--------------------------------------------------------------------------------------
// CGuiCheckBox class
//--------------------------------------------------------------------------------------

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDialog : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
CGuiCheckBox::CGuiCheckBox( CGuiDialog *pDialog )
{
    m_Type = GUI_CONTROL_CHECKBOX;
    m_pDialog = pDialog;

    m_bChecked = false;
}
    

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param uMsg : ÃèÊö
 * \param wParam : ÃèÊö
 * \param lParam : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
bool CGuiCheckBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    switch( uMsg )
    {
        case WM_KEYDOWN:
        {
            switch( wParam )
            {
                case VK_SPACE:
                    m_bPressed = true;
                    return true;
            }
        }

        case WM_KEYUP:
        {
            switch( wParam )
            {
                case VK_SPACE:
                    if( m_bPressed == true )
                    {
                        m_bPressed = false;
                        SetCheckedInternal( !m_bChecked, true );
                    }
                    return true;
            }
        }
    }
    return false;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param uMsg : ÃèÊö
 * \param pt : ÃèÊö
 * \param wParam : ÃèÊö
 * \param lParam : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
bool CGuiCheckBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if(CGuiControl::HandleMouse(uMsg,pt,wParam,lParam))
		return true;

    if( !m_bEnabled || !m_bVisible )
        return false;

    switch( uMsg )
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        {
            if( ContainsPoint( pt ) )
            {
                // Pressed while inside the control
                m_bPressed = true;
                //SetCapture( DXUTGetHWND() );

                if( !m_bHasFocus && m_pDialog->m_bKeyboardInput )
                    m_pDialog->RequestFocus( this );

                return true;
            }

            break;
        }

        case WM_LBUTTONUP:
        {
            if( m_bPressed )
            {
                m_bPressed = false;
                //ReleaseCapture();

                // Button click
                if( ContainsPoint( pt ) )
                    SetCheckedInternal( !m_bChecked, true );
                
                return true;
            }

            break;
        }
    };
    
    return false;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pt : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
bool CGuiCheckBox::ContainsPoint( POINT pt ) 
{ 
    return ( PtInRect( &m_rcBoundingBox, pt ) || 
             PtInRect( &m_rcButton, pt ) ); 
}



/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void CGuiCheckBox::UpdateRects()
{
    CGuiControl::UpdateRects();

    m_rcButton = m_rcBoundingBox;
	m_rcButton.right = m_rcButton.left + m_rcButton.Height();

    m_rcText = m_rcBoundingBox;
	m_rcText.left += (int) ( 1.25f * m_rcButton.Width() );
}



/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fElapsedTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void CGuiCheckBox::Render( float fElapsedTime )
{
	CGuiControl::Render(fElapsedTime);

    GUI_CONTROL_STATE iState = GUI_STATE_NORMAL;

    if( m_bVisible == false )
        iState = GUI_STATE_HIDDEN;
    else if( m_bEnabled == false )
        iState = GUI_STATE_DISABLED;
    else if( m_bPressed )
        iState = GUI_STATE_PRESSED;
    else if( m_bMouseOver )
        iState = GUI_STATE_MOUSEOVER;
    else if( m_bHasFocus )
        iState = GUI_STATE_FOCUS;

    //debug
    //m_pDialog->DrawRect( &m_rcBoundingBox, D3DCOLOR_ARGB(255, 255, 255, 0) );
    //m_pDialog->DrawRect( &m_rcButton, D3DCOLOR_ARGB(255, 0, 255, 255) );

    //TODO: remove magic numbers
    CGuiElement* pElement = m_Elements[0];
    
    float fBlendRate = ( iState == GUI_STATE_PRESSED ) ? 0.0f : 0.8f;

    pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
    pElement->FontColor.Blend( iState, fElapsedTime, fBlendRate );
	
	if(!(GetStyle() & GUI_ES_NOSKIN))
	{
        m_pDialog->DrawSprite( pElement, &m_rcButton );
	}

    m_pDialog->DrawText( m_strText, pElement, &m_rcText, FontEffect_Shadow );

    if( !m_bChecked )
        iState = GUI_STATE_HIDDEN;

    pElement = m_Elements[1];

    pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
    m_pDialog->DrawSprite( pElement, &m_rcButton );
}



