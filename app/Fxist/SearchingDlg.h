#pragma once

#include "ChildDialog.h"
#include "afxcmn.h"
#include "afxwin.h"
// CSearchingDlg 对话框

class CSearchingDlg : public CChildDialog
{
	DECLARE_DYNCREATE(CSearchingDlg)

public:
	CSearchingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSearchingDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SEARCHING };
protected:
	analyzer::CAnalyzer*	m_pAnalyzer;
	CString					m_strAnalyzerParam;
protected:
	virtual void	OnOpenIndex();
	virtual	void	OnCloseIndex();
	void			EnableControls(BOOL bEnable);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSearch();
	virtual BOOL OnInitDialog();
	CListCtrl m_ctrlListQueryHits;
	CComboBox m_ctrlComboSearchAnalyzer;
	CString m_strQueryExp;
	UINT m_nTopk;
	CComboBox m_ctrlSearchDefField;
	CString m_strSearchDefField;
	UINT m_uSearchMem;
	afx_msg void OnEnKillfocusEditSearchmem();
	afx_msg void OnBnClickedButtonAnaOption();
	CString m_strAnalyzer;
	afx_msg void OnCbnSelchangeComboSearchdeffield();
};
