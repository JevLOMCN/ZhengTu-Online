/**
 * \file      Fps.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     ********************************************************************
 * 
 *	      ********************************************************************
 */

#include "../include/engine.h"
#include "../include/fps.h"
#include "../Include/Timer.h"

CFps g_theFPS(60);
bool gEngineLimitFPS = true;
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CFps::KeepSmooth()
{
	if( m_nSleepTime > m_nTimePreFrame * 5 )
	{
		// Ìá¸ßÖ¡Êý
		if(m_nCurMaxFPS < m_maxFPS)
		{
			m_nCurMaxFPS++;
			m_nTimePreFrame = 1000/m_nCurMaxFPS;
		}
	}
	else if( m_nCurMaxFPS > 30 && m_nSleepTime <  m_nTimePreFrame * (-2) )
	{
		// ¼õÉÙÖ¡Êý
		m_nCurMaxFPS--;
		m_nTimePreFrame = 1000/m_nCurMaxFPS;
	}
	m_nSleepTime = 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CFps::BeginFrame()
{
	m_nBeginTimePreFrame  = xtimeGetTime();
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CFps::EndFrame()
{
	DWORD	nTime = xtimeGetTime();
	int		nElapsedTime = nTime - m_nBeginTimePreFrame;
	int		nSleepTime = m_nTimePreFrame - nElapsedTime;

#if defined _DEBUG || defined RELEASE_TEST
	if(gEngineLimitFPS)
#else
		if(1)
#endif
	{
		while(nElapsedTime < m_nTimePreFrame){
			Sleep(1);
			nTime = xtimeGetTime();
			nElapsedTime = nTime - m_nBeginTimePreFrame;
		}
	}
	m_nSleepTime += nSleepTime;

	m_frames++;
	if( nTime - m_nBeginTimePreSecond > 1000 )
	{
		m_nFPS = m_frames;
		m_nBeginTimePreSecond = nTime;
		m_frames=0;
		KeepSmooth();
	}
}