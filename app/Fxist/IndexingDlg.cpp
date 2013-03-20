// IndexingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Fxist.h"
#include "IndexingDlg.h"
#include ".\indexingdlg.h"
#include "FxistView.h"
#include "MainFrm.h"
#include "FxistDoc.h"
#include "IndexingProgressDlg.h"
#include "SchemaDlg.h"
#include "HierMergeAdvanceDlg.h"
#include "AnalyzerOptionDlg.h"
#include "ParserOptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define  WM_INDEXINGCOMPLETE WM_USER + 1100

// CIndexingDlg 对话框

IMPLEMENT_DYNCREATE(CIndexingDlg, CChildDialog)
CIndexingDlg::CIndexingDlg(CWnd* pParent /*=NULL*/)
	: CChildDialog(CIndexingDlg::IDD, pParent)
	, m_strIndexSrc(_T(""))
	, m_strIndexDst(_T(""))
	, m_uMaxIndexMem(28)
	, m_uMaxIndexTerms(10000)
	, m_uMaxIndexBarrels(50)
	, m_bAutoOpen(FALSE)
	, m_pProgressDlg(NULL)
	, m_pConfigurator(new Configurator())
	, m_bAppend(FALSE)
	, m_nMergeMode(0)
	, m_bMergeDelDocs(FALSE)
	, m_bAutoOpenLogWindow(TRUE)
	, m_uCacheDocs(100)
	, m_nGCThreshold(0)
	, m_fGCRatio(0.1f)
	, m_uGCDelDocs(1000)
	, m_nAccessMode(0)
	, m_sIndexingLevel(_T("WORD_LEVEL"))
	, m_bSearchable(FALSE)
	, m_strCollection(_T(""))
	, m_strParser(_T(""))
	, m_strAnalyzer(_T(""))
{
}

CIndexingDlg::~CIndexingDlg()
{	
	if(m_pProgressDlg)
	{
		delete m_pProgressDlg;
		m_pProgressDlg = NULL;
	}
	if(m_pConfigurator)
	{
		delete m_pConfigurator;
		m_pConfigurator = NULL;
	}
}

void CIndexingDlg::DoDataExchange(CDataExchange* pDX)
{
	CChildDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COLLECTION, m_ctrlComboCollection);
	DDX_Text(pDX, IDC_EDIT_SOURCE, m_strIndexSrc);
	DDX_Text(pDX, IDC_EDIT_INDEXDST, m_strIndexDst);
	DDX_Control(pDX, IDC_COMBO_PARSER, m_ctrlComboParser);
	DDX_Control(pDX, IDC_COMBO_ANALYZER, m_ctrlComboAnalyzer);
	DDX_Text(pDX, IDC_EDIT_MAXMEM, m_uMaxIndexMem);
	DDV_MinMaxUInt(pDX, m_uMaxIndexMem, 1, 4096);
	DDX_Text(pDX, IDC_EDIT_MAXTERMS, m_uMaxIndexTerms);
	DDV_MinMaxUInt(pDX, m_uMaxIndexTerms, 10, 1000000);
	DDX_Control(pDX, IDC_COMBO_LOGLEVEL, m_ctrlComboLogLevel);
	DDX_Check(pDX, IDC_CHECK_AUTOOPEN, m_bAutoOpen);
	DDX_Radio(pDX, IDC_RADIO_NOMERGE, m_nMergeMode);	
	DDX_Check(pDX, IDC_CHECK_AUTOOPENLOG, m_bAutoOpenLogWindow);	
	DDX_Text(pDX, IDC_EDIT_CACHEDOCS, m_uCacheDocs);
	DDV_MinMaxUInt(pDX, m_uCacheDocs, 1, 1000000);
	DDX_Control(pDX, IDC_COMBO_INDEXINGLEVEL, m_ctrlComboIndexingLevel);
	DDX_Radio(pDX, IDC_RADIO_RATIO, m_nGCThreshold);
	DDX_Text(pDX, IDC_EDIT_RATIO, m_fGCRatio);
	DDX_Text(pDX, IDC_EDIT_DELNUM, m_uGCDelDocs);
	DDV_MinMaxUInt(pDX, m_uGCDelDocs, 1, 1000000);
	DDX_Radio(pDX, IDC_RADIO_WRITEMODE, m_nAccessMode);
	DDX_CBString(pDX, IDC_COMBO_INDEXINGLEVEL, m_sIndexingLevel);
	DDX_Check(pDX, IDC_CHECK_SEARCHABLE, m_bSearchable);
	DDX_CBString(pDX, IDC_COMBO_COLLECTION, m_strCollection);
	DDX_CBString(pDX, IDC_COMBO_PARSER, m_strParser);
	DDX_CBString(pDX, IDC_COMBO_ANALYZER, m_strAnalyzer);
}


