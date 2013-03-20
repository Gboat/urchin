#pragma once

#include <map>
#include "ChildDialog.h"
#include "afxcmn.h"
#include "afxwin.h"
using namespace std;

// CDocumentsDlg 对话框
struct QueryResultDoc 
{
	QueryResultDoc()
	{
		docid = 0;
		tf = 0;
		doc = NULL;
	}
	~QueryResultDoc()
	{
		if(doc)
		{
			delete doc;
			doc = NULL;
		}
	}
	docid_t					docid;	//编号
	freq_t					tf;		//tf
	document::CDocument*	doc;	//文档对象
};

class CDocumentsDlg : public CChildDialog
{
	DECLARE_DYNCREATE(CDocumentsDlg)

public:
	CDocumentsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDocumentsDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_DOCUMENTS };
protected:
	index::CIndex*		m_pIndex;
	CTermDocFreqs*			m_pCurTermDocs;

	map<int,QueryResultDoc*>	m_queryResultCache;
	int						m_nCurDoc;
protected:
	virtual void	OnOpenIndex();
	virtual	void	OnCloseIndex();
	virtual void	OnActivate();

	void	DisplayDoc(document::CDocument* pDoc);
	void	DisplayResultDoc(QueryResultDoc* pResDoc);
	void	Query(LPCTSTR lpszField,LPCTSTR lpszTerm);
	void	ClearCachedQueryResult();

	void	EnableControls(BOOL bEnable);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持	

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_ctrlListDocResult;
	int m_nDocID;
	afx_msg void OnBnClickedButtonDocnoquery();
	afx_msg void OnBnClickedButtonDfirstdoc();
	afx_msg void OnBnClickedButtonDprevdoc();
	afx_msg void OnBnClickedButtonDnextdoc();
	afx_msg void OnBnClickedButtonDlastdoc();
	afx_msg void OnBnClickedButtonDeldoc();
	int m_nCurDocID;
	float m_fBoost;
	CString m_strQueryTerm;
	afx_msg void OnBnClickedButtonWordquery();
	UINT m_nDF;
	UINT m_nTF;
	afx_msg void OnBnClickedButtonWfirstdoc();
	afx_msg void OnBnClickedButtonWprevdoc();
	afx_msg void OnBnClickedButtonWnextdoc();
	afx_msg void OnBnClickedButtonWlastdoc();
	afx_msg void OnBnClickedButtonWdelalldocs();
	int m_nAnalyzer;
	CString m_strQueryField;
	CComboBox m_ctrlIndexFields;
	afx_msg void OnBnClickedButtonUndelall();
	CComboBox m_ctrlAnalyzer;
};
