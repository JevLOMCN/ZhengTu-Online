#include "public.h"
#include "miniusercommand.h"
#include ".\lordstrikecard.h"

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
CCard::CCard()
: m_pImage(NULL)
, m_CurrentColor(1,1,1,1)
, m_BlendType(Blend_Null)
, m_bSelected(false)
{
}

CCard::~CCard()
{
}

void CCard::SetCardID(int nCard)
{
	// 图片编号规律: 100 *　花色　＋  牌大小(1~15)
	// 花色: 
	// 0:黑桃
	// 1:红心
	// 2:梅花
	// 3:方块
	m_nCardID = nCard;
	stResourceLocation rl;
	rl.SetFileName("data\\interfaces.gl");
	rl.group = 134;
	rl.frame = m_nCardID;
	m_pImage = GetDevice()->FindBitmap(&rl);
}


void CCard::SetResource(const stResourceLocation &rl)
{
	m_rl = rl;
	IBitmaps * pBmps = GetDevice()->FindBitmaps(&rl);
	if(pBmps)
	{
             m_pImage = pBmps->GetBitmap(0);
	}
}

void CCard::Render(int x,int y)
{
	if(!m_pImage)
		return;
	m_pImage->Render(x,y,m_CurrentColor,m_BlendType);
}

void CCard::Render(int x,int y,const RECT* rcClip,const stPointF* scale)
{
	if(!m_pImage) 
		return;
	m_pImage->Render(x,y,rcClip,scale,m_CurrentColor,m_BlendType);
}

void CCard::Render(int x,int y,DWORD color)
{
	if(!m_pImage) 
		return;
	m_pImage->Render(x,y,ColorBlend(color,m_CurrentColor),m_BlendType);
}

void CCard::Render(int x,int y,const RECT* rcClip,const stPointF* scale,DWORD color)
{
	if(!m_pImage) 
		return;
	m_pImage->Render(x,y,rcClip,scale,ColorBlend(color,m_CurrentColor),m_BlendType);
}

int CCard::GetWidth()
{
	if(!m_pImage) 
		return 0;
	return m_pImage->GetWidth();
}

int CCard::GetHeight()
{
	if(!m_pImage) 
		return 0;
	return m_pImage->GetHeight();
}

CCard& CCard::operator = (const CCard& vCard)
{
	memcpy(this,&vCard,sizeof(CCard));
	return *this;
}

int CCard::GetAlpha(POINT pt)
{
	if(!m_pImage) return 0;
	return m_pImage->GetAlpha(pt);
}

bool CCard::IsNullImage()
{
	if(m_pImage == NULL) return true;
	return m_pImage->IsNull();
}

