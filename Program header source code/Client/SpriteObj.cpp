/*
   文本精灵类: 
       可以用于文本滚动控制等
*/
#include "public.h"
#include "..\engine\include\stringex.h"
#include "./SpriteObj.h"
 

/////////////////////////////////////////////////////////////////////
// 
//  浮点数转成整数,游戏编程里浮点很重要,只有最后的时候才转成整数
// 
/////////////////////////////////////////////////////////////////////
int FloatRound(float x)
{
	FUNCTION_BEGIN;

	int xx = (int)abs(x);
	float dot2 = x - xx;
	if( dot2 > -0.000001 && dot2 < 0.000001)
		dot2 = 0;
	if( dot2 == 0)
		return (int)x;
	else
	{
		if( x < 0 )
			return -xx-1;
		else
			return xx+1;
	}

	FUNCTION_END;
}


 
CSpriteObj::CSpriteObj()
{
	FUNCTION_BEGIN;

	ResetSprite();

	FUNCTION_END;
}

CSpriteObj::~CSpriteObj()
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

void CSpriteObj::ResetSprite()
{
	FUNCTION_BEGIN;

	bIsPlaying = FALSE;
	strText.clear();
	fRunTime = 0.0f;
	bLoop = FALSE;

	FUNCTION_END;
}

void CSpriteObj::BeginSprite(BOOL bLoop)
{
	FUNCTION_BEGIN;

	if( strText.length() == 0) return;
	
	ptCurPos.x = ptSrcPos.x;
	ptCurPos.y = ptSrcPos.y;


	bIsPlaying = true;
	fRunTime = 0;
	bLoop = bLoop;
	 
	
	fTotalTime =  (ptSrcPos.x - ptDstPos.x) / fSpeedX;
	if( fTotalTime < 0)
		fTotalTime =-fTotalTime;

	FUNCTION_END;

}

void CSpriteObj::UpdateSprite(float fElaspedTime)
{
	FUNCTION_BEGIN;

	if( !bIsPlaying) return;

	fRunTime += fElaspedTime;
	if(fRunTime >= fTotalTime)
	{
		if( bLoop )
		{
			fRunTime = 0 ;
			ptCurPos.x = ptSrcPos.x;
			ptCurPos.y = ptSrcPos.y;
		}
		else
		{
			bIsPlaying = false;
			ptCurPos.x = ptDstPos.x;
			ptCurPos.y = ptDstPos.y;
		
			EndSprite();
		}
	}
	else
	{
		//int fx =  FloatRound(fElaspedTime * fSpeedX);
		//int fy =  FloatRound(fElaspedTime * fSpeedY);
		float fx = fElaspedTime * fSpeedX;
		float fy = fElaspedTime * fSpeedY;
		ptCurPos.x +=  fx;
		ptCurPos.y +=  fy;

	}

	FUNCTION_END;
}

void CSpriteObj::RenderSprite()
{ 
	FUNCTION_BEGIN;

	if( !bIsPlaying) return;

	stRectI rcLayOut = rcRender;
	rcLayOut.left = (LONG)ptCurPos.x;
	rcLayOut.top = (LONG)ptCurPos.y;
	stPointI ptLayout((LONG)ptCurPos.x,(LONG)ptCurPos.y);
	ptLayout.y +=  (rcRender.Height() - GetDevice()->GetFontHeight(iFont)) / 2;

	if( m_bBackGround )
		GetDevice()->FillRect(&rcRender,dwBkColor);

	int iOldFont = GetDevice()->SetFont(iFont);

	RECT rcOldClip = GetDevice()->SetClipRect(&rcRender);
	GetDevice()->DrawString(strText.c_str(),ptLayout,
		dwTextColor,
		FontEffect_Border,
		NULL);
	GetDevice()->SetClipRect(&rcOldClip);

	GetDevice()->SetFont(iOldFont);

	FUNCTION_END;
}


CGMTextEffect::CGMTextEffect()
: nCurCount(0)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

CGMTextEffect::~CGMTextEffect()
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

void CGMTextEffect::AddText(const char * pText)
{
	FUNCTION_BEGIN;

	size_t len = strlen(pText); 
	char *szTemp = (char*)_alloca(sizeof(char) * (len + 1));
        memcpy(szTemp,pText,len);
	szTemp[len] = 0;
	str_trim_right(szTemp);
	str_trim_left(szTemp);

	if( strlen(szTemp) > 0 )
	{
		stTextItem item;
		item.nCount = 1;   // if ncount changed, do it here    
		item.szText = pText;
		m_vecText.push_back(item);
		
		if(!bIsPlaying && m_vecText.size() > 0 )
		{
			stTextItem & item = m_vecText.front();
			strText = item.szText;
			nCurCount = item.nCount;
			SimpleRemoveFromArray(m_vecText,0);
		}
	}
	FUNCTION_END;
}

void CGMTextEffect::EndSprite()
{
	FUNCTION_BEGIN;
        

	if( nCurCount > 0)
	{
		BeginSprite(bLoop);
	}
	else 	if( m_vecText.size() > 0 )
	{
		stTextItem & item = m_vecText.front();
		strText = item.szText;
		nCurCount  = item.nCount;
		SimpleRemoveFromArray(m_vecText,0);
                
		BeginSprite(bLoop);
	}

	FUNCTION_END;
}

