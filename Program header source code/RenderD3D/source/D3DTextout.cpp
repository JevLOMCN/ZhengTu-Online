/**
 * \file      D3DTextout.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2005-07-05 12:29:19
 * \brief     D3DÉè±¸ÎÄ±¾Êä³ö
 * 
 *	      D3DÉè±¸ÎÄ±¾Êä³ö
 */

#include "../../engine/include/engine.h"
#include "../include/d3dtextout.h"
#include "../include/D3DDevice.h"
#include "../include/D3DFont.h"
#include "../Include/RenderD3D.h"

 

const int CD3DTextout::Font_Count = 3;

const stFontInfo c_fontInfo[CD3DTextout::Font_Count] = {
	{"ĞÂËÎÌå","","","data\\song12.fnt","data\\song12.fnt",6,12,400,1,true,false},
	//{"¾­µäÁ¥±ä¼ò","¾­µäÁ¥±ä·±","data\\li16.fnt","data\\li16b.fnt",8,16,400,4,true,false},
	{"¾­µäÁ¥±ä¼ò","Á¥Êé","","data\\li16.fnt","data\\li16.fnt",8,16,400,4,true,false},
	{"¾­µäÁ¥Êé¼ò","","¾­µäÁ¥±ä¼ò","data\\li28.fnt","data\\li28.fnt",14,28,400,4,false,true},
};

