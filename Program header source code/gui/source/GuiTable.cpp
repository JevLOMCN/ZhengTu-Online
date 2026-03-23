/**
 * \file      GuiTable.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾@###.com
 *           $$$03¹«Ë¾@###.com
 * \date      2005-07-05 17:02:57
 * \brief     GUI±í¸ñÀà
 * 
 *	      GUI±í¸ñÀà
 */

#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"
#include "../include/guifloatitem.h"
#include "../../Engine/include/stack_alloc.h"

stPointI CGuiTable::s_ctrlBorderTopLeft(35,35);
stPointI CGuiTable::s_ctrlBorderRightBottom(36,36);
stRectI	CGuiTable::s_ctrlBorderRect(0,0,246,246);
SIZE	CGuiTable::s_ctrlCellSize = {35,35};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param row : ÃèÊö
 * \param col : ÃèÊö
 * \param rowCount : ÃèÊö
 * \param colCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
static stPointI s_cellOffsetTopLeft(0,0);
static stPointI s_cellOffsetRightBottom(1,1);
// 0 1 2
// 3 4 5
// 6 7 8
static int GetBackElementIndex(int row,int col,int rowCount,int colCount)
{
	if(row == 0)
	{
		if(col == 0)
			return 0;
		if(col + 1 >= colCount)
			return 2;
		return 1;
	}

	if(row + 1 == rowCount)
	{
		if(col == 0)
			return 6;
		if(col + 1 >= colCount)
			return 8;
		return 7;
	}

	if(col == 0) return 3;
	if(col + 1 >= colCount) return 5;

	return 4;
}

