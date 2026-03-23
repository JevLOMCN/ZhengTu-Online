// ToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ViewGL.h"
#include "ToolDlg.h"
#include ".\tooldlg.h"
#include "./CJShell.h"
#include <D3d8.h>
#include <D3dx8tex.h>

#include <string>
// CToolDlg 对话框

IMPLEMENT_DYNAMIC(CToolDlg, CDialog)
CToolDlg::CToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CToolDlg::IDD, pParent)
	, m_strSrcDir(_T(""))
	, m_strDstDir(_T(""))
{
}

CToolDlg::~CToolDlg()
{
}

void CToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strSrcDir);
	DDX_Text(pDX, IDC_EDIT2, m_strDstDir);
}


BEGIN_MESSAGE_MAP(CToolDlg, CDialog)
	ON_BN_CLICKED(IDC_DDS2TGA_SRC, OnBnClickedDds2tgaSrc)
	ON_BN_CLICKED(IDC_DDS2TGA_DST, OnBnClickedDds2tgaDst)
	ON_BN_CLICKED(IDC_DDS2TGA, OnBnClickedDds2tga)
END_MESSAGE_MAP()


// CToolDlg 消息处理程序

void CToolDlg::OnBnClickedDds2tgaSrc()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CCJShell dlg;
	dlg.m_strTitle = _T("请选择源目录");
	dlg.m_strSelDir = m_strSrcDir;
	if(dlg.BrowseForFolder(this))
	{
		m_strSrcDir = dlg.m_strPath;
		UpdateData(FALSE);
	}
}

void CToolDlg::OnBnClickedDds2tgaDst()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CCJShell dlg;
	dlg.m_strTitle = _T("请选择源目录");
	dlg.m_strSelDir = m_strDstDir;
	if(dlg.BrowseForFolder(this))
	{
		m_strDstDir = dlg.m_strPath;
		UpdateData(FALSE);
	}
}

//extern BOOL InitD3DDevice(HWND hWnd);
//extern BOOL DestroyD3DDevice();
//extern IDirect3DDevice8* GetD3DDevice();

#define	COLOR_A4R4G4B4( a, r, g, b )	( (WORD)( (((a) & 0xf0) << 8) | (((r) & 0xf0) << 4 ) | ((g) & 0xf0)  | (((b) & 0xf0) >> 4) ) )
#define COLOR_R5G6B5( r, g, b )		( (WORD)( ( ((r) & 0xf8) << 8 ) | ( ((g) & 0xfc) << 3 ) | ( ((b) & 0xf8) >> 3 ) ) )
#define COLOR_ARGB( a, r, g, b )	( (DWORD)( ( ((a) & 0xff) << 24 ) | ( ((r) & 0xff) << 16 ) | ( ((g) & 0xff)<< 8  ) | ( ((b) & 0xff)  ) ) )
#define A4R4G4B42A8R8G8B8(x) ( (DWORD)(  (((x) & 0xf000) << 16) | (((x) & 0xf00) << 12) | (((x) & 0xf0) << 8) | (((x) & 0xf) << 4) ) )
#define R5G6B52X8R8G8B8(x) ( (DWORD)( 0xff000000 | (((x) & 0xf800) << 8) | (((x) & 0x7e0) << 5) | (((x) & 0x1f) << 3) ))
#define R5G5B52X8R8G8B8(x) ( (DWORD)( 0xff000000 | (((x) & 0x7c00) << 9) | (((x) & 0x3e0) << 6) | (((x) & 0x1f) << 3) ))


BOOL D3DSurfaceToA8R8G8B8(IDirect3DSurface8* pSur,DWORD** pBuffer,int & width,int & height)
{
	return 0;
	/*
	HRESULT hr;
	D3DSURFACE_DESC desc;
	pSur->GetDesc(&desc);
	
	if(desc.Format != D3DFMT_A8R8G8B8)
	{
		IDirect3DSurface8* pSurNew;
		hr = GetD3DDevice()->CreateImageSurface(desc.Width,desc.Height,D3DFMT_A8R8G8B8,&pSurNew);
		if(FAILED(hr)) return FALSE;
		hr = D3DXLoadSurfaceFromSurface(pSurNew,NULL,NULL,pSur,NULL,NULL,D3DX_DEFAULT,0);
		if(FAILED(hr)) 
		{
			pSurNew->Release();
			return FALSE;
		}
		D3DSurfaceToA8R8G8B8(pSurNew,pBuffer,width,height);
		return TRUE;
	}

	D3DLOCKED_RECT lr;
	hr = pSur->LockRect(&lr,NULL,0);
	if(FAILED(hr)) return FALSE;

	*pBuffer = new DWORD [desc.Width * desc.Height];
	width = desc.Width;
	height = desc.Height;

	BYTE* p = (BYTE*)lr.pBits;
	DWORD* dst = *pBuffer;
	for(size_t i=0;i<desc.Height;++i)
	{
		switch(desc.Format)
		{
		case D3DFMT_A4R4G4B4:
			{
				WORD * p0 = (WORD*)p;
				for(size_t j=0;j<desc.Width;++j)
				{
					dst[j] = A4R4G4B42A8R8G8B8(p0[j]);
				}
			}
			break;
		case D3DFMT_R5G6B5:
			{
				WORD * p0 = (WORD*)p;
				for(size_t j=0;j<desc.Width;++j)
				{
					dst[j] = R5G6B52X8R8G8B8(p0[j]);
				}
			}
			break;
		case D3DFMT_X8R8G8B8:
			{
				DWORD * p0 = (DWORD*)p;
				for(size_t j=0;j<desc.Width;++j)
				{
					dst[j] = p0[j] |= 0xff000000;
				}
			}
			break;
		case D3DFMT_A8R8G8B8:
			memcpy(dst,p,sizeof(DWORD)* desc.Width);
			break;
		}
		p += lr.Pitch;
		dst += desc.Width;
	}
	pSur->UnlockRect();
	return TRUE;
	//*/
}

