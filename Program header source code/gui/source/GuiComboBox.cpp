/**
* \file      GuiComboBox.cpp
* \version   $Id$
* \author    $$$03¹«Ë¾@###.com
*           $$$03¹«Ë¾@###.com
* \date      2006-7-19  21:11:00
* \brief     GUI×éºÏ¿ò
* 
*	      GUI×éºÏ¿ò
*/

#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"

//--------------------------------------------------------------------------------------
// CGuiComboBox class
//--------------------------------------------------------------------------------------

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
//--------------------------------------------------------------------------------------
CGuiComboBox::CGuiComboBox( CGuiDialog *pDialog ) :
m_ScrollBar( pDialog )
,m_bSBInit(false)
,m_pEditBox(NULL)
, m_bInEditing(false)
{
	m_ScrollBar.SetCanHaveFocus(false);
	m_Type = GUI_CONTROL_COMBOBOX;
	m_pDialog = pDialog;

	m_nDropHeight = 100;

	m_nSBWidth = pDialog->m_nSBWidth;
	m_bOpened = false;
	m_iSelected = -1;
	m_iFocused = -1;
}


/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
//--------------------------------------------------------------------------------------
CGuiComboBox::~CGuiComboBox()
{
	RemoveAllItems();
}


/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param Color : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
//--------------------------------------------------------------------------------------
void CGuiComboBox::SetTextColor( DWORD Color )
{
	m_TextColor = Color;
	CGuiElement* pElement = m_Elements[ 0 ];

	if( pElement )
		pElement->FontColor.States[GUI_STATE_NORMAL] = Color;

}


/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
//--------------------------------------------------------------------------------------
void CGuiComboBox::UpdateRects()
{

	CGuiControl::UpdateRects();

	m_rcButton = m_rcBoundingBox;

	CGuiElement* pEmt = m_Elements[1];
	m_rcButton.left = m_rcButton.right - pEmt->rcTexture.Width();

	m_rcText = m_rcBoundingBox;
	m_rcText.right = m_rcButton.left;

	m_rcDropdown = m_rcBoundingBox;

	int iFontHeight,iTrueDropHeight;
	if( GUI_CS_DROPUP & GetStyle())
	{
		//OffsetRect( &m_rcDropdown, 0, (int) (0.90f * m_rcText.Height()) );
		m_rcDropdown.bottom = m_rcBoundingBox.top;

		stGuiCtrlBorderInfo* pBorderInfo = m_pDialog->GetCtrlBorderInfo(m_Type);
		// Dropdown box
		iFontHeight = GetDevice()->GetFontHeight(GetFont());

		iTrueDropHeight = getMax((int)GetItemCount(),(int)6) * iFontHeight + pBorderInfo->ptBorderLeftTop.y + pBorderInfo->ptBorderRightBottom.y;

		m_rcDropdown.top -= getMin( iTrueDropHeight,m_nDropHeight );
		m_rcDropdown.right -= m_nSBWidth;
		//m_rcDropdown.right += pBorderInfo->ptBorderRightBottom.x;

		m_rcDropdownText = m_rcDropdown;
		m_rcDropdownText.left += pBorderInfo->ptBorderLeftTop.x;
		m_rcDropdownText.right -= pBorderInfo->ptBorderRightBottom.x;
		m_rcDropdownText.top += pBorderInfo->ptBorderLeftTop.y;
		m_rcDropdownText.bottom -= pBorderInfo->ptBorderRightBottom.y;
   
	}
	else
	{
		//OffsetRect( &m_rcDropdown, 0, (int) (0.90f * m_rcText.Height()) );
		m_rcDropdown.top = m_rcBoundingBox.bottom;

		stGuiCtrlBorderInfo* pBorderInfo = m_pDialog->GetCtrlBorderInfo(m_Type);
		// Dropdown box
		iFontHeight = GetDevice()->GetFontHeight(GetFont());

		iTrueDropHeight = getMax((int)GetItemCount(),(int)6) * iFontHeight + pBorderInfo->ptBorderLeftTop.y + pBorderInfo->ptBorderRightBottom.y;

		m_rcDropdown.bottom += getMin( iTrueDropHeight,m_nDropHeight );
		m_rcDropdown.right -= m_nSBWidth;
		//m_rcDropdown.right += pBorderInfo->ptBorderRightBottom.x;

		m_rcDropdownText = m_rcDropdown;
		m_rcDropdownText.left += pBorderInfo->ptBorderLeftTop.x;
		m_rcDropdownText.right -= pBorderInfo->ptBorderRightBottom.x;
		m_rcDropdownText.top += pBorderInfo->ptBorderLeftTop.y;
		m_rcDropdownText.bottom -= pBorderInfo->ptBorderRightBottom.y;

	}
	
	// Update the scrollbar's rects
	m_ScrollBar.SetLocation( m_rcDropdown.right, m_rcDropdown.top);
	m_ScrollBar.SetSize( m_nSBWidth, m_rcDropdown.Height());

	//DXUTFontNode* pFontNode = GetGlobalDialogResourceManager()->GetFontNode( m_Elements[ 2 ]->iFont );

	if( iFontHeight )
	{
		m_ScrollBar.SetPageSize( m_rcDropdownText.Height() / iFontHeight);

		// The selected item may have been scrolled off the page.
		// Ensure that it is in page again.
		m_ScrollBar.ShowItem( m_iSelected );
	}
}


