//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: ¹ùÈð½Ü(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/12/27
//
#include "search/QueryHits.h"
#include "search/Searcher.h"
#include "search/Query.h"

namespace firtex
{
	namespace search
	{

		CQueryHits::CQueryHits(void)
			:m_pSearcher(NULL)
			,m_pQuery(NULL)
			,m_ppHitDocs(NULL)
			,m_nNumHitdDocs(0)
		{
		}
		CQueryHits::CQueryHits(CHitQueue* hq,CQuery* pQuery,CSearcher* pSearcher):m_pQuery(pQuery),m_pSearcher(pSearcher)
		{
			m_nNumHitdDocs = (int32_t)hq->size();
			m_ppHitDocs = new CHitDoc*[m_nNumHitdDocs];
			CScoreDoc* pScoreDoc = hq->pop();
			int32_t i = m_nNumHitdDocs - 1;
			while (pScoreDoc)
			{				
				m_ppHitDocs[i--] = new CHitDoc(pScoreDoc->m_docID,pScoreDoc->m_fScore,this);				
				delete pScoreDoc;
				pScoreDoc = hq->pop();
			}
		}
		CQueryHits::~CQueryHits(void)
		{
			clear();
		}	

		document::CDocument* CQueryHits::doc(int32_t n)
		{
			CHitDoc* pHitDoc = hitDoc(n);
			if(pHitDoc == NULL)
				return NULL;
			if (pHitDoc->m_doc == NULL)
			{
				pHitDoc->m_doc = m_pSearcher->doc(pHitDoc->m_docID);
			}

			return pHitDoc->m_doc;
		}						

		CQueryHits::CHitDoc* CQueryHits::hitDoc(int32_t n)
		{
			if( (n < 0) || (n >= m_nNumHitdDocs) )
			{
				return NULL;
			}
			return (CHitDoc*) m_ppHitDocs[n];
		}		
		void CQueryHits::getTopDocs(int32_t topk)
		{
		}
		void CQueryHits::clear()
		{
			for (int32_t i = 0;i<m_nNumHitdDocs;i++)
			{
				delete m_ppHitDocs[i];
			}
			delete[] m_ppHitDocs;
			m_ppHitDocs = NULL;
			m_nNumHitdDocs = 0;			
		}
		document::CDocument* CQueryHits::docByID(docid_t did)
		{

			CHitDoc* pHitDoc;
			for (int32_t i = 0;i<m_nNumHitdDocs;i++)
			{
				pHitDoc = m_ppHitDocs[i];
				if(pHitDoc->m_docID == did)
				{
					if(pHitDoc->m_doc == NULL)
						pHitDoc->m_doc = m_pSearcher->doc(pHitDoc->m_docID);
					return pHitDoc->m_doc;
				}			
			}
			return NULL;
		}
		document::CDocument* CQueryHits::getDoc(CHitDoc* pHitDoc)
		{
			if(pHitDoc->m_doc == NULL)
				pHitDoc->m_doc = m_pSearcher->doc(pHitDoc->m_docID);
			return pHitDoc->m_doc;
		}
	}
}
