/**
 * \file	Graphic.cpp
 * \version $Id$
 * \author  $$$03¹«Ë¾
 *          $$$03¹«Ë¾@###.com
 * \date    15:7:2005   9:14
 * \brief   
 *
 *
 *
 */

#include "../include/engine.h"

// 1 2 3
// 4 0 5
// 6 7 8

DWORD s_PatchAlignFormat[] = {
	DT_LEFT | DT_TOP , DT_LEFT | DT_TOP , DT_RIGHT | DT_TOP,
	DT_LEFT | DT_TOP , DT_LEFT | DT_TOP , DT_RIGHT | DT_TOP,
	DT_LEFT | DT_BOTTOM , DT_LEFT | DT_BOTTOM, DT_RIGHT | DT_BOTTOM,
};

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param rc : ÃèÊö
* \param lineSize : ÃèÊö
* \param color : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
void DrawRectangle(const RECT & rc,int lineSize,DWORD color)
{
	stRectI rc0;
	rc0.SetRect(rc.left,rc.top,rc.right,rc.top + lineSize);
	GetDevice()->FillRect(&rc0,color);

	rc0.SetRect(rc.left,rc.bottom - lineSize,rc.right,rc.bottom);
	GetDevice()->FillRect(&rc0,color);

	rc0.SetRect(rc.left,rc.top,rc.left + lineSize,rc.bottom);
	GetDevice()->FillRect(&rc0,color);

	rc0.SetRect(rc.right - lineSize,rc.top,rc.right,rc.bottom);
	GetDevice()->FillRect(&rc0,color);
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param pt : ÃèÊö
* \param rect : ÃèÊö
* \param pBmps : ÃèÊö
* \param NULL : ÃèÊö
* \param DT_LEFT : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
int GetPatchImageAlpha(POINT pt,const stRectI& rect,IBitmaps* pBmps,const stRectI * clip,DWORD dwAlignFormat)
{
	if(!pBmps) return 0;

	if(!rect.PtInRect(pt)) return 0;

	IBitmap* pBmp = pBmps->GetBitmapByTime(xtimeGetTime());

	stRectI rcClip;
	if(clip == NULL)
	{
		rcClip.SetRect(0,0,pBmp->GetWidth(),pBmp->GetHeight());
	}
	else
	{
		rcClip = * clip;
	}
	int clipHeight = rcClip.Height();
	int clipWidth = rcClip.Width();

	stRectI bmpClip;
	stRectI destRect;
	stPointI pt0;

	if( dwAlignFormat & DT_BOTTOM )
	{
		pt0.y = rcClip.Height() - (rect.bottom - pt.y)%clipHeight;
	}
	else
	{
		pt0.y = (pt.y-rect.top) % clipHeight;
	}

	if( dwAlignFormat & DT_RIGHT )
	{
		pt0.x = rcClip.Width() - (rect.right - pt.x)%clipWidth;
	}
	else
	{
		pt0.x = (pt.x - rect.left) % clipWidth;
	}

	pt0.x += rcClip.left;
	pt0.y += rcClip.top;

	return pBmp->GetAlpha(pt0);
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \param rect : ÃèÊö
* \param pBmps : ÃèÊö
* \param clip : ÃèÊö
* \param color : ÃèÊö
* \param blend : ÃèÊö
* \param dwAlignFormat : ÃèÊö
* \return ·µ»ØÖµµÄÃèÊö
*/
void PatchRenderImage(const stRectI & rect,IBitmaps* pBmps,const stRectI * clip ,DWORD color ,BlendType blend ,DWORD dwAlignFormat)
{
	if(!pBmps) return;
	IBitmap* pBmp = pBmps->GetBitmapByTime(xtimeGetTime());

	if(pBmp == 0) return;

	stRectI rcClip;
	if(clip == NULL)
	{
		rcClip.SetRect(0,0,pBmp->GetWidth(),pBmp->GetHeight());
	}
	else
	{
		rcClip = * clip;
	}
	int clipHeight = rcClip.Height();
	int clipWidth = rcClip.Width();
	Assert(clipHeight >= 0 && clipWidth >= 0);

	if(dwAlignFormat & DT_BOTTOM)
	{
		if(dwAlignFormat & DT_RIGHT)
		{
			for(int y = rect.bottom; y > rect.top; y -= clipHeight)
			{
				for(int x = rect.right; x > rect.left; x -= clipWidth)
				{
					stRectI destRect(x - clipWidth,y - clipHeight,x,y);
					destRect &= rect;

					stRectI bmpClip(rcClip.right - destRect.Width(),rcClip.bottom - destRect.Height(),rcClip.right,rcClip.bottom);

					pBmp->Render(destRect.left,destRect.top,&bmpClip,NULL,color,blend);
				}
			}
		}
		else
		{
			for(int y = rect.bottom; y > rect.top; y -= clipHeight)
			{
				for(int x = rect.left; x < rect.right; x += clipWidth)
				{
					stRectI destRect(x,y - clipHeight,x + clipWidth,y);
					destRect &= rect;

					stRectI bmpClip(rcClip.left,rcClip.bottom - destRect.Height(),rcClip.left + destRect.Width(),rcClip.bottom);
					pBmp->Render(destRect.left,destRect.top,&bmpClip,NULL,color,blend);
				}
			}
		}
	}
	else
	{
		if(dwAlignFormat & DT_RIGHT)
		{
			for(int y = rect.top; y < rect.bottom; y += clipHeight)
			{
				for(int x = rect.right; x > rect.left; x -= clipWidth)
				{
					stRectI destRect(x - clipWidth,y,x,y + clipHeight);
					destRect &= rect;
					stRectI bmpClip(rcClip.right - destRect.Width(),rcClip.top,rcClip.right,rcClip.top + destRect.Height());
					pBmp->Render(destRect.left,destRect.top,&bmpClip,NULL,color,blend);
				}
			}
		}
		else
		{
			for(int y = rect.top; y < rect.bottom; y += clipHeight)
			{
				for(int x = rect.left; x < rect.right; x += clipWidth)
				{
					stRectI destRect(x,y,x + clipWidth,y + clipHeight);
					destRect &= rect;
					stRectI bmpClip(rcClip.left,rcClip.top,rcClip.left + destRect.Width(),rcClip.top + destRect.Height());
					pBmp->Render(destRect.left,destRect.top,&bmpClip,NULL,color,blend);
				}
			}
		}
	}
}

/**
 * \brief  ÓÃÒ»¸ö²»¹æÔòÍ¼Æ¬Ìî³ä¾ØÐÎÇøÓò
 *
 *
 *
 * \param ? : ?
 *
 * \return  ?
 *
 */
void DrawImageRect(const RECT * prect,IBitmaps* pBmps,POINT ptOffsetLeftTop,POINT ptOffsetRightBottom,const RECT * clip ,DWORD color ,BlendType blend)
{
	if(pBmps->GetBitmapCount() == 0) return;

	stRectI rcRender[9];
	CuttRect9(*prect,ptOffsetLeftTop,ptOffsetRightBottom,rcRender);
	stRectI rcClip[9];
	stRectI rcClip0;
	
	if(clip == NULL)
	{
		rcClip0.SetRect(0,0,pBmps->GetBitmapByIndex(0)->GetWidth(),pBmps->GetBitmapByIndex(0)->GetHeight());
	}
	else
	{
		rcClip0 = *clip;
	}

	CuttRect9(rcClip0,ptOffsetLeftTop,ptOffsetRightBottom,rcClip);

	for(size_t i=0;i<9;++i)
	{
		PatchRenderImage(rcRender[i],pBmps,&rcClip[i],color,blend,s_PatchAlignFormat[i]);
	}
}