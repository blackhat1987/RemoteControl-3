
// TeleControlView.cpp: CTeleControlView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "TeleControl.h"
#endif

#include "TeleControlDoc.h"
#include "TeleControlView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTeleControlView

IMPLEMENT_DYNCREATE(CTeleControlView, CView)

BEGIN_MESSAGE_MAP(CTeleControlView, CView)
END_MESSAGE_MAP()

// CTeleControlView 构造/析构

CTeleControlView::CTeleControlView()
{
    // TODO: 在此处添加构造代码

}

CTeleControlView::~CTeleControlView()
{
}

BOOL CTeleControlView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: 在此处通过修改
    //  CREATESTRUCT cs 来修改窗口类或样式

    return CView::PreCreateWindow(cs);
}

// CTeleControlView 绘图

void CTeleControlView::OnDraw(CDC* /*pDC*/)
{
    CTeleControlDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // TODO: 在此处为本机数据添加绘制代码
}


// CTeleControlView 诊断

#ifdef _DEBUG
void CTeleControlView::AssertValid() const
{
    CView::AssertValid();
}

void CTeleControlView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CTeleControlDoc* CTeleControlView::GetDocument() const // 非调试版本是内联的
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTeleControlDoc)));
    return (CTeleControlDoc*)m_pDocument;
}
#endif //_DEBUG


// CTeleControlView 消息处理程序
