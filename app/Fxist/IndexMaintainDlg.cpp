// IndexMaintainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Fxist.h"
#include "IndexMaintainDlg.h"
#include ".\indexmaintaindlg.h"
#include "FxistView.h"
#include "FxistDoc.h"
#include "HierMergeAdvanceDlg.h"

#include "store/FSIndexInput.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define WM_MERGECOMPLETE	WM_USER + 0x1200
// CIndexMaintainDlg 对话框

IMPLEMENT_DYNCREATE(CIndexMaintainDlg, CChildDialog)
CIndexMaintainDlg::CIndexMaintainDlg(CWnd* pParent /*=NULL*/)
	: CChildDialog(CIndexMaintainDlg::IDD, pParent)
	, m_nMerge(0)
	, m_strMergeIndex(_T(""))
	, m_nGC(0)
	, m_strMergeStrategy(_T("HLM"))
	, m_strMergeParam(_T("m=3;c=3;s=1"))
	, m_nDelDocs(0)
	, m_fRatio(0)
	,m_pIndexToMerge(NULL)
{
}

CIndexMaintainDlg::~CIndexMaintainDlg()
{
	if(m_pIndexToMerge)
	{
		delete m_pIndexToMerge;
		m_pIndexToMerge = NULL;
	}
}

void CIndexMaintainDlg::DoDataExchange(CDataExchange* pDX)
{
	CChildDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MERGEINDEX, m_ctrlListMergeIndex);
	DDX_Control(pDX, IDC_LIST_BARRELS, m_ctrlListMergeBarrels);
	DDX_Radio(pDX, IDC_RADIO_MERGE, m_nMerge);	
	DDX_Text(pDX, IDC_EDIT_MERGEINDEX, m_strMergeIndex);
	DDX_Control(pDX, IDC_PROGRESS_MERGE, m_ctrlMergeProg);
	DDX_Radio(pDX, IDC_RADIO_MRATIO, m_nGC);
	DDX_Text(pDX, IDC_EDIT_MDELNUM, m_nDelDocs);
	DDV_MinMaxUInt(pDX, m_nDelDocs, 0, 100000000);
	DDX_Text(pDX, IDC_EDIT_MRATIO, m_fRatio);
}


BEGIN_MESSAGE_MAP(CIndexMaintainDlg, CChildDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADDINDEX, OnBnClickedButtonAddindex)
	ON_BN_CLICKED(IDC_BUTTON_DELINDEX, OnBnClickedButtonDelindex)
	ON_BN_CLICKED(IDC_BUTTON_MERGEINDEX, OnBnClickedButtonMergeindex)
	ON_BN_CLICKED(IDC_RADIO_MERGE, OnBnClickedRadioMerge)
	ON_BN_CLICKED(IDC_RADIO_OPTMERGE, OnBnClickedRadioOptmerge)
	ON_BN_CLICKED(IDC_RADIO_PARTMERGE, OnBnClickedRadioPartmerge)
	ON_BN_CLICKED(IDC_BUTTON_MERGEBARRELS, OnBnClickedButtonMergebarrels)
	ON_MESSAGE(WM_MERGECOMPLETE,OnMergeComplete)
	ON_BN_CLICKED(IDC_BUTTON_MERGEBROWSE, OnBnClickedButtonMergebrowse)
	ON_BN_CLICKED(IDC_RADIO_MRATIO, OnBnClickedRadioMratio)
	ON_BN_CLICKED(IDC_RADIO_MDELNUM, OnBnClickedRadioMdelnum)
	ON_BN_CLICKED(IDC_RADIO_MFORCEGC, OnBnClickedRadioMforcegc)
	ON_BN_CLICKED(IDC_BUTTON_HMADVANCE, OnBnClickedButtonHmadvance)
END_MESSAGE_MAP()


// CIndexMaintainDlg 消息处理程序



