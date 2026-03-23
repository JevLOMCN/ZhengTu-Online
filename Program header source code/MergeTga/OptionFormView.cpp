// OptionFormView.cpp : 实现文件
//

#include "stdafx.h"
#include "MergeTga.h"
#include "OptionFormView.h"
#include ".\optionformview.h"
#include "CJShell.h"

#include "MergeTgaDoc.h"
#include <vector>
#include <string>
using namespace std;

#include "../xml_parse/tinyxml.h"


#include "TgaCore.h"
#define COLOR_ARGB( a, r, g, b )	( (DWORD)( ( ((a) & 0xff) << 24 ) | ( ((r) & 0xff) << 16 ) | ( ((g) & 0xff)<< 8  ) | ( ((b) & 0xff)  ) ) )
// COptionFormView

IMPLEMENT_DYNCREATE(COptionFormView, CFormView)

COptionFormView::COptionFormView()
	: CFormView(COptionFormView::IDD)
	, m_bLook(FALSE)
	, m_strPath1(_T(""))
	, m_strPath2(_T(""))
	, m_strDst(_T(""))
	, m_iR(0)
	, m_iG(0)
	, m_iB(0)
{
}

COptionFormView::~COptionFormView()
{
}

void COptionFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_bLook);
	DDX_Control(pDX, IDC_LIST1, m_listbox);
	DDX_Text(pDX, IDC_EDIT1, m_strPath1);
	DDX_Text(pDX, IDC_EDIT2, m_strPath2);
	DDX_Text(pDX, IDC_EDIT3, m_strDst);
	DDX_Text(pDX, IDC_EDIT_R, m_iR);
	DDX_Text(pDX, IDC_EDIT_G, m_iG);
	DDX_Text(pDX, IDC_EDIT_B, m_iB);
	DDX_Control(pDX, IDC_EDIT_OUTPUT, m_wndOutput);
}

BEGIN_MESSAGE_MAP(COptionFormView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_MERGE_TGA, OnBnClickedButtonMergeTga)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON_1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON_2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON_START_SINGLE_PIC, OnBnClickedButtonStartSinglePic)
	ON_WM_DESTROY()
	ON_MESSAGE(CPN_SELENDOK,     OnSelEndOK)
END_MESSAGE_MAP()


// COptionFormView 诊断

#ifdef _DEBUG
void COptionFormView::AssertValid() const
{
	CFormView::AssertValid();
}

void COptionFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG



// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// 配置文件信息
struct stPair
{
	string strFold;
	UINT   r;
	UINT   g;
	UINT   b;
};

struct stColorInfo
{
	string strBody;
	string strBody1;
	vector<stPair> vecPairs;
};
	
static stColorInfo g_Colors;

