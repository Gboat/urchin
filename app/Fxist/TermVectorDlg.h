#pragma once
#include "ChildDialog.h"


// CTermVectorDlg �Ի���

class CTermVectorDlg : public CChildDialog
{
	DECLARE_DYNCREATE(CTermVectorDlg)

public:
	CTermVectorDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTermVectorDlg();


// �Ի�������
	enum { IDD = IDD_FORWARDINDEXDLG };

public:
	virtual void	OnOpenIndex();
	virtual	void	OnCloseIndex();

	void			EnableControls(BOOL bEnable);
	void			EnableRadios(int i);
protected:
	void			displayTermVector(docid_t	docID);
	void			displayTermVector(CTermSequenceVector* pSV);
	void			displayTermVector(CTermFreqVector* pFV);
	void			displayTermVector(CTermPositionVector* pPV);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTVQuery();
	virtual BOOL OnInitDialog();
	int m_tvDocNum;
	afx_msg void OnBnClickedButtonTvfirstdoc();
	afx_msg void OnBnClickedButtonTvprevdoc();
	afx_msg void OnBnClickedButtonTvnextdoc();
	afx_msg void OnBnClickedButtonTvlastdoc();
	afx_msg void OnCbnSelchangeComboTvfield();
	CString m_strComboField;
	int m_tvType;
protected:
	index::CForwardIndexReader*	m_pTVReader;
	index::CIndexReader*		m_pIndexReader;
public:
	CString m_strTermVector;
};

//////////////////////////////////////////////////////////////////////////

