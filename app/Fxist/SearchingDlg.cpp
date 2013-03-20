// SearchingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Fxist.h"
#include "SearchingDlg.h"
#include ".\searchingdlg.h"
#include "FxistView.h"
#include "FxistDoc.h"
#include "AnalyzerOptionDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CSearchingDlg 对话框

IMPLEMENT_DYNCREATE(CSearchingDlg, CChildDialog)
CSearchingDlg::CSearchingDlg(CWnd* pParent /*=NULL*/)
	: CChildDialog(CSearchingDlg::IDD, pParent)
	, m_strQueryExp(_T(""))
	, m_nTopk(100)
	, m_strSearchDefField(_T(""))
	, m_uSearchMem(0)
	, m_strAnalyzer(_T(""))
{
	m_pAnalyzer = NULL;
}

CSearchingDlg::~CSearchingDlg()
{
	if(m_pAnalyzer != NULL)
	{
		delete m_pAnalyzer;
		m_pAnalyzer = NULL; 
	}	

}

void CSearchingDlg::DoDataExchange(CDataExchange* pDX)
{
	CChildDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_QUERYHITS, m_ctrlListQueryHits);
	DDX_Control(pDX, IDC_COMBO_SEARCH_ANALYZER, m_ctrlComboSearchAnalyzer);
	DDX_Text(pDX, IDC_EDIT_QUERYEXP, m_strQueryExp);
	DDX_Text(pDX, IDC_EDIT_TOPK, m_nTopk);
	DDV_MinMaxUInt(pDX, m_nTopk, 1, 10000);
	DDX_Control(pDX, IDC_COMBO_SEARCHDEFFIELD, m_ctrlSearchDefField);
	DDX_CBString(pDX, IDC_COMBO_SEARCHDEFFIELD, m_strSearchDefField);
	DDX_Text(pDX, IDC_EDIT_SEARCHMEM, m_uSearchMem);
	DDV_MinMaxUInt(pDX, m_uSearchMem, 1, 10000);
	DDX_CBString(pDX, IDC_COMBO_SEARCH_ANALYZER, m_strAnalyzer);
}


BEGIN_MESSAGE_MAP(CSearchingDlg, CChildDialog)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnBnClickedButtonSearch)
	ON_EN_KILLFOCUS(IDC_EDIT_SEARCHMEM, OnEnKillfocusEditSearchmem)
	ON_BN_CLICKED(IDC_BUTTON_ANA_OPTION, OnBnClickedButtonAnaOption)
	ON_CBN_SELCHANGE(IDC_COMBO_SEARCHDEFFIELD, OnCbnSelchangeComboSearchdeffield)
END_MESSAGE_MAP()


// CSearchingDlg 消息处理程序

BOOL CSearchingDlg::OnInitDialog()
{
	CChildDialog::OnInitDialog();


	AddAnchor(IDC_GROUP41,TOP_LEFT,TOP_RIGHT);
	AddAnchor(IDC_LIST_QUERYHITS,TOP_LEFT,BOTTOM_RIGHT);
	AddAnchor(IDC_GROUP42,TOP_LEFT,BOTTOM_RIGHT);

	m_ctrlListQueryHits.InsertColumn(0,"序号",LVCFMT_LEFT,50);
	m_ctrlListQueryHits.InsertColumn(1,"打分",LVCFMT_LEFT,50);
	m_ctrlListQueryHits.InsertColumn(2,"文档编号",LVCFMT_LEFT,90);

	m_ctrlListQueryHits.SetExtendedStyle(m_ctrlListQueryHits.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	EnableControls(FALSE);
	m_uSearchMem = (UINT)(GlobalConfig.Search.memory/1000000);

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_TOPK))->SetRange32(1,10000);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_SEARCHMEM))->SetRange32(1,10000);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSearchingDlg::OnOpenIndex()
{
	CFxistDoc*	pDoc = ((CFxistView*)GetParent())->GetDocument();	
	search::CIndexSearcher* pSearcher = pDoc->GetIndexSearcher();
	firtex::search::CQueryParser* pQueryParser = pSearcher->getQueryParser();
	CFieldsInfo& fieldsInfo = pDoc->GetIndexReader()->getFieldsInfo();
	CFieldInfo* pFieldInfo;
	int nItem = 0;
	fieldsInfo.startIterator();
	while (fieldsInfo.hasNext())
	{
		pFieldInfo = fieldsInfo.next();
		if(pFieldInfo->isIndexed())
		{			
			m_ctrlSearchDefField.AddString(pFieldInfo->getName());			
			m_ctrlComboSearchAnalyzer.AddString(pQueryParser->getAnalyzer(pFieldInfo->getName())->getIdentifier());
			m_ctrlSearchDefField.SetItemData(nItem,(DWORD_PTR)nItem);
			nItem++;
		}
	}
	if(nItem > 0)
	{
		m_ctrlSearchDefField.SetCurSel(0);
		m_ctrlComboSearchAnalyzer.SetCurSel(0);
	}

	EnableControls(TRUE);
}
void CSearchingDlg::OnCloseIndex()
{
	m_ctrlListQueryHits.DeleteAllItems();
	m_ctrlSearchDefField.ResetContent();
	EnableControls(FALSE);
}

