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
#include "search/ConjunctionScorer.h"

namespace firtex
{
	namespace search
	{
		CConjunctionScorer::CConjunctionScorer(CSimilarity* pSimilarity):CScorer(pSimilarity)
		{
		}
		CConjunctionScorer::CConjunctionScorer(list<CScorer*>* scorers,CSimilarity* pSimilarity):CScorer(pSimilarity),m_numDocs(0)
		{
			m_scoresList.assign(scorers->begin(),scorers->end());
		}
		CConjunctionScorer::~CConjunctionScorer(void)
		{
			while(!m_scoresList.empty())
			{
				delete m_scoresList.front();
				m_scoresList.pop_front();
			}
		}
	
		count_t	CConjunctionScorer::nextDocs()
		{
			count_t numDocs = 0;
			docid_t target,nearTarget;
			score_t s=0;
			int nScorerNum = (int)m_scoresList.size();
			ScorerList::iterator iter;
			bool bNext = true;			
			m_numDocs = 0;
			int nMatch = 0;					
			while (numDocs < CONJUNCTION_PAGESIZE) 
			{		
				if(bNext)
				{
					CScorer* pS = m_scoresList.front();
					if(pS->next()==false)
						break;					
					target = pS->doc();
					m_scoresList.push_back(pS);
					m_scoresList.pop_front();
					nearTarget = target - 1;
					nMatch = 1;
					bNext = false;
				}											
				while ( nMatch < nScorerNum) 
				{					
					if(m_scoresList.front()->skipTo(target,nearTarget))//找到
						nMatch++;
					else
					{
						nMatch = 1;
						if(nearTarget>target)
						{
							target = nearTarget;
							nearTarget++;
						}
						else break;//到达末尾，匹配失败
					}
					//firstToLast;
					m_scoresList.push_back(m_scoresList.front());
					m_scoresList.pop_front();					
				}
				if(nMatch == nScorerNum)//找到一个匹配的文档编号
				{				
					m_docs[numDocs] = target;
					s = 0;
					//统计Score
					iter = m_scoresList.begin();
					while (iter != m_scoresList.end())
					{
						s += (*iter)->score();
						iter++;
					}
					m_scores[numDocs] = s;
					numDocs++;			
					bNext = true;
				}
				else 
				{
					break;
				}
			}			
			m_numDocs = numDocs;
			return numDocs;
		}
		
		count_t	CConjunctionScorer::scores(docid_t*& docs,score_t*& scores)
		{			
			docs = m_docs;
			scores = m_scores;
			return m_numDocs;
		}

		bool CConjunctionScorer::next()
		{			
			docid_t target,nearTarget;		
			int nScorerNum = (int)m_scoresList.size();
			ScorerList::iterator iter;						

			if(m_scoresList.front()->next()==false)
				return false;
			target = m_scoresList.front()->doc();
			m_scoresList.push_back(m_scoresList.front());
			m_scoresList.pop_front();
			nearTarget = target +1;
			int nMatch = 1;
			
			while ( nMatch < nScorerNum) 
			{					
				if(m_scoresList.front()->skipTo(target,nearTarget))//找到
					nMatch++;
				else
				{
					nMatch = 1;
					if(nearTarget>target)
					{
						target = nearTarget;
						nearTarget++;
					}
					else break;//到达末尾，匹配失败
				}
				//firstToLast;
				m_scoresList.push_back(m_scoresList.front());
				m_scoresList.pop_front();					
			}
			if(nMatch == nScorerNum)//找到一个匹配的文档编号
			{				
				m_docs[0] = target;						
				m_numDocs = 1;
				return true;
			}			
			return false;
		}		
		bool CConjunctionScorer::skipTo(docid_t target,docid_t& nearTarget)
		{			
			m_numDocs = 0;
			docid_t t = target;
			nearTarget = t - 1;
			int nMatch = 0;	
			int nScorer = (int)m_scoresList.size();
			while ( nMatch < nScorer)
			{					
				if(m_scoresList.front()->skipTo(t,nearTarget))//找到
					nMatch++;
				else
				{
					nMatch = 1;
					if(nearTarget > t)
					{
						t = nearTarget;
						nearTarget++;
					}
					else break;//m_pFirst到达末尾，匹配失败
				}
				//firstToLast;
				m_scoresList.push_back(m_scoresList.front());
				m_scoresList.pop_front();
			}
			if(nMatch == nScorer)//找到一个匹配的文档编号
			{				
				m_docs[0] = t;				
				m_numDocs = 1;				
				nearTarget = t;
			}
			return ( target == nearTarget);
		}
		
		score_t CConjunctionScorer::score()
		{
			score_t s = 0;
			//统计Score
			ScorerList::iterator iter = m_scoresList.begin();
			while (iter != m_scoresList.end())
			{
				s += (*iter)->score();
				iter++;
			}
			return s;
		}
		
		docid_t CConjunctionScorer::doc()
		{
			return m_docs[0];
		}
	}
}