BEGIN_MESSAGE_MAP(CIndexingDlg, CChildDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSESRC, OnBnClickedButtonBrowsesrc)
	ON_BN_CLICKED(IDC_BUTTON_INDEXDST, OnBnClickedButtonIndexdst)
	ON_BN_CLICKED(IDC_BUTTON_STARTINDEX, OnBnClickedButtonStartindex)

	ON_MESSAGE(WM_INDEXINGCOMPLETE,OnIndexingComplete)
	ON_CBN_SELCHANGE(IDC_COMBO_LOGLEVEL, OnCbnSelchangeComboLoglevel)
	ON_EN_KILLFOCUS(IDC_EDIT_MAXMEM, OnEnKillfocusEditMaxmem)
	ON_EN_KILLFOCUS(IDC_EDIT_MAXTERMS, OnEnKillfocusEditMaxterms)		
	ON_BN_CLICKED(IDC_BUTTON_OPENARG, OnBnClickedButtonOpenarg)
	ON_BN_CLICKED(IDC_BUTTON_SAVEARG, OnBnClickedButtonSavearg)
	ON_BN_CLICKED(IDC_BUTTON_SAVEASARG, OnBnClickedButtonSaveasarg)	
	ON_CBN_SELCHANGE(IDC_COMBO_PARSER, OnCbnSelchangeComboParser)
	ON_BN_CLICKED(IDC_RADIO_NOMERGE, OnBnClickedRadioNomerge)
	ON_BN_CLICKED(IDC_RADIO_HLMMERGE, OnBnClickedRadioHlmmerge)
	ON_BN_CLICKED(IDC_RADIO_OPTIMIZE, OnBnClickedRadioOptimize)
	ON_BN_CLICKED(IDC_RADIO_RATIO, OnBnClickedRadioRatio)
	ON_BN_CLICKED(IDC_RADIO_DELNUM, OnBnClickedRadioDelnum)	
	ON_BN_CLICKED(IDC_BUTTON_COLSCHEMA, OnBnClickedButtonColschema)
	ON_CBN_SELCHANGE(IDC_COMBO_COLLECTION, OnCbnSelchangeComboCollection)
	ON_BN_CLICKED(IDC_RADIO_FORCEGC, OnBnClickedRadioForcegc)
	ON_BN_CLICKED(IDC_BUTTON_HMADVANCE, OnBnClickedButtonHmadvance)
	ON_BN_CLICKED(IDC_BUTTON_ANALYZE_OPTION, OnBnClickedButtonAnalyzeOption)
	ON_BN_CLICKED(IDC_BUTTON_PARSER_OPTION, OnBnClickedButtonParserOption)
END_MESSAGE_MAP()


// CIndexingDlg 消息处理程序

