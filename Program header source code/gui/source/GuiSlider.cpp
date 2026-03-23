/**
 * \file      GuiSlider.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾@###.com
 *           $$$03¹«Ë¾@###.com
 * \date      2005-07-05 17:02:57
 * \brief     GUI¿Ì¶ÈÌõ
 * 
 *	      GUI¿Ì¶ÈÌõ
 */

#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pDialog : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
CGuiSlider::CGuiSlider( CGuiDialog *pDialog )
{
    m_Type = GUI_CONTROL_SLIDER;
    m_pDialog = pDialog;

    m_nMin = 0;
    m_nMax = 100;
    m_nValue = 50;

    m_bPressed = false;
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
bool CGuiSlider::ContainsPoint( POINT pt ) 
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
void CGuiSlider::UpdateRects()
{
    CGuiControl::UpdateRects();

    m_rcButton = m_rcBoundingBox;
    m_rcButton.right = m_rcButton.left + m_rcButton.Height();
    m_rcButton.OffsetRect( -m_rcButton.Width()/2, 0 );

    m_nButtonX = (int) ( (m_nValue - m_nMin) * (float)m_rcBoundingBox.Width() / (m_nMax - m_nMin) );
    m_rcButton.OffsetRect( m_nButtonX, 0 );
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param x : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CGuiSlider::ValueFromPos( int x )
{ 
    float fValuePerPixel = (float)(m_nMax - m_nMin) / m_rcBoundingBox.Width();
    return (int) (0.5f + m_nMin + fValuePerPixel * (x - m_rcBoundingBox.left)) ; 
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
bool CGuiSlider::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( !m_bEnabled || !m_bVisible )
        return false;

    switch( uMsg )
    {
        case WM_KEYDOWN:
        {
            switch( wParam )
            {
                case VK_HOME:
                    SetValueInternal( m_nMin, true );
                    return true;

                case VK_END:
                    SetValueInternal( m_nMax, true );
                    return true;

                case VK_PRIOR:
                case VK_LEFT:
                case VK_UP:
                    SetValueInternal( m_nValue - 1, true );
                    return true;

                case VK_NEXT:
                case VK_RIGHT:
                case VK_DOWN:
                    SetValueInternal( m_nValue + 1, true );
                    return true;
            }
            break;
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
bool CGuiSlider::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
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
            if( PtInRect( &m_rcButton, pt ) )
            {
                // Pressed while inside the control
                m_bPressed = true;
                //SetCapture( DXUTGetHWND() );

                m_nDragX = pt.x;
                //m_nDragY = pt.y;
                m_nDragOffset = m_nButtonX - m_nDragX;

                //m_nDragValue = m_nValue;

                if( !m_bHasFocus )
                    m_pDialog->RequestFocus( this );

                return true;
            }

            if( PtInRect( &m_rcBoundingBox, pt ) )
            {
               if( pt.x > m_nButtonX + m_x )
               {
                   SetValueInternal( m_nValue + 1, true );
                   return true;
               }

               if( pt.x < m_nButtonX + m_x )
               {
                   SetValueInternal( m_nValue - 1, true );
                   return true;
               }
            }

            break;
        }

        case WM_LBUTTONUP:
        {
            if( m_bPressed )
            {
                m_bPressed = false;
                //ReleaseCapture();
                m_pDialog->ClearFocus();
                m_pDialog->SendEvent( EVENT_SLIDER_VALUE_CHANGED, true, this );

                return true;
            }

            break;
        }

        case WM_MOUSEMOVE:
        {
            if( m_bPressed )
            {
                SetValueInternal( ValueFromPos( m_x + pt.x + m_nDragOffset ), true );
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
 * \param nMin : ÃèÊö
 * \param nMax : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void CGuiSlider::SetRange( int nMin, int nMax ) 
{
    m_nMin = nMin; 
    m_nMax = nMax; 

    SetValueInternal( m_nValue, false );
}



/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nValue : ÃèÊö
 * \param bFromInput : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void CGuiSlider::SetValueInternal( int nValue, bool bFromInput )
{
    // Clamp to range
    nValue = max( m_nMin, nValue );
    nValue = min( m_nMax, nValue );
    
    if( nValue == m_nValue )
        return;

    m_nValue = nValue;
    UpdateRects();

    m_pDialog->SendEvent( EVENT_SLIDER_VALUE_CHANGED, bFromInput, this );
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
void CGuiSlider::Render( float fElapsedTime )
{
	Assert(IsVisible());
	CGuiControl::Render(fElapsedTime);

    int nOffsetX = 0;
    int nOffsetY = 0;

    GUI_CONTROL_STATE iState = GUI_STATE_NORMAL;

    if( m_bVisible == false )
    {
        iState = GUI_STATE_HIDDEN;
    }
    else if( m_bEnabled == false )
    {
        iState = GUI_STATE_DISABLED;
    }
    else if( m_bPressed )
    {
        iState = GUI_STATE_PRESSED;

        nOffsetX = 1;
        nOffsetY = 2;
    }
    else if( m_bMouseOver )
    {
        iState = GUI_STATE_MOUSEOVER;
        
        nOffsetX = -1;
        nOffsetY = -2;
    }
    else if( m_bHasFocus )
    {
        iState = GUI_STATE_FOCUS;
    }

    float fBlendRate = ( iState == GUI_STATE_PRESSED ) ? 0.0f : 0.8f;

	

    CGuiElement* pElement = m_Elements[0];
    
    // Blend current color
    pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate ); 
    m_pDialog->DrawSprite( pElement, &m_rcBoundingBox );

    //TODO: remove magic numbers
    pElement = m_Elements[ 1 ];
       
    // Blend current color
    pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
    m_pDialog->DrawSprite( pElement, &m_rcButton );

	
}


