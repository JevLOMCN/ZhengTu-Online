#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"
#include "../../luabind/public.h"

stPointI CGuiScrollTab::s_borderTopLeft(0,0);
stPointI CGuiScrollTab::s_borderRightBottom(0,0);
int     CGuiScrollTab::s_nScrollItemHeight=19;

void CGuiScrollTab::bind_lua(lua_State * L)
{
	using namespace luabind;
	module(L)
	[
		class_<CGuiScrollTab,CGuiControl>("CGuiScrollTab")
			.def("AddControl",&CGuiScrollTab::AddControl)
	];
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiScrollTab::CGuiScrollTab(CGuiDialog* pDialog)
: CGuiControl(pDialog)
, m_eHeaderAlign(CGuiScrollTab::HeaderAlign_Top)
, m_bPressed(false)
, m_nPressItem(-1)
{
	
	m_nSelected = -1;
	m_nMouseOver = -1;
	m_Type = GUI_CONTROL_SCROLLTAB;
	m_TextColor = COLOR_ARGB(255,255,255,255);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiScrollTab::UpdateRects()
{
	CGuiControl::UpdateRects();

	CuttRect9(m_rcBoundingBox,s_borderTopLeft,s_borderRightBottom,m_rcRender);

	UpdateScrollTabRect();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiScrollTab::UpdateScrollTabRect()
{
	m_rcCenter = m_rcRender[0];

	int nSize = m_items.size();
	stPointI pt(4,4);

	for(int i=0 ; i < nSize; ++i)
	{
		m_items[i].rectTop.SetRect(m_rcCenter.left,m_rcCenter.top+i*s_nScrollItemHeight,
			m_rcCenter.right,m_rcCenter.top+(i+1)*s_nScrollItemHeight);

		m_items[i].rectBottom.SetRect(m_rcCenter.left,m_rcCenter.bottom-(nSize-i)*s_nScrollItemHeight,
			m_rcCenter.right,m_rcCenter.bottom-(nSize-i-1)*s_nScrollItemHeight);
	
		if (i<=m_nSelected)
		{
			m_items[i].bTop=true;
			CuttRect9( m_items[i].rectTop,pt,pt,m_items[i].rcRender );
		}
		else
		{
			m_items[i].bTop=false;
			CuttRect9( m_items[i].rectBottom,pt,pt,m_items[i].rcRender );
		}
	
	}

}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pszCaption : ÃèÊö
 * \param pData : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiScrollTab::AddItem(const char* pszCaption,void* pData)
{
	stScrollTabItem item;
	strncpy(item.szText,pszCaption,sizeof(item.szText));
	item.szText[sizeof(item.szText)-1] = 0;
	item.data = (DWORD)pData;
	m_items.push_back(item);
	UpdateScrollTabRect();
	if(m_items.size() == 1)
		SetCurItem(0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiScrollTab::SetCurItem(int index)
{
	if((size_t)index >= m_items.size()) return;
	if(m_nSelected != -1)
	{
		stScrollTabItem & item = m_items[m_nSelected];
		for(std::vector<CGuiControl*>::iterator it = item.aControls.begin(); it != item.aControls.end();++it)
		{
			(*it)->SetVisible(false);
		}
	}

	stScrollTabItem & item = m_items[index];
	m_nSelected = index;

	UpdateScrollTabRect();

	for(std::vector<CGuiControl*>::iterator it = item.aControls.begin(); it != item.aControls.end();++it)
	{
		(*it)->SetVisible(true);
	}

	m_pDialog->SendEvent( EVENT_TAB_SELECTION_CHANGED, true, this );
	
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \param fElapsedTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
// 1 2 3
// 4 0 5
// 6 7 8
void CGuiScrollTab::RenderItem(int index,float fElapsedTime)
{
	int iState = GUI_STATE_NORMAL;

	if(index == m_nSelected)
	{
		iState = GUI_STATE_PRESSED;
	}
	else if(index == m_nMouseOver)
	{
		iState = GUI_STATE_MOUSEOVER;
	}

	//float fBlendRate = ( iState == GUI_STATE_PRESSED ) ? 0.0f : 0.8f;

	float fBlendRate = 0.0f;

	stScrollTabItem& item = m_items[index];
	
	if (item.bTop)
	{
		m_pDialog->DrawControlBorder(item.rectTop,GUI_CONTROL_SCROLLTAB,-1,true);
	}
	else
		m_pDialog->DrawControlBorder(item.rectBottom,GUI_CONTROL_SCROLLTAB,-1,true);

	CGuiElement* pE = GetElement(0);
	if(pE)
	{
		pE->FontColor.Blend(GUI_STATE_NORMAL,fElapsedTime,fBlendRate);
		stRectI rc = item.rcRender[0];
		
		//stRectI rcBk=rc;
		//rcBk.OffsetRect(m_pDialog->GetClientX(),m_pDialog->GetClientY());
		//GetDevice()->FillRect(&rcBk,0xffff0000);
		rc.top -= 3;
		rc.bottom += 3;
		pE->dwTextFormat = DT_VCENTER|DT_CENTER;
		m_pDialog->DrawText( item.szText,pE,&rc);

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
void CGuiScrollTab::Render( float fElapsedTime )
{
	CGuiControl::Render(fElapsedTime);

	//m_pDialog->DrawControlBorder(m_rcBoundingBox,m_Type,-1,true);

	for(size_t i=0;i<m_items.size();++i)
	{
		if(i != m_nSelected)RenderItem(i,fElapsedTime);
	}

	if(m_nSelected != -1)
		RenderItem(m_nSelected,fElapsedTime);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
RECT CGuiScrollTab::GetItemRect(int index)
{
	if((size_t)index >= m_items.size()) 
	{
		stRectI rc(0,0,0,0);
		return rc;
	}
	if (m_items[index].bTop)
		return m_items[index].rectTop;
	else
		return m_items[index].rectBottom;

}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ptOffset : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CGuiScrollTab::GetItemAtPos(POINT ptOffset)
{
	for(size_t i=0;i<m_items.size();++i)
	{
		stRectI rc = GetItemRect(i);
		if(rc.PtInRect(ptOffset))
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
 * \param uMsg : ÃèÊö
 * \param pt : ÃèÊö
 * \param wParam : ÃèÊö
 * \param lParam : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiScrollTab::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
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
			
			stPointI ptOffset(pt.x ,pt.y );
			int ix;
			if( (ix = GetItemAtPos(ptOffset)) != -1)
			{
				m_nPressItem = ix;
				// Pressed while inside the control
				m_bPressed = true;

				//if( !m_bHasFocus )
				//	m_pDialog->RequestFocus( this );

				return true;
			}
			
			break;
		}

	case WM_LBUTTONUP:
		{
			if( m_bPressed )
			{
				m_bPressed = false;

				//m_pDialog->ClearFocus();

				// Button click
				int ix;
				stPointI ptOffset(pt.x ,pt.y );
				if( (ix = GetItemAtPos(ptOffset)) != -1 && ix == m_nPressItem)
				{
					SetCurItem(ix);
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
 * \param index : ÃèÊö
 * \param pControl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiScrollTab::AddControl(int index,CGuiControl* pControl)
{
	if((size_t)index >= m_items.size()) return ;
	if(!pControl) return;
	stScrollTabItem & item = m_items[index];
	item.aControls.push_back(pControl);
	pControl->SetVisible(index == m_nSelected);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \param pControl : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiScrollTab::RemoveControl(int index,CGuiControl* pControl)
{
	if((size_t)index >= m_items.size()) return ;
	stScrollTabItem & item = m_items[index];
	for(std::vector<CGuiControl*>::iterator it = item.aControls.begin(); it != item.aControls.end();++it)
	{
		if((*it) == pControl)
		{
			item.aControls.erase(it);
			return;
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
const char* CGuiScrollTab::GetItemText(int index)
{
	if((size_t)index >= m_items.size()) return NULL;
	return m_items[index].szText;
}
