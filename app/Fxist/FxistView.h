// FistView.h : CFxistView ��Ľӿ�
//


#pragma once

#include "TabView.h"


class CFxistView : public CTabView
{
protected: // �������л�����
	CFxistView();
	DECLARE_DYNCREATE(CFxistView)

	virtual BOOL OnInitPage (int nIndex, int nPageID);
	virtual void OnActivatePage (int nIndex, int nPageID);
	virtual void OnDeactivatePage (int nIndex, int nPageID);
	virtual void OnDestroyPage (int nIndex, int nPageID);
// ����
public:
	CFxistDoc* GetDocument() const;

// ����
public:

// ��д
	public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// ʵ��
public:
	virtual ~CFxistView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CFont m_font;

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnFileIndexing();
};

#ifndef _DEBUG  // FistView.cpp �ĵ��԰汾
inline CFxistDoc* CFxistView::GetDocument() const
   { return reinterpret_cast<CFxistDoc*>(m_pDocument); }
#endif