BOOL LoadConfigFile()
{
	static bool bLoaded = false;
	if( bLoaded)  return TRUE;

	const char* xml = "data\\config.xml";

	TiXmlDocument doc;
	if(!doc.LoadFile(xml))
		return false;
	TiXmlElement * pXMLColors = doc.FirstChildElement("colors");
	if(!pXMLColors) return false;
	TiXmlElement * pXMLColor = pXMLColors->FirstChildElement("color");
	if(pXMLColor)
	{
		const char*	body  = pXMLColor->Attribute("body");  
		if(!body) return FALSE;
        const char* body1 = pXMLColor->Attribute("body1");
		if(!body1) return FALSE;

		g_Colors.strBody = body;
		g_Colors.strBody1 = body1;

		TiXmlElement * pXMLPro = pXMLColor->FirstChildElement("pro");
		while(pXMLPro)
		{
			const char * folder = pXMLPro->Attribute("folder");
			if( !folder ) return FALSE;
				int r,g,b;
			if(pXMLPro->QueryIntAttribute("r",&r) != TIXML_SUCCESS)
				return FALSE;

			if(pXMLPro->QueryIntAttribute("g",&g) != TIXML_SUCCESS)
				return FALSE;

		    if(pXMLPro->QueryIntAttribute("b",&b) != TIXML_SUCCESS)
				return FALSE;

			g_Colors.vecPairs.push_back(stPair());
			stPair & pair = g_Colors.vecPairs.back();
			pair.strFold = folder;
			pair.r = r;
			pair.g = g;
			pair.b = b;
			
			pXMLPro = pXMLPro->NextSiblingElement("pro");
		}
	}

	bLoaded =  true;

#ifdef _DEBUG
    TRACE("%s,%s\n",g_Colors.strBody.c_str(),g_Colors.strBody1.c_str());
	for( int i = 0; i<g_Colors.vecPairs.size();i++)
	{
		stPair & pair = g_Colors.vecPairs[i];
		TRACE("%s\n",pair.strFold.c_str());
	}
#endif 
	return true;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// COptionFormView 消息处理程序
//
void COptionFormView::OnBnClickedButtonMergeTga()
{
	// TODO: 在此添加控件通知处理程序代码
    
	GetDlgItem(IDC_BUTTON_MERGE_TGA)->EnableWindow(FALSE);
	if( !m_bLook )
		GetDlgItem(IDC_CHECK1)->ShowWindow(FALSE);

	LoadConfigFile();

	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL,szPath,MAX_PATH);
	CString strOut = szPath;
	int ix = strOut.ReverseFind('\\');
	strOut = strOut.Left(ix); 
	strOut.TrimRight("\\");
	strOut+="\\new\\";
	CreateDirectory(strOut,NULL);

	CString strFolder  = g_Colors.strBody.c_str();
	strFolder.TrimRight("\\");
	strFolder+="\\";
    CString strFolder1 = g_Colors.strBody1.c_str();
	strFolder1.TrimRight("\\");
	strFolder1+="\\";

	
	for( int i = 0; i<g_Colors.vecPairs.size();i++)
	{
		CString strFilePath;
		CString strFilePath1;
		CString strOutPath = strOut;
		stPair & pair = g_Colors.vecPairs[i];
		CString _Path = pair.strFold.c_str();
	
		UINT r = g_Colors.vecPairs[i].r;
		UINT g = g_Colors.vecPairs[i].g;
		UINT b = g_Colors.vecPairs[i].b;

		strFilePath = strFolder + _Path;   // \Body\0001 侠客内衣
		strFilePath1 = strFolder1 + _Path; // \Body1\0001 侠客内衣
		strOutPath += _Path;               // data\0001 侠客内衣
		

		strFilePath.TrimRight("\\");
		strFilePath += "\\";
		strFilePath1.TrimRight("\\");
		strFilePath1 += "\\";
		strOutPath.TrimRight("\\");
		strOutPath += "\\"; //  data\0001 侠客内衣\ 
		
		CreateDirectory(strOutPath,NULL);

		vector<CString> m_vecFolder;         //  \Body\0001 侠客内衣/****  

		CFileFind ff;
		CString strFind;
		strFind.Format(_T("%s\\*.*"),strFilePath);
		BOOL bFind = ff.FindFile(strFind);

		while(bFind)
		{
			bFind = ff.FindNextFile();
		
			if( ff.IsDots())   // . && .. 
				continue;
	
			if( ff.IsDirectory())
			{
				m_vecFolder.push_back(ff.GetFileName());
				CString strFileName = ff.GetFileName();
				TRACE("%s\n",strFileName);
			}
		}

		 
		vector<CString>::iterator iter ; 
		for(iter = m_vecFolder.begin();iter!= m_vecFolder.end(); iter++)
		{
			CString strPath0000 =  * iter;  // 0000
			CString strPath = strFilePath + strPath0000;  // \Body\0001 侠客内衣\0000
			CString strPath1 = strFilePath1 + strPath0000; // \Body1\0001 侠客内衣\0000


			CString strOut2 = strOutPath + strPath0000;  // data\0001 侠客内衣\0000

			

			strPath.TrimRight("\\");
			strPath += "\\";


			strPath1.TrimRight("\\");
			strPath1 += "\\";

			strOut2.TrimRight("\\");
			strOut2 += "\\";

			CreateDirectory(strOut2,NULL);

			CFileFind _ff;
			CString _strFind;
			_strFind.Format(_T("%s\\*.*"),strPath);
			BOOL _bFind = _ff.FindFile(_strFind);
			while( _bFind )         
			{
				_bFind = _ff.FindNextFile();
				if( _ff.IsDots()) continue;

				CString _strFileName = _ff.GetFileName();  
				
                 
				CString strFileName = strPath + _strFileName;  // \Body\0001 侠客内衣\0000\1_000_0000.tga 
                
				TRACE("%s\n",strFileName);
				CString strFileName1 = strPath1 + _strFileName;  // \Body1\0001 侠客内衣\0000\1_000_0000.tga 

			    CString strDstPath = strOut2 + _strFileName;   // data\0001 侠客内衣\0000\1_000_0000.tga

				
			//	AfxBeginThread(MyMergeFunction,this);
				MergeTga(strFileName,strFileName1,strDstPath,r,g,b);
				Output("合并%s和%s完成\n",strFileName,strFileName1);
			}
		}
	}
	GetDlgItem(IDC_BUTTON_MERGE_TGA)->EnableWindow(TRUE);

	if( !m_bLook )
		GetDlgItem(IDC_CHECK1)->ShowWindow(FALSE);


	AfxMessageBox("合并完成");

}


