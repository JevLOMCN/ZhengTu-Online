/**
* \file      GuiScrollBar.cpp
* \version   $Id$
* \author    $$$03π´Àæ@###.com
*           $$$03π´Àæ@###.com
* \date      2005-07-05 17:02:57
* \brief     GUIπˆ∂ØÃı
* 
*	      GUIπˆ∂ØÃı
*/

#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"

//--------------------------------------------------------------------------------------
// CGuiScrollBar class
//--------------------------------------------------------------------------------------

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param pDialog : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
CGuiScrollBar::CGuiScrollBar( CGuiDialog *pDialog )
{
	m_Type = GUI_CONTROL_SCROLLBAR;
	m_pDialog = pDialog;

	m_bShowThumb = true;
	m_bCanHasFocus = true;

	m_rcUpButton.SetRect(  0, 0, 0, 0 );
	m_rcDownButton.SetRect(  0, 0, 0, 0 );
	m_rcTrack.SetRect(  0, 0, 0, 0 );
	m_rcThumb.SetRect(  0, 0, 0, 0 );
	m_nPosition = 0;
	m_nPageSize = 1;
	m_nLineSize = 1;
	m_nStart = 0;
	m_nEnd = 1;
	m_Arrow = CLEAR;
	m_dArrowTS = 0.0;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
CGuiScrollBar::~CGuiScrollBar()
{
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiScrollBar::UpdateRects()
{
	CGuiControl::UpdateRects();

	// Make the buttons square
	CGuiElement* pEmt;

	stGuiCtrlBorderInfo* pBorderInfo = m_pDialog->GetCtrlBorderInfo(m_Type);
	stGuiCtrlBorderInfo borderInfo;
	if(GetStyle() & NOSKIN)
	{
		borderInfo = *pBorderInfo;
		pBorderInfo = &borderInfo;
		borderInfo.ptBorderLeftTop = stPointI(0,0);
		borderInfo.ptBorderRightBottom = stPointI(0,0);
		//borderInfo.ptLeftTop = stPointI(0,0);
		//borderInfo.ptRightBottom = stPointI(0,0);
	}

	if(GetStyle() & HORIZONTAL2)
	{
		pEmt = m_Elements[5];
		m_rcUpButton.SetRect( 
			m_rcBoundingBox.left + pBorderInfo->ptBorderLeftTop.x, 
			m_rcBoundingBox.top + pBorderInfo->ptBorderLeftTop.y,
			m_rcBoundingBox.left + pEmt->rcTexture.Width(),
			m_rcBoundingBox.top + pBorderInfo->ptBorderLeftTop.y + pEmt->rcTexture.Height() );

		pEmt = m_Elements[7];
		m_rcDownButton.SetRect( 
			m_rcBoundingBox.right - pBorderInfo->ptBorderRightBottom.x - pEmt->rcTexture.Width(), 
			m_rcBoundingBox.top + pBorderInfo->ptBorderLeftTop.y,
			m_rcBoundingBox.right - pBorderInfo->ptBorderRightBottom.x, 
			m_rcBoundingBox.top + pBorderInfo->ptBorderLeftTop.y + pEmt->rcTexture.Height());

	}
	else 
	{
		pEmt = m_Elements[0];
		m_rcUpButton.SetRect( 
			m_rcBoundingBox.left + pBorderInfo->ptBorderLeftTop.x, 
			m_rcBoundingBox.top + pBorderInfo->ptBorderLeftTop.y,
			m_rcBoundingBox.right - pBorderInfo->ptBorderRightBottom.x, 
			m_rcBoundingBox.top + pBorderInfo->ptBorderLeftTop.y + pEmt->rcTexture.Height() );

		pEmt = m_Elements[2];
		m_rcDownButton.SetRect( 
			m_rcBoundingBox.left + pBorderInfo->ptBorderLeftTop.x, 
			m_rcBoundingBox.bottom - pBorderInfo->ptBorderRightBottom.y - pEmt->rcTexture.Height(),
			m_rcBoundingBox.right - pBorderInfo->ptBorderRightBottom.x, 
			m_rcBoundingBox.bottom -  pBorderInfo->ptBorderRightBottom.y);

	}


	if(GetStyle() & HORIZONTAL2)
	{
               // ’‚¿Ô”–∏ˆŒ Ã‚£¨±æ”¶∏√”√pBorderInfo->ptLeftTop.x,œ÷‘⁄‘› ±”√.y(ΩˆΩˆ»œŒ™.y ∫Õ .x ˝÷µ“ª—˘)
		m_rcTrack.SetRect( m_rcBoundingBox.left+ pBorderInfo->ptLeftTop.y, m_rcBoundingBox.top,
			m_rcBoundingBox.right-pBorderInfo->ptRightBottom.y, m_rcBoundingBox.bottom);
	}
	else
	{

		m_rcTrack.SetRect( m_rcUpButton.left, m_rcBoundingBox.top + pBorderInfo->ptLeftTop.y,
			m_rcDownButton.right, m_rcBoundingBox.bottom - pBorderInfo->ptRightBottom.y );
	}


	

	if(GetStyle() & HORIZONTAL2)
	{
		pEmt = m_Elements[9];
		m_rcThumb.top = m_rcUpButton.top  + (m_rcUpButton.Height() - pEmt->rcTexture.Height())/2 ; 
		m_rcThumb.bottom = m_rcThumb.top + pEmt->rcTexture.Height();
	}
	else
	{
		pEmt = m_Elements[4];
		m_rcThumb.left = m_rcUpButton.left + (m_rcUpButton.Width() - pEmt->rcTexture.Width())/2;
		m_rcThumb.right = m_rcThumb.left + pEmt->rcTexture.Width();
	}


	UpdateThumbRect();
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
// Compute the dimension of the scroll thumb
void CGuiScrollBar::UpdateThumbRect()
{
	if( m_nEnd - m_nStart > m_nPageSize )
	{
		if(GetStyle() & HORIZONTAL2)
		{
			//int nThumbHeight = max( m_rcTrack.Height() * m_nPageSize / ( m_nEnd - m_nStart ), SCROLLBAR_MINTHUMBSIZE );
			int nThumbWidth = GetElement(9)->rcTexture.Width();
			int nMaxPosition = m_nEnd - m_nStart - m_nPageSize;
			m_rcThumb.left = m_rcTrack.left + ( m_nPosition - m_nStart ) * ( m_rcTrack.Width() - nThumbWidth )
				/ nMaxPosition;
			m_rcThumb.right = m_rcThumb.left + nThumbWidth;
			m_bShowThumb = true;
		}
		else
		{
			//int nThumbHeight = max( m_rcTrack.Height() * m_nPageSize / ( m_nEnd - m_nStart ), SCROLLBAR_MINTHUMBSIZE );
			// ’‚¿Ô”¶∏√ «∏ˆbug,”¶∏√ «GetElement(4)
			int nThumbHeight = GetElement(2)->rcTexture.Height();
			int nMaxPosition = m_nEnd - m_nStart - m_nPageSize;
			m_rcThumb.top = m_rcTrack.top + ( m_nPosition - m_nStart ) * ( m_rcTrack.Height() - nThumbHeight )
				/ nMaxPosition;
			m_rcThumb.bottom = m_rcThumb.top + nThumbHeight;
			m_bShowThumb = true;
		}
	} 
	else
	{
		if(GetStyle() & HORIZONTAL2)
		{
			 
			m_rcThumb.left = m_rcThumb.right;
			m_bShowThumb = false;
		}
		else 
		{
			// No content to scroll
			m_rcThumb.bottom = m_rcThumb.top;
			m_bShowThumb = false;
		}		
	}
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param nDelta : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
// Scroll() scrolls by nDelta items.  A positive value scrolls down, while a negative
// value scrolls up.
void CGuiScrollBar::Scroll( int nDelta )
{
	// Perform scroll
	m_nPosition += nDelta;

	// Cap position
	Cap();

	// Update thumb position
	UpdateThumbRect();
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param nIndex : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiScrollBar::ShowItem( int nIndex )
{
	// Cap the index

	if( nIndex < 0 )
		nIndex = 0;

	if( nIndex >= m_nEnd )
		nIndex = m_nEnd - 1;

	// Adjust position

	if( m_nPosition > nIndex )
		m_nPosition = nIndex;
	else
		if( m_nPosition + m_nPageSize <= nIndex )
			m_nPosition = nIndex - m_nPageSize + 1;

	UpdateThumbRect();
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param uMsg : √Ë ˆ
* \param wParam : √Ë ˆ
* \param lParam : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
bool CGuiScrollBar::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return false;
}

static bool s_bDrag;
static CGuiScrollBar * s_dragScr;
bool CGuiScrollBar::IsKeepScroll()
{
	if(s_dragScr == this && s_bDrag) return true;
	if(IsFocus())
	{
		if(Engine_GetCursor()->IsLButtonDown()) return true;
	}
	return false;
}

void CGuiScrollBar::UpALine()
{
	m_nPosition -= m_nLineSize;
	if(m_nPosition < m_nStart) m_nPosition = m_nStart;
	UpdateThumbRect();
}

void CGuiScrollBar::DownALine()
{
	m_nPosition += m_nLineSize;
	if(m_nPosition + m_nPageSize > m_nEnd)
	{
		m_nPosition = m_nEnd - m_nPageSize;
		if(m_nPosition < m_nStart) m_nPosition = m_nStart;
	}

	UpdateThumbRect();
}

/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param uMsg : √Ë ˆ
* \param pt : √Ë ˆ
* \param wParam : √Ë ˆ
* \param lParam : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
bool CGuiScrollBar::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if(!IsVisible()) return false;

	if(CanHaveFocus())
	{
		if(CGuiControl::HandleMouse(uMsg,pt,wParam,lParam))
			return true;
		// First acquire focus
		if( WM_LBUTTONDOWN == uMsg )
			if( !m_bHasFocus )
				m_pDialog->RequestFocus( this );
	}

	static int s_ThumbOffsetY;
	static int s_ThumbOffsetX;

	m_LastMouse = pt;
	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			//SetCapture( DXUTGetHWND() );

			// Check for click on up button

			if( PtInRect( &m_rcUpButton, pt ) )
			{
				UpALine();
				m_Arrow = CLICKED_UP;
				m_dArrowTS = Engine_GetTime();
				return true;
			}

			// Check for click on down button

			if( PtInRect( &m_rcDownButton, pt ) )
			{
				DownALine();
				m_Arrow = CLICKED_DOWN;
				m_dArrowTS = Engine_GetTime();
				return true;
			}

			// Check for click on thumb

			if( PtInRect( &m_rcThumb, pt ) )
			{
				s_bDrag = true;
				s_dragScr = this;
			        if(GetStyle() & HORIZONTAL2) 
				{
					s_ThumbOffsetX = pt.x - m_rcThumb.left;
				}
				else 
					s_ThumbOffsetY = pt.y - m_rcThumb.top;
				
				return true;
			}

			// Check for click on track

			if(GetStyle() & HORIZONTAL2)
			{
				if(!m_rcThumb.IsRectEmpty() && m_rcThumb.top <= pt.y &&
					m_rcThumb.bottom > pt.y )
				{
					if( m_rcThumb.left > pt.x &&
						m_rcTrack.left <= pt.x )
					{
						Scroll( -( m_nPageSize - 1 ) );
						return true;
					} else
						if( m_rcThumb.right <= pt.x &&
							m_rcTrack.right > pt.x )
						{
							Scroll( m_nPageSize - 1 );
							return true;
						}
				}
			}
			else
			{
				if(!m_rcThumb.IsRectEmpty() && m_rcThumb.left <= pt.x &&
					m_rcThumb.right > pt.x )
				{
					if( m_rcThumb.top > pt.y &&
						m_rcTrack.top <= pt.y )
					{
						Scroll( -( m_nPageSize - 1 ) );
						return true;
					} else
						if( m_rcThumb.bottom <= pt.y &&
							m_rcTrack.bottom > pt.y )
						{
							Scroll( m_nPageSize - 1 );
							return true;
						}
				}
			}

			break;
		}

	case WM_LBUTTONUP:
		{
			s_bDrag = false;
			s_dragScr = NULL;
			//ReleaseCapture();
			UpdateThumbRect();
			m_Arrow = CLEAR;
			break;
		}

	case WM_MOUSEMOVE:
		{
			if( s_bDrag )
			{
				if(GetStyle() & HORIZONTAL2)
				{
					m_rcThumb.right += pt.x - s_ThumbOffsetX - m_rcThumb.left;
					m_rcThumb.left = pt.x - s_ThumbOffsetX;
					if( m_rcThumb.left < m_rcTrack.left )
						OffsetRect( &m_rcThumb, m_rcTrack.left - m_rcThumb.left,0);
					else
						if( m_rcThumb.right > m_rcTrack.right )
							OffsetRect( &m_rcThumb, m_rcTrack.right - m_rcThumb.right,0 );

					// Compute first item index based on thumb position

					int nMaxFirstItem = m_nEnd - m_nStart - m_nPageSize;  // Largest possible index for first item
					int nMaxThumb = m_rcTrack.Width() - m_rcThumb.Width();  // Largest possible thumb position from the top

					m_nPosition = m_nStart +
						(m_rcThumb.left-m_rcTrack.left + nMaxThumb/(nMaxFirstItem*2) )
						*  nMaxFirstItem/nMaxThumb; // Shift by half a row to avoid last row covered by only one pixel
					
					TRACE("m_nPosition = %d\n",m_nPosition);

				}
				else
				{
					m_rcThumb.bottom += pt.y - s_ThumbOffsetY - m_rcThumb.top;
					m_rcThumb.top = pt.y - s_ThumbOffsetY;
					if( m_rcThumb.top < m_rcTrack.top )
						OffsetRect( &m_rcThumb, 0, m_rcTrack.top - m_rcThumb.top );
					else
						if( m_rcThumb.bottom > m_rcTrack.bottom )
							OffsetRect( &m_rcThumb, 0, m_rcTrack.bottom - m_rcThumb.bottom );

					// Compute first item index based on thumb position

					int nMaxFirstItem = m_nEnd - m_nStart - m_nPageSize;  // Largest possible index for first item
					int nMaxThumb = m_rcTrack.Height() - m_rcThumb.Height();  // Largest possible thumb position from the top

					m_nPosition = m_nStart +
						( m_rcThumb.top - m_rcTrack.top +
						nMaxThumb / ( nMaxFirstItem * 2 ) ) * // Shift by half a row to avoid last row covered by only one pixel
						nMaxFirstItem  / nMaxThumb;

					TRACE("m_nPosition = %d\n",m_nPosition);
				}
				
				return true;
			}

			break;
		}
	case WM_MOUSEWHEEL:
		{
			if(!s_bDrag)
			{
				UINT uLines;
				SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
				int nScrollAmount = int((short)HIWORD(wParam)) / WHEEL_DELTA * uLines * m_nLineSize;
				Scroll( -nScrollAmount );
				return true;
			}
		}
	}

	return false;
}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param fElapsedTime : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiScrollBar::Render( float fElapsedTime )
{
	if(!IsVisible())
		return;

	CGuiControl::Render(fElapsedTime);
	// Check if the arrow button has been held for a while.
	// If so, update the thumb position to simulate repeated
	// scroll.
	if( m_Arrow != CLEAR )
	{
		double dCurrTime = Engine_GetTime();
		if( PtInRect( &m_rcUpButton, m_LastMouse ) )
		{
			switch( m_Arrow )
			{
			case CLICKED_UP:
				if( SCROLLBAR_ARROWCLICK_DELAY < dCurrTime - m_dArrowTS )
				{
					Scroll( -1 );
					m_Arrow = HELD_UP;
					m_dArrowTS = dCurrTime;
				}
				break;
			case HELD_UP:
				if( SCROLLBAR_ARROWCLICK_REPEAT < dCurrTime - m_dArrowTS )
				{
					Scroll( -1 );
					m_dArrowTS = dCurrTime;
				}
				break;
			}
		} else
			if( PtInRect( &m_rcDownButton, m_LastMouse ) )
			{
				switch( m_Arrow )
				{
				case CLICKED_DOWN:
					if( SCROLLBAR_ARROWCLICK_DELAY < dCurrTime - m_dArrowTS )
					{
						Scroll( 1 );
						m_Arrow = HELD_DOWN;
						m_dArrowTS = dCurrTime;
					}
					break;
				case HELD_DOWN:
					if( SCROLLBAR_ARROWCLICK_REPEAT < dCurrTime - m_dArrowTS )
					{
						Scroll( 1 );
						m_dArrowTS = dCurrTime;
					}
					break;
				}
			}
	}

	GUI_CONTROL_STATE iState = GUI_STATE_NORMAL;

	if( m_bVisible == false )
		iState = GUI_STATE_HIDDEN;
	else if( m_bEnabled == false || m_bShowThumb == false )
		iState = GUI_STATE_DISABLED;
	else if( m_bMouseOver )
		iState = GUI_STATE_MOUSEOVER;
	else if( m_bHasFocus )
		iState = GUI_STATE_FOCUS;


	float fBlendRate = ( iState == GUI_STATE_PRESSED ) ? 0.0f : 0.8f;

	// Background track layer
	CGuiElement* pElement;

	if(0 == (GetStyle() & NOSKIN))
	{
		//// Blend current color
		m_pDialog->DrawControlBorder(m_rcBoundingBox,m_Type,-1,true);
	}

	if(GetStyle() & HORIZONTAL2)
	{
		// left Arrow
		if ((m_Arrow == CLICKED_UP)||(m_Arrow == HELD_UP))
			pElement = m_Elements[6];
		else
			pElement = m_Elements[5];

	}
	else
	{
		// Up Arrow
		if ((m_Arrow == CLICKED_UP)||(m_Arrow == HELD_UP))
			pElement = m_Elements[1];
		else
			pElement = m_Elements[0];

	}
	
	stRectI rect;

	// Blend current color
	pElement->TextureColor.Blend( iState , fElapsedTime, fBlendRate );
	rect = m_rcUpButton;

        if(GetStyle() & HORIZONTAL2)
	{
		rect.top += (m_rcUpButton.Height() - pElement->rcTexture.Height())/2;
		rect.bottom = rect.top + pElement->rcTexture.Height();
	}
	else
	{
		//rect.top += (m_rcUpButton.Height() - pElement->rcTexture.Height())/2;
		rect.left += (m_rcUpButton.Width() - pElement->rcTexture.Width())/2;
		rect.right = rect.left + pElement->rcTexture.Width();
		//rect.bottom = rect.top + pElement->rcTexture.Height();

	}
	
	m_pDialog->DrawSprite( pElement, &rect );

	if(GetStyle() & HORIZONTAL2)
	{//right arrow
		if ((m_Arrow == CLICKED_DOWN)||(m_Arrow == HELD_DOWN))
			pElement = m_Elements[8];
		else
			pElement = m_Elements[7];
	}
	else
	{
		// Down Arrow
		if ((m_Arrow == CLICKED_DOWN)||(m_Arrow == HELD_DOWN))
			pElement = m_Elements[3];
		else
			pElement = m_Elements[2];
	}
	
	// Blend current color
	pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
	rect = m_rcDownButton;

	if(GetStyle() & HORIZONTAL2)
	{
		rect.top += (m_rcDownButton.Height() - pElement->rcTexture.Height())/2;
		rect.bottom = rect.top + pElement->rcTexture.Height();

	}
	else
	{
		//rect.top += (m_rcDownButton.Height() - pElement->rcTexture.Height())/2;
		rect.left += (m_rcDownButton.Width() - pElement->rcTexture.Width())/2;
		rect.right = rect.left + pElement->rcTexture.Width();
		//rect.bottom = rect.top + pElement->rcTexture.Height();
	}


	m_pDialog->DrawSprite( pElement, &rect );



	// Thumb button
	if(GetStyle() & HORIZONTAL2)
	{
		pElement = m_Elements[9];
	}
	else
		pElement = m_Elements[4];

	// Blend current color
	pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
	m_pDialog->DrawSprite( pElement, &m_rcThumb );



}


/**
* \brief ºÚ∂Ã√Ë ˆ
* 
* œÍœ∏√Ë ˆ
* 
* \param nStart : √Ë ˆ
* \param nEnd : √Ë ˆ
* \return ∑µªÿ÷µµƒ√Ë ˆ
*/
//--------------------------------------------------------------------------------------
void CGuiScrollBar::SetTrackRange( int nStart, int nEnd )
{
	m_nStart = nStart; m_nEnd = nEnd;
	Cap();
	UpdateThumbRect();
}


//--------------------------------------------------------------------------------------
void CGuiScrollBar::Cap()  // Clips position at boundaries. Ensures it stays within legal range.
{
	if( m_nPosition < m_nStart ||
		m_nEnd - m_nStart <= m_nPageSize )
	{
		m_nPosition = m_nStart;
	}
	else
		if( m_nPosition + m_nPageSize > m_nEnd )
			m_nPosition = m_nEnd - m_nPageSize;
}


bool CGuiScrollBar::CanHaveFocus() 
{
	return (m_bCanHasFocus && m_bVisible && m_bEnabled);
}