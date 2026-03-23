#include "public.h"

#include "MyTimer.h"

CMyTimer::CMyTimer()
{
	m_nLeftTime = 0;
	m_dwStartTime = 0;
}

CMyTimer::~CMyTimer()
{

}

bool CMyTimer::Start(int nMS)
{
	bool r = false;
	if (nMS > 0)
	{
		r = true;
		m_nLeftTime = nMS;
		m_dwTotalTime = static_cast<DWORD>(nMS);
		m_dwStartTime = timeGetTime();
	}

	return r;
}

DWORD CMyTimer::GetTotalTime()
{
	return m_dwTotalTime;
}

DWORD CMyTimer::GetLeftTime()
{
	return static_cast<DWORD>(m_nLeftTime);
}

bool CMyTimer::Update()
{
	DWORD dwNowTime = timeGetTime();
	DWORD dwElapseTime = dwNowTime - m_dwStartTime;

	m_dwStartTime = dwNowTime;
	m_nLeftTime -= static_cast<int>(dwElapseTime);
	if (m_nLeftTime <= 0)
	{
		m_nLeftTime = 0;
		return true;
	}

	return false;
}
