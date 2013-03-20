// TermVectorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Fxist.h"
#include "TermVectorDlg.h"
#include ".\termvectordlg.h"
#include "FxistDoc.h"
#include "FxistView.h"




// CTermVectorDlg 对话框

IMPLEMENT_DYNCREATE(CTermVectorDlg, CChildDialog)
CTermVectorDlg::CTermVectorDlg(CWnd* pParent /*=NULL*/)
	: CChildDialog(CTermVectorDlg::IDD, pParent)
	, m_tvDocNum(0)
	, m_strComboField(_T(""))
	, m_tvType(0)
	, m_pTVReader(NULL)
	,m_pIndexReader(NULL)
	, m_strTermVector(_T(""))
{
}

CTermVectorDlg::~CTermVectorDlg()
{
	if(m_pTVReader)
	{		
		m_pTVReader = NULL;
	}
}

void CTermVectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CChildDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TVDOCNO, m_tvDocNum);
	DDV_MinMaxInt(pDX, m_tvDocNum, 0, 100000000);
	DDX_CBString(pDX, IDC_COMBO_TVFIELD, m_strComboField);
	DDX_Radio(pDX, IDC_RADIO_SEQUENCE, m_tvType);
	DDX_Text(pDX, IDC_EDIT_FORWARDINDEX, m_strTermVector);
}


BEGIN_MESSAGE_MAP(CTermVectorDlg, CChildDialog)
	ON_BN_CLICKED(IDC_BUTTON_TVQUERY, OnBnClickedButtonTVQuery)
	ON_BN_CLICKED(IDC_BUTTON_TVFIRSTDOC, OnBnClickedButtonTvfirstdoc)
	ON_BN_CLICKED(IDC_BUTTON_TVPREVDOC, OnBnClickedButtonTvprevdoc)
	ON_BN_CLICKED(IDC_BUTTON_TVNEXTDOC, OnBnClickedButtonTvnextdoc)
	ON_BN_CLICKED(IDC_BUTTON_TVLASTDOC, OnBnClickedButtonTvlastdoc)
	ON_CBN_SELCHANGE(IDC_COMBO_TVFIELD, OnCbnSelchangeComboTvfield)
END_MESSAGE_MAP()


// CTermVectorDlg 消息处理程序
void CTermVectorDlg::OnOpenIndex()
{
	CFxistDoc*	pDoc = ((CFxistView*)GetParent())->GetDocument();
	m_pIndexReader = pDoc->GetIndexReader();
	if(!m_pIndexReader)
		return ;
	CFieldsInfo& fieldsInfo = m_pIndexReader->getFieldsInfo();	
	BOOL bEnable = FALSE;
	CFieldInfo* pFieldInfo;
	fieldsInfo.startIterator();
	while(fieldsInfo.hasNext())
	{
		pFieldInfo = fieldsInfo.next();
		if(pFieldInfo->isStoreForwardIndex())
		{
			((CComboBox*)GetDlgItem(IDC_COMBO_TVFIELD))->AddString(pFieldInfo->getName());
			bEnable = TRUE;
		}	
	}
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TVDOCNO))->SetRange32(0,m_pIndexReader->maxDoc()-1);
	m_tvDocNum = 0;
	EnableControls(bEnable);	
	if(bEnable)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_TVFIELD))->SetCurSel(0);
		OnCbnSelchangeComboTvfield();
	}
	UpdateData(FALSE);
}
void CTermVectorDlg::OnCloseIndex()
{
	((CComboBox*)GetDlgItem(IDC_COMBO_TVFIELD))->ResetContent();
	EnableControls(FALSE);
	m_pTVReader = NULL;
	m_pIndexReader = NULL;
}

