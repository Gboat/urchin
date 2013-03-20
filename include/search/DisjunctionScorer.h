//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: �����(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/5/9
//
#ifndef _DISJUNCTIONSCORER_H
#define _DISJUNCTIONSCORER_H

#include <list>
#include "../utility/PriorityQueue.h"
#include "Scorer.h"
using namespace std;
using namespace firtex::utility;

namespace firtex
{
	namespace search
	{
		class CDisjunctionScorer :	public CScorer
		{	
#define DISJUNCTION_PAGESIZE		1024

			class CScorerQueue : public CPriorityQueue<CScorer*>
			{
			public:
				CScorerQueue(size_t size) 
				{
					initialize(size,false);
				}
			protected:
				bool lessThan(CScorer* o1, CScorer* o2) 
				{
					return o1->doc() < o2->doc();
				}
			};

		public:
			CDisjunctionScorer();
			CDisjunctionScorer(list<CScorer*>* scorers,int minShouldMatch=1);
			virtual ~CDisjunctionScorer(void);
		public:
			/**
			 * ��ȡ��һ���ĵ�
			 * @return ��ȡ�ɹ�����true���Ѿ���ȡ��Ϸ���false
			 */
			count_t	nextDocs();

			/**
			 * ȡ�ĵ��Ĵ�ֽ��
			 * @param docs ����ĵ�id��docs��ַ���ڲ�ά��
			 * @param scores ����ĵ���ֽ����scores��ַ���ڲ�ά��			 
			 * @return ��ֵ��ĵ�����
			 */
			count_t	scores(docid_t*& docs,score_t*& scores);

			/** 
			 * �ƶ�����һ���ĵ���Ŵ��ڻ����target��λ�ã�һ�����{@link score()}ʹ��
			 * @param target Ŀ���ĵ����
			 * @param nearTarget �������ɹ�ʱnearTarget=target,���򣬷��ش���target������target��ӽ���doc id,
			 *             ���target��������ֵ�򷵻�С��target����target��ӽ���doc id
			 * @return ���������������ĵ����ʱ����true
			 */ 
			bool	skipTo(docid_t target,docid_t& nearTarget);

			/**
			 * ����ȡһ���ĵ����д��
			 * @return ����false���������β
			 */
			bool	next();

			/*
			 * ���ص�ǰλ���ĵ��Ĵ�ֽ�����������{@link skipTo(docid_t,DOC_ID)}��{@link next()}ʹ�ã�ֻ�з���true�����Ч
			 */
			score_t score();

			/*
			 * ���ص�ǰλ���ĵ�ID���������{@link skipTo(docid_t,DOC_ID)}��{@link next()}ʹ�ã�ֻ�з���true�����Ч
			 */
			docid_t doc();
	
			/*
			 * add one scorer
			 * @param pScorer The scorer witch will be added to the m_scoresList
			 */
			void	add(CScorer* pScorer){m_scoresList.push_back(pScorer);}
		protected:
			void	initScorerQueue();
			bool	doNext(docid_t& currentDoc,score_t& currentScore);
		protected:
			list<CScorer*>	m_scoresList;
			CScorerQueue*	m_pScorerQueue;

			int			m_minShouldMatch;				//ֻҪ��Ҫƥ��ĸ���

			docid_t		m_docs[DISJUNCTION_PAGESIZE];	//�ĵ�ID���ڴ���CTermDocFreqsά��			
			score_t		m_scores[DISJUNCTION_PAGESIZE];	//��ֽ��
			count_t		m_numDocs;						//�ĵ���	
		};
		//////////////////////////////////////////////////////////////////////////
		//
		inline bool CDisjunctionScorer::doNext(docid_t& currentDoc,score_t& currentScore)
		{			
			CScorer* top;			
			int32_t nrMatchers;	
			do
			{	// repeat until minimum nr of matchers
				top = m_pScorerQueue->top();
				currentDoc = top->doc();
				currentScore = top->score();
				nrMatchers = 1;
				do
				{	// Until all subscorers are after currentDoc
					if (top->next()) 
					{
						m_pScorerQueue->adjustTop();
					}
					else 
					{
						m_pScorerQueue->pop();
						if ((int32_t)m_pScorerQueue->size() < (m_minShouldMatch - nrMatchers)) 
						{
							// Not enough subscorers left for a match on this document,
							// and also no more chance of any further match.
							return false;
						}
						if (m_pScorerQueue->size() == 0)
						{
							break; // nothing more to advance, check for last match.
						}
					}
					top = m_pScorerQueue->top();
					if (top->doc() != currentDoc) 
					{
						break; // All remaining subscorers are after currentDoc.
					}
					else
					{
						currentScore += top->score();
						nrMatchers++;
					}
				} while (true);

				if (nrMatchers >= m_minShouldMatch)
				{
					return true;
				}
				else if ((int32_t)m_pScorerQueue->size() < m_minShouldMatch) 
				{
					return false;
				}
			} while (true);						
		}
	}
}

#endif

