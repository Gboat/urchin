#pragma once
#include "ChildDialog.h"
#include "afxcmn.h"

// CPluginDlg 对话框

class CPluginDlg : public CChildDialog
{
	DECLARE_DYNCREATE(CPluginDlg)

public:
	CPluginDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPluginDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_PLUGIN };
public:
	virtual void	OnOpenIndex();
	virtual	void	OnCloseIndex();
protected:
	void	LoadPlugins();
	int		FindFiles(CString strDir);
	void	displayPluginInfo(firtex::plugin::CPlugin* pPlugin);

	LONG	GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_ctrlistPlugin;
	afx_msg void OnLvnItemchangedListPlugin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSetup();
	afx_msg void OnBnClickedButtonGetmore();
	afx_msg void OnNMClickListPlugin(NMHDR *pNMHDR, LRESULT *pResult);
};