bool (* g_fnItemTableEvent)(CGuiTable* pTable,UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam) = NULL;
void (* g_fnItemRender)(CGuiTable* pTable,CGuiItemCell* pCell,float fElapsedTime) = NULL;
void (* g_fnTableRender)(CGuiTable* pTable,float fElapsedTime) = NULL;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiTable::CGuiTable(CGuiDialog *pDialog )
: CGuiControl(pDialog)
, m_ScrollBar(pDialog)
, m_bPressed(false)
, m_iTableType(0)
, m_iTableID(0)
, m_bVirtual(false)
, m_EquipPosition(0,0)
, m_itemRect(0,0,0,0)
, m_cellOffset(0,0)
, m_cellOffsetTopLeft(s_cellOffsetTopLeft)
, m_cellOffsetRightBottom(s_cellOffsetRightBottom)
, m_ctrlBorderTopLeft(s_ctrlBorderTopLeft)
, m_ctrlBorderRightBottom(s_ctrlBorderRightBottom)
{
	m_Type = GUI_CONTROL_TABLE;
	m_ScrollBar.SetCanHaveFocus(false);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiTable::~CGuiTable(void)
{
	ClearAll();
	m_arrayCell.resize(0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiTable::ClearAll()
{
	for(size_t i=0;i < m_listItem.size();++i)
	{
		delete m_listItem[i];
	}
	for(size_t i=0;i < m_arrayCell.size();++i)
	{
		m_arrayCell[i] = 0;
	}
	m_listItem.resize(0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pt : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiTable::ContainsPoint( POINT pt )
{
	if( m_bVirtual )
	{
		stRectI rc = m_itemRect;
		//rc.OffsetRect(m_x,m_y);
		return rc.PtInRect(pt);
	}

	return m_rcBoundingBox.PtInRect(pt) || m_ScrollBar.ContainsPoint(pt);
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
bool CGuiTable::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	// Let the scroll bar have a chance to handle it first
	if( m_ScrollBar.HandleKeyboard( uMsg, wParam, lParam ) )
	{
		UpdateItemPos();
		return true;
	}

	return CGuiControl::HandleKeyboard(uMsg,wParam,lParam);
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
bool CGuiTable::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if(CGuiControl::HandleMouse(uMsg,pt,wParam,lParam))
		return true;

	if( (!m_bEnabled ) || !m_bVisible )
		return false;

	// First acquire focus
	if( WM_LBUTTONDOWN == uMsg )
		if( !m_bHasFocus )
			m_pDialog->RequestFocus( this );

	// Let the scroll bar handle it first.
	if( m_ScrollBar.IsVisible() && m_ScrollBar.HandleMouse( uMsg, pt, wParam, lParam ) )
	{
		UpdateItemPos();
		return true;
	}

	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		//case WM_LBUTTONDBLCLK:
		if( ContainsPoint( pt ) )
		{
			if( !m_bHasFocus )
				m_pDialog->RequestFocus( this );

			if(g_fnItemTableEvent)
				return g_fnItemTableEvent(this,uMsg,pt,wParam,lParam);
			return false;
		}
		break;
	case WM_LBUTTONUP:
		{
			if( ContainsPoint( pt ) )
			{
				if(g_fnItemTableEvent)
					return g_fnItemTableEvent(this,uMsg,pt,wParam,lParam);
			}
		}
		break;
	case WM_RBUTTONDOWN:
		{
			if( ContainsPoint( pt ) )
			{
				if(g_fnItemTableEvent)
					return g_fnItemTableEvent(this,uMsg,pt,wParam,lParam);
			}
		}
		break;
	case WM_MOUSEMOVE:
		{
			if( ContainsPoint( pt ) )
			{
				if(g_fnItemTableEvent)
					return g_fnItemTableEvent(this,uMsg,pt,wParam,lParam);
			}
			else
			{
				m_toolTips.Clear();
			}
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
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiTable::UpdateItemPos()
{
	int offsetY = getMin(m_ScrollBar.GetTrackPos(),m_rowCount-m_showRowCount);

	//if(GetStyle() & TABLE_NOSKIN) m_cellOffset = stPointI(0,0);

	for(size_t i=0;i<m_listItem.size();++i)
	{
		CGuiItemCell* pItem = m_listItem[i];
		stPointI pos = pItem->GetPos();
		if(!m_bVirtual)
		{
			pItem->SetLocation(
				GetX() + m_cellOffsetTopLeft.x + pos.x * m_cellWidth,
				GetY() + m_cellOffsetTopLeft.y + (pos.y-offsetY) * m_cellHeight);
		}
		else
		{
			pItem->SetLocation(
				GetX() + m_cellOffset.x ,
				GetY() + m_cellOffset.y );
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiTable::UpdateRects()
{
	if( m_bVirtual )
	{
		CGuiControl::UpdateRects();
		return;
	}

	if( m_cellWidth == -1 || m_cellHeight == -1 )
	{
		m_cellWidth = s_ctrlCellSize.cx;
		m_cellHeight = s_ctrlCellSize.cy;
	}

	stRectI rect(
		m_x,
		m_y,
		m_x + m_cellWidth * m_colCount + m_cellOffsetTopLeft.x + m_cellOffsetRightBottom.x,
		m_y + m_cellHeight * m_showRowCount + m_cellOffsetTopLeft.y + m_cellOffsetRightBottom.y
		);

	if( m_rowCount <= m_showRowCount && 0 == (GetStyle() & TABLE_NOAUTOHIDESCROLLBAR))
	{
		m_ScrollBar.SetVisible(false);
		m_width = rect.Width();
	}
	else
	{
		m_width = rect.Width() + m_pDialog->m_nSBWidth;
		m_ScrollBar.SetVisible(true);
	}
	m_height = rect.Height();

	CGuiControl::UpdateRects();

	CuttRect9(rect,m_ctrlBorderTopLeft,m_ctrlBorderRightBottom,m_rcRender);


	m_ScrollBar.SetLocation(GetX() + m_width - m_pDialog->m_nSBWidth,GetY());
	m_ScrollBar.SetSize(m_pDialog->m_nSBWidth,GetHeight());
	m_ScrollBar.SetPageSize(m_rowCount/m_showRowCount);
	m_ScrollBar.SetTrackRange(0,m_rowCount);
	m_ScrollBar.SetTrackPos(0);

	UpdateItemPos();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fElapsedTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiTable::Render( float fElapsedTime )
{

	if(!m_bVirtual && ((GetStyle() & TABLE_NOSKIN) == 0) )
	{
		for(int i=0;i<9;++i)
		{
			CGuiElement* pElement = m_Elements[i];
			m_pDialog->PatchDrawSprite(pElement,&m_rcRender[i],s_PatchAlignFormat[i]);
		}
	}
	
	g_fnTableRender(this,fElapsedTime);

	if(!m_bVirtual)
	{
		stRectI rcClip(	
			m_pDialog->GetClientX() + GetX() + m_cellOffsetTopLeft.x,
			m_pDialog->GetClientY() + GetY() + m_cellOffsetTopLeft.y,
			m_pDialog->GetClientX() + GetX() + m_width - m_cellOffsetRightBottom.x ,
			m_pDialog->GetClientY() + GetY() + m_height - m_cellOffsetRightBottom.y);

		if(m_pDialog->IsCaption()) rcClip.OffsetRect(0,m_pDialog->GetCaptionHeight());

		stRectI rcOldClip = GetDevice()->SetClipRect(&rcClip);

#ifdef _DEBUG
		//GetDevice()->FillRect(&rcOldClip,COLOR_ARGB(128,0,255,0));
#endif

		for(size_t i=0;i<m_listItem.size();++i)
		{
			CGuiItemCell* pItem = m_listItem[i];
			if( pItem->IsVisible() )
				g_fnItemRender(this,pItem,fElapsedTime);
			//pItem->Render(fElapsedTime);
		}
		GetDevice()->SetClipRect(&rcOldClip);

		m_ScrollBar.Render(fElapsedTime);
	}
	else
	{
#ifdef _DEBUG
		stRectI rcItem = m_itemRect;
		//rcItem.OffsetRect(m_x,m_y);
		m_pDialog->DrawRect(&rcItem,COLOR_ARGB(128,0,255,0));
#endif
		for(size_t i=0;i<m_listItem.size();++i)
		{
			CGuiItemCell* pItem = m_listItem[i];
			//pItem->Render(fElapsedTime);
			if( pItem->IsVisible() )
				g_fnItemRender(this,pItem,fElapsedTime);
		}
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pSrcRect : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiTable::CanPushA(const RECT* pSrcRect)
{
	stRectI rcItem = *pSrcRect;
	stRectI rcTable(0,0,m_colCount,m_rowCount);

	if(m_bVirtual)
	{
		if(rcItem.IsRectEmpty()) return false;
	}
	else
	{
		rcItem &= rcTable;
		if(rcItem != *pSrcRect) return false;
	}

	for(int i=rcItem.top;i < rcItem.bottom;++i)
	{
		for(int j=rcItem.left;j < rcItem.right;++j)
		{
			if(GetItemCell(j,i)) return false;
		}
	}
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pRect : ÃèÊö
 * \param pBuffer : ÃèÊö
 * \param iBufferCount : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
// »ñµÃÕâ¸öÇøÓòµÄÎïÆ·,·µ»ØÊıÁ¿
int CGuiTable::GetRectItem(const RECT* pRect, CGuiItemCell** pBuffer,int iBufferCount)
{

	stRectI rcItem = *pRect;
	if(!m_bVirtual)
	{
		stRectI rcTable(0,0,m_colCount,m_rowCount);
		rcItem &= rcTable;
	}
	else
	{
		rcItem.right = rcItem.left + 1;
		rcItem.bottom = rcItem.top + 1;
	}

	CGuiItemCell* ppCell[16];
	int iCount = 0;
	for(int i=rcItem.top;i < rcItem.bottom;++i)
	{
		for(int j=rcItem.left;j < rcItem.right;++j)
		{
			CGuiItemCell* pCell;
			if(pCell = GetItemCell(j,i))
			{
				for(int k=0;k<iCount;++k)
				{
					if(ppCell[k] == pCell)
						break;
				}
				if(k >= iCount)
				{		
					ppCell[iCount++] = pCell;
					Assert(iCount <= count_of(ppCell));
				}
			}
		}
	}

	memcpy(pBuffer,ppCell,getMin(iCount,iBufferCount) * sizeof(CGuiItemCell*));

	return iCount;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param rect : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiTable::CanPush(const RECT * rect)
{
	stRectI rcItem = *rect;
	stRectI rcTable(0,0,m_colCount,m_rowCount);

	stRectI rc0 = rcItem;

	if(m_bVirtual)
	{
	}
	else
	{
		rcItem &= rcTable;
		if(rcItem != rc0) return false;
	}

	// ²âÊÔÕâ¸öÇøÓòÊÇ·ñ×î¶àÖ»ÓĞÒ»¸öÎïÆ·
	CGuiItemCell* pCell;
	int iCount;
	iCount = GetRectItem(&rcItem,&pCell,1);

	if(iCount > 1) return false;

	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pItem : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiTable::PopItem(CGuiItemCell* pItem)
{
	stPointI pos = pItem->GetPos();
	SIZE	size = pItem->GetSize();
	stRectI rcItem(pos.x,pos.y,pos.x + size.cx,pos.y + size.cy);
	if(m_bVirtual)
	{
		rcItem.right = rcItem.left + 1;
		rcItem.bottom = rcItem.top + 1;
	}
	for(int i=rcItem.top;i < rcItem.bottom;++i)
	{
		for(int j=rcItem.left;j < rcItem.right;++j)
		{
			Assert(*GetCell(j,i) == pItem);
			*GetCell(j,i) = NULL;
		}
	}
	RemoveItemItFromList(GetItemIt(pItem));
	pItem->m_pTable = NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pItem : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiTable::PushItem(CGuiItemCell* pItem)
{
	stPointI pos = pItem->GetPos();
	SIZE	size = pItem->GetSize();
	m_arrayCell.size();
	stRectI rcItem(pos.x,pos.y,pos.x + size.cx,pos.y + size.cy);
	if(m_bVirtual)
	{
		rcItem.right = rcItem.left + 1;
		rcItem.bottom = rcItem.top + 1;
	}
	for(int i = rcItem.top;i < rcItem.bottom;++i)
	{
		for(int j=rcItem.left;j < rcItem.right;++j)
		{
			Assert(*GetCell(j,i) == NULL);
			*GetCell(j,i) = pItem;
		}
	}
	m_listItem.push_back(pItem);

	pItem->m_pDialog = m_pDialog;

	UpdateItemPos();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param cx : ÃèÊö
 * \param cy : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiTable::ResetSize(int cx,int cy)
{
	if(cx == m_colCount && cy == m_rowCount)
		return;

	if(cx < m_colCount || cy < m_rowCount)
		return;

	SetFrameAllocator fa;
	std::vector<CGuiItemCell* , std::stack_allocator<CGuiItemCell*> > tempCells;

	tempCells.resize( m_arrayCell.size() );
	memcpy( &tempCells[0] , &m_arrayCell[0] , sizeof(CGuiItemCell*) * m_arrayCell.size() );
	
	m_arrayCell.resize( 0 );
	m_arrayCell.resize( cx * cy );
	memset( &m_arrayCell[0] , 0 , sizeof( CGuiItemCell* ) * m_arrayCell.size() );

	for( size_t i = 0 ; i < m_rowCount ; ++i )
	{
		memcpy( &m_arrayCell[i * cx] , &tempCells[i * m_colCount] , sizeof( CGuiItemCell* ) * m_colCount );
	}

	m_colCount = cx;
	m_rowCount = cy;

	UpdateRects();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param cx : ÃèÊö
 * \param cy : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
POINT CGuiTable::GetNewPos(int cx,int cy)
{
	for( size_t i = 0 ; i < m_rowCount ; i++ )
	{
		for( size_t j = 0 ; j < m_colCount ; ++j )
		{
			stRectI rcItem(j,i,j+cx,i+cy);
			if(!GetItemCell(j,i) && CanPushA(&rcItem))
			{
				return stPointI(j,i);
			}
		}
	}
	return stPointI(-1,-1);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
SIZE CGuiTable::GetUseItemSize()
{
	SIZE size = {0,0};
	for(t_ListItem::iterator it = m_listItem.begin(); 
		it != m_listItem.end(); ++it)
	{
		int r,b;
		stPointI pt = (*it)->GetPos();
		SIZE sz = (*it)->GetSize();
		r = pt.x + sz.cx;
		b = pt.y + sz.cy;
		if(r > size.cx) size.cx = r;
		if(b > size.cy) size.cy = b;
	}
	return size;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param cx : ÃèÊö
 * \param cy : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
POINT CGuiTable::AppendItemPosition(int cx,int cy)
{
	stPointI pt  = GetNewPos(cx,cy);
	if(pt.x == -1 || pt.y == -1)
	{
		SIZE size = GetUseItemSize();
		ResetSize(m_colCount,size.cy + cy);
		pt.x = 0;
		pt.y = size.cy;
	}
	return pt;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param cx : ÃèÊö
 * \param cy : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
// ¼ÓÈëÒ»¸öÎïÆ·£¬×Ô¶¯¼ÆËãÆäÎ»ÖÃ
CGuiItemCell* CGuiTable::AppendItem(int cx,int cy)
{
	stPointI pt  = GetNewPos(cx,cy);
	if(pt.x == -1 || pt.y == -1)
	{
		SIZE size = GetUseItemSize();
		ResetSize(size.cx,size.cy + cy);
		pt.x = 0;
		pt.y = size.cy;
	}
	CGuiItemCell* pCell = new CGuiItemCell;
	pCell->SetPos(pt.x,pt.y);
	pCell->SetSize(cx,cy);
	AddItem(pCell);
	return pCell;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pItem : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiTable::AddItem(CGuiItemCell* pItem)
{
	Assert( GetItemIt(pItem) == m_listItem.end());
	stPointI pos = pItem->GetPos();
	SIZE	size = pItem->GetSize();
	
	stRectI rcItem(pos.x,pos.y,pos.x + size.cx,pos.y + size.cy);
	stRectI rcTable(0,0,m_colCount,m_rowCount);
	stRectI rc0;
	
	if(m_bVirtual)
		rc0 = rcItem;
	else 
	{
		rc0 = rcItem & rcTable;
		Assert(rc0 == rcItem);
		if(rc0 != rcItem)
		{
			Assert(0);
			//return false;
		}
	}

	if(!CanPushA(&rcItem))
	//if(!CanPush(pos,pItem))
	{
		//Assert(0);
		//return false;
	}
	
	PushItem(pItem);
	pItem->m_pTable = this;
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param col : ÃèÊö
 * \param row : ÃèÊö
 * \param bDestroyItem : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiItemCell* CGuiTable::RemoveItem(int col,int row,bool bDestroyItem)
{
	CGuiItemCell* pItem = GetItemCell(col,row);
	if(!pItem) return NULL;

	PopItem(pItem);
	
	if(bDestroyItem) 
	{
		delete pItem;
		//pItem->m_pDialog->RemoveControl(pItem);
		return NULL;
	}

	return pItem;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param rect : ÃèÊö
 * \param pBufferRemoved : ÃèÊö
 * \param iBufferCount : ÃèÊö
 * \param bDestroyItem : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CGuiTable::RemoveItem(const RECT* rect,CGuiItemCell** pBufferRemoved,int iBufferCount,bool bDestroyItem)
{
	CGuiItemCell* ppCell[16];
	int iCount = GetRectItem(rect,ppCell,count_of(ppCell));
	for(int i=0;i<iCount;++i)
	{
		PopItem(ppCell[i]);
		if(bDestroyItem)
		{
			delete ppCell[i];
		}
	}

	if(!bDestroyItem)
		memcpy(pBufferRemoved,ppCell,getMin(iBufferCount,iCount) * sizeof(CGuiItemCell*));

	return iCount;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pt : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
POINT CGuiTable::GetGridPosByPixelPos(POINT pt)
{
	if(m_bVirtual)
	{
		if( (size_t)pt.x < (size_t)m_width && (size_t)pt.y < (size_t)m_height )
			return m_EquipPosition;
		return stPointI(-1,-1);
	}
	pt.x -= m_cellOffsetTopLeft.x;
	pt.y -= m_cellOffsetTopLeft.y;
	pt.x /= m_cellWidth;
	pt.y /= m_cellHeight;
	int offsetY = getMin(m_ScrollBar.GetTrackPos(),m_rowCount-m_showRowCount);
	pt.y += offsetY;
	return pt;
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param col : ÃèÊö
 * \param row : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiItemCell** CGuiTable::GetCell(int col,int row)
{
	if(m_bVirtual) 
	{
		col = 0;
		row = 0;
	}
	return &m_arrayCell[row * m_colCount + col];
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param col : ÃèÊö
 * \param row : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiItemCell* CGuiTable::GetItemCell(int col,int row)
{ 
	if(m_bVirtual) 
	{
		col = 0;
		row = 0;
	}
	size_t sz = m_arrayCell.size();
	if(row * m_colCount + col >= sz) return NULL;
	return m_arrayCell[row * m_colCount + col]; 
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pt : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiTable::IsValidGrid(POINT pt)
{
	if(m_bVirtual) return true;
	return pt.x >= 0 && pt.x < m_colCount && pt.y >= 0 && pt.y < m_rowCount;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CGuiTable::OnInit() 
{
	m_toolTips.Clear();

	if(m_bVirtual) 
	{
		m_arrayCell.resize(1);
		return S_OK;
	}
	if(GetStyle() & TABLE_NOSKIN)
	{
		m_cellOffsetTopLeft = stPointI(0,0);
		m_cellOffsetRightBottom = stPointI(0,0);
		m_ctrlBorderTopLeft = stPointI(0,0);
		m_ctrlBorderRightBottom = stPointI(0,0);
	}
	m_arrayCell.resize(m_colCount*m_rowCount,NULL); 
	return m_pDialog->InitControl( &m_ScrollBar ); 

}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pCell : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
RECT CGuiTable::GetCellPixelRect(CGuiItemCell* pCell)
{
	if(m_bVirtual)
	{
		stRectI rc = m_itemRect;

		rc.OffsetRect(m_pDialog->GetLocation());
		return rc;
	}
	SIZE size = pCell->GetSize();
	stPointI pt = pCell->GetLocation();
	stRectI rcResult(pt.x,pt.y,pt.x + size.cx * m_cellWidth ,pt.y + size.cy * m_cellHeight );
	stRectI rcClip(	
		GetX() + m_cellOffsetTopLeft.x,
		GetY() + m_cellOffsetTopLeft.y,
		GetX() + m_width - m_cellOffsetRightBottom.x ,
		GetY() + m_height - m_cellOffsetRightBottom.y);

	rcResult &= rcClip;
	rcResult .OffsetRect(m_pDialog->GetClientX(),m_pDialog->GetClientY());
	
	return rcResult;
}

void CGuiTable::OnMouseLeave()
{ 
	CGuiControl::OnMouseLeave();	
	m_toolTips.Clear();
}

/*
* °ü¹ûÊÇ·ñÂúÁË 
*/
bool CGuiTable::IsFull()
{
	return m_listItem.size() >= (m_rowCount * m_colCount);
}