// MainFrm.cpp : CMainFrame ���ʵ��
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
	ID_SEPARATOR,           // ״̬��ָʾ��	
	ID_INDICATOR_NUM,	
};


// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
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
	//	TRACE0("δ�ܴ���������\n");
	//	return -1;      // δ�ܴ���
	//}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	// TODO: �������Ҫ��������ͣ������ɾ��������
//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
//	EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBar(&m_wndToolBar);

	if(!m_logWindow.Create(IDD_DIALOG_LOG,this))
	{
		TRACE0("δ�ܴ����������м�¼����\n");
		return -1;      // δ�ܴ���
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
	// TODO: �ڴ˴�ͨ���޸� CREATESTRUCT cs ���޸Ĵ������
	// ��ʽ

	return TRUE;
}


// CMainFrame ���

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


// CMainFrame ��Ϣ�������
void CMainFrame::OnOpenIndex()
{
	CFxistDoc*	pDoc = (CFxistDoc*)GetActiveDocument();//((CFxistView*)GetParent())->GetDocument();	
	m_wndStatusBar.SetPaneText(0,pDoc->GetIndexName());
	CString str;
	str.Format(_T("%u���ĵ�"),pDoc->GetIndexReader()->numDocs());
	m_wndStatusBar.SetPaneText(1,str);
}
void CMainFrame::OnCloseIndex()
{
	m_wndStatusBar.SetPaneText(0,_T("��δ������"));
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
