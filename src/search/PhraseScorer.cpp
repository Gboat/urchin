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
// Created	: 2006/3/17
//
#include "search/PhraseScorer.h"
#include "search/Similarity.h"
#include "search/Weight.h"

namespace firtex
{
	namespace search
	{
		CPhraseScorer::CPhraseScorer(CWeight* pWeight,CSimilarity* pSimilarity,int32_t slop,CTermPositions** ppPos,int32_t size,CNormsBytes* norms):
		CScorer(pSimilarity),m_pWeight(pWeight),m_nSlop(slop),m_numDocs(0),m_norms(norms)
		{
			m_fWeights = m_pWeight->getValue();
			CPhraseQueue pq;
			for(int32_t i = 0;i < size;i++)
			{				
				pq.push(new CPhrasePositions(ppPos[i],i));
			}
			m_nNumPositions = (int32_t)pq.size();
			m_pFirst = pq.top();
			pq.pop();
			CPhrasePositions* pCur = m_pFirst;
			while (!pq.empty())
			{
				pCur->m_pNext = pq.top();
				pCur = pCur->m_pNext;
				pq.pop();
			}
			m_pLast = pCur;			

			for (int32_t i = 0; i < PHRASESCORE_CACHESIZE; i++)
				m_scoreCache[i] = pSimilarity->tf(i) * m_fWeights;
		}

		CPhraseScorer::~CPhraseScorer(void)
		{
			if(m_pFirst)
			{
				delete m_pFirst;
				m_pFirst = NULL;
				m_pLast = NULL;
			}
			if(m_pWeight)
			{
				delete m_pWeight;
				m_pWeight = NULL;
			}
			m_norms = NULL;
		}
		
		count_t	CPhraseScorer::nextDocs()
		{
			m_numDocs = 0;
			count_t numDocs = 0;
			docid_t target,nearTarget;
			freq_t freq = 0;
			int32_t nMatch;			
			bool bNext = true;

			while (numDocs < PHRASE_PAGESIZE) 
			{				
				if(bNext)
				{
					if(m_pFirst->next() == false)
						break;
					target = m_pFirst->doc();											
					firstToLast();
					nearTarget = target + 1;
					nMatch = 1;
					bNext = false;
				}
				while ( nMatch < m_nNumPositions) 
				{					
					if(m_pFirst->skipTo(target,nearTarget))//ÕÒµ½
						nMatch++;
					else
					{
						nMatch = 1;
						if(nearTarget>target)
						{							
							target = nearTarget;							
							nearTarget++;														
						}
						else break;//m_pFirstµ½´ïÄ©Î²£¬Æ¥ÅäÊ§°Ü
					}
					firstToLast();
				}
				if(nMatch == m_nNumPositions)//ÕÒµ½Ò»¸öÆ¥ÅäµÄÎÄµµ±àºÅ
				{
					freq = phraseFreq();
					if(freq > 0)
					{
						m_docs[numDocs] = target;
						m_freqs[numDocs] = freq;
						numDocs++;						
					}					
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
		
		count_t	CPhraseScorer::scores(docid_t*& docs,score_t*& scores)
		{
			freq_t f;			
			score_t* ps = m_scores;
			score_t s;
			for (int32_t i = 0;i < m_numDocs;i++)
			{
				f = m_freqs[i];
				s = (f < PHRASESCORE_CACHESIZE)?m_scoreCache[f]:(getSimilarity()->tf(f)*m_fWeights);
				ps[i] = (score_t)s * getSimilarity()->decodeNorm(m_norms->getNorm(m_docs[i]));
			}				
			docs = m_docs;
			scores = ps;
			return m_numDocs;
		}
		bool CPhraseScorer::next()
		{
			if(!m_pFirst->next())
				return false;
			docid_t target = m_pFirst->doc();
			docid_t nearTarget = target + 1;
			freq_t freq = 0;
			m_numDocs = 0;
			firstToLast();
			int32_t nMatch = 1;				
			do 
			{
				while ( nMatch < m_nNumPositions)
				{					
					if(m_pFirst->skipTo(target,nearTarget))//ÕÒµ½
						nMatch++;
					else
					{
						nMatch = 1;
						if(nearTarget > target)
						{
							target = nearTarget;
							nearTarget++;
						}
						else
						{
							return false;//m_pFirstµ½´ïÄ©Î²£¬Æ¥ÅäÊ§°Ü							
						}
					}
					firstToLast();
				}
				
				freq = phraseFreq();
				if(freq > 0)
				{
					m_docs[0] = target;
					m_freqs[0] = freq;
					m_numDocs = 1;
					return true;
				}
				else 
				{						
					if(!m_pFirst->next())
						return false;
					target = m_pFirst->doc();
					firstToLast();
					nMatch = 1;
				}				
			} while(true);			
			return false;
		}
		bool CPhraseScorer::skipTo(docid_t target,docid_t& nearTarget)
		{
			freq_t freq = 0;
			m_numDocs = 0;
			docid_t t = target;
			nearTarget = t - 1;
			int32_t nMatch = 0;	

			if(m_numDocs==1 && m_docs[0] == target)
			{
				nearTarget = target;
				return true;
			}

			while ( nMatch < m_nNumPositions ) 
			{					
				if(m_pFirst->skipTo(t,nearTarget))//ÕÒµ½
					nMatch++;
				else
				{
					nMatch = 1;
					if(nearTarget > t)
					{
						t = nearTarget;							
						//nearTarget++;
					}
					else 
					{
						nearTarget = target - 1;
						return false;//m_pFirstµ½´ïÄ©Î²£¬Æ¥ÅäÊ§°Ü
					}
				}
				firstToLast();
			}
			//if(nMatch == m_nNumPositions)//ÕÒµ½Ò»¸öÆ¥ÅäµÄÎÄµµ±àºÅ
			{
				freq = phraseFreq();
				if(freq > 0)
				{
					m_docs[0] = t;
					m_freqs[0] = freq;
					m_numDocs = 1;
					nearTarget = t;
					return (t == target);
					//return true;
				}
				else 
				{
					if(next())
					{
						nearTarget = m_docs[0];
						return false;
					}
					else 
					{
						nearTarget = target - 1;
						return false;
					}					
				}
			}					
			nearTarget = target - 1;
			return false;			
		}
		score_t CPhraseScorer::score()
		{
			freq_t f = m_freqs[0];			 
			float raw = ( (f < PHRASE_PAGESIZE)?m_scoreCache[f]:(getSimilarity()->tf(f)*m_fWeights) );			
			return (score_t)raw * getSimilarity()->decodeNorm(m_norms->getNorm(m_docs[0]));
		}
		docid_t CPhraseScorer::doc()
		{
			return m_docs[0];
		}		
	}
}
