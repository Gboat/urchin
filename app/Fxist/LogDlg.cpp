// LogDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Fxist.h"
#include "LogDlg.h"
#include ".\logdlg.h"


// CLogDlg 对话框

IMPLEMENT_DYNAMIC(CLogDlg, CResizableDialog)
CLogDlg::CLogDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CLogDlg::IDD, pParent)
{
}

CLogDlg::~CLogDlg()
{
}

void CLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_ctrlLog);
}


BEGIN_MESSAGE_MAP(CLogDlg, CResizableDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CLogDlg 消息处理程序

BOOL CLogDlg::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME),TRUE);
	AddAnchor(IDC_LIST_LOG,TOP_LEFT,BOTTOM_RIGHT);
	AddAnchor(IDOK,BOTTOM_CENTER);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CLogDlg::OnBnClickedOk()
{
	ShowWindow(SW_HIDE);
}

void CLogDlg::OnClose()
{
	ShowWindow(SW_HIDE);

	//CResizableDialog::OnClose();
}
