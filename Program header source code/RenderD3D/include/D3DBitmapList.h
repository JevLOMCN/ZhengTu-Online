/**
 * \file      D3DBitmapList.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2005-07-05 12:29:19
 * \brief     D3D图形设备的图形包类扩展
 * 
 *	      D3D图形设备的图形包类扩展
 */

#pragma once

#include "./D3DBitmap.h"
#include "../../engine/include/TBitmapList.h"

class CBmpTextureGroup;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CD3DBitmapList :
	public TBitmapList<CD3DBitmap>
{
	friend class CBmpTextureGroup;

	CBmpTextureGroup* m_pTexGroup;
	TextureUsageType m_usage;
public:
	typedef TBitmapList<CD3DBitmap> t_Parent;
	
	TextureUsageType GetUsage(){ return m_usage; }

	CD3DBitmapList(LPCTSTR lpszFileName,DWORD flags,DWORD dwID,TextureUsageType usage);
	~CD3DBitmapList(void);
	
	CBmpTextureGroup* GetTextureGroup() { return m_pTexGroup; }
	void CreateTextureGroup();
	void ReleaseTexture();
};
