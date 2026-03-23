/**
 * \file      Fps.h
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     ********************************************************************
 * 
 *	      ********************************************************************
 */

#pragma once

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CFps
{
	int		m_frames;
	int		m_maxFPS;
	DWORD	m_nBeginTimePreSecond;
	DWORD	m_nBeginTimePreFrame;
	DWORD	m_nFPS;
	int		m_nSleepTime;
	int		m_nTimePreFrame;

	int		m_nCurMaxFPS;
	HANDLE  m_hThread;
public:
	CFps(int maxFPS = 60)
		: m_frames(0)
		, m_nBeginTimePreSecond(0)
		, m_nFPS(0)
		, m_maxFPS(maxFPS)
		, m_nSleepTime(0)
		, m_hThread(::GetCurrentThread())
		, m_nBeginTimePreFrame(0)
	{
		m_nCurMaxFPS    = 30;
		m_nTimePreFrame = 1000/30;
	}

	~CFps(void){}

	void BeginFrame();
	void EndFrame();
	int GetFPS(){ return m_nFPS; }
	int GetLimitFPS(){ return m_nCurMaxFPS;}
	void KeepSmooth();
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
inline CFps* Engine_GetFps()
{
	extern CFps g_theFPS;
	return &g_theFPS;
}