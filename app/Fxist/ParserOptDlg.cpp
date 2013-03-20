// ParserOptDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Fxist.h"
#include "ParserOptDlg.h"
#include ".\parseroptdlg.h"


// CParserOptDlg 对话框

IMPLEMENT_DYNAMIC(CParserOptDlg, CDialog)
CParserOptDlg::CParserOptDlg(LPCTSTR lpszParam,CWnd* pParent /*=NULL*/)
	: CDialog(CParserOptDlg::IDD, pParent)
	, m_strParam(lpszParam)
	, m_strField(_T(""))
	, m_strMeta(_T(""))
{
}

CParserOptDlg::~CParserOptDlg()
{
}

void CParserOptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PARSERPARAM, m_strParam);
	DDX_Text(pDX, IDC_EDIT_PARSER_FILED, m_strField);
	DDX_CBString(pDX, IDC_COMBO_PARSER_META, m_strMeta);
	DDX_Control(pDX, IDC_COMBO_PARSER_FI, m_ctrlForwardIndex);
	DDX_Control(pDX, IDC_COMBO_PARSER_GETMETA, m_ctrlGetMeta);
}


BEGIN_MESSAGE_MAP(CParserOptDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_FI_ADD, OnBnClickedButtonFiAdd)
	ON_BN_CLICKED(IDC_BUTTON_META_ADD, OnBnClickedButtonMetaAdd)
END_MESSAGE_MAP()


// CParserOptDlg 消息处理程序

void CParserOptDlg::OnBnClickedOk()
{	
	UpdateData(TRUE);
	/*m_strParam.MakeLower();
	UpdateData(FALSE);*/
	OnOK();
}

void CParserOptDlg::OnBnClickedButtonFiAdd()
{
	UpdateData(TRUE);
	if(m_strField == _T(""))
	{
		AfxMessageBox(_T("请输入字段名"));
		return ;
	}
	if(m_strParam != _T(""))
		m_strParam += _T(";");
	m_strParam += m_strField;
	m_strParam += _T(".forwardindex=");
	if(m_ctrlForwardIndex.GetCurSel() == 0)
	{		
		m_strParam += _T("no");
	}
	else if(m_ctrlForwardIndex.GetCurSel() == 1)
	{		
		m_strParam += _T("sequence");
	}
	else if(m_ctrlForwardIndex.GetCurSel() == 2)
	{		
		m_strParam += _T("freq");
	}
	else if(m_ctrlForwardIndex.GetCurSel() == 3)
	{		
		m_strParam += _T("position");
	}
	UpdateData(FALSE);
}

void CParserOptDlg::OnBnClickedButtonMetaAdd()
{
	UpdateData(TRUE);	
	if(m_strMeta == _T(""))
	{
		AfxMessageBox(_T("请输元数据名"));
		return ;
	}
	if(m_strParam != _T(""))
		m_strParam += _T(";");
	m_strParam += m_strMeta;
	m_strParam += _T("=");
	if(m_ctrlGetMeta.GetCurSel() == 0)
		m_strParam += _T("true");
	else 
		m_strParam += _T("false");

	UpdateData(FALSE);
}

BOOL CParserOptDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
