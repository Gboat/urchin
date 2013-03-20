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
// Created	: 2006/5/9
//
#include "search/BooleanScorer.h"
#include "search/Similarity.h"
#include "utility/StdHeader.h"
#include "search/DisjunctionScorer.h"
#include "search/ConjunctionScorer.h"
#include "search/ReqOptScorer.h"
#include "search/ReqProhScorer.h"

namespace firtex
{
	namespace search
	{
		CBooleanScorer::CBooleanScorer(CSimilarity* pSimilarity,int minNrShouldMatch/*=1*/)
			:CScorer(pSimilarity)
			,m_minNrShouldMatch(minNrShouldMatch)
			,m_pScorer(NULL)
		{
			if(minNrShouldMatch < 0)
				throw new CIllegalArgumentException("Minimum number of optional scorers should not be negative");
		}
		CBooleanScorer::~CBooleanScorer(void)
		{
			if(m_pScorer)
			{
				delete m_pScorer;
				m_pScorer = NULL;
			}
			ScorerList::iterator iter = m_requiredScorers.begin();
			while (iter != m_requiredScorers.end())
			{
				delete (*iter);
				iter++;
			}
			iter = m_optionalScorers.begin();
			while (iter != m_optionalScorers.end())
			{
				delete (*iter);
				iter++;
			}
			iter = m_prohibitedScorers.begin();
			while (iter != m_prohibitedScorers.end())
			{
				delete (*iter);
				iter++;
			}
			m_requiredScorers.clear();
			m_prohibitedScorers.clear();
			m_optionalScorers.clear();
		}

		void CBooleanScorer::add(CScorer* pScorer,bool bRequired,bool bProhibited)
		{
			if (bRequired) 
			{
				if (bProhibited) 
				{
					throw new CIllegalArgumentException("scorer cannot be required and prohibited");
				}
				m_requiredScorers.push_back(pScorer);
			}
			else if (bProhibited) 
			{
				m_prohibitedScorers.push_back(pScorer);
			}
			else 
			{
				m_optionalScorers.push_back(pScorer);
			}
		}
		count_t CBooleanScorer::nextDocs()
		{
			if(m_pScorer == NULL)
			{
				m_pScorer = makeScorer();
			}
			if(m_pScorer == NULL)
				return 0;
			return m_pScorer->nextDocs();			
		}
		count_t	CBooleanScorer::scores(docid_t*& docs,score_t*& scores)
		{			
			return m_pScorer->scores(docs,scores);
		}

		bool CBooleanScorer::next()
		{
			if(m_pScorer == NULL)
			{
				m_pScorer = makeScorer();
			}
			if(m_pScorer == NULL)
				return false;
			return m_pScorer->next();
		}
		bool CBooleanScorer::skipTo(docid_t target,docid_t& nearTarget)
		{
			if(m_pScorer == NULL)
			{
				m_pScorer = makeScorer();
			}
			if(m_pScorer == NULL)
				return false;
			return m_pScorer->skipTo(target,nearTarget);
		}
		docid_t	CBooleanScorer::doc()
		{
			return m_pScorer->doc();
		}
		score_t	CBooleanScorer::score()
		{
			return m_pScorer->score();
		}

		CScorer* CBooleanScorer::makeScorer()
		{
			return (m_requiredScorers.size() == 0)
				? makeScorerNoReq()
				: makeScorerSomeReq();
		}
		CScorer* CBooleanScorer::makeScorerNoReq()
		{			
			if (m_optionalScorers.size() == 0)
			{
				return NULL;				
			} 
			else
			{ 
				int nrOptRequired = (m_minNrShouldMatch < 1) ? 1 :m_minNrShouldMatch;
				if ((int)m_optionalScorers.size() < nrOptRequired)
				{
					return NULL;
				} 
				else 
				{
					CScorer* requiredScorer;
					if((int)m_optionalScorers.size() > nrOptRequired)
					{
						 requiredScorer = makeDisjunctionScorer(&m_optionalScorers,nrOptRequired);
					}
					else 
					{
						if(m_optionalScorers.size() == 1)
						{
							requiredScorer = new CSingleMatchScorer(m_optionalScorers.front());
							m_optionalScorers.pop_front();
						}
						else requiredScorer = makeConjunctionScorer(&m_optionalScorers) ;
					}				
					return addProhibitedScorers(requiredScorer);
				}
			}
		}
		CScorer* CBooleanScorer::makeScorerSomeReq()
		{			
			if ((int)m_optionalScorers.size() < m_minNrShouldMatch)
			{
				return NULL; 
			}
			else if (m_optionalScorers.size() == m_minNrShouldMatch) 
			{
				CConjunctionScorer* pConScorer = (CConjunctionScorer*)makeConjunctionScorer(&m_requiredScorers);				
				return addProhibitedScorers(pConScorer);
			} 
			else // m_optionalScorers.size() > m_minNrShouldMatch, and at least one required scorer
			{ 
				CScorer* requiredScorer;
				if(m_requiredScorers.size() == 1)
				{
					requiredScorer = new CSingleMatchScorer(m_requiredScorers.front());
					m_requiredScorers.pop_front();
				}
				else 
				{
					requiredScorer = makeConjunctionScorer(&m_requiredScorers);
				}				
				if (m_minNrShouldMatch > 0)
				{	
					return addProhibitedScorers(dualConjunctionScorer(requiredScorer,
						makeDisjunctionScorer(&m_optionalScorers,m_minNrShouldMatch)));
				} 
				else // m_minNrShouldMatch == 0 
				{	
					CScorer* p ;
					if(m_optionalScorers.size() == 1) 
					{
						p = new CSingleMatchScorer(m_optionalScorers.front());
						m_optionalScorers.pop_front();
					}
					else 
						p =  makeDisjunctionScorer(&m_optionalScorers,1);
					return new CReqOptScorer(addProhibitedScorers(requiredScorer),p);
				}
			}
		}
		CScorer* CBooleanScorer::makeConjunctionScorer(ScorerList* scorerList)
		{
			CConjunctionScorer* pScorer = new CConjunctionScorer(scorerList,getSimilarity());
			scorerList->clear();
			return pScorer;
		}
		CScorer* CBooleanScorer::makeDisjunctionScorer(ScorerList* scorerList,int minNrShouldMatch)
		{
			CDisjunctionScorer* pScorer = new CDisjunctionScorer(scorerList,minNrShouldMatch);
			scorerList->clear();
			return pScorer;
		}
		CScorer* CBooleanScorer::addProhibitedScorers(CScorer* requiredScorer)
		{
			if((m_prohibitedScorers.size() == 0))
				return requiredScorer;
			else 
			{
				CScorer* p ;
				if(m_prohibitedScorers.size() == 1)
				{
					p = m_prohibitedScorers.front();
					m_prohibitedScorers.pop_front();					
				}
				else 
				{
					p = new CDisjunctionScorer(&m_prohibitedScorers);
					m_prohibitedScorers.clear();
				}
				return new CReqProhScorer(requiredScorer,p);
			}		
		}
		CScorer* CBooleanScorer::dualConjunctionScorer(CScorer* req1, CScorer* req2)
		{ 
			size_t requiredNrMatchers = m_requiredScorers.size();
			CConjunctionScorer* cs = new CConjunctionScorer(getSimilarity());		
			cs->add(req1);
			cs->add(req2);
			return cs;
		}
	}
}

