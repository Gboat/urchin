#pragma once

#include "ChildDialog.h"
#include "afxwin.h"

// CIndexingDlg 对话框

class CIndexingDlg;

class CIndexThreadParam
{
public:
	CIndexThreadParam()
	{
	}

	virtual ~CIndexThreadParam()
	{		
	}

	CIndexingDlg*		m_self;

	firtex::collection::CCollection*	m_pCol;
	CString				m_strLogFilePath;	
};


class CIndexingProgressDlg;
class CIndexingDlg : public CChildDialog
{
	DECLARE_DYNCREATE(CIndexingDlg)

public:
	CIndexingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIndexingDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_INDEXING };
protected:
	static LONG StartIndexing(LPVOID param);

	virtual void	OnOpenIndex(){}
	virtual	void	OnCloseIndex(){}

	void	UpdateConfigurator(BOOL bSave);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

protected:
	CIndexThreadParam		m_IndexThreadParam;
	CIndexingProgressDlg*	m_pProgressDlg;
	Configurator*			m_pConfigurator;

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_ctrlComboCollection;
	CString m_strIndexSrc;
	afx_msg void OnBnClickedButtonBrowsesrc();
	afx_msg void OnBnClickedButtonIndexdst();
	CString m_strIndexDst;
	CComboBox m_ctrlComboParser;
	CComboBox m_ctrlComboAnalyzer;
	UINT m_uMaxIndexMem;
	UINT m_uMaxIndexTerms;
	// 最多允许的索引桶
	UINT m_uMaxIndexBarrels;	
	CComboBox m_ctrlComboLogLevel;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonStartindex();

	afx_msg LRESULT	OnIndexingComplete(WPARAM wParam,LPARAM lParam);
	BOOL m_bAutoOpen;	
	afx_msg void OnCbnSelchangeComboLoglevel();
	afx_msg void OnEnKillfocusEditMaxmem();
	afx_msg void OnEnKillfocusEditMaxterms();	

	friend class CIndexingProgressDlg;
	afx_msg void OnBnClickedButtonOpenarg();
	afx_msg void OnBnClickedButtonSavearg();
	afx_msg void OnBnClickedButtonSaveasarg();
	BOOL m_bAppend;
	int m_nMergeMode;
	BOOL m_bMergeDelDocs;
	BOOL m_bAutoOpenLogWindow;	
	afx_msg void OnCbnSelchangeComboParser();
	UINT m_uCacheDocs;
	CComboBox m_ctrlComboIndexingLevel;
	int m_nGCThreshold;
	afx_msg void OnBnClickedRadioNomerge();
	afx_msg void OnBnClickedRadioHlmmerge();
	afx_msg void OnBnClickedRadioOptimize();
	afx_msg void OnBnClickedRadioRatio();
	afx_msg void OnBnClickedRadioDelnum();	
	float m_fGCRatio;
	UINT m_uGCDelDocs;
	int m_nAccessMode;
	CString m_sIndexingLevel;
	afx_msg void OnBnClickedButtonColschema();
	afx_msg void OnCbnSelchangeComboCollection();
	BOOL m_bSearchable;
	afx_msg void OnBnClickedRadioForcegc();
	afx_msg void OnBnClickedButtonHmadvance();
	afx_msg void OnBnClickedButtonAnalyzeOption();
	afx_msg void OnBnClickedButtonParserOption();
	CString m_strCollection;
	CString m_strParser;
	CString m_strAnalyzer;
};
