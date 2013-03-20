#pragma once

#include "ResizableDialog.h"
#include "afxwin.h"

// CLogDlg �Ի���

class CLogDlg : public CResizableDialog
{
	DECLARE_DYNAMIC(CLogDlg)

public:
	CLogDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLogDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListBox m_ctrlLog;
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
};