/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
//--------------------------------------------------------------------------------------
void CGuiComboBox::OnFocusOut()
{
	CGuiButton::OnFocusOut();

	m_bOpened = false;
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
bool CGuiComboBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	const DWORD	REPEAT_MASK = (0x40000000);

	if( !m_bEnabled || !m_bVisible )
		return false;

	// Let the scroll bar have a chance to handle it first
	if( m_ScrollBar.HandleKeyboard( uMsg, wParam, lParam ) )
		return true;

	switch( uMsg )
	{
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case VK_RETURN:
				if( m_bOpened )
				{
					if( m_iSelected != m_iFocused )
					{
						m_iSelected = m_iFocused;
						OnSelectChange();
					}
					m_bOpened = false;

					if( !m_pDialog->m_bKeyboardInput )
						m_pDialog->ClearFocus();

					return true;
				}
				break;

			case VK_F4:
				// Filter out auto-repeats
				if( lParam & REPEAT_MASK )
					return true;

				m_bOpened = !m_bOpened;

				if( !m_bOpened )
				{
					OnSelectChange();

					if( !m_pDialog->m_bKeyboardInput )
						m_pDialog->ClearFocus();
				}

				return true;

			case VK_LEFT:
			case VK_UP:
				if( m_iFocused > 0 )
				{
					m_iFocused--;
					m_iSelected = m_iFocused;

					if( !m_bOpened )
						OnSelectChange();
				}

				return true;

			case VK_RIGHT:
			case VK_DOWN:
				if( m_iFocused+1 < (int)GetItemCount() )
				{
					m_iFocused++;
					m_iSelected = m_iFocused;

					if( !m_bOpened )
						OnSelectChange();
				}

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
bool CGuiComboBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if(CGuiControl::HandleMouse(uMsg,pt,wParam,lParam))
		return true;

	if( !m_bEnabled || !m_bVisible )
		return false;

	if( WM_LBUTTONDOWN == uMsg || WM_LBUTTONDBLCLK == uMsg)
	{
		FinishEdit();
	}

	// Let the scroll bar handle it first.
	if( m_ScrollBar.HandleMouse( uMsg, pt, wParam, lParam ) )
		return true;

	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		{
			if( m_bOpened && PtInRect( &m_rcDropdown, pt ) )
			{
				// Determine which item has been selected
				for( int i=0; i < m_Items.size(); i++ )
				{
					GuiComboBoxItem* pItem = m_Items[ i ];
					if( pItem -> bVisible &&
						PtInRect( &pItem->rcActive, pt ) )
					{
						m_iFocused = i;
					}
				}
				return true;
			}
			break;
		}

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			if( WM_LBUTTONDBLCLK == uMsg && GetStyle() & GUI_CS_EDITTEXT )
			{
				if(!m_bOpened && PtInRect(&m_rcText, pt) ){
					if( !m_pEditBox ){    
						DWORD dwEditID =  m_pDialog->GenerateControlID();
						m_pDialog->AddEditBox(dwEditID,"",m_rcText.left+4,m_rcText.top,m_rcText.Width()-8,m_rcText.Height(),true,&m_pEditBox);
						if( m_pEditBox)
						{
							m_pEditBox->SetVisible(true);
							m_pEditBox->SetEnabled(true);
							m_pEditBox->ClearText();
							//m_pEditBox->SetText(GetText());
							m_pEditBox->SetTextColor(COLOR_ARGB(255,255,255,255));
							m_pEditBox->SetCaretColor(COLOR_ARGB(255,255,255,255));
							m_pEditBox->SetStyle(CGuiEditBox::GUI_ES_NOSKIN);
							m_pDialog->SetFocus(m_pEditBox);
						}
					}
					else{
						m_pEditBox->SetLocation(m_rcText.left+4,m_rcText.top);
						m_pEditBox->SetSize(m_rcText.Width()-8,m_rcText.Height());
						m_pEditBox->ClearText();
						//m_pEditBox->SetText(GetText());
						m_pEditBox->SetVisible(true);
						m_pEditBox->SetEnabled(true);
						m_pDialog->SetFocus(m_pEditBox);
					}
					m_bInEditing = true;
					return true;
				}				
			}

			if( ContainsPoint( pt ) )
			{
				// Pressed while inside the control
				m_bPressed = true;
				//SetCapture( DXUTGetHWND() );

				if( !m_bHasFocus )
					m_pDialog->RequestFocus( this );

				// Toggle dropdown
				if( m_bHasFocus )
				{
					m_bOpened = !m_bOpened;

					if( !m_bOpened )
					{
						if( !m_pDialog->m_bKeyboardInput )
							m_pDialog->ClearFocus();
					}
				}				

				return true;
			}

			// Perhaps this click is within the dropdown
			if( m_bOpened && PtInRect( &m_rcDropdown, pt ) )
			{
				// Determine which item has been selected
				for( int i=m_ScrollBar.GetTrackPos(); i < m_Items.size(); i++ )
				{
					GuiComboBoxItem* pItem = m_Items[i];
					if( pItem -> bVisible &&
						PtInRect( &pItem->rcActive, pt ) )
					{
						m_iFocused = m_iSelected = i;
						OnSelectChange();
						m_bOpened = false;

						if( !m_pDialog->m_bKeyboardInput )
							m_pDialog->ClearFocus();

						break;
					}
				}

				return true;
			}

			// Mouse click not on main control or in dropdown, fire an event if needed
			if( m_bOpened )
			{
				m_iFocused = m_iSelected;
				m_bOpened = false;
			}

			// Make sure the control is no longer in a pressed state
			m_bPressed = false;

			// Release focus if appropriate
			if( !m_pDialog->m_bKeyboardInput )
			{
				m_pDialog->ClearFocus();
			}

			break;
		}

	case WM_LBUTTONUP:
		{
			if( m_bPressed && ContainsPoint( pt ) )
			{
				// Button click
				m_bPressed = false;
				//ReleaseCapture();
				return true;
			}

			break;
		}

	case WM_MOUSEWHEEL:
		{
			int zDelta = (short) HIWORD(wParam) / WHEEL_DELTA;
			if( m_bOpened )
			{
				UINT uLines;
				SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
				m_ScrollBar.Scroll( -zDelta * uLines );
			} else
			{
				if( zDelta > 0 )
				{
					if( m_iFocused > 0 )
					{
						m_iFocused--;
						m_iSelected = m_iFocused;     

						if( !m_bOpened )
						{
							OnSelectChange();
						}
					}          
				}
				else
				{
					if( m_iFocused+1 < (int)GetItemCount() )
					{
						m_iFocused++;
						m_iSelected = m_iFocused;   

						if( !m_bOpened )
							OnSelectChange();
					}
				}
			}
			return true;
		}
	};

	return false;
}


