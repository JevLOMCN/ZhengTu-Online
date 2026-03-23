/**
 * \file      GuiStatic.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾@###.com
 *           $$$03¹«Ë¾@###.com
 * \date      2005-07-05 17:02:57
 * \brief     GUI¾²Ì¬¿Ø¼þ
 * 
 *	      GUI¾²Ì¬¿Ø¼þ
 */

#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"

//--------------------------------------------------------------------------------------
// CGuiStatic class
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
CGuiStatic::CGuiStatic( CGuiDialog *pDialog )
: m_ptDragStart(0,0)
, m_ptDragEnd(0,0)
{
    m_Type = GUI_CONTROL_STATIC;
    m_pDialog = pDialog;

    ZeroMemory( &m_strText, sizeof(m_strText) );  
	m_color = COLOR_ARGB(255, 0, 0, 0);
//	m_color = COLOR_ARGB(255,255,255,255);
	m_bDrawBackGround = false;

	m_bPressed = false;  
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
void CGuiStatic::Render(float fElapsedTime )
{    
    CGuiControl::Render(fElapsedTime);

    GUI_CONTROL_STATE iState = GUI_STATE_NORMAL;

    if( m_bEnabled == false )
        iState = GUI_STATE_DISABLED;
        
    CGuiElement* pElement = m_Elements[0];

    pElement->FontColor.Blend( iState, fElapsedTime );
    //pElement->dwTextFormat = m_dwTextFormat;
    m_pDialog->DrawText( m_strText, pElement, &m_rcBoundingBox, FontEffect_Shadow );
	if(m_bDrawBackGround)
	{
		stRectI rc = GetBoundingBox();
		m_pDialog->DrawRect(&rc,m_color);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param strDest : ÃèÊö
 * \param bufferCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
HRESULT CGuiStatic::GetTextCopy( char* strDest, UINT bufferCount )
{
    // Validate incoming parameters
    if( strDest == NULL || bufferCount == 0 )
    {
        return E_INVALIDARG;
    }

    // Copy the window text
    strncpy( strDest, m_strText, bufferCount );
    strDest[bufferCount-1] = 0;

    return S_OK;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param strText : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void CGuiStatic::SetText( const char* strText )
{
    if( strText == NULL )
    {
        m_strText[0] = 0;
        return ;
    }
    
    strncpy( m_strText, strText, MAX_PATH*2 );
	m_strText[MAX_PATH*2-1] = 0;
    return ;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param color : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiStatic::SetBackGroundColor(DWORD color)
{
	m_color = color;
	m_bDrawBackGround = true;
	//Render(10);
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
bool CGuiStatic::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	/*switch( uMsg )
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
	m_pDialog->SendEvent( EVENT_BUTTON_CLICKED, true, this );
	}
	return true;
	}
	}
	}*/
	return false;
}


POINT CGuiStatic::GetDragOffset()
{
	return m_ptDragEnd - m_ptDragStart;
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
bool CGuiStatic::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if(CGuiControl::HandleMouse(uMsg,pt,wParam,lParam))
		return true;

	if( !m_bVisible )
		return false;

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			if( ContainsPoint( pt ) )
			{
				if(!m_bEnabled ) return true;

				m_bPressed = true;

				if( !m_bHasFocus )
					m_pDialog->RequestFocus( this );

				m_ptDragStart = Engine_GetCursor()->GetPosition();
				m_ptDragEnd = Engine_GetCursor()->GetPosition();
				
				if (uMsg==WM_LBUTTONDOWN)
				{
					if( m_Type == GUI_CONTROL_BUTTON) // Ö»ÓÐ°´Å¥²Å·¢ÉùÒô
					{
					     Engine_GetCallBack()->PlayUISound(m_nSoundIndex);
					}
					m_pDialog->SendEvent( EVENT_LBUTTON_DOWN, true, this );
				}

				m_pDialog->SendEvent(EVENT_BUTTON_DRAG_BEGIN,true,this);

				return true;
			}
			break;
		}

	case WM_MOUSEMOVE:
		{
			if( m_bPressed )
			{
				m_ptDragEnd = Engine_GetCursor()->GetPosition();
				m_pDialog->SendEvent(EVENT_BUTTON_DRAG_MOVE,true,this);
				return true;
			}
		}
	case WM_LBUTTONUP:
		{
			if( m_bPressed )
			{
				if( !m_pDialog->m_bKeyboardInput )
					m_pDialog->ClearFocus();

				 m_bPressed = false; 

				 m_ptDragEnd = Engine_GetCursor()->GetPosition();
				// Button click
				if( ContainsPoint( pt ) )
					m_pDialog->SendEvent( EVENT_BUTTON_CLICKED, true, this );

				m_pDialog->SendEvent(EVENT_BUTTON_DRAG_END,true,this);
				
				return true;
			}

			break;
		}
	case WM_RBUTTONUP:
		{
			if( m_bPressed )
			{
				m_bPressed = false;
				//ReleaseCapture();

				if( !m_pDialog->m_bKeyboardInput )
					m_pDialog->ClearFocus();

				m_ptDragEnd = Engine_GetCursor()->GetPosition();
				// Button click
				if( ContainsPoint( pt ) )
					m_pDialog->SendEvent( EVENT_RBUTTON_CLICKED, true, this );
				m_pDialog->SendEvent(EVENT_BUTTON_DRAG_END,true,this);
				return true;
			}

			break;
		}
	};

	return false;
}


void CGuiStatic::OnHotkey() 
{ 
	m_pDialog->SendEvent( EVENT_BUTTON_CLICKED, true, this ); 
}