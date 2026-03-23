// MergeTgaDoc.cpp :  CMergeTgaDoc 类的实现
//

#include "stdafx.h"
#include "MergeTga.h"

#include "MergeTgaDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMergeTgaDoc

IMPLEMENT_DYNCREATE(CMergeTgaDoc, CDocument)

BEGIN_MESSAGE_MAP(CMergeTgaDoc, CDocument)
END_MESSAGE_MAP()


// CMergeTgaDoc 构造/析构

CMergeTgaDoc::CMergeTgaDoc()
{
	// TODO: 在此添加一次性构造代码

}

CMergeTgaDoc::~CMergeTgaDoc()
{
}

BOOL CMergeTgaDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CMergeTgaDoc 序列化

void CMergeTgaDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CMergeTgaDoc 诊断

#ifdef _DEBUG
void CMergeTgaDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMergeTgaDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMergeTgaDoc 命令
