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
#include "search/DisjunctionScorer.h"

namespace firtex
{
	namespace search
	{
		CDisjunctionScorer::CDisjunctionScorer()
			:m_numDocs(0),m_minShouldMatch(1)
		{
		}
		CDisjunctionScorer::CDisjunctionScorer(list<CScorer*>* scorers,int minShouldMatch)
			:m_numDocs(0),m_minShouldMatch(minShouldMatch)
		{
			m_scoresList.assign(scorers->begin(),scorers->end());
			m_pScorerQueue = NULL;
		}
		CDisjunctionScorer::~CDisjunctionScorer(void)
		{
			while(!m_scoresList.empty())
			{
				delete m_scoresList.front();
				m_scoresList.pop_front();
			}			
			if(m_pScorerQueue)
			{				
				delete m_pScorerQueue;
				m_pScorerQueue = NULL;
			}
			
			
		}
		void CDisjunctionScorer::initScorerQueue()
		{
			m_pScorerQueue = new CScorerQueue(m_scoresList.size());
			CScorer* pScorer;			
			list<CScorer*>::iterator iter = m_scoresList.begin();
			while (iter != m_scoresList.end())
			{
				pScorer = (*iter);
				if(pScorer->next())				
					m_pScorerQueue->insert(pScorer);
				iter ++;
			}
		}
			
		count_t CDisjunctionScorer::nextDocs()
		{			
			if(m_pScorerQueue == NULL)
				initScorerQueue();
			if ((int)m_pScorerQueue->size() < m_minShouldMatch) 
			{
				return false;
			}			 
		
			count_t numDocs = 0;
			CScorer* top;
			docid_t currentDoc;	
			score_t currentScore;									
			m_numDocs = 0;
			int nMatch = 1;		
			do 
			{	// repeat until minimum nr of matchers
				top = m_pScorerQueue->top();
				currentDoc=top->doc();	
				currentScore = top->score();										
				do 
				{ // Until all subscorers are after currentDoc
					if (top->next())//前进到下一个 
					{
						m_pScorerQueue->adjustTop();
					}
					else //到达结尾
					{
						m_pScorerQueue->pop();
						if ((int)m_pScorerQueue->size() < (m_minShouldMatch - nMatch)) 
						{
							//剩下的subscorer不够匹配这个文档														
							break;
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
						nMatch++;
					}
				} while (true);

				if (nMatch >= m_minShouldMatch)
				{
					m_docs[numDocs] = currentDoc;
					m_scores[numDocs] = currentScore;
					numDocs++;
					nMatch = 1;
					currentScore = 0;
				} 
				if ((int)m_pScorerQueue->size() < m_minShouldMatch) 
				{
					break;
				}
			} while (numDocs < DISJUNCTION_PAGESIZE);
			m_numDocs = numDocs;
			return numDocs;			
		}

		count_t CDisjunctionScorer::scores(docid_t*& docs,score_t*& scores)
		{
			docs = m_docs;
			scores = m_scores;
			return m_numDocs;
		}
		bool CDisjunctionScorer::next()
		{
			if(m_pScorerQueue == NULL)
				initScorerQueue();
			if ((int)m_pScorerQueue->size() < m_minShouldMatch) 
			{
				return false;
			}

			count_t numDocs = 0;
			CScorer* top;
			docid_t currentDoc;	
			score_t currentScore;									
			m_numDocs = 0;
			int nMatch = 1;		
			do 
			{	// repeat until minimum nr of matchers
				top = m_pScorerQueue->top();
				currentDoc=top->doc();	
				currentScore = top->score();										
				do 
				{ // Until all subscorers are after currentDoc
					if (top->next())//前进到下一个 
					{
						m_pScorerQueue->adjustTop();
					}
					else //到达结尾
					{
						m_pScorerQueue->pop();
						if ((int)m_pScorerQueue->size() < (m_minShouldMatch - nMatch)) 
						{
							return false;
						}
						if (m_pScorerQueue->size() == 0)
						{
							break; 
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
						nMatch++;
					}
				} while (true);

				if (nMatch >= m_minShouldMatch)
				{				
					m_docs[0] = currentDoc;
					m_scores[0] = currentScore;
					m_numDocs = 1;
					return true;
				} 
				if ((int)m_pScorerQueue->size() < m_minShouldMatch) 
				{
					break;
				}
			} while (true);
			m_numDocs = numDocs;
			return (numDocs==1);			
		}

		bool CDisjunctionScorer::skipTo(docid_t target,docid_t& nearTarget)
		{
			if(m_pScorerQueue == NULL)
				initScorerQueue();
			nearTarget = target - 1;
			if ((int)m_pScorerQueue->size() < m_minShouldMatch) 
			{
				return false;
			}			 			
			docid_t currentDoc = target;
			score_t currentScore = 0;		
			int32_t numDocs = -1;
			do 
			{
				CScorer* top = (CScorer*) m_pScorerQueue->top();
				if (top->doc() >= currentDoc) 
				{
					if(doNext(nearTarget,currentScore) == false)
					{
						nearTarget = -1;
						return false;
					}
					else 
					{
						m_numDocs = 1;
						m_docs[0] = nearTarget;
						m_scores[0] = currentScore;
						return (currentDoc == nearTarget);
					}
				}
				else if (top->skipTo(currentDoc,nearTarget))
				{
					m_pScorerQueue->adjustTop();
				} 
				else 
				{
					if(nearTarget < currentDoc)
					{
						m_pScorerQueue->pop();
						if ((int32_t)m_pScorerQueue->size() < m_minShouldMatch) 
						{
							return false;
						}
					}
					else m_pScorerQueue->adjustTop();
				}
			} while (true);							
		}
		score_t CDisjunctionScorer::score()
		{
			return m_scores[0];			
		}
		docid_t CDisjunctionScorer::doc()
		{
			return m_docs[0];
		}
	}
}
