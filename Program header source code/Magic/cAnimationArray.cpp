#include "../engine/include/engine.h"
#include "cAnimationArray.h"
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/**
 * \brief 是否按照排序的方法画这个keepIamge(残象)
 * 
 * 详细描述
 * 
 * \param ptScreen : ptScreen: 屏幕左上角相对世界地图起点的像素偏移,以前命名的这个家伙,为什么不命名成ptMapOffset,搞得这个ptScreen名实不一致
 * \param bOrder :  是否NPC,true代表不是npc，不要排序
 * \return 返回值的描述
 */
void stKeepImage::Draw(POINT ptScreen,bool bOrder)
{
	POINT ptPosition = ptPos;
	ptPosition -= ptScreen;
	BYTE* aOrder = pOrder;

	if(bOrder)
	{
		for (size_t j=0; j<m_aAnimation.size(); j++)
		{
			BYTE ePart = j;
			if (aOrder != NULL)
				ePart = aOrder[j];
			m_aAnimation[ePart]->SetAnimationAlpha(nAlpha);
			m_aAnimation[ePart]->Draw(ptPosition,ptCenter);
		}
	}
	else
	{
		for (size_t j=0; j<m_aAnimation.size(); j++)
		{
			m_aAnimation[j]->SetAnimationAlpha(nAlpha);
			m_aAnimation[j]->Draw(ptPosition,ptCenter);
		}
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param time_eslasped : 描述
 * \param ptPos : 世界像素坐标
 * \param ptCenter : 动画中心位置,相对于自己
 * \param pOrder : 非NPC的Draw次序
 * \return 返回值的描述
 */
void cAnimationArray::KeepImage(long time_eslasped,POINT ptPos,POINT ptCenter,BYTE* pOrder)
{
	if (!m_stKeepImageControl.IsRun())
		return;

	if (m_stKeepImageControl.Loop(time_eslasped))
	{
		stKeepImage* p = new stKeepImage;
		stKeepImage &st = *p;
		st.nAlpha = 255;
		st.ptCenter = ptCenter;
		st.pOrder = pOrder;
		st.ptPos = ptPos;
		for (size_t i=0; i<GetNum(); i++)
		{
			iAnimation* p = GetAnimation(i)->Clone();
			st.m_aAnimation.push_back(p);
		}
		m_listKeepImage.push_back(&st);
	}
}

/**
 * \brief if keepImage的alpha　> 0 ,则画，否则丢
 * 
 * 详细描述
 * 
 * \param ptScreen : 屏幕左上角相对世界地图起点的相对像素偏移
 * \param bOrder : 描述
 * \return 返回值的描述
 */
void cAnimationArray::DrawKeepImage(POINT ptScreen,bool bOrder)
{
	for (list<stKeepImage*>::iterator it = m_listKeepImage.begin(); it != m_listKeepImage.end(); ++it)
	{
		stKeepImage& st = *(*it);
		st.nAlpha -= m_stKeepImageControl.nAlphaDec * ::Engine_GetDrawElapsedTime();
		if (st.nAlpha > 0)
			st.Draw(ptScreen,bOrder);
	}
	if (!m_listKeepImage.empty())
	{
		if (m_listKeepImage.front()->nAlpha <= 0)
		{
			stKeepImage* p = m_listKeepImage.front();
			m_listKeepImage.pop_front();
			delete p;
		}
	}
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
HRESULT cAnimationArray::InvalidateDeviceObjects()
{
	for (size_t i=0; i<m_aAnimation.size(); i++)
		m_aAnimation[i]->InvalidateDeviceObjects();
	FinalRelease();
	return S_OK;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
HRESULT cAnimationArray::RestoreDeviceObjects()
{
	for (size_t i=0; i<m_aAnimation.size(); i++)
		m_aAnimation[i]->RestoreDeviceObjects();
	return S_OK;
}