void CSearchingDlg::OnBnClickedButtonSearch()
{
	UpdateData(TRUE);
	if(m_strQueryExp == "")
	{
		AfxMessageBox("请输入查询表达式。");
		return;
	}
	/*if(!m_pAnalyzer)
	{
		m_pAnalyzer = CAnalyzerFactory::createAnalyzer((LPCTSTR)m_strAnalyzer,(LPCTSTR)m_strAnalyzerParam);
		if(!m_pAnalyzer)
		{
			CString str = _T("创建内容分析器失败：") + m_strAnalyzer;
			AfxMessageBox(str);
			return;
		}
	}*/

	CFxistDoc*	pDoc = ((CFxistView*)GetParent())->GetDocument();
	search::CIndexSearcher* pSearcher = pDoc->GetIndexSearcher();
	CString str;

	try
	{
		firtex::search::CQueryHits* pHits = pSearcher->search((LPCTSTR)m_strQueryExp,NULL,m_nTopk);
		m_ctrlListQueryHits.DeleteAllItems();
		while (m_ctrlListQueryHits.GetHeaderCtrl()->GetItemCount() > 0)
		{
			m_ctrlListQueryHits.DeleteColumn(0);
		}
		m_ctrlListQueryHits.InsertColumn(0,"序号",LVCFMT_LEFT,50);
		m_ctrlListQueryHits.InsertColumn(1,"打分",LVCFMT_LEFT,50);
		m_ctrlListQueryHits.InsertColumn(2,"文档编号",LVCFMT_LEFT,90);
		int col = 3;
		if(pHits)
		{
			str.Format("%u",pHits->getHitNum());
			GetDlgItem(IDC_STATIC_TOTALHITS)->SetWindowText(str);		
			str.Format("%u",pHits->length());
			GetDlgItem(IDC_STATIC_RETNUMS)->SetWindowText(str);
			pHits->startIterator();
			document::CDocument* pDoc;
			firtex::search::CQueryHits::CHitDoc* pHitDoc;
			CField* pField;
			int order = 0;
			int sub = 1;
			while (pHits->hasNext())
			{			
				pHitDoc = pHits->nextHitDoc();
				pDoc = pHitDoc->doc();
				if(pDoc)
				{					
					order = m_ctrlListQueryHits.GetItemCount();
					str.Format("%u",order);
					m_ctrlListQueryHits.InsertItem(order,str);
					sub = 1;
					str.Format("%.4f",pHitDoc->score());
					m_ctrlListQueryHits.SetItemText(order,sub++,str);
					str.Format("%u",pDoc->getDocID());				
					m_ctrlListQueryHits.SetItemText(order,sub++,str);				
					pDoc->startIterator();
					while (pDoc->hasNext())
					{
						pField = pDoc->next();	
						if(col!=0)
							m_ctrlListQueryHits.InsertColumn(col++,pField->getName().c_str(),LVCFMT_LEFT,90);
						if(pField->getType() & CField::TEXT)
							m_ctrlListQueryHits.SetItemText(order,sub++,(const char*)*(pField->textAValue()));
						else if(pField->getType() & CField::NUMBER)
						{
							//if(pField->getFieldType()==FIELD_DATE)
							{
								m_ctrlListQueryHits.SetItemText(order,sub++,CDateTime::format(pField->numberValue().value.i64val).c_str());
							}
						}
					}										
					delete pDoc;
					col = 0;
				}
			}	
			delete pHits;
		}	
		else 
		{
			str.Format("%u",0);
			GetDlgItem(IDC_STATIC_TOTALHITS)->SetWindowText(str);		
			str.Format("%u",0);
			GetDlgItem(IDC_STATIC_RETNUMS)->SetWindowText(str);
		}
	}
	catch(CFirteXException& fe)
	{
		CString str;
		str.Format(_T("查询失败：%s"),fe.what());
		AfxMessageBox(str);
	}	
}

void CSearchingDlg::EnableControls(BOOL bEnable)
{
	GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(bEnable);
}

void CSearchingDlg::OnEnKillfocusEditSearchmem()
{
	UpdateData(TRUE);
	GlobalConfig.Search.memory = m_uSearchMem*1000000; 
}

void CSearchingDlg::OnBnClickedButtonAnaOption()
{
	CAnalyzerOptionDlg dlg((LPCTSTR)m_strAnalyzerParam);
	if(dlg.DoModal() == IDOK)
	{
		m_strAnalyzerParam = dlg.getOption();
		if(m_pAnalyzer)
		{
			delete m_pAnalyzer;
			m_pAnalyzer = NULL;
		}
	}
}

void CSearchingDlg::OnCbnSelchangeComboSearchdeffield()
{
	int nCurSel = m_ctrlSearchDefField.GetCurSel();
	int nAnalyzer = (int)m_ctrlSearchDefField.GetItemData(nCurSel);
	m_ctrlComboSearchAnalyzer.SetCurSel(nAnalyzer);
}
