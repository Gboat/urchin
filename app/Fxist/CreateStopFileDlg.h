#pragma once
#include "afxwin.h"


// CCreateStopFileDlg 对话框

class CCreateStopFileDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateStopFileDlg)

public:
	CCreateStopFileDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCreateStopFileDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_STOPFILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CComboBox m_ctrlSFAnalyzer;
	CString m_strSFSrc;
	CString m_strSFDst;
	afx_msg void OnBnClickedButtonSfsrcbrowse();
	afx_msg void OnBnClickedButtonSfdstbrowse();
};
