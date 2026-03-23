/**
 * \file      D3DBitmapList.cpp
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2005-07-05 12:29:19
 * \brief     D3D图形设备的图形包类扩展
 * 
 *	      D3D图形设备的图形包类扩展
 */

#include "../../engine/include/engine.h"
#include "../include/d3dbitmaplist.h"
#include "../include/TextureGroup.h"
#include "../include/TextureManager.h"
#include "../Include/RenderD3D.h"
 
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
//template <class CD3DBitmap >
CD3DBitmapList::CD3DBitmapList(LPCTSTR lpszFileName,DWORD flags,DWORD dwID,TextureUsageType usage)
: t_Parent(lpszFileName,flags,dwID)
, m_pTexGroup(NULL)
, m_usage(usage)
{
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CD3DBitmapList::~CD3DBitmapList(void)
{
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CD3DBitmapList::CreateTextureGroup()
{
	m_pTexGroup = GetTextureManager()->CreateTextureGroup(this);
	Assert(m_pTexGroup);
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CD3DBitmapList::ReleaseTexture()
{
	if(m_pTexGroup)
	{
		m_pTexGroup->DeleteBmpLists(this);
		m_pTexGroup = NULL;
	}
}
