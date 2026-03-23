/**
 * \file      IBitmaps.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     图片（多帧）操作接口
 * 
 *	      图片（多帧）操作接口
 */

#pragma once

//class IBitmap;
#include "IBitmap.h"

#include <vector>

#include "./engine.h"

class IBitmapList;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class IBitmaps
{
protected:
	stPackAniHeader m_aniInfo;
	DWORD			m_dwDataOffset;
	DWORD			m_dwDataSize;
	WORD			m_dwGroup;
	WORD			m_dwFrame;

	IBitmapList *	m_pBmpList;
	DWORD*			m_pFrameIndex;
public:
	DWORD GetDataOffset() { return m_dwDataOffset;}
	DWORD GetDataSize() { return m_dwDataSize;}
	DWORD GetFrameDataSize(int iFrame) { 
		int index = GetFrameIndex(iFrame);
		DWORD dwOffset1 = GetBitmapByIndex(index)->GetDataOffset();
		DWORD dwOffset2 = (index + 1 < m_aniInfo.wdBitmapCount ? GetBitmapByIndex(index+1)->GetDataOffset() : GetDataOffset() + GetDataSize());
		return dwOffset2 - dwOffset1;
	}
	const char* GetPackName();

	IBitmapList* GetBitmapList(){ return m_pBmpList; }

	int GetGroup(){ return m_dwGroup; }

	int GetFrame(){ return m_dwFrame; }

	IBitmaps(void);
	~IBitmaps()
	{
		/*if(m_pFrameIndex)
		{
			delete [] m_pFrameIndex;
			m_pFrameIndex = NULL;
		}*/
	}

	const stPackAniHeader* GetAniInfo(){ return &m_aniInfo; }

	virtual IBitmap* GetBitmap(size_t frame) = 0;
	virtual IBitmap* GetBitmapByIndex(size_t index) = 0;
	size_t	 GetFrameCount() { return m_aniInfo.wdFrameCount; }
	size_t	 GetBitmapCount(){ return m_aniInfo.wdBitmapCount;}

	size_t GetFrameIndex(size_t frame)
	{
		if(frame < GetFrameCount())
		{
			if(m_pFrameIndex) return m_pFrameIndex[frame];
			return frame;
		}
		return -1;
	}
	size_t GetBitmapFrame(size_t index)
	{
		if(m_pFrameIndex)
		{
			for(size_t i=0; i < GetFrameCount(); ++i)
			{
				if(m_pFrameIndex[i] == index) return i;
			}
		}
		return index;
	}

	IBitmap* GetBitmapByTime(DWORD time)
	{
		if(GetFrameCount() == 0) return NULL;
		size_t frameFrame;
		if(GetFrameCount() == 1 ) frameFrame = 0;
		else frameFrame = (time / m_aniInfo.wdAniSpeed) % GetFrameCount();
		return GetBitmap(frameFrame);
	}

	bool Render(size_t frame,int x,int y,DWORD color = 0xffffffff,BlendType blend = Blend_Null)
	{
		return GetBitmap(frame)->Render(x,y,color,blend);
	}

	bool Render(size_t frame,int x,int y,const RECT* rClip,const stPointF * scale,DWORD color = 0xffffffff,BlendType blend = Blend_Null)
	{
		return GetBitmap(frame)->Render(x,y,rClip,scale,color,blend);
	}

	// 渲染动画
	bool RenderAni(int x , int y , DWORD dwTime ,DWORD dwColor = -1,BlendType blend = Blend_Null)
	{
		if(GetFrameCount() == 0) return false;
		size_t frameFrame;
		if(GetFrameCount() == 1 ) frameFrame = 0;
		else frameFrame = (dwTime / m_aniInfo.wdAniSpeed) % GetFrameCount();
		return GetBitmap(frameFrame)->Render(x,y,dwColor,blend);
	}

	// 渲染动画
	bool RenderAni(int x , int y , DWORD dwTime  , const RECT* rClip = NULL , const stPointF * scale = NULL , DWORD dwColor = -1 , BlendType blend = Blend_Null)
	{
		if(GetFrameCount() == 0) return false;
		size_t frameFrame;
		if(GetFrameCount() == 1 ) frameFrame = 0;
		else frameFrame = (dwTime / m_aniInfo.wdAniSpeed) % GetFrameCount();
		return GetBitmap(frameFrame)->Render(x,y,rClip,scale,dwColor,blend);
	}

	// 渲染地图物件用的
	bool RenderAni2(int x , int y , DWORD dwTime ,DWORD dwColor = -1,BlendType blend = Blend_Null)
	{
		if(GetFrameCount() == 0) return false;
		size_t frameFrame;
		if(GetFrameCount() == 1) frameFrame = 0;
		else frameFrame = (dwTime / m_aniInfo.wdAniSpeed) % GetFrameCount();

		IBitmap* pBmp = GetBitmap(frameFrame);
		y -= pBmp->GetHeight();

		return pBmp->Render(x,y,dwColor,blend);
	}

	// 渲染地图物件用的
	bool RenderAni2(int x , int y , DWORD dwTime  , const RECT* rClip = NULL , const stPointF * scale = NULL , DWORD dwColor = -1 , BlendType blend = Blend_Null)
	{
		if(GetFrameCount() == 0) return false;
		size_t frameFrame;
		if(GetFrameCount() == 1 ) frameFrame = 0;
		else frameFrame = (dwTime / m_aniInfo.wdAniSpeed) % GetFrameCount();

		IBitmap* pBmp = GetBitmap(frameFrame);
		y -= (int)(pBmp->GetHeight() * ( scale ? scale->y : 1));

		return pBmp->Render(x,y,rClip,scale,dwColor,blend);
	}


	static void RenderBlendAni(IBitmaps* pBmps,IBitmaps* pAlphas,int x , int y , DWORD dwTime , const RECT* rClip = NULL , const stPointF * scale = NULL , DWORD dwColor = -1 , BlendType blend = Blend_Null);
	static void RenderBlendAni(IBitmaps* pBmps,IBitmaps* pAlphas,int x , int y , DWORD dwTime ,DWORD dwColor = -1,BlendType blend = Blend_Null);
	void SetAniSpeed(DWORD dwTime) { m_aniInfo.wdAniSpeed = dwTime; }

	virtual void ReleaseData() = 0;
};


