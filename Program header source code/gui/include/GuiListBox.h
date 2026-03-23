/**
 * \file      GuiListBox.h
 * \version   $Id$
 * \author    $$$03公司@###.com
 *           $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUI列表框
 * 
 *	      GUI列表框
 */

#pragma once
#include "guicontrol.h"
#include "guiscrollbar.h"
#include <algorithm>
//-----------------------------------------------------------------------------
// ListBox control
//-----------------------------------------------------------------------------
enum enumGuiListBoxColFmt
{
	GuiListBoxColFmt_Left,
	GuiListBoxColFmt_Center,
	GuiListBoxColFmt_Right,
};

struct GuiListBoxCol
{
	tString<256> strText;
	DWORD color;
	int width;
	DWORD fromat;

	IBitmaps* pBmp;

	bool bEditable;
	GuiListBoxCol*	pNextCol;
	GuiListBoxCol()
	{
		pNextCol = NULL;
		bChecked = false;
		pBmp = NULL;
		bEditable = false;
	}
	~GuiListBoxCol()
	{
		pNextCol = NULL;
	}
	bool bChecked;
};

struct GuiListBoxItem
{
	std::vector< GuiListBoxCol > aCol;
	void*  pData;
	stRectI  rcActive;
	DWORD color;
	bool  bSelected;

	GuiListBoxItem()
	{
		bSelected = false;
		pData = NULL;
	}
	~GuiListBoxItem()
	{
		for ( int i = 0; i < aCol.size(); ++i )
		{
			if ( aCol[i].pNextCol )
			{
				delete aCol[i].pNextCol;
				aCol[i].pNextCol = NULL;
			}
		}
	}
};

bool Compare2String(const char* psz1,const char* psz2);

