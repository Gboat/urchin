#pragma once

#include "ChildDialog.h"
#include "afxcmn.h"
#include "afxwin.h"
// CSearchingDlg �Ի���

class CSearchingDlg : public CChildDialog
{
	DECLARE_DYNCREATE(CSearchingDlg)

public:
	CSearchingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSearchingDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_SEARCHING };
protected:
	analyzer::CAnalyzer*	m_pAnalyzer;
	CString					m_strAnalyzerParam;
protected:
	virtual void	OnOpenIndex();
	virtual	void	OnCloseIndex();
	void			EnableControls(BOOL bEnable);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
