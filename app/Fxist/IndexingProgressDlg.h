#pragma once
#include "TextProgressCtrl.h"


// CIndexingProgressDlg 对话框
class CIndexingDlg;
class CIndexingProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CIndexingProgressDlg)

public:
	CIndexingProgressDlg(CIndexingDlg* pParent,CWnd* pLogWnd);   // 标准构造函数
	virtual ~CIndexingProgressDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_INDEXINGPROG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
