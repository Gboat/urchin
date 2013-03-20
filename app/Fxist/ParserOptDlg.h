#pragma once
#include "afxwin.h"


// CParserOptDlg �Ի���

class CParserOptDlg : public CDialog
{
	DECLARE_DYNAMIC(CParserOptDlg)

public:
	CParserOptDlg(LPCTSTR lpszParam,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParserOptDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_PARSER_OPTION };
public:
	CString	getParam(){return m_strParam;}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