/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
//--------------------------------------------------------------------------------------
void CGuiComboBox::OnHotkey()
{
	if( m_bOpened )
		return;

	if( m_iSelected == -1 )
		return;

	m_iSelected++;

	if( m_iSelected >= (int) m_Items.size() )
		m_iSelected = 0;

	m_iFocused = m_iSelected;
	OnSelectChange();
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
void CGuiComboBox::Render( float fElapsedTime )
{
	CGuiControl::Render(fElapsedTime);

	GUI_CONTROL_STATE iState = GUI_STATE_NORMAL;

	if( !m_bOpened )
		iState = GUI_STATE_HIDDEN;

	// Dropdown box
	CGuiElement* pElement = m_Elements[ 0 ];

	// If we have not initialized the scroll bar page size,
	// do that now.

	if( !m_bSBInit )
	{
		// Update the page size of the scroll bar
		int iFontHeight = GetDevice()->GetFontHeight(GetFont());
		if( iFontHeight )
			m_ScrollBar.SetPageSize( m_rcDropdownText.Height() / iFontHeight );
		else
			m_ScrollBar.SetPageSize( m_rcDropdownText.Height() );
		m_bSBInit = true;
	}

	// Scroll bar
	if( m_bOpened )
	{
		if( (GetStyle() & GUI_CS_SCROLLBAR_NOSKIN) == 0 )
			m_ScrollBar.Render( fElapsedTime );
		m_pDialog->DrawControlBorder(m_rcDropdown,m_Type,-1,true);
	}
	/*
	pElement->TextureColor.Blend( iState, fElapsedTime );
	pElement->FontColor.Blend( iState, fElapsedTime );

	m_pDialog->DrawSprite( pElement, &m_rcDropdown );
	//*/

	// Selection outline
	pElement = m_Elements[ 0 ];

	//DXUTFontNode* pFont = m_pDialog->GetFont( pElement->iFont );
	int iFontHeight = GetDevice()->GetFontHeight(GetFont());
	int curY = m_rcDropdownText.top;
	int nRemainingHeight = m_rcDropdownText.Height();
	//char strDropdown[4096] = {0};

	for( int i = m_ScrollBar.GetTrackPos(); i < m_Items.size(); i++ )
	{
		GuiComboBoxItem* pItem = m_Items[i];

		// Make sure there's room left in the dropdown
		nRemainingHeight -= iFontHeight;
		if( nRemainingHeight < 0 )
		{
			pItem->bVisible = false;
			continue;
		}

		pItem->rcActive.SetRect( m_rcDropdownText.left, curY, m_rcDropdownText.right, curY + iFontHeight );
		curY += iFontHeight;

		//debug
		//int blue = 50 * i;
		//m_pDialog->DrawRect( &pItem->rcActive, 0xFFFF0000 | blue );

		pItem->bVisible = true;

		if( m_bOpened )
		{
			if( (int)i == m_iFocused )
			{
				m_pDialog->DrawRect(&pItem->rcActive,COLOR_ARGB(255,0,0,255));
			}
			m_pDialog->DrawText( pItem->strText, pElement, &pItem->rcActive );
		}
	}

	int nOffsetX = 0;
	int nOffsetY = 0;

	iState = GUI_STATE_NORMAL;

	if( m_bVisible == false )
		iState = GUI_STATE_HIDDEN;
	else if( m_bEnabled == false )
		iState = GUI_STATE_DISABLED;
	else if( m_bPressed )
	{
		iState = GUI_STATE_PRESSED;

		//nOffsetX = 1;
		//nOffsetY = 2;
	}
	else if( m_bMouseOver )
	{
		iState = GUI_STATE_MOUSEOVER;

		// nOffsetX = -1;
		// nOffsetY = -2;
	}
	else if( m_bHasFocus )
		iState = GUI_STATE_FOCUS;

	float fBlendRate = ( iState == GUI_STATE_PRESSED ) ? 0.0f : 0.8f;

	// Button
	pElement = m_Elements[ 1 ];

	// Blend current color
	pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );

	RECT rcWindow = m_rcButton;
	OffsetRect( &rcWindow, nOffsetX, nOffsetY );
	m_pDialog->DrawSprite( pElement, &rcWindow );

	if( m_bOpened )
		iState = GUI_STATE_PRESSED;

	// Main text box
	//TODO: remove magic numbers
	pElement = m_Elements[ 0 ];

	/*
	// Blend current color
	pElement->TextureColor.Blend( iState, fElapsedTime, fBlendRate );
	pElement->FontColor.Blend( iState, fElapsedTime, fBlendRate );

	m_pDialog->DrawSprite( pElement, &m_rcText);
	//*/

	if( 0 == (GetStyle() & GUI_CS_NOSKIN) )   // Not draw the textbox 
		m_pDialog->DrawControlBorder(m_rcText,m_Type);

	/*if( m_iSelected >= 0 && m_iSelected < (int) m_Items.size() )
	{
	GuiComboBoxItem* pItem = m_Items[ m_iSelected ];
	if( pItem != NULL )
	{
	m_pDialog->DrawText( pItem->strText, pElement, &m_rcText );

	}
	}*/
	if(m_pEditBox && m_pEditBox->IsVisible() && m_bInEditing )
	{
		stRectI rcFill = m_rcText;
		rcFill.left += 6;
		rcFill.right -= 6;
		rcFill.top +=3;
		rcFill.bottom -=3;
		m_pDialog->FillRect(&rcFill,COLOR_ARGB(255,0,0,0) );
	}
	else 
	{
		if(!m_strText.empty())
			m_pDialog->DrawText( m_strText.c_str(), pElement, &m_rcText );
	}

}


