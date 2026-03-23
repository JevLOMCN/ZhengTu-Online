#pragma once


// CDlgSaveToTga 对话框

class CDlgSaveToTga : public CDialog
{
	DECLARE_DYNAMIC(CDlgSaveToTga)

public:
	CDlgSaveToTga(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSaveToTga();

// 对话框数据
	enum { IDD = IDD_DLGSAVETOTGA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strDstPath;
	BOOL m_bObject;
	afx_msg void OnBnClickedSetForder();
};
