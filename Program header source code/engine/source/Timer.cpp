/**
 * \file      Timer.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     ¸ß¾«¶È¼ÆÊ±Æ÷
 * 
 *	      ¸ß¾«¶È¼ÆÊ±Æ÷
 */

#include "../include/engine.h"
#include "../include/timer.h"


LARGE_INTEGER CTimer::sCpuFrequency = {0};
bool CTimer::sRDTSC = false;

inline QWORD GetCurCPUNum()
{
	QWORD num;
	_asm{
		RDTSC
		mov DWORD PTR num ,		eax
		mov DWORD PTR [num+4] , edx
	}
	return num;
}

#pragma optimize( "", off )

static QWORD GetFrequency(DWORD div = 1)
{

	LARGE_INTEGER CurrTIcks ,TicksCount;
	QWORD iStartCounter,iStopCounter;

	DWORD dwOldProcessP = GetPriorityClass(GetCurrentProcess());
	DWORD dwOldThreadP = GetThreadPriority(GetCurrentThread());

	SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentProcess(),THREAD_PRIORITY_TIME_CRITICAL);

	for(int i=0; i<50;++i)
		GetCurCPUNum();

	time_t tt1,tt2;

	time(&tt1);
	QueryPerformanceFrequency(&TicksCount);
	QueryPerformanceCounter(&CurrTIcks);
	
	TicksCount.QuadPart /= div;
	TicksCount.QuadPart += CurrTIcks.QuadPart;

	iStartCounter = GetCurCPUNum();

	while(CurrTIcks.QuadPart < TicksCount.QuadPart)
	{
		QueryPerformanceCounter(&CurrTIcks);
	}

	iStopCounter = GetCurCPUNum();
	time(&tt2);

	SetThreadPriority(GetCurrentProcess(),dwOldThreadP);
	SetPriorityClass(GetCurrentProcess(),dwOldProcessP);
	if(div >= 1)
	{
		double dt = difftime(tt2,tt1);
		if(dt < 1.0) exit(0);
	}

	return (iStopCounter - iStartCounter) * div;
}
#pragma optimize( "", on )


/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
CTimer* Engine_GetTimer()
{
	static CTimer g_EngineTimer;
	return &g_EngineTimer;
}

/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
CTimer* Engine_GetDrawTimer()
{
	static CTimer g_EngineDrawTimer;
	return &g_EngineDrawTimer;
}

static bool s_bInitUpdateCpuFrequency = false;
static time_t s_prevTime;
static LARGE_INTEGER s_prevCpuNum;
static LARGE_INTEGER s_prevCounter;
static LARGE_INTEGER s_curFrequency;
void CTimer::UpdateCpuFrequency()
{
	if(!s_bInitUpdateCpuFrequency)
	{
		s_bInitUpdateCpuFrequency = true;
		time(&s_prevTime);
		s_prevCpuNum.QuadPart = GetCurCPUNum();
		QueryPerformanceFrequency(&s_curFrequency);
		QueryPerformanceCounter(&s_prevCounter);
	}
	else
	{
		LARGE_INTEGER curCounter;
		LARGE_INTEGER curCpuNum;
		time_t curTime;
		double diffTime;

		time(&curTime);
		QueryPerformanceCounter(&curCounter);
		curCpuNum.QuadPart = GetCurCPUNum();
		diffTime = (double)(curCounter.QuadPart - s_prevCounter.QuadPart)/(double)s_curFrequency.QuadPart;
		if( diffTime - 0.1 < difftime(curTime,s_prevTime) )
		{
			sCpuFrequency.QuadPart = (LONGLONG)((double)(curCpuNum.QuadPart - s_prevCpuNum.QuadPart) / diffTime);
		
		}
		else
		{
#if defined _DEBUG || defined RELEASE_TEST
			Engine_WriteLog("¿Í»§¶Ë¼ÓËÙÁË£¡\n");
#endif		
		}
		

		s_prevTime = curTime;
		s_prevCounter = curCounter;
		s_prevCpuNum = curCpuNum;

#if defined _DEBUG || defined RELEASE_TEST
		Engine_WriteLogF("Current cpu frequency %I64u\n",s_curFrequency.QuadPart);
#endif

	}
}

//--------------------------------------------------------------------------------------
CTimer::CTimer()
{

	if( sCpuFrequency.QuadPart == 0 )
	{
		sRDTSC = 0 != IsProcessorFeaturePresent(PF_RDTSC_INSTRUCTION_AVAILABLE);
		if( sRDTSC )
		{
			//sCpuFrequency.QuadPart = ::GetFrequency();
			QueryPerformanceFrequency(&sCpuFrequency);
		}

#if defined _DEBUG || defined RELEASE_TEST
		Engine_WriteLogF("CpuFrequency£º%I64u\n",sCpuFrequency.QuadPart);
#endif

	}

	Reset();

}


/**
* \brief ¼ò¶ÌÃèÊö
* 
* ÏêÏ¸ÃèÊö
* 
* \return ·µ»ØÖµµÄÃèÊö
*/
//--------------------------------------------------------------------------------------
void CTimer::Reset()
{
	if(sRDTSC)
	{
		QueryPerformanceCounter(&mInitCpuNum);
		//mInitCpuNum.QuadPart = GetCurCPUNum();
		mPrevCpuNum = mInitCpuNum;
	}
	else
	{
		mPrevTimer = timeGetTime();
	}
}



/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
float CTimer::GetAbsoluteTime()
{
	if(sRDTSC)
	{
		LARGE_INTEGER curCpuNum;
		QueryPerformanceCounter(&curCpuNum);
		//curCpuNum.QuadPart = GetCurCPUNum();
		return (float)((double)(curCpuNum.QuadPart - mInitCpuNum.QuadPart)/(double)sCpuFrequency.QuadPart);
	}
	return (float)(timeGetTime() / 1000.0);
}

DWORD CTimer::GetAbsoluteTimeMS()
{
	if(sRDTSC)
	{
		LARGE_INTEGER curCpuNum;
		QueryPerformanceCounter(&curCpuNum);
		//curCpuNum.QuadPart = GetCurCPUNum();
		m_currTime = 1000 * (DWORD)(double(curCpuNum.QuadPart - mInitCpuNum.QuadPart)/(double)sCpuFrequency.QuadPart);
		return m_currTime;
	}
	m_currTime = timeGetTime();
	return m_currTime;
}

void  CTimer::DisposeAccelerate()
{
	Assert(0);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
//--------------------------------------------------------------------------------------
float CTimer::GetElapsedTime()
{
	if(sRDTSC)
	{
		LARGE_INTEGER curCpuNum;
		QueryPerformanceCounter(&curCpuNum);
		//curCpuNum.QuadPart = GetCurCPUNum();
		float f = (float)((double)(curCpuNum.QuadPart - mPrevCpuNum.QuadPart)/(double)GetCpuFrequency());
		mPrevCpuNum = curCpuNum;
		return f;
	}
	else
	{
		DWORD curTimer = timeGetTime();
		float f = (curTimer - mPrevTimer)/1000.0f;
		mPrevTimer = curTimer;
		return f;
	}
}

QWORD CTimer::GetCpuFrequency()
{
	return sCpuFrequency.QuadPart;
}

bool CTimer::isRDTSC()
{
	return sRDTSC;
}

DWORD xtimeGetTime()
{
	return timeGetTime();
	//return Engine_GetTimer()->GetAbsoluteTimeMS();
}