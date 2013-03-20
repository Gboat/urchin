//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: 郭瑞杰(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/6/9
//

#include "stdafx.h"
#include "Fxist.h"
#include "CollectionCfgDlg.h"


// CCollectionCfgDlg 对话框

IMPLEMENT_DYNAMIC(CCollectionCfgDlg, CDialog)
CCollectionCfgDlg::CCollectionCfgDlg(CArgument* pArgument,CWnd* pParent /*=NULL*/)
: CDialog(CCollectionCfgDlg::IDD, pParent)
, m_bIndexModifyDate(FALSE)
, m_strDateField(_T("modified"))
, m_bStore(FALSE)
, m_bIndex(FALSE)
,m_pArgument(pArgument)
{
}

CCollectionCfgDlg::~CCollectionCfgDlg()
{
}

void CCollectionCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_INDEXMODIFY, m_bIndexModifyDate);
	DDX_Text(pDX, IDC_EDIT_FIELDNAME, m_strDateField);
	DDX_Check(pDX, IDC_CHECK_STORE, m_bStore);
	DDX_Check(pDX, IDC_CHECK_INDEX, m_bIndex);
}


BEGIN_MESSAGE_MAP(CCollectionCfgDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_INDEXMODIFY, OnBnClickedCheckIndexmodify)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_STORE, OnBnClickedCheckStore)
	ON_BN_CLICKED(IDC_CHECK_INDEX, OnBnClickedCheckIndex)
END_MESSAGE_MAP()


// CCollectionCfgDlg 消息处理程序

void CCollectionCfgDlg::OnBnClickedCheckIndexmodify()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_EDIT_FIELDNAME)->EnableWindow(m_bIndexModifyDate);
	GetDlgItem(IDC_CHECK_STORE)->EnableWindow(m_bIndexModifyDate);
	GetDlgItem(IDC_CHECK_INDEX)->EnableWindow(m_bIndexModifyDate);
	if(m_bIndexModifyDate)
	{
		if(m_strDateField == _T(""))
			m_strDateField = _T("modified");
		if(!m_bIndex && !m_bStore)
			m_bStore = TRUE;
		UpdateData(FALSE);
	}

}

void CCollectionCfgDlg::OnBnClickedOk()
{
	if(m_bIndexModifyDate)
	{
		m_pArgument->collection.modifieddate.field = (LPCTSTR)m_strDateField;
		m_pArgument->collection.modifieddate.store = (m_bStore==TRUE);
		m_pArgument->collection.modifieddate.index = (m_bIndex==TRUE);		
	}
	else 
	{
		m_pArgument->collection.modifieddate.field = _T("");
		m_pArgument->collection.modifieddate.store = false;
		m_pArgument->collection.modifieddate.index = false;	
	}
	OnOK();
}

void CCollectionCfgDlg::OnBnClickedCheckStore()
{
	UpdateData(TRUE);
	if(!m_bIndex && !m_bStore)
	{
		AfxMessageBox(_T("日期字段必须或者索引或者存储"));
		m_bStore = TRUE;
		UpdateData(FALSE);
	}
}

void CCollectionCfgDlg::OnBnClickedCheckIndex()
{
	UpdateData(TRUE);
	if(!m_bIndex && !m_bStore)
	{
		AfxMessageBox(_T("日期字段必须或者索引或者存储"));
		m_bStore = TRUE;
		UpdateData(FALSE);
	}
}

BOOL CCollectionCfgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_strDateField = m_pArgument->collection.modifieddate.field.c_str();	
	m_bStore = (m_pArgument->collection.modifieddate.store == true);
	m_bIndex = (m_pArgument->collection.modifieddate.index == true);
	if(m_bStore || m_bIndex)
		m_bIndexModifyDate = TRUE;

	UpdateData(FALSE);
	OnBnClickedCheckIndexmodify();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
