/**
 * \file      IBitmapList.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     图形包接口
 * 
 *	      图形包接口
 */

#ifndef _IBITMAPLIST_H
#define _IBITMAPLIST_H

//class IBitmap;
#include "IBitmaps.h"

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class IBitmapList{
	friend class IBitmap;
protected:
	DWORD	m_dwGrpVersion;
public:
	DWORD GetGrpVersion(){return m_dwGrpVersion;}
	virtual IBitmap* LoadFrame(size_t group,size_t frame,size_t frameframe=0) = 0;

	enum{
		flag_MapView	= BIT(1),
		flag_MultiFile	= BIT(2),
		flag_AlwaysOpen = BIT(3),
		flag_LoadByGroup= BIT(4),
		flag_LoadByThread = BIT(5),
		flag_NoRelease	= BIT(6),
		flag_LoadHdr	= BIT(7),
	};

	virtual DWORD GetResourceID() = 0;
	virtual bool IsLoadByGroup() = 0;
	virtual bool IsFlag(DWORD flag) = 0;
	virtual bool IsOpen() = 0;
	
	virtual bool	IsExistGroup(int groupID) = 0;
	virtual size_t	 GetGroupCount() = 0;
	virtual size_t	 GetGroupID(size_t iGroup) = 0;
	virtual size_t	 GetFrameCount(size_t groupID) = 0;
	virtual size_t	 GetFrameFrameCount(size_t group,size_t frame) = 0;
	
	virtual IBitmaps* GetBitmaps(size_t group,size_t frame) = 0;
	virtual bool IsExist(size_t group,size_t frame) = 0;
	virtual const char* GetFileName(){ return NULL; }
	IBitmap* GetBitmap(size_t group,size_t frame,size_t frame0 = 0)
	{
		IBitmaps* p = GetBitmaps(group,frame);
		if(!p) return NULL;
		return p->GetBitmap(frame0);
	}
	virtual void ThreadLoadAImage(size_t group,size_t frame) = 0;
	virtual void ThreadLoadImage(DWORD* aImageID,size_t size) = 0;
	virtual void ThreadLoadImage(IBitmaps** pBmps,size_t size) = 0;
	virtual void ReleaseImage(DWORD* aImageID,size_t size) = 0;
	virtual void ReleaseAImage(size_t group,size_t frame) = 0;
	//virtual BYTE* GetFileDataAddress(DWORD dwGroupID,DWORD dwFileOffset) = 0;
	virtual BYTE* GetFileDataAddress(DWORD dwDataOffset,DWORD dwDataSize) = 0;
	virtual bool IsMapView() = 0;
	//virtual void MarkGroupTime(DWORD dwGroupID,DWORD dwTime) = 0;
	//virtual void DeleteLoadResource() = 0;
	//virtual void UnmapFileGroup(DWORD dwGroupID) = 0;
};

#endif