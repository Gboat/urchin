// DocumentsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Fxist.h"
#include "DocumentsDlg.h"
#include ".\documentsdlg.h"
#include "FxistView.h"
#include "FxistDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CDocumentsDlg 对话框

IMPLEMENT_DYNCREATE(CDocumentsDlg, CChildDialog)
CDocumentsDlg::CDocumentsDlg(CWnd* pParent /*=NULL*/)
	: CChildDialog(CDocumentsDlg::IDD, pParent)
	, m_nDocID(0)
	, m_nCurDocID(0)
	, m_fBoost(1.0)
	, m_strQueryTerm(_T(""))
	, m_nDF(0)
	, m_nTF(0)
	, m_nAnalyzer(0)
	, m_strQueryField(_T(""))
{
	m_pCurTermDocs = NULL;
}

CDocumentsDlg::~CDocumentsDlg()
{
	m_pIndex = NULL;
	if (m_pCurTermDocs)
	{
		delete m_pCurTermDocs;
		m_pCurTermDocs = NULL;
	}
	ClearCachedQueryResult();
}

void CDocumentsDlg::DoDataExchange(CDataExchange* pDX)
{
	CChildDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DOCRESULT, m_ctrlListDocResult);
	DDX_Text(pDX, IDC_EDIT_DOCNO, m_nDocID);
	DDV_MinMaxInt(pDX, m_nDocID, 0, 100000000);
	DDX_Text(pDX, IDC_EDIT_CURDOCNO, m_nCurDocID);
	DDX_Text(pDX, IDC_EDIT_BOOST, m_fBoost);
	DDX_Text(pDX, IDC_EDIT_QUERYWORD, m_strQueryTerm);
	DDX_Text(pDX, IDC_EDIT_DF, m_nDF);
	DDX_Text(pDX, IDC_EDIT_TF, m_nTF);
	DDX_CBIndex(pDX, IDC_COMBO_WANALYZER, m_nAnalyzer);
	DDX_CBString(pDX, IDC_COMBO_INDEXFIELD, m_strQueryField);
	DDX_Control(pDX, IDC_COMBO_INDEXFIELD, m_ctrlIndexFields);
	DDX_Control(pDX, IDC_COMBO_WANALYZER, m_ctrlAnalyzer);
}


BEGIN_MESSAGE_MAP(CDocumentsDlg, CChildDialog)
	ON_BN_CLICKED(IDC_BUTTON_DOCNOQUERY, OnBnClickedButtonDocnoquery)
	ON_BN_CLICKED(IDC_BUTTON_DFIRSTDOC, OnBnClickedButtonDfirstdoc)
	ON_BN_CLICKED(IDC_BUTTON_DPREVDOC, OnBnClickedButtonDprevdoc)
	ON_BN_CLICKED(IDC_BUTTON_DNEXTDOC, OnBnClickedButtonDnextdoc)
	ON_BN_CLICKED(IDC_BUTTON_DLASTDOC, OnBnClickedButtonDlastdoc)
	ON_BN_CLICKED(IDC_BUTTON_DELDOC, OnBnClickedButtonDeldoc)
	ON_BN_CLICKED(IDC_BUTTON_WORDQUERY, OnBnClickedButtonWordquery)
	ON_BN_CLICKED(IDC_BUTTON_WFIRSTDOC, OnBnClickedButtonWfirstdoc)
	ON_BN_CLICKED(IDC_BUTTON_WPREVDOC, OnBnClickedButtonWprevdoc)
	ON_BN_CLICKED(IDC_BUTTON_WNEXTDOC, OnBnClickedButtonWnextdoc)
	ON_BN_CLICKED(IDC_BUTTON_WLASTDOC, OnBnClickedButtonWlastdoc)
	ON_BN_CLICKED(IDC_BUTTON_WDELALLDOCS, OnBnClickedButtonWdelalldocs)
	ON_BN_CLICKED(IDC_BUTTON_UNDELALL, OnBnClickedButtonUndelall)
END_MESSAGE_MAP()


// CDocumentsDlg 消息处理程序

