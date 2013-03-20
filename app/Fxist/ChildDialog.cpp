#include "stdafx.h"
#include "ChildDialog.h"
#include ".\childdialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildDialog implementation

BEGIN_MESSAGE_MAP(CChildDialog, CResizableDialog)
	ON_MESSAGE(DM_GETDEFID, OnGetDefID) // ²»Ê¹ÓÃ
END_MESSAGE_MAP()


CChildDialog::CChildDialog (UINT nIDTemplate, CWnd* pParentWnd):CResizableDialog(nIDTemplate,pParentWnd)
{
#ifndef _AFX_NO_OCC_SUPPORT
	AfxEnableControlContainer ();
#endif // !_AFX_NO_OCC_SUPPORT
}

BOOL CChildDialog::OnCommand (WPARAM wParam, LPARAM lParam) 
{	
	return CResizableDialog::OnCommand(wParam,lParam);
}

BOOL CChildDialog::OnNotify (WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{	
	return CResizableDialog::OnNotify(wParam,lParam,pResult);
}

BOOL CChildDialog::OnCmdMsg (UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	//
	// Forward ActiveX control events to the dialog's parent.
	//
#ifndef _AFX_NO_OCC_SUPPORT
	if (nCode == CN_EVENT)
		return GetParent ()->OnCmdMsg (nID, nCode, pExtra, pHandlerInfo);
#endif // !_AFX_NO_OCC_SUPPORT

	return CResizableDialog::OnCmdMsg (nID, nCode, pExtra, pHandlerInfo);
}

BOOL CChildDialog::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	ShowSizeGrip(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control	
}
LRESULT CChildDialog::OnGetDefID(WPARAM wp, LPARAM lp)
{	
	return MAKELONG(0,DC_HASDEFID);
}
