/**
 * \file      GuiEditBox.cpp
 * \version   $Id$
 * \author    $$$03π´Àæ@###.com
 *           $$$03π´Àæ@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUIµ•––±‡º≠øÚ
 * 
 *	      GUIµ•––±‡º≠øÚ
 */

#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"
#include "../../engine/include/PhraseFilter.h"
/*
// Static member initialization
HINSTANCE CGuiEditBox::CUniBuffer::s_hDll = NULL;
HRESULT (WINAPI *CGuiEditBox::CUniBuffer::_ScriptApplyDigitSubstitution)( const SCRIPT_DIGITSUBSTITUTE*, SCRIPT_CONTROL*, SCRIPT_STATE* )
= Dummy_ScriptApplyDigitSubstitution;
HRESULT (WINAPI *CGuiEditBox::CUniBuffer::_ScriptStringAnalyse)( HDC, const void *, int, int, int, DWORD, int, SCRIPT_CONTROL*, SCRIPT_STATE*,
const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS* )
= Dummy_ScriptStringAnalyse;
HRESULT (WINAPI *CGuiEditBox::CUniBuffer::_ScriptStringCPtoX)( SCRIPT_STRING_ANALYSIS, int, BOOL, int* )
= Dummy_ScriptStringCPtoX;
HRESULT (WINAPI *CGuiEditBox::CUniBuffer::_ScriptStringXtoCP)( SCRIPT_STRING_ANALYSIS, int, int*, int* )
= Dummy_ScriptStringXtoCP;
HRESULT (WINAPI *CGuiEditBox::CUniBuffer::_ScriptStringFree)( SCRIPT_STRING_ANALYSIS* )
= Dummy_ScriptStringFree;
const SCRIPT_LOGATTR* (WINAPI *CGuiEditBox::CUniBuffer::_ScriptString_pLogAttr)( SCRIPT_STRING_ANALYSIS )
= Dummy_ScriptString_pLogAttr;
const int* (WINAPI *CGuiEditBox::CUniBuffer::_ScriptString_pcOutChars)( SCRIPT_STRING_ANALYSIS )
= Dummy_ScriptString_pcOutChars;
//*/

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool g_bHideCaret = false;   // If true, we don't render the caret.
char g_szCurInput[3]={'\0','\0','\0'};
bool g_bWaitNextChar = false;

CGuiEditBox*	CGuiEditBox::m_pLastFocusEditBox = NULL;

#define ItemSpecialChar(charID)	( ((BYTE)(charID) == (BYTE)ITEM_SPECIAL_CHAR) \
								&& ( ((charID - (BYTE)ITEM_SPECIAL_CHAR) == 0) \
									|| ((charID - (BYTE)ITEM_SPECIAL_CHAR) == ITEM_SPECIAL_CHAR - (BYTE)ITEM_SPECIAL_CHAR)) )
