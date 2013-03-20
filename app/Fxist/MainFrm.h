// MainFrm.h : CMainFrame ��Ľӿ�
//
#pragma once

#include "LogDlg.h"


class CListBoxLogDelegater : public firtex::utility::tlog_delegater
{
public:
	CListBoxLogDelegater(CListBox* pListBox):m_ctrlListBox(pListBox)
	{
		m_pos = 0;
	}
	~CListBoxLogDelegater(){}
public:
	void log(const tchar* text)
	{
		tchar* p = (char*)text;
		while (*p && *p != _T('\n'))
		{
			m_cachedText[m_pos] = *p;
			p++;
			m_pos++;
		}
		if(*p == _T('\n'))
		{
			while (m_ctrlListBox->GetCount() > 1000)
			{
				m_ctrlListBox->DeleteString(0);
			}
			m_cachedText[m_pos] = 0;
			m_ctrlListBox->AddString(m_cachedText);
			m_ctrlListBox->SetCurSel(m_ctrlListBox->GetCount() - 1);
			m_pos = 0;
		}
	}
protected:
	tchar		m_cachedText[512];
	int			m_pos;
	CListBox*	m_ctrlListBox;
};

class CMainFrame : public CFrameWnd
{
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
public:
	CWnd*		GetLogWindow();
	void		ShowLogWindow(BOOL bShow);
// ����
public:
	virtual	void	OnOpenIndex();
	virtual	void	OnCloseIndex();
// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

	CLogDlg		m_logWindow;
	firtex::utility::tlog_delegater*	m_pLogDeletater;

	CString		m_logToFile;


// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnToolCreatefirtexcol();
	afx_msg void OnViewLogwindow();
	afx_msg void OnUpdateViewLogwindow(CCmdUI *pCmdUI);
	afx_msg void OnToolLogoption();
	afx_msg void OnToolCreatestopfile();
};


