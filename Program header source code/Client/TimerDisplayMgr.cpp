#include "public.h"

#include <sstream>
#include "MyTimer.h"
#include "GameScene.h"

#include "TimerDisplayMgr.h"

namespace
{
	const DWORD MS_PER_SECOND = 1000; // 每秒的毫秒数
	const DWORD MS_PER_MINUTE = 60 * MS_PER_SECOND; // 每分的毫秒数
	const DWORD MS_PER_HOUR = 60 * 60 * MS_PER_SECOND; // 每时的毫秒数
}

CTimerDisplayMgr::CTimerDisplayMgr()
{

}

CTimerDisplayMgr::~CTimerDisplayMgr()
{
	std::map<TIMER_TYPE, CMyTimer*>::iterator iter;
	for (iter = m_mapTimers.begin(); iter != m_mapTimers.end(); ++iter)
	{
		SAFE_DELETE(iter->second);
	}
	m_mapTimers.clear();
}

bool CTimerDisplayMgr::AddTimer(TIMER_TYPE eType, DWORD dwTime)
{
	bool r = false;
	CMyTimer* pTimer = new CMyTimer;
	if (pTimer)
	{
		r = pTimer->Start(dwTime);
		if (r)
		{
			m_mapTimers.insert(std::make_pair(eType, pTimer));
		}
	}

	return r;
}

void CTimerDisplayMgr::RemoveTimer(TIMER_TYPE eType)
{
	m_mapTimers.erase(eType);
}

void CTimerDisplayMgr::ShowTimers(float /*fElapsedTime*/)
{
	this->Update();

	std::map<TIMER_TYPE, CMyTimer*>::iterator iter;
	for (iter = m_mapTimers.begin(); iter != m_mapTimers.end(); ++iter)
	{
		switch (iter->first)
		{
		case TIMERTYPE_SPA:
			{
				this->ShowSpaTimer(iter->second->GetLeftTime());
			}
			break;
		}
	}
}

void CTimerDisplayMgr::Update()
{
	std::map<TIMER_TYPE, CMyTimer*>::iterator iter;
	for (iter = m_mapTimers.begin(); iter != m_mapTimers.end();)
	{
		if (iter->second)
		{
			if (iter->second->Update())
			{
				SAFE_DELETE(iter->second);
				m_mapTimers.erase(iter++);
			}
			else
			{
				 ++iter;
			}
		}
	}
}

void CTimerDisplayMgr::ShowSpaTimer(DWORD dwTime)
{
	DWORD dwHours = (dwTime / MS_PER_HOUR) % 24; //soke 小时
	
	DWORD dwMins = (dwTime / MS_PER_MINUTE) % 60; //soke 分钟
	
	DWORD dwSecs = (dwTime / MS_PER_SECOND) % 60; //soke 秒钟

//	dwTime -= (dwHours*MS_PER_HOUR);
    dwTime -= (dwMins*MS_PER_MINUTE);

	std::stringstream ss;
	ss << "温泉剩余时间:" << dwHours << "时" << dwMins << "分" << dwSecs << "秒";
	POINT pos;
	pos.x = static_cast<int>(GetScene()->GetScreenPixelWidth() * 0.62f);
	pos.y = static_cast<int>(GetScene()->GetScreenPixelHeight() * 0.1f);

	int iFont = GetDevice()->SetFont(1);
	GetDevice()->DrawString(ss.str().c_str(), pos, 0xff00ff00);
	GetDevice()->SetFont(iFont);
}

CTimerDisplayMgr& GetTimerDisplayMgr()
{
	static CTimerDisplayMgr mgr;
	return mgr;
}