void CIndexingDlg::OnBnClickedButtonBrowsesrc()
{
	int sel = m_ctrlComboCollection.GetCurSel();		
	if(sel == 1)//filelist
	{
		CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"列表文件(*.txt)|*.txt|所有文件(*.*)|*.*||");
		if(dlg.DoModal() == IDOK)
			m_strIndexSrc = dlg.GetPathName();
	}	
	else
	{
		CFxistDoc*	pDoc = ((CFxistView*)GetParent())->GetDocument();
		m_strIndexSrc = pDoc->BrowseForFolder();
	}
	UpdateData(FALSE);
}

void CIndexingDlg::OnBnClickedButtonIndexdst()
{
	CFxistDoc*	pDoc = ((CFxistView*)GetParent())->GetDocument();
	m_strIndexDst = pDoc->BrowseForFolder();
	UpdateData(FALSE);
}

BOOL CIndexingDlg::OnInitDialog()
{
	CChildDialog::OnInitDialog();

	m_ctrlComboCollection.SetCurSel(0);
	m_ctrlComboParser.SetCurSel(0);
	m_ctrlComboAnalyzer.SetCurSel(0);
	m_ctrlComboLogLevel.SetCurSel(2);


	m_uMaxIndexMem = (UINT)GlobalConfig.Index.memory/1000000;
	m_uMaxIndexTerms = (UINT)GlobalConfig.Index.maxIndexTerms;		

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_INDEXMEM))->SetRange32(1,4096);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_MAXTERMS))->SetRange32(10,1000000);	
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_CACHEDOCS))->SetRange32(1,1000000);	

	firtex::collection::collection_list cl = firtex::collection::CCollectionFactory::getRegisteredCollections();
	int nItem = 0;
	firtex::collection::collection_list::iterator iter = cl.begin();
	while (iter != cl.end())
	{
		m_ctrlComboCollection.InsertString(nItem,(*iter).c_str());
		iter++;
		nItem++;
	}
	m_ctrlComboCollection.SetCurSel(m_ctrlComboCollection.FindString(0,_T("collection.directory")));

	firtex::parser::parser_list pl = firtex::parser::CParserFactory::getRegisteredParsers();
	nItem = 0;
	firtex::parser::parser_list::iterator iter2 = pl.begin();
	while (iter2 != pl.end())
	{
		m_ctrlComboParser.InsertString(nItem,(*iter2).c_str());
		iter2++;
		nItem++;
	}

	m_ctrlComboParser.SetCurSel(m_ctrlComboParser.FindString(0,_T("parser.plain")));

	firtex::analyzer::analyzer_list al = firtex::analyzer::CAnalyzerFactory::getRegisteredAnalyzers();
	nItem = 0;
	firtex::analyzer::analyzer_list::iterator iter3 = al.begin();
	while (iter3 != al.end())
	{
		m_ctrlComboAnalyzer.InsertString(nItem,(*iter3).c_str());
		iter3++;
		nItem++;
	}

	m_ctrlComboAnalyzer.SetCurSel(m_ctrlComboAnalyzer.FindString(0,_T("analyzer.standard")));
		
	UpdateData(FALSE);

	OnBnClickedRadioNomerge();
	OnBnClickedRadioRatio();
	
	m_ctrlComboLogLevel.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CIndexingDlg::OnBnClickedButtonStartindex()
{	
	UpdateData(TRUE);
	if(m_strIndexSrc == _T(""))
	{
		AfxMessageBox(_T("请输入要索引的文档集路径"));
		return;
	}
	if(m_strIndexDst == _T(""))
	{
		AfxMessageBox(_T("请输入要索引存放位置"));
		return;
	}
	m_IndexThreadParam.m_self = this;	
	UpdateConfigurator(TRUE);
	
	if(m_bAutoOpen)
	{
		CFxistDoc*	pDoc = ((CFxistView*)GetParent())->GetDocument();
		pDoc->CloseIndex();
	}

	CWnd* pLogWnd = pLogWnd = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetLogWindow();
	if(m_bAutoOpenLogWindow)
	{
		((CMainFrame*)AfxGetApp()->GetMainWnd())->ShowLogWindow(TRUE);		
	}

	//m_pConfigurator->updateGlobalConfig();
		
	AfxBeginThread((AFX_THREADPROC)StartIndexing,(LPVOID)&m_IndexThreadParam);
	if(!m_pProgressDlg)
	{
		m_pProgressDlg = new CIndexingProgressDlg(this,pLogWnd);
	}
	m_pProgressDlg->DoModal();	
}
LONG CIndexingDlg::StartIndexing(LPVOID param)
{
	CIndexThreadParam* pIndexParam = (CIndexThreadParam*)param;
	try
	{
		firtex::collection::CCollection* pCol = CIndexWriter::prepareBuildIndex(pIndexParam->m_self->m_pConfigurator);
		if(pCol == NULL)
		{
			AfxMessageBox(_T("建立索引失败。"));		
		}
		else 
		{
			pIndexParam->m_pCol = pCol;
			pCol->scan();
			pIndexParam->m_pCol = NULL;
			delete pCol;
		}
	}
	catch (CFirteXException& fe)
	{
		CString str;
		str.Format(_T("建立索引失败:%s"),fe.what());
		AfxMessageBox(str);
	}
	

	pIndexParam->m_self->SendMessage(WM_INDEXINGCOMPLETE);	
	return 0L;
}


