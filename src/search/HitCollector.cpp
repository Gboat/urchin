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
// Created	: 2006/3/23
//
#include "search/HitCollector.h"

namespace firtex
{
	namespace search
	{
		CSimpleTopDocsCollector::CSimpleTopDocsCollector(CHitQueue* pHitQueue,count_t* totalHits,count_t nDoc,const CBitVector* pFilterBits,score_t ms)
			:m_pHitQueue(pHitQueue)
			,m_pTotalHits(totalHits)
			,m_minScore(ms)
			,m_maxDocs(nDoc)
			,m_pFilterBits(pFilterBits)
		{
		}
		CSimpleTopDocsCollector::~CSimpleTopDocsCollector()
		{			
			m_pFilterBits = NULL;
		}

		void CSimpleTopDocsCollector::collect(docid_t docid,float score)
		{
			if (score > 0.0f &&(m_pFilterBits || m_pFilterBits->test(docid))) 
			{	  
				m_pTotalHits[0]++;
				if ((int)m_pHitQueue->size() < m_maxDocs || (m_minScore== -1.0f || score >= m_minScore))
				{
                    CScoreDoc* sd = new CScoreDoc(docid, score);
					if(!m_pHitQueue->insert(sd))
						delete sd;
					else 
						m_minScore = ((CScoreDoc*)m_pHitQueue->top())->m_fScore; 
				}
			}
		}

		void CSimpleTopDocsCollector::collect(docid_t docid[],score_t score[],count_t size)
		{
			for (int i = 0; i < size; i++)
			{
				if (score[i] > 0.0f &&(!m_pFilterBits || m_pFilterBits->test(docid[i])))
				{	  
					m_pTotalHits[0]++;
					if ((int)m_pHitQueue->size() < m_maxDocs || (m_minScore== -1.0f || score[i] >= m_minScore))
					{
						CScoreDoc* sd = new CScoreDoc(docid[i], score[i]);
						if(!m_pHitQueue->insert(sd))
							delete sd;
						else 
							m_minScore = ((CScoreDoc*)m_pHitQueue->top())->m_fScore; 
					}
				}
			}
			
		}

		//////////////////////////////////////////////////////////////////////////
		//CSortedTopDocsCollector
		CSortedTopDocsCollector::CSortedTopDocsCollector(CFieldDocSortedHitQueue* pHitQueue,count_t* totalHits,count_t nDoc,const CBitVector* pFilterBits)
			:m_pHitQueue(pHitQueue)
			,m_pTotalHits(totalHits)
			,m_maxDocs(nDoc)
			,m_pFilterBits(pFilterBits)
		{
		}
		CSortedTopDocsCollector::~CSortedTopDocsCollector()
		{			
		}

		void CSortedTopDocsCollector::collect(docid_t docid,float score)
		{
			if (score > 0.0f && (m_pFilterBits == NULL || m_pFilterBits->test(docid)) ) 
			{	  
				m_pTotalHits[0]++;
				CScoreDoc* sd = new CScoreDoc(docid, score);
				if(!m_pHitQueue->insert(sd))
					delete sd;
			}
		}

		void CSortedTopDocsCollector::collect(docid_t docid[],score_t score[],count_t size)
		{
			for (int i = 0; i < size; i++)
			{
				if (score [i]> 0.0f && (m_pFilterBits == NULL || m_pFilterBits->test(docid[i])) ) 
				{	  
					m_pTotalHits[0]++;
					CScoreDoc* sd = new CScoreDoc(docid[i], score[i]);
					if(!m_pHitQueue->insert(sd))
						delete sd;					
				}
			}
		}
	}
}
