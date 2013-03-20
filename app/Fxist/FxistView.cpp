// FistView.cpp : CFxistView 类的实现
//

#include "stdafx.h"
#include "Fxist.h"

#include "MainFrm.h"
#include "FxistDoc.h"
#include "FxistView.h"
#include ".\fxistview.h"
#include "OverviewDlg.h"
#include "DocumentsDlg.h"
#include "IndexingDlg.h"
#include "SearchingDlg.h"
#include "IndexMaintainDlg.h"
#include "TermVectorDlg.h"
#include "PluginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFxistView

IMPLEMENT_DYNCREATE(CFxistView, CTabView)

BEGIN_MESSAGE_MAP(CFxistView, CTabView)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_INDEXING, OnFileIndexing)
END_MESSAGE_MAP()

// CFxistView 构造/析构

CFxistView::CFxistView()
{
	// TODO: 在此处添加构造代码

}

CFxistView::~CFxistView()
{
}

BOOL CFxistView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式

	return CTabView::PreCreateWindow(cs);
}

// CFxistView 绘制

void CFxistView::OnDraw(CDC* /*pDC*/)
{
	CFxistDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CFxistView 诊断

#ifdef _DEBUG
void CFxistView::AssertValid() const
{
	CTabView::AssertValid();
}

void CFxistView::Dump(CDumpContext& dc) const
{
	CTabView::Dump(dc);
}

CFxistDoc* CFxistView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFxistDoc)));
	return (CFxistDoc*)m_pDocument;
}
#endif //_DEBUG


// CFxistView 消息处理程序

/////////////////////////////////////////////////////////////////////////////
// CFxistView overrides

BOOL CFxistView::OnInitPage (int nIndex, int nPageID)
{		
	return TRUE; // Set the focus to the first control on the page
}

void CFxistView::OnActivatePage (int nIndex, int nPageID)
{
	CChildDialog* pDlg = (CChildDialog*)GetPage(nIndex);
	pDlg->OnActivate();
}

void CFxistView::OnDeactivatePage (int nIndex, int nPageID)
{
	CChildDialog* pDlg = (CChildDialog*)GetPage(nIndex);
	pDlg->OnDeactivate();
}

void CFxistView::OnDestroyPage (int nIndex, int nPageID)
{

}

int CFxistView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (m_font.CreatePointFont (80, _T ("MS Sans Serif")))
		GetTabCtrl ().SetFont (&m_font);
	//
	// Add pages.
	//
	int nIndex = AddPage (_T ("基本信息"),0,RUNTIME_CLASS(COverviewDlg),IDD_DIALOG_OVERVIEW);
	ASSERT (nIndex != -1);
	nIndex = AddPage (_T ("文档"),1,RUNTIME_CLASS(CDocumentsDlg),IDD_DIALOG_DOCUMENTS);
	ASSERT (nIndex != -1);
	nIndex = AddPage (_T ("索引"),2,RUNTIME_CLASS(CIndexingDlg),IDD_DIALOG_INDEXING);
	ASSERT (nIndex != -1);
	nIndex = AddPage (_T ("检索"),3,RUNTIME_CLASS(CSearchingDlg),IDD_DIALOG_SEARCHING);
	ASSERT (nIndex != -1);
	nIndex = AddPage (_T ("索引维护"),3,RUNTIME_CLASS(CIndexMaintainDlg),IDD_DIALOG_INDEXMAINTAIN);
	ASSERT (nIndex != -1);
	nIndex = AddPage (_T ("前向索引"),3,RUNTIME_CLASS(CTermVectorDlg),IDD_FORWARDINDEXDLG);
	ASSERT (nIndex != -1);
	nIndex = AddPage (_T ("插件"),3,RUNTIME_CLASS(CPluginDlg),IDD_DIALOG_PLUGIN);
	ASSERT (nIndex != -1);
	
	return 0;
}



void CFxistView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{	
	if(lHint == UPDATEVIEW_OPENINDEX)
	{
		for (int i = 0;i<GetPageCount();i++)
		{
			CChildDialog* pDlg = (CChildDialog*)GetPage(i);
			CFxistDoc* pDoc = GetDocument();
			pDlg->OnOpenIndex();
		}		
		((CMainFrame*)AfxGetApp()->GetMainWnd())->OnOpenIndex();
	}
	else if(lHint == UPDATEVIEW_CLOSEINDEX) 
	{
		for (int i = 0;i<GetPageCount();i++)
		{
			CChildDialog* pDlg = (CChildDialog*)GetPage(i);
			CFxistDoc* pDoc = GetDocument();
			pDlg->OnCloseIndex();
		}		
		((CMainFrame*)AfxGetApp()->GetMainWnd())->OnCloseIndex();
	}
}

void CFxistView::OnFileIndexing()
{
	ActivatePage(2);
}
