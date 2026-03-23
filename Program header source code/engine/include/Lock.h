/**
 * \file      Lock.h
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     »¥³âËø
 * 
 *	      »¥³âËø
 */

#pragma once
#include <windows.h>

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class iLock{
public:
	virtual void  Lock() = 0;
	virtual void  Unlock() = 0;
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CLock : public iLock
{
	CRITICAL_SECTION	m_section;
public:
	CLock(void);
	virtual ~CLock(void);
	void Lock();
	void Unlock();
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CMyLock : public iLock
{
	long volatile m_iLockCount;
public:
	CMyLock(void);
	void Lock();
	void Unlock();
};

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 */
class CSafeLock
{
	iLock* m_plock;
public:
	CSafeLock(iLock* plock)
	{
		m_plock = plock;
		m_plock->Lock();
	}

	~CSafeLock()
	{
		m_plock->Unlock();
	}
};