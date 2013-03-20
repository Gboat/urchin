// FistDoc.h :  CFxistDoc 类的接口
//

#pragma once


#define UPDATEVIEW_OPENINDEX	1
#define UPDATEVIEW_CLOSEINDEX	2




class CFxistDoc : public ::CDocument
{
protected: // 仅从序列化创建
	CFxistDoc();
	DECLARE_DYNCREATE(CFxistDoc)

// 属性
public:

// 操作
public:
	BOOL OpenIndex(const CString& strIndexName,CIndex::ACCESS_MODE am);
	void CloseIndex();

	CString GetIndexName(){return m_sIndexName;}
	index::CIndex*			GetIndex(){return m_pIndex;}
	index::CIndexReader*	GetIndexReader();	
	search::CIndexSearcher*	GetIndexSearcher();

// 重写
	public:
	virtual void Serialize(CArchive& ar);

// 实现
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
// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileClose(CCmdUI *pCmdUI);
	afx_msg void OnFileClose();
};


