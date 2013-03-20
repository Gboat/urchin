// ResizableDialog.cpp : implementation file
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

#include "stdafx.h"
#include "ResizableDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CResizableLayout


//#define BS_TYPEMASK SS_TYPEMASK

void CResizableLayout::AddAnchor(HWND hWnd, CSize sizeTypeTL, CSize sizeTypeBR)
{
	CWnd* pParent = GetResizableWnd();

	// child window must be valid
	ASSERT(::IsWindow(hWnd));
	// must be child of parent window
	ASSERT(::IsChild(pParent->GetSafeHwnd(), hWnd));
	// top-left anchor must be valid
	ASSERT(sizeTypeTL != NOANCHOR);

	// get control's window class
	CString st;
	GetClassName(hWnd, st.GetBufferSetLength(MAX_PATH), MAX_PATH);
	st.ReleaseBuffer();

	// add the style 'clipsiblings' to a GroupBox
	// to avoid unnecessary repainting of controls inside
	if (st == "Button")
	{
		DWORD style = GetWindowLong(hWnd, GWL_STYLE);
		if ((style & SS_TYPEMASK) == BS_GROUPBOX)
			SetWindowLong(hWnd, GWL_STYLE, style | WS_CLIPSIBLINGS);
	}

	// window classes that don't redraw client area correctly
	// when the hor scroll pos changes due to a resizing
	BOOL bHScroll = FALSE;
	if (st == "ListBox")
		bHScroll = TRUE;

	// window classes that need refresh when resized
	BOOL bRefresh = NeedsRefresh(hWnd);

	// get parent window's rect
	CRect rectParent;
	GetTotalClientRect(&rectParent);
	// and child control's rect
	CRect rectChild;
	::GetWindowRect(hWnd, &rectChild);
	pParent->ScreenToClient(&rectChild);

	// go calculate margins
	CSize sizeMarginTL, sizeMarginBR;

	if (sizeTypeBR == NOANCHOR)
		sizeTypeBR = sizeTypeTL;

	// calculate margin for the top-left corner

	sizeMarginTL.cx = rectChild.left - rectParent.Width() * sizeTypeTL.cx / 100;
	sizeMarginTL.cy = rectChild.top - rectParent.Height() * sizeTypeTL.cy / 100;

	// calculate margin for the bottom-right corner

	sizeMarginBR.cx = rectChild.right - rectParent.Width() * sizeTypeBR.cx / 100;
	sizeMarginBR.cy = rectChild.bottom - rectParent.Height() * sizeTypeBR.cy / 100;

	// add to the list
	LayoutInfo layout(hWnd, sizeTypeTL, sizeMarginTL,
		sizeTypeBR, sizeMarginBR, bHScroll, bRefresh);
	// always add before callbacks
	m_arrLayout.InsertAt(m_iFirstCallback++, layout);
}

// one callback control cannot rely upon another callback control's
// size and/or position (they're updated all together at the end)
void CResizableLayout::AddAnchorCallback(UINT nCallbackID)
{
	// add to the list
	LayoutInfo layout;
	layout.nCallbackID = nCallbackID;
	// callbacks added to tail (for efficiency)
	m_arrLayout.Add(layout);
}

BOOL CResizableLayout::ArrangeLayoutCallback(LayoutInfo& /*layout*/)
{
	ASSERT(FALSE);
	// must be overridden, if callback is used

	return FALSE;	// no output data
}

