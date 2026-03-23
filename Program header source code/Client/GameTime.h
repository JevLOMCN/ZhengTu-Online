#pragma once

struct stGameDateTime{
	DWORD tm_year;
	DWORD tm_mon;
	DWORD tm_mday;
	DWORD tm_hour;
	DWORD tm_min;
	DWORD tm_sec;
};

struct stGameTimeTimerUserCmd;
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CGameTime
{
	uint64_t m_qwServerTime;
	uint64_t m_qwLastConfirmTimer;
	DWORD m_dwTimer;
	DWORD m_nTimeInit;
	
public:
	stGameDateTime m_time;
	QWORD		   m_qwSecond;
	bool           m_bConnWay;
	virtual const char* GetZSTimeString();
	virtual const char* GetZTTimeString();
	CGameTime(void);
	~CGameTime(void);
	virtual DWORD GetTimestamp();
	virtual void  InitTimestamp();
	virtual void  SetGameTime(stGameTimeTimerUserCmd* pCmd);
	virtual void  Confirm();
	virtual void  Run();
	virtual int	  GetDayNum();
	virtual bool  IsDayNumChanged();
};

extern const char* GetTimeString(QWORD dwSecond);
extern void timeGetGameTime(stGameDateTime * pRet,QWORD qwSecond/*系统时间(服务器运行了多少秒)*/);
extern QWORD timeGetTimeSecond(stGameDateTime * dt);
extern void timeGetTimeData(stGameDateTime * pRet,QWORD qwSecond/*(秒)*/);

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
inline CGameTime * GetGameTime()
{
	extern CGameTime g_GameTime;
	return &g_GameTime;
}