void CGMTextEffect::BeginSprite(BOOL bLoop)
{
	FUNCTION_BEGIN;


	nCurCount --;
	if( nCurCount < 0) return;

	ptDstPos.x = (float)(rcRender.left - ((int)strText.length()) * GetDevice()->GetFontWidth(iFont)) ;
	ptDstPos.y = (float)rcRender.top;

	ptSrcPos.x = (float)rcRender.right;
	ptSrcPos.y = (float)rcRender.top;

	CSpriteObj::BeginSprite(bLoop);

	FUNCTION_END;
}


CMiniMapTextEffect::CMiniMapTextEffect()
: bAnimation(true)
, m_MouseMode(MouseOnNull)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

CMiniMapTextEffect::~CMiniMapTextEffect()
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

void CMiniMapTextEffect::BeginSprite(bool bLoop)
{
	FUNCTION_BEGIN;

	int iTextWidth = (int)((float)strText.length() * GetDevice()->GetFontWidth(iFont) * GetDevice()->GetFontScale().y);
	if( iTextWidth <= rcRender.Width())
	{
		bAnimation = false;
		bIsPlaying = false;
	}
	else
	{
		if( !bIsPlaying )
		{
			//ptDstPos.x = (float)rcRender.left -  (iTextWidth - rcRender.Width());
			//ptDstPos.y = (float)rcRender.top;
			//ptSrcPos.x = (float)rcRender.left;
			//ptSrcPos.y = (float)rcRender.top;

			bAnimation = true;

			ptDstPos.x = (float)(rcRender.left - iTextWidth);
			ptDstPos.y = (float)rcRender.top;

			ptSrcPos.x = (float)rcRender.right;
			ptSrcPos.y = (float)rcRender.top;


			CSpriteObj::BeginSprite(bLoop);
		}
	}
	FUNCTION_END;

}

void CMiniMapTextEffect::UpdateSprite(float fElapsedTime)
{
       FUNCTION_BEGIN;

       if( bAnimation)
	      CSpriteObj::UpdateSprite(fElapsedTime);

       FUNCTION_END;
}

void CMiniMapTextEffect::RenderSprite()
{
      FUNCTION_BEGIN;

     if( bAnimation)
     {
	     if(m_MouseMode == MouseOnLeft)
	     {
		     RECT rcOldClip = GetDevice()->SetClipRect(&rcRender);
		     GetDevice()->DrawString(strText.c_str(),rcRender,0xffffffff,DT_LEFT,FontEffect_Border);
		     GetDevice()->SetClipRect(&rcOldClip);		     
	     }
	     else if(m_MouseMode == MouseOnRight)
	     {
		     RECT rcOldClip = GetDevice()->SetClipRect(&rcRender);
		     GetDevice()->DrawString(strText.c_str(),rcRender,0xffffffff,DT_RIGHT,FontEffect_Border);
		     GetDevice()->SetClipRect(&rcOldClip);		    
	     }
	     else 
	     {
		     CSpriteObj::RenderSprite();
	     }
     }
     else
     {
	     GetDevice()->DrawString(strText.c_str(),rcRender,0xffffffff,DT_CENTER,FontEffect_Border);
     }

     FUNCTION_END;
}

void CCreateRoleTextEffect::BeginSprite(bool bLoop)
{
	FUNCTION_BEGIN;

        if(!m_pBitmap) return;

	stRectI rcClip = m_pBitmap->GetClipRect();
	m_fscale.x = (float)rcRender.Width()/rcClip.Width();
	m_fscale.y = (float)rcRender.Height()/rcClip.Height();

	ptSrcPos.x = rcRender.left - m_fscale.x * rcClip.left;
	ptSrcPos.y = rcRender.bottom - m_fscale.y * rcClip.top;
	ptDstPos.x = ptSrcPos.x;
	ptDstPos.y = rcRender.top - rcClip.Height() * m_fscale.y;  

	ptCurPos.x = ptSrcPos.x;
	ptCurPos.y = ptSrcPos.y;

	bIsPlaying = true;
	bLoop = bLoop;
	fRunTime = 0;

        assert(fSpeedY != 0);
	fTotalTime =  (ptSrcPos.y - ptDstPos.y) / fSpeedY;
	if( fTotalTime < 0)
		fTotalTime =-fTotalTime;
}

void CCreateRoleTextEffect::RenderSprite()
{ 
	FUNCTION_BEGIN;

	if( !bIsPlaying) return;

	RECT rcOldClip = GetDevice()->SetClipRect(&rcRender);
	m_pBitmap->Render((int)(ptCurPos.x),(int)(ptCurPos.y),NULL,&m_fscale);
	GetDevice()->SetClipRect(&rcOldClip);

	FUNCTION_END;
}
void CCreateRoleTextEffect::Release()
{
	FUNCTION_BEGIN;
	m_pBitmap = NULL;
	bIsPlaying = false;
	FUNCTION_END;
}