void COptionFormView::MergeTga(const CString & str1File,
							   const CString & str2File,
							   const CString & strDst,
							   UINT r3,UINT g3,UINT b3)
{
	CString strFile1 = str1File;
	CString strFile2 = str2File;
 
	// tga1 
	CGraphicsFileData tga1;
	tga1.LoadTGAFile(strFile1);

	DWORD * pData1 = (DWORD *)tga1.m_pFileData;
	CGraphicsFileData::stFileData * pData11 = (CGraphicsFileData::stFileData *)tga1.m_pstFileData;

    int nTgaHeight1 = tga1.GetHeight();
	int nTgaWidth1  = tga1.GetWidth();

	// tga2 
	CGraphicsFileData tga2;
	tga2.LoadTGAFile(strFile2);

	DWORD * pData2 = (DWORD *)tga2.m_pFileData;
	CGraphicsFileData::stFileData * pData22 = (CGraphicsFileData::stFileData *)tga2.m_pstFileData;

    int nTgaHeight2 = tga2.GetHeight();
	int nTgaWidth2  = tga2.GetWidth();


	ASSERT(nTgaHeight1 == nTgaHeight2 && nTgaWidth2 == nTgaWidth1);

	// Save to Dst Tga 
	TGAFILEHEADER hdr;
	hdr.Width = nTgaWidth2;
	hdr.Height = nTgaHeight2;
	//long rowStride = (nTgaWidth2 + 3) & (~3); // DWORD aligned
	//DWORD* pDstData = new DWORD[nTgaHeight2 * rowStride];
	DWORD* pDstData = new DWORD[nTgaHeight2 * nTgaWidth2];
	int index = 0;
	for(size_t i=0;i<nTgaHeight1;++i)
	{
		      // long rowStride = (nTgaWidth2 + 3) & (~3); // DWORD aligned
				for(size_t j=0;j<nTgaWidth2;++j)
				{
					UINT a2 = pData11[index].A;
					UINT r2 = pData11[index].R;
					UINT g2 = pData11[index].G;
					UINT b2 = pData11[index].B;


					UINT a1 = pData22[index].A;
					UINT r1 = pData22[index].R;
					UINT g1 = pData22[index].G;
					UINT b1 = pData22[index].B;			
				
					//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
					UINT a = a1 + a2;
					if( a > 255 ) a = 255;

					//a1 = a1/255;
					UINT r = ((r1 * r3)/255 * a1/255 ) + (r2 * (255-a1))/255;
					UINT g = ((g1 * g3)/255 * a1/255 ) + (g2 * (255-a1))/255;
					UINT b = ((b1 * b3)/255 * a1/255 ) + (b2 * (255-a1))/255;
					//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

					//pDstData[(nTgaHeight1 - i - 1) * rowStride + j] = COLOR_ARGB(a,r,g,b);
					pDstData[(nTgaHeight1 - i - 1) * nTgaWidth2 + j] = COLOR_ARGB(a,r,g,b);
					index ++;
				}
				pData1 += nTgaWidth2;
				pData2 += nTgaWidth2;
	}

	const char * fileName = (const char *)strDst;
	CFile file;
	CFileException e;
	if( !file.Open( (LPCTSTR )strDst, CFile::modeCreate | CFile::modeWrite, &e ) )
    {
    }
	file.Write(&hdr,sizeof(hdr));
	file.Write(pDstData,nTgaWidth2*nTgaHeight1*4);
	file.Close();

	file.Flush();
	delete [] pDstData;
}



void COptionFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	m_strDst = AfxGetApp()->GetProfileString("Settings","dstDirectory");

	m_btnColor.SubclassDlgItem(IDC_BUTTON_COLOR,this);
	m_btnColor.SetColor(RGB(0, 0, 255));

	LoadConfigFile();
	m_strPath1 = g_Colors.strBody.c_str() ;

	m_strPath2 = g_Colors.strBody1.c_str() ;
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////////
//  DESC: Help functions 
BOOL  COptionFormView::GetAFilePath(CString & strPath)
{
	static char BASED_CODE szFilter[] = "TGA Files (*.TGA)|*.TGA|All Files (*.*)|*.*||";
    CFileDialog dlg(TRUE,"tga","*.tga",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter,this);

    if( IDOK != dlg.DoModal())  return FALSE;
    strPath =  dlg.GetPathName() ;
	UpdateData(FALSE);
	return TRUE;
}

void COptionFormView::OnDestroy()
{
	UpdateData(FALSE);
	AfxGetApp()->WriteProfileString("Settings","dstDirectory",m_strDst);
	CFormView::OnDestroy();
	// TODO: 在此处添加消息处理程序代码
}


void COptionFormView::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CCJShell shl;
	shl.m_strTitle = _T("请选择输出目录");
	if(shl.BrowseForFolder(AfxGetMainWnd()))
	{
		//p->OutputAllToTga(shl.m_strPath);
		m_strPath1 = shl.m_strPath;
		UpdateData(FALSE);
	}
}

BOOL COptionFormView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
    if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;
	return CFormView::PreTranslateMessage(pMsg);
}

void COptionFormView::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	CCJShell shl;
	shl.m_strTitle = _T("请选择输出目录");
	if(shl.BrowseForFolder(AfxGetMainWnd()))
	{
		//p->OutputAllToTga(shl.m_strPath);
		m_strDst = shl.m_strPath;
		UpdateData(FALSE);
	}
}

void COptionFormView::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CCJShell shl;
	shl.m_strTitle = _T("请选择输出目录");
	if(shl.BrowseForFolder(AfxGetMainWnd()))
	{
		//p->OutputAllToTga(shl.m_strPath);
		m_strPath2 = shl.m_strPath;
		UpdateData(FALSE);
	}
}

