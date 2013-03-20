// LogOptionDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Fxist.h"
#include "LogOptionDlg.h"
#include ".\logoptiondlg.h"


// CLogOptionDlg �Ի���

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


// CLogOptionDlg ��Ϣ�������

void CLogOptionDlg::OnBnClickedButtonLogbrowse()
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"�б��ļ�(*.txt)|*.txt|�����ļ�(*.*)|*.*||");
	if(dlg.DoModal() == IDOK)
	{
		m_strLogFile = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CLogOptionDlg::OnBnClickedButtonViewlog()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		AfxMessageBox(_T("������log�ļ�·��"));
		return;
	}
	OnOK();
}

BOOL CLogOptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	OnBnClickedRadioTowindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
