// MakeExeDlg.h : 头文件
//

#pragma once
#include <vector>
#include "afxwin.h"


// CMakeExeDlg 对话框
class CMakeExeDlg : public CDialog
{
	std::vector<std::string>  m_fileList;

	void Recurse(LPCTSTR pstr);
// 构造
public:
	CMakeExeDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MAKEEXE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	CString m_strSrcPath;
	CString m_strDesPath;
	CString m_strVersion;
	CListBox m_listboxOfFileList;
	afx_msg void OnBnClickedButtonSelectFile();
};