template<bool bUp,int col>
bool CompareGuiListBoxItem(GuiListBoxItem*& it1,GuiListBoxItem*& it2)
{
	Assert(it1->aCol.size() > col);
	Assert(it2->aCol.size() > col);
	
	return bUp
		? Compare2String(it1->aCol[col].strText, it2->aCol[col].strText)
		: Compare2String(it2->aCol[col].strText, it1->aCol[col].strText);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGuiListBox :
	public CGuiControl
{
public:
	enum STYLE{
		MULTISELECTION = 1,
		NOSCROLLBAR = 2,
		MOUSEMOVEEVENT = 4,
		NOSKIN	= 0x8,
		USECLIP = 0x10,
	};
	CGuiListBox( CGuiDialog *pDialog = NULL );
	virtual ~CGuiListBox();

	virtual HRESULT OnInit();
	virtual bool    CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	virtual bool    HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool    HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );

	virtual void Render( float fElapsedTime );
	virtual void    UpdateRects();


	int GetCount() const { return m_Items.size(); }
	void SetScrollBarWidth( int nWidth ) { m_nSBWidth = nWidth; UpdateRects(); }
	void SetBorder( int nBorder, int nMargin ) { m_nBorder = nBorder; m_nMargin = nMargin; }
	void AddItem( const char *szText, void *pData ,DWORD color = -1);
	HRESULT InsertItem( int nIndex, const char *szText, void *pData  ,DWORD color = -1);
	void RemoveItem( int nIndex );
	void RemoveItemByText( char *szText );
	void RemoveItemByData( void *pData );
	void RemoveAllItems();

	GuiListBoxItem *GetItem( int nIndex );

	GuiListBoxCol * GetCol(int item,int subItem);
	GuiListBoxCol * GetAndAddCol(int item,int subItem);

	bool RemoveSubItem(int nItem,int nSubItem)
	{
		GuiListBoxItem* pItem = GetItem(nItem);
		if(pItem  && pItem->aCol.size() > (size_t)nSubItem)
		{
			pItem->aCol.erase(pItem->aCol.begin() + nSubItem);
			return true;
		}
		return false;
	}

	const char* GetItemText(int nIndex,int iSubItem = 0)
	{
		GuiListBoxItem* pItem = GetItem(nIndex);
		if( pItem && (size_t)iSubItem < pItem->aCol.size() ) return pItem->aCol[iSubItem].strText;
		return NULL;
	}

	DWORD GetItemColor(int nIndex,int iSubItem = 0)
	{
		GuiListBoxItem* pItem = GetItem(nIndex);
		if( pItem && (size_t)iSubItem < pItem->aCol.size() ) return pItem->aCol[iSubItem].color;
		return 0;
	}

	void SetItemText( int nIndex,int iSubItem,const char* pszText,int width = -1,int format = GuiListBoxColFmt_Left)
	{
		GuiListBoxCol* pCol = GetAndAddCol(nIndex,iSubItem);
		pCol->strText = pszText;
		pCol->fromat = format;
		pCol->width = width;
		pCol->pBmp = NULL;
	}

	void SetItemText2( int nIndex,int iSubItem,const char* pszText,int width = -1,int format = GuiListBoxColFmt_Left)
	{
		GuiListBoxCol* pCol = GetAndAddCol(nIndex,iSubItem);
		pCol->strText = pszText;
		pCol->fromat = format;
		pCol->width = width / GetDevice()->GetFontWidth(GetFont());
		pCol->pBmp = NULL;
	}

	void SetItemEditable(int nIndex,int iSubItem,bool bEditable)
	{
		GuiListBoxCol* pCol = GetAndAddCol(nIndex,iSubItem);
		if( pCol )
		{			 
			pCol->bEditable  = bEditable;
		}
	}
	void SetItemTextEx(int nIndex,int iSubItem,const char* pszText,const char* pszText1 = NULL,int width = -1)
	{
		GuiListBoxCol* pCol = GetAndAddCol(nIndex,iSubItem);
		if( pCol )
		{
			pCol->strText = pszText;
			pCol->width = width;
			if ( NULL == pCol->pNextCol )
			{
				pCol->pNextCol = new GuiListBoxCol();
			}			
			pCol->pNextCol->strText = pszText1;
		}
	}

	void SetItemColor(int nIndex,int iSubItem,DWORD color,DWORD dwColor1 = -1)
	{
		GuiListBoxCol* pCol = GetAndAddCol(nIndex,iSubItem);
		if(pCol)
		{
			pCol->color = color;
			if( pCol->pNextCol )
			{
				pCol->pNextCol->color = dwColor1;
			}
		}
	}

	void SetItemBmp(int nIndex,int iSubItem,const stResourceLocation *prl)
	{
		GuiListBoxCol* pCol = GetAndAddCol(nIndex,iSubItem);
		pCol->pBmp = GetDevice()->FindBitmaps(prl);
	}


	void* GetItemData(int nIndex)
	{
		GuiListBoxItem* pItem = GetItem( nIndex );
		if( pItem ) return pItem->pData;
		return NULL;
	}

	int    GetItemCount() { return m_Items.size(); }
	int	   GetItemColCount(int nIndex){ if(GetItemCount() > nIndex) return m_Items[nIndex]->aCol.size(); return 0;}

	void SetCurItem(int index){ SelectItem(index,false);}
	int  GetCurItem(){ return GetSelectedIndex();}

	int GetSelectedIndex( int nPreviousSelected = -1 );
	GuiListBoxItem *GetSelectedItem( int nPreviousSelected = -1 ) { return GetItem( GetSelectedIndex( nPreviousSelected ) ); }
	void SelectItem( int nNewIndex ,bool bSendMsg = true);
	void RenderItem(GuiListBoxItem * pItem,RECT * prect);
	void SetSelectColor(DWORD color)
	{
		m_dwSelectColor = color;
	}
	int GetIndexInPoint(POINT pt);
	int GetFirstShowItem(){ return getMax(0,m_ScrollBar.GetTrackPos());}
	int GetLastShowItem();
	int GetMoveSel(){ return m_nMoveSel;}
	stRectI GetMoveSelRect(){ return m_rectMoveSel;}

	void SetTextHeight(int height) { m_nTextHeight = height;}

	int GetTextHeight(){return m_nTextHeight;}

	void OnFocusOut();

	void FinishEdit();

	bool OnLButtonClickOnItem();

	template<bool bUp,int col>
	void Sort()
	{
		std::sort( m_Items.begin(),m_Items.end(),CompareGuiListBoxItem<bUp,col> );
	}

	void FindTextBySpell(char spell,int col = 0);


	void SetOffset(int x,int y )
	{
		m_ptOffset.x = x;
		m_ptOffset.y = y;
	};

	stPointI GetOffset()
	{
		return m_ptOffset;
	}

	void SetListBoxClipRect(stRectI rcClip)
	{
		m_rcClip = rcClip;
	}
protected:
	stRectI m_rcText;      // Text rendering bound
	stRectI m_rcSelection; // Selection box bound
	CGuiScrollBar m_ScrollBar;
	DWORD m_dwSelectColor;
	DWORD m_dwMoveSelColor;

	int m_nSBWidth;
	int m_nBorder;
	int m_nMargin;
	int m_nTextHeight;  // Height of a single line of text
	int m_nSelected;    // Index of the selected item for single selection list box
	int m_nSelStart;    // Index of the item where selection starts (for handling multi-selection)
	bool m_bDrag;       // Whether the user is dragging the mouse to select
	bool m_bSBInit;
	int  m_nMoveSel;    //当前鼠标位置所选中的内容
	stRectI m_rectMoveSel;    //当前鼠标位置所选中的位置
	std::vector< GuiListBoxItem* > m_Items;

	int  m_nLastShowItem;

	void  SetMoveSelItem(POINT pt);

	CGuiEditBox *  m_pEditBox;
	GuiListBoxCol * m_pCurEditCol;

	stPointI m_ptOffset;
	stRectI  m_rcClip;
};