void CResizableLayout::ArrangeLayout()
{
	CWnd* pParent = GetResizableWnd();

	// get parent window's rect
	CRect rectParent;
	GetTotalClientRect(&rectParent);

	// init some vars
	int i, count = m_arrLayout.GetSize();
	HDWP hdwp = BeginDeferWindowPos(count);

	for (i=0; i<count; ++i)
	{
		LayoutInfo layout = m_arrLayout[i];

		if (layout.hWnd == NULL)	// callback
		{
			if (i == m_iFirstCallback)	// first time only
			{
				// update previous controls
				EndDeferWindowPos(hdwp);
				// start again for callback controls
				hdwp = BeginDeferWindowPos(count-i);
			}
			// callbacks are added at the end, so that
			// you don't have multiple screen updates

			if (!ArrangeLayoutCallback(layout))	// request data
				continue;
		}

		CRect rectChild, newrc;
		CWnd* pWnd = CWnd::FromHandle(layout.hWnd); // temporary solution

		pWnd->GetWindowRect(&rectChild);
		pParent->ScreenToClient(&rectChild);

		// calculate new top-left corner

		newrc.left = layout.sizeMarginTL.cx + rectParent.Width() * layout.sizeTypeTL.cx / 100;
		newrc.top = layout.sizeMarginTL.cy + rectParent.Height() * layout.sizeTypeTL.cy / 100;

		// calculate new bottom-right corner

		newrc.right = layout.sizeMarginBR.cx + rectParent.Width() * layout.sizeTypeBR.cx / 100;
		newrc.bottom = layout.sizeMarginBR.cy + rectParent.Height() * layout.sizeTypeBR.cy / 100;

		if (!newrc.EqualRect(&rectChild))
		{
			if (layout.bAdjHScroll)
			{
				// needs repainting, due to horiz scrolling
				int diff = newrc.Width() - rectChild.Width();
				int max = pWnd->GetScrollLimit(SB_HORZ);

				layout.bNeedRefresh = FALSE;
				if (max > 0 && pWnd->GetScrollPos(SB_HORZ) > max - diff)
				{
					layout.bNeedRefresh = TRUE;
				}
			}

			// set flags 
			DWORD flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREPOSITION;
			if (layout.bNeedRefresh)
				flags |= SWP_NOCOPYBITS;
			if (newrc.TopLeft() == rectChild.TopLeft())
				flags |= SWP_NOMOVE;
			if (newrc.Size() == rectChild.Size())
				flags |= SWP_NOSIZE;

			hdwp = DeferWindowPos(hdwp, layout.hWnd, NULL, newrc.left, newrc.top,
				newrc.Width(), newrc.Height(), flags);
		}
	}
	// go re-arrange child windows
	EndDeferWindowPos(hdwp);
}

void CResizableLayout::EnumAndClipChildWindow(HWND hWnd, CDC* pDC)
{
	// obtain window position
	CRect rect;
	::GetWindowRect(hWnd, &rect);
	GetResizableWnd()->ScreenToClient(&rect);
	pDC->DPtoLP(&rect);

	// use window region if any
	CRgn rgn;
	rgn.CreateRectRgn(0,0,0,0);
	if (COMPLEXREGION == ::GetWindowRgn(hWnd, rgn))
	{
		rgn.OffsetRgn(rect.TopLeft());
	}
	else
	{
		rgn.SetRectRgn(&rect);
	}

	// go clipping?
	if (LikesClipping(hWnd))
		pDC->SelectClipRgn(&rgn, RGN_DIFF);
	else
		pDC->SelectClipRgn(&rgn, RGN_OR);
}

void CResizableLayout::ClipChildren(CDC *pDC)
{
	// only clips anchored controls
	for (int i=0; i<m_arrLayout.GetSize(); ++i)
	{
		HWND hWnd = m_arrLayout[i].hWnd;
		if (hWnd != NULL && ::IsWindowVisible(hWnd))
			EnumAndClipChildWindow(m_arrLayout[i].hWnd, pDC);
	}
}

void CResizableLayout::GetTotalClientRect(LPRECT lpRect)
{
	GetResizableWnd()->GetClientRect(lpRect);
}