BOOL CIndexMaintainDlg::OnInitDialog()
{
	CChildDialog::OnInitDialog();
	m_ctrlListMergeIndex.InsertColumn(0,"索引路径",LVCFMT_LEFT,290);
	m_ctrlListMergeIndex.InsertColumn(1,"索引桶数",LVCFMT_LEFT,60);
	m_ctrlListMergeIndex.InsertColumn(2,"索引文档数",LVCFMT_LEFT,80);

	m_ctrlListMergeBarrels.InsertColumn(0,"索引桶",LVCFMT_LEFT,50);
	m_ctrlListMergeBarrels.InsertColumn(1,"文档起始编号",LVCFMT_LEFT,90);
	m_ctrlListMergeBarrels.InsertColumn(2,"文档总数",LVCFMT_LEFT,80);	

	AddAnchor(IDC_GROUP51,TOP_LEFT,TOP_RIGHT);
	AddAnchor(IDC_LIST_MERGEINDEX,TOP_LEFT,TOP_RIGHT);
	AddAnchor(IDC_BUTTON_ADDINDEX,TOP_RIGHT,TOP_RIGHT);
	AddAnchor(IDC_BUTTON_DELINDEX,TOP_RIGHT,TOP_RIGHT);
	AddAnchor(IDC_BUTTON_MERGEINDEX,TOP_RIGHT,TOP_RIGHT);	
	AddAnchor(IDC_GROUP52,TOP_LEFT,BOTTOM_RIGHT);
	AddAnchor(IDC_GROUP53,TOP_LEFT,TOP_RIGHT);
	AddAnchor(IDC_LIST_BARRELS,TOP_LEFT,BOTTOM_RIGHT);
	AddAnchor(IDC_BUTTON_MERGEBARRELS,BOTTOM_RIGHT,BOTTOM_RIGHT);
	AddAnchor(IDC_PROGRESS_MERGE,BOTTOM_LEFT,BOTTOM_RIGHT);	
	AddAnchor(IDC_GROUP54,TOP_RIGHT,BOTTOM_RIGHT);
	AddAnchor(IDC_RADIO_MRATIO,TOP_RIGHT,TOP_RIGHT);
	AddAnchor(IDC_EDIT_MRATIO,TOP_RIGHT,TOP_RIGHT);
	AddAnchor(IDC_RADIO_MDELNUM,TOP_RIGHT,TOP_RIGHT);
	AddAnchor(IDC_EDIT_MDELNUM,TOP_RIGHT,TOP_RIGHT);
	AddAnchor(IDC_RADIO_MFORCEGC,TOP_RIGHT,TOP_RIGHT);
	AddAnchor(IDC_SPIN_MDELNUM,TOP_RIGHT,TOP_RIGHT);
	AddAnchor(IDC_STATIC51,TOP_RIGHT,TOP_RIGHT);
	AddAnchor(IDC_STATIC52,TOP_RIGHT,TOP_RIGHT);

	m_ctrlMergeProg.SetWindowText(_T(""));
	m_ctrlMergeProg.SetMarquee(FALSE, 40);

	m_ctrlListMergeBarrels.SetExtendedStyle(m_ctrlListMergeBarrels.GetExtendedStyle()|LVS_EX_CHECKBOXES);

	OnBnClickedRadioMerge();	
	OnBnClickedRadioMratio();
	EnableControls(FALSE);
	GetDlgItem(IDC_BUTTON_MERGEINDEX)->EnableWindow(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CIndexMaintainDlg::OnOpenIndex()
{
	CFxistDoc*	pDoc = ((CFxistView*)GetParent())->GetDocument();
	
	index::CIndexReader* pReader = pDoc->GetIndexReader();

	m_strMergeIndex = pDoc->GetIndexName();
	UpdateData(FALSE);

	//索引桶总数
	CBarrelsInfo* pBarrelsInfo = pReader->getBarrelsInfo();	
	
	pBarrelsInfo->startIterator();
	CBarrelInfo* pBaInfo;
	int order;
	m_ctrlListMergeBarrels.DeleteAllItems();
	CString str;
	while (pBarrelsInfo->hasNext())
	{
		pBaInfo = pBarrelsInfo->next();

		order = m_ctrlListMergeBarrels.GetItemCount();
		m_ctrlListMergeBarrels.InsertItem(order,pBaInfo->getName().c_str());
		str.Format("%u",pBaInfo->getMinDocID());
		m_ctrlListMergeBarrels.SetItemText(order,1,str);
		str.Format("%u",pBaInfo->getDocCount());
		m_ctrlListMergeBarrels.SetItemText(order,2,str);
	}

	if(pBarrelsInfo->getBarrelCount() == 1)
	{
		GetDlgItem(IDC_RADIO_OPTMERGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PARTMERGE)->EnableWindow(FALSE);
	}
	EnableControls(pBarrelsInfo->getBarrelCount()>1);
}
void CIndexMaintainDlg::OnCloseIndex()
{
	m_ctrlListMergeBarrels.DeleteAllItems();
	EnableControls(FALSE);	
}
void CIndexMaintainDlg::OnBnClickedButtonAddindex()
{
	CFxistDoc*	pDoc = ((CFxistView*)GetParent())->GetDocument();
	CString strIndex = pDoc->BrowseForFolder();
	CString str;
	if(!strIndex.IsEmpty())
	{
		try
		{
			int order = m_ctrlListMergeIndex.GetItemCount();
			m_ctrlListMergeIndex.InsertItem(order,strIndex);
			index::CIndex r((LPCTSTR)strIndex,CIndex::ACCESS_READONLY);
			str.Format("%d",r.getBarrelsInfo()->getBarrelCount());
			m_ctrlListMergeIndex.SetItemText(order,1,str);
			index::CIndexReader* pReader = r.acquireReader();
			str.Format("%d",pReader->numDocs());
			m_ctrlListMergeIndex.SetItemText(order,2,str);	
		}
		catch (CFirteXException& fe)
		{
			AfxMessageBox(fe.what());
		}
	}
	if(m_ctrlListMergeIndex.GetItemCount() >= 2)
		GetDlgItem(IDC_BUTTON_MERGEINDEX)->EnableWindow(TRUE);
}
void CIndexMaintainDlg::OnBnClickedButtonDelindex()
{
	if(m_ctrlListMergeIndex.GetSelectedCount() <= 0)
	{
		AfxMessageBox(_T("请选择要移除的索引"));
		return ;
	}
	POSITION pos = m_ctrlListMergeIndex.GetFirstSelectedItemPosition();
	while (pos)
	{
		int order  = m_ctrlListMergeIndex.GetNextSelectedItem(pos);
		m_ctrlListMergeIndex.DeleteItem(order);
	}	
	if(m_ctrlListMergeIndex.GetItemCount() < 2)
		GetDlgItem(IDC_BUTTON_MERGEINDEX)->EnableWindow(FALSE);
}

void CIndexMaintainDlg::OnBnClickedButtonMergeindex()
{

}

void CIndexMaintainDlg::OnBnClickedRadioMerge()
{
	GetDlgItem(IDC_LIST_BARRELS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_HMADVANCE)->EnableWindow(TRUE);
}

void CIndexMaintainDlg::OnBnClickedRadioOptmerge()
{
	GetDlgItem(IDC_LIST_BARRELS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_HMADVANCE)->EnableWindow(FALSE);
}

void CIndexMaintainDlg::OnBnClickedRadioPartmerge()
{
	GetDlgItem(IDC_LIST_BARRELS)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_HMADVANCE)->EnableWindow(FALSE);
}

void CIndexMaintainDlg::OnBnClickedButtonMergebarrels()
{
	UpdateData(TRUE);
	if(m_strMergeIndex == _T(""))
	{
		AfxMessageBox("请打开索引后再进行合并。");
		return ;
	}	
	CFxistDoc*	pDoc = ((CFxistView*)GetParent())->GetDocument();	
	if(m_strMergeIndex == pDoc->GetIndexName())
		pDoc->CloseIndex();
	if(m_pIndexToMerge)
	{
		delete m_pIndexToMerge;
		m_pIndexToMerge = NULL;
	}	
	Configurator config;
	switch(m_nMerge)
	{
	case 0:
		{
			config.mergestrategy.strategy = (LPCTSTR)m_strMergeStrategy;
			config.mergestrategy.param = (LPCTSTR)m_strMergeParam;			
		}
		break;
	case 1:
		{					
			config.mergestrategy.strategy = _T("OPTIMIZE");
			config.mergestrategy.param = _T("");
		}
		break;
	case 2:
		{
			AfxMessageBox(_T("还未实现此功能:)"));
			return;
			//TODO:
		}
		break;
	}
	tstring str;
	if(m_nGC == 0)
	{			
		str = _T("r=");
		config.mergestrategy.gc_threshold = firtex::string_utils::append(str,m_fRatio);

	}
	else if(m_nGC == 1)
	{
		str = _T("n=");
		config.mergestrategy.gc_threshold = firtex::string_utils::append(str,m_nDelDocs);
	}
	else
	{
		config.mergestrategy.gc_threshold = _T("n=0");
	}
	//config.updateGlobalConfig();
	m_pIndexToMerge = new CIndex(m_strMergeIndex,CIndex::ACCESS_MODIFY);
	CIndexWriter* pWriter = m_pIndexToMerge->acquireWriter();
	pWriter->acquireMerger();

	m_ctrlMergeProg.SetWindowText(_T("正在合并..."));
	m_ctrlMergeProg.SetMarquee(TRUE, 40);
	
	GetDlgItem(IDC_EDIT_MERGEINDEX)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_MERGEBROWSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_MERGEBARRELS)->EnableWindow(FALSE);
	EnableControls(FALSE);
	AfxBeginThread((AFX_THREADPROC)MergeThreadProc,(LPVOID)this);
	
}
LONG CIndexMaintainDlg::MergeThreadProc(LPVOID param)
{
	CIndexMaintainDlg* pThis = (CIndexMaintainDlg*)param;	
	pThis->m_pIndexToMerge->acquireWriter()->mergeIndex();	
	pThis->SendMessage(WM_MERGECOMPLETE);
	return 0L;
}
LRESULT CIndexMaintainDlg::OnMergeComplete(WPARAM wParam,LPARAM lParam)
{
	m_ctrlMergeProg.SetWindowText(_T(""));
	m_ctrlMergeProg.SetMarquee(FALSE, 40);	
	GetDlgItem(IDC_EDIT_MERGEINDEX)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_MERGEBROWSE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_MERGEBARRELS)->EnableWindow(TRUE);
	EnableControls(TRUE);
	try
	{
		delete m_pIndexToMerge;
		m_pIndexToMerge = NULL;
		CFxistDoc*	pDoc = ((CFxistView*)GetParent())->GetDocument();
		if(m_strMergeIndex == pDoc->GetIndexName())
			pDoc->OpenIndex(m_strMergeIndex,CIndex::ACCESS_SEARCH);
	}
	catch(CFirteXException& fe)
	{
		CString str = _T("打开合并后的索引失败：可能原始索引已损坏.错误信息:");
		str += fe.what();
		AfxMessageBox(str);
	}	
	AfxMessageBox(_T("索引合并完成!"));

	return 0L;
}
void CIndexMaintainDlg::EnableControls(BOOL bEnable)
{
	UpdateData(TRUE);
	GetDlgItem(IDC_BUTTON_MERGEBARRELS)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_MERGE)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_OPTMERGE)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_PARTMERGE)->EnableWindow(bEnable);

	
	GetDlgItem(IDC_RADIO_MRATIO)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_MRATIO)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_MDELNUM)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_MDELNUM)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_MFORCEGC)->EnableWindow(bEnable);
	GetDlgItem(IDC_SPIN_MDELNUM)->EnableWindow(bEnable);	

	GetDlgItem(IDC_BUTTON_HMADVANCE)->EnableWindow(bEnable);
}

