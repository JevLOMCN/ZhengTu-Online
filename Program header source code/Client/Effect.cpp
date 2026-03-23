#include "public.h"

#include "EffectManager.h"

#include "Effect.h"

stEffectInfo::stEffectInfo()
{
	pBitmaps = NULL;
	nOffsetX = 0;
	nOffsetY = 0;
}

CEffect::CEffect(const std::string& strName) : m_strName(strName)
{
	FUNCTION_BEGIN;

	if (!GetEffectManager().GetEffect(strName, m_stEffectInfo))
	{
		std::string strLog = "effect.xml中没有名为" + strName + "的特效！\n";
		Engine_WriteLog(strLog.c_str());
	}

	FUNCTION_END;
}

CEffect::~CEffect()
{

}

void CEffect::Render(const stPointI& pos)
{
	if (GetEffectManager().GetEffect(m_strName, m_stEffectInfo))
	{
		m_stEffectInfo.pBitmaps = GetDevice()->FindBitmaps(&m_stEffectInfo.stResourceLocation);
		if(m_stEffectInfo.pBitmaps && m_stEffectInfo.pBitmaps->GetBitmapCount())
		{
			m_stEffectInfo.pBitmaps->RenderAni(pos.x + m_stEffectInfo.nOffsetX, pos.y + m_stEffectInfo.nOffsetY, xtimeGetTime(), 0xffffffff);
		}
	}
}
