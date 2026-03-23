/**
* \file      GuiTree.h
* \version   $Id$
* \author    $$$03公司@###.com
*           $$$03公司@###.com
* \date      2006-7-19  21:11:00
* \brief     树控件
* 
*	      树控件
*/

#pragma once
#include "guicontrol.h"
#include "guiscrollbar.h"
//-----------------------------------------------------------------------------
// Tree control
//-----------------------------------------------------------------------------
struct TreeItem
{
	tString<256> strText;
	DWORD color;
	DWORD fromat;
	bool IsExpland;
	TreeItem* brother;
	TreeItem* child;
	void * pItemData;
	TreeItem()
	{
		IsExpland = false;
		pItemData = NULL;
	}
};
/**
* \brief 简短描述
* 
* 详细描述
* 
*/
class CGuiTree :
	public CGuiControl
{
public:
	enum STYLE{
		MULTISELECTION = 1,
		NOSCROLLBAR = 2,
		MOUSEMOVEEVENT = 4,
		NOSKIN	= 0x8,
	};

	CGuiTree( CGuiDialog *pDialog = NULL );
	virtual ~CGuiTree();

	virtual HRESULT OnInit() { return m_pDialog->InitControl( &m_ScrollBar ); }
	virtual bool    CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	virtual bool    HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool    HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );

	virtual void    Render( float fElapsedTime );
	virtual void    UpdateRects();

	void RenderItem(TreeItem * pItem,int xpos,int &ypos);
	void AddItem(const char* strText,DWORD color);
	void AddChildItem(const char* strText,DWORD color);
	TreeItem * AddChildItem(TreeItem * pParentItem,const char * strText,DWORD clr);
	TreeItem * AddSiblingItem(TreeItem * pSiblingItem,const char * strText,DWORD clr);

	void LoadTreeItemsFromXML(TiXmlElement* item,TreeItem* pIter,TreeItem* pIterItem);
	void LoadItemsFromXML(TiXmlElement* item);

	TreeItem * GetCurTreeItem();

	void RemoveAllItems();
	

protected:
	stRectI		m_rcText;      // Text rendering bound
	stRectI		m_rcSelection; // Selection box bound
	CGuiScrollBar	m_ScrollBar;
	DWORD		m_dwSelectColor;
	DWORD		m_dwMoveSelColor;

	int		m_nSBWidth;
	int		m_nBorder;
	int		m_nMargin;
	int		m_nTextHeight;  // Height of a single line of text
	int		m_nSelected;    // Index of the selected item for single selection list box
	int		m_nSelStart;    // Index of the item where selection starts (for handling multi-selection)
	bool		m_bDrag;       // Whether the user is dragging the mouse to select
	bool		m_bSBInit;
	int		m_nMoveSel;    //当前鼠标位置所选中的内容

	std::vector< TreeItem* > m_Items;
	TreeItem*	Root;
	TreeItem*	pIterItem;
	TreeItem*	pIterItem1;
	int		m_nLastShowItem;

	void  SetMoveSelItem(POINT pt);
	void RemoveItem(TreeItem * pItem);
};
