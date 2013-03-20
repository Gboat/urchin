// SchemaDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Fxist.h"
#include "SchemaDlg.h"
#include ".\schemadlg.h"


// CSchemaDlg 对话框

IMPLEMENT_DYNAMIC(CSchemaDlg, CDialog)
CSchemaDlg::CSchemaDlg(Configurator* pArg,CWnd* pParent /*=NULL*/)
	: CDialog(CSchemaDlg::IDD, pParent)
	, m_sFieldName(_T(""))
	, m_sFieldAlias(_T(""))
	, m_fBoost(1.0)
	, m_pConfigurator(pArg)
	, m_nIndexLevel(0)
	, m_bDirty(FALSE)
	, m_nIndex(0)
	, m_nStore(0)
	, m_nTermVector(0)
	, m_strIndexer(_T(""))
	, m_strAnalyzer(_T(""))
	, m_sDocTag(_T(""))
{
	if(m_pConfigurator)
	{
		if(m_pConfigurator->getSchema())
			m_pDocSchema = new document::CDocumentSchema(*(m_pConfigurator->getSchema()));
		else 
			m_pDocSchema = NULL;
	}
}

CSchemaDlg::~CSchemaDlg()
{
	if(m_pDocSchema)
	{
		delete m_pDocSchema;
		m_pDocSchema = NULL;
	}
}

void CSchemaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FIELDNAME, m_sFieldName);
	DDX_Text(pDX, IDC_EDIT_ALIAS, m_sFieldAlias);
	DDX_Control(pDX, IDC_COMBO_INDEXER, m_comboCtrlIndexer);
	DDX_Control(pDX, IDC_COMBO_FIELDANALYZER, m_comboCtrlAnalyzer);
	DDX_Control(pDX, IDC_COMBO_STORE, m_comboCtrlStore);
	DDX_Control(pDX, IDC_COMBO_INDEX, m_comboCtrlIndex);
	DDX_Control(pDX, IDC_COMBO_FORWARDINDEX, m_comboCtrlTermVector);
	DDX_Text(pDX, IDC_EDIT_FIELDBOOST, m_fBoost);
	DDX_Control(pDX, IDC_LIST_SCHEMA, m_ctrlListSchema);
	DDX_CBIndex(pDX, IDC_COMBO_SCHEMAINDEXINGLEVEL, m_nIndexLevel);
	DDX_CBIndex(pDX, IDC_COMBO_INDEX, m_nIndex);
	DDX_CBIndex(pDX, IDC_COMBO_STORE, m_nStore);
	DDX_CBIndex(pDX, IDC_COMBO_FORWARDINDEX, m_nTermVector);
	DDX_CBString(pDX, IDC_COMBO_INDEXER, m_strIndexer);
	DDX_CBString(pDX, IDC_COMBO_FIELDANALYZER, m_strAnalyzer);
	DDX_Text(pDX, IDC_EDIT_DOCTAG, m_sDocTag);
}


BEGIN_MESSAGE_MAP(CSchemaDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SCMODIFY, OnBnClickedButtonScmodify)
	ON_BN_CLICKED(IDC_BUTTON_SCADD, OnBnClickedButtonScadd)
	ON_BN_CLICKED(IDC_BUTTON_SCDEL, OnBnClickedButtonScdel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SCHEMA, OnLvnItemchangedListSchema)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CSchemaDlg 消息处理程序

