#pragma once


// CToolDlg 对话框

class CToolDlg : public CDialog
{
	DECLARE_DYNAMIC(CToolDlg)

public:
	CToolDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CToolDlg();

// 对话框数据
	enum { IDD = IDD_TOOLDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strSrcDir;
	CString m_strDstDir;
	afx_msg void OnBnClickedDds2tgaSrc();
	afx_msg void OnBnClickedDds2tgaDst();
	afx_msg void OnBnClickedDds2tga();
};
