#pragma once
#include "ChildDialog.h"
#include "afxcmn.h"
#include "textprogressctrl.h"

// CIndexMaintainDlg 对话框

class CIndexMaintainDlg;
class CMergeProgressDlg;
class CIndexMaintainDlg : public CChildDialog
{
	DECLARE_DYNCREATE(CIndexMaintainDlg)

public:
	CIndexMaintainDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIndexMaintainDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_INDEXMAINTAIN };

protected:
	virtual void	OnOpenIndex();
	virtual	void	OnCloseIndex();
	void			EnableControls(BOOL bEnable);
	static	LONG	MergeThreadProc(LPVOID param);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAddindex();
	virtual BOOL OnInitDialog();
	CListCtrl m_ctrlListMergeIndex;
	afx_msg void OnBnClickedButtonDelindex();
	afx_msg void OnBnClickedButtonMergeindex();
	CListCtrl m_ctrlListMergeBarrels;	
	afx_msg void OnBnClickedRadioMerge();
	afx_msg void OnBnClickedRadioOptmerge();
	afx_msg void OnBnClickedRadioPartmerge();
	int m_nMerge;
	afx_msg void OnBnClickedButtonMergebarrels();
	afx_msg LRESULT	OnMergeComplete(WPARAM wParam,LPARAM lParam);
protected:	
	CString				m_strMergeStrategy;
	CString				m_strMergeParam;
	CIndex*				m_pIndexToMerge;	
public:
	afx_msg void OnBnClickedButtonMergebrowse();
	CString m_strMergeIndex;
	CTextProgressCtrl m_ctrlMergeProg;
	afx_msg void OnBnClickedRadioMratio();
	afx_msg void OnBnClickedRadioMdelnum();
	afx_msg void OnBnClickedRadioMforcegc();
	int m_nGC;
	afx_msg void OnBnClickedButtonHmadvance();
	UINT m_nDelDocs;
	float m_fRatio;
};
