// ProgressDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ViewGL.h"
#include "ProgressDlg.h"
#include ".\progressdlg.h"


// CProgressDlg 对话框

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
IMPLEMENT_DYNAMIC(CProgressDlg, CDialog)
CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
	, m_strCaption(_T(""))
{
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
CProgressDlg::~CProgressDlg()
{
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pDX : 描述
 * \return 返回值的描述
 */
void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_wndProgress);
	DDX_Text(pDX, IDC_STATIC_CAPTION, m_strCaption);
	DDX_Control(pDX, IDC_STATIC_CAPTION, m_wndCaption);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	ON_STN_CLICKED(IDC_STATIC_CAPTION, OnStnClickedStaticCaption)
END_MESSAGE_MAP()


// CProgressDlg 消息处理程序

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
BOOL CProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rect;
	GetWindowRect(&rect);
	SetWindowPos(NULL,(GetSystemMetrics(SM_CXSCREEN)-rect.Width())/2,(GetSystemMetrics(SM_CYSCREEN)-rect.Height())/2,0,0,SWP_NOZORDER|SWP_NOSIZE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
void CProgressDlg::OnStnClickedStaticCaption()
{
	// TODO: 在此添加控件通知处理程序代码
}
