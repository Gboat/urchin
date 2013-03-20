// AnalyzerOptionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Fxist.h"
#include "AnalyzerOptionDlg.h"
#include ".\analyzeroptiondlg.h"


// CAnalyzerOptionDlg 对话框

IMPLEMENT_DYNAMIC(CAnalyzerOptionDlg, CDialog)
CAnalyzerOptionDlg::CAnalyzerOptionDlg(LPCTSTR lpszParam,CWnd* pParent /*=NULL*/)
	: CDialog(CAnalyzerOptionDlg::IDD, pParent)
	, m_strParameter(lpszParam)
	, m_strStopFile(_T(""))
	, m_strStopFilter(_T(""))
	, m_strStemmer(_T(""))
	, m_strLanguage(_T(""))
{
}

CAnalyzerOptionDlg::~CAnalyzerOptionDlg()
{
}

void CAnalyzerOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_STOPFILE, m_strStopFile);
	DDX_CBString(pDX, IDC_COMBO_STOPFILTER, m_strStopFilter);
	DDX_CBString(pDX, IDC_COMBO_STEMMING, m_strStemmer);
	DDX_CBString(pDX, IDC_COMBO_STEMMING_LANGUAGE, m_strLanguage);
}


BEGIN_MESSAGE_MAP(CAnalyzerOptionDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_STOPFILEBROWSE, OnBnClickedButtonStopfilebrowse)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_STOPFILTER, OnCbnSelchangeComboStopfilter)
	ON_CBN_SELCHANGE(IDC_COMBO_STEMMING, OnCbnSelchangeComboStemming)
END_MESSAGE_MAP()


// CAnalyzerOptionDlg 消息处理程序

void CAnalyzerOptionDlg::OnBnClickedButtonStopfilebrowse()
{
	CFileDialog dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"列表文件(*.stf)|*.stf|所有文件(*.*)|*.*||");
	if(dlg.DoModal() == IDOK)
	{
		m_strStopFile = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CAnalyzerOptionDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	if(m_strStopFilter != _T("") && m_strStopFile == _T(""))
	{
		AfxMessageBox(_T("请指定停用词文件"));
		return;
	}
	CString str;
	if(m_strStopFilter != _T(""))
	{
		str += _T("stopword=");
		str += m_strStopFilter;
		str += _T(":");
		str += m_strStopFile;
	}
	if(m_strStemmer != _T(""))
	{
		if(str != _T(""))
			str += _T(";");
		str += _T("stemmer=");
		str += m_strStemmer;
		if(m_strLanguage != _T(""))
		{
			str += _T(":");
			str += m_strLanguage;
		}
	}
	m_strParameter = str;

	OnOK();
}

BOOL CAnalyzerOptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(m_strParameter == _T(""))
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_STOPFILTER))->SetCurSel(0);
		((CComboBox*)GetDlgItem(IDC_COMBO_STEMMING))->SetCurSel(0);
		((CComboBox*)GetDlgItem(IDC_COMBO_STEMMING_LANGUAGE))->SetCurSel(0);
	}
	else 
	{
		CParamParser pp;
		pp.parse((LPCTSTR)m_strParameter,_T(";"));
		tstring str;
        if(pp.getParam(_T("stopword"),str))
		{			
			tstring val;
			CParamParser pp2;			
			pp2.parse2(str.c_str(),_T(":"));
			if(pp2.getParam2(0,val))
			{
				m_strStopFilter = val.c_str();
			}
			if(pp2.getParam2(1,val))
			{				
				m_strStopFile = val.c_str();
			}
			
		}

		if(pp.getParam(_T("stemmer"),str))
		{			
			tstring val;
			CParamParser pp2;			
			pp2.parse2(str.c_str(),_T(":"));
			if(pp2.getParam2(0,val))
			{
				m_strStemmer = val.c_str();
			}
			if(pp2.getParam2(1,val))
			{				
				m_strLanguage = val.c_str();
			}
			
		}
		UpdateData(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CAnalyzerOptionDlg::OnCbnSelchangeComboStopfilter()
{
	if(((CComboBox*)GetDlgItem(IDC_COMBO_STOPFILTER))->GetCurSel() != 0)
	{
		UpdateData(TRUE);
		if(m_strStopFile == _T(""))
		{
			m_strStopFile = _T("stopword.stf");
			UpdateData(FALSE);
		}	
	}
}

void CAnalyzerOptionDlg::OnCbnSelchangeComboStemming()
{
	if(((CComboBox*)GetDlgItem(IDC_COMBO_STEMMING))->GetCurSel() != 0)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_STEMMING_LANGUAGE))->SetCurSel(1);
	}
}
