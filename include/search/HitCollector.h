//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: 郭瑞杰(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/3/23
//
#ifndef _HITCOLLECTOR_H
#define _HITCOLLECTOR_H
#include "../utility/typedefs.h"
#include "ScoreDoc.h"
#include "../utility/BitVector.h"
#include "../utility/PriorityQueue.h"
#include "HitQueue.h"
using namespace firtex::utility;
using namespace std;
using namespace firtex;

namespace firtex
{
	namespace search
	{		
		class CHitCollector
		{
		public:
			CHitCollector(void)
			{
			}

			virtual ~CHitCollector(void)
			{
			}
		public:
			virtual void collect(docid_t docid,score_t score)=0;
			virtual void collect(docid_t docid[],score_t score[],count_t size)=0;
		};

		struct ScoreDocLess
		{
			bool operator () (const CScoreDoc*& left,const CScoreDoc*& right) const
			{
				if (left->m_fScore == right->m_fScore)
					return left->m_docID < right->m_docID; 
				else
					return left->m_fScore < right->m_fScore;				
			}
		};
				
		//typedef priority_queue<CScoreDoc*,deque<CScoreDoc*>,ScoreDocLess> CHitQueue;

		class CSimpleTopDocsCollector : public CHitCollector
		{
		public:
			CSimpleTopDocsCollector(CHitQueue* pHitQueue,count_t* totalHits,count_t nDoc,const CBitVector* pFilterBits = NULL,score_t ms=-1.0f);
			~CSimpleTopDocsCollector();

		public:
			void collect(docid_t docid,score_t score);
			void collect(docid_t docid[],score_t score[],count_t size);
		public:
			count_t*			m_pTotalHits;
			count_t				m_maxDocs;			//返回最大文档数
			CHitQueue*			m_pHitQueue;
			const CBitVector*	m_pFilterBits;

			score_t				m_minScore;			
		};

		//////////////////////////////////////////////////////////////////////////
		//CSortedTopDocsCollector
		class CSortedTopDocsCollector : public CHitCollector
		{
		public:
			CSortedTopDocsCollector(CFieldDocSortedHitQueue* pHitQueue,count_t* totalHits,count_t nDoc,const CBitVector* pFilterBits = NULL);
			~CSortedTopDocsCollector();

		public:
			void collect(docid_t docid,score_t score);
			void collect(docid_t docid[],score_t score[],count_t size);
		public:
			count_t*					m_pTotalHits;
			count_t						m_maxDocs;			//返回最大文档数
			CFieldDocSortedHitQueue*	m_pHitQueue;
			const CBitVector*			m_pFilterBits;
		};
	}
}

#endif
