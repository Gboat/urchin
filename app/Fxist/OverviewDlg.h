#pragma once

#include "ChildDialog.h"
#include "afxcmn.h"

// COverviewDlg �Ի���


class COverviewDlg : public CChildDialog
{
	DECLARE_DYNCREATE(COverviewDlg)

public:
	COverviewDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~COverviewDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_OVERVIEW };

public:
	virtual void	OnOpenIndex();
	virtual	void	OnCloseIndex();
public:
	void	FillFieldInfo();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	CListCtrl m_ctrlListField;
	CListCtrl m_ctrlListBarrels;	
};