/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param strText : ÃèÊö
* \param pData : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
//--------------------------------------------------------------------------------------
void CGuiComboBox::AddItem( const char* strText, void* pData )
{
	// Validate parameters
	if( strText== NULL )
	{
		return ;
	}

	// Create a new item and set the data
	GuiComboBoxItem* pItem = new GuiComboBoxItem;
	if( pItem == NULL )
	{
		return ;
		//return DXTRACE_ERR_MSGBOX( L"new", E_OUTOFMEMORY );
	}

	ZeroMemory( pItem, sizeof(GuiComboBoxItem) );
	strncpy( pItem->strText, strText, 255 );
	pItem->pData = pData;

	m_Items.push_back( pItem );

	// Update the scroll bar with new range
	m_ScrollBar.SetTrackRange( 0, m_Items.size() );

	// If this is the only item in the list, it's selected
	if( GetItemCount() == 1 )
	{
		m_iSelected = 0;
		m_iFocused = 0;
		OnSelectChange();
	}

	UpdateRects();
}


/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param index : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
//--------------------------------------------------------------------------------------
void CGuiComboBox::RemoveItem( UINT index )
{
	GuiComboBoxItem* pItem = m_Items[ index ];
	SAFE_DELETE( pItem );
	m_Items.erase( m_Items.begin() + index );
	m_ScrollBar.SetTrackRange( 0, m_Items.size() );
	if( m_iSelected >= m_Items.size() )
		m_iSelected = m_Items.size() - 1;
	UpdateRects();
}


