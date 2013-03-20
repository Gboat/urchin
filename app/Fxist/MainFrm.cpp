// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "Fxist.h"

#include "MainFrm.h"
#include "FxistDoc.h"
#include ".\mainfrm.h"
#include "CreateFXCollectionDlg.h"
#include "LogOptionDlg.h"

#include "CreateStopFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOL_CREATEFIRTEXCOL, OnToolCreatefirtexcol)
	ON_COMMAND(ID_VIEW_LOGWINDOW, OnViewLogwindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOGWINDOW, OnUpdateViewLogwindow)
	ON_COMMAND(ID_TOOL_LOGOPTION, OnToolLogoption)
	ON_COMMAND(ID_TOOL_CREATESTOPFILE, OnToolCreatestopfile)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器	
	ID_INDICATOR_NUM,	
};


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
	if(m_pLogDeletater)
	{
		delete m_pLogDeletater;
		m_pLogDeletater = NULL;
		GlobalConfig.Logger.reset(m_pLogDeletater);
	}
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
	//	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	//{
	//	TRACE0("未能创建工具栏\n");
	//	return -1;      // 未能创建
	//}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	// TODO: 如果不需要工具栏可停靠，则删除这三行
//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
//	EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBar(&m_wndToolBar);

	if(!m_logWindow.Create(IDD_DIALOG_LOG,this))
	{
		TRACE0("未能创建程序运行记录窗口\n");
		return -1;      // 未能创建
	}
	m_logWindow.SetWindowPos(&wndTop,0,0,0,0,SWP_HIDEWINDOW|SWP_NOMOVE|SWP_NOSIZE);	

	m_pLogDeletater = new CListBoxLogDelegater((CListBox*)&(m_logWindow.m_ctrlLog));
	
	if(m_logToFile.IsEmpty())
		GlobalConfig.Logger.reset(m_pLogDeletater);
	else
	{
		GlobalConfig.Logger.reset((LPCTSTR)m_logToFile);
	} 	

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式

	return TRUE;
}


// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 消息处理程序
void CMainFrame::OnOpenIndex()
{
	CFxistDoc*	pDoc = (CFxistDoc*)GetActiveDocument();//((CFxistView*)GetParent())->GetDocument();	
	m_wndStatusBar.SetPaneText(0,pDoc->GetIndexName());
	CString str;
	str.Format(_T("%u个文档"),pDoc->GetIndexReader()->numDocs());
	m_wndStatusBar.SetPaneText(1,str);
}
void CMainFrame::OnCloseIndex()
{
	m_wndStatusBar.SetPaneText(0,_T("还未打开索引"));
}

void CMainFrame::OnToolCreatefirtexcol()
{
	CCreateFXCollectionDlg dlg;
	dlg.DoModal();
}

CWnd* CMainFrame::GetLogWindow()
{
	return &m_logWindow;
}
void CMainFrame::ShowLogWindow(BOOL bShow)
{
	if(m_logToFile.IsEmpty())
		m_logWindow.ShowWindow(bShow);
}

void CMainFrame::OnViewLogwindow()
{
	ShowLogWindow(TRUE);
}

void CMainFrame::OnUpdateViewLogwindow(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_logWindow.IsWindowVisible());
}

void CMainFrame::OnToolLogoption()
{
	CLogOptionDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.m_nLogMode == 0)
		{
			m_logToFile = dlg.m_strLogFile;
			GlobalConfig.Logger.reset((LPCTSTR)m_logToFile);			
		}
		else
		{
			m_logToFile = _T("");
            GlobalConfig.Logger.reset(m_pLogDeletater);			
		}
	}	
}

void CMainFrame::OnToolCreatestopfile()
{
	CCreateStopFileDlg dlg;
	dlg.DoModal();
}
