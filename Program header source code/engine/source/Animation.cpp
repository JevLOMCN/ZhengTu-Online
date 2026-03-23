#include "../include/engine.h"
#include "../include/animation.h"
 
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CAnimation::CAnimation(void)
: m_pImages(NULL)
, m_CurrentColor(1,1,1,1)
, m_EndColor(1,1,1,1)
, m_fFrame(0.0f)
, m_BlendType(Blend_Null)
, m_fColorTime(0.0f)
, m_fSpeed(0.0f)
, m_bLoop(true)
, m_bStop(true)
, m_bRandomPlay(false)
, m_StartFramel(0)
, m_EndFrame(0)
{
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CAnimation::~CAnimation(void)
{
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param prl : ÃèÊö
 * \param bRandomBeginFrame : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CAnimation::Create(const stResourceLocation* prl,bool bRandomBeginFrame)
{
	//MyOutputDebugString("aaaaaaaaaa %p\n",&m_pImages);
	IBitmaps* pBitmaps = GetDevice()->FindBitmaps(prl);
	if(!pBitmaps) return false;
	Assert(pBitmaps);
	m_StartFramel = 0;
	m_EndFrame = pBitmaps->GetFrameCount();
	return Create(pBitmaps,bRandomBeginFrame);
}

bool CAnimation::Create(const stResourceLocation* prl,WORD startFramel,WORD endFrame,bool bRandomBeginFrame)
{
	//MyOutputDebugString("aaaaaaaaaa %p\n",&m_pImages);
	IBitmaps* pBitmaps = GetDevice()->FindBitmaps(prl);
	if(!pBitmaps) return false;
	Assert(pBitmaps);
	m_StartFramel = startFramel;
	if(endFrame > pBitmaps->GetFrameCount())
		m_EndFrame = pBitmaps->GetFrameCount();
	else
		m_EndFrame = endFrame;

	return Create(pBitmaps,bRandomBeginFrame);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pBitmaps : ÃèÊö
 * \param bRandomBeginFrame : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CAnimation::Create(IBitmaps* pBitmaps,bool bRandomBeginFrame)
{
	Assert(pBitmaps);
	m_pImages = pBitmaps;
	const stPackAniHeader* p = pBitmaps->GetAniInfo();
	SetColor(p->dwColor);
	SetBlendType((BlendType)p->blend);
	SetSpeed(p->wdAniSpeed);

	m_fFrame =-1;
	m_fColorTime = -1;
	if(pBitmaps->GetFrameCount())
	{
		//pBitmaps->TryLoadTexture();
		if( bRandomBeginFrame )
			m_fFrame = (float)(rand() % pBitmaps->GetFrameCount());
		else
			m_fFrame = 0.0f;
	}
	m_bStop = false;
	return true;
}

float CAnimation::GetSpeed()
{
	return m_fSpeed;
}
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CAnimation::UpdateFrame()
{
	if(m_bStop) return;
	if(!m_pImages) return;
	if(m_EndFrame == 0) m_EndFrame = m_pImages->GetFrameCount();
	if((m_EndFrame - m_StartFramel) < 2) return;

	float fTime = Engine_GetDrawElapsedTime();

	if ( m_bLoop && m_bRandomPlay )
	{
		static float lastTime = 0;
		lastTime += fTime * m_fSpeed;
		if ( fabs(lastTime) >= 1 )
		{
			m_fFrame = (float)( rand() % m_EndFrame );
			lastTime = 0;
		}
	}
	else
        m_fFrame += fTime * m_fSpeed;

	if( (int)m_fFrame >= m_EndFrame)
	{
		if(!m_bLoop) 
		{
			m_bStop = true;
			m_fFrame = m_EndFrame - 1.0f;
		}
		else
			m_fFrame = m_StartFramel;
	}
	if(m_fFrame < m_StartFramel)
	{
		if(!m_bLoop) 
		{
			m_fFrame = m_StartFramel;
			m_bStop = true;
		}
		else
			m_fFrame = m_EndFrame - 1.0f;
	}
	if(m_fColorTime != -1)
	{
		float fRate = fTime * m_fColorTime;
		D3DXColorLerp(&m_CurrentColor,&m_CurrentColor,&m_EndColor,fRate);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param x : ÃèÊö
 * \param y : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CAnimation::Render(int x,int y,bool bUpdateFrame)
{
	if(m_bStop) 
	{
		//OutputDebugString("******************************m_bStop \n");
		return;
	}
	if(!m_pImages) 
	{
		//OutputDebugString("*************sldkfal;sdfkjas;lfkjas;df\n");
		return;
	}
	if(bUpdateFrame)
		UpdateFrame();

	//if(m_bStop) return;
	if((int)m_fFrame  != -1)
	{
		IBitmap* pBmp = m_pImages->GetBitmap((int)m_fFrame);
		assert(pBmp!=NULL);
		if(pBmp) pBmp->Render(x,y,m_CurrentColor,m_BlendType);
	}
	else
	{
		//OutputDebugString("******************************m_fFrame == -1\n");
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param x : ÃèÊö
 * \param y : ÃèÊö
 * \param rcClip : ÃèÊö
 * \param scale : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CAnimation::Render(int x,int y,const RECT* rcClip,const stPointF* scale,bool bUpdateFrame)
{
	if(m_bStop) return;
	if(!m_pImages) return;
	if(bUpdateFrame)
		UpdateFrame();
	//if(m_bStop) return;
	if((int)m_fFrame  != -1)
	{
		IBitmap* pBmp = m_pImages->GetBitmap((int)m_fFrame);
		assert(pBmp!=NULL);
		if( pBmp) pBmp->Render(x,y,rcClip,scale,m_CurrentColor,m_BlendType);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param x : ÃèÊö
 * \param y : ÃèÊö
 * \param color : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CAnimation::Render(int x,int y,DWORD color,bool bUpdateFrame)
{
	if(m_bStop) return;
	if(!m_pImages) return;
	if(bUpdateFrame)
		UpdateFrame();
	//if(m_bStop) return;
	if((int)m_fFrame  != -1)
	{
		IBitmap* pBmp = m_pImages->GetBitmap((int)m_fFrame);
		assert(pBmp!=NULL);
		if( pBmp ) pBmp->Render(x,y,ColorBlend(color,m_CurrentColor),m_BlendType);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param x : ÃèÊö
 * \param y : ÃèÊö
 * \param rcClip : ÃèÊö
 * \param scale : ÃèÊö
 * \param color : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CAnimation::Render(int x,int y,const RECT* rcClip,const stPointF* scale,DWORD color,bool bUpdateFrame)
{
	if(m_bStop) return;
	if(!m_pImages) return;
	if(bUpdateFrame)
		UpdateFrame();
	//if(m_bStop) return;
	if((int)m_fFrame  != -1)
	{
		IBitmap* pBmp = m_pImages->GetBitmap((int)m_fFrame);
		assert(pBmp!=NULL);
		if( pBmp ) pBmp->Render(x,y,rcClip,scale,ColorBlend(color,m_CurrentColor),m_BlendType);
	}
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iFrame : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
IBitmap* CAnimation::GetFrame(int iFrame)
{
	if(!m_pImages) return NULL;
	if(iFrame >= m_pImages->GetFrameCount()) return NULL;
	return m_pImages->GetBitmap(iFrame);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iFrame : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CAnimation::GetWidth(int iFrame)
{
	IBitmap* pBitmap = GetFrame(iFrame);
	if(!pBitmap) return 0;
	return pBitmap->GetWidth();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iFrame : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CAnimation::GetHeight(int iFrame)
{
	IBitmap* pBitmap = GetFrame(iFrame);
	if(!pBitmap) return 0;
	return pBitmap->GetHeight();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param vAni : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
CAnimation& CAnimation::operator = (const CAnimation& vAni)
{
	memcpy(this,&vAni,sizeof(CAnimation));
	return *this;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pt : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
int CAnimation::GetAlpha(POINT pt)
{
	if(!m_pImages) return 0;
	IBitmap* pBmp = m_pImages->GetBitmap(0);
        assert(pBmp != NULL);
	return pBmp && pBmp->GetAlpha(pt);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param iFrame : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CAnimation::IsNullImage(int iFrame)
{
	if(m_pImages == NULL || iFrame >= m_pImages->GetFrameCount()) return true;
	IBitmap * pBmp = m_pImages->GetBitmap(iFrame);
        assert( pBmp != NULL);
	return pBmp && pBmp->IsNull();
}

int CAnimation::GetFrameCount()
{
	return (m_pImages == NULL ? 0 : m_pImages->GetFrameCount());
}

void CAnimation::SetFrame(float iFrame)
{
	m_fFrame = iFrame;
}

float CAnimation::GetFrame()
{
	return m_fFrame;
}