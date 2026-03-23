/**
 * \file      GuiListBox.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾@###.com
 *           $$$03¹«Ë¾@###.com
 * \date      2005-07-05 17:02:57
 * \brief     GUIÁĞ±í¿ò
 * 
 *	      GUIÁĞ±í¿ò
 */

#include "../../engine/include/engine.h"
#include "../../engine/include/Spell.h"
#include "../include/guiTypes.h"

const int s_nSBSpace = 8;
const int s_nSBTopOffset = 13;
const int s_nSBBottomOffset = 8;




//--------------------------------------------------------------------------------------
// CGuiListBox class
//--------------------------------------------------------------------------------------

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
CGuiListBox::CGuiListBox( CGuiDialog *pDialog ) :
m_ScrollBar( pDialog )
, m_bSBInit(false)
, m_dwSelectColor(COLOR_ARGB(255,0,0,255))
, m_dwMoveSelColor(COLOR_ARGB(128,0,0,0))
, m_pEditBox(NULL)
, m_pCurEditCol(NULL)
, m_ptOffset(0,0)
{
	m_ScrollBar.SetCanHaveFocus(false);
	m_Type = GUI_CONTROL_LISTBOX;
	m_pDialog = pDialog;

	m_dwStyle = 0;
	m_nSBWidth = pDialog->m_nSBWidth;
	m_nSelected = -1;
	m_nSelStart = 0;
	m_bDrag = false;
	m_nBorder = 6;
	m_nMargin = 5;
	m_nTextHeight = 0;
	m_nMoveSel = -1;

	m_nLastShowItem = 0;
	
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
CGuiListBox::~CGuiListBox()
{
	RemoveAllItems();
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void CGuiListBox::UpdateRects()
{
	CGuiControl::UpdateRects();

	stGuiCtrlBorderInfo* pBorderInfo = m_pDialog->GetCtrlBorderInfo(m_Type);

	stGuiCtrlBorderInfo borderInfo;
	if(GetStyle() & NOSKIN)
	{
		m_ScrollBar.SetStyle(m_ScrollBar.GetStyle() | NOSKIN);

		pBorderInfo = &borderInfo;
		borderInfo.ptBorderLeftTop = stPointI(0,0);
		borderInfo.ptBorderRightBottom = stPointI(0,0);
		borderInfo.ptLeftTop = stPointI(0,0);
		borderInfo.ptRightBottom = stPointI(0,0);
	}

	m_rcSelection = m_rcBoundingBox;
	m_rcSelection.left += pBorderInfo->ptBorderLeftTop.x;
	m_rcSelection.top += pBorderInfo->ptBorderLeftTop.y;
	m_rcSelection.bottom -= pBorderInfo->ptBorderRightBottom.y;
	//m_rcSelection.right -= pBorderInfo->ptBorderRightBottom.x;
	if( GetStyle() & NOSCROLLBAR )
	{

	}
	else
	{
		if(GetStyle() & NOSKIN) 
			m_rcSelection.right -= m_nSBWidth;
		else	
			m_rcSelection.right -= m_nSBWidth + s_nSBSpace;
	}

	m_rcText = m_rcSelection;

	// Update the scrollbar's rects
	if(GetStyle() & NOSKIN)
	{
		m_ScrollBar.SetLocation( m_rcBoundingBox.right - m_nSBWidth, m_rcBoundingBox.top);
		m_ScrollBar.SetSize( m_nSBWidth, m_height);
	}
	else
	{
		m_ScrollBar.SetLocation( m_rcBoundingBox.right - m_nSBWidth - s_nSBSpace, m_rcBoundingBox.top + s_nSBTopOffset);
		m_ScrollBar.SetSize( m_nSBWidth, m_height - (s_nSBTopOffset + s_nSBBottomOffset));
	}

	int iFontHeight = GetDevice()->GetFontHeight( GetFont() );
	if( iFontHeight )
	{
		m_ScrollBar.SetPageSize( m_rcText.Height() / iFontHeight );
		m_ScrollBar.ShowItem( m_nSelected );
	}

	if( GetStyle() & NOSCROLLBAR )
	{
		m_ScrollBar.SetVisible(false);
	}
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param szText : ÃèÊö
 * \param pData : ÃèÊö
 * \param color : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void CGuiListBox::AddItem( const char *szText, void *pData  ,DWORD color)
{
	GuiListBoxItem *pNewItem = new GuiListBoxItem;
	if( !pNewItem )
		return ;
	pNewItem->pData = pData;
	pNewItem->rcActive.SetRect( 0, 0, 0, 0 );
	pNewItem->bSelected = false;
	pNewItem->color = color;	
	m_Items.push_back( pNewItem );
	m_ScrollBar.SetTrackRange( 0, m_Items.size() );
	SetItemText(m_Items.size()-1,0,szText);
	SetItemColor(m_Items.size()-1,0,color);
	
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nIndex : ÃèÊö
 * \param szText : ÃèÊö
 * \param pData : ÃèÊö
 * \param color : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
HRESULT CGuiListBox::InsertItem( int nIndex, const char *szText, void *pData ,DWORD color)
{
	GuiListBoxItem *pNewItem = new GuiListBoxItem;
	if( !pNewItem )
		return E_OUTOFMEMORY;

	//strncpy( pNewItem->strText, szText, 256 );
	//pNewItem->strText[255] = L'\0';
	pNewItem->pData = pData;
	pNewItem->rcActive.SetRect( 0, 0, 0, 0 );
	pNewItem->bSelected = false;
	pNewItem->color = color;
	m_Items.insert(m_Items.begin() + nIndex,pNewItem);
	m_ScrollBar.SetTrackRange( 0, m_Items.size() );
	SetItemText(nIndex,0,szText);
	SetItemColor(nIndex,0,color);

	return S_OK;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nIndex : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void CGuiListBox::RemoveItem( int nIndex )
{
	if( nIndex < 0 || nIndex >= (int)m_Items.size() )
		return;

	GuiListBoxItem *pItem = m_Items[ nIndex ];

	delete pItem;
	m_Items.erase( m_Items.begin() + nIndex );
	m_ScrollBar.SetTrackRange( 0, m_Items.size() );
	if( m_nSelected >= (int)m_Items.size() )
		m_nSelected = m_Items.size() - 1;

	m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );

}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param szText : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void CGuiListBox::RemoveItemByText( char *szText )
{
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pData : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void CGuiListBox::RemoveItemByData( void *pData )
{
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void CGuiListBox::RemoveAllItems()
{
	for( int i = 0; i < (int)m_Items.size(); ++i )
	{
		GuiListBoxItem *pItem = m_Items[ i ];
		delete pItem;
	}

	m_Items.resize(0);
	m_ScrollBar.SetTrackRange( 0, 1 );
	m_nSelected = -1;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nPreviousSelected : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CGuiListBox::GetSelectedIndex( int nPreviousSelected )
{
	if( nPreviousSelected < -1 )
		return -1;

	if( m_dwStyle & MULTISELECTION )
	{
		// Multiple selection enabled. Search for the next item with the selected flag.
		for( int i = nPreviousSelected + 1; i < (int)m_Items.size(); ++i )
		{
			GuiListBoxItem *pItem = m_Items[ i ];

			if( pItem->bSelected )
				return i;
		}

		return -1;
	}
	else
	{
		// Single selection
		return m_nSelected;
	}
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nNewIndex : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void CGuiListBox::SelectItem( int nNewIndex ,bool bSendMsg)
{
	// If no item exists, do nothing.
	if( m_Items.size() == 0 )
		return;

	int nOldSelected = m_nSelected;

	// Adjust m_nSelected
	m_nSelected = nNewIndex;

	// Perform capping
	if( m_nSelected < 0 )
		m_nSelected = -1;

	if( m_nSelected >= (int)m_Items.size() )
		m_nSelected = m_Items.size() - 1;

	if( nOldSelected != m_nSelected )
	{
		if(m_nSelected >= 0)
		{
			if( m_dwStyle & MULTISELECTION )
			{
				m_Items[m_nSelected]->bSelected = true;
			}

			// Update selection start
			m_nSelStart = m_nSelected;

			// Adjust scroll bar
			m_ScrollBar.ShowItem( m_nSelected );
		}
		else
			m_nSelStart = 0;
	}
	if(bSendMsg)
		m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
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
bool CGuiListBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	// Let the scroll bar have a chance to handle it first
	if( m_ScrollBar.HandleKeyboard( uMsg, wParam, lParam ) )
	{
		return true;
	}

	switch( uMsg )
	{
	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_UP:
		case VK_DOWN:
		case VK_NEXT:
		case VK_PRIOR:
		case VK_HOME:
		case VK_END:

			// If no item exists, do nothing.
			if( m_Items.size() == 0 )
				return true;

			int nOldSelected = m_nSelected;

			// Adjust m_nSelected
			switch( wParam )
			{
			case VK_UP: --m_nSelected; break;
			case VK_DOWN: ++m_nSelected; break;
			case VK_NEXT: m_nSelected += m_ScrollBar.GetPageSize() - 1; break;
			case VK_PRIOR: m_nSelected -= m_ScrollBar.GetPageSize() - 1; break;
			case VK_HOME: m_nSelected = 0; break;
			case VK_END: m_nSelected = m_Items.size() - 1; break;
			}

			// Perform capping
			if( m_nSelected < 0 )
				m_nSelected = 0;
			if( m_nSelected >= (int)m_Items.size() )
				m_nSelected = m_Items.size() - 1;

			if( nOldSelected != m_nSelected )
			{
				if( m_dwStyle & MULTISELECTION )
				{
					// Multiple selection

					// Clear all selection
					for( int i = 0; i < (int)m_Items.size(); ++i )
					{
						GuiListBoxItem *pItem = m_Items[i];
						pItem->bSelected = false;
					}

					if( GetKeyState( VK_SHIFT ) < 0 )
					{
						// Select all items from m_nSelStart to
						// m_nSelected
						int nEnd = max( m_nSelStart, m_nSelected );

						for( int n = min( m_nSelStart, m_nSelected ); n <= nEnd; ++n )
							m_Items[n]->bSelected = true;
					}
					else
					{
						m_Items[m_nSelected]->bSelected = true;

						// Update selection start
						m_nSelStart = m_nSelected;
					}
				} else
					m_nSelStart = m_nSelected;

				// Adjust scroll bar

				m_ScrollBar.ShowItem( m_nSelected );

				// Send notification

				m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
			}
			return true;
		}
		break;
	}

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
int CGuiListBox::GetIndexInPoint(POINT pt)
{
	pt.x += m_x;
	pt.y += m_y;
	if( m_Items.size() > 0 && PtInRect( &m_rcSelection, pt ) )
	{
		int nClicked;
		if( m_nTextHeight )
			nClicked = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
		else
			nClicked = -1;

		if( nClicked >= m_ScrollBar.GetTrackPos() &&
			nClicked < (int)m_Items.size() &&
			nClicked < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
		{
			return nClicked;
		}
		
	}
	return -1;
}

void CGuiListBox::OnFocusOut() 
{
	CGuiControl::OnFocusOut();
	if( m_pEditBox && !m_pEditBox->IsFocus() && m_pEditBox->IsVisible())
	{
           m_pEditBox->SetVisible(false);
	}
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
bool CGuiListBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if(CGuiControl::HandleMouse(uMsg,pt,wParam,lParam))
		return true;

	if( !m_bEnabled || !m_bVisible )
		return false;

	// First acquire focus
	if( WM_LBUTTONDOWN == uMsg )
		if( !m_bHasFocus )
			m_pDialog->RequestFocus( this );

	if( WM_LBUTTONDOWN == uMsg || WM_LBUTTONDBLCLK == uMsg)
	{
		FinishEdit();
	}
	// Let the scroll bar handle it first.
	if( m_ScrollBar.HandleMouse( uMsg, pt, wParam, lParam ) )
	{
		return true;
	}

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		// Check for clicks in the text area
		if( m_Items.size() > 0 && PtInRect( &m_rcSelection, pt ) )
		{
			// Compute the index of the clicked item

			int nClicked;
			if( m_nTextHeight )
				nClicked = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
			else
				nClicked = -1;

			// Only proceed if the click falls on top of an item.

			if( nClicked >= m_ScrollBar.GetTrackPos() &&
				nClicked < (int)m_Items.size() &&
				nClicked < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
			{
				//SetCapture( DXUTGetHWND() );
				m_bDrag = true;


				///////////////////////////////////////////////////////////////////////////////////
				// which special col was clicked 
				GuiListBoxItem * pItem = GetItem(nClicked);
				if( pItem )
				{
					stRectI rcItem = m_rcText;
					rcItem.top = m_rcText.top + (nClicked - m_ScrollBar.GetTrackPos()) * m_nTextHeight;
					rcItem.bottom = rcItem.top + m_nTextHeight;

					for(int nCol = 0; nCol < pItem->aCol.size(); nCol++)
					{
					    GuiListBoxCol & col = pItem->aCol[nCol];

					    stRectI rcCol = rcItem; 
					    stRectI rcLayOutText = rcCol;
					    int _ColWidth = col.width * GetDevice()->GetFontWidth(GetFont());
					    int _ColHeight = m_nTextHeight;
                                            
					    if( col.pBmp != NULL )
					    {
					    }
					    else if( col.bEditable)
					    {     						 
						    if( PtInRect( &rcLayOutText, pt ) )
						    {
							    col.bChecked = true;

							    m_pCurEditCol = &pItem->aCol[nCol];
							    if( !m_pEditBox )
							    {    
								    DWORD editID =  m_pDialog->GenerateControlID();
								    m_pDialog->AddEditBox(editID,col.strText,rcCol.left,rcCol.top,_ColWidth,_ColHeight,true,&m_pEditBox);
								    if( m_pEditBox)
								    {
									    m_pEditBox->SetVisible(true);
									    m_pEditBox->ClearText();
									    m_pEditBox->SetText(col.strText);
									    m_pEditBox->SetTextColor(COLOR_ARGB(255,0,0,0));
									    m_pEditBox->SetCaretColor(COLOR_ARGB(255,0,0,0));
									    m_pEditBox->SetStyle(CGuiEditBox::GUI_ES_NOSKIN);
									    m_pDialog->SetFocus(m_pEditBox);
								    }
							    }
							    else
							    {
								    m_pEditBox->SetLocation(rcCol.left,rcCol.top);
								    m_pEditBox->SetSize(_ColWidth,_ColHeight);
								    m_pEditBox->ClearText();
								    m_pEditBox->SetText(col.strText);
								    m_pEditBox->SetVisible(true);
								    m_pDialog->SetFocus(m_pEditBox);
							    }

						    }
						    else 
							    col.bChecked = false;
					    }					  

					    rcItem.left = rcCol.left + _ColWidth; // prepare for the next col
					}
				}
				///////////////////////////////////////////////////////////////////////////////////


				// If this is a double click, fire off an event and exit
				// since the first click would have taken care of the selection
				// updating.
				
				m_pDialog->SendEvent( EVENT_LISTBOX_ITEM_CLICK, true, this );

				if( uMsg == WM_LBUTTONDBLCLK )
				{
					m_pDialog->SendEvent( EVENT_LISTBOX_ITEM_DBLCLK, true, this );
					return true;
				}
				

				m_nSelected = nClicked;
				if( !( wParam & MK_SHIFT ) )
					m_nSelStart = m_nSelected;

				// If this is a multi-selection listbox, update per-item
				// selection data.

				if( m_dwStyle & MULTISELECTION )
				{
					// Determine behavior based on the state of Shift and Ctrl

					GuiListBoxItem *pSelItem = m_Items[ m_nSelected ];
					if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == MK_CONTROL )
					{
						// Control click. Reverse the selection of this item.

						pSelItem->bSelected = !pSelItem->bSelected;
					} else
						if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == MK_SHIFT )
						{
							// Shift click. Set the selection for all items
							// from last selected item to the current item.
							// Clear everything else.

							int nBegin = min( m_nSelStart, m_nSelected );
							int nEnd = max( m_nSelStart, m_nSelected );

							for( int i = 0; i < nBegin; ++i )
							{
								GuiListBoxItem *pItem = m_Items[ i ];
								pItem->bSelected = false;
							}

							for( int i = nEnd + 1; i < (int)m_Items.size(); ++i )
							{
								GuiListBoxItem *pItem = m_Items[ i ];
								pItem->bSelected = false;
							}

							for( int i = nBegin; i <= nEnd; ++i )
							{
								GuiListBoxItem *pItem = m_Items[ i ];
								pItem->bSelected = true;
							}
						} else
							if( ( wParam & (MK_SHIFT|MK_CONTROL) ) == ( MK_SHIFT|MK_CONTROL ) )
							{
								// Control-Shift-click.

								// The behavior is:
								//   Set all items from m_nSelStart to m_nSelected to
								//     the same state as m_nSelStart, not including m_nSelected.
								//   Set m_nSelected to selected.

								int nBegin = min( m_nSelStart, m_nSelected );
								int nEnd = max( m_nSelStart, m_nSelected );

								// The two ends do not need to be set here.

								bool bLastSelected = m_Items[ m_nSelStart ]->bSelected;
								for( int i = nBegin + 1; i < nEnd; ++i )
								{
									GuiListBoxItem *pItem = m_Items[ i ];
									pItem->bSelected = bLastSelected;
								}

								pSelItem->bSelected = true;

								// Restore m_nSelected to the previous value
								// This matches the Windows behavior

								m_nSelected = m_nSelStart;
							} else
							{
								// Simple click.  Clear all items and select the clicked
								// item.


								for( int i = 0; i < (int)m_Items.size(); ++i )
								{
									GuiListBoxItem *pItem = m_Items[ i ];
									pItem->bSelected = false;
								}

								pSelItem->bSelected = true;
							}
				}  // End of multi-selection case

				m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
			}

			return true;
		}
		break;

	case WM_LBUTTONUP:
		{
			//ReleaseCapture();
			m_bDrag = false;

			if( m_nSelected != -1 )
			{
				// Set all items between m_nSelStart and m_nSelected to
				// the same state as m_nSelStart
				int nEnd = max( m_nSelStart, m_nSelected );

				for( int n = min( m_nSelStart, m_nSelected ) + 1; n < nEnd; ++n )
					m_Items[n]->bSelected = m_Items[m_nSelStart]->bSelected;
				m_Items[m_nSelected]->bSelected = m_Items[m_nSelStart]->bSelected;

				// If m_nSelStart and m_nSelected are not the same,
				// the user has dragged the mouse to make a selection.
				// Notify the application of this.
				if( m_nSelStart != m_nSelected )
					m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
			}
			return false;
		}

	case WM_MOUSEMOVE:
		if( m_bDrag )
		{
			// Compute the index of the item below cursor

			int nItem;
			if( m_nTextHeight )
				nItem = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcText.top ) / m_nTextHeight;
			else
				nItem = -1;

			// Only proceed if the cursor is on top of an item.

			if( nItem >= (int)m_ScrollBar.GetTrackPos() &&
				nItem < (int)m_Items.size() &&
				nItem < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
			{
				m_nSelected = nItem;
				m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
			} else
				if( nItem < (int)m_ScrollBar.GetTrackPos() )
				{
					// User drags the mouse above window top
					m_ScrollBar.Scroll( -1 );
					m_nSelected = m_ScrollBar.GetTrackPos();
					m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
				} else
					if( nItem >= m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
					{
						// User drags the mouse below window bottom
						m_ScrollBar.Scroll( 1 );
						m_nSelected = min( (int)m_Items.size(), m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() ) - 1;
						m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
					}
		}
		else
		{
			SetMoveSelItem(pt);
		}
		break;

	case WM_MOUSEWHEEL:
		{
			UINT uLines;
			SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
			int nScrollAmount = int((short)HIWORD(wParam)) / WHEEL_DELTA * uLines;
			m_ScrollBar.Scroll( -nScrollAmount );
			return true;
		}
	}

	return false;
}



/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nIndex : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
GuiListBoxItem *CGuiListBox::GetItem( int nIndex )
{
	if( nIndex < 0 || nIndex >= (int)m_Items.size() )
		return NULL;

	return m_Items[nIndex];
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param item : ÃèÊö
 * \param subItem : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
GuiListBoxCol * CGuiListBox::GetCol(int item,int subItem)
{
	GuiListBoxItem * pItem = GetItem(item);
	if( pItem && (size_t) subItem < pItem->aCol.size())
		return &pItem->aCol[subItem];
	return NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param item : ÃèÊö
 * \param subItem : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
GuiListBoxCol * CGuiListBox::GetAndAddCol(int item,int subItem)
{
	GuiListBoxItem * pItem = GetItem(item);
	if( pItem )
	{
		if((size_t) subItem >= pItem->aCol.size())
		{
			pItem->aCol.resize(subItem+1);
			pItem->aCol[subItem].color = pItem->color;
			pItem->aCol[subItem].fromat = 0;
			pItem->aCol[subItem].width = -1;
			pItem->aCol[subItem].pBmp = NULL;
		}
		return &pItem->aCol[subItem];
	}
	return NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pItem : ÃèÊö
 * \param prect : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiListBox::RenderItem(GuiListBoxItem * pItem,RECT * prect)
{
	CGuiElement* pElement = m_Elements[0];

	stRectI rect = *prect;
	for( size_t i=0; i<pItem->aCol.size(); ++i )
	{
		GuiListBoxCol & col = pItem->aCol[i];
		pElement->FontColor.Current = col.color;
		pElement->dwTextFormat = col.fromat;

		int width;
		if(col.width == -1) width = m_rcSelection.Width();
		else width = col.width * GetDevice()->GetFontWidth(GetFont());
		stRectI rc = rect;
		rc.right = rc.left + width;
		rect.left = rc.right; // prepare for the next col
		if (col.pBmp!=NULL)
		{
			int offsetH=(col.pBmp->GetBitmap(0)->GetHeight()-(rc.bottom-rc.top))/2;
			int offsetW=0;
			switch(col.fromat) {
			case GuiListBoxColFmt_Left:
				break;
			case GuiListBoxColFmt_Center:
				offsetW=(col.pBmp->GetBitmap(0)->GetWidth()-width)/2;
				break;
			case GuiListBoxColFmt_Right:
				offsetW=col.pBmp->GetBitmap(0)->GetWidth()-width;
				break;
			}
			/*if( col.bChecked)
			{
				IBitmap * pBitmap = col.pBmp->GetBitmap(1);
				if( !pBitmap )   pBitmap = col.pBmp->GetBitmap(0);
				if( pBitmap )
				{
					pBitmap->Render(rc.left-offsetW+m_pDialog->GetClientX(),
						rc.top-offsetH+m_pDialog->GetClientY());
				}
			}/*/
			int x = rc.left-offsetW+m_pDialog->GetClientX();
			int y = rc.top-offsetH+m_pDialog->GetClientY();
			if(GetStyle() & USECLIP)
			{
				x -= m_ptOffset.x;
				y -= m_ptOffset.y;
			}
			col.pBmp->RenderAni(x,y,xtimeGetTime(),-1,Blend_Null);
					
			
		}
		else
		{
			if(col.bEditable && m_pEditBox && m_pEditBox->IsVisible() && m_pCurEditCol && m_pCurEditCol == &col)
			{
			     stRectI _rc = m_pEditBox->GetBoundingBox();
			     if(GetStyle() & USECLIP)
			     {
				     OffsetRect(&_rc,-m_ptOffset.x,-m_ptOffset.y);		
			     }
                             m_pDialog->FillRect(&_rc,COLOR_ARGB(255,255,255,255) );
			}
			else
			{
				if( col.pNextCol )
				{
					stRectI rcTemp1(rc.left,rc.top ,rc.right,rc.top+rc.Height()/2);
					if(GetStyle() & USECLIP)
					{
						OffsetRect(&rcTemp1,-m_ptOffset.x,-m_ptOffset.y);
					}
					m_pDialog->DrawText( col.strText,pElement,&rcTemp1 );
					stRectI rcTemp2(rc.left,rc.top+ rc.Height()/2,rc.right,rc.bottom );

					if(GetStyle() & USECLIP)
					{
						OffsetRect(&rcTemp2,-m_ptOffset.x,-m_ptOffset.y);
					}
					m_pDialog->DrawText( col.pNextCol->strText,pElement,&rcTemp2 );
				}
				else
				{
					if(GetStyle() & USECLIP)
					{
						OffsetRect(&rc,-m_ptOffset.x,-m_ptOffset.y);
					}
					m_pDialog->DrawText( col.strText,pElement,&rc );
				}
			}
		}
	}
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
void CGuiListBox::Render( float fElapsedTime )
{
	CGuiControl::Render(fElapsedTime);


	stRectI rcOldClip;
	if(GetStyle() & USECLIP)
	{
		stRectI rcRect = m_rcClip;
		OffsetRect(&rcRect,m_pDialog->GetClientX(),m_pDialog->GetClientY());
		rcOldClip = GetDevice()->SetClipRect(&rcRect);
	}
	CGuiElement* pElement = m_Elements[0];

	stRectI rect = m_rcBoundingBox;
	if(0 == (GetStyle() & NOSKIN))
	{
		//rect.right -= m_nSBWidth + s_nSBSpace;
		m_pDialog->DrawControlBorder(rect,m_Type);
	}

	if( m_Items.size() > 0 )
	{
		stRectI rc = m_rcText;
		stRectI rcSel = m_rcSelection;
		rc.bottom = rc.top + GetDevice()->GetFontHeight( GetFont() );
		int iHeight = rc.bottom - rc.top;

		//Èç¹ûÃ»ÓĞÉè¶¨m_nTextHeightÔòÄ¬ÈÏÎªµ±Ç°×ÖÌåµÄ¸ß¶È
		if( m_nTextHeight <  iHeight )
		{
			m_nTextHeight = iHeight;
		}		
		else
		{
			rc.bottom = rc.top + m_nTextHeight;
		}

		if( !m_bSBInit )
		{
			if( m_nTextHeight )
				m_ScrollBar.SetPageSize( m_rcText.Height() / m_nTextHeight );
			else
				m_ScrollBar.SetPageSize( m_rcText.Height() );
			m_bSBInit = true;
		}

		rc.right = m_rcText.right;
		for( int i = getMax(0,m_ScrollBar.GetTrackPos()); i < (int)m_Items.size(); ++i )
		{
			if( rc.bottom > m_rcText.bottom )
				break;

			GuiListBoxItem *pItem = m_Items[ i ];

			bool bSelectedStyle = false;

			if( !( m_dwStyle & MULTISELECTION ) && i == m_nSelected )
				bSelectedStyle = true;
			else
				if( m_dwStyle & MULTISELECTION )
				{
					if( m_bDrag &&
						( ( i >= m_nSelected && i < m_nSelStart ) ||
						( i <= m_nSelected && i > m_nSelStart ) ) )
						bSelectedStyle = m_Items[m_nSelStart]->bSelected;
					else
						if( pItem->bSelected )
							bSelectedStyle = true;
				}

				if( bSelectedStyle )
				{
					rcSel.top = rc.top; rcSel.bottom = rc.top + m_nTextHeight;
					//m_pDialog->DrawSprite( pSelElement, &rcSel );
					m_pDialog->DrawRect(&rcSel,m_dwSelectColor);
				}

				if ((i==m_nMoveSel)&&( GetStyle() & MOUSEMOVEEVENT ))
				{
					rcSel.top = rc.top; rcSel.bottom = rc.top + m_nTextHeight;
					//m_pDialog->DrawSprite( pSelElement, &rcSel );
					m_pDialog->DrawRect(&rcSel,m_dwMoveSelColor);
				}

				RenderItem(m_Items[i],&rc);
				//pElement->FontColor.Current = m_Items[i]->color;
				//m_pDialog->DrawText( pItem->strText, pElement, &rc );

				rc.OffsetRect( 0, m_nTextHeight );
		}
	}

	if(GetStyle() & USECLIP)
	{
		GetDevice()->SetClipRect(&rcOldClip);
	}
	// Render the scroll bar

	m_ScrollBar.Render( fElapsedTime );

	
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CGuiListBox::GetLastShowItem()
{
	CGuiElement* pElement = m_Elements[0];

	if( m_Items.size() > 0 )
	{
		stRectI rc = m_rcText;
		stRectI rcSel = m_rcSelection;
		rc.bottom = rc.top + GetDevice()->GetFontHeight( GetFont() );
		int iHeight = rc.bottom - rc.top;

		//Èç¹ûÃ»ÓĞÉè¶¨m_nTextHeightÔòÄ¬ÈÏÎªµ±Ç°×ÖÌåµÄ¸ß¶È
		if( m_nTextHeight <  iHeight )
		{
			m_nTextHeight = iHeight;
		}		
		else
		{
			rc.bottom = rc.top + m_nTextHeight;
		}

		if( !m_bSBInit )
		{
			if( m_nTextHeight )
				m_ScrollBar.SetPageSize( m_rcText.Height() / m_nTextHeight );
			else
				m_ScrollBar.SetPageSize( m_rcText.Height() );
			m_bSBInit = true;
		}

		rc.right = m_rcText.right;
		for( int i = getMax(0,m_ScrollBar.GetTrackPos()); i < (int)m_Items.size(); ++i )
		{
			if( rc.bottom > m_rcText.bottom )
				break;

			m_nLastShowItem = i;

			rc.OffsetRect( 0, m_nTextHeight );
		}
	}
	
	return m_nLastShowItem;

}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pt : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void  CGuiListBox::SetMoveSelItem(POINT pt)
{
	int nMoveSel=-1;
	
	CGuiElement* pElement = m_Elements[0];

	if( m_Items.size() > 0 )
	{
		stRectI rc = m_rcText;
		stRectI rcSel = m_rcSelection;
		rc.bottom = rc.top + GetDevice()->GetFontHeight( GetFont() );

		int iHeight = rc.bottom - rc.top;

		//Èç¹ûÃ»ÓĞÉè¶¨m_nTextHeightÔòÄ¬ÈÏÎªµ±Ç°×ÖÌåµÄ¸ß¶È
		if( m_nTextHeight <  iHeight )
		{
			m_nTextHeight = iHeight;
		}		
		else
		{
			rc.bottom = rc.top + m_nTextHeight;
		}

		if( !m_bSBInit )
		{
			if( m_nTextHeight )
				m_ScrollBar.SetPageSize( m_rcText.Height() / m_nTextHeight );
			else
				m_ScrollBar.SetPageSize( m_rcText.Height() );
			m_bSBInit = true;
		}

		rc.right = m_rcText.right;
		for( int i = getMax(0,m_ScrollBar.GetTrackPos()); i < (int)m_Items.size(); ++i )
		{
			if( rc.bottom > m_rcText.bottom )
				break;
			
			if (PtInRect(&rc,pt))
			{
				nMoveSel=i;
				m_rectMoveSel = rc;
				break;
			}

			rc.OffsetRect( 0, m_nTextHeight );
		}
	}

	if (nMoveSel!=m_nMoveSel)
	{
		m_nMoveSel=nMoveSel;
		m_pDialog->SendEvent( EVENT_LISTBOX_MOVESELECTION, true, this );
	}
}


void CGuiListBox::FinishEdit()
{
	if( m_pEditBox) 
	{
		m_pEditBox->SetVisible(false);
		if( m_pCurEditCol )
		{
			m_pCurEditCol->strText = m_pEditBox->GetText();
			m_pCurEditCol = NULL;
		}
	}
}

bool CGuiListBox::OnLButtonClickOnItem()
{
	stPointI ptCursor = Engine_GetCursor()->GetPosition();
	stPointI _ptCursor;
	_ptCursor.x = ptCursor.x - m_pDialog->GetX();
	_ptCursor.y = ptCursor.y - m_pDialog->GetY();

	int iFirstShowItem = GetFirstShowItem();
	int iLastShowItem  = GetLastShowItem();

	stPointI pt = GetLocation();
	stRectI rcTemp;
	rcTemp.left = pt.x ;
	rcTemp.top = pt.y ;
	rcTemp.right = rcTemp.left + GetWidth();
	rcTemp.bottom = rcTemp.top + GetTextHeight();
	
	for(int i = iFirstShowItem;i <= iLastShowItem;i++)
	{
		if( rcTemp.PtInRect(_ptCursor) )
		{
			SetCurItem(i);
			return true;
		}
		rcTemp.OffsetRect(0,GetTextHeight());
	}
	return false;
}


HRESULT CGuiListBox::OnInit() 
{ 
	return m_pDialog->InitControl( &m_ScrollBar ); 
}


typedef std::vector<int>				tAvailableItemList;
typedef tAvailableItemList::iterator	tAvailableItemListIt;

int GetNextItem(const tAvailableItemList& list,int lastSel = -1)
{
	if( list.empty() ) return -1;

	for( int i = 0; i < list.size(); i++ )
	{
		if( list[i] == lastSel )
		{
			if( i == list.size() - 1 ) return list[0];
			else return list[i +1];
		}
	}
	return list[0];
}

void CGuiListBox::FindTextBySpell(char spell,int col)
{
	int iLastSel = GetCurItem();

	tAvailableItemList	aList;
	for( int i = 0; i < (int)m_Items.size(); ++i )
	{
		GuiListBoxItem *pItem = m_Items[ i ];
		if ( pItem->aCol.size() > col )
		{
			const char* pszText = pItem->aCol[col].strText;
			char sp = GetSpellManager().GetFirstSpell(pszText);
			if ( sp == spell )
			{
				aList.push_back(i);
			}
		}
	}

	int iNext = GetNextItem(aList,iLastSel);
	if( iNext != -1 )
	{
		SetCurItem(iNext);
	}
}

bool Compare2String(const char* psz1,const char* psz2)
{
	return Compare2StringBySpell(psz1,psz2);
}