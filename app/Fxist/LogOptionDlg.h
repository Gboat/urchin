#pragma once


// CLogOptionDlg �Ի���

class CLogOptionDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogOptionDlg)

public:
	CLogOptionDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLogOptionDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_LOGOPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLogbrowse();
	CString m_strLogFile;
	afx_msg void OnBnClickedButtonViewlog();
	afx_msg void OnBnClickedRadioTofile();
	afx_msg void OnBnClickedRadioTowindow();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	int m_nLogMode;
};
