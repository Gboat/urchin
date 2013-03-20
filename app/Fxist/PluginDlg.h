#pragma once
#include "ChildDialog.h"
#include "afxcmn.h"

// CPluginDlg �Ի���

class CPluginDlg : public CChildDialog
{
	DECLARE_DYNCREATE(CPluginDlg)

public:
	CPluginDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPluginDlg();

// �Ի�������
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_ctrlistPlugin;
	afx_msg void OnLvnItemchangedListPlugin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSetup();
	afx_msg void OnBnClickedButtonGetmore();
	afx_msg void OnNMClickListPlugin(NMHDR *pNMHDR, LRESULT *pResult);
};
