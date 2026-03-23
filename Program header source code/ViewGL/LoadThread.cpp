// LoadThread.cpp : 实现文件
//

#include "stdafx.h"
#include "ViewGL.h"
#include "LoadThread.h"
#include "ProgressDlg.h"

// CLoadThread

IMPLEMENT_DYNCREATE(CLoadThread, CWinThread)

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CLoadThread::CLoadThread()
{
	m_bInit = FALSE;
	this->m_bAutoDelete = TRUE;
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CLoadThread::~CLoadThread()
{
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
BOOL CLoadThread::InitInstance()
{
	// TODO: 在此执行任意逐线程初始化
	CWnd wnd;
	m_wndProgress.Create(IDD_PROGRESSDLG,&wnd);
	m_pMainWnd = &m_wndProgress;
	m_wndProgress.ShowWindow(SW_SHOW);
	m_wndProgress.UpdateWindow();
	m_wndProgress.SetForegroundWindow();
	m_bInit = TRUE;
	return TRUE;
}

void CLoadThread::HideProcessWnd()
{
	m_wndProgress.m_wndProgress.ShowWindow(SW_HIDE);
}
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
int CLoadThread::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CLoadThread, CWinThread)
END_MESSAGE_MAP()


// CLoadThread 消息处理程序

