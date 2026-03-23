#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "CJColorPicker.h"


// COptionFormView 窗体视图

class COptionFormView : public CFormView
{
	DECLARE_DYNCREATE(COptionFormView)

protected:
	COptionFormView();           // 动态创建所使用的受保护的构造函数
	virtual ~COptionFormView();

public:
	enum { IDD = IDD_OPTIONFORMVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

static	void MergeTga(const CString & strFile1,const CString & strFile2,const CString & strDstFile,UINT r,UINT g,UINT b);
	afx_msg void OnBnClickedButtonMergeTga();
	BOOL m_bLook;
	CListBox m_listbox;
	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedButton1();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButton3();
	CString m_strPath1;
	CString m_strPath2;
	CString m_strDst;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButtonStartSinglePic();
	afx_msg void OnDestroy();
	afx_msg LONG OnSelEndOK(UINT lParam, LONG wParam);

	// Help functions 
private:
	BOOL  GetAFilePath(CString & strPath);
	void  Output(LPCTSTR lpszFmt,...);
public:
	CCJColorPicker m_btnColor;
	UINT m_iR;
	UINT m_iG;
	UINT m_iB;
	CEdit m_wndOutput;
};


