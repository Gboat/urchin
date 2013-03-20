#pragma once
#include "textprogressctrl.h"


// CCreateFXCollectionDlg 对话框

class CCreateFXCollectionDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateFXCollectionDlg)

public:
	CCreateFXCollectionDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCreateFXCollectionDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_CREATEFXCOL };

protected:
	void			EnableControls(BOOL bEnable);
	static LONG		CreateThreadProc(void* param);
	static int		CreateIndexFiles(LPCTSTR lpszMainFileDir,LPCTSTR lpszIndexFile,int minSize,int maxSize,BOOL* bStop,LPCTSTR lpszMergeFile = NULL);
	static int		CreateIndexFile(LPCTSTR lpszMainFile,LPCTSTR lpszIndexFile,int minSize,int maxSize,int startDocNo,BOOL* bStop,int hMainFile = -1);
	static int		SpliteFiles(LPCTSTR lpszMainFileDir,int64_t spliteSize);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strSrcCol;
	afx_msg void OnBnClickedButtonFxcolbrowse();
	BOOL m_bMergeToOne;
	int m_nSpliteMode;
	int m_nMinSize;
	int m_nMaxSize;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonCreate();
	afx_msg void OnBnClickedRadioNosplite();
	afx_msg void OnBnClickedRadioRandsplite();
	afx_msg void OnBnClickedRadioTagsplite();
	afx_msg LRESULT	OnCreateComplete(WPARAM wParam,LPARAM lParam);
	CString m_strBeginTag;
	CString m_strEndTag;
	CTextProgressCtrl m_ctrlCreateProg;

protected:
	BOOL	m_bStopCreate;
	BOOL	m_bCreating;

public:
	CString m_strMergeFile;
	afx_msg void OnBnClickedButtonMergefilebrowse();
	afx_msg void OnBnClickedCheckMergetoone();
};