//--------------------------------------------------------------------------------------
// For single-selection listbox, returns the index of the selected item.
// For multi-selection, returns the first selected item after the nPreviousSelected position.
// To search for the first selected item, the app passes -1 for nPreviousSelected.  For
// subsequent searches, the app passes the returned index back to GetSelectedIndex as.
// nPreviousSelected.
// Returns -1 on error or if no item is selected.
CGuiEditBox::CGuiEditBox( CGuiDialog *pDialog )
: CGuiControl(pDialog)
, m_Buffer(this)
, m_uage(USAGE_NAME)
{
	m_Type = GUI_CONTROL_EDITBOX;
	m_pDialog = pDialog;

	m_bCaretOn = true;
	m_bInsertMode = true;
	m_bMouseDrag = false;

	m_dfBlink = GetCaretBlinkTime() * 0.001f;
	m_dfLastBlink = Engine_GetTimer()->GetAbsoluteTime();
	m_nFirstVisible = 0;
	m_TextColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	m_SelTextColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	m_SelBkColor = D3DCOLOR_ARGB( 255, 0, 0, 255 );
	m_CaretColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	m_nCaret = m_nSelStart = 0;
	m_maxCharCount = 1024;
	m_minNumber = 0;
	m_maxNumber = 0x7fffffff;

	//media about
	m_activeMediaType = ACTIVE_MEDIA_NONE;
	m_mediaList.clear();

	//Chat history about
	m_bSaveHistory = false;
	m_strChatHistory.clear();
	m_curChatHistory = 0;
	m_maxChatHistoryNum = 0;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param maxNum : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
int CGuiEditBox::SetMaxChatHistory( int maxNum )
{
	if ( !m_bSaveHistory )
		return 0;
	if ( maxNum <= 0 )
	{
		m_maxCharCount = 0;
		m_bSaveHistory = false;
		return 0;
	}

	m_maxChatHistoryNum = getMin( maxNum, MAX_CHAT_HISTORY_NUM );
	m_bSaveHistory = true;

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
	return m_maxChatHistoryNum;
}
//--------------------------------------------------------------------------------------
int CGuiEditBox::GetChatHistoryNum()
{
	if ( !m_bSaveHistory )
		return 0;
	if ( m_strChatHistory.empty() )
		return 0;

	return m_strChatHistory.size();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param  : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiEditBox::ChangeCurChatHistory( bool bDown /* = true  */ )
{
	if ( !m_bSaveHistory )
		return;

	int sizeChat = GetChatHistoryNum();
	if ( sizeChat <= 0 )
		return;

	if ( m_curChatHistory >= sizeChat )
		m_curChatHistory = sizeChat-1;
	if ( m_curChatHistory < 0 )
		m_curChatHistory = 0;

	

	if ( bDown )
	{
		m_curChatHistory++ ;
		//if ( m_curChatHistory >= sizeChat )
		//	m_curChatHistory -= sizeChat;
		if ( m_curChatHistory >= sizeChat )
			m_curChatHistory = sizeChat-1;

	}
	else
	{
		m_curChatHistory-- ;
		/*if ( m_curChatHistory < 0 )
			m_curChatHistory += sizeChat;*/
		if ( m_curChatHistory < 0 )
			m_curChatHistory = 0;
	}	
	//we insert cur chat history to edit box
	InsertCurChatHistory();
}
/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
char* CGuiEditBox::GetCurChatHistory()
{
	if ( !m_bSaveHistory )
		return NULL;

	int sizeChat = GetChatHistoryNum();
	if( sizeChat <= 0 )
		return NULL;

	if ( (m_curChatHistory < 0) || (m_curChatHistory>=sizeChat) )
		return NULL;

	std::list<stChatInfo>::iterator it = m_strChatHistory.begin();
	for( int i=0;i<m_curChatHistory;i++ )
		it++ ;

	return (*it).szChatStr;

	return NULL;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//////////////////////////////////////////////////////////////////////////
void CGuiEditBox::InsertCurChatHistory()
{
	char* strCur = GetCurChatHistory();

	if ( strCur )
	{
		//First clear all
		ClearText();
		ClearMediaList();

		WCHAR pwszCur[MAX_CHATINFO];
		my_mbstowcs( pwszCur, strCur, -1 );
		m_Buffer.InsertString( 0, pwszCur );
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//////////////////////////////////////////////////////////////////////////
void CGuiEditBox::SaveCurContent()
{
	if ( !m_bSaveHistory	)
		return;

	if ( NULL == GetText() )
		return;

	if ( m_mediaList.size() > 0 )
		return;
	
	for(std :: list<stChatInfo>::iterator iter = m_strChatHistory.begin() ; iter != m_strChatHistory.end() ; ++iter)
	{
		const stChatInfo & info = *iter;
		if(strcmp(info.szChatStr,GetText()) == 0 )
			return;           //not save the same 
	}

	stChatInfo curChatInfo;
	sprintf( curChatInfo.szChatStr, GetText() );
	
	//pre treat
	if ( GetStyle() & GUI_ES_MULTIMEDIA )
		str_del_sp_full( curChatInfo.szChatStr, char( 0xfe ) );

	//push to history list
	if ( m_strChatHistory.empty() )
	{
		m_strChatHistory.push_back( curChatInfo );
		m_curChatHistory = 0;
	}
	else
	{
		if ( m_strChatHistory.size() < MAX_CHAT_HISTORY_NUM )
		{
			m_strChatHistory.push_back( curChatInfo );
			m_curChatHistory = m_strChatHistory.size() - 1;
		}
		else
		{
			m_strChatHistory.pop_front();
			m_strChatHistory.push_back( curChatInfo );
			m_curChatHistory = m_strChatHistory.size() - 1;
		}
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//////////////////////////////////////////////////////////////////////////
CGuiEditBox::~CGuiEditBox()
{
	if( m_pLastFocusEditBox == this )
	{
		m_pLastFocusEditBox = NULL;
	}
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param nCP : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
// PlaceCaret: Set the caret to a character position, and adjust the scrolling if
//             necessary.
//--------------------------------------------------------------------------------------
void CGuiEditBox::PlaceCaret( int nCP )
{
	if ( nCP < 0 )
		return;

	if ( nCP > m_Buffer.GetTextSize() )
		return;

	//assert( nCP >= 0 && nCP <= m_Buffer.GetTextSize() );
	m_nCaret = nCP;

	int nX1st, nX, nX2, nXOffset;
	m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nX1st ); 
	int bTail;
	m_Buffer.XtoCP(nX1st,&m_nFirstVisible,&bTail);
	nXOffset = nX1st;
	m_Buffer.CPtoX( nCP, FALSE, &nX );  

	if( nCP == m_Buffer.GetTextSize() )
		nX2 = nX;
	else
		nX2 = nX + m_Buffer.CharWidth(nCP);

	if( nX < nX1st )
	{
		m_nFirstVisible = nCP;
	}
	else
	{
		if( nX2 > nX1st + m_rcText.Width() )
		{
			int nXNewLeft = nX2 - m_rcText.Width();

			int nCPNew1st, nNewTrail;
			m_Buffer.XtoCP( nXNewLeft, &nCPNew1st, &nNewTrail );

			if(nNewTrail) m_nFirstVisible = nCPNew1st + 1;
			else m_nFirstVisible = nCPNew1st;
		}
	}
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiEditBox::ClearText()
{
	m_Buffer.Clear();
	PlaceCaret( 0 );
	m_nSelStart = 0;
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param wszText : √Ë ˆ
 * \param bSelected : √Ë ˆ
 * \param pData : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CGuiEditBox::SetText( LPCTSTR wszText, bool bSelected ,DWORD* pData)
{
	//USES_CONVERSION;
	//SetText(T2W(wszText),bSelected);
	WCHAR* p = (WCHAR*)_alloca( (strlen(wszText) + 1) * 2 );
	my_mbstowcs(p,wszText,-1);
	SetText(p,bSelected,pData);
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param wszText : √Ë ˆ
 * \param bSelected : √Ë ˆ
 * \param pData : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiEditBox::SetText( LPCWSTR wszText, bool bSelected ,DWORD* pData)
{
	assert( wszText != NULL );

	m_Buffer.SetText( wszText,pData );
	// Move the caret to the end of the text
	PlaceCaret( m_Buffer.GetTextSize() );
	m_nSelStart = bSelected ? 0 : m_nCaret;
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param strDest : √Ë ˆ
 * \param bufferCount : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
HRESULT CGuiEditBox::GetTextCopy( LPWSTR strDest, UINT bufferCount )
{
	assert( strDest );

	wcsncpy( strDest, m_Buffer.GetBuffer(), bufferCount );
	*(strDest + bufferCount - 1) = L'\0';

	return S_OK;
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiEditBox::DeleteSelectionText()
{
	int nFirst = getMin( m_nCaret, m_nSelStart );
	int nLast = getMax( m_nCaret, m_nSelStart );
	// Update caret and selection
	PlaceCaret( nFirst );
	m_nSelStart = m_nCaret;
	// Remove the characters
	// Find the real start and end pos
	// Cause we need to delete the whole media name
	//Get real start pos
	while ( !CanInput( nFirst ) )
	{
		nFirst-- ;
	}
	
	//Get real end pos
	while ( !CanInput( nLast ) )
	{
		nLast++ ;
	}

	//Remove each char
	for( int i = nFirst; i < nLast; ++i )
		m_Buffer.RemoveChar( nFirst );
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiEditBox::UpdateRects()
{
	CGuiControl::UpdateRects();
	if(GetStyle() & GUI_ES_NOSKIN)
	{
		CuttRect9(m_rcBoundingBox,stPointI(0,0),stPointI(0,0),m_rcRender);
	}
	else
	{
		stGuiCtrlBorderInfo* pBorderInfo = m_pDialog->GetCtrlBorderInfo(m_Type);
		if(pBorderInfo)
		{
			CuttRect9(m_rcBoundingBox,pBorderInfo->ptBorderLeftTop,pBorderInfo->ptBorderRightBottom,m_rcRender);
		}
	}
	m_rcText = m_rcRender[0];
	//m_rcText.bottom = m_rcText.top + GetDevice()->GetFontHeight(m_Buffer.GetFontIndex());
	//m_rcRender[0] = m_rcText;
	m_rcText.top += (m_rcText.Height() - GetDevice()->GetFontHeight(GetFont()))/2;
	m_rcText.bottom = m_rcText.top + GetDevice()->GetFontHeight(GetFont());
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CGuiEditBox::CopyToClipboard()
{
	// Copy the selection text to the clipboard
	if(m_dwStyle & GUI_ES_PASSWORD) return;
	if( m_nCaret != m_nSelStart && OpenClipboard( NULL ) )
	{
		EmptyClipboard();

		HGLOBAL hBlock = GlobalAlloc( GMEM_MOVEABLE, sizeof(char) * ( m_Buffer.GetTextSize() * 2 + 1 ) );
		if( hBlock )
		{
			char *pszText  = (char*)GlobalLock( hBlock );
			if( pszText )
			{
				pszText[0] = 0;
				int nFirst = getMin( m_nCaret, m_nSelStart );
				int nLast  = getMax( m_nCaret, m_nSelStart );
				if( nLast - nFirst > 0 )
				{
					my_wcstombs(pszText,m_Buffer.GetBuffer() + nFirst,nLast - nFirst);
					//int len = WideCharToMultiByte(CP_ACP,0,m_Buffer.GetBuffer() + nFirst,nLast - nFirst,pszText,m_Buffer.GetTextSize() * 2 + 1,NULL,NULL);
					//pszText[len] = '\0';
				}

				GlobalUnlock( hBlock );
			}
			SetClipboardData( CF_TEXT, hBlock );
		}
		CloseClipboard();
		// We must not free the object until CloseClipboard is called.
		if( hBlock )
			GlobalFree( hBlock );
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pszText : √Ë ˆ
 * \param pData : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CGuiEditBox::SetSelText(const char* pszText,DWORD* pData)
{
	DeleteSelectionText();

	WCHAR* pwszText = new WCHAR[strlen(pszText) +1];
	my_mbstowcs(pwszText,pszText,-1);
	WCHAR* p = pwszText;
	while(*p)
	{
		if(*p == L'\r' || *p == L'\n')
		{
			*p = 0;
			break;
		}
		++p;
	}

	// Copy all characters up to null.
	if( m_Buffer.InsertString( m_nCaret, pwszText ,pData) )
		PlaceCaret( m_nCaret + lstrlenW( pwszText ) );
	
	m_nSelStart = m_nCaret;

	delete [] pwszText;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CGuiEditBox::PasteFromClipboard()
{
	if(GetStyle() & GUI_ES_READONLY) return false;

	if ( !CanInput( m_nCaret ) )
		return false;

	bool bResult = false;
	if( OpenClipboard( NULL ) )
	{
		HANDLE handle = GetClipboardData( CF_TEXT );
		if( handle )
		{
			// Convert the ANSI string to Unicode, then
			// insert to our buffer.

			char *pszText = (char*)GlobalLock( handle );
			if(pszText)  
			{
				DeleteSelectionText();

				//delete media special char
				str_del( pszText, ITEM_SPECIAL_CHAR );

				//USES_CONVERSION;

				//WCHAR* pwszText = A2W(pszText);
				WCHAR* pwszText = new WCHAR[strlen(pszText) +1];
				my_mbstowcs(pwszText,pszText,-1);
				WCHAR* p = pwszText;
				while(*p)
				{
					if(*p == L'\r' || *p == L'\n')
					{
						*p = 0;
						break;
					}
					++p;
				}

				// Copy all characters up to null.
				if( m_Buffer.InsertString( m_nCaret, pwszText ) )
					PlaceCaret( m_nCaret + lstrlenW( pwszText ) );
				m_nSelStart = m_nCaret;

				delete [] pwszText;
				GlobalUnlock( handle );

				bResult = true;
			}
		}
		CloseClipboard();
	}
	return bResult;
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
bool CGuiEditBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	bool bHandled = false;

	switch( uMsg )
	{
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case VK_HOME:
				PlaceCaret( 0 );
				if( GetKeyState( VK_SHIFT ) >= 0 )
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_END:
				PlaceCaret( m_Buffer.GetTextSize() );
				if( GetKeyState( VK_SHIFT ) >= 0 )
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_INSERT:
				if( 0 == (GetStyle() & GUI_ES_READONLY))
				{
					if( GetKeyState( VK_CONTROL ) < 0 )
					{
						// Control Insert. Copy to clipboard
						CopyToClipboard();
					} 
					else if( GetKeyState( VK_SHIFT ) < 0 )
					{
						// Shift Insert. Paste from clipboard
						if(PasteFromClipboard())
							OnTextChange();
					} 
					else
					{
						// Toggle caret insert mode
						m_bInsertMode = !m_bInsertMode;
					}
				}
				break;

			case VK_DELETE:
				// Check if there is a text selection.
				if( 0 == (GetStyle() & GUI_ES_READONLY))
				{
					if( m_nCaret != m_nSelStart )
					{
						DeleteSelectionText();
						OnTextChange();
					}
					else
					{
						// Deleting one character
						if ( GetStyle() & GUI_ES_MULTIMEDIA )
						{
							int startPos = m_nCaret+1;
							int endPos = startPos;

							while ( !CanInput( startPos ) )
							{
								startPos-- ;
							}

							while ( !CanInput( endPos ) )
							{
								endPos++ ;
							}

							if ( startPos < endPos )
							{
								PlaceCaret( startPos );
								m_nSelStart = m_nCaret;									

								//Remove each char
								for( int i = startPos; i < endPos; ++i )
									m_Buffer.RemoveChar( startPos );
							}
							else
							{
								if( m_Buffer.RemoveChar( m_nCaret ) )
									OnTextChange();	
							}
						}
						else
						{
							if( m_Buffer.RemoveChar( m_nCaret ) )
								OnTextChange();	
						}
										
					}
					ResetCaretBlink();
					bHandled = true;
				}
				break;

			case VK_LEFT:
				if( GetKeyState( VK_CONTROL ) < 0 )
				{
					// Control is down. Move the caret to a new item
					// instead of a character.
					m_Buffer.GetPriorItemPos( m_nCaret, &m_nCaret );
					PlaceCaret( m_nCaret );
				}
				else
					if( m_nCaret > 0 )
						PlaceCaret( m_nCaret - 1 );
				if( GetKeyState( VK_SHIFT ) >= 0 )
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_RIGHT:
				if( GetKeyState( VK_CONTROL ) < 0 )
				{
					// Control is down. Move the caret to a new item
					// instead of a character.
					m_Buffer.GetNextItemPos( m_nCaret, &m_nCaret );
					PlaceCaret( m_nCaret );
				}
				else
					if( m_nCaret < m_Buffer.GetTextSize() )
						PlaceCaret( m_nCaret + 1 );
				if( GetKeyState( VK_SHIFT ) >= 0 )
					// Shift is not down. Update selection
					// start along with the caret.
					m_nSelStart = m_nCaret;
				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_UP:
			case VK_DOWN:
				// Trap up and down arrows so that the dialog
				// does not switch focus to another control.
				bHandled = true;
				break;
			default:
				bHandled = (wParam != VK_ESCAPE && wParam != VK_TAB && wParam != VK_RETURN && (wParam < VK_F1 || wParam > VK_F24));  // Let the application handle Esc.
			}
		}
	}
	return bHandled;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param id : √Ë ˆ
 * \param pName : √Ë ˆ
 * \param  : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
bool CGuiEditBox::AddOneMedia(const DWORD& id, const char* pName, const int& pos /* = -1 */)
{
	if ( !CanHaveInputFocus() )
		return false;
	if ( NULL == pName )
		return false;

	int insertPos = pos;
	int startPos = pos;
	if ( -1 == pos )
	{
		insertPos = m_nCaret;
		startPos = m_nCaret;
	}

	if ( insertPos > m_Buffer.GetTextSize() )
	{
		return false;
	}

	if ( (strlen(GetText()) + strlen(pName) + 2 ) >= GetMaxCharCount() )
	{
		return false;
	}

	//add media to list
	MediaNameData itemName( pName );

	int numMedia = m_mediaList.size();
	m_mediaList.insert( pairMediaNameData( id, itemName ) );

	if ( m_mediaList.size() <= numMedia )		//replace the old one
		return false;

	//we really add one item
	{
		//Can not input among exchange item name
		//Adjust input pos
		while( insertPos < m_Buffer.GetTextSize() && ItemSpecialChar(m_Buffer[insertPos]) )
		{
			insertPos++ ;								
		}
		
		//Add a comma between two item name
		bool bNeedComma = false;
		if ( insertPos > startPos )
		{
			bNeedComma = true;
		}
		else
		{
			if ( insertPos > 0 && ItemSpecialChar(m_Buffer[insertPos-1]) )
			{
				bNeedComma = true;
			}
		}
		
		if ( bNeedComma )
		{
			//Add a comma
			WCHAR cComma = '°¢';
			m_Buffer.InsertChar( insertPos, cComma, NULL );

			PlaceCaret( insertPos+1 );
			m_nSelStart = m_nCaret;
			insertPos = m_nCaret;			
		}

		//Insert item special char
		if ( 0 )		//for each character we see it as wide char
		{
			for( int i=0;i<GetCharacterCount( pName );i++ )
			{
				WCHAR itemSP = ITEM_SPECIAL_CHAR;
				DWORD thisId = id;
				m_Buffer.InsertChar( insertPos, itemSP, &thisId );

				PlaceCaret( insertPos+1 );

				m_nSelStart = m_nCaret;
				insertPos = m_nCaret;
			}
		}
		else
		{
			int count = strlen( pName );
			int i=0;

			while ( (i<count) && pName[i] )
			{
				WCHAR itemSP = ITEM_SPECIAL_CHAR;
				DWORD thisId = id;
				if ( pName[i] & 0x80 )
				{
					itemSP = ITEM_SPECIAL_CHAR;
					m_Buffer.InsertChar( insertPos, itemSP, &thisId );

					i += 2;
				}
				else
				{
					itemSP = (BYTE)ITEM_SPECIAL_CHAR;
					m_Buffer.InsertChar( insertPos, itemSP, &thisId );

					i++ ;
				}

				PlaceCaret( insertPos+1 );

				m_nSelStart = m_nCaret;
				insertPos = m_nCaret;
			}
		}	

		ResetCaretBlink();
	}
	return true;
}
//--------------------------------------------------------------------------------------


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
bool CGuiEditBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
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
			if( !m_bHasFocus )
				m_pDialog->RequestFocus( this );

			if( !ContainsPoint( pt ) )
				return false;
			m_bMouseDrag = true;
			//SetCapture( DXUTGetHWND() );
			// Determine the character corresponding to the coordinates.
			int nCP, nTrail, nX1st;
			m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nX1st );  // X offset of the 1st visible char
			if( SUCCEEDED( m_Buffer.XtoCP( pt.x - m_rcText.left + nX1st, &nCP, &nTrail ) ) )
			{
				bool bDone = false;
				if ( !bDone )
				{
					// Cap at the NULL character.
					if( nTrail && nCP < m_Buffer.GetTextSize() )
						PlaceCaret( nCP + 1 );
					else
						PlaceCaret( nCP );
					m_nSelStart = m_nCaret;
					ResetCaretBlink();
				}
				
			}
			return true;
		}

	case WM_LBUTTONUP:
		//ReleaseCapture();
		m_bMouseDrag = false;
		break;
	case WM_RBUTTONDOWN:
		if( !ContainsPoint( pt ) )
			return false;
		if( !m_bHasFocus )
			m_pDialog->RequestFocus( this );
		return true;
		break;
	case WM_RBUTTONUP:
		{
			if( !ContainsPoint( pt ) )
				return false;
			if(GUI_ES_TEXTBOX & GetStyle())
				return false;
			GetGuiManager()->PopupEditMenu(m_pDialog);
			return true;
		}
		break;
	case WM_MOUSEMOVE:
		if( m_bMouseDrag )
		{
			// Determine the character corresponding to the coordinates.
			int nCP, nTrail, nX1st;
			m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nX1st );  // X offset of the 1st visible char
			if( SUCCEEDED( m_Buffer.XtoCP( pt.x - m_rcText.left + nX1st, &nCP, &nTrail ) ) )
			{
				// Cap at the NULL character.
				if( nTrail && nCP < m_Buffer.GetTextSize() )
					PlaceCaret( nCP + 1 );
				else
					PlaceCaret( nCP );
			}
		}
		break;
	}

	return false;
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiEditBox::OnFocusIn()
{
	CGuiControl::OnFocusIn();
	ResetCaretBlink();
	m_pLastFocusEditBox = this;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CGuiEditBox::SelectAll()
{
	//if( m_nSelStart != m_nCaret )
	{
		m_nSelStart = 0;
		PlaceCaret( m_Buffer.GetTextSize() );
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param posInsert : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
bool CGuiEditBox::CanInput( const int& posInsert )
{
	if ( !CanHaveInputFocus() )
		return false;
	if ( !( GetStyle() & GUI_ES_MULTIMEDIA ) )
		return true;

	if ( posInsert < 0 )
		return false;
	if ( posInsert > m_Buffer.GetTextSize() )
		return true;
	if ( posInsert == m_Buffer.GetTextSize() )
		return true;
	if ( 0 == posInsert )
		return true;

	//Can not input among exchange item name
	const WCHAR* pwszBuffer = m_Buffer.GetBuffer();
	if( ItemSpecialChar(pwszBuffer[posInsert]) )
	{
		if ( ItemSpecialChar(pwszBuffer[posInsert-1]) )
		{
			if ( *( m_Buffer.GetData() + posInsert ) == *( m_Buffer.GetData() + posInsert - 1 ) )
				return false;
		}
	}

	return true;
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
bool CGuiEditBox::MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	switch( uMsg )
	{
	case WM_CHAR:
		{
			switch( (WCHAR)wParam )
			{
				// Backspace
			case VK_BACK:
				{
					if( 0 == (GetStyle() & GUI_ES_READONLY))
					{
						// If there's a selection, treat this
						// like a delete key.
						if( m_nCaret != m_nSelStart )
						{
							DeleteSelectionText();
							OnTextChange();
						}
						else if( m_nCaret > 0 )
						{
							// Move the caret, then delete the char.
							int startPos = m_nCaret -1;
							int endPos = startPos;
							if ( GetStyle() & GUI_ES_MULTIMEDIA )
							{
								//Get real delete range
								while ( !CanInput( startPos ) )
								{
									startPos-- ;
								}
								while ( !CanInput( endPos ) )
								{	
									endPos++ ;
								}

								if ( startPos < endPos )		//we meet media name
								{
									PlaceCaret( startPos );
									m_nSelStart = m_nCaret;									

									//Remove each char
									for( int i = startPos; i < endPos; ++i )
										m_Buffer.RemoveChar( startPos );
								}
								else
								{
									PlaceCaret( m_nCaret - 1 );
									m_nSelStart = m_nCaret;
									m_Buffer.RemoveChar( m_nCaret );
								}
							}
							else
							{
								PlaceCaret( m_nCaret - 1 );
								m_nSelStart = m_nCaret;
								m_Buffer.RemoveChar( m_nCaret );
							}						
							OnTextChange();
						}
						ResetCaretBlink();
					}
					break;
				}
			case 24:        // Ctrl-X Cut
			case VK_CANCEL: // Ctrl-C Copy
				{

					CopyToClipboard();

					if( 0 == (GetStyle() & GUI_ES_READONLY))
					{
						// If the key is Ctrl-X, delete the selection too.
						if( (WCHAR)wParam == 24 )
						{
							DeleteSelectionText();
							OnTextChange();
						}
					}

					break;
				}

				// Ctrl-V Paste
			case 22:
				{
					if( 0 == (GetStyle() & GUI_ES_READONLY) )
					{
						if( PasteFromClipboard() )
							OnTextChange();
					}
					break;
				}

				// Ctrl-A Select All
			case 1:
				SelectAll();
				break;

			case VK_RETURN:
				// Invoke the callback when the user presses Enter.
				m_pDialog->SendEvent( EVENT_EDITBOX_STRING, true, this );
				break;

				// Junk characters we don't want in the string
			case 26:  // Ctrl Z
			case 2:   // Ctrl B
			case 14:  // Ctrl N
			case 19:  // Ctrl S
			case 4:   // Ctrl D
			case 6:   // Ctrl F
			case 7:   // Ctrl G
			case 10:  // Ctrl J
			case 11:  // Ctrl K
			case 12:  // Ctrl L
			case 17:  // Ctrl Q
			case 23:  // Ctrl W
			case 5:   // Ctrl E
			case 18:  // Ctrl R
			case 20:  // Ctrl T
			case 25:  // Ctrl Y
			case 21:  // Ctrl U
			case 9:   // Ctrl I
			case 15:  // Ctrl O
			case 16:  // Ctrl P
			case 27:  // Ctrl [
			case 29:  // Ctrl ]
			case 28:  // Ctrl \ 
				break;

			default:
				{
					WCHAR curInput[2];
					curInput[1] = 0;
					if(GetStyle() & GUI_ES_READONLY) break;
					// If there's a selection and the user
					// starts to type, the selection should
					// be deleted.
					if( m_nCaret != m_nSelStart )
						DeleteSelectionText();

					// If we are in overwrite mode and there is already
					// a char at the caret's position, simply replace it.
					// Otherwise, we insert the char as normal.
					if(g_bWaitNextChar)
					{
						//USES_CONVERSION;
						g_szCurInput[1] = (char)wParam;
						//curInput = A2W(g_szCurInput)[0];
						my_mbstowcs(curInput,g_szCurInput,-1);
						g_bWaitNextChar = false;
					}
					else
					{
						if(((WCHAR)wParam) & 0x80)
						{
							g_bWaitNextChar = true;
							g_szCurInput[0] = (char)wParam;
							return true;
						}
						else
						{
							curInput[0] = (WCHAR)wParam;
						}
					}

					bool bAllowInput = true;
					if(	/*(m_dwStyle & GUI_ES_PASSWORD) ||*/
						(m_dwStyle & GUI_ES_NUMBER)  ||
						(m_dwStyle & GUI_ES_NOCHINESE) )
					{
						// ≤ªƒ‹ ‰»ÎøÌ◊÷∑˚
						bAllowInput = ( (curInput[0] & 0xff00) == 0 );
					}
					if( bAllowInput && (m_dwStyle & GUI_ES_NUMBER) )
					{
						bAllowInput = (L'0' <= curInput[0] && curInput[0] <= L'9');
					}
					
					//Can not input among exchange item name
					if (bAllowInput)
						bAllowInput = CanInput( m_nCaret );


					if(bAllowInput && !(GetStyle() & GUI_ES_READONLY))
					{
						if( !m_bInsertMode && m_nCaret < m_Buffer.GetTextSize() )
						{
							m_Buffer[m_nCaret] = curInput[0];
							PlaceCaret( m_nCaret + 1 );
							m_nSelStart = m_nCaret;
						} 
						else
						{
							// Insert the char
							if( m_Buffer.InsertChar( m_nCaret, curInput[0] ) )
							{
								PlaceCaret( m_nCaret + 1 );
								m_nSelStart = m_nCaret;
							}
						}


						//-----------------------------------------------------------
						// ¥¶¿Ì ‰»Îµƒ±Ì«È◊™“Â◊÷∑˚
						//-----------------------------------------------------------
						if(GetStyle() & GUI_ES_MULTIMEDIA)
						{
							char * szStr = (char*)_alloca(m_nCaret * 2 + 1);
							my_wcstombs(szStr,m_Buffer.GetBuffer(),m_nCaret);
							WORD ch;
							int charNum;
							if(Engine_GetAppation()->ConverPicChar(szStr,ch,charNum))
							{
								while(charNum > 0)
								{
									PlaceCaret( m_nCaret - 1 );
									m_nSelStart = m_nCaret;
									m_Buffer.RemoveChar( m_nCaret );
									charNum--;
								}
								WORD wch = my_mbstowcs(ch);
								
								// Insert the char
								if( m_Buffer.InsertChar( m_nCaret, wch ) )
								{
									PlaceCaret( m_nCaret + 1 );
									m_nSelStart = m_nCaret;
								}
							}
						}
						//-----------------------------------------------------------------
						//-----------------------------------------------------------------

						ResetCaretBlink();
						OnTextChange();
					}
				}
			}
			return true;
		}
	}
	return false;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CGuiEditBox::OnTextChange()
{
	if(m_dwStyle & GUI_ES_NUMBER)
	{
		const WCHAR* wszText = GetTextW();
		wchar_t* p;
		size_t number = wcstoul(wszText,&p,10);
		if(*p) 
		{
			SetText("");
		}
		else
		{
			bool bChange = false;
			if(number < m_minNumber)
			{
				number = m_minNumber;
				bChange = true;
			}
			else if(number > m_maxNumber)
			{
				number = m_maxNumber;
				bChange = true;
			}
			if(bChange)
			{
				WCHAR wszTemp[32];
				swprintf(wszTemp,L"%u",number);
				SetText(wszTemp);
			}
		}
	}
	else 
	{
		///////////////////////////////////////////////////////////////
		// begin filter
		if( !(GetStyle() & GUI_ES_NOFILTER))
		{
			const char * pword = GetText();
			DWORD * pData = GetData();
			char szPword[MAX_CHATINFO];
			memset(szPword,0,sizeof(szPword));

			typedef bool (* PFUNCTIONFILTER)(const char * pword,char * pOutter,size_t nlen);
			PFUNCTIONFILTER pFunction;
			if( USAGE_NAME == m_uage)
			{
				pFunction = G_PhraseFilterName;
			}
			else if(USAGE_CHAT == m_uage)
			{
				pFunction = G_PhraseFilterChat;
			}
			else 
				pFunction = NULL;
			if(pFunction)
			{
				if( (*pFunction)(pword,szPword,MAX_CHATINFO))
				{
					int oldParet = GetCurrPos();
					int m_oldSelStart = m_nSelStart;
					SetText(szPword,false,pData);
					PlaceCaret(oldParet);
					m_nSelStart = m_oldSelStart;
				}
			}			
		}
		///////////////////////////////////////////////////////////////

		const char* szText = GetText();
		size_t len = strlen(szText);
		if( len > m_maxCharCount)
		{
			char *szTemp = (char*)_alloca(sizeof(char) * (m_maxCharCount + 1));
			memcpy(szTemp,szText,m_maxCharCount);
			size_t i;
			for(i=0;i < m_maxCharCount;)
			{
				if(szTemp[i] & 0x80)
				{
					if( i + 1 >= m_maxCharCount)
						break;
					i+=2;
				}
				else
				{
					i++;
				}
			}
			szTemp[i] = 0;
			SetText(szTemp);
		}
	}
	m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param startPos : √Ë ˆ
 * \param fElapsedTime : √Ë ˆ
 * \param mediaWidthBeforeCaret : √Ë ˆ
 * \param mediaWidthBeforeStart : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CGuiEditBox::RenderFrom( int& startPos, const float fElapsedTime, int& mediaWidthBeforeCaret, int& mediaWidthBeforeStart )
{
	if ( startPos >= GetTextLength() )
	{
		return;
	}

	const WCHAR* pwszBuffer = m_Buffer.GetBuffer();
	int endPos = startPos;
	//Draw a item name if the first chat is item special char
	if ( ItemSpecialChar(pwszBuffer[startPos]) )
	{
		char itemName[MAX_NAMESIZE];
		mapMediaNameData::iterator		itor;

		DWORD itemId;
		itor = m_mediaList.end();
		if ( m_Buffer.GetData() )
		{
			itemId = *( m_Buffer.GetData()  + startPos );
			itor = m_mediaList.find( itemId );
		}
		
		int count = 0;
		WCHAR pwszName[MAX_NAMESIZE];
		if ( itor != m_mediaList.end() )
		{
			sprintf( itemName, itor->second.mName );
			count = GetCharacterCount( itemName );
			//Get real count
			if ( startPos == m_nFirstVisible )
			{
				int offsetPos = startPos;
				while ( !CanInput( offsetPos ) )
				{
					count-- ;
					offsetPos-- ;
				}
			}
			
						
			//Draw text
			my_mbstowcs( pwszName, itemName, -1 );

			//Make some offset of item name when at first visible
			int offset = GetCharacterCount( itemName ) - count;
			offset = getMax( offset, 0 );
									
			stRectI rcDraw = m_rcText;
			int left, right;
			int nFirst;
			m_Buffer.CPtoX( m_nFirstVisible, false, &nFirst );
			m_Buffer.CPtoX( startPos, false, &left );
			m_Buffer.CPtoX( startPos+count, false, &right );
			rcDraw.left += left - nFirst;
			rcDraw.right = getMin( m_rcText.left + right - nFirst, m_rcText.right );

			m_Elements[0]->FontColor.Current = 0xffff0000;		//red text color
			m_pDialog->DrawText( (pwszName+offset), m_Elements[0], &rcDraw, FontEffect_None, count, NULL );
		}
		endPos += getMax(count, 1);
		
		//add media width before start and caret
		for( int i=0;i<GetCharacterCount( itemName);i++ )
		{
			int width = WCharWidth( pwszName[i], GetFontIndex() );
			mediaWidthBeforeStart += width;
			if ( endPos <= m_nCaret )
			{
				mediaWidthBeforeCaret += width;
			}			
		}		
	}
	else
	{
		//Get end pos
		while ( !ItemSpecialChar(pwszBuffer[endPos])
				&& endPos < GetTextLength() )
		{
			endPos += 1;
		}

		//Draw without any multimedia
		stRectI rcDraw = m_rcText;
		int left, right;
		int nFirst;
		
		m_Buffer.CPtoX( m_nFirstVisible, false, &nFirst );
		m_Buffer.CPtoX( startPos , false, &left );
		m_Buffer.CPtoX( endPos, false, &right );
		rcDraw.left += left-nFirst;
		rcDraw.right = getMin( m_rcText.left + right - nFirst, m_rcText.right );

		//Draw text
		m_Elements[0]->FontColor.Current = m_TextColor;
		m_pDialog->DrawText( pwszBuffer+startPos, m_Elements[0], &rcDraw, FontEffect_None, endPos-startPos, (m_Buffer.GetData() ? m_Buffer.GetData() + startPos : NULL) );
	}

	startPos = endPos;
	
	//call back
	//return RenderFrom( endPos, fElapsedTime, mediaWidthBeforeCaret, mediaWidthBeforeStart );
		
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
void CGuiEditBox::Render( float fElapsedTime )
{

	if ( GetStyle() & GUI_ES_MULTIMEDIA )
	{
		CGuiControl::Render(fElapsedTime);

		HRESULT hr;
		int nSelStartX = 0, nCaretX = 0;  // Left and right X cordinates of the selection region

		// Render the control graphics
		if(!(m_dwStyle & GUI_ES_NOSKIN))
		{
			m_pDialog->DrawControlBorder(m_rcBoundingBox,m_Type);
		}

		//
		// Compute the X coordinates of the first visible character.
		//
		int nXFirst;
		m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nXFirst );

		//
		// Compute the X coordinates of the selection rectangle
		//
		hr = m_Buffer.CPtoX( m_nCaret, FALSE, &nCaretX );
		if( m_nCaret != m_nSelStart )
			hr = m_Buffer.CPtoX( m_nSelStart, FALSE, &nSelStartX );
		else
			nSelStartX = nCaretX;

		//Draw selection rect
		stRectI rcSelection;
		int nSelLeftX = getMin(nCaretX,nSelStartX);
		int nSelRightX = getMax(nCaretX,nSelStartX);

		rcSelection.SetRect( nSelLeftX, m_rcText.top, nSelRightX, m_rcText.bottom );
		rcSelection.OffsetRect( m_rcText.left - nXFirst, 0 );
		rcSelection &= m_rcText;
		m_pDialog->DrawRect( &rcSelection, m_SelBkColor );

		
		int mediaWidthBeforeCaret = 0;
		int mediaWidthBeforeStart = 0;
		int startPos = m_nFirstVisible;
		
		//call render
		while ( startPos < GetTextLength() )
		{
			RenderFrom( startPos, fElapsedTime, mediaWidthBeforeCaret, mediaWidthBeforeStart );
		}
		

		//
		// Blink the caret
		//
		if( Engine_GetTimer()->GetAbsoluteTime() - m_dfLastBlink >= m_dfBlink )
		{
			m_bCaretOn = !m_bCaretOn;
			m_dfLastBlink = Engine_GetTimer()->GetAbsoluteTime();
		}

		//
		// Render the caret if this control has the focus
		//
		if( m_bHasFocus && m_pDialog->IsFocus() && m_bCaretOn && !g_bHideCaret )
		{
			// Start the rectangle with insert mode caret
			RECT rcCaret = { m_rcText.left - nXFirst + nCaretX - 1, m_rcText.top,
				m_rcText.left - nXFirst + nCaretX + 1, m_rcText.bottom };

			// If we are in overwrite mode, adjust the caret rectangle
			// to fill the entire character.
			if( !m_bInsertMode )
			{
				// Obtain the right edge X coord of the current character
				//int nRightEdgeX;
				//m_Buffer.CPtoX( m_nCaret, TRUE, &nRightEdgeX );
				//rcCaret.right = m_rcText.left - nXFirst + nRightEdgeX;
				rcCaret.right += GetDevice()->GetFontWidth(GetFont());
			}

			m_pDialog->DrawRect( &rcCaret, m_CaretColor );
		}
	}
	else
	{
		CGuiControl::Render(fElapsedTime);

		HRESULT hr;
		int nSelStartX = 0, nCaretX = 0;  // Left and right X cordinates of the selection region

		// Render the control graphics
		if(!(m_dwStyle & GUI_ES_NOSKIN))
		{
			m_pDialog->DrawControlBorder(m_rcBoundingBox,m_Type);
			/*for( int e = 0; e < 9; ++e )
			{
			CGuiElement* pElement = m_Elements[ e ];
			pElement->TextureColor.Blend( GUI_STATE_NORMAL, fElapsedTime );
			if(e == 0)
			{
			stRectI rc = m_rcRender[0];
			rc.InflateRect(1,1);
			m_pDialog->DrawSprite( pElement, &rc );
			}
			else
			{
			m_pDialog->DrawSprite( pElement, &m_rcRender[e] );
			}
			}*/
		}

		//
		// Compute the X coordinates of the first visible character.
		//
		int nXFirst;
		m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nXFirst );

		//
		// Compute the X coordinates of the selection rectangle
		//
		hr = m_Buffer.CPtoX( m_nCaret, FALSE, &nCaretX );
		if( m_nCaret != m_nSelStart )
			hr = m_Buffer.CPtoX( m_nSelStart, FALSE, &nSelStartX );
		else
			nSelStartX = nCaretX;

		//
		// Render the selection rectangle
		//
		stRectI rcSelection;  // Make this available for rendering selected text

		WCHAR* pwszPassword;
		const WCHAR* pwszBuffer;
		const WCHAR* ptemp;
		if(m_dwStyle & GUI_ES_PASSWORD)
		{
			ptemp = m_Buffer.GetBuffer();
			size_t bufferlen = wcslen(ptemp);
			pwszPassword = (WCHAR*)_alloca( (bufferlen + 1) * sizeof(WCHAR));
			for(int i=0;i<bufferlen;i++)
			{
				if (ptemp[i]&0x80)
				{
					pwszPassword[i] = '£™';
				}
				else
				{
					pwszPassword[i] = '*';
				}
			}

			pwszPassword[bufferlen] = 0;
			pwszBuffer = pwszPassword;
					
			//bufferlen =_mbstrlen(GetText());
			//PlaceCaret(bufferlen);
			//m_nSelStart = m_nCaret;
		}
		else
		{
			pwszBuffer = m_Buffer.GetBuffer();
		}

		if( m_nCaret != m_nSelStart && m_bHasFocus )
		{
			int nSelLeftX = getMin(nCaretX,nSelStartX);
			int nSelRightX = getMax(nCaretX,nSelStartX);

			rcSelection.SetRect( nSelLeftX, m_rcText.top, nSelRightX, m_rcText.bottom );
			rcSelection.OffsetRect( m_rcText.left - nXFirst, 0 );
			rcSelection &= m_rcText;
			m_pDialog->DrawRect( &rcSelection, m_SelBkColor );

			int nFirstToRender = getMax( m_nFirstVisible, getMin( m_nSelStart, m_nCaret ) );
			int nNumChatToRender = getMax( m_nSelStart, m_nCaret ) - nFirstToRender;

			//
			// Render the text
			//
			// Element 0 for text
			m_Elements[0]->FontColor.Current = m_TextColor;
			m_pDialog->DrawText( pwszBuffer + m_nFirstVisible, m_Elements[0], &m_rcText ,FontEffect_None,nFirstToRender-m_nFirstVisible,(m_Buffer.GetData() ? m_Buffer.GetData() + m_nFirstVisible : NULL));

			m_Elements[0]->FontColor.Current = m_SelTextColor;
			m_pDialog->DrawText( pwszBuffer + nFirstToRender,
				m_Elements[0], &rcSelection, FontEffect_None, nNumChatToRender ,(m_Buffer.GetData() ? m_Buffer.GetData() + nFirstToRender : NULL));

			stRectI rc = m_rcText;
			rc.left = rcSelection.right;
			m_Elements[0]->FontColor.Current = m_TextColor;
			m_pDialog->DrawText( pwszBuffer + nFirstToRender + nNumChatToRender, m_Elements[0], &rc,FontEffect_None,-1,(m_Buffer.GetData() ? m_Buffer.GetData() + nFirstToRender + nNumChatToRender : NULL));
		}
		else
		{
			//
			// Render the text
			//
			// Element 0 for text
			m_Elements[0]->FontColor.Current = m_TextColor;
			m_pDialog->DrawText( pwszBuffer + m_nFirstVisible, m_Elements[0], &m_rcText,FontEffect_None,-1,(m_Buffer.GetData() ? m_Buffer.GetData() + m_nFirstVisible : NULL) );
		}

		//
		// Blink the caret
		//
		if( Engine_GetTimer()->GetAbsoluteTime() - m_dfLastBlink >= m_dfBlink )
		{
			m_bCaretOn = !m_bCaretOn;
			m_dfLastBlink = Engine_GetTimer()->GetAbsoluteTime();
		}

		//
		// Render the caret if this control has the focus
		//
		if( m_bHasFocus && m_pDialog->IsFocus() && m_bCaretOn && !g_bHideCaret && !(GetStyle() & GUI_ES_TEXTBOX))
		{
			// Start the rectangle with insert mode caret
			RECT rcCaret = { m_rcText.left - nXFirst + nCaretX - 1, m_rcText.top,
				m_rcText.left - nXFirst + nCaretX + 1, m_rcText.bottom };

			// If we are in overwrite mode, adjust the caret rectangle
			// to fill the entire character.
			if( !m_bInsertMode )
			{
				// Obtain the right edge X coord of the current character
				//int nRightEdgeX;
				//m_Buffer.CPtoX( m_nCaret, TRUE, &nRightEdgeX );
				//rcCaret.right = m_rcText.left - nXFirst + nRightEdgeX;
				rcCaret.right += GetDevice()->GetFontWidth(GetFont());
			}

			m_pDialog->DrawRect( &rcCaret, m_CaretColor );
		}
	}
	
}


#define IN_FLOAT_CHARSET( c ) \
	( (c) == L'-' || (c) == L'.' || ( (c) >= L'0' && (c) <= L'9' ) )

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pNumbers : √Ë ˆ
 * \param nCount : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CGuiEditBox::ParseFloatArray( float *pNumbers, int nCount )
{
	int nWritten = 0;  // Number of floats written
	const WCHAR *pToken, *pEnd;
	WCHAR wszToken[60];

	pToken = m_Buffer.GetBuffer();
	while( nWritten < nCount && *pToken != L'\0' )
	{
		// Skip leading spaces
		while( *pToken == L' ' )
			++pToken;

		if( *pToken == L'\0' )
			break;

		// Locate the end of number
		pEnd = pToken;
		while( IN_FLOAT_CHARSET( *pEnd ) )
			++pEnd;

		// Copy the token to our buffer
		int nTokenLen = min( sizeof(wszToken) / sizeof(wszToken[0]) - 1, int(pEnd - pToken) );
		wcsncpy( wszToken, pToken, nTokenLen );
		wszToken[nTokenLen] = L'\0';
		*pNumbers = (float)wcstod( wszToken, NULL );
		++nWritten;
		++pNumbers;
		pToken = pEnd;
	}
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pNumbers : √Ë ˆ
 * \param nCount : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CGuiEditBox::SetTextFloatArray( const float *pNumbers, int nCount )
{
	WCHAR wszBuffer[512];
	WCHAR *pNext = wszBuffer;

	for( int i = 0; i < nCount; ++i )
	{
		pNext += _snwprintf( pNext, 512 - (pNext - wszBuffer), L"%.4f ", pNumbers[i] );
	}

	// Don't want the last space
	if( nCount > 0 )
		*(pNext - 1) = L'\0';
	else
		*pNext = L'\0';

	SetText( wszBuffer );
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
/*
//--------------------------------------------------------------------------------------
void CGuiEditBox::CUniBuffer::InitializeUniscribe()
{

char szPath[MAX_PATH+1];
if( !::GetSystemDirectory( szPath, MAX_PATH+1 ) )
return;
strcat( szPath, UNISCRIBE_DLLNAME );
s_hDll = LoadLibrary( szPath );
if( s_hDll )
{
FARPROC Temp;
GETPROCADDRESS( s_hDll, ScriptApplyDigitSubstitution, Temp );
GETPROCADDRESS( s_hDll, ScriptStringAnalyse, Temp );
GETPROCADDRESS( s_hDll, ScriptStringCPtoX, Temp );
GETPROCADDRESS( s_hDll, ScriptStringXtoCP, Temp );
GETPROCADDRESS( s_hDll, ScriptStringFree, Temp );
GETPROCADDRESS( s_hDll, ScriptString_pLogAttr, Temp );
GETPROCADDRESS( s_hDll, ScriptString_pcOutChars, Temp );
}
}



//--------------------------------------------------------------------------------------
void CGuiEditBox::CUniBuffer::UninitializeUniscribe()
{
if( s_hDll )
{
PLACEHOLDERPROC( ScriptApplyDigitSubstitution );
PLACEHOLDERPROC( ScriptStringAnalyse );
PLACEHOLDERPROC( ScriptStringCPtoX );
PLACEHOLDERPROC( ScriptStringXtoCP );
PLACEHOLDERPROC( ScriptStringFree );
PLACEHOLDERPROC( ScriptString_pLogAttr );
PLACEHOLDERPROC( ScriptString_pcOutChars );

FreeLibrary( s_hDll );
s_hDll = NULL;
}
}
//*/

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param nNewSize : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
bool CGuiEditBox::CUniBuffer::Grow( int nNewSize )
{
	// If the current size is already the maximum allowed,
	// we can't possibly allocate more.
	if( m_nBufferSize == GUI_MAX_EDITBOXLENGTH )
		return false;

	int nAllocateSize = ( nNewSize == -1 || nNewSize < m_nBufferSize * 2 ) ? ( m_nBufferSize ? m_nBufferSize * 2 : 256 ) : nNewSize * 2;


	// Cap the buffer size at the maximum allowed.
	if( nAllocateSize > GUI_MAX_EDITBOXLENGTH )
		nAllocateSize = GUI_MAX_EDITBOXLENGTH;

	//Engine_WriteLogF("nAllocateSize = %u\n",nAllocateSize);
	WCHAR *pTempBuffer = new WCHAR[nAllocateSize];
	if( !pTempBuffer )
		return false;
	if( m_pwszBuffer )
		memcpy( pTempBuffer, m_pwszBuffer, (m_nTextSize + 1) * sizeof(WCHAR) );
	delete[] m_pwszBuffer;
	m_pwszBuffer = pTempBuffer;

	if( IsMultimedia() )
	{
		DWORD* pData = new DWORD[nAllocateSize];
		if(m_pData)
			memcpy(pData,m_pData,(m_nTextSize + 1) * sizeof(DWORD));
		delete [] m_pData;
		m_pData = pData;
	}

	m_nBufferSize = nAllocateSize;
	return true;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
/*
//--------------------------------------------------------------------------------------
// Uniscribe -- Analyse() analyses the string in the buffer
//--------------------------------------------------------------------------------------
HRESULT CGuiEditBox::CUniBuffer::Analyse()
{
if( m_Analysis )
_ScriptStringFree( &m_Analysis );

SCRIPT_CONTROL ScriptControl; // For uniscribe
SCRIPT_STATE   ScriptState;   // For uniscribe
ZeroMemory( &ScriptControl, sizeof(ScriptControl) );
ZeroMemory( &ScriptState, sizeof(ScriptState) );
_ScriptApplyDigitSubstitution ( NULL, &ScriptControl, &ScriptState );

//DXUTFontNode* pFontNode = DXUTGetGlobalDialogResourceManager()->GetFontNode( m_iFont );

HRESULT hr = _ScriptStringAnalyse( pFontNode->pFont ? pFontNode->pFont->GetDC() : NULL,
m_pwszBuffer,
m_nTextSize + 1,  // NULL is also analyzed.
m_nTextSize * 3 / 2 + 16,
-1,
SSA_BREAK | SSA_GLYPHS | SSA_FALLBACK | SSA_LINK,
0,
&ScriptControl,
&ScriptState,
NULL,
NULL,
NULL,
&m_Analysis );
if( SUCCEEDED( hr ) )
m_bAnalyseRequired = false;  // Analysis is up-to-date
return hr;
}
//*/

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pEditBox : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
CGuiEditBox::CUniBuffer::CUniBuffer( CGuiEditBox* pEditBox )
{
	m_pEditBox = pEditBox;
	m_pwszBuffer = NULL;
	m_pData = NULL;

	m_nBufferSize = 0;
	m_nTextSize = 0;
	
	//m_bAnalyseRequired = true;
	//m_Analysis = NULL;
	//m_iFont = 0;
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
CGuiEditBox::CUniBuffer::~CUniBuffer()
{
	delete[] m_pwszBuffer;
	delete[] m_pData;
	/*
	if( m_Analysis )
	_ScriptStringFree( &m_Analysis );
	//*/
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
bool CGuiEditBox::CUniBuffer::IsMultimedia()
{
	return (m_pEditBox->GetStyle() & CGuiMLEditBox::GUI_ES_MULTIMEDIA) != 0;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param nInitialSize : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
void CGuiEditBox::CUniBuffer::Init( int nInitialSize )
{
	if(m_pwszBuffer) delete [] m_pwszBuffer;
	m_pwszBuffer = new WCHAR[nInitialSize];
	*m_pwszBuffer = 0;

	if(m_pData) delete [] m_pData;
	if(IsMultimedia())
		m_pData = new DWORD[nInitialSize];
	else
		m_pData = NULL;
	m_nBufferSize = nInitialSize;
	m_nTextSize = 0;

	//m_bAnalyseRequired = true;
	//m_Analysis = NULL;
	//m_iFont = 0;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param nSize : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
bool CGuiEditBox::CUniBuffer::SetBufferSize( int nSize )
{
	while( m_nBufferSize < nSize )
	{
		if( !Grow() )
			return false;
	}
	return true;
}


//--------------------------------------------------------------------------------------
WCHAR& CGuiEditBox::CUniBuffer::operator[]( int n )  // No param checking
{
	// This version of operator[] is called only
	// if we are asking for write access, so
	// re-analysis is required.
	//m_bAnalyseRequired = true;
	return m_pwszBuffer[n];
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiEditBox::CUniBuffer::Clear()
{
	*m_pwszBuffer = L'\0';
	m_nTextSize = 0;
	//m_bAnalyseRequired = true;
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param nIndex : √Ë ˆ
 * \param wChar : √Ë ˆ
 * \param data : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
// Inserts the char at specified index.
// If nIndex == -1, insert to the end.
//--------------------------------------------------------------------------------------
bool CGuiEditBox::CUniBuffer::InsertChar( int nIndex, WCHAR wChar ,DWORD* data)
{
	assert( nIndex >= 0 );

	if( nIndex > m_nTextSize )
		return false;  // invalid index

	// Check for maximum length allowed
	if( m_nTextSize + 1 >= GUI_MAX_EDITBOXLENGTH )
		return false;
	
	if ( m_pEditBox->GetStyle() & GUI_ES_MULTIMEDIA )
	{
		if ( (strlen(m_pEditBox->GetText()) + (wChar & 0xff00 ? 2 : 1) ) >= m_pEditBox->GetMaxCharCount() )
		{
			return false;
		}
	}
	

	if( m_nTextSize + 1 >= m_nBufferSize )
	{
		if( !Grow() )
			return false;  // out of memory
	}

	memmove( m_pwszBuffer + nIndex + 1, m_pwszBuffer + nIndex, sizeof(WCHAR) * ( m_nTextSize - nIndex + 1 ) );
	m_pwszBuffer[ nIndex ] = wChar;
	if(m_pData)
	{
		memmove(m_pData + nIndex + 1, m_pData + nIndex, sizeof(DWORD) * (m_nTextSize - nIndex + 1) );
		if(data)
			m_pData[nIndex ] = *data;
		else
			InitStringData(m_pwszBuffer + nIndex , m_pData + nIndex, 1,m_pEditBox->GetTextColor());
	}
	++m_nTextSize;
	//m_bAnalyseRequired = true;

	return true;
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
// Removes the char at specified index.
// If nIndex == -1, remove the last char.
//--------------------------------------------------------------------------------------
bool CGuiEditBox::CUniBuffer::RemoveChar( int nIndex )
{
	if( !m_nTextSize || nIndex < 0 || nIndex >= m_nTextSize )
		return false;  // Invalid index

	if ( m_pwszBuffer[nIndex] == ITEM_SPECIAL_CHAR )
	{
		m_pEditBox->m_mediaList.erase( m_pData[nIndex] );
	}
	
	memmove( m_pwszBuffer + nIndex, m_pwszBuffer + nIndex + 1, sizeof(WCHAR) * ( m_nTextSize - nIndex ) );
	if(m_pData)
	{
		memmove(m_pData + nIndex,m_pData + nIndex + 1,sizeof(DWORD) * (m_nTextSize - nIndex));
	}
	--m_nTextSize;
	//m_bAnalyseRequired = true;
	return true;
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param nIndex : √Ë ˆ
 * \param pStr : √Ë ˆ
 * \param pData : √Ë ˆ
 * \param nCount : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
// Inserts the first nCount characters of the string pStr at specified index.
// If nCount == -1, the entire string is inserted.
// If nIndex == -1, insert to the end.
//--------------------------------------------------------------------------------------
bool CGuiEditBox::CUniBuffer::InsertString( int nIndex, const WCHAR *pStr, const DWORD* pData,int nCount )
{
	assert( nIndex >= 0 );

	if( nIndex > m_nTextSize )
		return false;  // invalid index

	if( -1 == nCount )
		nCount = lstrlenW( pStr );

	// Check for maximum length allowed
	if( GetTextSize() + nCount >= GUI_MAX_EDITBOXLENGTH )
		return false;

	if( m_nTextSize + nCount >= m_nBufferSize )
	{
		if( !Grow( m_nTextSize + nCount + 1 ) )
			return false;  // out of memory
	}

	memmove( m_pwszBuffer + nIndex + nCount, m_pwszBuffer + nIndex, sizeof(WCHAR) * ( m_nTextSize - nIndex + 1 ) );
	memcpy( m_pwszBuffer + nIndex, pStr, nCount * sizeof(WCHAR) );
	if(m_pData)
	{
		memmove(m_pData + nIndex + nCount , m_pData + nIndex, sizeof(DWORD) * (m_nTextSize - nIndex + 1) );
		if(pData)
		{
			memcpy(m_pData + nIndex, pData, nCount * sizeof(DWORD));
		}
		else
		{
			InitStringData(m_pwszBuffer + nIndex,m_pData + nIndex,nCount,m_pEditBox->GetTextColor());
		}
	}
	m_nTextSize += nCount;
	//m_bAnalyseRequired = true;

	return true;
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param wszText : √Ë ˆ
 * \param pData : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
bool CGuiEditBox::CUniBuffer::SetText( LPCWSTR wszText ,DWORD* pData)
{
	assert( wszText != NULL );

	int nRequired = int(wcslen( wszText ) + 1);

	// Check for maximum length allowed
	if( nRequired >= GUI_MAX_EDITBOXLENGTH )
		return false;

	while( GetBufferSize() < nRequired )
		if( !Grow() )
			break;
	// Check again in case out of memory occurred inside while loop.
	if( GetBufferSize() >= nRequired )
	{
		wcscpy( m_pwszBuffer, wszText );
		if(m_pData)
		{
			if(pData)
				memcpy(m_pData,pData,nRequired - 1);
			else
				InitStringData(m_pwszBuffer, m_pData,-1,m_pEditBox->GetTextColor());
		}

		m_nTextSize = nRequired - 1;
		//m_bAnalyseRequired = true;
		return true;
	}
	else
		return false;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param nCP : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
inline int CGuiEditBox::CUniBuffer::CharWidth(int nCP)
{
	return WCharWidth(m_pwszBuffer[nCP],GetFontIndex());
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param nCP : √Ë ˆ
 * \param bTrail : √Ë ˆ
 * \param pX : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
HRESULT CGuiEditBox::CUniBuffer::CPtoX( int nCP, BOOL bTrail, int *pX )
{
	assert( pX );

	//int iFont = GetDevice()->SetFont(m_pEditBox->GetFont());
	WStringCPtoX(m_pwszBuffer,nCP,bTrail != 0,pX,GetFontIndex());

	//GetDevice()->SetFont(iFont);
	return S_OK;

	/*
	*pX = 0;  // Default

	HRESULT hr = S_OK;
	if( m_bAnalyseRequired )
	hr = Analyse();

	if( SUCCEEDED( hr ) )
	hr = _ScriptStringCPtoX( m_Analysis, nCP, bTrail, pX );

	return hr;
	//*/
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param nX : √Ë ˆ
 * \param pCP : √Ë ˆ
 * \param pnTrail : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
HRESULT CGuiEditBox::CUniBuffer::XtoCP( int nX, int *pCP, int *pnTrail )
{
	assert( pCP && pnTrail );

	//int iFont = GetDevice()->SetFont(m_pEditBox->GetFont());
	WStringXtoCP(m_pwszBuffer,nX,pCP,pnTrail,GetFontIndex());
	//GetDevice()->SetFont(iFont);
	return S_OK;

	/*
	*pCP = 0; *pnTrail = FALSE;  // Default

	HRESULT hr = S_OK;
	if( m_bAnalyseRequired )
	hr = Analyse();

	if( SUCCEEDED( hr ) )
	hr = _ScriptStringXtoCP( m_Analysis, nX, pCP, pnTrail );

	// If the coordinate falls outside the text region, we
	// can get character positions that don't exist.  We must
	// filter them here and convert them to those that do exist.
	if( *pCP == -1 && *pnTrail == TRUE )
	{
	*pCP = 0; *pnTrail = FALSE;
	} else
	if( *pCP > m_nTextSize && *pnTrail == FALSE )
	{
	*pCP = m_nTextSize; *pnTrail = TRUE;
	}

	return hr;
	//*/
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param nCP : √Ë ˆ
 * \param pPrior : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
// ªÒ»°…œ“ª∏ˆ—°œÓ(¥ )Œª÷√
void CGuiEditBox::CUniBuffer::GetPriorItemPos( int nCP, int *pPrior )
{
	*pPrior = WStringPriorItemPos(m_pwszBuffer,nCP);
	/*
	*pPrior = nCP;  // Default is the char itself

	if( m_bAnalyseRequired )
	if( FAILED( Analyse() ) )
	return;

	const SCRIPT_LOGATTR *pLogAttr = _ScriptString_pLogAttr( m_Analysis );
	if( !pLogAttr )
	return;

	if( !_ScriptString_pcOutChars( m_Analysis ) )
	return;
	int nInitial = *_ScriptString_pcOutChars( m_Analysis );
	if( nCP - 1 < nInitial )
	nInitial = nCP - 1;
	for( int i = nInitial; i > 0; --i )
	if( pLogAttr[i].fWordStop ||       // Either the fWordStop flag is set
	( !pLogAttr[i].fWhiteSpace &&  // Or the previous char is whitespace but this isn't.
	pLogAttr[i-1].fWhiteSpace ) )
	{
	*pPrior = i;
	return;
	}
	// We have reached index 0.  0 is always a break point, so simply return it.
	*pPrior = 0;
	//*/
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param nCP : √Ë ˆ
 * \param pPrior : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
// ªÒ»°…œ“ª∏ˆ—°œÓ(¥ )Œª÷√
void CGuiEditBox::CUniBuffer::GetNextItemPos( int nCP, int *pPrior )
{
	*pPrior = WStringNextItemPos(m_pwszBuffer,nCP);

	/*
	*pPrior = nCP;  // Default is the char itself

	HRESULT hr = S_OK;
	if( m_bAnalyseRequired )
	hr = Analyse();
	if( FAILED( hr ) )
	return;

	const SCRIPT_LOGATTR *pLogAttr = _ScriptString_pLogAttr( m_Analysis );
	if( !pLogAttr )
	return;

	if( !_ScriptString_pcOutChars( m_Analysis ) )
	return;
	int nInitial = *_ScriptString_pcOutChars( m_Analysis );
	if( nCP + 1 < nInitial )
	nInitial = nCP + 1;
	for( int i = nInitial; i < *_ScriptString_pcOutChars( m_Analysis ) - 1; ++i )
	{
	if( pLogAttr[i].fWordStop )      // Either the fWordStop flag is set
	{
	*pPrior = i;
	return;
	}
	else
	if( pLogAttr[i].fWhiteSpace &&  // Or this whitespace but the next char isn't.
	!pLogAttr[i+1].fWhiteSpace )
	{
	*pPrior = i+1;  // The next char is a word stop
	return;
	}
	}
	// We have reached the end. It's always a word stop, so simply return it.
	*pPrior = *_ScriptString_pcOutChars( m_Analysis ) - 1;
	//*/
}


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//--------------------------------------------------------------------------------------
void CGuiEditBox::ResetCaretBlink()
{
	m_bCaretOn = true;
	m_dfLastBlink = Engine_GetTimer()->GetAbsoluteTime();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
const char* CGuiEditBox::GetText()
{
	/*int len = WideCharToMultiByte(CP_ACP,0,GetTextW(),-1,NULL,0,NULL,FALSE);

	char* pText = m_strTextA.GetBuffer(len+1);
	len = WideCharToMultiByte(CP_ACP,0,GetTextW(),-1,pText,len,NULL,FALSE);
	pText[len] = 0;
	m_strTextA.ReleaseBuffer();
//*/
	m_strTextA.resize(wcslen(GetTextW()) * 2);
	char* pText = &m_strTextA[0];
	my_wcstombs(pText,GetTextW(),-1);
	return m_strTextA.c_str();
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
HRESULT CGuiEditBox::OnInit()
{
	CGuiControl::OnInit();
	m_Buffer.Init();
	return S_OK;
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \param pos : √Ë ˆ
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//Get media name count
int CGuiEditBox::GetMediaNameCount( int pos )
{
	if ( m_Buffer.GetData() + pos )
	{
		char itemName[MAX_NAMESIZE];
		mapMediaNameData::iterator		itor;
		itor = m_mediaList.end();

		DWORD itemId = *( m_Buffer.GetData()  + pos );
		itor = m_mediaList.find( itemId );

		if ( itor != m_mediaList.end() )
		{
			sprintf( itemName, itor->second.mName );
			int count = GetCharacterCount( itemName );
			if ( count > 0)
			{
				return count;
			}
			else
			{
				return 0;
			}
		}
		return 0;
	}
	else
	{
		return 0;
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//update media list before send
void CGuiEditBox::UpdateMediaListBeforeSend()
{
	
	const WCHAR* pwszText = m_Buffer.GetBuffer();
	m_mediaList.clear();
	
	int mediaNum = 0;
	for( int i=0;i<m_Buffer.GetTextSize();i++ )
	{
		bool bNeedAdd = false;
		if ( ItemSpecialChar(pwszText[i]) )
		{
			if ( i == 0 )
			{
				bNeedAdd = true;
			}
			else
			{
				if ( !ItemSpecialChar(pwszText[i-1]) )
				{
					bNeedAdd = true;
				}
				else
				{
					if ( *( m_Buffer.GetData() + i ) != *( m_Buffer.GetData() + i - 1 ) )
					{
						bNeedAdd = true;
					}
				}
			}
		}
		if ( bNeedAdd )
		{
			MediaNameData curItem( "" );
			curItem.mNumber = *(m_Buffer.GetData()+i);
			m_mediaList.insert( pairMediaNameData( mediaNum, curItem  ) );
			mediaNum++ ;
		}
	}
}

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
//Clear media list
void CGuiEditBox::ClearMediaList()
{
	m_mediaList.clear();
}

DWORD* CGuiEditBox::GetData()
{
	return m_Buffer.GetData();
}	

CGuiEditBox* CGuiEditBox::GetLastFocusEditBox()
{
	return m_pLastFocusEditBox;
}