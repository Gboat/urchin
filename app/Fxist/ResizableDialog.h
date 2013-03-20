#if !defined(AFX_RESIZABLEDIALOG_H__INCLUDED_)
#define AFX_RESIZABLEDIALOG_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <afxtempl.h>

// ResizableDialog.h : header file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2000-2001 by Paolo Messina
// (http://www.geocities.com/ppescher - ppescher@yahoo.com)
//
// The contents of this file are subject to the Artistic License (the "License").
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.opensource.org/licenses/artistic-license.html
//
// If you find this code useful, credits would be nice!
//
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//CResizableLayout
const CSize NOANCHOR(-1,-1),
TOP_LEFT(0,0), TOP_CENTER(50,0), TOP_RIGHT(100,0),
MIDDLE_LEFT(0,50), MIDDLE_CENTER(50,50), MIDDLE_RIGHT(100,50),
BOTTOM_LEFT(0,100), BOTTOM_CENTER(50,100), BOTTOM_RIGHT(100,100);


class CResizableLayout
{
	class LayoutInfo
	{
	public:
		HWND hWnd;
		UINT nCallbackID;

		BOOL bAdjHScroll;
		BOOL bNeedRefresh;

		// upper-left corner
		SIZE sizeTypeTL;
		SIZE sizeMarginTL;

		// bottom-right corner
		SIZE sizeTypeBR;
		SIZE sizeMarginBR;

	public:
		LayoutInfo()
		{ ZeroMemory(this, sizeof(LayoutInfo)); }

		LayoutInfo(HWND hwnd, SIZE tl_t, SIZE tl_m, 
			SIZE br_t, SIZE br_m, BOOL hscroll, BOOL refresh)
		{
			hWnd = hwnd;
			nCallbackID = 0;

			bAdjHScroll = hscroll;
			bNeedRefresh = refresh;

			sizeTypeTL = tl_t;
			sizeMarginTL = tl_m;

			sizeTypeBR = br_t;
			sizeMarginBR = br_m;
		}
	};

	// list of repositionable controls (in 2 parts: anchors, callbacks)
	CArray<LayoutInfo, LayoutInfo&> m_arrLayout;
	int m_iFirstCallback;		// index of first callback

	void EnumAndClipChildWindow(HWND hWnd, CDC* pDC);

protected:
	virtual BOOL LikesClipping(HWND hWnd);
	virtual BOOL NeedsRefresh(HWND hWnd);

	// exclude child windows from the clipping region
	void ClipChildren(CDC *pDC);

	// override for scrollable or expanding parent windows
	virtual void GetTotalClientRect(LPRECT lpRect);

	// add anchors to a control, given its HWND
	void AddAnchor(HWND hWnd, CSize sizeTypeTL, CSize sizeTypeBR = NOANCHOR);

	// add anchors to a control, given its ID
	void AddAnchor(UINT nID, CSize sizeTypeTL, CSize sizeTypeBR = NOANCHOR)
	{
		AddAnchor(::GetDlgItem(GetResizableWnd()->GetSafeHwnd(), nID),
			sizeTypeTL, sizeTypeBR);
	}

	// add a callback (control ID or HWND is unknown or may change)
	void AddAnchorCallback(UINT nCallbackID);

	// adjust children's layout, when parent's size changes
	void ArrangeLayout();

	// override to provide dynamic control's layout info
	virtual BOOL ArrangeLayoutCallback(LayoutInfo& layout);

	// reset layout content
	void RemoveAllAnchors()
	{
		m_arrLayout.RemoveAll();
		m_iFirstCallback = 0;
	}

	virtual CWnd* GetResizableWnd() = 0;

public:
	CResizableLayout() { m_iFirstCallback = 0; }

	virtual ~CResizableLayout()
	{
		// just for safety
		RemoveAllAnchors();
	}
};

////////////////////////////////////////////////////////////////////////////
//CResizableGrip
#define WS_EX_LAYOUT_RTL	0x00400000

class CResizableGrip  
{
private:
	SIZE m_sizeGrip;		// holds grip size

	CScrollBar m_wndGrip;	// grip control

private:
	static BOOL IsRTL(HWND hwnd)
	{
		DWORD dwExStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);
		return (dwExStyle & WS_EX_LAYOUT_RTL);
	}

	static LRESULT CALLBACK GripWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	BOOL InitGrip();
	void UpdateGripPos();
	void ShowSizeGrip(BOOL bShow = TRUE);	// show or hide the size grip

	virtual CWnd* GetResizableWnd() = 0;

public:
	CResizableGrip();
	virtual ~CResizableGrip();

};

////////////////////////////////////////////////////////////////////////////
//CResizableState
class CResizableState  
{
protected:
	// non-zero if successful
	BOOL LoadWindowRect(LPCTSTR pszSection, BOOL bRectOnly);
	BOOL SaveWindowRect(LPCTSTR pszSection, BOOL bRectOnly);

	virtual CWnd* GetResizableWnd() = 0;

public:
	CResizableState();
	virtual ~CResizableState();
};

////////////////////////////////////////////////////////////////////////////
//CResizableMinMax

class CResizableMinMax  
{
	// Attributes
private:
	// flags
	BOOL m_bUseMaxTrack;
	BOOL m_bUseMinTrack;
	BOOL m_bUseMaxRect;

	POINT m_ptMinTrackSize;		// min tracking size
	POINT m_ptMaxTrackSize;		// max tracking size
	POINT m_ptMaxPos;			// maximized position
	POINT m_ptMaxSize;			// maximized size

public:
	CResizableMinMax();
	virtual ~CResizableMinMax();

protected:
	void MinMaxInfo(LPMINMAXINFO lpMMI);

	void SetMaximizedRect(const CRect& rc);		// set window rect when maximized
	void ResetMaximizedRect();					// reset to default maximized rect
	void SetMinTrackSize(const CSize& size);	// set minimum tracking size
	void ResetMinTrackSize();					// reset to default minimum tracking size
	void SetMaxTrackSize(const CSize& size);	// set maximum tracking size
	void ResetMaxTrackSize();					// reset to default maximum tracking size
};

/////////////////////////////////////////////////////////////////////////////
// CResizableDialog window

class CResizableDialog : public CDialog, public CResizableLayout,
						 public CResizableGrip, public CResizableMinMax,
						 public CResizableState
{

// Construction
public:
	CResizableDialog();
	CResizableDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	CResizableDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);

// Attributes
private:
	// flags
	BOOL m_bEnableSaveRestore;
	BOOL m_bRectOnly;

	// internal status
	CString m_sSection;			// section name (identifies a parent window)
	BOOL m_bInitDone;			// if all internal vars initialized

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResizableDialog)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CResizableDialog();

// used internally
private:
	void Construct();

// callable from derived classes
protected:
	// section to use in app's profile
	void EnableSaveRestore(LPCTSTR pszSection, BOOL bRectOnly = FALSE);

	virtual CWnd* GetResizableWnd()
	{
		// make the layout know its parent window
		return this;
	};

// Generated message map functions
protected:
	//{{AFX_MSG(CResizableDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESIZABLEDIALOG_H__INCLUDED_)
