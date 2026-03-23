/**
 * \file      Timer.h
 * \version   $Id$
 * \author    $$$03公司
 *            $$$03公司@###.com
 * \date      2006-7-19  21:11:00
 * \brief     高精度计时器
 * 
 *	      高精度计时器
 */

#pragma once

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CTimer
{
	DWORD m_prevTime;
	DWORD m_currTime;

	void  DisposeAccelerate();

public:
    CTimer();

    void Reset(); // resets the timer
    void Advance(); // advance the timer by 0.1 seconds
    float GetAbsoluteTime(); // get the absolute system time
	float GetElapsedTime(); // get the time that elapsed between GetElapsedTime() calls
	static QWORD GetCpuFrequency();
	static bool isRDTSC();
	DWORD GetAbsoluteTimeMS();
	static void UpdateCpuFrequency();
protected:
	static LARGE_INTEGER sCpuFrequency;
	static bool  sRDTSC;
	LARGE_INTEGER mInitCpuNum;

	union{
		LARGE_INTEGER mPrevCpuNum;
		DWORD mPrevTimer;
	};

	double CPUNumToTimer(QWORD nCpuNum);
};

CTimer* Engine_GetTimer();
CTimer* Engine_GetDrawTimer();
DWORD xtimeGetTime();