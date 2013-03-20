#pragma once


// CAnalyzerOptionDlg 对话框

class CAnalyzerOptionDlg : public CDialog
{
	DECLARE_DYNAMIC(CAnalyzerOptionDlg)

public:
	CAnalyzerOptionDlg(LPCTSTR lpszParam,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAnalyzerOptionDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_ANALYZER_OPTION };
public:
	LPCTSTR	getOption(){return m_strParameter;}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