BOOL CDocumentsDlg::OnInitDialog()
{
	CChildDialog::OnInitDialog();

	
	AddAnchor(IDC_GROUP24,TOP_LEFT,TOP_RIGHT);	

	AddAnchor(IDC_LIST_DOCRESULT,TOP_LEFT,BOTTOM_RIGHT);

	m_ctrlListDocResult.InsertColumn(0,"字段",LVCFMT_LEFT,60);
	m_ctrlListDocResult.InsertColumn(1,"Boost",LVCFMT_LEFT,60);
	m_ctrlListDocResult.InsertColumn(2,"值",LVCFMT_LEFT,260);	

	firtex::analyzer::analyzer_list al = firtex::analyzer::CAnalyzerFactory::getRegisteredAnalyzers();
	int nItem = 0;
	firtex::analyzer::analyzer_list::iterator iter3 = al.begin();
	while (iter3 != al.end())
	{
		m_ctrlAnalyzer.InsertString(nItem,(*iter3).c_str());
		iter3++;
		nItem++;
	}
	
	EnableControls(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CDocumentsDlg::OnOpenIndex()
{
	CFxistDoc*	pDoc = ((CFxistView*)GetParent())->GetDocument();
	m_pIndex = pDoc->GetIndex();
	index::CIndexReader* pReader = m_pIndex->acquireReader();
	CFieldsInfo& fieldsInfo = pReader->getFieldsInfo();
	CFieldInfo* pFieldInfo;
	fieldsInfo.startIterator();
	while (fieldsInfo.hasNext())
	{
		pFieldInfo = fieldsInfo.next();
		if(pFieldInfo->isIndexed())
		{
			m_ctrlIndexFields.AddString(pFieldInfo->getName());
		}
	}
	if(m_ctrlIndexFields.GetCount()>0)
		m_ctrlIndexFields.SetCurSel(0);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_DOCNO))->SetRange32(0,pReader->maxDoc()-1);
	EnableControls(TRUE);
}
void CDocumentsDlg::OnCloseIndex()
{
	m_pIndex = NULL;
	if (m_pCurTermDocs)
	{
		delete m_pCurTermDocs;
		m_pCurTermDocs = NULL;
	}
	m_ctrlIndexFields.ResetContent();
	ClearCachedQueryResult();
	m_nDocID = 0;
	m_nCurDocID = 0;
	m_fBoost = 1.0;
	m_strQueryTerm = _T("");
	m_nDF = 0;
	m_nTF = 0;
    m_strQueryField = _T("");
	UpdateData(FALSE);
	EnableControls(FALSE);
}
void CDocumentsDlg::OnActivate()
{
	if(m_pIndex)
	{		
		CFxistDoc*	pDoc = ((CFxistView*)GetParent())->GetDocument();
		m_pIndex = pDoc->GetIndex();
	}
}
void CDocumentsDlg::DisplayResultDoc(QueryResultDoc* pResDoc)
{	
	DisplayDoc(pResDoc->doc);	
	m_nTF = pResDoc->tf;
	UpdateData(FALSE);
}
void CDocumentsDlg::DisplayDoc(document::CDocument* pDoc)
{
	m_ctrlListDocResult.DeleteAllItems();
	if(!pDoc)
		return ;
	int order = 0;
	CField* pField;
	CString str;	
	pDoc->startIterator();
	while (pDoc->hasNext())
	{
		pField = pDoc->next();
		order = m_ctrlListDocResult.GetItemCount();
		m_ctrlListDocResult.InsertItem(order,pField->getName().c_str());
		m_fBoost = pField->getBoost();
		str.Format("%.2f",m_fBoost);
		m_ctrlListDocResult.SetItemText(order,1,str);
		if(pField->getType() == CField::TEXT)
			m_ctrlListDocResult.SetItemText(order,2,*(pField->textAValue()));
		else if(pField->getType() & CField::NUMBER)
		{
			//if(pField->getFieldType()==FIELD_DATE)
			{
				m_ctrlListDocResult.SetItemText(order,2,CDateTime::format(pField->numberValue().value.i64val).c_str());				
			}
		}

	}	
}