/*
const stFontInfo c_fontInfo[CD3DTextout::Font_Count] = {
	{"ĞÂËÎÌå","","","data\\song12.fnt","data\\song12.fnt",6,12,400,1,true,false},
	{"¾­µäÁ¥±ä¼ò","Á¥Êé","","data\\song12.fnt","data\\song12.fnt",8,16,400,4,true,false},
	{"¾­µäÁ¥Êé¼ò","","¾­µäÁ¥±ä¼ò","data\\song12.fnt","data\\song12.fnt",14,28,400,4,false,true},
};
*/
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CD3DTextout::CD3DTextout(void)
: m_scale(1,1)
, m_pPicCharInfo(NULL)
, m_nPicCharCount(0)
, m_nCurFont(-1)
{
	m_aFont.reserve(Font_Count);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CD3DTextout::~CD3DTextout(void)
{
	for(size_t i=0;i<m_aFont.size();++i)
	{
		delete m_aFont[i].pNormal;
		delete m_aFont[i].pBorder;
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CD3DTextout::Init()
{
	if(GetD3DDevice()->GetProgramType() != eProgram_ViewGL)
	{
		int iFontIndex = 0;
		m_aFont.resize(Font_Count);
		for(size_t i=0;i<Font_Count;++i)
		{ 
			const stFontInfo* pInfo = &c_fontInfo[i];
			m_aFont[i].pNormal = new CD3DFont(iFontIndex ++ ,pInfo->bits,false,pInfo->bLageLib);
			if(!m_aFont[i].pNormal->LoadBmpFont(pInfo->pszFontFile))
			{
				m_aFont[i].pNormal->CreateFont(pInfo->fontName,pInfo->fontNameFT,pInfo->fontNameE,pInfo->pszFontFile,pInfo->width,pInfo->height,pInfo->weight,false);
			}
			if(c_fontInfo[i].bNoBorderFont)
			{
				m_aFont[i].pBorder = NULL;
			}
			else
			{
				m_aFont[i].pBorder = new CD3DFont(iFontIndex ++ ,pInfo->bits,true,pInfo->bLageLib);
				m_aFont[i].pBorder->DuplicateFont(m_aFont[i].pNormal);
			}
		}
	}
	//m_aFont[1]->m_bRenderBorder = true;
	m_nTabSize = 4;
	SetFont(0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iFont : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
float CD3DTextout::GetFontWcharWidth(int iFont)
{
	if((size_t)iFont >= Font_Count) iFont = m_nCurFont;
	if((size_t)iFont >= m_aFont.size())
		return 0.0f;
	return m_aFont[iFont].pNormal->GetHSpaceWchar() * m_scale.x;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iFont : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
float CD3DTextout::GetFontWidth(int iFont)
{
	if((size_t)iFont >= Font_Count) iFont = m_nCurFont;
	if((size_t)iFont >= m_aFont.size())
		return 0.0f;
	return m_aFont[iFont].pNormal->GetHSpace() * m_scale.x;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iFont : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
float CD3DTextout::GetFontHeight(int iFont)
{
	if((size_t)iFont >= Font_Count) iFont = m_nCurFont;
	if((size_t)iFont >= m_aFont.size())
		return 0.0f;
	return m_aFont[iFont].pNormal->GetVSpace() * m_scale.y;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iFont : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CD3DTextout::SetFont(int iFont)
{
	if((size_t)iFont >= m_aFont.size())
		return -1;
	int iOldFont = m_nCurFont;
	if((size_t)iFont >= Font_Count)
	{
		Assert(0);
		return m_nCurFont;
	}
	m_nCurFont = iFont;
	return iOldFont;

}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param WCHAR* : ÃèÊö
 * \param count : ÃèÊö
 * \param RECT : ÃèÊö
 * \param clrText : ÃèÊö
 * \param dwTextFormat : ÃèÊö
 * \param fontEffect : ÃèÊö
 * \param clrShadow : ÃèÊö
 * \param pData : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CD3DTextout::DrawString(const WCHAR* lpszString , size_t count,const RECT &layoutRect ,DWORD clrText,DWORD dwTextFormat,enumFontEffect fontEffect ,DWORD clrShadow,DWORD* pData)
{
	/*if(count == -1) count = wcslen(lpszString);
	char *szChar = (char*)_alloca(count * 2 + 1);
	int len ;
	if(!(len = WideCharToMultiByte(CP_ACP,0,lpszString,count,szChar,count * 2 + 1,NULL,false)))
		return;
	szChar[len] = 0;*/
	if(count == -1) count = wcslen(lpszString);
	char * szChar = (char*) _alloca(count * 2 + 1);
	int len = my_wcstombs(szChar,lpszString,count);
	DrawString(szChar,len,layoutRect,clrText,dwTextFormat,fontEffect,clrShadow,pData);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param char* : ÃèÊö
 * \param count : ÃèÊö
 * \param RECT : ÃèÊö
 * \param clrText : ÃèÊö
 * \param dwTextFormat : ÃèÊö
 * \param fontEffect : ÃèÊö
 * \param clrShadow : ÃèÊö
 * \param pData : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CD3DTextout::DrawString(const char* lpszString , size_t count,const RECT & i_layoutRect,DWORD clrText ,DWORD dwTextFormat ,enumFontEffect fontEffect ,DWORD clrShadow ,DWORD* pData)
{
	if(m_nCurFont == -1){
		return;
	}

	if( (clrText & 0xff000000) == 0)
		return;
	
	if(count == -1) count = strlen(lpszString);

	stRectI clip = *(GetD3DDevice()->GetRenderRect());

	stRectI totalRect;
	stRectI layoutRect = i_layoutRect;

	SetFrameAllocator fa;
	t_aRenderFontItem renderList;
	t_aRenderLine lineList;
	if(dwTextFormat & DT_UNDERLINE)
		GetStringRect2(lpszString,count,layoutRect,totalRect,dwTextFormat,fontEffect,&renderList,&lineList,pData,clrText);
	else
		GetStringRect2(lpszString,count,layoutRect,totalRect,dwTextFormat,fontEffect,&renderList,NULL,pData,clrText);

	switch(fontEffect)
	{
	case FontEffect_Shadow:
		layoutRect.right++;
		layoutRect.bottom++;
		break;
	case FontEffect_Border:
		layoutRect.left--;
		layoutRect.top--;
		layoutRect.right++;
		layoutRect.bottom++;
		break;
	}
	clip &= layoutRect;
	if(clip.IsRectEmpty()) return;

	//bool bMagLinear,bMinLinear;
	//if(m_scale.x != 1 || m_scale.y != 1)
	{
		/*GetD3DDevice()->GetTexFilter(bMagLinear,bMinLinear);
		GetD3DDevice()->SetTexFilter(false,false);*/
	}
	int y = totalRect.top;
	int width ;
	width = totalRect.left;
	stPointF * scale = NULL;

	if(m_scale.x != 1.0f || m_scale.y != 1.0f)
	{
		scale = &m_scale;
	}

	int iFontHeight = (int)GetFontHeight();
	DWORD dwTime = xtimeGetTime();
	int nPicOffsetY = -4;
	switch(fontEffect)
	{
	case FontEffect_None:
		if( (clrText & 0xf0000000) == 0xf0000000)
		{
			for( t_aRenderFontItem::iterator it = renderList.begin();it != renderList.end();++it )
			{
				stRenderFontItem& st = (*it);
				stPointF pt(totalRect.left + st.pt.x,totalRect.top+st.pt.y);
				if( st.pPic && st.pPic->pBmps ) 
				{
					//stRectI clip2(0,0,(clip.right - pt.x)*2,getMax(st.pPic->size.cy * 2,clip.bottom - pt.y));
				
					st.pPic->pBmps->RenderAni((int)pt.x,(int)pt.y + nPicOffsetY,dwTime - st.time,NULL,&st.pPic->scale,clrText | 0xffffff);
				}
				else GetCurFont()->Render(st.ichar,pt.x,pt.y,st.color,&clip,scale);
			}
		}
		else
		{
			for( t_aRenderFontItem::iterator it = renderList.begin();it != renderList.end();++it )
			{
				stRenderFontItem& st = (*it);
				stPointF pt(totalRect.left + st.pt.x,totalRect.top+st.pt.y);
				if( st.pPic && st.pPic->pBmps ) 
				{
					//stRectI clip2(0,0,(clip.right - pt.x)*2,getMax(st.pPic->size.cy * 2,clip.bottom - pt.y));

					st.pPic->pBmps->RenderAni((int)pt.x,(int)pt.y + nPicOffsetY,dwTime - st.time,NULL,&st.pPic->scale,clrText | 0xffffff);
				}
				else GetCurFont()->Render(st.ichar,pt.x,pt.y,ColorBlend(st.color,clrText),&clip,scale);
			}
		}

		for(t_aRenderLine::iterator it = lineList.begin(); it != lineList.end(); ++it)
		{
			stLineInfo& st = (*it);
			stPointI pt1(totalRect.left + (long)st.pt1.x,totalRect.top + (long)st.pt1.y + iFontHeight),
					pt2(totalRect.left + (long)st.pt2.x,totalRect.top + (long)st.pt2.y + iFontHeight);
			GetDevice()->DrawLine(&pt1,&pt2,clrText);
		}

		break;
	case FontEffect_Shadow:
		if( (clrText & 0xf0000000) == 0xf0000000)
		{
			for(t_aRenderFontItem::iterator it = renderList.begin();it != renderList.end();++it)
			{
				stRenderFontItem& st = (*it);
				stPointF pt(totalRect.left + st.pt.x+m_scale.x,totalRect.top+st.pt.y+m_scale.y);
				if( st.pPic && st.pPic->pBmps ) 
				{
					//stRectI clip2(0,0,(clip.right - pt.x)*2,getMax(st.pPic->size.cy*2,clip.bottom - pt.y));
					st.pPic->pBmps->RenderAni((int)pt.x,(int)pt.y + nPicOffsetY,dwTime - st.time,NULL,&st.pPic->scale,clrText | 0xffffff);
				}
				else 
				{
					GetCurFont()->Render(st.ichar,pt.x,pt.y,clrShadow,&clip,scale);
					pt.x-= m_scale.x;
					pt.y-= m_scale.y;
					GetCurFont()->Render(st.ichar,pt.x,pt.y,st.color,&clip,scale);
				}
			}
		}
		else
		{
			for(t_aRenderFontItem::iterator it = renderList.begin();it != renderList.end();++it)
			{
				stRenderFontItem& st = (*it);
				stPointF pt(totalRect.left + st.pt.x+m_scale.x,totalRect.top+st.pt.y+m_scale.y);
				if( st.pPic && st.pPic->pBmps ) 
				{
					//stRectI clip2(0,0,(clip.right - pt.x)*2,getMax(st.pPic->size.cy*2,clip.bottom - pt.y));
					st.pPic->pBmps->RenderAni((int)pt.x,(int)pt.y + nPicOffsetY,dwTime - st.time,NULL,&st.pPic->scale,clrText | 0xffffff);
				}
				else 
				{
					GetCurFont()->Render(st.ichar,pt.x,pt.y,clrShadow,&clip,scale);
					pt.x-= m_scale.x;
					pt.y-= m_scale.y;
					GetCurFont()->Render(st.ichar,pt.x,pt.y,ColorBlend(st.color,clrText),&clip,scale);
				}
			}
		}

		for( t_aRenderLine::iterator it = lineList.begin(); it != lineList.end(); ++it )
		{
			stLineInfo& st = (*it);
			
			stPointI pt1(totalRect.left + (long)st.pt1.x + (long)m_scale.x,totalRect.top + (long)st.pt1.y + iFontHeight + (long)m_scale.y),
				pt2(totalRect.left + (long)st.pt2.x + (long)m_scale.x,totalRect.top + (long)st.pt2.y + iFontHeight + (long)m_scale.y);

			GetDevice()->DrawLine(&pt1,&pt2,clrShadow);
			pt1.x-= (long)m_scale.x;
			pt1.y-= (long)m_scale.y;
			pt2.x-= (long)m_scale.x;
			pt2.y-= (long)m_scale.y;
			GetDevice()->DrawLine(&pt1,&pt2,clrText);
		}

		break; 
	case FontEffect_Border:
		if(m_aFont[m_nCurFont].pBorder)
		{
			for(t_aRenderFontItem::iterator it = renderList.begin();it != renderList.end();++it)
			{
				stRenderFontItem& st = (*it);
				stPointF pt(totalRect.left + st.pt.x - m_scale.x,totalRect.top+st.pt.y - m_scale.y);
				if(st.pPic && st.pPic->pBmps) {}
				else m_aFont[m_nCurFont].pBorder->Render(st.ichar,pt.x,pt.y,clrShadow,&clip,scale);
			}
		}
		else
		{
			for(t_aRenderFontItem::iterator it = renderList.begin();it != renderList.end();++it)
			{
				stRenderFontItem& st = (*it);
				stPointF pt(totalRect.left + st.pt.x - m_scale.x,totalRect.top+st.pt.y - m_scale.y);
				if(st.pPic && st.pPic->pBmps) {}
				else
				{
					m_aFont[m_nCurFont].pNormal->Render(st.ichar,pt.x,pt.y,clrShadow,&clip,scale);
					m_aFont[m_nCurFont].pNormal->Render(st.ichar,pt.x + 1,pt.y,clrShadow,&clip,scale);
					m_aFont[m_nCurFont].pNormal->Render(st.ichar,pt.x + 2,pt.y,clrShadow,&clip,scale);

					m_aFont[m_nCurFont].pNormal->Render(st.ichar,pt.x,	  pt.y + 1,clrShadow,&clip,scale);
					m_aFont[m_nCurFont].pNormal->Render(st.ichar,pt.x + 2,pt.y + 1,clrShadow,&clip,scale);

					m_aFont[m_nCurFont].pNormal->Render(st.ichar,pt.x,	  pt.y + 2,clrShadow,&clip,scale);
					m_aFont[m_nCurFont].pNormal->Render(st.ichar,pt.x + 1,pt.y + 2,clrShadow,&clip,scale);
					m_aFont[m_nCurFont].pNormal->Render(st.ichar,pt.x + 2,pt.y + 2,clrShadow,&clip,scale);
				}
			}
		}

		if( (clrText & 0xf0000000) == 0xf0000000)
		{
			for(t_aRenderFontItem::iterator it = renderList.begin();it != renderList.end();++it)
			{
				stRenderFontItem& st = (*it);
				stPointF pt(totalRect.left + st.pt.x,totalRect.top+st.pt.y);
				if( st.pPic && st.pPic->pBmps ) 
				{
					//stRectI clip2(0,0,(clip.right - pt.x)*2,getMax(st.pPic->size.cy*2,clip.bottom - pt.y));
					st.pPic->pBmps->RenderAni((int)pt.x,(int)pt.y + nPicOffsetY,dwTime - st.time,NULL,&st.pPic->scale,clrText | 0xffffff);
				}
				else 
				{
					m_aFont[m_nCurFont].pNormal->Render(st.ichar,pt.x,pt.y,st.color,&clip,scale);
				}
			}
		}
		else
		{
			for(t_aRenderFontItem::iterator it = renderList.begin();it != renderList.end();++it)
			{
				stRenderFontItem& st = (*it);
				stPointF pt(totalRect.left + st.pt.x,totalRect.top+st.pt.y);
				if( st.pPic && st.pPic->pBmps ) 
				{
					//stRectI clip2(0,0,(clip.right - pt.x)*2,getMax(st.pPic->size.cy*2,clip.bottom - pt.y));
					st.pPic->pBmps->RenderAni((int)pt.x,(int)pt.y + nPicOffsetY,dwTime - st.time,NULL,&st.pPic->scale,clrText | 0xffffff);
				}
				else
				{
					m_aFont[m_nCurFont].pNormal->Render(st.ichar,pt.x,pt.y,ColorBlend(st.color,clrText),&clip,scale);
				}
			}
		}

		for(t_aRenderLine::iterator it = lineList.begin(); it != lineList.end(); ++it)
		{
			stLineInfo& st = (*it);
			
			stPointI pt1(totalRect.left + (long)st.pt1.x,totalRect.top + (long)st.pt1.y + iFontHeight),
				pt2(totalRect.left + (long)st.pt2.x,totalRect.top + (long)st.pt2.y + iFontHeight);

			stRectI rect1(pt1.x-1,pt1.y -1,pt2.x + 1,pt2.y+1);

			GetDevice()->FillRect(&rect1,clrShadow);
			GetDevice()->DrawLine(&pt1,&pt2,clrText);
		}

		break;
	}

	//if(m_scale.x != 1 || m_scale.y != 1)
	{
		//GetD3DDevice()->SetTexFilter(bMagLinear,bMinLinear);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param char* : ÃèÊö
 * \param count : ÃèÊö
 * \param & : ÃèÊö
 * \param & : ÃèÊö
 * \param dwTextFormat : ÃèÊö
 * \param fontEffect : ÃèÊö
 * \param pRenderList : ÃèÊö
 * \param pLineList : ÃèÊö
 * \param pData : ÃèÊö
 * \param color : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CD3DTextout::GetStringRect2(const char* lpszString, size_t count,RECT & layoutRect,RECT & totalRect,DWORD dwTextFormat ,enumFontEffect fontEffect,t_aRenderFontItem* pRenderList,t_aRenderLine* pLineList,DWORD* pData,DWORD color)
{
	SIZE size = GetStringSize(lpszString,count,layoutRect.right - layoutRect.left,(0 != dwTextFormat),fontEffect,pRenderList,pLineList,pData,color);

	stRectI & rectDst = (stRectI&)layoutRect;

	if(dwTextFormat & DT_VCENTER)
	{
		Assert(!(dwTextFormat & DT_BOTTOM));
		Assert(!(dwTextFormat & DT_TOP));
		totalRect.top = rectDst.top + (rectDst.Height() - size.cy)/2;
	}
	else if(dwTextFormat & DT_BOTTOM)
	{
		Assert(!(dwTextFormat & DT_VCENTER));
		Assert(!(dwTextFormat & DT_TOP));
		totalRect.top = rectDst.bottom - size.cy;
	}
	else
	{
		Assert(!(dwTextFormat & DT_BOTTOM));
		Assert(!(dwTextFormat & DT_VCENTER));
		totalRect.top = rectDst.top;
	}

	if( dwTextFormat & DT_CENTER )
	{
		Assert(!(dwTextFormat & DT_LEFT));
		Assert(!(dwTextFormat & DT_RIGHT));
		totalRect.left = rectDst.left + (rectDst.Width() - size.cx)/2;
	}

	else if( dwTextFormat & DT_RIGHT )
	{
		Assert(!(dwTextFormat & DT_LEFT));
		Assert(!(dwTextFormat & DT_CENTER));
		totalRect.left = rectDst.right - size.cx;
	}
	else
	{
		Assert(!(dwTextFormat & DT_CENTER));
		Assert(!(dwTextFormat & DT_RIGHT));
		totalRect.left = rectDst.left;
	}

	totalRect.right = totalRect.left + size.cx;
	totalRect.bottom = totalRect.top + size.cy;

	rectDst &= totalRect;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param char* : ÃèÊö
 * \param count : ÃèÊö
 * \param & : ÃèÊö
 * \param dwTextFormat : ÃèÊö
 * \param fontEffect : ÃèÊö
 * \param pRenderList : ÃèÊö
 * \param pLineList : ÃèÊö
 * \param pData : ÃèÊö
 * \param color : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CD3DTextout::GetStringRect(const char* lpszString, size_t count,RECT & layoutRect,DWORD dwTextFormat,enumFontEffect fontEffect,t_aRenderFontItem* pRenderList,t_aRenderLine* pLineList,DWORD* pData,DWORD color )
{
	RECT totalRect;
	GetStringRect2(lpszString,count,layoutRect,totalRect,dwTextFormat,fontEffect,pRenderList,pLineList,pData,color);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param char* : ÃèÊö
 * \param count : ÃèÊö
 * \param maxWidth : ÃèÊö
 * \param bSingleLine : ÃèÊö
 * \param fontEffect : ÃèÊö
 * \param pRenderList : ÃèÊö
 * \param pLineList : ÃèÊö
 * \param pData : ÃèÊö
 * \param color : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
SIZE CD3DTextout::GetStringSize(const char* lpszString,
								size_t count,
								size_t maxWidth,
								bool bSingleLine,
								enumFontEffect fontEffect,
								t_aRenderFontItem* pRenderList,
								t_aRenderLine* pLineList,
								DWORD* pData,
								DWORD color)
{
	if(m_nCurFont == -1)
	{
		SIZE size1 = {0,0};
		return size1;
	}

	const char* p = lpszString;
	const char* end;
	size_t len = strlen(p);
	if(count > len) count = len;
	end = p + count;

	int linenum = 1;
	float width = 0;
	
	SIZE size = {0,0};

	int iFont = m_nCurFont;
	//if(fontEffect == FontEffect_Border)
	//	iFont = 1;

	float nFontWidth,nFontHeight;
	nFontWidth = GetFontWidth(iFont);
	nFontHeight = GetFontHeight(iFont);

	float fontHeight = nFontHeight * m_scale.y + m_nVSpace;
	float tabwidth0 = m_nTabSize * nFontWidth+ (m_nTabSize > 1 ? (m_nTabSize-1) * m_nHSpace : 0);
	float tabwidth = m_nTabSize * (nFontWidth+ m_nHSpace);
	
	float y = 0;
	size_t dataIndex = 0;
	if(pRenderList)
	{
		while(p != end)
		{
			float addw = 0;
			if((*p) & 0x80)
			{
				if(p+1 == end) break;
				int ichar = *((WORD*)p);
				if( !bSingleLine )
				{
					if(width != 0) addw += m_nHSpace;
					addw += GetAFontWidth( ichar ,iFont);
					if( width && width + addw > maxWidth )
					{
						if(pLineList)
						{
							stLineInfo line;
							line.pt1.x = 0;
							line.pt1.y = y;
							line.pt2.x = width;
							line.pt2.y = y;
							pLineList->push_back(line);
						}

						linenum++;
						if(width > size.cx) size.cx = (long)width;
						width = 0;
						y += fontHeight;
					}
					else
					{
						pRenderList->push_back(stRenderFontItem());
						pRenderList->back().ichar = ichar;
						pRenderList->back().pPic = GetPicCharInfo(ichar);
						pRenderList->back().pt = stPointI((long)width,(long)y);

						if(pData)
						{
							if(pRenderList->back().pPic)
								pRenderList->back().time = pData[dataIndex];
							else
								pRenderList->back().color = pData[dataIndex];
						}
						else
						{
							pRenderList->back().color = color;
							pRenderList->back().time = 0;
						}

						width += addw;
						p+=2;
						dataIndex++;
					}
				}
				else 
				{
					pRenderList->push_back(stRenderFontItem());
					pRenderList->back().ichar = ichar;
					pRenderList->back().pPic = GetPicCharInfo(ichar);
					pRenderList->back().pt = stPointI((long)width,(long)y);
					
					if(pData)
					{
						if(pRenderList->back().pPic)
							pRenderList->back().time = pData[dataIndex];
						else
							pRenderList->back().color = pData[dataIndex];
					}
					else
					{
						pRenderList->back().color = color;
						pRenderList->back().time = 0;
					}

					if(width != 0) addw += m_nHSpace;
					addw += GetAFontWidth(ichar,iFont);
					width += addw;
					p+=2;
					dataIndex++;
				}
			}
			else
			{
				int ichar = *p;
				size_t charnum;
				if( IsNewLine(p,charnum) )
				{
					if(pLineList)
					{
						stLineInfo line;
						line.pt1.x = 0;
						line.pt1.y = y;
						line.pt2.x = width;
						line.pt2.y = y;
						pLineList->push_back(line);
					}

					linenum++;
					p += charnum;
					dataIndex += charnum;

					if(width > size.cx) size.cx = (long)width;
					width = 0;
					y += fontHeight;
				}
				else if(*p == '\t')
				{
					if(width == 0) addw = tabwidth0;
					else addw = tabwidth;
					if( width && width + addw > maxWidth)
					{
						linenum++;
						y += fontHeight;
						if(width > size.cx) size.cx = (long)width;
						width = 0;
					}
					else
					{
						width += addw;
					}
					p++;
					dataIndex ++;
				}
				else
				{
					if(!bSingleLine)
					{
						if(width != 0) addw += m_nHSpace;
						addw += GetAFontWidth(ichar,iFont);

						if( width && width + addw > maxWidth)
						{
							if(pLineList)
							{
								stLineInfo line;
								line.pt1.x = 0;
								line.pt1.y = y;
								line.pt2.x = width;
								line.pt2.y = y;
								pLineList->push_back(line);
							}

							linenum++;
							y += fontHeight;
							if(width > size.cx) size.cx = (long)width;
							width = 0;
						}
						else
						{
							pRenderList->push_back( stRenderFontItem() );
							pRenderList->back().ichar = ichar;
							pRenderList->back().pPic = GetPicCharInfo(ichar);
							pRenderList->back().pt = stPointI((long)width,(long)y);

							if(pData)
							{
								if(pRenderList->back().pPic)
									pRenderList->back().time = pData[dataIndex];
								else
									pRenderList->back().color = pData[dataIndex];
							}
							else
							{
								pRenderList->back().color = color;
								pRenderList->back().time = 0;
							}

							width += addw;
							p++;
							dataIndex++;
						}
					}
					else 
					{
						pRenderList->push_back(stRenderFontItem());
						pRenderList->back().ichar = ichar;
						pRenderList->back().pPic = GetPicCharInfo(ichar);
						pRenderList->back().pt = stPointI((long)width,(long)y);
						
						if(pData)
						{
							if(pRenderList->back().pPic)
								pRenderList->back().time = pData[dataIndex];
							else
								pRenderList->back().color = pData[dataIndex];
						}
						else
						{
							pRenderList->back().color = color;
							pRenderList->back().time = 0;
						}

						if(width != 0) addw += m_nHSpace;
						addw += GetAFontWidth(ichar,iFont);
						width += addw;
						p++;
						dataIndex++;
					}
				}
			}
		}

		if(width && pLineList)
		{
			stLineInfo line;
			line.pt1.x = 0;
			line.pt1.y = y;
			line.pt2.x = width;
			line.pt2.y = y;
			pLineList->push_back(line);
		}
	}
	else
	{
		while(p != end)
		{
			int addw = 0;
			if((*p) & 0x80)
			{
				if(p+1 == end) break;
				int ichar = *((WORD*)p);
				if(!bSingleLine)
				{
					if((int)width != 0) addw += m_nHSpace;
					addw += (int)GetAFontWidth( ichar ,iFont);
					if( width && width + addw > maxWidth)
					{
						linenum++;
						if(width > size.cx) size.cx = (long)width;
						width = 0;
						y += fontHeight;
					}
					else
					{
						width += addw;
						p+=2;
					}
				}
				else 
				{
					if(width != 0) addw += m_nHSpace;
					addw += (int)GetAFontWidth(ichar,iFont);
					width += addw;
					p+=2;
				}
			}
			else
			{
				int ichar = *p;
				size_t charnum;
				if(IsNewLine(p,charnum))
				{
					linenum++;
					p += charnum;
					if(width > size.cx) size.cx = (long)width;
					width = 0;
					y += fontHeight;
				}
				else if(*p == '\t')
				{
					if(width == 0) addw = (int)tabwidth0;
					else addw = (int)tabwidth;
					if( width && width + addw > maxWidth)
					{
						linenum++;
						y += fontHeight;
						if(width > size.cx) size.cx = (long)width;
						width = 0;
					}
					else
					{
						width += addw;
					}
					p++;
				}
				else
				{
					if(!bSingleLine)
					{
						if(width != 0) addw += m_nHSpace;
						addw += (int)GetAFontWidth(ichar,iFont);

						if( width && width + addw > maxWidth)
						{
							linenum++;
							y += fontHeight;
							if(width > size.cx) size.cx = (long)width;
							width = 0;
						}
						else
						{
							width += addw;
							p++;
						}
					}
					else 
					{
						if(width != 0) addw += m_nHSpace;
						addw += (int)GetAFontWidth(ichar,iFont);
						width += addw;
						p++;
					}
				}
			}
		}
	}
	if(width > size.cx) 
	{
		size.cx = (long)width;
	}

	size.cy = linenum * (int)GetFontHeight(iFont) + (linenum -1) * m_nVSpace;

	return size;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ichar : ÃèÊö
 * \param iFont : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
float CD3DTextout::GetAFontWidth(int ichar,int iFont)
{
	if( ichar - c_nPicCharBase < m_nPicCharCount) return (float)m_pPicCharInfo[ichar-c_nPicCharBase].size.cx;
	return ((ichar & 0x80) ? GetFontWcharWidth(iFont) : GetFontWidth(iFont));
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param ichar : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
stPicCharInfo* CD3DTextout::GetPicCharInfo(int ichar)
{
	if( ichar - c_nPicCharBase < m_nPicCharCount) 
	{
		//Engine_WriteLog("lksdfj;laskfja;sdf\n");
		return &m_pPicCharInfo[ichar-c_nPicCharBase];
	}
	return NULL;
}