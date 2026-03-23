/**
 * \file      GuiTable.h
 * \version   $Id$
 * \author    $$$03公司@###.com
 *           $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUI表格类
 * 
 *	      GUI表格类
 */

#pragma once
#include "guicontrol.h"
#include "GuiScrollBar.h"

struct  ShowNew{
	int nShowX;
	int nShowY;
	int nShowID;
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGuiTable :
	public CGuiControl
{
	friend bool ItemTableEvent(CGuiTable* pTable,UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam);
public:

	stPointI m_cellOffsetTopLeft;
	stPointI m_cellOffsetRightBottom;
	stPointI m_ctrlBorderTopLeft;
	stPointI m_ctrlBorderRightBottom;
	enum STYLE{
		TABLE_NOSKIN = 1,
		TABLE_NOAUTOHIDESCROLLBAR = 2,
	};
	static stPointI s_ctrlBorderTopLeft;
	static stPointI s_ctrlBorderRightBottom;
	static stRectI	s_ctrlBorderRect;
	static SIZE		s_ctrlCellSize;
	stRectI			m_rcRender[9];
	int m_iTableType;
	int m_iTableID;
	int m_AniTye;  //格子动画是否播放
	stPointI m_EquipPosition;
	stPointI m_cellOffset;
	stRectI m_itemRect;

	typedef std::vector<CGuiItemCell*> t_ListItem;
	typedef t_ListItem::iterator t_ItItem;

	std::vector<CGuiItemCell*>	m_listItem;		// 物品列表
	std::vector<CGuiItemCell*>	m_arrayCell;	// 格子二维数组

	int		m_rowCount;
	int		m_colCount;
	int		m_cellWidth;
	int		m_cellHeight;
	int		m_showRowCount;
	CGuiScrollBar m_ScrollBar;
	
	CToolTips	m_toolTips;
	DWORD		m_tipFlag;

	void	AddFlag(DWORD dwFlag)
	{
		m_tipFlag |= dwFlag;
	}

	bool	bSettedFlag(DWORD dwFlag)
	{
		return (0 != (m_tipFlag & dwFlag));
	}

	void	ClearFlag(DWORD dwFlag)
	{
		m_tipFlag ^= dwFlag;
	}
private:
	void PopItem(CGuiItemCell* pItem);
	void PushItem(CGuiItemCell* pItem);

	CGuiItemCell** GetCell(int col,int row);

	bool m_bPressed;
public:
	void OnMouseLeave();
	CGuiTable(CGuiDialog *pDialog = NULL);
	~CGuiTable(void);

	void Render( float fElapsedTime );
	HRESULT OnInit();
	void ResetItemRect(stRectI& rc){ m_itemRect = rc;}
	bool    CanHaveFocus() { return (m_bVisible && m_bEnabled); }
	bool    HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool    HandleMouse( UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam );
	virtual bool	ContainsPoint( POINT pt );
	void	UpdateRects();
	void	UpdateItemPos();
	CGuiItemCell* AppendItem(int cx,int cy);
	POINT GetNewPos(int cx,int cy);
	SIZE GetUseItemSize();

	t_ItItem GetItemIt(CGuiItemCell* pItem)
	{
		for(t_ItItem it = m_listItem.begin() ; it != m_listItem.end(); ++ it)
		{
			if((*it) == pItem)return it;
		}
		return m_listItem.end();
	}

	void RemoveItemItFromList(t_ItItem it)
	{
		Assert(it != m_listItem.end());
		(*it) = *(-- m_listItem.end());
		m_listItem.erase(--m_listItem.end());
	}

	bool CanPushA(const RECT* pSrcRect);
	bool CanPush(const RECT * rect);
	// 获得这个区域的物品,返回数量
	int GetRectItem(const RECT* pRect, CGuiItemCell** pBuffer,int iBufferCount);

	bool AddItem(CGuiItemCell* pItem);
	
	CGuiItemCell* RemoveItem(int col,int row,bool bDestroyItem = true);

	// 删除这一区域的物品，返回删除的数量
	int RemoveItem(const RECT* rect,CGuiItemCell** pBufferRemoved,int iBufferCount,bool bDestroyItem = true);

	void ClearAll();
	CGuiItemCell* GetItemCell(int col,int row);

	POINT GetGridPosByPixelPos(POINT pt);

	RECT GetCellPixelRect(CGuiItemCell* pCell);

	void ResetSize(int cx,int cy);
	bool IsValidGrid(POINT pt);
	POINT AppendItemPosition(int cx,int cy);
	bool IsFull();
	bool m_bVirtual;
};

extern bool (* g_fnItemTableEvent)(CGuiTable* pTable,UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam);
extern void (* g_fnItemRender)(CGuiTable* pTable,CGuiItemCell* pCell,float fElapsedTime);
extern void (* g_fnTableRender)(CGuiTable* pTable,float fElapsedTime);