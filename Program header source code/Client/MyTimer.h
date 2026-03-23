#pragma once

class CMyTimer
{
public:
	CMyTimer();
	~CMyTimer();

	bool Start(int nMS);
	DWORD GetTotalTime();
	DWORD GetLeftTime();
	bool Update();

private:
	int m_nLeftTime;
	DWORD m_dwTotalTime;
	DWORD m_dwStartTime;
};