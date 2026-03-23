// MergeTgaView.cpp : CMergeTgaView 类的实现
//

#include "stdafx.h"
#include "MergeTga.h"

#include "MergeTgaDoc.h"
#include "MergeTgaView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMergeTgaView

IMPLEMENT_DYNCREATE(CMergeTgaView, CScrollView)

BEGIN_MESSAGE_MAP(CMergeTgaView, CScrollView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CMergeTgaView 构造/析构

CMergeTgaView::CMergeTgaView()
{
	// TODO: 在此处添加构造代码

}

CMergeTgaView::~CMergeTgaView()
{
}

BOOL CMergeTgaView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式

	return CScrollView::PreCreateWindow(cs);
}

// CMergeTgaView 绘制

void CMergeTgaView::OnDraw(CDC* /*pDC*/)
{
	CMergeTgaDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}

void CMergeTgaView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	CSize sizeTotal;
	// TODO: 计算此视图的合计大小
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


// CMergeTgaView 打印

BOOL CMergeTgaView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMergeTgaView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 打印前添加额外的初始化
}

void CMergeTgaView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 打印后添加清除过程
}


// CMergeTgaView 诊断

#ifdef _DEBUG
void CMergeTgaView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CMergeTgaView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CMergeTgaDoc* CMergeTgaView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMergeTgaDoc)));
	return (CMergeTgaDoc*)m_pDocument;
}
#endif //_DEBUG


// CMergeTgaView 消息处理程序
