#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CProgressDlg 对话框

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 */
class CProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProgressDlg();

// 对话框数据
	enum { IDD = IDD_PROGRESSDLG };
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_wndProgress;
	CString m_strCaption;
	virtual BOOL OnInitDialog();
	CStatic m_wndCaption;
	afx_msg void OnStnClickedStaticCaption();
};
