#pragma once


// CHierMergeAdvanceDlg �Ի���

class CHierMergeAdvanceDlg : public CDialog
{
	DECLARE_DYNAMIC(CHierMergeAdvanceDlg)

public:
	CHierMergeAdvanceDlg(LPCTSTR pszStrategy,LPCTSTR pszParam,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CHierMergeAdvanceDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_HMADVANCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString		m_strStrategy;
	CString		m_strParam;
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	int m_nStrategy;
	int m_nGPParam;
	UINT m_nHLMM;
	UINT m_nHLMC;
	UINT m_nHLMS;
	UINT m_nGPR;
	UINT m_nGPP;
	afx_msg void OnBnClickedRadioHlm();
	afx_msg void OnBnClickedRadioGp();
	afx_msg void OnBnClickedRadioLog();
	afx_msg void OnBnClickedRadioGpR();
	afx_msg void OnBnClickedRadioGpP();
};
