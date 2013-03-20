#pragma once
#include "afxwin.h"


// CParserOptDlg 对话框

class CParserOptDlg : public CDialog
{
	DECLARE_DYNAMIC(CParserOptDlg)

public:
	CParserOptDlg(LPCTSTR lpszParam,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParserOptDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_PARSER_OPTION };
public:
	CString	getParam(){return m_strParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_strParam;
	CString m_strField;
	afx_msg void OnBnClickedButtonFiAdd();
	CString m_strMeta;
	CComboBox m_ctrlForwardIndex;
	CComboBox m_ctrlGetMeta;
	afx_msg void OnBnClickedButtonMetaAdd();
	virtual BOOL OnInitDialog();
};
