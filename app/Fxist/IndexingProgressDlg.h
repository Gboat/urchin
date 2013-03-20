#pragma once
#include "TextProgressCtrl.h"


// CIndexingProgressDlg �Ի���
class CIndexingDlg;
class CIndexingProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CIndexingProgressDlg)

public:
	CIndexingProgressDlg(CIndexingDlg* pParent,CWnd* pLogWnd);   // ��׼���캯��
	virtual ~CIndexingProgressDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_INDEXINGPROG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CTextProgressCtrl m_progCtrlIndexing;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

protected:
	CIndexingDlg*	m_pParent;
	CWnd*			m_pLogWnd;
public:
	afx_msg void OnBnClickedButtonShowlog();
};