LRESULT	CIndexingDlg::OnIndexingComplete(WPARAM wParam,LPARAM lParam)
{
	if(m_pProgressDlg && m_pProgressDlg->m_hWnd)
		m_pProgressDlg->SendMessage(WM_CLOSE);
	if(m_bAutoOpen)
	{
		CFxistDoc*	pDoc = ((CFxistView*)GetParent())->GetDocument();
		pDoc->OpenIndex(m_strIndexDst,CIndex::ACCESS_SEARCH);
	}
	AfxMessageBox(_T("索引建立完毕!"));
	return 0L;
}

void CIndexingDlg::OnCbnSelchangeComboLoglevel()
{
	int nCurSel = m_ctrlComboLogLevel.GetCurSel();	
	switch(nCurSel)
	{
	case 0:
		GlobalConfig.setLogLevel(_T("default_level"));
		break;
	case 1:
		GlobalConfig.setLogLevel(_T("err"));
		break;
	case 2:
		GlobalConfig.setLogLevel(_T("warn"));
		break;
	case 3:
		GlobalConfig.setLogLevel(_T("info"));
		break;
	case 4:
		GlobalConfig.setLogLevel(_T("dbg"));
		break;
	case 5:
		GlobalConfig.setLogLevel(_T("disable_all"));
		break;	
	}	
}

void CIndexingDlg::OnEnKillfocusEditMaxmem()
{
	UpdateData(TRUE);
	m_pConfigurator->index.memory = m_uMaxIndexMem*1000000;
}

void CIndexingDlg::OnEnKillfocusEditMaxterms()
{
	UpdateData(TRUE);
	m_pConfigurator->index.maxterms= m_uMaxIndexTerms;
}


void CIndexingDlg::OnBnClickedButtonOpenarg()
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"列表文件(*.xml)|*.xml|所有文件(*.*)|*.*||");
	if(dlg.DoModal() == IDOK)
	{
		try
		{
			if(m_pConfigurator)
				delete m_pConfigurator;
			m_pConfigurator = new Configurator();
			m_pConfigurator->load((LPCTSTR)dlg.GetPathName());
			UpdateConfigurator(FALSE);
		}
		catch(CFirteXException& fe)
		{
			CString str;
			str.Format(_T("打开参数文件失败:%s"),fe.what());
			AfxMessageBox(str);
		}				
	}
}

void CIndexingDlg::OnBnClickedButtonSavearg()
{
	try
	{
		UpdateConfigurator(TRUE);
		if(!_tcscmp(m_pConfigurator->getPath(),_T("")))
		{
			OnBnClickedButtonSaveasarg();
		}
		else 
		{
			m_pConfigurator->save();
			AfxMessageBox(_T("参数保存成功!"));
		}
	}
	catch(CFirteXException& fe)
	{
		CString str;
		str.Format(_T("保存参数失败:%s"),fe.what());
		AfxMessageBox(str);
	}
}

