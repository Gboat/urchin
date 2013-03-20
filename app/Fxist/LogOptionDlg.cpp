// LogOptionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Fxist.h"
#include "LogOptionDlg.h"
#include ".\logoptiondlg.h"


// CLogOptionDlg 对话框

IMPLEMENT_DYNAMIC(CLogOptionDlg, CDialog)
CLogOptionDlg::CLogOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogOptionDlg::IDD, pParent)
	, m_strLogFile(_T(""))
	, m_nLogMode(1)
{
}

CLogOptionDlg::~CLogOptionDlg()
{
}

void CLogOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LOGFILE, m_strLogFile);
	DDX_Radio(pDX, IDC_RADIO_TOFILE, m_nLogMode);
}


BEGIN_MESSAGE_MAP(CLogOptionDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_LOGBROWSE, OnBnClickedButtonLogbrowse)
	ON_BN_CLICKED(IDC_BUTTON_VIEWLOG, OnBnClickedButtonViewlog)
	ON_BN_CLICKED(IDC_RADIO_TOFILE, OnBnClickedRadioTofile)
	ON_BN_CLICKED(IDC_RADIO_TOWINDOW, OnBnClickedRadioTowindow)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CLogOptionDlg 消息处理程序

void CLogOptionDlg::OnBnClickedButtonLogbrowse()
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"列表文件(*.txt)|*.txt|所有文件(*.*)|*.*||");
	if(dlg.DoModal() == IDOK)
	{
		m_strLogFile = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CLogOptionDlg::OnBnClickedButtonViewlog()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CLogOptionDlg::OnBnClickedRadioTofile()
{
	GetDlgItem(IDC_EDIT_LOGFILE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_LOGBROWSE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_VIEWLOG)->EnableWindow(TRUE);
}

void CLogOptionDlg::OnBnClickedRadioTowindow()
{
	GetDlgItem(IDC_EDIT_LOGFILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_LOGBROWSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_VIEWLOG)->EnableWindow(FALSE);	
}

void CLogOptionDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	if(m_strLogFile == _T(""))
	{
		AfxMessageBox(_T("请输入log文件路径"));
		return;
	}
	OnOK();
}

BOOL CLogOptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	OnBnClickedRadioTowindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
