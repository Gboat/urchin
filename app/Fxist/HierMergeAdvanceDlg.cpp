// HierMergeAdvanceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Fxist.h"
#include "HierMergeAdvanceDlg.h"


// CHierMergeAdvanceDlg 对话框

IMPLEMENT_DYNAMIC(CHierMergeAdvanceDlg, CDialog)
CHierMergeAdvanceDlg::CHierMergeAdvanceDlg(LPCTSTR pszStrategy,LPCTSTR pszParam,CWnd* pParent /*=NULL*/)
	: CDialog(CHierMergeAdvanceDlg::IDD, pParent)
	, m_strStrategy(pszStrategy)
	, m_strParam(pszParam)
	, m_nStrategy(0)
	, m_nGPParam(0)
	, m_nHLMM(3)
	, m_nHLMC(3)
	, m_nHLMS(1)
	, m_nGPR(3)
	, m_nGPP(2)
{
}

CHierMergeAdvanceDlg::~CHierMergeAdvanceDlg()
{
}

void CHierMergeAdvanceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_HLM, m_nStrategy);
	DDX_Radio(pDX, IDC_RADIO_GP_R, m_nGPParam);
	DDX_Text(pDX, IDC_EDIT_HLM_M, m_nHLMM);
	DDX_Text(pDX, IDC_EDIT_HLM_C, m_nHLMC);
	DDV_MinMaxUInt(pDX, m_nHLMM, 2, 100);
	DDV_MinMaxUInt(pDX, m_nHLMC, 2, 100);
	DDX_Text(pDX, IDC_EDIT_HLM_S, m_nHLMS);
	DDV_MinMaxUInt(pDX, m_nHLMS, 0, 10000000);
	DDX_Text(pDX, IDC_EDIT_GPR, m_nGPR);
	DDV_MinMaxUInt(pDX, m_nGPR, 2, 100);
	DDX_Text(pDX, IDC_EDIT_GPP, m_nGPP);
	DDV_MinMaxUInt(pDX, m_nGPP, 1, 10000);
}


BEGIN_MESSAGE_MAP(CHierMergeAdvanceDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_HLM, OnBnClickedRadioHlm)
	ON_BN_CLICKED(IDC_RADIO_GP, OnBnClickedRadioGp)
	ON_BN_CLICKED(IDC_RADIO_LOG, OnBnClickedRadioLog)
	ON_BN_CLICKED(IDC_RADIO_GP_R, OnBnClickedRadioGpR)
	ON_BN_CLICKED(IDC_RADIO_GP_P, OnBnClickedRadioGpP)
END_MESSAGE_MAP()


// CHierMergeAdvanceDlg 消息处理程序

void CHierMergeAdvanceDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	if(m_nStrategy == 0)
	{
		m_strStrategy = _T("HLM");
		m_strParam.Format("m=%d;c=%d;s=%d",m_nHLMM,m_nHLMC,m_nHLMS);
	}
	else if(m_nStrategy == 1)
	{
		m_strStrategy = _T("GP");
		if(m_nGPParam == 0)
			m_strParam.Format("r=%d",m_nGPR);
		else 
			m_strParam.Format("p=%d",m_nGPP);
	}
	else
	{
		m_strStrategy = _T("LOG");
	}
	OnOK();
}

BOOL CHierMergeAdvanceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_HLM_M))->SetRange32(2,100);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_HLM_C))->SetRange32(2,100);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_HLM_S))->SetRange32(0,10000000);

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_GPR))->SetRange32(2,100);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_GPP))->SetRange32(1,10000);


	if(!m_strStrategy.CompareNoCase(_T("HLM")))
	{
		m_nStrategy = 0;
		CParamParser pp;
		if(pp.parse((LPCTSTR)m_strParam))
		{		
			int32_t p;
			if(pp.getParam(_T("m"),p))
				m_nHLMM = p;
			if(pp.getParam(_T("c"),p))
				m_nHLMC = p;
			if(pp.getParam(_T("s"),p))
				m_nHLMS = p;
		}
		OnBnClickedRadioHlm();
	}
	else if(!m_strStrategy.CompareNoCase(_T("GP")))
	{
		m_nStrategy = 1;
		CParamParser pp;
		if(pp.parse((LPCTSTR)m_strParam))
		{		
			int32_t p;
			if(pp.getParam(_T("r"),p))
			{
				m_nGPR = p;
				m_nGPParam = 0;
				OnBnClickedRadioGpR();
			}
			else if(pp.getParam(_T("p"),p))
			{
				m_nGPP = p;
				m_nGPParam = 1;
				OnBnClickedRadioGpP();
			}
		}
		OnBnClickedRadioGp();
	}
	else if(!m_strStrategy.CompareNoCase(_T("GP")))
	{
		m_nStrategy = 2;
		OnBnClickedRadioLog();
	}
	else
	{
		m_nStrategy = 0;
		OnBnClickedRadioHlm();
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CHierMergeAdvanceDlg::OnBnClickedRadioHlm()
{
	GetDlgItem(IDC_SPIN_HLM_M)->EnableWindow(TRUE);
	GetDlgItem(IDC_SPIN_HLM_C)->EnableWindow(TRUE);
	GetDlgItem(IDC_SPIN_HLM_S)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_HLM_M)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_HLM_C)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_HLM_S)->EnableWindow(TRUE);


	GetDlgItem(IDC_SPIN_GPR)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_GPP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_GPR)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_GPP)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_GP_R)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_GP_P)->EnableWindow(FALSE);
}

void CHierMergeAdvanceDlg::OnBnClickedRadioGp()
{
	GetDlgItem(IDC_SPIN_HLM_M)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_HLM_C)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_HLM_S)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_HLM_M)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_HLM_C)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_HLM_S)->EnableWindow(FALSE);


	GetDlgItem(IDC_SPIN_GPR)->EnableWindow(TRUE);
	GetDlgItem(IDC_SPIN_GPP)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_GPR)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_GPP)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_GP_R)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_GP_P)->EnableWindow(TRUE);
	OnBnClickedRadioGpR();
}

void CHierMergeAdvanceDlg::OnBnClickedRadioLog()
{
	GetDlgItem(IDC_SPIN_HLM_M)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_HLM_C)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_HLM_S)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_HLM_M)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_HLM_C)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_HLM_S)->EnableWindow(FALSE);


	GetDlgItem(IDC_SPIN_GPR)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_GPP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_GPR)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_GPP)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_GP_R)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_GP_P)->EnableWindow(FALSE);
}

void CHierMergeAdvanceDlg::OnBnClickedRadioGpR()
{
	GetDlgItem(IDC_SPIN_GPR)->EnableWindow(TRUE);
	GetDlgItem(IDC_SPIN_GPP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_GPR)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_GPP)->EnableWindow(FALSE);
}

void CHierMergeAdvanceDlg::OnBnClickedRadioGpP()
{
	GetDlgItem(IDC_SPIN_GPR)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_GPP)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_GPR)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_GPP)->EnableWindow(TRUE);
}
