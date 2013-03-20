// OverviewDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Fxist.h"
#include "OverviewDlg.h"
#include "FxistView.h"
#include "FxistDoc.h"
#include ".\overviewdlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// COverviewDlg �Ի���

IMPLEMENT_DYNCREATE(COverviewDlg, CChildDialog)
COverviewDlg::COverviewDlg(CWnd* pParent /*=NULL*/)
	: CChildDialog(COverviewDlg::IDD, pParent)
{
}

COverviewDlg::~COverviewDlg()
{
}

void COverviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CChildDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FIELD, m_ctrlListField);
	DDX_Control(pDX, IDC_LIST_BARRELS, m_ctrlListBarrels);
}


BEGIN_MESSAGE_MAP(COverviewDlg, CChildDialog)
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()


// COverviewDlg ��Ϣ�������



BOOL COverviewDlg::OnInitDialog()
{
	CChildDialog::OnInitDialog();

	AddAnchor(IDC_GROUP11,TOP_LEFT,TOP_RIGHT);
	AddAnchor(IDC_STATIC12,TOP_LEFT,TOP_RIGHT);
	AddAnchor(IDC_LIST_BARRELS,TOP_LEFT,BOTTOM_LEFT);	
	AddAnchor(IDC_STATIC_CURBARREL,TOP_LEFT,TOP_RIGHT);
	AddAnchor(IDC_LIST_FIELD,TOP_LEFT,BOTTOM_LEFT);
//	AddAnchor(IDC_STATIC13,TOP_LEFT,BOTTOM_RIGHT);
	
	m_ctrlListBarrels.InsertColumn(0,"����Ͱ",LVCFMT_LEFT,50);
	m_ctrlListBarrels.InsertColumn(1,"�ĵ���ʼ���",LVCFMT_LEFT,90);
	m_ctrlListBarrels.InsertColumn(2,"�ĵ�����",LVCFMT_LEFT,80);	

	m_ctrlListField.InsertColumn(0,"����",LVCFMT_LEFT,60);
	m_ctrlListField.InsertColumn(1,"�洢",LVCFMT_LEFT,40);
	m_ctrlListField.InsertColumn(2,"����",LVCFMT_LEFT,40);
	m_ctrlListField.InsertColumn(3,"����",LVCFMT_LEFT,40);
	m_ctrlListField.InsertColumn(4,"ǰ������",LVCFMT_LEFT,50);

	m_ctrlListField.SetExtendedStyle(m_ctrlListField.GetExtendedStyle()|LVS_EX_FULLROWSELECT);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void COverviewDlg::OnOpenIndex()
{	
	CFxistDoc*	pDoc = ((CFxistView*)GetParent())->GetDocument();

	CString str;
	GetDlgItem(IDC_STATIC_INAME)->SetWindowText(pDoc->GetIndexName());//������
	index::CIndexReader* pReader = pDoc->GetIndexReader();
	GetDlgItem(IDC_STATIC_IVERSION)->SetWindowText(pReader->getVersion().c_str());//�����汾

	//����Ͱ����
	CBarrelsInfo* pBarrelsInfo = pReader->getBarrelsInfo();	
	str.Format("%d",pBarrelsInfo->getBarrelCount());
	GetDlgItem(IDC_STATIC_BARRELNUM)->SetWindowText(str);

	//�ĵ�����
	str.Format("%u",pBarrelsInfo->getDocCount());
	GetDlgItem(IDC_STATIC_DOCNUM)->SetWindowText(str);

	//�ֶ�����
	CFieldsInfo& fieldInfo = pReader->getFieldsInfo();
	str.Format("%u",fieldInfo.numFields());
	GetDlgItem(IDC_STATIC_FIELDNUM)->SetWindowText(str);	

	//������
	str.Format("%u",pReader->getNumTerms());
	GetDlgItem(IDC_STATIC_WORDNUM)->SetWindowText(str);

	//ɾ���ĵ���
	str.Format("%u",pBarrelsInfo->numDeletedDocs());
	GetDlgItem(IDC_STATIC_DELDOCNUM)->SetWindowText(str);

	//������
	GetDlgItem(IDC_STATIC_ANALYZER)->SetWindowText("analyzer.standard");


	//����޸�ʱ��
	CDateTime dt;
	pReader->lastModified(dt);
	GetDlgItem(IDC_STATIC_LASTMODIFY)->SetWindowText(dt.format().c_str());	

	pBarrelsInfo->startIterator();
	CBarrelInfo* pBaInfo;
	int order;
	while (pBarrelsInfo->hasNext())
	{
		pBaInfo = pBarrelsInfo->next();

		order = m_ctrlListBarrels.GetItemCount();
		m_ctrlListBarrels.InsertItem(order,pBaInfo->getName().c_str());

		str.Format("%u",pBaInfo->getMinDocID());
		m_ctrlListBarrels.SetItemText(order,1,str);
		str.Format("%u",pBaInfo->getDocCount());
		m_ctrlListBarrels.SetItemText(order,2,str);
	}
	FillFieldInfo();
}
void COverviewDlg::OnCloseIndex()
{
	GetDlgItem(IDC_STATIC_INAME)->SetWindowText(_T("?"));//������	
	GetDlgItem(IDC_STATIC_IVERSION)->SetWindowText(_T("?"));//�����汾

	//����Ͱ����	
	GetDlgItem(IDC_STATIC_BARRELNUM)->SetWindowText(_T("?"));

	//�ĵ�����	
	GetDlgItem(IDC_STATIC_DOCNUM)->SetWindowText(_T("?"));	
	GetDlgItem(IDC_STATIC_WORDNUM)->SetWindowText(_T("?"));

	GetDlgItem(IDC_STATIC_DELDOCNUM)->SetWindowText(_T("?"));

	//����޸�ʱ��	
	GetDlgItem(IDC_STATIC_LASTMODIFY)->SetWindowText(_T("?"));		
	m_ctrlListBarrels.DeleteAllItems();
	m_ctrlListField.DeleteAllItems();
}

void COverviewDlg::FillFieldInfo()
{
	CFxistDoc*	pDoc = ((CFxistView*)GetParent())->GetDocument();
	index::CIndexReader* pReader = pDoc->GetIndexReader();
	CFieldsInfo& fieldInfo = pReader->getFieldsInfo();
	fieldInfo.startIterator();
	CFieldInfo* pFieldInfo;
	m_ctrlListField.DeleteAllItems();
	int order;
	while (fieldInfo.hasNext())
	{
		pFieldInfo = fieldInfo.next();
		order = m_ctrlListField.GetItemCount();
		m_ctrlListField.InsertItem(order,pFieldInfo->getName());
		fieldflag_t f= pFieldInfo->getFlag();
		if(pFieldInfo->isStored())
			m_ctrlListField.SetItemText(order,1,"S");
		else 
		{
			if(pFieldInfo->isCompressed())
				m_ctrlListField.SetItemText(order,1,"C");
			else 
				m_ctrlListField.SetItemText(order,1,"N");
		}						
		if(pFieldInfo->isAnalyzed())
			m_ctrlListField.SetItemText(order,2,"A");
		else 
			m_ctrlListField.SetItemText(order,2,"N");					
		if(pFieldInfo->isIndexed())
			m_ctrlListField.SetItemText(order,3,"I");
		else 
			m_ctrlListField.SetItemText(order,3,"N");

		if(pFieldInfo->isSequenceVector())
			m_ctrlListField.SetItemText(order,4,"S");
		else if(pFieldInfo->isFreqVector())
			m_ctrlListField.SetItemText(order,4,"F");
		else if(pFieldInfo->isPositionVector())
			m_ctrlListField.SetItemText(order,4,"P");
		else 
			m_ctrlListField.SetItemText(order,4,"N");
	}
}

