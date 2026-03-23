#pragma once
#include "progressdlg.h"



// CLoadThread

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CLoadThread : public CWinThread
{
	DECLARE_DYNCREATE(CLoadThread)

protected:
	CLoadThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CLoadThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	BOOL m_bInit;
protected:
	DECLARE_MESSAGE_MAP()
public:
	CProgressDlg m_wndProgress;
	void SetRange(int nLower,int nUpper)
	{
		m_wndProgress.m_wndProgress.SetRange32(nLower,nUpper);
	}
	void SetPos(int pos)
	{
		m_wndProgress.m_wndProgress.SetPos(pos);
	}
	void SetCaption(LPCTSTR lpszMsg)
	{
		//m_wndProgress.m_strCaption = lpszMsg;
		//m_wndProgress.UpdateData(FALSE);
		m_wndProgress.m_wndCaption.SetWindowText(lpszMsg);
	}

	void HideProcessWnd();
};


