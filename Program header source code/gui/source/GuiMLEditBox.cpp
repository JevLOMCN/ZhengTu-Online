/**
 * \file      GuiMLEditBox.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾@###.com
 *           $$$03¹«Ë¾@###.com
 * \date      2005-07-05 17:02:57
 * \brief     GUI¶àĞĞ±à¼­¿ò
 * 
 *	      GUI¶àĞĞ±à¼­¿ò
 */

#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"
#include "../../engine/include/PhraseFilter.h"

const int s_nSBSpace =8;
const int s_nSBTopOffset = 13;
const int s_nSBBottomOffset = 8;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pEditBox : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiMLEditBox::CUniBuffer::CUniBuffer(CGuiMLEditBox* pEditBox)
{
	m_pwszBuffer = NULL;
	m_nBufferSize = 0;
	m_pData = NULL;

	m_nTextSize = 0;
	//m_iFont = 0;
	m_nLineWidth = 0;
	m_nWidth = 0;
	m_nLineHeight = 12;
	m_pEditBox = pEditBox;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nInitialSize : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLEditBox::CUniBuffer::Init(int nInitialSize )
{

	if(m_pwszBuffer) delete [] m_pwszBuffer;
	m_pwszBuffer = new WCHAR[nInitialSize];
	m_pwszBuffer[0] = 0;
	m_nBufferSize = nInitialSize;

	if(IsMultimedia())
	{
		if(m_pData) delete [] m_pData;
		m_pData = new DWORD[nInitialSize];
	}

	m_nTextSize = 0;
	//m_iFont = 0;
	m_nLineWidth = 0;
	m_nWidth = 0;
	m_nLineHeight = 12;

}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiMLEditBox::CUniBuffer::~CUniBuffer()
{
	delete [] m_pwszBuffer;
	delete [] m_pData;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiMLEditBox::CUniBuffer::IsMultimedia()
{
	return (m_pEditBox->GetStyle() & CGuiMLEditBox::GUI_ES_MULTIMEDIA) != 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
stPointI CGuiMLEditBox::CUniBuffer::IndextoCP(int index)
{
	for(int i = GetLineCount() - 1; i >= 0; --i)
	{
		if(index >= m_Lines[i].index)
		{
			stPointI ptCP;
			ptCP.x = getMin(index - m_Lines[i].index,m_Lines[i].count);
			ptCP.y = i;
			return ptCP;
		}
	}
	return stPointI(-1,-1);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptCP : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiMLEditBox::CUniBuffer::IsLineTail(stPointI ptCP)
{
	if(GetLineCount() == 0) return true;
	Assert(ptCP.y < GetLineCount());

	return ptCP.x >= m_Lines[ptCP.y].count;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptCP : ÃèÊö
 * \param bNewLineIncChar : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLEditBox::CUniBuffer::ForwardCP(stPointI & ptCP,bool bNewLineIncChar)
{
	if(GetLineCount() == 0) return;

	Assert(ptCP.y < GetLineCount());

	if(ptCP.x < m_Lines[ptCP.y].count)
	{
		ptCP.x++;
	}
	else
	{
		if( ptCP.y < GetLineCount() - 1 )
		{
			ptCP.y ++;
			if(bNewLineIncChar)
			{
				if(GetLineLength(ptCP.y) > 0)
					ptCP.x = 1;
				else
					ptCP.x = 0;
			}
			else
			{
				ptCP.x = 0;
			}
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptCP : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLEditBox::CUniBuffer::BackwardCP(stPointI & ptCP)
{
	if(GetLineCount() == 0) return;

	Assert(ptCP.y < GetLineCount());

	if(ptCP.x > 0)
	{
		ptCP.x --;
	}
	else
	{
		if(ptCP.y > 0)
		{
			ptCP.y --;
			ptCP.x = m_Lines[ptCP.y].count;
		}
	}

}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nNewSize : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
bool CGuiMLEditBox::CUniBuffer::Grow( int nNewSize )
{
	// If the current size is already the maximum allowed,
	// we can't possibly allocate more.
	if( m_nBufferSize == GUI_MAX_EDITBOXLENGTH )
		return false;

	int nAllocateSize = ( nNewSize == -1 || nNewSize < m_nBufferSize * 2 ) ? ( m_nBufferSize ? m_nBufferSize * 2 : 256 ) : nNewSize * 2;

	// Cap the buffer size at the maximum allowed.
	if( nAllocateSize > GUI_MAX_EDITBOXLENGTH )
		nAllocateSize = GUI_MAX_EDITBOXLENGTH;

	WCHAR *pTempBuffer = new WCHAR[nAllocateSize];
	if( !pTempBuffer )
		return false;
	if( m_pwszBuffer )
		memcpy( pTempBuffer, m_pwszBuffer, (m_nTextSize + 1) * sizeof(WCHAR) );

	if(m_pEditBox->GetStyle() & GUI_ES_MULTIMEDIA)
	{
		DWORD* pTempData = new DWORD[nAllocateSize];
		if(pTempData)
			memcpy( pTempData,m_pData,(m_nTextSize + 1) * sizeof(DWORD));
		delete[] m_pData;
		m_pData = pTempData;
	}

	delete[] m_pwszBuffer;
	m_pwszBuffer = pTempBuffer;

	m_nBufferSize = nAllocateSize;
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLEditBox::CUniBuffer::CalcuWidth()
{
	m_nWidth = 0;
	for(int i=0;i<m_Lines.size();++i)
	{
		if(m_Lines[i].width > m_nWidth) m_nWidth = m_Lines[i].width;
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nStartLine : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLEditBox::CUniBuffer::Refresh(int nStartLine)
{
	int index;

	if(nStartLine == 0) index = 0;
	else index =  m_Lines[nStartLine].index;

	m_Lines.resize(nStartLine);
	int WcharWidth = GetDevice()->GetFontWcharWidth(GetFontIndex());
	int CharWidth = GetDevice()->GetFontWidth(GetFontIndex());
	int width = 0;

	int index0 = index;
	while(m_pwszBuffer[index])
	{
		int w;
		size_t charnum;
		if(IsNewLineW(&m_pwszBuffer[index],charnum))
		{
			stLine line;
			line.index = index0;
			line.count = index-index0;
			line.width = width;
			m_Lines.push_back(line);
			width = 0;

			if(m_pwszBuffer[index + charnum] == 0 && line.count > 0)
			{
				stLine line;
				line.index = index;
				line.count = 0;
				line.width = 0;
				m_Lines.push_back(line);
			}

			index0 = index + charnum;
			index += charnum;
		}
		else
		{
			if(m_pwszBuffer[index] & 0xff00)
			{
				w = WCharWidth(m_pwszBuffer[index],GetFontIndex());
			}
			else if ( m_pwszBuffer[index] == L'\t' )
			{
				w = c_nTabSize * CharWidth;
			}
			else w = CharWidth;

			if( index != index0 && (m_pEditBox->GetStyle() & AUTONEWLINE) && width + w > m_nLineWidth )
			{
				stLine line;
				line.index = index0;
				line.count = index - index0;
				line.width = width;
				m_Lines.push_back(line);
				index0 = index;
				width = 0;
			}
			else
			{
				width += w;
				index++;
			}
		}
	}
	if(index != index0)
	{
		stLine line;
		line.index = index0;
		line.count = index-index0;
		line.width = width;
		m_Lines.push_back(line);
	}
	CalcuWidth();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param cp : ÃèÊö
 * \param pStr : ÃèÊö
 * \param pData : ÃèÊö
 * \param count : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiMLEditBox::CUniBuffer::InsertString(stPointI cp,const WCHAR* pStr,const DWORD* pData,int count)
{
	int nIndex = CPtoIndex(cp);
	if(count == -1) count = wcslen(pStr);
	if(m_nTextSize + count >= m_nBufferSize)
	{
		if( !Grow( m_nTextSize + count + 1 ) )
			return false;  // out of memory
	}
	memmove( m_pwszBuffer + nIndex + count, m_pwszBuffer + nIndex, sizeof(WCHAR) * ( m_nTextSize - nIndex + 1 ) );
	memcpy( m_pwszBuffer + nIndex, pStr, count * sizeof(WCHAR) );

	if(m_pData)
	{
		memmove( m_pData + nIndex + count, m_pData + nIndex, sizeof(DWORD) * (m_nTextSize - nIndex + 1) );
		if(pData)
			memcpy( m_pData + nIndex, pData,count * sizeof(DWORD));
		else
			InitStringData(m_pwszBuffer + nIndex,m_pData + nIndex,count,m_pEditBox->GetTextColor());
	}

	m_nTextSize += count;
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param cp : ÃèÊö
 * \param wChar : ÃèÊö
 * \param data : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiMLEditBox::CUniBuffer::InsertChar(stPointI cp,WCHAR wChar,DWORD *data)
{
	int nIndex = CPtoIndex(cp);
	if( nIndex > m_nTextSize )
		return false;  // invalid index

	// Check for maximum length allowed
	if( m_nTextSize + 1 >= GUI_MAX_EDITBOXLENGTH )
		return false;

	if( m_nTextSize + 1 >= m_nBufferSize )
	{
		if( !Grow() )
			return false;  // out of memory
	}

	memmove( m_pwszBuffer + nIndex + 1, m_pwszBuffer + nIndex, sizeof(WCHAR) * ( m_nTextSize - nIndex + 1 ) );
	m_pwszBuffer[ nIndex ] = wChar;
	if(m_pData)
	{
		memmove( m_pData + nIndex + 1, m_pData + nIndex, sizeof(DWORD) * ( m_nTextSize - nIndex + 1 ) );
		if(data)
			m_pData[nIndex] = *data;
		else
			InitStringData(m_pwszBuffer + nIndex,m_pData + nIndex,1,m_pEditBox->GetTextColor());
	}
	++m_nTextSize;

	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \param ch : ÃèÊö
 * \param data : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLEditBox::CUniBuffer::SetWChar(int index,WCHAR ch,DWORD* data) 
{ 
	m_pwszBuffer[index] = ch; 
	if(m_pData)
	{
		if(data)
		{
			m_pData[index] = *data;
		}
		else
		{
			InitStringData(m_pwszBuffer + index,m_pData + index,1,m_pEditBox->GetTextColor());
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param cp : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiMLEditBox::CUniBuffer::RemoveChar(stPointI cp)
{
	int nIndex = CPtoIndex(cp);
	memmove( m_pwszBuffer + nIndex, m_pwszBuffer + nIndex + 1, sizeof(WCHAR) * ( m_nTextSize - nIndex ) );
	if(m_pData)
	{
		memmove( m_pData + nIndex, m_pData + nIndex + 1, sizeof(DWORD) * ( m_nTextSize - nIndex ) );
	}
	--m_nTextSize;
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptStart : ÃèÊö
 * \param ptEnd : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
// É¾³ı×Ö·û
bool CGuiMLEditBox::CUniBuffer::RemoveChar(stPointI ptStart, stPointI ptEnd)
{
	Assert( ptStart.y >= 0 && ptStart.y < GetLineCount() );
	Assert( ptEnd.y >= 0 && ptEnd.y < GetLineCount() );

	WCHAR* first = GetLine(ptStart.y) + ptStart.x;
	WCHAR* last = GetLine(ptEnd.y) + ptEnd.x;
	int len = wcslen(m_pwszBuffer);
	memcpy(first,last,(len-(last-m_pwszBuffer) + 1)*sizeof(WCHAR));
	if(m_pData)
	{
		DWORD* first = GetLineData(ptStart.y) + ptStart.x;
		DWORD* last = GetLineData(ptEnd.y) + ptEnd.x;
		memcpy(first,last,(len-(last-m_pData) + 1)*sizeof(DWORD));
	}
	m_nTextSize -= (last - first);
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param wszText : ÃèÊö
 * \param pData : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiMLEditBox::CUniBuffer::SetText(const WCHAR* wszText,const DWORD* pData)
{
	assert( wszText != NULL );

	if(wszText[0] == 0) wszText = L"\n";
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
			{
				memcpy(m_pData,pData,nRequired-1);
			}
			else
			{
				InitStringData(m_pwszBuffer,m_pData,nRequired-1,m_pEditBox->GetTextColor());
			}
		}
		m_nTextSize = nRequired - 1;
		//m_bAnalyseRequired = true;
		return true;
	}
	else
		return false;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param cp : ÃèÊö
 * \param bTrail : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
stPointI CGuiMLEditBox::CUniBuffer::CPtoXY(stPointI cp,bool bTrail)
{
	if(GetLineCount() == 0) return stPointI(0,0);
	Assert(cp.y < GetLineCount());
	if(m_Lines[cp.y].count == 0) return stPointI(0,cp.y * m_nLineHeight);

	Assert(cp.x <= m_Lines[cp.y].count);

	stPointI pt;
	pt.y = cp.y * m_nLineHeight;
	WCHAR* pwszBuffer = GetLine(cp.y);
	//int iFont = GetDevice()->SetFont(m_pEditBox->GetFont());
	WStringCPtoX(pwszBuffer,cp.x,bTrail,(int*)&pt.x,GetFontIndex());
	//GetDevice()->SetFont(iFont);
	return pt;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pt : ÃèÊö
 * \param pnTrail : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
stPointI CGuiMLEditBox::CUniBuffer::XYtoCP(stPointI pt,int *pnTrail)
{
	if(GetLineCount() == 0) return stPointI(0,0);
	stPointI cp;
	cp.y = pt.y/m_nLineHeight;

	if(cp.y >= GetLineCount()) cp.y = GetLineCount()-1;
	if(cp.y < 0) cp.y = 0;

	WCHAR* pwszBuffer = GetLine(cp.y);
	WStringNXtoCP(pwszBuffer,m_Lines[cp.y].count,pt.x,(int*)&cp.x,pnTrail,GetFontIndex());
	return cp;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptCP : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline int CGuiMLEditBox::CUniBuffer::CharWidth(stPointI ptCP)
{
	if(IsLineTail(ptCP)) return 0;
	return WCharWidth(*(GetLine(ptCP.y) + ptCP.x),GetFontIndex());
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiMLEditBox::CGuiMLEditBox(CGuiDialog* pDialog,DWORD dwStyle)
: m_HScrollBar(pDialog)
, m_VScrollBar(pDialog)
, m_Buffer(this)
{
	m_HScrollBar.SetCanHaveFocus(false);
	m_VScrollBar.SetCanHaveFocus(false);

	m_Type = GUI_CONTROL_MLEDITBOX;
	m_pDialog = pDialog;

	m_bCaretOn = true;
	m_bInsertMode = true;
	m_bSBInit	= false;
	m_bMouseDrag = false;

	m_nBorder = 5;  // Default border width
	m_nSpacing = 0;  // Default spacing

	m_nSBWidth = pDialog->m_nSBWidth;

	m_nLineHeight = 12;
	m_nMaxShowLineCount = 0;

	m_ptCaret.x = 0;
	m_ptCaret.y = 0;
	m_ptSelStart.x = 0;
	m_ptSelStart.y = 0;
	m_ptFirstVisible.x = 0;
	m_ptFirstVisible.y = 0;
	memset(&m_rcRender,0,sizeof(m_rcRender));
	m_rcText.SetRectEmpty();
	m_dwStyle = dwStyle;

	m_TextColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	m_SelTextColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	m_SelBkColor = D3DCOLOR_ARGB( 255, 0, 0, 255 );
	m_CaretColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );

	m_dfBlink = GetCaretBlinkTime() * 0.001f;
	m_dfLastBlink = Engine_GetTimer()->GetAbsoluteTime();


	m_maxCharCount = 1024;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiMLEditBox::~CGuiMLEditBox(void)
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
void CGuiMLEditBox::OnFocusIn()
{
	CGuiControl::OnFocusIn();

	ResetCaretBlink();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
const char* CGuiMLEditBox::GetText()
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
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param strText : ÃèÊö
 * \param pData : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLEditBox::SetText(const char* strText,DWORD* pData)
{
	if(m_dwStyle & GUI_ES_MAXCOUNT)
	{
               SelectAll();
	       SetSelText(strText,false,pData);
	}
	else
	{
		/*USES_CONVERSION;
		SetText(T2W(strText));*/
		wchar_t * p = (wchar_t*)_alloca((strlen(strText) + 1)*2);
		my_mbstowcs(p,strText,-1);
		SetText(p,pData);
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
void CGuiMLEditBox::SetText(const WCHAR* strText,DWORD* pData)
{	
	m_Buffer.SetText(strText,pData);
	m_ptCaret.x = 0;
	m_ptCaret.y = 0;
	m_ptFirstVisible.x = 0;
	m_ptFirstVisible.y =0 ;
	m_ptSelStart.x = 0;
	m_ptSelStart.y = 0;
	Refresh(0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLEditBox::SelectAll()
{
	//if( m_ptSelStart != m_ptCaret )
	{
		m_ptSelStart = stPointI(0,0);
		stPointI ptCP = m_Buffer.IndextoCP(m_Buffer.GetTextSize());
		PlaceCaret( ptCP );
	}
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
bool CGuiMLEditBox::MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
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
						if( m_ptCaret != m_ptSelStart )
						{
							DeleteSelectionText();
							OnTextChange();
						}
						else
						{
							stPointI ptFirst = m_ptCaret;
							m_Buffer.BackwardCP(ptFirst);

							// Move the caret, then delete the char.
							if(ptFirst != m_ptCaret)
							{
								stPointI ptCaretOld = m_ptCaret;
								PlaceCaret( ptFirst);
								m_Buffer.RemoveChar( ptFirst,ptCaretOld );
								m_ptSelStart = m_ptCaret = ptFirst;
								Refresh(ptFirst.y);
								OnTextChange();
							}
						}
						ResetCaretBlink();
					}
					break;
				}

			case 24:        // Ctrl-X Cut
			case VK_CANCEL: // Ctrl-C Copy
				{
					CopyToClipboard();

					// If the key is Ctrl-X, delete the selection too.
					if( 0 == (GetStyle() & GUI_ES_READONLY) )
					{
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
					if( 0 == (GetStyle() & GUI_ES_READONLY))
					{
						PasteFromClipboard();
						OnTextChange();
					}
					break;
				}

				// Ctrl-A Select All
			case 1:
				SelectAll();
				break;

			//case VK_RETURN:
				// Invoke the callback when the user presses Enter.
			//	m_pDialog->SendEvent( EVENT_EDITBOX_STRING, true, this );
			//	break;

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
					// If there's a selection and the user
					// starts to type, the selection should
					// be deleted.
					if(GetStyle() & GUI_ES_READONLY) break;

					if( m_ptCaret != m_ptSelStart )
						DeleteSelectionText();

					// If we are in overwrite mode and there is already
					// a char at the caret's position, simply replace it.
					// Otherwise, we insert the char as normal.
					WCHAR curInput[2];
					curInput[1] = 0;
					if(g_bWaitNextChar)
					{
						g_szCurInput[1] = (char)wParam;
						//curInput = A2W(g_szCurInput)[0];
						my_mbstowcs(curInput,g_szCurInput,-1);
						g_bWaitNextChar = false;
					}
					else
					{
						if( ((WCHAR)wParam) & 0x80 )
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

					if( !m_bInsertMode && !m_Buffer.IsLineTail(m_ptCaret) )
					{
						m_Buffer.SetWChar(m_Buffer.CPtoIndex(m_ptCaret),curInput[0]);
						stPointI ptCP = m_ptCaret;
						m_Buffer.ForwardCP(ptCP,(WCHAR)wParam != VK_RETURN);
						PlaceCaret( ptCP );
						m_ptSelStart = m_ptCaret;
					}
					else
					{
						// Insert the char
						if( m_Buffer.InsertChar( m_ptCaret, curInput[0] ) )
						{
							Refresh(m_ptCaret.y);
							stPointI ptCP = m_ptCaret;
							m_Buffer.ForwardCP(ptCP ,(WCHAR)wParam != VK_RETURN);
							PlaceCaret( ptCP);
							m_ptSelStart = m_ptCaret;
						}
					}
					ResetCaretBlink();
					OnTextChange();
				}
			}
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
 * \param uMsg : ÃèÊö
 * \param wParam : ÃèÊö
 * \param lParam : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
bool CGuiMLEditBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	bool bHandled = false;

	if(m_VScrollBar.HandleKeyboard(uMsg, wParam, lParam))
	{
		ScrollToLine(m_VScrollBar.GetTrackPos());
		return true;
	}
	if(GetStyle() & AUTONEWLINE)
	{	
	}
	else
	{
		if(m_HScrollBar.HandleKeyboard(uMsg, wParam, lParam))
		{
			stPointI pt = m_ptFirstVisible;
			pt.x = m_HScrollBar.GetTrackPos();
			ScrollTo(pt);
			return true;
		}
	}

	switch( uMsg )
	{
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case VK_HOME:
				{
					stPointI ptCP = m_ptCaret;
					ptCP.x = 0;
					PlaceCaret( ptCP );

					// Shift is not down. Update selection
					// start along with the caret.
					if( GetKeyState( VK_SHIFT ) >= 0 )
						m_ptSelStart = m_ptCaret;

					ResetCaretBlink();
					bHandled = true;
					break;
				}
			case VK_END:
				{
					stPointI ptCP = m_ptCaret;
					ptCP.x = m_Buffer.GetLineLength(ptCP.y);
					PlaceCaret( ptCP );

					// Shift is not down. Update selection
					// start along with the caret.
					if( GetKeyState( VK_SHIFT ) >= 0 )
						m_ptSelStart = m_ptCaret;

					ResetCaretBlink();
					bHandled = true;
				}
				break;

			case VK_INSERT:
				if( 0 == (GetStyle() & GUI_ES_READONLY))
				{
					if( GetKeyState( VK_CONTROL ) < 0 )
					{
						// Control Insert. Copy to clipboard
						CopyToClipboard();
					} 
					else
					{
						if( GetKeyState( VK_SHIFT ) < 0 )
						{
							// Shift Insert. Paste from clipboard
							PasteFromClipboard();
						} 
						else
						{
							// Toggle caret insert mode
							m_bInsertMode = !m_bInsertMode;
						}
					}
				}
				break;

			case VK_DELETE:
				// Check if there is a text selection.
				if( 0 == (GetStyle() & GUI_ES_READONLY))
				{
					if( m_ptCaret != m_ptSelStart )
					{
						DeleteSelectionText();
						OnTextChange();
					}
					else
					{
						// Deleting one character
						stPointI ptLast = m_ptCaret;
						m_Buffer.ForwardCP(ptLast,true);

						// Move the caret, then delete the char.
						if(ptLast != m_ptCaret)
						{
							m_Buffer.RemoveChar( m_ptCaret,ptLast );
							Refresh(ptLast.y);
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

					//m_Buffer.GetPriorItemPos( m_nCaret, &m_nCaret );
					//PlaceCaret( m_nCaret );
				}
				else
				{
					stPointI ptCP = m_ptCaret;
					if(ptCP.x > 0)
					{
						ptCP.x --;
						PlaceCaret(ptCP);
					}
				}
				if( GetKeyState( VK_SHIFT ) >= 0 )
					// Shift is not down. Update selection
					// start along with the caret.
					m_ptSelStart = m_ptCaret;

				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_RIGHT:
				if( GetKeyState( VK_CONTROL ) < 0 )
				{
					// Control is down. Move the caret to a new item
					// instead of a character.

					//m_Buffer.GetNextItemPos( m_nCaret, &m_nCaret );
					//PlaceCaret( m_nCaret );
				}
				else
				{
					stPointI ptCP = m_ptCaret;
					if(ptCP.x < m_Buffer.GetLineLength(ptCP.y))
					{
						ptCP.x ++;
						PlaceCaret(ptCP);
					}
				}
				if( GetKeyState( VK_SHIFT ) >= 0 )
					// Shift is not down. Update selection
					// start along with the caret.
					m_ptSelStart = m_ptCaret;

				ResetCaretBlink();
				bHandled = true;
				break;

			case VK_UP:
				if( GetKeyState( VK_CONTROL ) < 0 )
				{
					// Control is down. Move the caret to a new item
					// instead of a character.

					//m_Buffer.GetPriorItemPos( m_nCaret, &m_nCaret );
					//PlaceCaret( m_nCaret );
				}
				else
				{
					if(m_ptCaret.y > 0)
					{
						stPointI ptXY = m_Buffer.CPtoXY(m_ptCaret,false);
						ptXY.y -= m_nLineHeight;
						int nTail;
						stPointI ptCP = m_Buffer.XYtoCP(ptXY,&nTail);
						if(ptCP.x > m_Buffer.GetLineLength(ptCP.y))
							ptCP.x = m_Buffer.GetLineLength(ptCP.y);

						PlaceCaret(ptCP);
					}
				}
				if( GetKeyState( VK_SHIFT ) >= 0 )
					m_ptSelStart = m_ptCaret;

				ResetCaretBlink();
				bHandled = true;
				break;
			case VK_DOWN:
				if( GetKeyState( VK_CONTROL ) < 0 )
				{
					// Control is down. Move the caret to a new item
					// instead of a character.

					//m_Buffer.GetPriorItemPos( m_nCaret, &m_nCaret );
					//PlaceCaret( m_nCaret );
				}
				else
				{
					stPointI ptCP = m_ptCaret;
					if(ptCP.y < m_Buffer.GetLineCount() -1)
					{
						stPointI ptXY = m_Buffer.CPtoXY(m_ptCaret,false);
						ptXY.y += m_nLineHeight;
						int nTail;
						stPointI ptCP = m_Buffer.XYtoCP(ptXY,&nTail);
						if(ptCP.x > m_Buffer.GetLineLength(ptCP.y))
							ptCP.x = m_Buffer.GetLineLength(ptCP.y);

						PlaceCaret(ptCP);
					}
				}
				if( GetKeyState( VK_SHIFT ) >= 0 )
					m_ptSelStart = m_ptCaret;

				ResetCaretBlink();
				bHandled = true;
				break;

			default:
				bHandled = (wParam != VK_ESCAPE && wParam != VK_TAB && (wParam < VK_F1 || wParam > VK_F24));  // Let the application handle Esc.
			}
		}
	}
	return bHandled;
}

void CGuiMLEditBox::OnTextChange()
{
	if(m_dwStyle & GUI_ES_MAXCOUNT)
	{
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

		        SelectAll();
			SetSelText(szTemp);
			 
		}
	}
	 
	///////////////////////////////////////////////////////////////
	// begin filter
	const char * pword = GetText();
	char szPword[MAX_CHATINFO];
	memset(szPword,0,sizeof(szPword));
	if( G_PhraseFilterName(pword,szPword,MAX_CHATINFO))
	{
		stPointI ptOldCP = m_ptCaret;
		stPointI ptOldSel = m_ptSelStart;
		SetText(szPword);
		m_ptCaret = ptOldCP;
		m_ptSelStart = ptOldSel;

		//// scroll to the last line 
		//stPointI ptCP = m_ptCaret;
		//stPointI ptXY = m_Buffer.CPtoXY(m_ptCaret,false);
		//while(ptCP.y < m_Buffer.GetLineCount() -1)
		//{
		//	ptXY.y += m_nLineHeight;
		//	int nTail;
		//	ptCP = m_Buffer.XYtoCP(ptXY,&nTail);
		//}
		//// scroll to the last col 
		//ptCP.x = m_Buffer.GetLineLength(ptCP.y);
		//PlaceCaret( ptCP );
		//ResetCaretBlink();
	}
	/*else 
	{
		SetText(pword);
	}*/
	// end filter
	///////////////////////////////////////////////////////////////
	 
	m_pDialog->SendEvent( EVENT_EDITBOX_CHANGE, true, this );
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
bool CGuiMLEditBox::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if(CGuiControl::HandleMouse(uMsg,pt,wParam,lParam))
		return true;

	if( !m_bEnabled || !m_bVisible )
		return false;

	// First acquire focus
	if( WM_LBUTTONDOWN == uMsg || WM_RBUTTONDOWN == uMsg)
		if( !m_bHasFocus )
			m_pDialog->RequestFocus( this );
	
	if(m_VScrollBar.HandleMouse(uMsg, pt,wParam, lParam))
	{
		ScrollToLine(m_VScrollBar.GetTrackPos());
		return true;
	}
	if(GetStyle() & AUTONEWLINE)
	{
	}
	else
	{
		if(m_HScrollBar.HandleMouse(uMsg, pt,wParam, lParam))
		{
			stPointI pt = m_ptFirstVisible;
			pt.x = m_HScrollBar.GetTrackPos();
			ScrollTo(pt);
			return true;
		}
	}

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

			stPointI nCP, nX1st;
			int nTrail;
			nX1st = m_Buffer.CPtoXY( m_ptFirstVisible, false );  
			stPointI ptXY(pt.x - m_rcText.left + nX1st.x,pt.y - m_rcText.top + nX1st.y);
			nCP = m_Buffer.XYtoCP( ptXY, &nTrail );

			// Cap at the NULL character.
			if( nTrail && !m_Buffer.IsLineTail(nCP))
			{
				nCP.x ++;
				PlaceCaret( nCP );
			}
			else
			{
				PlaceCaret( nCP );
			}

			m_ptSelStart = m_ptCaret;
			ResetCaretBlink();
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
			if((wParam & MK_LBUTTON) == 0)
				m_bMouseDrag = false;
			else
			{
				// Determine the character corresponding to the coordinates.
				stPointI nCP, nX1st;
				int nTrail;

				nX1st = m_Buffer.CPtoXY( m_ptFirstVisible, false );  // X offset of the 1st visible char
				stPointI ptXY(pt.x - m_rcText.left + nX1st.x,pt.y - m_rcText.top + nX1st.y);

				nCP = m_Buffer.XYtoCP( ptXY, &nTrail );

				if( nTrail && !m_Buffer.IsLineTail(nCP) )
				{
					nCP.x ++;
					PlaceCaret( nCP);
				}
				else
					PlaceCaret( nCP );
			}
		}
		break;
	}

	return false;
}

void  CGuiMLEditBox::GetTextByFM( char* pStr, const int& length /* = -1 */ )
{
	int len = my_wcstombs(pStr,m_Buffer.GetBuffer(), m_Buffer.GetTextSize());
#ifdef WIN32
	//'\n' ==> '\r\n'
	int i=0;
	int ilen = len+1;
	while(pStr[i])
	{
		int icharnum;
		if(pStr[i] & 0x80)
		{
			if(pStr[i+1] == 0) break;
			i+=2;
		}
		else
		{
			if(IsNewLine(&pStr[i],(size_t&)icharnum))
			{
				if(icharnum!=2)
				{
					memmove(&pStr[i+2],&pStr[i+icharnum],ilen-(i+icharnum));
					ilen += (2 - icharnum);
				}
				pStr[i] = '\r';
				pStr[i+1] = '\n';
				i+=2;
			}
			else
			{
				i++;
			}
		}
	}
#endif
}
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLEditBox::CopyToClipboard()
{
	// Copy the selection text to the clipboard
	if( m_ptCaret != m_ptSelStart && OpenClipboard( NULL ) )
	{
		EmptyClipboard();

		HGLOBAL hBlock = GlobalAlloc( GMEM_MOVEABLE, sizeof(char) * ( m_Buffer.GetTextSize() * 4 + 1 ) );
		if( hBlock )
		{
			char *pszText = (char*)GlobalLock( hBlock );
			if( pszText )
			{
				pszText[0] = 0;
				stPointI ptFirst,ptLast;
				GetFirstLastPoint(m_ptCaret,m_ptSelStart,ptFirst,ptLast);
				int nFirst,nLast;
				nFirst = m_Buffer.CPtoIndex(ptFirst);
				nLast = m_Buffer.CPtoIndex(ptLast);

				if( nLast - nFirst > 0 )
				{
					//int len = WideCharToMultiByte(CP_ACP,0,m_Buffer.GetBuffer() + nFirst,nLast - nFirst,pszText,m_Buffer.GetTextSize() * 2 + 1,NULL,NULL);
					//pszText[len] = '\0';
					int len = my_wcstombs(pszText,m_Buffer.GetBuffer() + nFirst,nLast - nFirst);
#ifdef WIN32
					//'\n' ==> '\r\n'
					int i=0;
					int ilen = len+1;
					while(pszText[i])
					{
						int icharnum;
						if(pszText[i] & 0x80)
						{
							if(pszText[i+1] == 0) break;
							i+=2;
						}
						else
						{
							if(IsNewLine(&pszText[i],(size_t&)icharnum))
							{
								if(icharnum!=2)
								{
									memmove(&pszText[i+2],&pszText[i+icharnum],ilen-(i+icharnum));
									ilen += (2 - icharnum);
								}
								pszText[i] = '\r';
								pszText[i+1] = '\n';
								i+=2;
							}
							else
							{
								i++;
							}
						}
					}
#endif
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
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
// É¾³ıÑ¡ÔñµÄÎÄ±¾
void CGuiMLEditBox::DeleteSelectionText(void)
{
	if(m_ptCaret != m_ptSelStart)
	{
		stPointI LeftTop,RightBottom;

		GetFirstLastPoint(m_ptCaret,m_ptSelStart,LeftTop,RightBottom);

		PlaceCaret( LeftTop );

		m_ptSelStart = m_ptCaret;

		// Remove the characters
		m_Buffer.RemoveChar(LeftTop,RightBottom);
		Refresh(LeftTop.y);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptCP : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLEditBox::SetCaretPos(stPointI ptCP)
{
	PlaceCaret(ptCP);
	m_ptSelStart = m_ptCaret;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptCP : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
// ÉèÖÃ¹â±êÎ»ÖÃ
void CGuiMLEditBox::PlaceCaret(stPointI ptCP)
{
	if(m_Buffer.GetLineCount() == 0)
	{
		m_ptCaret.x = 0;
		m_ptCaret.y = 0;
		m_ptFirstVisible.x = 0;
		m_ptFirstVisible.y =0 ;
		m_ptSelStart.x = 0;
		m_ptSelStart.y = 0;
		return;
	}
	Assert( ptCP.y >= 0 && ptCP.y < m_Buffer.GetLineCount() );

	m_ptCaret = ptCP;

	stPointI oldFirstVisibleXY, curXY, curXY2;
	oldFirstVisibleXY = m_Buffer.CPtoXY( m_ptFirstVisible, FALSE); 
	curXY = m_Buffer.CPtoXY( ptCP, FALSE); 

	curXY2 = curXY;

	if( m_Buffer.IsLineTail(ptCP) )
		;
	else
		curXY2.x = curXY.x + m_Buffer.CharWidth(ptCP);

	curXY2.y += m_nLineHeight;
	
	//------------------------------------------------
	// ÉèÖÃ X Î»ÖÃ
	//------------------------------------------------
	if( curXY.x < oldFirstVisibleXY.x )
	{
		// Èç¹ûµ±Ç°¹â±êÔÚ¿É¼ûµãµÄ×ó±ß
		m_ptFirstVisible.x = ptCP.x;
	}
	else
	{
		if( curXY2.x > oldFirstVisibleXY.x + m_rcText.Width() )
		{
			// Èç¹û³¬³öÁË·¶Î§

			stPointI nXYNewLeft(curXY2.x - m_rcText.Width(),curXY2.y);

			stPointI nCPNew1st;
			int nNewTrail;
			nCPNew1st = m_Buffer.XYtoCP( nXYNewLeft, &nNewTrail );

			if(nNewTrail) m_ptFirstVisible.x = nCPNew1st.x + 1;
			else m_ptFirstVisible.x = nCPNew1st.x;
		}
	}
	//-------------------------------------------------


	//-------------------------------------------------
	// ÉèÖÃ Y Î»ÖÃ
	//-------------------------------------------------
	if(curXY.y < oldFirstVisibleXY.y)
	{
		// Èç¹ûµ±Ç°¹â±êÔÚ¿É¼ûµãµÄÉÏ·½
		m_ptFirstVisible.y = ptCP.y;
	}
	else
	{
		if(curXY2.y > oldFirstVisibleXY.y + m_rcText.Height())
		{
			// Èç¹û³¬³öÁË·¶Î§

			int nNewTop = curXY2.y - m_rcText.Height();
			int nCPNew1st;

			nCPNew1st = nNewTop / m_nLineHeight;

			if(nNewTop - nCPNew1st * m_nLineHeight > 0)
				nCPNew1st++;

			if(nCPNew1st >= m_Buffer.GetLineCount())
				nCPNew1st--;

			m_ptFirstVisible.y = nCPNew1st;
		}
	}
	//--------------------------------------------------

	m_VScrollBar.SetTrackPos(m_ptFirstVisible.y);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLEditBox::ResetCaretBlink()
{
	m_bCaretOn = true;
	m_dfLastBlink = Engine_GetTimer()->GetAbsoluteTime();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszText : ÃèÊö
 * \param bScroll : ÃèÊö
 * \param pData : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLEditBox::SetSelText(const char* pszText,bool bScroll,DWORD* pData)
{
	DeleteSelectionText();

	//WCHAR* pwszText = A2W(pszText);
	WCHAR* pwszText = (WCHAR*)_alloca((strlen(pszText) + 1)*2);
	my_mbstowcs(pwszText,pszText,-1);
	// Copy all characters up to null.
	if( m_Buffer.InsertString( m_ptCaret, pwszText ,pData) )
	{
		Refresh(m_ptCaret.y);
		stPointI ptCP = m_Buffer.IndextoCP(m_Buffer.CPtoIndex(m_ptCaret) + wcslen(pwszText));
		PlaceCaret(ptCP);
	}
	m_ptSelStart = m_ptCaret;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLEditBox::PasteFromClipboard(void)
{
	if(GetStyle() & GUI_ES_READONLY) return;
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
				SetSelText(pszText);

				GlobalUnlock( handle );
			}
		}
		CloseClipboard();
		OnTextChange();
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
void CGuiMLEditBox::Render( float fElapsedTime )
{

	CGuiControl::Render(fElapsedTime);

	if(m_Buffer.GetLineCount() > 0)
	{
		 if( !m_bSBInit )
        {
            // Update the page size of the scroll bar
            if( m_nLineHeight )
                m_VScrollBar.SetPageSize( m_rcText.Height() / m_nLineHeight );
            else
                m_VScrollBar.SetPageSize( m_rcText.Height() );

			if(GetStyle() & AUTONEWLINE)
			{
			}
			else
			{
				m_HScrollBar.SetPageSize(getMin(m_rcText.Width(),(long)m_Buffer.GetWidth()));
			}
            m_bSBInit = true;
        }
	}
	CGuiElement* pElement = GetElement( 0 );
	if( pElement )
	{
		//m_Buffer.SetFontIndex( pElement->iFont );
		//PlaceCaret( m_nCaret );  // Call PlaceCaret now that we have the DC,
		// so that scrolling can be handled.
	}

	// Render the control graphics
	if(!(m_dwStyle & GUI_ES_NOSKIN))
	{
		stRectI rect = m_rcBoundingBox;
		//rect.right -= m_nSBWidth + s_nSBSpace;
		m_pDialog->DrawControlBorder(rect,m_Type);
	}

	//
	// Compute the X coordinates of the first visible character.
	//
	stPointI ptXYFirst;
	ptXYFirst = m_Buffer.CPtoXY( m_ptFirstVisible, false);

	//
	// Render the selection rectangle
	//
	stRectI rcText;
	rcText = m_rcText;
	rcText.left -= ptXYFirst.x;
	rcText.top -= ptXYFirst.y;

	int beginShowLine = m_ptFirstVisible.y;
	int endShowLine = getMin((long)m_Buffer.GetLineCount(),m_ptFirstVisible.y + m_nMaxShowLineCount);
	rcText.top += beginShowLine * m_nLineHeight;

	int iFontHeight = GetDevice()->GetFontHeight(GetFont());

	if(m_ptSelStart != m_ptCaret && m_bHasFocus)
	{
		stPointI ptLeftTop,ptRightBottom;
		stPointI ptLeftTopXY,ptRightBottomXY;
		GetFirstLastPoint(m_ptSelStart,m_ptCaret,ptLeftTop,ptRightBottom);
		ptLeftTopXY = m_Buffer.CPtoXY(ptLeftTop,false);
		ptRightBottomXY = m_Buffer.CPtoXY(ptRightBottom,false);

		for(int i=beginShowLine;i<endShowLine;++i)
		{	
			if(i>= ptLeftTop.y && i <= ptRightBottom.y)
			{
				stRectI rcSelection;
				
				int iCharOffset = 0;
				int iCharCount = m_Buffer.GetLineLength(i);
				rcSelection.top = rcText.top;
				rcSelection.bottom = rcText.top + m_nLineHeight;

				rcSelection.left = rcText.left ;
				if(i == ptLeftTop.y)
				{
					rcSelection.left += ptLeftTopXY.x;
					iCharOffset = ptLeftTop.x;
					iCharCount = m_Buffer.GetLineLength(i) - iCharOffset;
				}

				if(i == ptRightBottom.y)
				{
					rcSelection.right = rcText.left + ptRightBottomXY.x;
					iCharCount = ptRightBottom.x - iCharOffset;
				}
				else
				{
					//stPointI ptCP(m_Buffer.GetLineLength(i),i);
					//stPointI ptXY = m_Buffer.CPtoXY(ptCP,false);
					rcSelection.right =rcText.left + m_Buffer.GetLineWidth(i);
				}

				stRectI rcDrawRect = rcSelection;
				rcDrawRect.OffsetRect(0,-(m_nLineHeight - iFontHeight)/2);
				m_pDialog->DrawRect( &rcDrawRect, m_SelBkColor );

				m_Elements[0]->FontColor.Current = m_SelTextColor;
				m_pDialog->DrawText(m_Buffer.GetLine(i) + iCharOffset,m_Elements[0],&rcSelection,FontEffect_None,iCharCount,(m_Buffer.GetLineData(i) ? m_Buffer.GetLineData(i) + iCharOffset : NULL));

				if(i == ptLeftTop.y)
				{
					m_Elements[0]->FontColor.Current = m_TextColor;
					m_pDialog->DrawText(m_Buffer.GetLine(i),m_Elements[0],&rcText,FontEffect_None,ptLeftTop.x,m_Buffer.GetLineData(i) );
				}

				if(i == ptRightBottom.y)
				{
					stRectI rc = rcText;
					rc.left = rcSelection.right;
					m_Elements[0]->FontColor.Current = m_TextColor;
					m_pDialog->DrawText(m_Buffer.GetLine(i) + ptRightBottom.x,m_Elements[0],&rc,FontEffect_None,m_Buffer.GetLineLength(i) - ptRightBottom.x,( m_Buffer.GetLineData(i) ? m_Buffer.GetLineData(i) + ptRightBottom.x : NULL));
				}
			}
			else
			{
				m_Elements[0]->FontColor.Current = m_TextColor;
				m_pDialog->DrawText(m_Buffer.GetLine(i),m_Elements[0],&rcText,FontEffect_None,m_Buffer.GetLineLength(i),m_Buffer.GetLineData(i));
			}
			rcText.top += m_nLineHeight;
		}
	}
	else
	{
		m_Elements[0]->FontColor.Current = m_TextColor;
		for(int i=beginShowLine;i<endShowLine;++i)
		{
			m_pDialog->DrawText(m_Buffer.GetLine(i),m_Elements[0],&rcText,FontEffect_None,m_Buffer.GetLineLength(i),m_Buffer.GetLineData(i));
			rcText.top += m_nLineHeight;
		}
	}

	//GetDevice()->SetClipRect(&rcOld);

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
	if( m_bHasFocus && m_pDialog->IsFocus() && m_bCaretOn && !g_bHideCaret && !(GUI_ES_TEXTBOX & GetStyle()))
	{
		stPointI ptCaretXY;  // Left and right X cordinates of the selection region

		//
		// Compute the X coordinates of the selection rectangle
		//
		
		ptCaretXY = m_Buffer.CPtoXY( m_ptCaret, false);

		rcText = m_rcText;
		rcText.left -= ptXYFirst.x;
		rcText.top -= ptXYFirst.y;
		// Start the rectangle with insert mode caret
		stRectI rcCaret ( rcText.left + ptCaretXY.x - 1, rcText.top + ptCaretXY.y,
			rcText.left + ptCaretXY.x + 1, rcText.top + ptCaretXY.y + m_nLineHeight );

		// If we are in overwrite mode, adjust the caret rectangle
		// to fill the entire character.
		if( !m_bInsertMode )
		{
			// Obtain the right edge X coord of the current character
			rcCaret.right += GetDevice()->GetFontWidth(GetFont());
		}

		stRectI rc0 = m_rcText;
		rc0.left--;
		rc0.right ++;
		rcCaret &= rc0;
		if(!rcCaret.IsRectEmpty())
		{
			rcCaret.OffsetRect(0,-(m_nLineHeight - iFontHeight)/2);
			m_pDialog->DrawRect( &rcCaret, m_CaretColor );
		}
	}

	m_VScrollBar.Render(fElapsedTime);
	if(GetStyle() & AUTONEWLINE)
	{
	}
	else
	{
		m_HScrollBar.Render(fElapsedTime);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pRect : ÃèÊö
 * \param dwStyle : ÃèÊö
 * \param nSBWidth : ÃèÊö
 * \param pBorderInfo : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiMLEditBox::AdjustControlRect(RECT* pRect,DWORD dwStyle,int nSBWidth,stGuiCtrlBorderInfo* pBorderInfo)
{
	stRectI rcBoundBox = *pRect;
	if(dwStyle & GUI_ES_NOSKIN)
	{
		rcBoundBox.right += nSBWidth + s_nSBSpace;
	}
	else
	{
		rcBoundBox.right += nSBWidth + s_nSBSpace;
		if(pBorderInfo)
		{
			rcBoundBox.left -= pBorderInfo->ptBorderLeftTop.x;
			rcBoundBox.top -= pBorderInfo->ptBorderLeftTop.y;
			rcBoundBox.right += pBorderInfo->ptBorderRightBottom.x;
			rcBoundBox.bottom += pBorderInfo->ptBorderRightBottom.y;
		}
	}
	*pRect = rcBoundBox;
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLEditBox::UpdateRects(void)
{
	if(m_nLineHeight < GetDevice()->GetFontHeight(GetFont()))
		m_nLineHeight = GetDevice()->GetFontHeight(GetFont());

	CGuiControl::UpdateRects();

	stRectI rect = m_rcBoundingBox;
    // Update the render rectangles
	if(GetStyle() & GUI_ES_NOSKIN)
	{
		if(0 == (GetStyle() & GUI_ES_SCROLLBAR_HAS_SKIN))
			m_VScrollBar.SetStyle(CGuiScrollBar::NOSKIN);

		if(m_VScrollBar.IsVisible())
			rect.right -= m_nSBWidth + s_nSBSpace;

		CuttRect9(rect,stPointI(0,0),stPointI(0,0),m_rcRender);
		m_rcText = rect;
		stGuiCtrlBorderInfo* pBorderInfo = m_pDialog->GetCtrlBorderInfo( m_Type );
		if(0 == (GetStyle() & GUI_ES_SCROLLBAR_HAS_SKIN))
			if( pBorderInfo )
			{
				m_rcText.top += pBorderInfo->ptBorderLeftTop.x;
				m_rcText.bottom -= pBorderInfo->ptBorderRightBottom.y;
			}
	}
	else
	{
		if(m_VScrollBar.IsVisible())
			rect.right -= m_nSBWidth + s_nSBSpace;
		stGuiCtrlBorderInfo* pBorderInfo = m_pDialog->GetCtrlBorderInfo( m_Type );
		m_rcText = rect;
		if( pBorderInfo )
		{
			m_rcText.left += pBorderInfo->ptBorderLeftTop.x;
			m_rcText.top += pBorderInfo->ptBorderLeftTop.y;
			m_rcText.right -= pBorderInfo->ptBorderRightBottom.x;
			m_rcText.bottom -= pBorderInfo->ptBorderRightBottom.y;
			CuttRect9(rect,pBorderInfo->ptLeftTop,pBorderInfo->ptRightBottom,m_rcRender);
		}
	}

    //m_rcText = m_rcRender[0];

    // Inflate further by m_nSpacing

	//*
	if(GetStyle() & GUI_ES_NOSKIN)
	{
		m_VScrollBar.SetLocation( m_rcBoundingBox.right - m_nSBWidth - s_nSBSpace, m_rcBoundingBox.top );
		m_VScrollBar.SetSize( m_nSBWidth, m_rcBoundingBox.Height());
	}
	else
	//*/
	{
		//m_rcText.right -= m_nSBWidth;
		// Update the scrollbar's rects
		m_VScrollBar.SetLocation( m_rcBoundingBox.right - m_nSBWidth - s_nSBSpace, m_rcBoundingBox.top + s_nSBTopOffset );
		m_VScrollBar.SetSize( m_nSBWidth, m_rcBoundingBox.Height()  - (s_nSBTopOffset + s_nSBBottomOffset));
		if(GetStyle() & AUTONEWLINE)
		{
		}
		else
		{
			//m_rcText.bottom -= m_nSBWidth;
			// Update the scrollbar's rect
			m_HScrollBar.SetLocation( m_rcText.left, m_rcText.bottom );
			m_HScrollBar.SetSize( m_rcText.Width(),m_nSBWidth);
		}
	}

	InflateRect( &m_rcText, -m_nSpacing, -m_nSpacing );

	m_Buffer.SetLineHeight(m_nLineHeight);
	m_Buffer.SetLineWidth(m_rcText.Width());
	m_nMaxShowLineCount = m_rcText.Height()/m_nLineHeight;
	if(!m_rcText.IsRectEmpty() && (GetStyle() & AUTONEWLINE))
	{

		int indexSel = m_Buffer.CPtoIndex(m_ptSelStart);
		int indexCaret = m_Buffer.CPtoIndex(m_ptCaret);
		int indexFirstVisible = m_Buffer.CPtoIndex(m_ptFirstVisible);

		Refresh(0);
		m_ptSelStart = m_Buffer.IndextoCP(indexSel);
		m_ptFirstVisible = m_Buffer.IndextoCP(indexFirstVisible);
		//m_ptCaret = m_Buffer.IndextoCP(indexCaret);
		PlaceCaret(m_Buffer.IndextoCP(indexCaret));
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CGuiMLEditBox::OnInit()
{
	m_Buffer.Init();
	m_pDialog->InitControl( &m_VScrollBar ); 
	if(GetStyle() & AUTONEWLINE)
	{
	}
	else
	{
		m_pDialog->InitControl( &m_VScrollBar ); 
	}
	SetText(L"\n");
	m_Elements[0]->dwTextFormat |= DT_SINGLELINE;
	return S_OK;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nStartLine : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
// Ë¢ĞÂÎÄ±¾ĞĞ
void CGuiMLEditBox::Refresh(int nStartLine)
{
	m_Buffer.Refresh(nStartLine);
	m_VScrollBar.SetPageSize(m_Buffer.GetLineCount() ? max(1,m_rcText.Height()/m_Buffer.GetLineCount()) : 1);
	m_VScrollBar.SetTrackRange(0,m_Buffer.GetLineCount());
	
	if(GetStyle() & AUTONEWLINE)
	{
	}
	else
	{
		m_HScrollBar.SetTrackRange(0,m_Buffer.GetWidth());
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param line : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLEditBox::ScrollToLine(int line)
{
	m_ptFirstVisible.y = line;
	if(m_ptFirstVisible.y + m_nMaxShowLineCount > m_Buffer.GetLineCount())
	{
		m_ptFirstVisible.y = m_Buffer.GetLineCount() - m_nMaxShowLineCount;
	}

	if(m_ptFirstVisible.y < 0) m_ptFirstVisible.y = 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pt : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLEditBox::ScrollTo(POINT pt)
{
	ScrollToLine(pt.y);

	stPointI pt0;
	pt0.x = pt.x;
	pt0.y = m_ptFirstVisible.y * m_nLineHeight;
	int nLineWidth = m_Buffer.GetWidth();
	
	if(pt0.x + m_rcText.Width() > nLineWidth)
	{
		pt0.x = nLineWidth - m_rcText.Width();
	}
	
	if(pt0.x < 0) pt0.x = 0;

	int nTail;
	pt0 = m_Buffer.XYtoCP(pt0,&nTail);

	if(nTail) m_ptFirstVisible.x = pt0.x + 1;
	else m_ptFirstVisible.x = pt0.x;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param line : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CGuiMLEditBox::GetLineLength(int line)
{
	if(line < 0 || line >= m_Buffer.GetLineCount())
		return 0;
	return m_Buffer.GetLineLength(line);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CGuiMLEditBox::GetLineCount()
{
	return m_Buffer.GetLineCount();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param nLineHeight : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiMLEditBox::SetLineHeight(int nLineHeight)
{
	m_nLineHeight = nLineHeight;
	UpdateRects();
}