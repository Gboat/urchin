// FistView.h : CFxistView 类的接口
//


#pragma once

#include "TabView.h"


class CFxistView : public CTabView
{
protected: // 仅从序列化创建
	CFxistView();
	DECLARE_DYNCREATE(CFxistView)

	virtual BOOL OnInitPage (int nIndex, int nPageID);
	virtual void OnActivatePage (int nIndex, int nPageID);
	virtual void OnDeactivatePage (int nIndex, int nPageID);
	virtual void OnDestroyPage (int nIndex, int nPageID);
// 属性
public:
	CFxistDoc* GetDocument() const;

// 操作
public:

// 重写
	public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CFxistView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CFont m_font;

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnFileIndexing();
};

#ifndef _DEBUG  // FistView.cpp 的调试版本
inline CFxistDoc* CFxistView::GetDocument() const
   { return reinterpret_cast<CFxistDoc*>(m_pDocument); }
#endif

