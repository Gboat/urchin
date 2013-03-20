#pragma once

#include "ChildDialog.h"
#include "afxcmn.h"

// COverviewDlg 对话框


class COverviewDlg : public CChildDialog
{
	DECLARE_DYNCREATE(COverviewDlg)

public:
	COverviewDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COverviewDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_OVERVIEW };

public:
	virtual void	OnOpenIndex();
	virtual	void	OnCloseIndex();
public:
	void	FillFieldInfo();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	CListCtrl m_ctrlListField;
	CListCtrl m_ctrlListBarrels;	
};