BOOL CResizableLayout::NeedsRefresh(HWND hWnd)
{
	// get window class
	CString st;
	GetClassName(hWnd, st.GetBufferSetLength(MAX_PATH), MAX_PATH);
	st.ReleaseBuffer();

	// optimistic, no need to refresh
	BOOL bRefresh = FALSE;

	// window classes that need refresh when resized
	if (st == "Static")
	{
		DWORD style = GetWindowLong(hWnd, GWL_STYLE);

		switch (style & SS_TYPEMASK)
		{
		case SS_LEFT:
		case SS_CENTER:
		case SS_RIGHT:
		case SS_ENHMETAFILE:
		case SS_BITMAP:
			// word-wrapped text needs bRefresh
			bRefresh = TRUE;
			break;

		case SS_ICON:
		case SS_SIMPLE:
		case SS_ETCHEDHORZ:
		case SS_ETCHEDVERT:
			break;

		default:
			// centered images or text need bRefresh
			if (style & SS_CENTERIMAGE)
				bRefresh = TRUE;
		}
	}

	return bRefresh;
}

BOOL CResizableLayout::LikesClipping(HWND hWnd)
{
	// check child type
	CString st;
	GetClassName(hWnd, st.GetBufferSetLength(MAX_PATH), MAX_PATH);
	st.ReleaseBuffer();

	DWORD style = GetWindowLong(hWnd, GWL_STYLE);

	// skip windows that wants background repainted
	if (st == TOOLBARCLASSNAME && (style & TBSTYLE_TRANSPARENT))
		return FALSE;
	if (st == "Button" && (style & SS_TYPEMASK) == BS_GROUPBOX)
		return FALSE;
	if (st == "Static")
	{
		switch (style & SS_TYPEMASK)
		{
		case SS_BLACKRECT:
		case SS_GRAYRECT:
		case SS_WHITERECT:
		case SS_ETCHEDHORZ:
		case SS_ETCHEDVERT:
		case SS_BITMAP:
			break;
		case SS_ICON:
		case SS_ENHMETAFILE:
			if (style & SS_CENTERIMAGE)
				return FALSE;
			break;
		default:
			return FALSE;
		}
	}

	// assume the others like clipping
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CResizableGrip

CResizableGrip::CResizableGrip()
{
	m_sizeGrip.cx = GetSystemMetrics(SM_CXVSCROLL);
	m_sizeGrip.cy = GetSystemMetrics(SM_CYHSCROLL);
}

CResizableGrip::~CResizableGrip()
{

}

void CResizableGrip::UpdateGripPos()
{
	// size-grip goes bottom right in the client area
	// (any right-to-left adjustment should go here)

	RECT rect;
	GetResizableWnd()->GetClientRect(&rect);

	rect.left = rect.right - m_sizeGrip.cx;
	rect.top = rect.bottom - m_sizeGrip.cy;

	// must stay below other children
	m_wndGrip.SetWindowPos(&CWnd::wndBottom, rect.left, rect.top, 0, 0,
		SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREPOSITION);

	// maximized windows cannot be resized
	if (GetResizableWnd()->IsZoomed())
		m_wndGrip.EnableWindow(FALSE);
	else
		m_wndGrip.EnableWindow(TRUE);
}

void CResizableGrip::ShowSizeGrip(BOOL bShow)
{
	m_wndGrip.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
}

#define RSZ_GRIP_OBJ	_T("ResizableGrip")

BOOL CResizableGrip::InitGrip()
{
	CRect rect(0 , 0, m_sizeGrip.cx, m_sizeGrip.cy);

	BOOL bRet = m_wndGrip.Create(WS_CHILD | WS_CLIPSIBLINGS | SBS_SIZEGRIP,
		rect, GetResizableWnd(), 0);

	if (bRet)
	{
		// set a triangular window region
		CRgn rgnGrip, rgn;
		rgn.CreateRectRgn(0,0,1,1);
		rgnGrip.CreateRectRgnIndirect(&rect);

		for (int y=0; y<m_sizeGrip.cy; y++)
		{
			rgn.SetRectRgn(0, y, m_sizeGrip.cx-y, y+1);
			rgnGrip.CombineRgn(&rgnGrip, &rgn, RGN_DIFF);
		}
		m_wndGrip.SetWindowRgn((HRGN)rgnGrip.Detach(), FALSE);

		// subclass control
		::SetProp(m_wndGrip, RSZ_GRIP_OBJ,
			(HANDLE)::GetWindowLong(m_wndGrip, GWL_WNDPROC));
		::SetWindowLong(m_wndGrip, GWL_WNDPROC, (LONG)GripWindowProc);

		// update pos
		UpdateGripPos();
		ShowSizeGrip();
	}

	return bRet;
}

LRESULT CResizableGrip::GripWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WNDPROC oldWndProc = (WNDPROC)::GetProp(hwnd, RSZ_GRIP_OBJ);

	switch (msg)
	{
	case WM_NCHITTEST:

		// choose proper cursor shape
		if (IsRTL(hwnd))
			return HTBOTTOMLEFT;
		else
			return HTBOTTOMRIGHT;

	case WM_DESTROY:

		// unsubclass
		::RemoveProp(hwnd, RSZ_GRIP_OBJ);
		::SetWindowLong(hwnd, GWL_WNDPROC, (LONG)oldWndProc);

		break;
	}

	return ::CallWindowProc(oldWndProc, hwnd, msg, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////////
// CResizableState

CResizableState::CResizableState()
{

}

CResizableState::~CResizableState()
{

}


// used to save/restore window's size and position
// either in the registry or a private .INI file
// depending on your application settings

#define PLACEMENT_ENT	_T("WindowPlacement")
#define PLACEMENT_FMT 	_T("%d,%d,%d,%d,%d,%d")

BOOL CResizableState::SaveWindowRect(LPCTSTR pszSection, BOOL bRectOnly)
{
	CString data;
	WINDOWPLACEMENT wp;

	ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
	wp.length = sizeof(WINDOWPLACEMENT);
	if (!GetResizableWnd()->GetWindowPlacement(&wp))
		return FALSE;

	RECT& rc = wp.rcNormalPosition;	// alias

	if (bRectOnly)	// save size/pos only (normal state)
	{
		// use screen coordinates
		GetResizableWnd()->GetWindowRect(&rc);

		data.Format(PLACEMENT_FMT, rc.left, rc.top,
			rc.right, rc.bottom, SW_NORMAL, 0);
	}
	else	// save also min/max state
	{
		// use workspace coordinates
		data.Format(PLACEMENT_FMT, rc.left, rc.top,
			rc.right, rc.bottom, wp.showCmd, wp.flags);
	}

	return AfxGetApp()->WriteProfileString(pszSection, PLACEMENT_ENT, data);
}

BOOL CResizableState::LoadWindowRect(LPCTSTR pszSection, BOOL bRectOnly)
{
	CString data;
	WINDOWPLACEMENT wp;

	data = AfxGetApp()->GetProfileString(pszSection, PLACEMENT_ENT);

	if (data.IsEmpty())	// never saved before
		return FALSE;

	ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
	wp.length = sizeof(WINDOWPLACEMENT);
	if (!GetResizableWnd()->GetWindowPlacement(&wp))
		return FALSE;

	RECT& rc = wp.rcNormalPosition;	// alias

	if (_stscanf(data, PLACEMENT_FMT, &rc.left, &rc.top,
		&rc.right, &rc.bottom, &wp.showCmd, &wp.flags) == 6)
	{
		if (bRectOnly)	// restore size/pos only
		{
			CRect rect(rc);
			return GetResizableWnd()->SetWindowPos(NULL, rect.left, rect.top,
				rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER |
				SWP_NOREPOSITION);
		}
		else	// restore also min/max state
		{
			return GetResizableWnd()->SetWindowPlacement(&wp);
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CResizableMinMax


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResizableMinMax::CResizableMinMax()
{
	m_bUseMinTrack = FALSE;
	m_bUseMaxTrack = FALSE;
	m_bUseMaxRect = FALSE;
}

CResizableMinMax::~CResizableMinMax()
{

}

void CResizableMinMax::MinMaxInfo(LPMINMAXINFO lpMMI)
{
	if (m_bUseMinTrack)
		lpMMI->ptMinTrackSize = m_ptMinTrackSize;

	if (m_bUseMaxTrack)
		lpMMI->ptMaxTrackSize = m_ptMaxTrackSize;

	if (m_bUseMaxRect)
	{
		lpMMI->ptMaxPosition = m_ptMaxPos;
		lpMMI->ptMaxSize = m_ptMaxSize;
	}
}

void CResizableMinMax::SetMaximizedRect(const CRect& rc)
{
	m_bUseMaxRect = TRUE;

	m_ptMaxPos = rc.TopLeft();
	m_ptMaxSize.x = rc.Width();
	m_ptMaxSize.y = rc.Height();
}

void CResizableMinMax::ResetMaximizedRect()
{
	m_bUseMaxRect = FALSE;
}

void CResizableMinMax::SetMinTrackSize(const CSize& size)
{
	m_bUseMinTrack = TRUE;

	m_ptMinTrackSize.x = size.cx;
	m_ptMinTrackSize.y = size.cy;
}

void CResizableMinMax::ResetMinTrackSize()
{
	m_bUseMinTrack = FALSE;
}

void CResizableMinMax::SetMaxTrackSize(const CSize& size)
{
	m_bUseMaxTrack = TRUE;

	m_ptMaxTrackSize.x = size.cx;
	m_ptMaxTrackSize.y = size.cy;
}

void CResizableMinMax::ResetMaxTrackSize()
{
	m_bUseMaxTrack = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CResizableDialog

inline void CResizableDialog::Construct()
{
	m_bInitDone = FALSE;

	m_bEnableSaveRestore = FALSE;
}

CResizableDialog::CResizableDialog()
{
	Construct();
}

CResizableDialog::CResizableDialog(UINT nIDTemplate, CWnd* pParentWnd)
	: CDialog(nIDTemplate, pParentWnd)
{
	Construct();
}

CResizableDialog::CResizableDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
	: CDialog(lpszTemplateName, pParentWnd)
{
	Construct();
}

CResizableDialog::~CResizableDialog()
{
}


BEGIN_MESSAGE_MAP(CResizableDialog, CDialog)
	//{{AFX_MSG_MAP(CResizableDialog)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CResizableDialog message handlers

int CResizableDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// keep client area
	CRect rect;
	GetClientRect(&rect);
	// set resizable style
//	ModifyStyle(DS_MODALFRAME, WS_POPUP | WS_THICKFRAME);
	// adjust size to reflect new style
	::AdjustWindowRectEx(&rect, GetStyle(),
		::IsMenu(GetMenu()->GetSafeHmenu()), GetExStyle());
	SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_FRAMECHANGED|
		SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOREPOSITION);

	if (!InitGrip())
		return -1;

	return 0;
}

BOOL CResizableDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// gets the template size as the min track size
	CRect rc;
	GetWindowRect(&rc);
	SetMinTrackSize(rc.Size());

	// init

	UpdateGripPos();

	m_bInitDone = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CResizableDialog::OnDestroy() 
{
	if (m_bEnableSaveRestore)
		SaveWindowRect(m_sSection, m_bRectOnly);

	// remove child windows
	RemoveAllAnchors();

	CDialog::OnDestroy();
}

void CResizableDialog::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	if (nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW)
		return;		// arrangement not needed

	if (m_bInitDone)
	{
		// update size-grip
		UpdateGripPos();

		ArrangeLayout();
	}
}

void CResizableDialog::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (!m_bInitDone)
		return;

	MinMaxInfo(lpMMI);
}

// NOTE: this must be called after setting the layout
//       to have the dialog and its controls displayed properly
void CResizableDialog::EnableSaveRestore(LPCTSTR pszSection, BOOL bRectOnly)
{
	m_sSection = pszSection;

	m_bEnableSaveRestore = TRUE;
	m_bRectOnly = bRectOnly;

	// restore immediately
	LoadWindowRect(pszSection, bRectOnly);
}


BOOL CResizableDialog::OnEraseBkgnd(CDC* pDC) 
{
	ClipChildren(pDC);

	return CDialog::OnEraseBkgnd(pDC);
}
