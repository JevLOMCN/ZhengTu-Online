/**
 * \file      Thread.cpp
 * \version   $Id$
 * \author    $$$03¹«Ë¾
 *            $$$03¹«Ë¾@###.com
 * \date      2006-7-19  21:11:00
 * \brief     ********************************************************************
 * 
 *	      ********************************************************************
 */

#include "../include/engine.h"
#include "../include/thread.h"

std::map<unsigned int,CThread*> CThread::s_mapThread;
CLock						    CThread::s_Lock;

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CThread* CThread::GetCurrentThread()
{
	DWORD id = GetCurrentThreadId();
	CSafeLock lock(&s_Lock);
	std::map<unsigned int,CThread*>::iterator it = s_mapThread.find(id);
	if(it != s_mapThread.end()) return it->second;
	return NULL;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param pThread : ÃèÊö
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CThread::AppendThread(CThread* pThread,unsigned int id)
{
	CSafeLock lock(&s_Lock);
	s_mapThread[id] = pThread;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param id : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
void CThread::RemoveThread(unsigned int id)
{
	CSafeLock lock(&s_Lock);
	std::map<unsigned int,CThread*>::iterator it = s_mapThread.find(id);
	if(it != s_mapThread.end())
		s_mapThread.erase(it);
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CThread::CThread(void)
: m_bEnd(false)
, m_handle(NULL)
, m_id(0)
{
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
CThread::~CThread(void)
{
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \param startAddress : ÃèÊö
 * \param stack_size : ÃèÊö
 * \param arglist : ÃèÊö
 * \param initflag : ÃèÊö
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CThread::BeginThread(t_start_address startAddress,
				 unsigned stack_size,
				 void *arglist,
				 enumThreadInit initflag)
{
	int iInitFlag = 0;
	if(initflag == ThreadInit_Suspended)
		iInitFlag = CREATE_SUSPENDED;
	m_bEnd = false;
	m_handle = (HANDLE)_beginthreadex(NULL,stack_size,startAddress,arglist,iInitFlag,&m_id);

	if(m_handle)
	{
		AppendThread(this,m_id);
		return true;
	}

	return false;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CThread::ResumeThread()
{
	return ::ResumeThread(m_handle) != 0;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CThread::EndThread()
{
	if(!m_handle) return false;
	m_bEnd = true;
	return true;
}

/**
 * \brief ¼ò¶ÌÃèÊö
 * 
 * ÏêÏ¸ÃèÊö
 * 
 * \return ·µ»ØÖµµÄÃèÊö
 */
bool CThread::WaitEndThread()
{
	if(!m_handle) return false;
	m_bEnd = true;
	WaitForSingleObject(m_handle,-1);
	CloseHandle(m_handle);
	m_handle = NULL;
	return true;
}