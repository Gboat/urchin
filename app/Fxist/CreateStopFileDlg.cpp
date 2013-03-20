// CreateStopFileDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Fxist.h"
#include "CreateStopFileDlg.h"
#include ".\createstopfiledlg.h"


// CCreateStopFileDlg �Ի���

IMPLEMENT_DYNAMIC(CCreateStopFileDlg, CDialog)
CCreateStopFileDlg::CCreateStopFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateStopFileDlg::IDD, pParent)
	, m_strSFSrc(_T(""))
	, m_strSFDst(_T(""))
{
}

CCreateStopFileDlg::~CCreateStopFileDlg()
{
}

void CCreateStopFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SFANALYZE, m_ctrlSFAnalyzer);
	DDX_Text(pDX, IDC_EDIT_SFSRC, m_strSFSrc);
	DDX_Text(pDX, IDC_EDIT_SFDST, m_strSFDst);
}


BEGIN_MESSAGE_MAP(CCreateStopFileDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SFSRCBROWSE, OnBnClickedButtonSfsrcbrowse)
	ON_BN_CLICKED(IDC_BUTTON_SFDSTBROWSE, OnBnClickedButtonSfdstbrowse)
END_MESSAGE_MAP()

BOOL CCreateStopFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_ctrlSFAnalyzer.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
// CCreateStopFileDlg ��Ϣ�������

void CCreateStopFileDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	if(m_strSFSrc == _T(""))
	{
		AfxMessageBox(_T("��ָ��ͣ�ô�Դ�ļ�"));
		return ;
	}	
	if(m_strSFDst == _T(""))
	{
		AfxMessageBox(_T("��ָ��ͣ�ô�Ŀ���ļ�"));
		return ;
	}	

	vector<wordid_t>ids;
	char line[80];
	CStandardAnalyzer a;
	wordid_t wid;
	FILE* file = fopen((LPCTSTR)m_strSFSrc,"r");
	while (fgets(line,80,file))
	{		
		line[strlen(line)-1]=0;
		if ( strlen(line) == 0 )
			continue;
		
		wid = a.getWordId(line);
		if(wid != -1)
			ids.push_back(wid);		
	}
	fclose(file);
	file = fopen((LPCTSTR)m_strSFDst,"wb");
	stable_sort(ids.begin(),ids.end(),std::less<termid_t>());
	
	vector<wordid_t>::iterator iter = ids.begin();
	while (iter != ids.end())
	{
		wid = (*iter);
		fwrite(&wid,1,sizeof(wordid_t),file);
		iter++;
	}
	fclose(file);

	CString str;
	str.Format(_T("ͣ�ô��ļ�������ϣ��ܴ�����%d"),ids.size());
	AfxMessageBox(str);
	

//	OnOK();
}



void CCreateStopFileDlg::OnBnClickedButtonSfsrcbrowse()
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"ͣ�ô��ļ�(*.txt)|*.txt|�����ļ�(*.*)|*.*||");
	if(dlg.DoModal() == IDOK)
	{
		m_strSFSrc = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CCreateStopFileDlg::OnBnClickedButtonSfdstbrowse()
{
	CFileDialog dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"ͣ�ô��ļ�(*.stf)|*.stf|�����ļ�(*.*)|*.*||");
	if(dlg.DoModal() == IDOK)
	{
		m_strSFDst = dlg.GetPathName();
		UpdateData(FALSE);
	}	
}
