/**
 * \file      GuiRadioButton.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾@###.com
 *           $$$03¹«Ë¾@###.com
 * \date      2005-07-05 17:02:57
 * \brief     GUIµ¥Ñ¡¿ò
 * 
 *	      GUIµ¥Ñ¡¿ò
 */

#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"

//--------------------------------------------------------------------------------------
// CGuiRadioButton class
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
CGuiRadioButton::CGuiRadioButton( CGuiDialog *pDialog )
{
    m_Type = GUI_CONTROL_RADIOBUTTON;
    m_pDialog = pDialog;
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
bool CGuiRadioButton::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
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
                        
                        m_pDialog->ClearRadioButtonGroup( m_nButtonGroup );
                        m_bChecked = !m_bChecked;

                        m_pDialog->SendEvent( EVENT_RADIOBUTTON_CHANGED, true, this );
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
bool CGuiRadioButton::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
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
                {
                    m_pDialog->ClearRadioButtonGroup( m_nButtonGroup );
                    m_bChecked = !m_bChecked;

                    m_pDialog->SendEvent( EVENT_RADIOBUTTON_CHANGED, true, this );
                }

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
 * \param bChecked : ÃèÊö
 * \param bClearGroup : ÃèÊö
 * \param bFromInput : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void CGuiRadioButton::SetCheckedInternal( bool bChecked, bool bClearGroup, bool bFromInput )
{
    if( bChecked && bClearGroup )
        m_pDialog->ClearRadioButtonGroup( m_nButtonGroup );

    m_bChecked = bChecked;
    m_pDialog->SendEvent( EVENT_RADIOBUTTON_CHANGED, bFromInput, this );
}




