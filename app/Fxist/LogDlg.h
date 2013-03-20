#pragma once

#include "ResizableDialog.h"
#include "afxwin.h"

// CLogDlg 对话框

class CLogDlg : public CResizableDialog
{
	DECLARE_DYNAMIC(CLogDlg)

public:
	CLogDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLogDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListBox m_ctrlLog;
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
};
