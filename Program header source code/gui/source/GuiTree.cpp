/**
* \file      GuiTree.cpp
* \version   $Id$
* \author    $$$03¹«Ë¾@###.com
*           $$$03¹«Ë¾@###.com
* \date      2005-07-05 17:02:57
* \brief     Ê÷¿Ø¼þ
* 
*	      Ê÷¿Ø¼þ
*/

#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"
#include "../../xml_parse/XmlParseEx.h"

const int LeftOffset = 25;
const int TopOffset  = 25;
const int FontWidth  = 10;
const int ItemHeight = 20;
const int ChildItemOffset = 20;
const int s_nSBSpace = 8;
const int s_nSBTopOffset = 13;
const int s_nSBBottomOffset = 8;

//--------------------------------------------------------------------------------------
// CGuiTree class
//--------------------------------------------------------------------------------------

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
//--------------------------------------------------------------------------------------
CGuiTree::CGuiTree( CGuiDialog *pDialog ) :
m_ScrollBar( pDialog )
, m_bSBInit(false)
, m_dwSelectColor(COLOR_ARGB(255,0,0,255))
, m_dwMoveSelColor(COLOR_ARGB(128,0,0,0))
{
	m_Type = GUI_CONTROL_LISTBOX;
	m_pDialog = pDialog;
	m_dwStyle = 0;
	m_bDrag = false;
	m_nBorder = 6;
	m_nMargin = 5;
	m_nTextHeight = 0;
	m_Items.empty();
	m_nSBWidth = pDialog->m_nSBWidth;

	Root= new TreeItem;
	Root->strText ="treeroot";
	Root->brother=NULL;
	Root->child =NULL;
	Root->color=COLOR_ARGB(255,255,255,255);
	Root->IsExpland =true;


	//pIterItem = new TreeItem;
	pIterItem = Root;

	m_nSelected = -1;
}


/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
//--------------------------------------------------------------------------------------
CGuiTree::~CGuiTree()
{
	RemoveAllItems();
	m_Items.empty();
	delete Root;
	//delete pIterItem;
}
//--------------------------------------------------------------------------------------
void CGuiTree::UpdateRects()
{
	CGuiControl::UpdateRects();

	stGuiCtrlBorderInfo* pBorderInfo = m_pDialog->GetCtrlBorderInfo(m_Type);

	m_rcSelection = m_rcBoundingBox;
	m_rcSelection.left += pBorderInfo->ptBorderLeftTop.x;
	m_rcSelection.top += pBorderInfo->ptBorderLeftTop.y;
	m_rcSelection.bottom -= pBorderInfo->ptBorderRightBottom.y;
	m_rcSelection.right -= pBorderInfo->ptBorderRightBottom.x;
	if( GetStyle() & NOSCROLLBAR )
	{
	}
	else
	{
		m_rcSelection.right -= m_nSBWidth + s_nSBSpace;
	}
	m_rcText = m_rcSelection;
	m_ScrollBar.SetLocation( m_rcBoundingBox.right - m_nSBWidth - s_nSBSpace, m_rcBoundingBox.top + s_nSBTopOffset);
	m_ScrollBar.SetSize( m_nSBWidth, m_height - (s_nSBTopOffset + s_nSBBottomOffset));


	m_ScrollBar.SetPageSize( m_height / ItemHeight );
	m_ScrollBar.ShowItem(  m_height / ItemHeight );


	if( GetStyle() & NOSCROLLBAR )
	{
		m_ScrollBar.SetVisible(false);
	}

}

