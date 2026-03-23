#ifndef _ENGINE_GRAPHIC_H
#define _ENGINE_GRAPHIC_H

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param totalRect : 描述
* \param leftTopBorder : 描述
* \param rightBottomBorder : 描述
* \param rects : 描述
* \return 返回值的描述
*/
//把一个区域切成9块，第0块为中心块
// 1 2 3
// 4 0 5
// 6 7 8
inline void CuttRect9(const RECT & totalRect,POINT leftTopBorder,POINT rightBottomBorder,RECT *rects)
{
	stRectI * pRect = (stRectI*)rects;
	pRect[0] = totalRect;

	pRect[0].left += leftTopBorder.x;
	pRect[0].top += leftTopBorder.y;
	pRect[0].right -= rightBottomBorder.x;
	pRect[0].bottom -= rightBottomBorder.y;

	pRect[1].SetRect(totalRect.left,							totalRect.top,								totalRect.left + leftTopBorder.x,				totalRect.top + leftTopBorder.y);
	pRect[2].SetRect(totalRect.left + leftTopBorder.x,			totalRect.top,								totalRect.right - rightBottomBorder.x,			totalRect.top + leftTopBorder.y);
	pRect[3].SetRect(totalRect.right - rightBottomBorder.x,		totalRect.top,								totalRect.right,								totalRect.top + leftTopBorder.y);

	pRect[4].SetRect(totalRect.left,							totalRect.top + leftTopBorder.y,			totalRect.left + leftTopBorder.x,				totalRect.bottom - rightBottomBorder.y);
	pRect[5].SetRect(totalRect.right - rightBottomBorder.x,		totalRect.top + leftTopBorder.y,			totalRect.right,								totalRect.bottom - rightBottomBorder.y);

	pRect[6].SetRect(totalRect.left,							totalRect.bottom - rightBottomBorder.y,		totalRect.left + leftTopBorder.x,				totalRect.bottom);
	pRect[7].SetRect(totalRect.left + leftTopBorder.x,			totalRect.bottom - rightBottomBorder.y,		totalRect.right - rightBottomBorder.x,			totalRect.bottom);
	pRect[8].SetRect(totalRect.right - rightBottomBorder.x,		totalRect.bottom - rightBottomBorder.y,		totalRect.right,								totalRect.bottom);
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param totalRect : 描述
* \param leftTopBorder : 描述
* \param rightBottomBorder : 描述
* \param nBorder2 : 描述
* \param rects : 描述
* \return 返回值的描述
*/
inline void CuttRect9_2(const RECT & totalRect,POINT leftTopBorder,POINT rightBottomBorder,int nBorder2,RECT *rects)
{
	stRectI * pRect = (stRectI*)rects;
	pRect[0] = totalRect;

	pRect[0].left += leftTopBorder.x - nBorder2;
	pRect[0].top += leftTopBorder.y - nBorder2;
	pRect[0].right -= rightBottomBorder.x + nBorder2;
	pRect[0].bottom -= rightBottomBorder.y + nBorder2;

	pRect[1].SetRect(totalRect.left,							totalRect.top,								totalRect.left + leftTopBorder.x,				totalRect.top + leftTopBorder.y);
	pRect[2].SetRect(totalRect.left + leftTopBorder.x,			totalRect.top,								totalRect.right - rightBottomBorder.x,			totalRect.top + leftTopBorder.y);
	pRect[3].SetRect(totalRect.right - rightBottomBorder.x,		totalRect.top,								totalRect.right,								totalRect.top + leftTopBorder.y);

	pRect[4].SetRect(totalRect.left,							totalRect.top + leftTopBorder.y,			totalRect.left + leftTopBorder.x,				totalRect.bottom - rightBottomBorder.y);
	pRect[5].SetRect(totalRect.right - rightBottomBorder.x,		totalRect.top + leftTopBorder.y,			totalRect.right,								totalRect.bottom - rightBottomBorder.y);

	pRect[6].SetRect(totalRect.left,							totalRect.bottom - rightBottomBorder.y,		totalRect.left + leftTopBorder.x,				totalRect.bottom);
	pRect[7].SetRect(totalRect.left + leftTopBorder.x,			totalRect.bottom - rightBottomBorder.y,		totalRect.right - rightBottomBorder.x,			totalRect.bottom);
	pRect[8].SetRect(totalRect.right - rightBottomBorder.x,		totalRect.bottom - rightBottomBorder.y,		totalRect.right,								totalRect.bottom);
}

/**
* \brief 简短描述
* 
* 把一个区域切成3块，第0块为中心块(1 0 2)
* 
* \param totalRect : 原先矩形
* \param leftTopBorder : 
* \param rightTopBorder : 描述
* \param rects : 描述
* \return 返回值的描述
*/
inline void CuttRect3(const RECT & totalRect,int Width,RECT *rects)
{
	stRectI * pRect = (stRectI*)rects;
	pRect[0] = totalRect;

	pRect[0].left += Width;
	pRect[0].right -= Width;

	pRect[1].SetRect(totalRect.left,totalRect.top,pRect[0].left,totalRect.bottom);
	pRect[2].SetRect(pRect[0].right,totalRect.top,totalRect.right,totalRect.bottom);
}

void DrawRectangle(const RECT & rc,int lineSize,DWORD color);
int GetPatchImageAlpha(POINT pt,const stRectI& rect,IBitmaps* pBmps,const stRectI * clip = NULL,DWORD dwAlignFormat = DT_TOP | DT_LEFT);
void PatchRenderImage(const stRectI & rect,IBitmaps* pBmps,const stRectI * clip = NULL,DWORD color = 0xffffffff,BlendType blend = Blend_Null,DWORD dwAlignFormat = DT_TOP | DT_LEFT);
void DrawImageRect(const RECT * prect,IBitmaps* pBmps,POINT ptOffsetLeftTop,POINT ptOffsetRightBottom,const RECT * clip = NULL,DWORD color = 0xffffffff,BlendType blend = Blend_Null);

#endif