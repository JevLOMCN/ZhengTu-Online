// DlgSaveToTga.cpp : 实现文件
//

#include "stdafx.h"
#include "ViewGL.h"
#include "DlgSaveToTga.h"
#include ".\dlgsavetotga.h"

#include "CJShell.h"
// CDlgSaveToTga 对话框

IMPLEMENT_DYNAMIC(CDlgSaveToTga, CDialog)
CDlgSaveToTga::CDlgSaveToTga(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSaveToTga::IDD, pParent)
	, m_strDstPath(_T(""))
	, m_bObject(FALSE)
{
}

CDlgSaveToTga::~CDlgSaveToTga()
{
}

void CDlgSaveToTga::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strDstPath);
	DDX_Check(pDX, IDC_CHECK1, m_bObject);
}


BEGIN_MESSAGE_MAP(CDlgSaveToTga, CDialog)
	ON_BN_CLICKED(IDC_SET_FORDER, OnBnClickedSetForder)
END_MESSAGE_MAP()


// CDlgSaveToTga 消息处理程序

void CDlgSaveToTga::OnBnClickedSetForder()
{
	// TODO: 在此添加控件通知处理程序代码
	CCJShell dlg;
	UpdateData(TRUE);
	dlg.m_strTitle = _T("请选择目标目录");
	dlg.m_strSelDir = m_strDstPath;
	if(dlg.BrowseForFolder(AfxGetMainWnd()))
	{
		m_strDstPath = dlg.m_strPath;
		UpdateData(FALSE);
	}
}