void CTermVectorDlg::EnableControls(BOOL bEnable)
{
	GetDlgItem(IDC_BUTTON_TVQUERY)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_TVFIRSTDOC)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_TVPREVDOC)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_TVNEXTDOC)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_TVLASTDOC)->EnableWindow(bEnable);
	GetDlgItem(IDC_COMBO_TVFIELD)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_TVDOCNO)->EnableWindow(bEnable);
	GetDlgItem(IDC_SPIN_TVDOCNO)->EnableWindow(bEnable);
	if(!bEnable)
		EnableRadios(-1);
}
void CTermVectorDlg::EnableRadios(int i)
{
	switch(i)
	{
	case -1:
		GetDlgItem(IDC_RADIO_SEQUENCE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_FREQ)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_POSITION)->EnableWindow(FALSE);		
		break;
	case 0:
		GetDlgItem(IDC_RADIO_SEQUENCE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_FREQ)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_POSITION)->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_SEQUENCE))->SetCheck(TRUE);
		break;
	case 1:
		GetDlgItem(IDC_RADIO_SEQUENCE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_FREQ)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_POSITION)->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_FREQ))->SetCheck(TRUE);
		break;
	case 2:
		GetDlgItem(IDC_RADIO_SEQUENCE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_FREQ)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_POSITION)->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_POSITION))->SetCheck(TRUE);
		break;
	}

}
void CTermVectorDlg::OnBnClickedButtonTVQuery()
{
	UpdateData(TRUE);
	if(!m_pIndexReader->isDeleted(m_tvDocNum))
		displayTermVector(m_tvDocNum);
}

