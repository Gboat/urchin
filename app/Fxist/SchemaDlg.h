#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CSchemaDlg 对话框

class CSchemaDlg : public CDialog
{
	DECLARE_DYNAMIC(CSchemaDlg)

public:
	CSchemaDlg(Configurator* pArg,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSchemaDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SCHEMA };

protected:
	void	UpdateSchemaData(BOOL bSave);
	void	UpdateSchemaItem(int nItem,document::CSchemaItem* pSchemaItem = NULL);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_sFieldName;
	CString m_sFieldAlias;
	CComboBox m_comboCtrlIndexer;
	CComboBox m_comboCtrlAnalyzer;
	CComboBox m_comboCtrlStore;
	CComboBox m_comboCtrlIndex;
	CComboBox m_comboCtrlTermVector;
	float m_fBoost;
	afx_msg void OnBnClickedButtonScmodify();
	afx_msg void OnBnClickedButtonScadd();
	afx_msg void OnBnClickedButtonScdel();
	afx_msg void OnBnClickedOk();
	CListCtrl m_ctrlListSchema;
	Configurator*	m_pConfigurator;
	int m_nIndexLevel;
protected:
	BOOL		m_bDirty;
	document::CDocumentSchema* m_pDocSchema;
public:
	int m_nIndex;
	int m_nStore;
	int m_nTermVector;
	afx_msg void OnLvnItemchangedListSchema(NMHDR *pNMHDR, LRESULT *pResult);
	CString m_strIndexer;
	CString m_strAnalyzer;
	afx_msg void OnBnClickedCancel();
	CString m_sDocTag;
};