void CGuiTree::RemoveItem(TreeItem * pItem)
{
	if(!pItem) return;
	if(pItem->child)
	{
		RemoveItem(pItem->child);
		pItem->child = NULL;
	}
	if(pItem->brother)
	{
		RemoveItem(pItem->brother);
		pItem->brother =  NULL;
	}
	delete pItem;
}
void CGuiTree::RemoveAllItems()
{
	m_Items.clear();
	TreeItem * pItem = Root->child;
	if(pItem)
	{ 
             if(pItem->child)
		     RemoveItem(pItem->child);
	     if(pItem->brother)
		     RemoveItem(pItem->brother);
	     delete pItem;
	     Root->child = NULL;
	}
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param strText : ÃèÊö
* \param color : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
void CGuiTree::AddItem(const char* strText,DWORD color)
{
	TreeItem* pNewItem = new TreeItem;
	if( !pNewItem )
		return ;
	pNewItem->strText =strText;
	pNewItem->brother=NULL;
	pNewItem->child =NULL;
	pNewItem->color=color;
	pIterItem->brother=pNewItem;
	pIterItem=pIterItem->brother;
}

void CGuiTree::AddChildItem(const char* strText,DWORD color)
{
	TreeItem* pNewItem = new TreeItem;
	if( !pNewItem )
		return ;
	pNewItem->IsExpland = true;
	pNewItem->strText =strText;
	pNewItem->brother=NULL;
	pNewItem->child =NULL;
	pNewItem->color=color;
	pIterItem->child=pNewItem;
	pIterItem=pIterItem->brother;
}

TreeItem * CGuiTree::AddChildItem(TreeItem * pParentItem,const char * strText,DWORD color)
{
	if(!pParentItem)
		pParentItem = Root;
	if(!pParentItem)
		return NULL;
	TreeItem* pNewItem = new TreeItem;
	if( !pNewItem )
		return NULL;
	pNewItem->IsExpland = true;
	pNewItem->strText =strText;
	pNewItem->brother=NULL;
	pNewItem->child =NULL;
	pNewItem->color=color;
	if(!pParentItem->child)
		pParentItem->child=pNewItem;
	else 	
	{
		TreeItem * pLastBrother = pParentItem->child;
		while(pLastBrother->brother)
		{
			pLastBrother =  pLastBrother->brother;
		};
		pLastBrother->brother = pNewItem;
	}

	return pNewItem;
}

TreeItem *  CGuiTree::AddSiblingItem(TreeItem * pSiblingItem,const char * strText,DWORD color)
{
	if(pSiblingItem)
		return NULL;

	TreeItem* pNewItem = new TreeItem;
	if( !pNewItem )
		return NULL;
	pNewItem->strText =strText;
	pNewItem->IsExpland = true;
	pNewItem->brother=pSiblingItem;
	pNewItem->child =NULL;
	pNewItem->color=color;

	return pNewItem;
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
bool CGuiTree::HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam )
{
	if(CGuiControl::HandleMouse(uMsg,pt,wParam,lParam))
		return true;

	if( !m_bEnabled || !m_bVisible )
		return false;

	// First acquire focus
	if( WM_LBUTTONDOWN == uMsg )
		if( !m_bHasFocus )
			m_pDialog->RequestFocus( this );

	if( m_ScrollBar.HandleMouse( uMsg, pt, wParam, lParam ) )
	{
		return true;
	}
	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:

		m_bDrag = true;
		// Check for clicks in the text area
		if( m_Items.size() > 0 && PtInRect( &m_rcBoundingBox, pt ) )
		{
			// Compute the index of the clicked item
			int index = (int)(pt.y-TopOffset-m_rcBoundingBox.top)/ItemHeight;
			if(index<m_Items.size())
			{
				if(m_Items[index]->IsExpland)
				{
					m_Items[index]->IsExpland=false;
				}
				else
				{
					m_Items[index]->IsExpland=true;
				}
				m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
			}  


			int nClicked;
			if( ItemHeight )
				nClicked = m_ScrollBar.GetTrackPos() + ( pt.y - m_rcSelection.top ) / ItemHeight;
			else
				nClicked = -1;

			// Only proceed if the click falls on top of an item.

			if( nClicked >= m_ScrollBar.GetTrackPos() &&
				nClicked < (int)m_Items.size() &&
				nClicked < m_ScrollBar.GetTrackPos() + m_ScrollBar.GetPageSize() )
			{
				//SetCapture( DXUTGetHWND() );
				m_bDrag = true;

				m_nSelected = nClicked;
				if( !( wParam & MK_SHIFT ) )
					m_nSelStart = m_nSelected;
				// If this is a double click, fire off an event and exit
				// since the first click would have taken care of the selection
				// updating.
				if( uMsg == WM_LBUTTONDBLCLK )
				{
					m_pDialog->SendEvent( EVENT_LISTBOX_ITEM_DBLCLK, true, this );
					return true;
				}

			}
			return true;
		}


		break;

	case WM_LBUTTONUP:

		m_bDrag = false;

		return false;

	case WM_MOUSEMOVE:
		if( m_bDrag )
		{
			m_pDialog->SendEvent( EVENT_LISTBOX_SELECTION, true, this );
		}
		else
		{
			SetMoveSelItem(pt);
		}
		break;


	}

	return false;
}

TreeItem * CGuiTree::GetCurTreeItem()
{
	if(m_nSelected>=0 && m_nSelected< m_Items.size())
		return m_Items[m_nSelected];
	return NULL;
}

//--------------------------------------------------------------------------------------
/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param pItem : ÃèÊö
* \param xpos : ÃèÊö
* \param ypos : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
enum nodetype
{
	add,
	sub,
};
void DrawNode(nodetype type,int x,int y)
{
	static IBitmaps * pAddBmps = NULL;
	static IBitmaps * pSubBmps = NULL;
	if( type == add )
	{
		if(!pAddBmps)
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces.gl"); 
			rl.group = 135;
			rl.frame = 11;
			pAddBmps = GetDevice()->FindBitmaps(&rl);
		}
		if(pAddBmps && pAddBmps->GetFrameCount())
		{	
			RECT rcClip  = pAddBmps->GetBitmap(0)->GetClipRect();
			x -= rcClip.left;
			y -= rcClip.top;
			pAddBmps->Render(0,x,y);
		}
	}
	else if( type == sub)
	{
		if(!pSubBmps)
		{
			stResourceLocation rl;
			rl.SetFileName("data\\interfaces.gl"); 
			rl.group = 135;
			rl.frame = 10;
			pSubBmps = GetDevice()->FindBitmaps(&rl);
		}
		if(pSubBmps && pSubBmps->GetFrameCount())
		{	
			RECT rcClip  = pSubBmps->GetBitmap(0)->GetClipRect();
			x -= rcClip.left;
			y -= rcClip.top;
			pSubBmps->Render(0,x,y);
		}
	}	
}