void CIndexMaintainDlg::OnBnClickedButtonMergebrowse()
{
	CFxistDoc*	pDoc = ((CFxistView*)GetParent())->GetDocument();
	CString strIndex = pDoc->BrowseForFolder();
	if(strIndex != _T(""))
	{
		m_strMergeIndex = strIndex;
		UpdateData(FALSE);

		try
		{
			CIndex ind(m_strMergeIndex,CIndex::ACCESS_READONLY);
			CIndexReader* pReader = ind.acquireReader();			

			//索引桶总数
			CBarrelsInfo* pBarrelsInfo = pReader->getBarrelsInfo();	

			pBarrelsInfo->startIterator();
			CBarrelInfo* pBaInfo;
			int order;
			m_ctrlListMergeBarrels.DeleteAllItems();
			CString str;
			while (pBarrelsInfo->hasNext())
			{
				pBaInfo = pBarrelsInfo->next();

				order = m_ctrlListMergeBarrels.GetItemCount();
				m_ctrlListMergeBarrels.InsertItem(order,pBaInfo->getName().c_str());
				str.Format("%u",pBaInfo->getMinDocID());
				m_ctrlListMergeBarrels.SetItemText(order,1,str);
				str.Format("%u",pBaInfo->getDocCount());
				m_ctrlListMergeBarrels.SetItemText(order,2,str);
			}

			if(pBarrelsInfo->getBarrelCount() == 1)
			{
				GetDlgItem(IDC_RADIO_OPTMERGE)->EnableWindow(FALSE);
				GetDlgItem(IDC_RADIO_PARTMERGE)->EnableWindow(FALSE);
			}
			EnableControls(pBarrelsInfo->getBarrelCount()>1);
		}
		catch(CFirteXException& fe)
		{
			CString str;
			str.Format(_T("打开索引失败:%s"),fe.what());
			AfxMessageBox(str);
		}		
	}
}

