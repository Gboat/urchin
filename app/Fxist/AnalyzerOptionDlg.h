#pragma once


// CAnalyzerOptionDlg �Ի���

class CAnalyzerOptionDlg : public CDialog
{
	DECLARE_DYNAMIC(CAnalyzerOptionDlg)

public:
	CAnalyzerOptionDlg(LPCTSTR lpszParam,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAnalyzerOptionDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_ANALYZER_OPTION };
public:
	LPCTSTR	getOption(){return m_strParameter;}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStopfilebrowse();
	afx_msg void OnBnClickedOk();
protected:
	CString m_strParameter;
public:
	CString m_strStopFile;
	CString m_strStopFilter;
	CString m_strStemmer;
	CString m_strLanguage;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboStopfilter();
	afx_msg void OnCbnSelchangeComboStemming();
};
