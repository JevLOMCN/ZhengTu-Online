#include "public.h"
#include ".\SceneMovie.h"

CSceneMovie::CSceneMovie()
{
	m_pImage = NULL;
	m_dwStartTime = 0;
	m_dwLastTime = 0;
	m_dwColor = 0xFFFFFFFF;
	b_Stop = false;
	m_ptLocation.x = 0;
	m_ptLocation.y = 0;
}

bool CSceneMovie::Create(const char* pszFile, int nGroup, int nFrame, DWORD dwColor )
{
	stResourceLocation rl;
	rl.SetFileName(pszFile);
	rl.group = nGroup;
	rl.frame = nFrame;
	m_pImage = GetDevice()->FindBitmaps(&rl);
	AssertFatal(m_pImage,avar("нд╪Ч%s [%d][%d] error or not exist",rl.szFileName,rl.group,rl.frame));

	if(m_pImage)
		m_pImage->SetAniSpeed(100);
	else
		return false;

	return true;
}

void CSceneMovie::SetTimeLine(DWORD dwStart, DWORD dwLast)
{
	m_dwStartTime = dwStart;
	m_dwLastTime = dwLast;
}

void CSceneMovie::SetLocation(LONG lX, LONG lY)
{
	m_ptLocation.x = lX;
	m_ptLocation.y = lY;
}

bool CSceneMovie::Draw(long lLastTime)
{
	if( m_pImage && lLastTime >= m_dwStartTime )
	{
		DWORD tTime = lLastTime-m_dwStartTime;
		if ( (tTime)/(m_pImage->GetAniInfo()->wdAniSpeed) < m_pImage->GetFrameCount() )
		{
			m_pImage->RenderAni(m_ptLocation.x,m_ptLocation.y,tTime,m_dwColor);
		}
	}
	else
		return false;
	return true;
}

CSceneMovie::~CSceneMovie()
{
}