void COptionFormView::OnBnClickedButtonStartSinglePic()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);



	if( m_strPath1.IsEmpty() || m_strPath2.IsEmpty() || m_strDst.IsEmpty())
	{
		AfxMessageBox("目录不能为空");
		return;
	}


	CString strFilePath1 = m_strPath1;
	CString strFilePath2 = m_strPath2;


	strFilePath1.TrimRight("\\");
	strFilePath2.TrimRight("\\");


	vector<CString>  vecSrcFolder1;
	vector<CString>  vecSrcFolder2;

	CFileFind ff;
	CString strFind;
	strFind.Format(_T("%s\\*.*"),strFilePath1);
	BOOL bFind = ff.FindFile(strFind);

	while(bFind)
	{
		bFind = ff.FindNextFile();

		if( ff.IsDots())   // . && .. 
			continue;

		if( ff.IsDirectory())
		{

		}
		else
		{
			vecSrcFolder1.push_back(ff.GetFileName());
			CString strFileName = ff.GetFileName();
			TRACE("%s\n",strFileName);
		}
	}

	strFind.Format(_T("%s\\*.*"),strFilePath2);
	bFind = ff.FindFile(strFind);

	while(bFind)
	{
		bFind = ff.FindNextFile();

		if( ff.IsDots())   // . && .. 
			continue;

		if( ff.IsDirectory())
		{}
		else
		{
			vecSrcFolder2.push_back(ff.GetFileName());
			CString strFileName = ff.GetFileName();
			TRACE("%s\n",strFileName);
		}
	}

	strFilePath1 +="\\";
	strFilePath2 +="\\";

	CString strDst = m_strDst;
	strDst.TrimRight("\\");
	strDst += "\\new\\";
	CreateDirectory(strDst,NULL);

	for( int i = 0 ;i< vecSrcFolder1.size() ; i++)
	{
		int r ;
		int g ;
		int b ;
		CString strFileName1 = vecSrcFolder1[i];
		CString strNum1 = strFileName1.Left(4);
		CString strFileName2;
		CString strFileName3;
		for( int j = 0 ; j < vecSrcFolder2.size() ; j++)
		{
			strFileName2 = vecSrcFolder2[j];
			CString strNum2 = strFileName2.Left(4);
			if(strNum1 == strNum2)
			{
				break;
			}
		}
		if( j == vecSrcFolder2.size())
		{
			AfxMessageBox("文件"+strFileName1+"在"+ m_strPath2 +"目录中找不到对应的文件");
			return;
		}

		for (int k =0 ; k<g_Colors.vecPairs.size();k++)
		{
			stPair & pair = g_Colors.vecPairs[k];
			strFileName3 = pair.strFold.c_str();
			CString strNum3 = strFileName3.Left(4);
			if( strNum3 == strNum1)
			{
				r = pair.r;
				g = pair.g;
				b = pair.b;
				break;
			} 
		}
		if( k== g_Colors.vecPairs.size())
		{
			AfxMessageBox("配置文件中找不到对应文件"+strNum1+"开头的文件信息");
			return;
		}

		CString strFile1 = strFilePath1 + strFileName1;
		CString strFile2 = strFilePath2 + strFileName2;
		CString strDst1   = strDst + strFileName3 ;
		CreateDirectory(strDst1,NULL);
		strDst1 += "\\" + strFileName1;
		MergeTga(strFile1,strFile2,strDst1,r,g,b);

		Output("合并%s和%s完成 \n",strFile1,strFile2);

	}
}

void  COptionFormView::OnBnClickedCheck1(void)
{
}

LONG COptionFormView::OnSelEndOK(UINT lParam, LONG /*wParam*/)
{
	int ii = 0 ;
	COLORREF clr = m_btnColor.GetColor();
	m_iR = GetRValue(clr);
	m_iG = GetGValue(clr);
	m_iB = GetBValue(clr);
	UpdateData(FALSE);
	return 1;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// 辅助函数
void COptionFormView::Output(LPCTSTR lpszFmt,...)
{
	va_list args;
	va_start(args,lpszFmt);
	char szText[1024];
	_vsnprintf(szText,1024,lpszFmt,args);
	va_end(args);
	szText[sizeof(szText)-1] = 0;

	m_wndOutput.SetSel(-1,-1);
	m_wndOutput.ReplaceSel(szText);
}
