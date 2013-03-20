// FistDoc.h :  CFxistDoc ��Ľӿ�
//

#pragma once


#define UPDATEVIEW_OPENINDEX	1
#define UPDATEVIEW_CLOSEINDEX	2




class CFxistDoc : public ::CDocument
{
protected: // �������л�����
	CFxistDoc();
	DECLARE_DYNCREATE(CFxistDoc)

// ����
public:

// ����
public:
	BOOL OpenIndex(const CString& strIndexName,CIndex::ACCESS_MODE am);
	void CloseIndex();

	CString GetIndexName(){return m_sIndexName;}
	index::CIndex*			GetIndex(){return m_pIndex;}
	index::CIndexReader*	GetIndexReader();	
	search::CIndexSearcher*	GetIndexSearcher();

// ��д
	public:
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CFxistDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CString BrowseForFolder();
	
protected:
	index::CIndex*			m_pIndex;
	search::CIndexSearcher*	m_pIndexSearcher;
	CString					m_sIndexName;

	friend class CFxistView;
// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileClose(CCmdUI *pCmdUI);
	afx_msg void OnFileClose();
};