void CIndexingDlg::OnBnClickedButtonSaveasarg()
{	
	CFileDialog dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"列表文件(*.xml)|*.xml|所有文件(*.*)|*.*||");
	if(dlg.DoModal() == IDOK)
	{
		try
		{
			UpdateConfigurator(TRUE);			
			m_pConfigurator->save((LPCTSTR)dlg.GetPathName());			
			AfxMessageBox(_T("参数保存成功!"));
		}
		catch(CFirteXException& fe)
		{
			CString str;
			str.Format(_T("保存参数失败:%s"),fe.what());
			AfxMessageBox(str);
		}
	}
}

void CIndexingDlg::UpdateConfigurator(BOOL bSave)
{
	if(bSave)
	{
		UpdateData(TRUE);
		m_pConfigurator->collection.identifier = (LPCTSTR)m_strCollection;
		
		m_pConfigurator->analyzer.identifier = (LPCTSTR)m_strAnalyzer;
		
		m_pConfigurator->parser.identifier = (LPCTSTR)m_strParser;		

		if(m_nAccessMode == 0)
			m_pConfigurator->index.accessmode = _T("w");
		else if(m_nAccessMode == 1)
			m_pConfigurator->index.accessmode = _T("a");
		if(m_bSearchable)
			m_pConfigurator->index.accessmode += _T("+s");

		m_pConfigurator->index.source = (LPCTSTR)m_strIndexSrc;
		m_pConfigurator->index.indexlocation = (LPCTSTR)m_strIndexDst;	
		m_pConfigurator->index.memory = m_uMaxIndexMem*1000000;
		m_pConfigurator->index.maxterms= m_uMaxIndexTerms;
		m_pConfigurator->index.cachedocs  = m_uCacheDocs;

		switch(m_nMergeMode)
		{
		case 0:
			{
				m_pConfigurator->mergestrategy.strategy = _T("NO");				
			}
			break;
		case 1:
			{
				if(!_tcsicmp(m_pConfigurator->mergestrategy.strategy.c_str(),_T("NO")) || !_tcsicmp(m_pConfigurator->mergestrategy.strategy.c_str(),_T("OPTIMIZE")))
				{
					m_pConfigurator->mergestrategy.strategy = _T("HLM");				
					m_pConfigurator->mergestrategy.param = _T("m=3;c=3;s=1");
				}				
			}
			break;
		case 2:
			{
				m_pConfigurator->mergestrategy.strategy = _T("OPTIMIZE");
			}
		}
		if(m_nGCThreshold == 0)
		{
			tstring s = _T("r=");
			m_pConfigurator->mergestrategy.gc_threshold = firtex::string_utils::append(s,m_fGCRatio);
		}
		else if(m_nGCThreshold == 1)
		{
			tstring s = _T("n=");
			m_pConfigurator->mergestrategy.gc_threshold = firtex::string_utils::append(s,m_uGCDelDocs);
		}
		else 
		{
			m_pConfigurator->mergestrategy.gc_threshold = _T("n=1");
		}

		m_pConfigurator->index.level = m_ctrlComboIndexingLevel.GetCurSel();

		int nCurSel = m_ctrlComboLogLevel.GetCurSel();	
		switch(nCurSel)
		{
		case 0:
			GlobalConfig.setLogLevel(_T("default_level"));
			break;
		case 1:
			GlobalConfig.setLogLevel(_T("err"));
			break;
		case 2:
			GlobalConfig.setLogLevel(_T("warn"));
			break;
		case 3:
			GlobalConfig.setLogLevel(_T("info"));
			break;
		case 4:
			GlobalConfig.setLogLevel(_T("dbg"));
			break;
		case 5:
			GlobalConfig.setLogLevel(_T("disable_all"));
			break;	
		}	
		//m_pConfigurator->replaceStdArg();
	}
	else 
	{
		m_strCollection = m_pConfigurator->collection.identifier.c_str();

		m_strAnalyzer = m_pConfigurator->analyzer.identifier.c_str();
		
		m_strParser = m_pConfigurator->parser.identifier.c_str();

		
		if(!_tcsicmp(m_pConfigurator->index.accessmode.c_str(),_T("w")))
			m_nAccessMode = 0;
		else if(!_tcsicmp(m_pConfigurator->index.accessmode.c_str(),_T("a")))
			m_nAccessMode = 1;
		else if(!_tcsicmp(m_pConfigurator->index.accessmode.c_str(),_T("w+s")))
		{
			m_nAccessMode = 0;
			m_bSearchable = TRUE;
		}
		else if(!_tcsicmp(m_pConfigurator->index.accessmode.c_str(),_T("a+s")))
		{
			m_nAccessMode = 1;
			m_bSearchable = TRUE;
		}
		
		if(!_tcsicmp(m_pConfigurator->mergestrategy.strategy.c_str(),_T("HLM")))
		{
			m_nMergeMode = 1;			
			OnBnClickedRadioHlmmerge();	
		}
		else if(!_tcsicmp(m_pConfigurator->mergestrategy.strategy.c_str(),_T("OPTIMIZE")))
		{
			m_nMergeMode = 2;
			m_bMergeDelDocs = FALSE;
			OnBnClickedRadioOptimize();
		}			
		else 
		{
			m_nMergeMode = 0;
			OnBnClickedRadioNomerge();
		}

		
		m_ctrlComboIndexingLevel.SetCurSel(m_pConfigurator->index.level);

		m_uCacheDocs = m_pConfigurator->index.cachedocs;

		m_strIndexSrc = m_pConfigurator->index.source.c_str();
		m_strIndexDst = m_pConfigurator->index.indexlocation.c_str();	
		m_uMaxIndexMem = (UINT)(m_pConfigurator->index.memory/1000000);
		m_uMaxIndexTerms = m_pConfigurator->index.maxterms;


		CParamParser pp;
		if(pp.parse(m_pConfigurator->mergestrategy.gc_threshold.c_str()))
		{
			float fv;
			int32_t n;
			if(pp.getParam(_T("r"),fv))
			{
				m_nGCThreshold = 0;
				m_fGCRatio = fv;				
				OnBnClickedRadioRatio();
			}
			else if(pp.getParam(_T("n"),n))
			{
				if(n <= 1)
				{
					m_nGCThreshold = 2;
					OnBnClickedRadioForcegc();
				}
				else 
				{
					m_nGCThreshold = 1;
					m_uGCDelDocs = n;
					OnBnClickedRadioDelnum();
				}				
			}
			else 
			{
				m_nGCThreshold = 0;
				m_fGCRatio = 0.1f;
				OnBnClickedRadioRatio();
			}
		}
		else 
		{
			m_nGCThreshold = 0;
			m_fGCRatio = 0.1f;
			OnBnClickedRadioRatio();
		}		

		if(!_tcsicmp(GlobalConfig.getLogLevel().c_str(),_T("default_level")))
			m_ctrlComboLogLevel.SetCurSel(0);
		else if(!_tcsicmp(GlobalConfig.getLogLevel().c_str(),_T("err")))
			m_ctrlComboLogLevel.SetCurSel(1);
		else if(!_tcsicmp(GlobalConfig.getLogLevel().c_str(),_T("warn")))
			m_ctrlComboLogLevel.SetCurSel(2);
		else if(!_tcsicmp(GlobalConfig.getLogLevel().c_str(),_T("info")))
			m_ctrlComboLogLevel.SetCurSel(3);
		else if(!_tcsicmp(GlobalConfig.getLogLevel().c_str(),_T("dbg")))
			m_ctrlComboLogLevel.SetCurSel(4);
		else if(!_tcsicmp(GlobalConfig.getLogLevel().c_str(),_T("disable_all")))
			m_ctrlComboLogLevel.SetCurSel(5);
		else
			m_ctrlComboLogLevel.SetCurSel(0);
		UpdateData(FALSE);
	}
}