BOOL CTermVectorDlg::OnInitDialog()
{
	CChildDialog::OnInitDialog();

	AddAnchor(IDC_EDIT_FORWARDINDEX,TOP_LEFT,BOTTOM_RIGHT);

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TVDOCNO))->SetRange32(0,0);

	EnableControls(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CTermVectorDlg::displayTermVector(docid_t	docID)
{
	if(m_tvDocNum < 0 || m_pIndexReader->isDeleted(m_tvDocNum))
		return ;		
	switch(m_pTVReader->getForwardIndexType((LPCTSTR)m_strComboField))
	{
	case ForwardIndex::SEQUENCE:
		{
			CTermSequenceVector* pSV = m_pTVReader->getSquenceVector(docID,(LPCTSTR)m_strComboField);
			if(pSV)
			{
				displayTermVector(pSV);
				delete pSV;
			}
		}
		break;
	case ForwardIndex::FREQ:
		{
			CTermFreqVector* pFV = m_pTVReader->getFreqVector(docID,(LPCTSTR)m_strComboField);
			if(pFV)
			{
				displayTermVector(pFV);
				delete pFV;
			}
		}
		break;
	case ForwardIndex::POSITION:
		{
			CTermPositionVector* pPV = m_pTVReader->getPositionVector(docID,(LPCTSTR)m_strComboField);
			if(pPV)
			{
				displayTermVector(pPV);
				delete pPV;
			}
		}
		break;
	}
	
}
void CTermVectorDlg::displayTermVector(CTermSequenceVector* pSV)
{
	count_t numTerms = pSV->numTerms();
	const termid_t* terms = pSV->getTermIDs();

	CString tmp;
	tmp.Format(_T("词总数：%u\r\n"),numTerms);
	m_strTermVector = tmp;
	for(count_t i = 0;i< numTerms;i++)
	{
		tmp.Format(_T("<%u>,"),terms[i]);
		m_strTermVector += tmp;
	}
	m_strTermVector.Truncate(m_strTermVector.GetLength() - 1);
	UpdateData(FALSE);

}
void CTermVectorDlg::displayTermVector(CTermFreqVector* pFV)
{
	count_t numTerms = pFV->numTerms();
	count_t numDistTerms = pFV->numDistinctTerms();
	CString tmp;
	tmp.Format(_T("词总数：%u\r\n非重复词总数：%u\r\n"),numTerms,numDistTerms);
	m_strTermVector = tmp;
	const termid_t*	termIDs = pFV->getTermIDs();
	const freq_t*		freqs = pFV->getTermFrequencies();
	for(count_t i = 0;i< numDistTerms;i++)
	{
		tmp.Format(_T("<%u;%u>,"),termIDs[i],freqs[i]);
		m_strTermVector += tmp;
	}
	m_strTermVector.Truncate(m_strTermVector.GetLength() - 1);
	UpdateData(FALSE);
}
void CTermVectorDlg::displayTermVector(CTermPositionVector* pPV)
{
	count_t numTerms = pPV->numTerms();
	count_t numDistTerms = pPV->numDistinctTerms();
	CString tmp;
	tmp.Format(_T("词总数：%u\r\n非重复词总数：%u\r\n"),numTerms,numDistTerms);
	m_strTermVector = tmp;
	const termid_t*	termIDs = pPV->getTermIDs();
	const freq_t*		freqs = pPV->getTermFrequencies();
	const loc_t* locs = pPV->getTermPositions();
	int32_t nFreqs = 0;
	for(count_t i = 0;i< numDistTerms;i++)
	{
		tmp.Format(_T("<%u;%u;"),termIDs[i],freqs[i]);		
		for(int32_t j = 0;j<freqs[i];j++)
		{
			if(j == freqs[i] - 1)
				tmp.AppendFormat(_T("%u>,"),locs[nFreqs + j]);
			else 
				tmp.AppendFormat(_T("%u,"),locs[nFreqs + j]);
		}
		nFreqs += freqs[i];
		m_strTermVector += tmp;
	}
	m_strTermVector.Truncate(m_strTermVector.GetLength() - 1);
	UpdateData(FALSE);
}

void CTermVectorDlg::OnBnClickedButtonTvfirstdoc()
{
	m_tvDocNum = 0;
	while(m_pIndexReader->isDeleted(m_tvDocNum))
		m_tvDocNum++;
	displayTermVector(m_tvDocNum);
	UpdateData(FALSE);
}


void CTermVectorDlg::OnBnClickedButtonTvprevdoc()
{
	if(m_tvDocNum > 0)
		m_tvDocNum--;
	while(m_tvDocNum > 0 && m_pIndexReader->isDeleted(m_tvDocNum))
		m_tvDocNum--;	
	displayTermVector(m_tvDocNum);
	UpdateData(FALSE);
}

void CTermVectorDlg::OnBnClickedButtonTvnextdoc()
{
	if(m_tvDocNum < (int)(m_pIndexReader->maxDoc()-1)) 
		m_tvDocNum++;
	while( (m_tvDocNum < (int)(m_pIndexReader->maxDoc()-1)) && m_pIndexReader->isDeleted(m_tvDocNum))
		m_tvDocNum++;
	displayTermVector(m_tvDocNum);
	UpdateData(FALSE);
}

void CTermVectorDlg::OnBnClickedButtonTvlastdoc()
{
	m_tvDocNum = m_pIndexReader->maxDoc() - 1;
	while(m_tvDocNum > 0 && m_pIndexReader->isDeleted(m_tvDocNum))
		m_tvDocNum--;	
	displayTermVector(m_tvDocNum);
	UpdateData(FALSE);
}

void CTermVectorDlg::OnCbnSelchangeComboTvfield()
{
	if(!m_pIndexReader)
		return ;
	if(!m_pTVReader)
	{
		m_pTVReader = m_pIndexReader->getForwardIndexReader();
		if(!m_pTVReader)
		{
			AfxMessageBox(_T("获取前向索引错误!"));
			return ;
		}	
	}	
	
	UpdateData(TRUE);
	CFieldsInfo& fieldsInfo = m_pIndexReader->getFieldsInfo();
	CFieldInfo* pFieldInfo = fieldsInfo.getField((LPCTSTR)m_strComboField);
	if(!pFieldInfo)
	{
		EnableControls(FALSE);
	}
	if(pFieldInfo->isSequenceVector())
	{
		m_tvType = 0;
		EnableControls(TRUE);		
		EnableRadios(m_tvType);
		GetDlgItem(IDC_STATIC_TVTIP)->SetWindowText(_T("格式：词ID,词ID,..."));
	}
	else if(pFieldInfo->isFreqVector())
	{
		m_tvType = 1;
		EnableControls(TRUE);
		EnableRadios(m_tvType);
		GetDlgItem(IDC_STATIC_TVTIP)->SetWindowText(_T("格式：<词ID;词频>"));
	}

	else if(pFieldInfo->isPositionVector())
	{
		m_tvType = 2;
		EnableControls(TRUE);
		EnableRadios(m_tvType);
	}
	else
	{
		m_tvType = -1;
		EnableControls(FALSE);
		EnableRadios(m_tvType);
	}
	UpdateData(FALSE);
}
