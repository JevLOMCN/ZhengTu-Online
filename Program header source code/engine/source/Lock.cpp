/**
 * \file      Lock.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     »¥³âËø
 * 
 *	      »¥³âËø
 */

#include "../include/engine.h"
#include "../include/Lock.h"
 
/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CLock::CLock(void)
{
	InitializeCriticalSection(&m_section);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CLock::~CLock(void)
{
	DeleteCriticalSection(&m_section);
}


/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CLock::Lock()
{
	EnterCriticalSection(&m_section);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CLock::Unlock()
{
	LeaveCriticalSection(&m_section);
} 

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CMyLock::CMyLock(void)
{
	m_iLockCount = 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CMyLock::Lock()
{
	while(InterlockedCompareExchange(&m_iLockCount,1,0))
	{
	}
	/*
	while(m_iLockCount)
	{
		Sleep(0);
	}
	m_iLockCount++;
	//*/
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CMyLock::Unlock()
{
	assert(m_iLockCount == 1);
	InterlockedExchange(&m_iLockCount,0);
	/*
	m_iLockCount--;
	Sleep(0);
	//*/
}