void D3DSurfaceToTga(IDirect3DSurface8* pSur,const char* pszFileName)
{
	#pragma pack(1)
	typedef struct _TAGFILEHEADER{
		BYTE IDLength;
		BYTE ColormapType;
		BYTE ImageType;
		WORD ColormapOffset;		//颜色表首址 
		WORD ColormapLength;		//颜色表的长度
		BYTE ColormapBitDepth;		//颜色表表项的位（bit）数
		WORD XOrigin;
		WORD YOrigin;
		WORD Width;
		WORD Height;
		BYTE PixelDepth;
		BYTE ImageDescriptor;
		_TAGFILEHEADER()
		{
			memset(this,0,sizeof(*this));
			ImageType=2;
			PixelDepth=32;
		}
	}TGAFILEHEADER, FAR * LPTGAFILEHEADER;
	#pragma pack()
	DWORD* pBuffer;
	int width,height;
	if(!D3DSurfaceToA8R8G8B8(pSur,&pBuffer,width,height))
		return ;

	TGAFILEHEADER fh;
    fh.Width=width;
    fh.Height=height;
    fh.PixelDepth= 32 ;
	FILE* fp = fopen(pszFileName,"wb");
	if(!fp) return;
	fwrite(&fh,sizeof(fh),1,fp);
	DWORD* p = pBuffer + width*height;
	for(size_t i=0;i<height;++i)
	{
		p -= width;
		fwrite(p,sizeof(DWORD)*width,1,fp);
	}
	//fwrite(pBuffer,sizeof(DWORD)*width*height,1,fp);
	fclose(fp);
	delete [] pBuffer;
}

void D3DTextureToTga(IDirect3DTexture8* pTex,const char* pszFileName)
{
	HRESULT hr;
	IDirect3DSurface8 * pSur;
	hr = pTex->GetSurfaceLevel(0,&pSur);
	if(FAILED(hr)) return;
	D3DSurfaceToTga(pSur,pszFileName);
	pSur->Release();
}

void ConversionDDS2TGA(const char* pszSrcFileName,const char* pszDstFileName)
{
	HRESULT hr ;
	IDirect3DTexture8 * pTex;
	//hr = D3DXCreateTextureFromFileEx(GetD3DDevice(),pszSrcFileName,D3DX_DEFAULT,D3DX_DEFAULT,1,0,D3DFMT_UNKNOWN,D3DPOOL_SYSTEMMEM ,D3DX_DEFAULT ,D3DX_DEFAULT ,0,NULL,NULL,&pTex);
	//if(FAILED(hr)) return;
	//D3DTextureToTga(pTex,pszDstFileName);
	//pTex->Release();
}

//inline std::string GetFileNameExt(const char* pszFileName)
//{
//	const char* p = strrchr(pszFileName,'.');
//	if(p && (*p)) return (p+1);
//	return "";
//}
void ConversionFolderDDS2TGA(const char* pszDstFolder,const char* pszSrcFolder)
{
	CFileFind ff;
	CString strFind;
	strFind.Format(_T("%s\\*.*"),pszSrcFolder);
	BOOL b = ff.FindFile(strFind);
	while(b)
	{
		b = ff.FindNextFile();
		if(ff.IsDirectory())
		{
			if(ff.IsDots()) continue;
			CString strDst,strSrc;
			strDst.Format(_T("%s\\%s"),pszDstFolder,ff.GetFileName());
			strSrc.Format(_T("%s\\%s"),pszSrcFolder,ff.GetFileName());
			CreateDirectory(strDst,NULL);
			ConversionFolderDDS2TGA(strDst,strSrc);
		}
		else
		{
			if(stricmp("dds",GetFileNameExt(ff.GetFilePath()).c_str()) == 0)
			{
				CString strDst,strSrc;
				strDst.Format(_T("%s\\%s.tga"),pszDstFolder,ff.GetFileTitle());
				strSrc.Format(_T("%s\\%s"),pszSrcFolder,ff.GetFileName());
				ConversionDDS2TGA(strSrc,strDst);
			}
		}
	}
}

void CToolDlg::OnBnClickedDds2tga()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	if(m_strDstDir.IsEmpty()) return;
	if(m_strSrcDir.IsEmpty()) return;
	if(m_strDstDir.Right(1) == _T("\\"))
		m_strDstDir = m_strDstDir.Left(m_strDstDir.GetLength()-1);
	if(m_strSrcDir.Right(1) == _T("\\"))
		m_strSrcDir = m_strSrcDir.Left(m_strSrcDir.GetLength()-1);
	//if(!InitD3DDevice(m_hWnd)) return;
	//ConversionFolderDDS2TGA(m_strDstDir,m_strSrcDir);
	//DestroyD3DDevice();
}
