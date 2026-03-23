/**
 * \file      GuiControl.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾@###.com
 *           $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUI¿Ø¼ş»ùÀà
 * 
 *	      GUI¿Ø¼ş»ùÀà
 */

#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"
#include "../../luabind/public.h"
//--------------------------------------------------------------------------------------
// CGuiControl class
//--------------------------------------------------------------------------------------

void CGuiControl::bind_lua(lua_State* L)
{
	using namespace luabind;
	module(L)
	[
		class_<CGuiControl>("CGuiControl")
			.def("GetID",&CGuiControl::GetID)
			.def("GetX",&CGuiControl::GetX)
			.def("GetY",&CGuiControl::GetY)
			.def("GetWidth",&CGuiControl::GetWidth)
			.def("GetHeight",&CGuiControl::GetHeight)
			.def("SetLocation",&CGuiControl::SetLocation)
			.def("SetSize",&CGuiControl::SetSize)
			.def("GetText",&CGuiControl::GetText)
			.def("SetText",&CGuiControl::SetText)
			.def("AddItem",&CGuiControl::AddItemInScript)
			.def("GetCurItem",&CGuiControl::GetCurItem)
			.def("GetItemCount",&CGuiControl::GetItemCount)
			.def("SetCurItem",&CGuiControl::SetCurItem)
			.def("SetValue",&CGuiControl::SetValue)
			.def("GetValue",&CGuiControl::GetValue)
			.def("SetRange",&CGuiControl::SetRange)
			.def("GetRangeMin",&CGuiControl::GetRangeMin)
			.def("GetRangeMax",&CGuiControl::GetRangeMax)
			.def("GetPageSize",&CGuiControl::GetPageSize)
			.def("SetPageSize",&CGuiControl::SetPageSize)
			.def("SetVisible",&CGuiControl::SetVisible)
			.def("SetEnabled",&CGuiControl::SetEnabled)
			.def("IsVisible",&CGuiControl::IsVisible)
			.def("IsEnabled",&CGuiControl::IsEnabled)
	];
}
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param DT_CENTER : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
CGuiControl::CGuiControl( CGuiDialog *pDialog )
: m_dwStyle(0)
, m_TextColor(COLOR_ARGB(255,255,255,255))
, m_bDeleted(false)
, m_iFont(1)
, m_rectClip(-10000,-10000,10000,10000)
, m_TipsType(GUI_ALIGN_LEFTTOP)
//, m_dwTextFormat(DT_VCENTER | DT_CENTER)
{
    m_Type = GUI_CONTROL_BUTTON;
    m_pDialog = pDialog;
    m_ID = 0;
    m_pUserData = NULL;

    m_bEnabled = true;
    m_bVisible = true;
    m_bMouseOver = false;
    m_bHasFocus = false;
    m_bIsDefault = false;

    m_x = 0;
    m_y = 0;
    m_width = 0;
    m_height = 0;

	m_dwBeginEnableTime = 0;
	m_dwDelayEnableTime = 0;

	m_dwBeginHideTime = 0;
	m_dwDelayHideTime = 0;

   ZeroMemory( &m_rcBoundingBox, sizeof( m_rcBoundingBox ) );
#ifdef _DEBUG
   m_bDrag = false;
   m_bSizing = false;
   m_dwHiTest = HTERROR;
#endif
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiControl::~CGuiControl()
{
    for( int i = 0; i < m_Elements.size(); ++i )
    {
        delete m_Elements[i];
    }
    m_Elements.resize(0);
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
void CGuiControl::SetTextColor( DWORD Color)
{
	m_TextColor = Color;
	Color &= 0x00ffffff;
	for(size_t i=0;i<m_Elements.size();++i)
	{
		for( int j=0; j < MAX_CONTROL_STATES; j++ )
		{
			CGuiElement* pEmt = m_Elements[i];
			pEmt->FontColor.States[j] = (pEmt->FontColor.States[j] & 0xff000000) | Color;
		}
	}
	/*
	if(m_Elements.size() == 0) return;
    CGuiElement* pElement = m_Elements[ 0 ];

    if( pElement )
        pElement->FontColor.States[GUI_STATE_NORMAL] = Color;
		//*/
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
DWORD CGuiControl::GetTextColor()
{
	return m_TextColor;
	/*CGuiElement* pElement = m_Elements[ 0 ];

    if( pElement )
        return pElement->FontColor.States[GUI_STATE_NORMAL];
	return -1;*/
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iElement : ÃèÊö
 * \param pElement : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
HRESULT CGuiControl::SetElement( UINT iElement, CGuiElement* pElement )
{
    HRESULT hr = S_OK;

    if( pElement == NULL )
        return E_INVALIDARG;

    // Make certain the array is this large
    for( UINT i=m_Elements.size(); i <= iElement; i++ )
    {
        CGuiElement* pNewElement = new CGuiElement();
        if( pNewElement == NULL )
            return E_OUTOFMEMORY;

        m_Elements.push_back( pNewElement );
    }

    // Update the data
    CGuiElement* pCurElement = m_Elements[ iElement ];
    *pCurElement = *pElement;
    
    return S_OK;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
void CGuiControl::Refresh()
{
	m_bMouseOver = false;
	m_bHasFocus = false;

	for( int i=0; i < m_Elements.size(); i++ )
	{
		CGuiElement* pElement = m_Elements[ i ];
		pElement->Refresh();
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
void CGuiControl::UpdateRects()
{
	m_rcBoundingBox.SetRect( m_x, m_y, m_x + m_width, m_y + m_height );
	for(size_t i=0;i<m_Elements.size();++i)
	{
		m_Elements[i]->iFont = m_iFont;
	}
}


/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param pInsertAfter : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
void CGuiControl::SetZOrder(CGuiControl* pInsertAfter)
{
	m_pDialog->SetControlZOrder(this,pInsertAfter);
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
#ifdef _DEBUG
static CGuiControl * s_pMouseOverControl = NULL;
static CGuiControl * s_pCurMoveControl = NULL;
static CGuiControl * s_pCurSizingControl = NULL;
#endif
bool CGuiControl::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam ) 
{
#ifdef _DEBUG
	if(!IsVisible()) return false;
	if(GetKeyState(VK_SHIFT) < 0 && GetKeyState(VK_CONTROL) < 0)
	{
		static stPointI ptOffset;
		if(m_bDrag)
		{
			if(s_pCurMoveControl != this) 
			{
				m_bDrag = false;
				return true;
			}
			if(uMsg == WM_LBUTTONUP)
			{
				m_bDrag = false;
				return true;
			}
			else if(uMsg == WM_MOUSEMOVE)
			{
				pt -= ptOffset;
				SetLocation(pt.x,pt.y);
				return true;
			}
		}
		else if(m_bSizing)
		{
			if(s_pCurSizingControl != this) 
			{
				m_bSizing = false;
				return true;
			}
			if(uMsg == WM_LBUTTONUP)
			{
				m_bSizing = false;
				return true;
			}
			else if(uMsg == WM_MOUSEMOVE)
			{
				pt -= ptOffset;
				stRectI rc(m_x,m_y,GetWidth() + m_x,GetHeight() + m_y);
				switch(m_dwHiTest)
				{
				case HTLEFT       ://    10
					rc.left = pt.x;
					break;
				case HTTOP        ://    12
					rc.top = pt.y;
					break;
				case HTTOPLEFT    ://    13
					rc.left = pt.x;
					rc.top = pt.y;
					break;
				case HTRIGHT      ://    11
					rc.right = pt.x;
					break;
				case HTTOPRIGHT   ://    14
					rc.top = pt.y;
					rc.right = pt.x;
					break;
				case HTBOTTOM     ://    15
					rc.bottom = pt.y;
					break;
				case HTBOTTOMLEFT ://    16
					rc.bottom = pt.y;
					rc.left = pt.x;
					break;
				case HTBOTTOMRIGHT://    17
					rc.right = pt.x;
					rc.bottom = pt.y;
					break;
				}
				SetLocation(rc.left,rc.top);
				SetSize(rc.Width(),rc.Height());
				return true;
			}
		}
		else
		{
			m_bDrag = false;
			m_bSizing = false;
			if(uMsg == WM_LBUTTONDOWN)
			{
				bool bHandle = false;
				m_dwHiTest = OnHiTest(pt);
				switch(m_dwHiTest)
				{
				case HTCLIENT:
					m_pDialog->RequestFocus(this);
					m_bDrag = true;
					ptOffset = pt - GetLocation();
					s_pCurMoveControl = this;
					bHandle = true;
					break;
				case HTLEFT       ://    10
				case HTRIGHT      ://    11
				case HTTOP        ://    12
				case HTTOPLEFT    ://    13
				case HTTOPRIGHT   ://    14
				case HTBOTTOM     ://    15
				case HTBOTTOMLEFT ://    16
				case HTBOTTOMRIGHT://    17
					m_bSizing = true;
					m_pDialog->RequestFocus(this);
					s_pMouseOverControl = this;
					s_pCurSizingControl = this;
					bHandle = true;
					break;
				default:
					break;
				}
				stPointI pt0 = GetLocation();
				switch(m_dwHiTest)
				{
				case HTLEFT       ://    10
				case HTTOP        ://    12
				case HTTOPLEFT    ://    13
					ptOffset = pt - pt0;
					break;
				case HTRIGHT      ://    11
				case HTTOPRIGHT   ://    14
					pt0.x += GetWidth();
					ptOffset = pt - pt0;
					break;
				case HTBOTTOM     ://    15
				case HTBOTTOMLEFT ://    16
					pt0.y += GetHeight();
					ptOffset = pt - pt0;
					break;
				case HTBOTTOMRIGHT://    17
					pt0.x += GetWidth();
					pt0.y += GetHeight();
					ptOffset = pt - pt0;
					break;
				}
				return bHandle;
			}
			else if(uMsg == WM_MOUSEMOVE)
			{
				m_dwHiTest = OnHiTest(pt);
				switch(m_dwHiTest)
				{
				case HTCLIENT:
				case HTLEFT       ://    10
				case HTRIGHT      ://    11
				case HTTOP        ://    12
				case HTTOPLEFT    ://    13
				case HTTOPRIGHT   ://    14
				case HTBOTTOM     ://    15
				case HTBOTTOMLEFT ://    16
				case HTBOTTOMRIGHT://    17
					s_pMouseOverControl = this;
					break;
				}
			}
		}
	}
	else
	{
		m_bSizing = false;
		m_bDrag = false;
		s_pMouseOverControl = NULL;
	}
#endif
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
DWORD CGuiControl::OnHiTest(POINT pt)
{
	if(ContainsPoint(pt))
	{
		int d = 3;
		if(pt.x - m_rcBoundingBox.left < d)
		{
			//left
			if(pt.y - m_rcBoundingBox.top < d)
			{
				// top
				return HTTOPLEFT;
			}
			else if(m_rcBoundingBox.bottom - pt.y < d)
			{
				// bottom;
				return HTBOTTOMLEFT;
			}
			return HTLEFT;
		}
		else if(m_rcBoundingBox.right - pt.x < d)
		{
			//left
			if(pt.y - m_rcBoundingBox.top < d)
			{
				// top
				return HTTOPRIGHT;
			}
			else if(m_rcBoundingBox.bottom - pt.y < d)
			{
				// bottom;
				return HTBOTTOMRIGHT;
			}
			return HTRIGHT;
		}
		else
		{
			//left
			if(pt.y - m_rcBoundingBox.top < d)
			{
				// top
				return HTTOP;
			}
			else if(m_rcBoundingBox.bottom - pt.y < d)
			{
				// bottom;
				return HTBOTTOM;
			}
		}
		return HTCLIENT;
	}
	return HTERROR;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param fElapsedTime : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
void CGuiControl::Render( float fElapsedTime )
{
#ifdef _DEBUG
	if(s_pMouseOverControl == this)
	{
		switch(m_dwHiTest)
		{
		case HTLEFT       ://    10
		case HTRIGHT      ://    11
		case HTTOP        ://    12
		case HTTOPLEFT    ://    13
		case HTTOPRIGHT   ://    14
		case HTBOTTOM     ://    15
		case HTBOTTOMLEFT ://    16
		case HTBOTTOMRIGHT://    17
			RenderBoundingBox(fElapsedTime);
			break;
		}
	}
#endif



	if( m_dwBeginEnableTime != 0 )
	{
		if( xtimeGetTime() - m_dwBeginEnableTime > m_dwDelayEnableTime )
		{
			m_bEnabled = true;
			m_dwBeginEnableTime = 0;
			m_dwDelayEnableTime = 0;
		}
	}

	if( m_dwBeginHideTime != 0 )
	{
		if( xtimeGetTime() - m_dwBeginHideTime > m_dwDelayHideTime )
		{
			m_bVisible = false;
			m_dwBeginHideTime = 0;
			m_dwDelayHideTime = 0;
		}
	}

	if( !m_ToolTips.Empty() && IsVisible() && IsEnabled() && GetGuiManager()->GetToolTips() == NULL && m_bMouseOver && GetGuiManager()->GetMouseOverDlg() == m_pDialog)
	{
		//GetGuiManager()->SetToolTips(&m_ToolTips,stPointI(m_pDialog->GetClientX() + m_rcBoundingBox.left - 20,m_pDialog->GetClientY() + m_rcBoundingBox.top - 20));
		stPointI pt;
		pt.x = m_pDialog->GetClientX() + m_rcBoundingBox.left - m_ToolTips.GetWidth();
		if(pt.x < 0)
			pt.x = m_pDialog->GetClientX() + m_rcBoundingBox.right;

		switch(m_TipsType) {
		case GUI_ALIGN_LEFTTOP:
			{
				pt.y = m_pDialog->GetClientY() + m_rcBoundingBox.top - m_ToolTips.GetHeight();
				if(pt.y < 0)
					pt.y = m_pDialog->GetClientY() + m_rcBoundingBox.bottom;
				if ( pt.y < 0 )
					pt.y = 0;
			}
			break;
		case GUI_ALIGN_LEFTBUTTOM:
			{
				pt.y = m_pDialog->GetClientY() + m_rcBoundingBox.bottom ;
				if(pt.y > GetDevice()->GetHeight())
					pt.y = GetDevice()->GetHeight() - m_ToolTips.GetHeight();
			}
			break;
		}
		
		
		

		GetGuiManager()->SetToolTips(&m_ToolTips, pt);
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
void CGuiControl::RenderBoundingBox( float fElapsedTime )
{
	m_pDialog->DrawRectangle(&m_rcBoundingBox,COLOR_ARGB(255,255,0,0));
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
bool CGuiControl::IsFocus()
{
	return CGuiDialog::IsFocus(this);
}

//void CGuiControl::SaveConfigXML(TiXmlElement* item)
//{
//	
//}
//
//void CGuiControl::LoadConfigXML(TiXmlElement* item)
//{
//}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param dwStyle : ÃèÊö
* \param bReinit : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
void CGuiControl::SetStyle( DWORD dwStyle, bool bReinit) 
{ 
	m_dwStyle = dwStyle;
	if(bReinit && m_pDialog)
	{
		OnInit();
		UpdateRects();
	}
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param pszText : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
void CGuiControl::SetToolTips(const char* pszText)
{
	m_ToolTips.Clear();
	m_ToolTips.SetCurColor( 0xFFFFFFFF );
	m_ToolTips.AddText(pszText);
	m_ToolTips.Resize();
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
const char* CGuiControl::GetToopTips()
{
	if(m_ToolTips.Empty()) return NULL;
	return m_ToolTips.GetText();
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param x : ÃèÊö
* \param y : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
void CGuiControl::SetLocation( int x, int y ) 
{ 
	m_x = x; m_y = y; 
	UpdateRects(); 
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param width : ÃèÊö
* \param height : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
void CGuiControl::SetSize( int width, int height ) 
{ 
	m_width = width;
	m_height = height;
	UpdateRects(); 
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
void CGuiControl::OnMouseEnter() 
{ 
	m_bMouseOver = true;
	m_pDialog->SendEvent(EVENT_CONTROL_MOUSEENTER,true,this);
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
void CGuiControl::OnMouseLeave() 
{ 
	m_bMouseOver = false;
	m_pDialog->SendEvent(EVENT_CONTROL_MOUSELEAVE,true,this);
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param lua : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
void CGuiControl::RegisterScript(lua_State* lua)
{
	bind_lua(lua);
	CGuiButton::bind_lua(lua);
	CGuiTab::bind_lua(lua);
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param pt : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
POINT CGuiControl::ScreenToClient(POINT pt)
{
	pt.x -= (m_pDialog->GetClientX() + m_x);
	pt.y -= (m_pDialog->GetClientY() + m_y);
	return pt;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param iFont : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
void CGuiControl::SetFont(int iFont)
{
	if(iFont != m_iFont)
	{
		m_iFont = iFont;
		for(size_t i=0;i<m_Elements.size();++i)
		{
			m_Elements[i]->iFont = m_iFont;
		}

		if(m_pDialog) UpdateRects();
	}
}

void CGuiControl::SetClipRect(const RECT * prect/*Ïà¶Ô¸¸´°¿ÚµÄ¿Í»§ÇøÓò*/)
{
	m_rectClip = *prect;
	UpdateRects();
	stRectI rc = m_rcBoundingBox & m_rectClip;
	if(rc.IsRectEmpty()) SetVisible(false);
	else SetVisible(true);
}

bool CGuiControl::ContainsPoint( POINT pt ) 
{ 
	if(!m_rectClip.PtInRect(pt)) return false;
	return m_rcBoundingBox.PtInRect(pt); 
}

void CGuiControl::RequestFocus()
{
	if( NULL != m_pDialog )
	{
		m_pDialog->SetFocus( this );
	}
}