void CDocumentsDlg::OnBnClickedButtonDocnoquery()
{	
	UpdateData(TRUE);

	index::CIndexReader* pReader = m_pIndex->acquireReader();
	if(pReader->isDeleted((docid_t)m_nDocID))
	{
		AfxMessageBox(_T("该文档已经删除!"));
		return ;
	}
	document::CDocument* pDoc = pReader->document((docid_t)m_nDocID);
	if(pDoc)
	{	
		DisplayDoc(pDoc);		
		delete pDoc;
		m_nCurDocID = m_nDocID;
	}
	else 
	{
		m_nCurDocID = -1;
		DisplayDoc(NULL);
	}
	UpdateData(FALSE);
}

void CDocumentsDlg::OnBnClickedButtonDfirstdoc()
{
	m_nDocID = 0;
	index::CIndexReader* pReader = m_pIndex->acquireReader();
	while (pReader->isDeleted((docid_t)m_nDocID))
	{
		m_nDocID++;
		if(m_nDocID >= pReader->maxDoc())
		{
			m_nCurDocID = -1;
			DisplayDoc(NULL);
			return;
		}
	}
	document::CDocument* pDoc = pReader->isDeleted((docid_t)m_nDocID)?NULL:pReader->document((docid_t)m_nDocID);
	if(pDoc)
	{	
		DisplayDoc(pDoc);		
		delete pDoc;
		m_nCurDocID = m_nDocID;
	}
	else 
	{
		m_nCurDocID = -1;
		DisplayDoc(NULL);
	}
	UpdateData(FALSE);
}

void CDocumentsDlg::OnBnClickedButtonDprevdoc()
{
	index::CIndexReader* pReader = m_pIndex->acquireReader();
	if(m_nDocID <= 0)
		return;
	do
	{
		m_nDocID--;
	}while (m_nDocID > 0 && pReader->isDeleted((docid_t)m_nDocID));

	if(m_nDocID == 0)
	{
		while(pReader->isDeleted((docid_t)m_nDocID))
			m_nDocID++;
	}
	
	if(m_nDocID < 0)
	{
		m_nCurDocID = -1;
		DisplayDoc(NULL);
		return;
	}	
	document::CDocument* pDoc = pReader->document((docid_t)m_nDocID);
	if(pDoc)
	{	
		DisplayDoc(pDoc);		
		delete pDoc;
		m_nCurDocID = m_nDocID;
	}
	else 
	{
		m_nCurDocID = -1;
		DisplayDoc(NULL);
	}
	UpdateData(FALSE);
}

void CDocumentsDlg::OnBnClickedButtonDnextdoc()
{
	index::CIndexReader* pReader = m_pIndex->acquireReader();

	if(m_nDocID >= (pReader->maxDoc()))
		return ;

	do 
	{
		m_nDocID++;
	} while((m_nDocID < (pReader->maxDoc())) && pReader->isDeleted((docid_t)m_nDocID));
	if(m_nDocID >= (pReader->maxDoc()))
	{
		m_nCurDocID = -1;
		DisplayDoc(NULL);
		return ;
	}
	document::CDocument* pDoc = pReader->document((docid_t)m_nDocID);
	if(pDoc)
	{	
		DisplayDoc(pDoc);		
		delete pDoc;
		m_nCurDocID = m_nDocID;
	}
	else 
	{
		m_nCurDocID = -1;
		DisplayDoc(NULL);
	}
	UpdateData(FALSE);
}

void CDocumentsDlg::OnBnClickedButtonDlastdoc()
{
	index::CIndexReader* pReader = m_pIndex->acquireReader();

	m_nDocID = pReader->maxDoc() - 1;
	while (pReader->isDeleted((docid_t)m_nDocID))
	{
		m_nDocID--;
		if(m_nDocID < 0)
		{
			m_nCurDocID = -1;
			DisplayDoc(NULL);
			return;
		}
	}
	document::CDocument* pDoc = pReader->document((docid_t)m_nDocID);
	if(pDoc)
	{	
		DisplayDoc(pDoc);		
		delete pDoc;
		m_nCurDocID = m_nDocID;
	}
	else 
	{
		m_nCurDocID = -1;
		DisplayDoc(NULL);
	}
	UpdateData(FALSE);
}

