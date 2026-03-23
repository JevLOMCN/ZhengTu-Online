// MakeExeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MakeExe.h"
#include "MakeExeDlg.h"
#include ".\makeexedlg.h"
#include "./CJShell.h"
#include "./KillProcessDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMakeExeDlg 对话框


void CMakeExeDlg::Recurse(LPCTSTR pstr)
{
	
	CFileFind finder;

	m_fileList.clear();

	// build a string with wildcards
	CString strWildcard(pstr);
	strWildcard += _T("\\*.*");

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!

		if (finder.IsDots())
			continue;

		// if it's a directory, recursively search it

		if (finder.IsDirectory()==false)
		{
			CString str = finder.GetFilePath();
			m_fileList.push_back(str.GetString());
		}
	}

	finder.Close();
}


CMakeExeDlg::CMakeExeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMakeExeDlg::IDD, pParent)
	, m_strSrcPath(_T("..\\exe"))
	, m_strDesPath(_T("..\\dat"))
	, m_strVersion(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	
}

void CMakeExeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strSrcPath);
	DDX_Text(pDX, IDC_EDIT2, m_strDesPath);
	DDX_Text(pDX, IDC_EDIT3, m_strVersion);
	DDV_MaxChars(pDX, m_strVersion, VERSION_HEADER_LEN);
	DDX_Control(pDX, IDC_LIST_ZTFF_FILELIST, m_listboxOfFileList);
}

BEGIN_MESSAGE_MAP(CMakeExeDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_FILE, OnBnClickedButtonSelectFile)
END_MESSAGE_MAP()


// CMakeExeDlg 消息处理程序

BOOL CMakeExeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMakeExeDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CMakeExeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMakeExeDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	int nFileNameNums = m_listboxOfFileList.GetCount();
	if(nFileNameNums <= 0 )
	{
	      AfxMessageBox("请选择关键的要检测的文件名");
	      return;
	}

	UpdateData(true);
	
	Recurse(m_strSrcPath);

	m_strDesPath += FILENAME;

	::DeleteFile(m_strDesPath);

	FILE *file;

	if ((file=fopen(m_strDesPath,"wb"))==NULL)
	{
		goto err;
	}


	stFileHeader FileHeader;
	memset(&FileHeader,0,sizeof(stFileHeader));
	strncpy(FileHeader.szVersion,m_strVersion,VERSION_HEADER_LEN);
       
	int nFileNameLen =  0; 
	for(size_t iListbox = 0; iListbox < nFileNameNums; iListbox++)
	{
                CString strText;          
		m_listboxOfFileList.GetText(iListbox,strText);

		nFileNameLen +=strlen(PathFindFileName(strText));
		nFileNameLen +=1;  // ';' 
	}

	FileHeader.nLenOfFileList =  nFileNameLen+1;  // strcat(";") 导致+1

	// not need nFileNameLen+1,because just some binary,not string 
	char * pFileNameList = new char[nFileNameLen+1];
	if(!pFileNameList)  
	{
		AfxMessageBox("内存分配失败!");
		return;
	}
	memset(pFileNameList,0,nFileNameLen+1);
	for(iListbox = 0 ; iListbox < nFileNameNums; iListbox++)
	{
		CString strText;
		m_listboxOfFileList.GetText(iListbox,strText);
		CString strTe = PathFindFileName(strText)  ;
		strcat(pFileNameList,PathFindFileName(strText));
		strcat(pFileNameList,";");
	}
	
        

	int __count = fwrite((void*)&FileHeader,sizeof(char),sizeof(FileHeader),file);
	if (__count  !=  sizeof(FileHeader))
	{
		CString msg="写版本号时发生错误";
		AfxMessageBox(msg);
		return;
	}

	__count = fwrite((void*)pFileNameList,sizeof(char),(nFileNameLen+1)*sizeof(char),file);
	if(__count != (nFileNameLen+1)*sizeof(char))
	{
		CString msg="写文件名出错";
		AfxMessageBox(msg);
		return;
	}

	for (int i=0;i<m_fileList.size();i++)
	{
		FILE *fl;
		byte data[READCOUNT]={0};

		std::string strTest = m_fileList[i];
		if ((fl=fopen(m_fileList[i].c_str(),"rb"))!=NULL)
		{
			if (fseek(fl,READOFFSET,SEEK_SET) == 0)
			{
				int count = fread(data,sizeof(byte),READCOUNT,fl);
				if (count==READCOUNT)
				{
					stKillProcess st={0};
					strncpy(st.szExeFile,PathFindFileName(m_fileList[i].c_str()),sizeof(st.szExeFile));
					memcpy(st.data,data,sizeof(st.data));
					count = fwrite((void*)&st,sizeof(byte),sizeof(stKillProcess),file);
					if (count!=sizeof(stKillProcess))
					{
						CString msg="发生错误：";
						AfxMessageBox(msg + m_fileList[i].c_str());

					}
				}

			}

			fclose(fl);
		}
		
	}

	fclose(file);
	
	AfxMessageBox("文件生成成功！");

err:

	OnOK();

	delete []pFileNameList;
}

void CMakeExeDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	
	OnCancel();
}

void CMakeExeDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CCJShell dlg;
	UpdateData();
	dlg.m_strTitle = _T("外挂，内挂程序所在的目录");
	dlg.m_strSelDir = m_strSrcPath;
	if(dlg.BrowseForFolder(this))
	{
		m_strSrcPath = dlg.m_strPath;
		UpdateData(FALSE);
	}
}

void CMakeExeDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CCJShell dlg;
	UpdateData();
	dlg.m_strTitle = _T("生成文件的目录");
	dlg.m_strSelDir = m_strDesPath;
	if(dlg.BrowseForFolder(this))
	{
		m_strDesPath = dlg.m_strPath;
		UpdateData(FALSE);
	}
}

void CMakeExeDlg::OnBnClickedButtonSelectFile()
{
	// TODO: 在此添加控件通知处理程序代码
	// szFilters is a text string that includes two file name filters:
	// "*.my" for "MyType Files" and "*.*' for "All Files."
	char szFilters[]=
		"Data Files (*.dat)|*.dat|DLL Files (*.dll)|*.dll||";


	// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg (TRUE, "dat", "*.dat",
		OFN_FILEMUSTEXIST| OFN_ALLOWMULTISELECT, szFilters, this);

	fileDlg.m_ofn.lpstrInitialDir =  m_strSrcPath;
	// Display the file dialog. When user clicks OK, fileDlg.DoModal() 
	// returns IDOK.
	if( fileDlg.DoModal ()==IDOK )
	{
		POSITION ps = fileDlg.GetStartPosition();
		while(ps)
		{
			CString strPath = fileDlg.GetNextPathName(ps);
			m_listboxOfFileList.AddString(strPath);
		}		
	}

	
}
