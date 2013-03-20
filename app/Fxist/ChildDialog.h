#if !defined(AFX_CHILDDIALOG_H__619331B3_7DE7_4DB1_A039_2103E87E8E71__INCLUDED_)
#define AFX_CHILDDIALOG_H__619331B3_7DE7_4DB1_A039_2103E87E8E71__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CChildDialog declaration

#include "ResizableDialog.h"

class CFxistDoc;
class CChildDialog : public CResizableDialog
{
public:
	CChildDialog (UINT nIDTemplate, CWnd* pParentWnd = NULL);

protected:
	virtual BOOL OnCommand (WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify (WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCmdMsg (UINT nID, int nCode, void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo);
public:
	virtual BOOL OnInitDialog();
public:	
	virtual	void OnOpenIndex() = 0;
	virtual	void OnCloseIndex() = 0;
	virtual void OnActivate(){}
	virtual void OnDeactivate(){}

	afx_msg LRESULT OnGetDefID(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_CHILDDIALOG_H__619331B3_7DE7_4DB1_A039_2103E87E8E71__INCLUDED_)