void CDocumentsDlg::OnBnClickedButtonDeldoc()
{
	CString str;
	UpdateData(TRUE);
	if(m_nDocID >= 0)
	{
		str.Format(_T("你确实要删除文档%d吗？"),m_nDocID);
		if(AfxMessageBox(str,MB_YESNO) == IDYES)
		{
			index::CIndexReader* pReader = m_pIndex->acquireReader();
			
			pReader->deleteDocument(m_nDocID);
	
		}
	}	
	else 
	{
		AfxMessageBox("无效的文档编号。");
	}
}

void CDocumentsDlg::OnBnClickedButtonUndelall()
{
	CString str;
	str.Format(_T("你确实要恢复所有删除的文档吗？"),m_nCurDocID);
	if(AfxMessageBox(str,MB_YESNO) == IDYES)
	{
		index::CIndexReader* pReader = m_pIndex->acquireReader();
		pReader->undeleteAll();

	}
}

void CDocumentsDlg::Query(LPCTSTR lpszField,LPCTSTR lpszTerm)
{	
	CStandardAnalyzer ca;

	ca.setMaxTokensToAnalyze(1000);

	CToken* t;
	CTerm term;
	
	CStringReader sr(lpszTerm);	
	CTokens* pTokens = ca.nextTokens(&sr);
	if(pTokens->getNumTokens() <= 0)
	{
		AfxMessageBox("分析器分析结果为空，你输入的查询词可能是停用词或无效字符.");
		return;
	}
	if(pTokens->getNumTokens() > 1)
	{
		//printf("The analyze result has more than one token,it can only process one token.\n");
		AfxMessageBox("分析器分析结果多于两个词，这里只能查询单个词，如要查询多词组合请进入检索页面.");
		return;
	}
	if(m_pCurTermDocs)
	{
		delete m_pCurTermDocs;
		m_pCurTermDocs = NULL;
	}	
	ClearCachedQueryResult();

	pTokens->startIterator();
	if(pTokens->hasNext())
	{
		firtex::index::CIndexReader* pReader = m_pIndex->acquireReader();

		t = pTokens->next();
		CTerm* pTerm = t->toTerm(lpszField);//TODO:		
		firtex::index::CTermReader* pTermReader = pReader->termReader();
		if(pTermReader->seek(pTerm))
		{
			m_pCurTermDocs = pTermReader->termDocFreqs();
			m_nDF = m_pCurTermDocs->docFreq();	
			EnableControls(TRUE);
			UpdateData(FALSE);
		}
		else 
		{
			AfxMessageBox("没有这个词的相关信息.");
		}
		delete pTerm;
		delete pTermReader;
	}		
}

void CDocumentsDlg::ClearCachedQueryResult()
{
	map<int,QueryResultDoc*>::iterator iter = m_queryResultCache.begin();
	while (iter != m_queryResultCache.end())
	{
		delete iter->second;
		iter++;
	}
	m_queryResultCache.clear();
}
void CDocumentsDlg::OnBnClickedButtonWordquery()
{
	UpdateData(TRUE);
	if(m_strQueryTerm != "")
		Query(m_strQueryField,m_strQueryTerm);
	else 
	{
		AfxMessageBox("请输入查询词");
	}
}

void CDocumentsDlg::OnBnClickedButtonWfirstdoc()
{	
	index::CIndexReader* pReader = m_pIndex->acquireReader();

	m_nCurDocID = 0;
	if(!m_queryResultCache.empty())
	{
		map<int,QueryResultDoc*>::iterator iter = m_queryResultCache.begin();

		if(iter->second->doc == NULL)
			iter->second->doc = pReader->document(iter->second->docid);
		DisplayResultDoc(iter->second);
	}
	else 
	{
		if(m_pCurTermDocs->next())
		{
			QueryResultDoc* doc = new QueryResultDoc();
			doc->docid = m_pCurTermDocs->doc();
			doc->tf = m_pCurTermDocs->freq();
			doc->doc = pReader->document(doc->docid);
			DisplayResultDoc(doc);
			m_queryResultCache.insert(make_pair(m_nCurDocID,doc));
		}
	}
}

