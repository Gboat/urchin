// Fist.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "Fxist.h"
#include "MainFrm.h"

#include "FxistDoc.h"
#include "FxistView.h"
#include "hyperlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFxistApp

BEGIN_MESSAGE_MAP(CFxistApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CFxistApp ����

CFxistApp::CFxistApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CFxistApp ����

CFxistApp theApp;

// CFxistApp ��ʼ��

BOOL CFxistApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
	InitCommonControls();
	CWinApp::InitInstance();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)
	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CFxistDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(CFxistView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// ������׼������DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	m_pMainWnd->SetWindowText(_T("Fxist-FirteX Index/Search Toolbox"));
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// �������ں�׺ʱ�ŵ��� DragAcceptFiles��
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand  ֮����
	return TRUE;
}



// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
	CHyperLink m_ctrlEmail;
	virtual BOOL OnInitDialog();
	CHyperLink m_ctrlHomepage1;
	CHyperLink m_ctrlHomepage2;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_EMAIL, m_ctrlEmail);
	DDX_Control(pDX, IDC_STATIC_HOMEPAGE1, m_ctrlHomepage1);
	DDX_Control(pDX, IDC_STATIC_HOMEPAGE2, m_ctrlHomepage2);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CFxistApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CFxistApp ��Ϣ�������


BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctrlEmail.SetURL(_T("mailto:ruijieguo@software.ict.ac.cn"));
	m_ctrlHomepage1.SetURL(_T("http://www.firtex.org"));
	m_ctrlHomepage2.SetURL(_T("http://firtex.sourceforge.net"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