/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
//--------------------------------------------------------------------------------------
void CGuiComboBox::RemoveAllItems()
{
	for( int i=0; i < m_Items.size(); i++ )
	{
		GuiComboBoxItem* pItem = m_Items[i];
		SAFE_DELETE( pItem );
	}

	m_Items.resize(0);
	m_ScrollBar.SetTrackRange( 0, 1 );
	m_iFocused = m_iSelected = -1;
	UpdateRects();
}



/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param strText : ÃèÊö
* \param iStart : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
//--------------------------------------------------------------------------------------
bool CGuiComboBox::ContainsItem( const char* strText, UINT iStart )
{
	return ( -1 != FindItem( strText, iStart ) );
}


/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param strText : ÃèÊö
* \param iStart : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
//--------------------------------------------------------------------------------------
int CGuiComboBox::FindItem( const char* strText, UINT iStart )
{
	if( strText == NULL )
		return -1;

	for( int i = iStart; i < m_Items.size(); i++ )
	{
		GuiComboBoxItem* pItem = m_Items[i];

		if( 0 == strcmp( pItem->strText, strText ) )
		{
			return i;
		}
	}

	return -1;
}


/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
//--------------------------------------------------------------------------------------
void* CGuiComboBox::GetSelectedData()
{
	if( m_iSelected < 0 )
		return NULL;

	GuiComboBoxItem* pItem = m_Items[ m_iSelected ];
	return pItem->pData;
}