void CDocumentsDlg::OnBnClickedButtonWprevdoc()
{
	if(m_nCurDocID == 0)
		return;
	m_nCurDocID--;

	index::CIndexReader* pReader = m_pIndex->acquireReader();
	map<int,QueryResultDoc*>::iterator iter = m_queryResultCache.find(m_nCurDocID);
	if(iter != m_queryResultCache.end())
	{
		if(iter->second->doc == NULL)
			iter->second->doc = pReader->document(iter->second->docid);
		DisplayResultDoc(iter->second);
	}
	else 
	{
		if(m_pCurTermDocs->next())
		{
			QueryResultDoc* doc = new QueryResultDoc();
			doc->docid = m_pCurTermDocs->doc();
			doc->tf = m_pCurTermDocs->freq();
			doc->doc = pReader->document(doc->docid);
			DisplayResultDoc(doc);
			m_nCurDocID = doc->docid;
			m_queryResultCache.insert(make_pair(m_nCurDocID,doc));
		}
	}
}

void CDocumentsDlg::OnBnClickedButtonWnextdoc()
{
	if(m_nCurDocID >= m_pCurTermDocs->docFreq())
		return;
	m_nCurDocID++;
	index::CIndexReader* pReader = m_pIndex->acquireReader();
	map<int,QueryResultDoc*>::iterator iter = m_queryResultCache.find(m_nCurDocID);
	if(iter != m_queryResultCache.end())
	{
		if(iter->second->doc == NULL)
			iter->second->doc = pReader->document(iter->second->docid);
		DisplayResultDoc(iter->second);
	}
	else 
	{
		if(m_pCurTermDocs->next())
		{
			QueryResultDoc* doc = new QueryResultDoc();
			doc->docid = m_pCurTermDocs->doc();
			doc->tf = m_pCurTermDocs->freq();
			doc->doc = pReader->document(doc->docid);
			DisplayResultDoc(doc);
			m_queryResultCache.insert(make_pair(m_nCurDocID,doc));
		}
	}
}

void CDocumentsDlg::OnBnClickedButtonWlastdoc()
{
	if(m_nCurDocID == m_pCurTermDocs->docFreq()-1)
		return;
	m_nCurDocID = m_pCurTermDocs->docFreq()-1;
	index::CIndexReader* pReader = m_pIndex->acquireReader();
	map<int,QueryResultDoc*>::iterator iter = m_queryResultCache.find(m_nCurDocID);
	if(iter != m_queryResultCache.end())
	{
		if(iter->second->doc == NULL)
			iter->second->doc = pReader->document(iter->second->docid);
		DisplayResultDoc(iter->second);
	}
	else 
	{
		if(m_pCurTermDocs->next())
		{
			QueryResultDoc* doc = new QueryResultDoc();
			doc->docid = m_pCurTermDocs->doc();
			doc->tf = m_pCurTermDocs->freq();
			doc->doc = pReader->document(doc->docid);
			DisplayResultDoc(doc);
			m_queryResultCache.insert(make_pair(m_nCurDocID,doc));
		}
	}
}

void CDocumentsDlg::OnBnClickedButtonWdelalldocs()
{
	AfxMessageBox(_T("尚未提供此功能"));
}
void CDocumentsDlg::EnableControls(BOOL bEnable)
{
	GetDlgItem(IDC_EDIT_DOCNO)->EnableWindow(bEnable);
	GetDlgItem(IDC_SPIN_DOCNO)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_DOCNOQUERY)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_DFIRSTDOC)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_DPREVDOC)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_DNEXTDOC)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_DLASTDOC)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_DELDOC)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_UNDELALL)->EnableWindow(bEnable);
	GetDlgItem(IDC_COMBO_INDEXFIELD)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_QUERYWORD)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_WORDQUERY)->EnableWindow(bEnable);
	GetDlgItem(IDC_COMBO_WANALYZER)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_WFIRSTDOC)->EnableWindow(bEnable && m_pCurTermDocs);
	GetDlgItem(IDC_BUTTON_WPREVDOC)->EnableWindow(bEnable && m_pCurTermDocs);
	GetDlgItem(IDC_BUTTON_WNEXTDOC)->EnableWindow(bEnable&& m_pCurTermDocs);
	GetDlgItem(IDC_BUTTON_WLASTDOC)->EnableWindow(bEnable&& m_pCurTermDocs);
	GetDlgItem(IDC_BUTTON_WDELALLDOCS)->EnableWindow(bEnable&& m_pCurTermDocs);
}
