// IndexingProgressDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Fxist.h"
#include "IndexingProgressDlg.h"
#include ".\indexingprogressdlg.h"

#include "IndexingDlg.h"

// CIndexingProgressDlg 对话框

IMPLEMENT_DYNAMIC(CIndexingProgressDlg, CDialog)
CIndexingProgressDlg::CIndexingProgressDlg(CIndexingDlg* pParent,CWnd* pLogWnd)
	: CDialog(CIndexingProgressDlg::IDD, pParent)
	,m_pParent(pParent)
	,m_pLogWnd(pLogWnd)
{
}

CIndexingProgressDlg::~CIndexingProgressDlg()
{
}

void CIndexingProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_INDEXING, m_progCtrlIndexing);
}


BEGIN_MESSAGE_MAP(CIndexingProgressDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SHOWLOG, OnBnClickedButtonShowlog)
END_MESSAGE_MAP()


// CIndexingProgressDlg 消息处理程序

BOOL CIndexingProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_progCtrlIndexing.SetWindowText(_T("正在建立索引..."));
	m_progCtrlIndexing.SetMarquee(true, 40);


	if(m_pLogWnd)
	{
		CRect rect;		
		CWnd* pW = GetParent();		
		pW->GetWindowRect(&rect);
				
		CRect rect2;
		m_pLogWnd->GetClientRect(rect2);
		CPoint pt;
		pt.y = rect.top + rect.Height()/2 + 70;
		pt.x = rect.left + rect.Width()/2 - rect2.Width()/2;
		if(pt.x < 0)
			pt.x = 0;		
		m_pLogWnd->MoveWindow(pt.x,pt.y,rect2.Width(),rect2.Height());
		
		if(m_pLogWnd->IsWindowVisible())
		{			
			GetDlgItem(IDC_BUTTON_SHOWLOG)->SetWindowText(_T("隐藏运行记录窗口"));
		}
		else 
		{		
			GetDlgItem(IDC_BUTTON_SHOWLOG)->SetWindowText(_T("显示运行记录窗口"));
		}		
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CIndexingProgressDlg::OnBnClickedOk()
{
	//用户终止
	if(m_pParent->m_IndexThreadParam.m_pCol)
	{
		if(AfxMessageBox(_T("你真的要终止建立索引吗?"),MB_YESNO)== IDYES)
		{
			m_pParent->m_IndexThreadParam.m_pCol->stop();
			m_progCtrlIndexing.SetWindowText(_T("正在终止索引..."));
			GetDlgItem(IDOK)->EnableWindow(FALSE);
		}		
	}
//	OnOK();
}

void CIndexingProgressDlg::OnBnClickedButtonShowlog()
{
	if(m_pLogWnd)
	{
		if(m_pLogWnd->IsWindowVisible())
		{
			m_pLogWnd->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_SHOWLOG)->SetWindowText(_T("显示运行记录窗口"));
		}
		else 
		{
			m_pLogWnd->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_SHOWLOG)->SetWindowText(_T("隐藏运行记录窗口"));
		}
	}
}
