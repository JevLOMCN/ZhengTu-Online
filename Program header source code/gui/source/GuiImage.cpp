/**
 * \file      GuiImage.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾@###.com
 *           $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     GUIÍ¼Ïñ¿Ø¼ş
 * 
 *	      GUIÍ¼Ïñ¿Ø¼ş
 */

#include "../../engine/include/engine.h"
#include "../include/guiTypes.h"

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiImage::CGuiImage(CGuiDialog *pDialog)
: CGuiControl(pDialog)
, m_TextColor(-1)
, m_dwTextShadowColor(0xa0101010)
, m_dwTextFormat(DT_CENTER | DT_VCENTER)
, m_eFontEffect(FontEffect_Shadow)
, m_bIsBack(false)
{
	m_Type = GUI_CONTROL_IMAGE;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CGuiImage::~CGuiImage(void)
{
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param rl : ÃèÊö
 * \param rc : ÃèÊö
 * \param color : ÃèÊö
 * \param bHide : ÃèÊö
 * \param useType : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
HRESULT CGuiImage::SetImage(const stResourceLocation & rl,const RECT * rc,DWORD color, const bool bHide, const enumUSE_TYPE& useType)
{
	m_Images.resize(0);
	stRectI rect;
	rect.left = 0;
	rect.top = 0;
	if(GetStyle() & FULLSIZE_IMAGE)
	{
		m_width = 0;
		m_height = 0;
		if(rc)
		{
			rect.right = rc->right - rc->left;
			rect.bottom = rc->bottom - rc->top;
		}
		else
		{
			IBitmap* pBmp = GetDevice()->FindBitmap(&rl);
			if(pBmp)
			{
				rect.left = 0;
				rect.top = 0;
				rect.right = pBmp->GetWidth();
				rect.bottom = pBmp->GetHeight();
			}
			else
			{
				rect.right = 0;
				rect.bottom = 0;
			}
		}
	}
	else
	{
		rect.right = m_width;
		rect.bottom = m_height;
		
	}

	AddAnimation(rl,color,rc,&rect, bHide, useType );
	return S_OK;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param  : ÃèÊö
 * \param  : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
//Show or hide image by using type
void CGuiImage::ShowImageByType(const enumUSE_TYPE& useType /* = enumUSE_TYPE::USE_TYPE_NUMS */, const bool& bHide /* = false  */)
{
	//Wether a valide use type
	if ( useType < 0 || useType >= USE_TYPE_NUMS )
	{
		return;
	}

	for(std::vector<stImgInfo>::iterator it = m_Images.begin(); it != m_Images.end();++it)
	{
		stImgInfo* st = &(*it);

		if ( useType == st->useType )
		{
			//change show or hide
			st->bHide = bHide;
		}
		
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param fElapsedTime : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiImage::Render( float fElapsedTime )
{
	if (!m_bVisible)
		return;

	stRectI oldClip;
	if(m_rectClip.left != -10000)
	{
		stRectI rc = m_rectClip;
		rc.OffsetRect(m_pDialog->GetClientX(),m_pDialog->GetClientY());
		oldClip = GetDevice()->SetClipRect(&rc);
	}

	CGuiControl::Render(fElapsedTime);
	/*for(std::vector<stImgInfo>::reverse_iterator it = m_Images.rbegin(); it != m_Images.rend();++it)
	{
		stImgInfo& st = *it;
		
		stPointI ptDest(m_x + m_pDialog->GetClientX() + st.rect.left,m_y + m_pDialog->GetClientY() + st.rect.top);
		
		if( m_pDialog->IsCaption() )
			ptDest.y += m_pDialog->GetCaptionHeight();

		if(st.rect.Width() == st.clip.Width() && st.rect.Height() == st.clip.Height())
		{
			st.image.Render(ptDest.x,ptDest.y,&st.clip,NULL);
		}
		else
		{
			stPointF scale((float)st.rect.Width()/(float)st.clip.Width(),(float)st.rect.Height()/(float)st.clip.Height());
			st.image.Render(ptDest.x,ptDest.y,&st.clip,&scale);
		}
	}*/

	
	for(std::vector<stImgInfo>::iterator it = m_Images.begin(); it != m_Images.end();++it)
	{
		stImgInfo& st = *it;

		//see wether cur image is hide
		if( st.bHide )	continue;

		stPointI ptDest(m_x + m_pDialog->GetClientX() + st.rect.left,m_y + m_pDialog->GetClientY() + st.rect.top);

		if( m_pDialog->IsCaption() )
			ptDest.y += m_pDialog->GetCaptionHeight();

		if(st.rect.Width() == st.clip.Width() && st.rect.Height() == st.clip.Height())
		{
			st.image.Render(ptDest.x,ptDest.y,&st.clip,NULL,m_pDialog->GetColor());
		}
		else
		{
			stPointF scale((float)st.rect.Width()/(float)st.clip.Width(),(float)st.rect.Height()/(float)st.clip.Height());
			st.image.Render(ptDest.x,ptDest.y,&st.clip,&scale,m_pDialog->GetColor());
		}
	}

	if(!m_strText.empty())
	{
		stRectI rect;
		rect.left = m_x + m_pDialog->GetClientX();
		rect.top = m_y + m_pDialog->GetClientY();
		if(m_pDialog->IsCaption())
			rect.top += m_pDialog->GetCaptionHeight();

		rect.right = rect.left + m_width;
		rect.bottom = rect.top + m_height;


		GetDevice()->DrawString(m_strText.c_str(),rect,m_TextColor,m_dwTextFormat,m_eFontEffect,m_dwTextShadowColor);
	}
	if(m_rectClip.left != -10000)
		GetDevice()->SetClipRect(&oldClip);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CAnimation * CGuiImage::GetImage(int index)
{
	return &m_Images[index].image;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param rl : ÃèÊö
 * \param color : ÃèÊö
 * \param clip : ÃèÊö
 * \param rect : ÃèÊö
 * \param bHide : ÃèÊö
 * \param useType : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiImage::AddAnimation(const stResourceLocation & rl,DWORD color,const RECT * clip,const RECT * rect, const bool bHide, const enumUSE_TYPE& useType )
{
	m_Images.push_back(stImgInfo());
	stImgInfo& st = m_Images.back();

	st.bHide = bHide;
	st.useType = useType;

	st.image.Create(&rl);
	st.image.SetColor(color);
	
	st.clip.SetRect(0,0,st.image.GetWidth(),st.image.GetHeight());
	if(clip) st.clip = *clip;
	
	st.rect.SetRect(0,0,st.image.GetWidth(),st.image.GetHeight());
	if(rect) st.rect = *rect;

	if(st.rect.right > m_width) m_width = st.rect.right;
	if(st.rect.bottom > m_height) m_height = st.rect.bottom;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiImage::Clear()
{
	m_Images.resize(0);
	m_width = 0;
	m_height = 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiImage::RemoveAnimation(int index)
{
	std::vector<stImgInfo>::iterator it = m_Images.begin();
	for(int i=0;i<index;++i)++it;
	m_Images.erase(it);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
const char* CGuiImage::GetImgPackName()
{
	if(m_Images.size())
	{
		IBitmap* pBmp = m_Images[0].image.GetFrame(0);
		if(pBmp) return pBmp->GetPackName();
	}
	return NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CGuiImage::GetImgGroup()
{
	if(m_Images.size())
	{
		IBitmap* pBmp = m_Images[0].image.GetFrame(0);
		if(pBmp) return pBmp->GetGroup();
	}
	return -1;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CGuiImage::GetImgFrame()
{
	if(m_Images.size())
	{
		IBitmap* pBmp = m_Images[0].image.GetFrame(0);
		if(pBmp) return pBmp->GetFrame();
	}
	return -1;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pt : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiImage::ContainsPoint( POINT pt )
{
	if(m_Images.empty()) return false;
	stImgInfo& st = m_Images[0];

	pt.x -= st.rect.left;
	pt.y -= st.rect.top;
	IBitmap* pBmp = st.image.GetFrame(0);
	if(!pBmp) return false;
	pt.x = pt.x * pBmp->GetWidth()/st.rect.Width();
	pt.y = pt.y * pBmp->GetHeight()/st.rect.Height();
	return pBmp->IsAt(pt,false,false);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param color : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiImage::SetColor(DWORD color)
{
	for(size_t i=0;i<m_Images.size();++i)
	{
		m_Images[i].image.SetColor(color);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
DWORD CGuiImage::GetColor()
{
	if(m_Images.empty()) return 0;
	return m_Images[0].image.GetColor();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CGuiImage::IsNullImage(int index)
{
	if((size_t)index >= m_Images.size()) return true;
	return m_Images[index].image.IsNullImage(0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \param color : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiImage::SetImageColor(int index,DWORD color)
{
	if((size_t)index >= m_Images.size()) return;
	m_Images[index].image.SetColor(color);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param index : ÃèÊö
 * \param type : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CGuiImage::SetImageBlend(int index,BlendType type)
{
	if((size_t)index >= m_Images.size()) return;
	m_Images[index].image.SetBlendType(type);
}