void CIndexMaintainDlg::OnBnClickedRadioMratio()
{
	GetDlgItem(IDC_EDIT_MRATIO)->EnableWindow(TRUE);	
	GetDlgItem(IDC_EDIT_MDELNUM)->EnableWindow(FALSE);	
	GetDlgItem(IDC_SPIN_MDELNUM)->EnableWindow(FALSE);	
}

void CIndexMaintainDlg::OnBnClickedRadioMdelnum()
{	
	GetDlgItem(IDC_EDIT_MRATIO)->EnableWindow(FALSE);	
	GetDlgItem(IDC_EDIT_MDELNUM)->EnableWindow(TRUE);	
	GetDlgItem(IDC_SPIN_MDELNUM)->EnableWindow(TRUE);	
}

void CIndexMaintainDlg::OnBnClickedRadioMforcegc()
{	
	GetDlgItem(IDC_EDIT_MRATIO)->EnableWindow(FALSE);	
	GetDlgItem(IDC_EDIT_MDELNUM)->EnableWindow(FALSE);	
	GetDlgItem(IDC_SPIN_MDELNUM)->EnableWindow(FALSE);	
}

void CIndexMaintainDlg::OnBnClickedButtonHmadvance()
{
	
	CHierMergeAdvanceDlg dlg(m_strMergeStrategy,m_strMergeParam);
	if(dlg.DoModal() == IDOK)
	{		
		m_strMergeStrategy = dlg.m_strStrategy;
		m_strMergeParam = dlg.m_strParam;
	}
}
