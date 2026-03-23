/**
 * \file      EngineState.h
 * \version   $Id$
 * \author    $$$03π´Àæ
 *            $$$03π´Àæ@###.com
 * \date      2006-7-19  21:11:00
 * \brief     ********************************************************************
 * 
 *	      ********************************************************************
 */

#pragma once

#include "timer.h"

typedef void    (CALLBACK *LPCALLBACKFRAMERUN)( double fTime, float fElapsedTime );
typedef void    (CALLBACK *LPCALLBACKFRAMERENDER)( double fTime, float fElapsedTime );
typedef void	(CALLBACK *LPCALLBACKFRAMELOOP)(void);

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CEngineState
{
public:

	struct STATE{
		float m_Time;                       // current time in seconds
        float m_ElapsedTime;                // time elapsed since last frame
		float m_DrawElapsedTime;
		bool m_bAltDown;
		bool m_bCtrlDown;
		bool m_bShiftDown;
		//HWND	m_hWnd;						
		/*LPCALLBACKFRAMERUN		m_FrameRunFunc;
		LPCALLBACKFRAMERENDER	m_FrameRenderFunc;
		LPCALLBACKFRAMELOOP		m_FrameLoopFunc;*/
	};

	STATE m_state;

	CEngineState(void)
	{
		Create();
	}

	~CEngineState(void)
	{
		Destroy();
	}

	void Create(void)
	{
		memset(&m_state,0,sizeof(m_state));
		Engine_GetTimer()->Reset();
		Engine_GetDrawTimer()->Reset();
	}

	void Destroy(void)
	{
	}

	void  RefreshDraw(){ m_state.m_DrawElapsedTime = Engine_GetDrawTimer()->GetElapsedTime();}

	float GetDrawElapsedTime() { return m_state.m_DrawElapsedTime;}
	//HWND	GetHwnd(){ return m_hWnd; }
	//void	SetHwnd(HWND hWnd) { m_hWnd = hWnd; }

	float	GetTime() { return m_state.m_Time; }
	void	SetTime(float time) { m_state.m_Time = time; }

	float GetElapsedTime() { return m_state.m_ElapsedTime; }
	void SetElapsedTime(float time) { m_state.m_ElapsedTime = time; }

	/*LPCALLBACKFRAMERUN GetFrameRunFunc(){ return m_state.m_FrameRunFunc; }
	void	SetFrameRunFunc(LPCALLBACKFRAMERUN func){ m_state.m_FrameRunFunc = func; }

	LPCALLBACKFRAMERENDER GetFrameRunderFunc() { return m_state.m_FrameRenderFunc; }
	void	SetFrameRenderFunc(LPCALLBACKFRAMERENDER func) { m_state.m_FrameRenderFunc = func; }

	LPCALLBACKFRAMELOOP GetFrameLoopFunc() { return m_state.m_FrameLoopFunc; }
	void	SetFrameLoopFunc(LPCALLBACKFRAMELOOP func) { m_state.m_FrameLoopFunc = func; }*/
	
	bool IsAltDown() { return m_state.m_bAltDown;}
	bool IsCtrlDown(){ return m_state.m_bCtrlDown;}
	bool IsShiftDown(){ return m_state.m_bShiftDown;}

	void Refresh()
	{
		m_state.m_bAltDown = GetKeyState(VK_MENU) < 0;
		m_state.m_bCtrlDown = GetKeyState(VK_CONTROL) < 0;
		m_state.m_bShiftDown = GetKeyState(VK_SHIFT) < 0;

		SetTime(Engine_GetTimer()->GetAbsoluteTime());
		SetElapsedTime((float)Engine_GetTimer()->GetElapsedTime());
	}
};

/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 * \return ∑µªÿ÷µµƒ√Ë ˆ
 */
inline CEngineState* Engine_GetState()
{
	extern CEngineState g_EngineState;
	return & g_EngineState;
}