void CIndexingDlg::OnCbnSelchangeComboParser()
{
	/*if( (m_ctrlComboCollection.GetCurSel() != 0) && m_ctrlComboParser.GetCurSel() == 4)
	{
		m_ctrlComboParser.SetCurSel(0);
		AfxMessageBox("只能对目录形式的文档集设置自动识别文档类型选项.");
	}*/
}

void CIndexingDlg::OnBnClickedRadioNomerge()
{
	GetDlgItem(IDC_BUTTON_HMADVANCE)->EnableWindow(FALSE);	
}

void CIndexingDlg::OnBnClickedRadioHlmmerge()
{
	GetDlgItem(IDC_BUTTON_HMADVANCE)->EnableWindow(TRUE);	
}

void CIndexingDlg::OnBnClickedRadioOptimize()
{
	GetDlgItem(IDC_BUTTON_HMADVANCE)->EnableWindow(FALSE);	
}

void CIndexingDlg::OnBnClickedRadioRatio()
{
	GetDlgItem(IDC_EDIT_RATIO)->EnableWindow(TRUE);
	//GetDlgItem(IDC_SPIN_RATIO)->EnableWindow(TRUE);

	GetDlgItem(IDC_EDIT_DELNUM)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_DELNUM)->EnableWindow(FALSE);
}

