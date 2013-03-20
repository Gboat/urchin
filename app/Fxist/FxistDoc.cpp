// FistDoc.cpp :  CFxistDoc 类的实现
//

#include "stdafx.h"
#include "Fxist.h"

#include "FxistDoc.h"
#include ".\fxistdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFxistDoc

IMPLEMENT_DYNCREATE(CFxistDoc, CDocument)

BEGIN_MESSAGE_MAP(CFxistDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
END_MESSAGE_MAP()


// CFxistDoc 构造/析构

CFxistDoc::CFxistDoc()
{
	m_pIndex = NULL;
	m_pIndexSearcher = NULL;
}

CFxistDoc::~CFxistDoc()
{	
	
	if(m_pIndex)
	{
		delete m_pIndex;
		m_pIndex = NULL;
	}
	if(m_pIndexSearcher)
	{
		m_pIndexSearcher->detachReader();
		delete m_pIndexSearcher;
		m_pIndexSearcher = NULL;
	}	
}




// CFxistDoc 序列化

void CFxistDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CFxistDoc 诊断

#ifdef _DEBUG
void CFxistDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFxistDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CFxistDoc 命令

void CFxistDoc::OnFileOpen()
{	
	CString sFolder = BrowseForFolder();
	if(sFolder != _T(""))
	{		
		OpenIndex(sFolder,CIndex::ACCESS_SEARCH);
	}
}
void CFxistDoc::OnFileClose()
{
	CloseIndex();
	UpdateAllViews(NULL,UPDATEVIEW_CLOSEINDEX);
}
BOOL CFxistDoc::OpenIndex(const CString& strIndexName,CIndex::ACCESS_MODE am)
{
	CloseIndex();
	try
	{
		m_sIndexName = strIndexName;
		m_pIndex = new CIndex((LPCTSTR)strIndexName,am);		
		UpdateAllViews(NULL,UPDATEVIEW_OPENINDEX);		
	}
	catch (CFirteXException& e)
	{
		CString strError = _T("打开索引失败：");
		strError += e.what();
		AfxMessageBox(strError);
	}
	return TRUE;
}
void CFxistDoc::CloseIndex()
{		
	try
	{
		if(m_pIndexSearcher)
			m_pIndexSearcher->detachReader();
		if(m_pIndex)
		{
			delete m_pIndex;		
			m_pIndex = NULL;
		}
		if(m_pIndexSearcher)
		{
			delete m_pIndexSearcher;		
			m_pIndexSearcher = NULL;
		}	
		UpdateAllViews(NULL,UPDATEVIEW_CLOSEINDEX);
	}
	catch (CFirteXException& fe)
	{		
		CString strError = _T("关闭索引失败：");
		strError += fe.what();
		AfxMessageBox(strError);
	}
	
}

index::CIndexReader* CFxistDoc::GetIndexReader()
{	
	ASSERT(m_pIndex);
	return m_pIndex->acquireReader();
}
search::CIndexSearcher* CFxistDoc::GetIndexSearcher()
{
	ASSERT(m_pIndex);
	if(m_pIndexSearcher == NULL)
		m_pIndexSearcher = new search::CIndexSearcher(GetIndexReader());
	else 
	{
		m_pIndexSearcher->detachReader();
		m_pIndexSearcher->attachReader(GetIndexReader());
	}
	
	return m_pIndexSearcher;
}


CString CFxistDoc::BrowseForFolder()
{	
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));

	bi.hwndOwner = AfxGetApp()->GetMainWnd()->m_hWnd;
	bi.ulFlags   = BIF_RETURNONLYFSDIRS;

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	BOOL bRet = FALSE;

	TCHAR szFolder[MAX_PATH*2];
	szFolder[0] = _T('\0');

	if (pidl)
	{
		if (SHGetPathFromIDList(pidl, szFolder))
		{
			bRet = TRUE;
		}

		::IMalloc *pMalloc = NULL; 
		if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc) 
		{  
			pMalloc->Free(pidl);  
			pMalloc->Release(); 
		}
	}
	if(!bRet)
		return _T("");
	return szFolder;	
}

void CFxistDoc::OnUpdateFileClose(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pIndex!=NULL);
}



