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
			 * 提取下一批文档
			 * @return 提取成功返回true，已经提取完毕返回false
			 */
			count_t	nextDocs();

			/**
			 * 取文档的打分结果
			 * @param docs 存放文档id，docs地址由内部维护
			 * @param scores 存放文档打分结果，scores地址由内部维护			 
			 * @return 打分的文档总数
			 */
			count_t	scores(docid_t*& docs,score_t*& scores);

			/** 
			 * 移动到第一个文档编号大于或等于target的位置，一般配合{@link score()}使用
			 * @param target 目标文档编号
			 * @param nearTarget 当搜索成功时nearTarget=target,否则，返回大于target并且与target最接近的doc id,
			 *             如果target大于所有值则返回小于target且与target最接近的doc id
			 * @return 仅当存在这样的文档编号时返回true
			 */ 
			bool	skipTo(docid_t target,docid_t& nearTarget);

			/**
			 * 依次取一个文档进行打分
			 * @return 返回false表明到达结尾
			 */
			bool	next();

			/*
			 * 返回当前位置文档的打分结果，必须配合{@link skipTo(docid_t,DOC_ID)}或{@link next()}使用，只有返回true后才有效
			 */
			score_t score();

			/*
			 * 返回当前位置文档ID，必须配合{@link skipTo(docid_t,DOC_ID)}或{@link next()}使用，只有返回true后才有效
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

			int			m_minShouldMatch;				//只要需要匹配的个数

			docid_t		m_docs[DISJUNCTION_PAGESIZE];	//文档ID，内存由CTermDocFreqs维护			
			score_t		m_scores[DISJUNCTION_PAGESIZE];	//打分结果
			count_t		m_numDocs;						//文档数	
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