void CIndexingDlg::OnBnClickedRadioDelnum()
{
	GetDlgItem(IDC_EDIT_RATIO)->EnableWindow(FALSE);
	//GetDlgItem(IDC_SPIN_RATIO)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_DELNUM)->EnableWindow(TRUE);
	GetDlgItem(IDC_SPIN_DELNUM)->EnableWindow(TRUE);
}

void CIndexingDlg::OnBnClickedRadioForcegc()
{
	GetDlgItem(IDC_EDIT_RATIO)->EnableWindow(FALSE);
	//GetDlgItem(IDC_SPIN_RATIO)->EnableWindow(TRUE);

	GetDlgItem(IDC_EDIT_DELNUM)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_DELNUM)->EnableWindow(FALSE);
}


void CIndexingDlg::OnBnClickedButtonColschema()
{
	UpdateConfigurator(TRUE);
	CSchemaDlg dlg(m_pConfigurator,this);
	dlg.DoModal();
}

void CIndexingDlg::OnCbnSelchangeComboCollection()
{
	if(m_pConfigurator)
	{
		m_pConfigurator->setSchema(NULL);///clear schema
	}
}



void CIndexingDlg::OnBnClickedButtonHmadvance()
{
	CHierMergeAdvanceDlg dlg(m_pConfigurator->mergestrategy.strategy.c_str(),m_pConfigurator->mergestrategy.param.c_str());
	if(dlg.DoModal() == IDOK)
	{
		m_pConfigurator->mergestrategy.strategy = (LPCTSTR)dlg.m_strStrategy;
		m_pConfigurator->mergestrategy.param = (LPCTSTR)dlg.m_strParam;		
	}
}

void CIndexingDlg::OnBnClickedButtonAnalyzeOption()
{
	CAnalyzerOptionDlg dlg(m_pConfigurator->analyzer.parameter.c_str());
	if(dlg.DoModal() == IDOK)
	{
		m_pConfigurator->analyzer.parameter = dlg.getOption();
	}
}

void CIndexingDlg::OnBnClickedButtonParserOption()
{
	CParserOptDlg dlg(m_pConfigurator->parser.parameter.c_str());
	if(dlg.DoModal() == IDOK)
	{
		m_pConfigurator->parser.parameter = (LPCTSTR)dlg.getParam();
	}
}
