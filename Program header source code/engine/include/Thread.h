/**
 * \file      Thread.h
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
class CThread
{
	HANDLE m_handle;
	unsigned int m_id;
	bool m_bEnd;
	static std::map<unsigned int,CThread*> s_mapThread;
	static CLock						   s_Lock;
	static void AppendThread(CThread* pThread,unsigned int id);
	static void RemoveThread(unsigned int id);

public:
	static CThread* GetCurrentThread();

	typedef unsigned int ( __stdcall *t_start_address )( void * );
	enum enumThreadInit{
		ThreadInit_Normal,
		ThreadInit_Suspended,
	};

	CThread(void);
	~CThread(void);
	bool BeginThread(t_start_address startAddress,
		unsigned stack_size = 0,
		void *arglist = NULL,
		enumThreadInit initflag = ThreadInit_Normal);
	bool ResumeThread();
	bool EndThread();
	bool WaitEndThread();
	bool IsEndThread(){ return m_bEnd; }
	HANDLE GetHandle(){ return m_handle;}
};
