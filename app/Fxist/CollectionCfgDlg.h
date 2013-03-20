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
#pragma once

#include "IndexingDlg.h"

// CCollectionCfgDlg 对话框

using namespace firtex::collection;
class CCollectionCfgDlg : public CDialog
{
	DECLARE_DYNAMIC(CCollectionCfgDlg)

public:
	CCollectionCfgDlg(CArgument* pArgument,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCollectionCfgDlg();

	// 对话框数据
	enum { IDD = IDD_DIALOG_COLCFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bIndexModifyDate;
	afx_msg void OnBnClickedCheckIndexmodify();
	CString m_strDateField;
	BOOL m_bStore;
	BOOL m_bIndex;
	afx_msg void OnBnClickedOk();
protected:
	CArgument*	m_pArgument;
public:
	afx_msg void OnBnClickedCheckStore();
	afx_msg void OnBnClickedCheckIndex();
	virtual BOOL OnInitDialog();
};

