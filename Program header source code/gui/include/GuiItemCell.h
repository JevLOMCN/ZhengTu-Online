/**
 * \file      GuiItemCell.h
 * \version   $Id$
 * \author    $$$03公司@###.com
 *           $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUI物品格子
 * 
 *	      GUI物品格子
 */

#pragma once
#include "guiimage.h"

class CGuiTable;

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGuiItemCell :
	public CGuiImage
{
	stPointI m_Pos;
	SIZE	 m_Size;
	friend void RoleItemRender(CGuiTable* pTable,CGuiItemCell* pCell,float fElapsedTime);
public:
	void*		m_pItemData;
	CGuiTable* m_pTable;
	CGuiItemCell(CGuiDialog * pDialog = NULL);
	~CGuiItemCell(void);

	stPointI GetPos() { return m_Pos;  }
	SIZE	 GetSize(){ return m_Size; }
	//Get cell item name
	
	void SetPos(int col,int row) { m_Pos.x = col;m_Pos.y = row;}
	void SetSize(int cx,int cy) { m_Size.cx = cx;m_Size.cy = cy;}
	void Render( float fElapsedTime );
	stRectI GetCellPixelRect();
};
