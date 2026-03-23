/**
 * \file      GuiItemCell.cpp
 * \version   $Id$
 * \author    $$$03公司@###.com
 *           $$$03公司@###.com
 * \date      2005-07-05 17:02:57
 * \brief     GUI物品格子
 * 
 *	      GUI物品格子
 */

#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CGuiItemCell::CGuiItemCell(CGuiDialog * pDialog)
: CGuiImage(pDialog)
, m_pTable(NULL)
, m_pItemData(NULL)
{
	m_dwTextFormat = DT_BOTTOM | DT_RIGHT;
	m_eFontEffect = FontEffect_Border;
	m_dwStyle |= FULLSIZE_IMAGE;
	m_dwStyle &= (~STRETCH_IMAGE);
	m_Type = GUI_CONTROL_ITEMCELL;
	m_bEnabled = true;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CGuiItemCell::~CGuiItemCell(void)
{
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
stRectI CGuiItemCell::GetCellPixelRect()
{
	if(m_pTable == NULL)
		return stRectI(0,0,0,0);

	return m_pTable->GetCellPixelRect(this);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param fElapsedTime : 描述
 * \return 返回值的描述
 */
void CGuiItemCell::Render( float fElapsedTime )
{
	CGuiImage::Render(fElapsedTime);
}