/**
 * \file      GuiTab.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾@###.com
 *           $$$03¹«Ë¾@###.com
 * \date      2005-07-05 17:02:57
 * \brief     ********************************************************************
 * 
 *	      ********************************************************************
 */

#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"
#include "../../luabind/public.h"
stPointI CGuiTab::s_borderTopLeft(10,10);
stPointI CGuiTab::s_borderRightBottom(9,13);

void CGuiTab::bind_lua(lua_State* L)
{
	using namespace luabind;
	module(L)
	[
		class_<CGuiTab,CGuiControl>("CGuiTab")
			.def("AddControl",&CGuiTab::AddControl)
	];
}
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiTab::CGuiTab(CGuiDialog* pDialog)
: CGuiControl(pDialog)
, m_eHeaderAlign(CGuiTab::HeaderAlign_Top)
, m_Panel(pDialog)
, m_bPressed(false)
, m_nPressItem(-1)
{
	m_ptItemSpacing = stPointI(2,3);
	m_nSelected = -1;
	m_nMouseOver = -1;
	m_Type = GUI_CONTROL_TAB;
	m_TextColor = COLOR_ARGB(255,255,255,255);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiTab::~CGuiTab(void)
{
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiElement* CGuiTab::GetItemElement(int index)
{
	return NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CGuiTab::GetItemElementCount(int index)
{
	return 0;
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
void CGuiTab::RenderItem(int index,float fElapsedTime)
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

	stTabItem& item = m_items[index];
	for(int i = 0; i < 2; ++i)
	{
		for(int j=0; j < 9; ++j)
		{
			CGuiElement* pE = GetAElement(j,i);
			
			if(!pE) break;
			
			pE->TextureColor.Current = pE->TextureColor.States[iState];
			stRectI rc = item.rcRender[j];
			rc.OffsetRect(GetX(),GetY());

			//m_pDialog->DrawSprite( pE,&rc );
			m_pDialog->PatchDrawSprite(pE,&rc,s_PatchAlignFormat[j]);
		}
	}
	CGuiElement* pE = GetAElement(0,0);
	if(pE)
	{
		pE->FontColor.Blend(GUI_STATE_NORMAL,fElapsedTime,fBlendRate);
		stRectI rc = item.rcRender[0];
		if(m_eHeaderAlign == HeaderAlign_Bottom)
			rc.top += 10;	
		else
			rc.bottom -= 10;
		rc.OffsetRect(GetX(),GetY());

		m_pDialog->DrawText( item.szShowText,pE,&rc);
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
void CGuiTab::Render( float fElapsedTime )
{
	if(!m_items.empty())
	{
		stGuiCtrlBorderInfo * pInfo = m_pDialog->GetCtrlBorderInfo(GUI_CONTROL_TAB);
		stRectI rect;
		rect.left = m_items.front().rect.left;
		rect.top = m_items.front().rect.top;
		rect.bottom = m_items.front().rect.bottom;
		//rect.top -= pInfo->ptBorderLeftTop.y;
		rect.right = m_items.back().rect.right;
		rect.OffsetRect(m_x,m_y);
		m_pDialog->DrawControlBorder(rect,GUI_CONTROL_TAB,m_pDialog->GetColor());
	}

	CGuiControl::Render(fElapsedTime);
	for(size_t i=0;i<m_items.size();++i)
	{
		if(i != m_nSelected)RenderItem(i,fElapsedTime);
	}
	if(m_Panel.IsVisible())
		m_Panel.Render(fElapsedTime);
	if(m_nSelected != -1)
		RenderItem(m_nSelected,fElapsedTime);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiTab::UpdateRects()
{
	CGuiControl::UpdateRects();
	UpdateItemShowText();
	int addx = s_borderTopLeft.x + s_borderRightBottom.x;
	int addy = s_borderTopLeft.y + s_borderRightBottom.y;
	//int addx = s_borderTopLeft.x + s_borderRightBottom.x;
	//int addy = s_borderTopLeft.y + s_borderRightBottom.y;
	int iFont = GetDevice()->SetFont(GetFont());
	if(m_eHeaderAlign == HeaderAlign_Top || m_eHeaderAlign == HeaderAlign_Bottom)
	{
		int x = 5;
		for(t_Items::iterator it = m_items.begin(); it != m_items.end(); ++it)
		{
			SIZE size = GetDevice()->GetStringSize((*it).szShowText,-1,-1);
			(*it).rect.left = x;
			(*it).rect.right = x + size.cx + 2 * m_ptItemSpacing.x + addx;

			if(m_eHeaderAlign == HeaderAlign_Top)
			{
				(*it).rect.top = 0;
				(*it).rect.bottom = size.cy + 2 * m_ptItemSpacing.y + addy;//GetHeight();
			}
			else
			{
				(*it).rect.top = GetHeight() - (size.cy + 2 * m_ptItemSpacing.y + addy);
				(*it).rect.bottom = GetHeight();
			}

			stRectI rect = (*it).rect;
			CuttRect9( rect,s_borderTopLeft,s_borderRightBottom,(*it).rcRender );
			x = (*it).rect.right;
		}

		SIZE size = GetDevice()->GetStringSize("Ñ¡Ïî",-1,-1);
		stRectI rcPanel;
		if(m_eHeaderAlign == HeaderAlign_Top)
			rcPanel.SetRect(-6,size.cy + 2 * m_ptItemSpacing.y + addy - CGuiPanel::s_borderTopLeft.y,GetWidth(),GetHeight());
		else
			rcPanel.SetRect(-6,0,GetWidth(),GetHeight() - (size.cy + 2 * m_ptItemSpacing.y + addy) + CGuiPanel::s_borderRightBottom.y );

		m_Panel.SetVisible(!rcPanel.IsRectEmpty());
		rcPanel.OffsetRect(GetX(),GetY());
		m_Panel.SetLocation(rcPanel.left,rcPanel.top);
		m_Panel.SetSize(rcPanel.Width(),rcPanel.Height());
	}
	else
	{
		int y = 0;
		for(t_Items::iterator it = m_items.begin(); it != m_items.end(); ++it)
		{
			SIZE size = GetDevice()->GetStringSize((*it).szShowText,-1,-1);
			(*it).rect.top = y;
			(*it).rect.bottom = y + size.cy + 2 * m_ptItemSpacing.x + addy;
			if(m_eHeaderAlign == HeaderAlign_Left)
			{
				(*it).rect.left = 0;
				(*it).rect.right = size.cx + 2 * m_ptItemSpacing.y + addx;//GetWidth();
			}
			else
			{
				(*it).rect.left = GetWidth() - (size.cx + 2 * m_ptItemSpacing.y + addx);
				(*it).rect.right = GetWidth();//GetWidth();
			}

			stRectI rect = (*it).rect;
			CuttRect9( rect,s_borderTopLeft,s_borderRightBottom,(*it).rcRender );

			y = (*it).rect.bottom;
		}
		SIZE size = GetDevice()->GetStringSize("Ñ¡Ïî",-1,-1);
		stRectI rcPanel;
		
		if(m_eHeaderAlign == HeaderAlign_Left)
			rcPanel.SetRect(size.cx + 2 * m_ptItemSpacing.y + addx - CGuiPanel::s_borderTopLeft.x,0,GetWidth(),GetHeight());
		else
			rcPanel.SetRect(0,0,GetWidth()-(size.cx + 2 * m_ptItemSpacing.y + addx - CGuiPanel::s_borderRightBottom.x),GetHeight());

		m_Panel.SetVisible(!rcPanel.IsRectEmpty());
		rcPanel.OffsetRect(GetX(),GetY());
		m_Panel.SetLocation(rcPanel.left,rcPanel.top);
		m_Panel.SetSize(rcPanel.Width(),rcPanel.Height());
	}
	GetDevice()->SetFont(iFont);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiTab::UpdateItemShowText()
{
	if(m_eHeaderAlign == HeaderAlign_Top || m_eHeaderAlign == HeaderAlign_Bottom)
	{
		for(t_Items::iterator it = m_items.begin(); it != m_items.end(); ++it)
		{
			strcpy((*it).szShowText,(*it).szText);
		}
	}
	else
	{
		for(t_Items::iterator it = m_items.begin(); it != m_items.end(); ++it)
		{
			// ÎÄ×Ö´¹Ö±ÅÅÁĞ
			char* dest = (*it).szShowText;
			char* src = (*it).szText;
			for(;;)
			{
				int c = _mbsnextc((unsigned char*)src);
				char* p = (char*)_mbsinc((unsigned char*)src);
				memcpy(dest,p,p-src);
				dest += (p-src);
				src = p;
				if(*src) *dest++ = '\n';
				else break;
			}
			*dest = 0;
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
void CGuiTab::AddItem(const char* pszCaption,void* pData)
{
	stTabItem item;
	strncpy(item.szText,pszCaption,sizeof(item.szText));
	item.szText[sizeof(item.szText)-1] = 0;
	item.data = (DWORD)pData;
	m_items.push_back(item);
	UpdateRects();
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
void CGuiTab::SetCurItem(int index)
{
	if((size_t)index >= m_items.size()) return;
	if(m_nSelected != -1)
	{
		stTabItem & item = m_items[m_nSelected];
		for(std::vector<CGuiControl*>::iterator it = item.aControls.begin(); it != item.aControls.end();++it)
		{
			(*it)->SetVisible(false);
		}
	}
	stTabItem & item = m_items[index];
	m_nSelected = index;

	for(std::vector<CGuiControl*>::iterator it = item.aControls.begin(); it != item.aControls.end();++it)
	{
		(*it)->SetVisible(true);
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
bool CGuiTab::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
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
			stPointI ptOffset(pt.x - GetX(),pt.y - GetY());
			int ix;
			if( (ix = GetItemAtPos(ptOffset)) != -1)
			{
				m_nPressItem = ix;
				// Pressed while inside the control
				m_bPressed = true;

				if( !m_bHasFocus )
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

				m_pDialog->ClearFocus();

				// Button click
				int ix;
				stPointI ptOffset(pt.x - GetX(),pt.y - GetY());
				if( (ix = GetItemAtPos(ptOffset)) != -1 && ix == m_nPressItem)
				{
					Engine_GetCallBack()->PlayUISound(m_nSoundIndex);
					SetCurItem(ix);
					m_pDialog->SendEvent( EVENT_TAB_SELECTION_CHANGED, true, this );
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
 * \param ptOffset : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CGuiTab::GetItemAtPos(POINT ptOffset)
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
 * \param index : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
RECT CGuiTab::GetItemRect(int index)
{
	if((size_t)index >= m_items.size()) 
	{
		stRectI rc(0,0,0,0);
		return rc;
	}
	return m_items[index].rect;

}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiTab::RemoveItem(int index)
{
	if((size_t)index >= m_items.size()) return ;
	t_Items::iterator it = m_items.begin();
	it += index;

	stTabItem & item = m_items[index];
	for(std::vector<CGuiControl*>::iterator it1 = item.aControls.begin(); it1 != item.aControls.end();++it1)
	{
		(*it1)->SetVisible(false);
	}

	m_items.erase(it);

	if(m_nSelected == index)
	{
		m_nSelected++;
		if((size_t)m_nSelected >= m_items.size()) m_nSelected--;
	}

	UpdateRects();
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
void CGuiTab::AddControl(int index,CGuiControl* pControl)
{
	if((size_t)index >= m_items.size()) return ;
	if(!pControl) return;
	stTabItem & item = m_items[index];
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
void CGuiTab::RemoveControl(int index,CGuiControl* pControl)
{
	if((size_t)index >= m_items.size()) return ;
	stTabItem & item = m_items[index];
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
const char* CGuiTab::GetItemText(int index)
{
	if((size_t)index >= m_items.size()) return NULL;
	return m_items[index].szText;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \param pszText : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiTab::SetItemText(int index,const char* pszText)
{
	if((size_t)index >= m_items.size()) return;
	stTabItem & item = m_items[index];
	strncpy(item.szText,pszText,sizeof(item.szText));
	item.szText[sizeof(item.szText)] = 0;
	UpdateRects();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
stRectI CGuiTab::GetPanelRect()
{
	return m_Panel.GetBoundingBox();
}


HRESULT CGuiTab::OnInit() 
{ 
	return m_pDialog->InitControl( &m_Panel ); 
}