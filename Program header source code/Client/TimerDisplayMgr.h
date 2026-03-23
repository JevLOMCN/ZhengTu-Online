#pragma once

enum TIMER_TYPE
{
	TIMERTYPE_SPA, // 温泉系统计时器
};

class CMyTimer;
class CTimerDisplayMgr
{
public:
	CTimerDisplayMgr();
	~CTimerDisplayMgr();

	bool AddTimer(TIMER_TYPE eType, DWORD dwTime);
	void RemoveTimer(TIMER_TYPE eType);
	void ShowTimers(float fElapsedTime);

private:
	void Update();
	void ShowSpaTimer(DWORD dwTime);

private:
	std::map<TIMER_TYPE, CMyTimer*> m_mapTimers;
};

CTimerDisplayMgr& GetTimerDisplayMgr();