BOOL CSchemaDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctrlListSchema.InsertColumn(0,"字段名称",LVCFMT_LEFT,80);
	m_ctrlListSchema.InsertColumn(1,"字段别名",LVCFMT_LEFT,80);
	m_ctrlListSchema.InsertColumn(2,"索引器",LVCFMT_LEFT,80);	
	m_ctrlListSchema.InsertColumn(3,"分析器",LVCFMT_LEFT,80);		
	m_ctrlListSchema.InsertColumn(4,"索引级别",LVCFMT_LEFT,80);	
	m_ctrlListSchema.InsertColumn(5,"索引属性",LVCFMT_LEFT,80);	
	m_ctrlListSchema.InsertColumn(6,"存储属性",LVCFMT_LEFT,80);	
	m_ctrlListSchema.InsertColumn(7,"前向索引属性",LVCFMT_LEFT,80);	
	m_ctrlListSchema.InsertColumn(8,"重要度",LVCFMT_LEFT,80);	
	
	m_ctrlListSchema.SetExtendedStyle(m_ctrlListSchema.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	if((!_tcsicmp(m_pConfigurator->collection.identifier.c_str(),_T("collection.trec")))
		|| (!_tcsicmp(m_pConfigurator->collection.identifier.c_str(),_T("collection.trec.gz")))
		|| (!_tcsicmp(m_pConfigurator->collection.identifier.c_str(),_T("collection.trec.zip"))) 
		|| (!_tcsicmp(m_pConfigurator->collection.identifier.c_str(),_T("collection.tw"))) 
		|| (!_tcsicmp(m_pConfigurator->collection.identifier.c_str(),_T("collection.tw.gz"))) )
	{
		GetDlgItem(IDC_EDIT_DOCTAG)->EnableWindow(TRUE);		
		if(!m_pConfigurator->collection.parameter.empty() && !_tcsncmp(m_pConfigurator->collection.parameter.c_str(),_T("doc_tag="),8))
		{			
			m_sDocTag = m_pConfigurator->collection.parameter.c_str() + 8;
		}
		else 
		{
			if( (!_tcsicmp(m_pConfigurator->collection.identifier.c_str(),_T("collection.tw"))) 
				|| (!_tcsicmp(m_pConfigurator->collection.identifier.c_str(),_T("collection.gztw"))) )
                m_sDocTag = _T("version");
			else m_sDocTag = _T("DOC");
		}
	}
	else 
	{
		GetDlgItem(IDC_EDIT_DOCTAG)->EnableWindow(FALSE);
	}
	UpdateData(FALSE);

	UpdateSchemaData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CSchemaDlg::UpdateSchemaData(BOOL bSave)
{
	if(bSave)
	{
		if(m_ctrlListSchema.GetItemCount() == 0)
		{			
			if(m_pConfigurator->getSchema())
				m_pConfigurator->getSchema()->clear();
		}
	}
	else 
	{
		if(!m_pDocSchema)
			m_ctrlListSchema.DeleteAllItems();
		else 
		{			
			m_ctrlListSchema.DeleteAllItems();
			int nItem = 0;
			m_pDocSchema->startIterator();
			while (m_pDocSchema->hasNext())
			{
				document::CSchemaItem* pItem = m_pDocSchema->next();
				m_ctrlListSchema.InsertItem(nItem,pItem->getName());
				m_ctrlListSchema.SetItemData(nItem,(DWORD_PTR)pItem);
				UpdateSchemaItem(nItem,pItem);
				nItem++;
			}					
		}
	}
}
void CSchemaDlg::UpdateSchemaItem(int nItem,document::CSchemaItem* pSchemaItem)
{
	if(!pSchemaItem)
	{
		if(nItem >= m_ctrlListSchema.GetItemCount())
			return;
		pSchemaItem = (document::CSchemaItem*)m_ctrlListSchema.GetItemData(nItem);
	}
	

	m_ctrlListSchema.SetItemText(nItem,0,pSchemaItem->getName());
	m_ctrlListSchema.SetItemText(nItem,1,pSchemaItem->getAlias());
	m_ctrlListSchema.SetItemText(nItem,2,pSchemaItem->getComponent(_T("indexer")));
	m_ctrlListSchema.SetItemText(nItem,3,pSchemaItem->getComponent(_T("analyzer")));

	if(pSchemaItem->getIndex() != Index::NO)
	{
		INDEX_LEVEL il = pSchemaItem->getIndexLevel();
		if(il == WORD_LEVEL)
			m_ctrlListSchema.SetItemText(nItem,4,_T("WORD_LEVEL"));
		else if(il == DOCUMENT_LEVEL)
			m_ctrlListSchema.SetItemText(nItem,4,_T("DOCUMENT_LEVEL"));
		else m_ctrlListSchema.SetItemText(nItem,4,_T("DOCUMENT_LEVEL_0"));	
	}
	else 
	{
		m_ctrlListSchema.SetItemText(nItem,4,_T(""));
	}

	if(pSchemaItem->getIndex() == Index::ANALYZE)
		m_ctrlListSchema.SetItemText(nItem,5,_T("ANALYZE"));
	else if(pSchemaItem->getIndex() == Index::UN_ANALYZE)
		m_ctrlListSchema.SetItemText(nItem,5,_T("UN_ANALYZE"));
	else m_ctrlListSchema.SetItemText(nItem,5,_T("NO"));

	if(pSchemaItem->getStore() == Store::YES)
		m_ctrlListSchema.SetItemText(nItem,6,_T("YES"));
	else if(pSchemaItem->getStore() == Store::COMPRESS)
		m_ctrlListSchema.SetItemText(nItem,6,_T("COMPRESS"));
	else m_ctrlListSchema.SetItemText(nItem,6,_T("NO"));

	if(pSchemaItem->getForwardIndex() == ForwardIndex::SEQUENCE)
		m_ctrlListSchema.SetItemText(nItem,7,_T("SEQUENCE"));
	else if(pSchemaItem->getForwardIndex() == ForwardIndex::FREQ)
		m_ctrlListSchema.SetItemText(nItem,7,_T("FREQ"));
	else if(pSchemaItem->getForwardIndex() == ForwardIndex::POSITION)
		m_ctrlListSchema.SetItemText(nItem,7,_T("POSITION"));
	else m_ctrlListSchema.SetItemText(nItem,7,_T("NO"));

	float fBoost = pSchemaItem->getBoost();
	CString str;
	str.Format(_T("%.2f"),fBoost);
	m_ctrlListSchema.SetItemText(nItem,8,str);
	nItem++;
}

void CSchemaDlg::OnBnClickedButtonScmodify()
{
	if(m_ctrlListSchema.GetSelectedCount() == 0)
	{
		AfxMessageBox(_T("请选择要修改的项."));
		return;
	}
	POSITION pos = m_ctrlListSchema.GetFirstSelectedItemPosition();
	int nCurSel = m_ctrlListSchema.GetNextSelectedItem(pos);
	document::CSchemaItem* pSchemaItem = (document::CSchemaItem*)m_ctrlListSchema.GetItemData(nCurSel);
	ASSERT(pSchemaItem);

	UpdateData(TRUE);

	index_t index;
	store_t store;
	forwardindex_t tv;
	if(m_nIndex == 0)
		index = Index::NO;
	else if(m_nIndex == 1)
		index = Index::ANALYZE;
	else 
		index = Index::UN_ANALYZE;

	if(m_nStore == 0)
		store = Store::NO;
	else if(m_nStore == 1)
		store = Store::YES;
	else 
		store = Store::COMPRESS;

	if(m_nTermVector == 0)
		tv = ForwardIndex::NO;
	else if(m_nTermVector == 1)
		tv = ForwardIndex::SEQUENCE;
	else if(m_nTermVector == 2)
		tv = ForwardIndex::FREQ;
	else 
		tv = ForwardIndex::POSITION;
	pSchemaItem->setName((LPCTSTR)m_sFieldName);
	if(m_sFieldAlias != _T(""))
		pSchemaItem->setAlias((LPCTSTR)m_sFieldAlias);

	pSchemaItem->setIndex(index);
	pSchemaItem->setStore(store);
	pSchemaItem->setForwardIndex(tv);
	if(m_fBoost > 0.0)
		pSchemaItem->setBoost(m_fBoost);
	if(index != Index::NO)
	{
		if(m_nIndexLevel == 1)
			pSchemaItem->setIndexLevel(WORD_LEVEL);
		else if(m_nIndexLevel == 2)
			pSchemaItem->setIndexLevel(DOCUMENT_LEVEL);
		else if(m_nIndexLevel == 3)
			pSchemaItem->setIndexLevel(DOCUMENT_LEVEL_0);
		if(m_strIndexer != _T(""))
			pSchemaItem->setComponent(_T("indexer"),(LPCTSTR)m_strIndexer);
		if(m_strAnalyzer != _T(""))
			pSchemaItem->setComponent(_T("analyzer"),(LPCTSTR)m_strAnalyzer);

	}
	UpdateSchemaItem(nCurSel);
	m_pDocSchema->updateID();
	m_bDirty = TRUE;
}

void CSchemaDlg::OnBnClickedButtonScadd()
{
	UpdateData(TRUE);
	if(m_sFieldName == _T(""))
	{
		AfxMessageBox(_T("请至少指定字段名称!"));
		return;
	}	
	if(!m_pDocSchema)
	{
		m_pDocSchema = new document::CDocumentSchema();		
	}
	if(m_pDocSchema->findSchema((LPCTSTR)m_sFieldName))
	{
		AfxMessageBox(_T("该字段已存在!"));
		return;
	}
	UpdateData(TRUE);

	index_t index;
	store_t store;
	forwardindex_t tv;
	if(m_nIndex == 0)
		index = Index::NO;
	else if(m_nIndex == 1)
		index = Index::ANALYZE;
	else 
		index = Index::UN_ANALYZE;

	if(m_nStore == 0)
		store = Store::NO;
	else if(m_nStore == 1)
		store = Store::YES;
	else 
		store = Store::COMPRESS;

	if(m_nTermVector == 0)
		tv = ForwardIndex::NO;
	else if(m_nTermVector == 1)
		tv = ForwardIndex::SEQUENCE;
	else if(m_nTermVector == 2)
		tv = ForwardIndex::FREQ;
	else 
		tv = ForwardIndex::POSITION;
		
	m_pDocSchema->addItem(m_sFieldName,index,store,tv);
	document::CSchemaItem* pSchemaItem = m_pDocSchema->findSchema((LPCTSTR)m_sFieldName);
	if(m_fBoost > 0.0)
		pSchemaItem->setBoost(m_fBoost);
	if(m_sFieldAlias != _T(""))
		pSchemaItem->setAlias((LPCTSTR)m_sFieldAlias);
	if(index != Index::NO)
	{
		if(m_nIndexLevel == 1)
			pSchemaItem->setIndexLevel(WORD_LEVEL);
		else if(m_nIndexLevel == 2)
			pSchemaItem->setIndexLevel(DOCUMENT_LEVEL);
		else if(m_nIndexLevel == 3)
			pSchemaItem->setIndexLevel(DOCUMENT_LEVEL_0);
		if(m_strIndexer != _T(""))
			pSchemaItem->setComponent(_T("indexer"),(LPCTSTR)m_strIndexer);
		if(m_strAnalyzer != _T(""))
			pSchemaItem->setComponent(_T("analyzer"),(LPCTSTR)m_strAnalyzer);
	}
	int nItem = m_ctrlListSchema.GetItemCount();
	m_ctrlListSchema.InsertItem(nItem,pSchemaItem->getName());
	m_ctrlListSchema.SetItemData(nItem,(DWORD_PTR)pSchemaItem);
	UpdateSchemaItem(nItem);
	m_bDirty = TRUE;
}

void CSchemaDlg::OnBnClickedButtonScdel()
{
	if(m_ctrlListSchema.GetSelectedCount() == 0)
	{
		AfxMessageBox(_T("请选择要删除的项."));
		return;
	}
	if(AfxMessageBox(_T("确实要删除该项吗？"),MB_YESNO) == IDYES)
	{
		POSITION pos = m_ctrlListSchema.GetFirstSelectedItemPosition();
		int nCurSel = m_ctrlListSchema.GetNextSelectedItem(pos);
		document::CSchemaItem* pSchemaItem = (document::CSchemaItem*)m_ctrlListSchema.GetItemData(nCurSel);
		ASSERT(pSchemaItem);
		m_ctrlListSchema.DeleteItem(nCurSel);
		m_pDocSchema->deleteItem(pSchemaItem->getName());
		m_bDirty = TRUE;
	}	
}

void CSchemaDlg::OnBnClickedOk()
{
	if(m_bDirty)
		m_pConfigurator->setSchema(new document::CDocumentSchema(*m_pDocSchema));
	UpdateData(TRUE);
	if((!_tcsicmp(m_pConfigurator->collection.identifier.c_str(),_T("collection.trec")))
		|| (!_tcsicmp(m_pConfigurator->collection.identifier.c_str(),_T("collection.trec.gz")))
		|| (!_tcsicmp(m_pConfigurator->collection.identifier.c_str(),_T("collection.trec.zip"))) )
	{
		if(m_sDocTag != _T("DOC"))
		{			
			m_pConfigurator->collection.parameter = _T("doc_tag=");
			m_pConfigurator->collection.parameter += (LPCTSTR)m_sDocTag;		
		}		
	}

	OnOK();
}

void CSchemaDlg::OnLvnItemchangedListSchema(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(m_ctrlListSchema.GetSelectedCount() > 0)
	{
		POSITION pos = m_ctrlListSchema.GetFirstSelectedItemPosition();
		int nCurSel = m_ctrlListSchema.GetNextSelectedItem(pos);
		document::CSchemaItem* pSchemaItem = (document::CSchemaItem*)m_ctrlListSchema.GetItemData(nCurSel);
		m_sFieldName = pSchemaItem->getName();
		if(pSchemaItem->getAlias())
			m_sFieldAlias = pSchemaItem->getAlias();
		else m_sFieldAlias = _T("");
		if(pSchemaItem->getComponent(_T("indexer")))
			m_strIndexer = pSchemaItem->getComponent(_T("indexer"));
		else m_strIndexer = _T("");
		if(pSchemaItem->getComponent(_T("analyzer")))
			m_strAnalyzer = pSchemaItem->getComponent(_T("analyzer"));
		else m_strAnalyzer = _T("");
		if(pSchemaItem->getIndex() != Index::NO)
		{
			if(pSchemaItem->getIndexLevel() == WORD_LEVEL)
				m_nIndexLevel = 1;
			else if(pSchemaItem->getIndexLevel() == DOCUMENT_LEVEL)
				m_nIndexLevel = 2;
			else if(pSchemaItem->getIndexLevel() == DOCUMENT_LEVEL_0)
				m_nIndexLevel = 3;
			else m_nIndexLevel = 0;
		}
		else m_nIndexLevel = 0;

		if(pSchemaItem->getIndex() == Index::ANALYZE)
			m_nIndex = 1;
		else if(pSchemaItem->getIndex() == Index::UN_ANALYZE)
			m_nIndex = 2;
		else m_nIndex = 0;

		if(pSchemaItem->getStore() == Store::YES)
			m_nStore = 1;
		else if(pSchemaItem->getStore() == Store::COMPRESS)
			m_nStore = 2;
		else m_nStore = 0;

		if(pSchemaItem->getForwardIndex() == ForwardIndex::SEQUENCE)
			m_nTermVector = 1;
		else if(pSchemaItem->getForwardIndex() == ForwardIndex::FREQ)
			m_nTermVector = 2;
		else if(pSchemaItem->getForwardIndex() == ForwardIndex::POSITION)
			m_nTermVector = 3;
        else m_nTermVector = 0;
		UpdateData(FALSE);

	}
	*pResult = 0;
}

void CSchemaDlg::OnBnClickedCancel()
{
	if(m_bDirty)
	{
		if(AfxMessageBox(_T("您已经做了修改，真的要放弃修改吗？"),MB_YESNO) == IDYES)
			OnCancel();
	}		
	else OnCancel();
}