void CGuiTree::RenderItem(TreeItem * pItem,int xpos,int &ypos)
{
	if(!pItem)
		return;

	if(ypos<(m_rcBoundingBox.bottom-ItemHeight))
	{
		static CGuiElement s_Element;
		CGuiElement* pElement = &s_Element;//new CGuiElement ;
		stRectI rc ;
		rc.left      = xpos;
		rc.top       = ypos;
		rc.right     = rc.left+pItem->strText.length()*FontWidth + 2;
		rc.bottom    = rc.top+ItemHeight;

		int x = rc.left - 20 + m_pDialog->GetClientX();
		int y = rc.top + m_pDialog->GetClientY();
		if(pItem->child == NULL)
		{
			//DrawNode(add,x,y);
		}
		else 
		{
			if(!pItem->IsExpland)
				DrawNode(add,x,y);
			else
				DrawNode(sub,x,y);
		}


		//////////////////////////////////////
		pElement->FontColor.Current = pItem->color;

		pElement->dwTextFormat =0;

		if(m_Items.size() == m_nSelected)
		{
			if(rc.right >= m_rcSelection.right)
				rc.right = m_rcSelection.right;
			m_pDialog->DrawRect(&rc,m_dwSelectColor);
		}

		m_pDialog->DrawText( pItem->strText,pElement,&rc );

		m_Items.push_back(pItem);

		if(pItem->IsExpland)
		{
			if(pItem->child!=NULL)
			{

				ypos+=ItemHeight;
				RenderItem(pItem->child,xpos+ChildItemOffset,ypos);

			}
		}
		if(pItem->brother!=NULL)
		{
			ypos+=ItemHeight;

			RenderItem(pItem->brother,xpos,ypos);
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
void CGuiTree::Render( float fElapsedTime )
{

	CGuiControl::Render(fElapsedTime);

	stRectI rect = m_rcBoundingBox;

	int xpos,ypos;

	xpos=rect.left+LeftOffset;

	ypos=rect.top+TopOffset;

	if(!(m_dwStyle & NOSKIN))
	{
		stRectI rect = m_rcBoundingBox;
		m_pDialog->DrawControlBorder(rect,m_Type);	
	}

	m_Items.clear();
	RenderItem(Root->child,xpos,ypos);
	if( !m_bSBInit )
	{
		if( m_nTextHeight )
			m_ScrollBar.SetPageSize( m_rcText.Height() / m_nTextHeight );
		else
			m_ScrollBar.SetPageSize( m_rcText.Height() );
		m_bSBInit = true;
	}
	m_ScrollBar.SetTrackRange( 0, m_Items.size() );
	m_ScrollBar.Render( fElapsedTime );

	
}
//--------------------------------------------------------------------------------------
/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param pt : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
void CGuiTree::SetMoveSelItem(POINT pt)
{
	int nMoveSel=-1;
	m_pDialog->SendEvent( EVENT_LISTBOX_MOVESELECTION, true, this );

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
bool CGuiTree::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bEnabled || !m_bVisible )
		return false;

	// Let the scroll bar have a chance to handle it first
	if( m_ScrollBar.HandleKeyboard( uMsg, wParam, lParam ) )
	{
		return true;
	}

	return false;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param item : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
void CGuiTree::LoadItemsFromXML(TiXmlElement* item)
{
	while(item)
	{		
		TreeItem* pNewItem = new TreeItem;
		if( !pNewItem )
			return ;
		pNewItem->strText =item->Attribute("text");
		pNewItem->brother=NULL;
		pNewItem->child =NULL;
		pNewItem->color=COLOR_ARGB(255,255,255,255);
		pNewItem->IsExpland=false;

		pIterItem->brother = pNewItem;

		pIterItem = pIterItem->brother;	

		LoadTreeItemsFromXML(item->FirstChildElement("item"),pIterItem1,pIterItem);

		item = item->NextSiblingElement("item");

	}
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param item : ÃèÊö
* \param pIter : ÃèÊö
* \param pIterItem : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
void CGuiTree::LoadTreeItemsFromXML(TiXmlElement* item, TreeItem* pIter,TreeItem* pIterItem)
{

	if(item!=NULL)
	{								
		pIter = new TreeItem;
		int i =0;
		while(item)
		{
			TreeItem* pNewItem = new TreeItem;
			if( !pNewItem )
				return ;
			pNewItem->strText =item->Attribute("text");
			pNewItem->brother=NULL;
			pNewItem->child =NULL;
			pNewItem->color=COLOR_ARGB(255,255,255,255);
			pNewItem->IsExpland=false;


			pIter->brother = pNewItem;

			pIter = pIter->brother;	
			if(i==0)	
			{

				pIterItem->child= pIter;
			}
			LoadTreeItemsFromXML(item->FirstChildElement("item"),pIterItem1,pIter);
			item = item->NextSiblingElement("item");
			i++;

		}

	}       
}