/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
//--------------------------------------------------------------------------------------
GuiComboBoxItem* CGuiComboBox::GetSelectedItem()
{
	if( m_iSelected < 0 )
		return NULL;

	return m_Items[ m_iSelected ];
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
void* CGuiComboBox::GetItemData( const char* strText )
{
	int index = FindItem( strText );
	if( index == -1 )
	{
		return NULL;
	}

	GuiComboBoxItem* pItem = m_Items[index];
	if( pItem == NULL )
	{
		DXTRACE_ERR( "std::vector::[]", E_FAIL );
		return NULL;
	}

	return pItem->pData;
}



/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param index : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
//--------------------------------------------------------------------------------------
HRESULT CGuiComboBox::SetSelectedByIndex( UINT index )
{
	if( index >= GetItemCount() )
		return E_INVALIDARG;

	m_iFocused = m_iSelected = index;
	OnSelectChange(false);
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
HRESULT CGuiComboBox::SetSelectedByText( const char* strText )
{
	if( strText == NULL )
		return E_INVALIDARG;

	int index = FindItem( strText );
	if( index == -1 )
		return E_FAIL;

	m_iFocused = m_iSelected = index;
	OnSelectChange(false);

	return S_OK;
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
HRESULT CGuiComboBox::SetSelectedByData( void* pData )
{
	for( int i=0; i < m_Items.size(); i++ )
	{
		GuiComboBoxItem* pItem = m_Items[i];

		if( pItem->pData == pData )
		{
			m_iFocused = m_iSelected = i;
			OnSelectChange(false);
			return S_OK;
		}
	}

	return E_FAIL;
}

void CGuiComboBox::SetText(const char* pszText)
{
	int ix = FindItem(pszText);
	if(ix != -1) m_iFocused = m_iSelected = ix;
	else m_iFocused = m_iSelected = -1;
	m_strText = pszText;
}

void CGuiComboBox::SetWindowText(const char* pszText)
{
	m_strText = pszText;
}

void CGuiComboBox::OnSelectChange(bool bSendMsg)
{
	if(m_iSelected == -1)
		m_strText = "";
	else
		m_strText = GetItemText(m_iSelected);
	if(bSendMsg)
		m_pDialog->SendEvent( EVENT_COMBOBOX_SELECTION_CHANGED, true, this );
}


bool CGuiComboBox::PtInCombox(POINT ptScreen)
{   
	if( m_bOpened )
	{
		stRectI rcWindow = m_rcDropdown;
		rcWindow.OffsetRect( m_pDialog->GetClientX(), m_pDialog->GetClientY() );
		if( PtInRect(&rcWindow,ptScreen))
			return true;
	}
	return ContainsPoint(ptScreen);
}

void CGuiComboBox::FinishEdit()
{
	if( GetStyle() & GUI_CS_EDITTEXT)
	{
		if( m_bInEditing)
		{
			AddItem(m_pEditBox->GetText(),NULL);
			SetText(m_pEditBox->GetText());
			
			m_pEditBox->SetVisible(false);
			m_pEditBox->SetEnabled(false);
			m_bInEditing = false;
		}
	}

}

HRESULT CGuiComboBox::OnInit() 
{ 
	return m_pDialog->InitControl( &m_ScrollBar ); 
}

//GuiComboBoxItem* CGuiComboBox::GetItem( UINT index )
//{ 
//	if ( index < m_Items.size() )
//	{
//		return m_Items[index]; 
//	}
//	return NULL;
//}