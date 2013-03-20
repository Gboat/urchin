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
#include "utility//StdHeader.h"

#include "search/BooleanQuery.h"
#include "search/BooleanClause.h"
#include "search/BooleanScorer.h"

namespace firtex
{
	namespace search
	{
		//////////////////////////////////////////////////////////////////////////
		//CBooleanWeight	
		CBooleanQuery::CBooleanWeight::CBooleanWeight(CSearcher* pSearcher,CBooleanQuery* pQuery)
			:m_pSearcher(pSearcher)
			,CWeight(pQuery,false)
		{
			m_fWeight = 0.0f;
			CBooleanClause* c;
			ClauseIterator iter = pQuery->m_clauses.begin();
			while (iter != pQuery->m_clauses.end())
			{
				c = (*iter);
				if (!c->isProhibited())
				{
					m_weights.push_back( (*iter)->getQuery()->createWeight(pSearcher) );
				}				
				iter++;
			}			
		}
		CBooleanQuery::CBooleanWeight::~CBooleanWeight()
		{
			WeightIterator iter = m_weights.begin();
			while (iter != m_weights.end())
			{
				delete (*iter);
				iter++;
			}
			m_weights.clear();
		}	
		
		CQuery* CBooleanQuery::optimize(firtex::index::CIndexReader* pReader)
		{
			if (m_clauses.size() == 1)
			{ 
				CBooleanClause* c = m_clauses[0];
				if (!c->isProhibited())
				{
					CQuery* query = c->getQuery()->optimize(pReader); 					
					if ( query == c->getQuery() )
						query = query->clone();

					if (getBoost() != 1.0f) 
					{                 // incorporate boost
						query->setBoost(getBoost() * query->getBoost());
					}

					return query;
				}
			}

			CBooleanQuery* clone = NULL; 
			for (uint32_t i = 0 ; i < m_clauses.size(); i++) 
			{
				CBooleanClause* c = m_clauses[i];
				CQuery* query = c->getQuery()->optimize(pReader);
				if (query != c->getQuery()) 
				{
					if (clone == NULL)
						clone = (CBooleanQuery*)this->clone();

					delete clone->m_clauses[i];
					clone->m_clauses[i] = new CBooleanClause(query,c->isRequired(), c->isProhibited());
					delete query;
				}
			}
			if (clone != NULL) 
			{
				return clone;                               
			} 
			else
				return this;                                
		}		
		
		weight_t CBooleanQuery::CBooleanWeight::getValue()
		{
			if(m_fWeight == 0.0f)
			{
				weight_t sum = 0.0f;
				WeightIterator iter = m_weights.begin();
				CWeight* w;
				while (iter != m_weights.end())
				{
					w = (*iter);					
					sum += w->getValue();
					iter++;				
				}			
				m_fWeight = sum;
			}
			return m_fWeight;
		}

		//////////////////////////////////////////////////////////////////////////
		//CBooleanQuery
		int32_t CBooleanQuery::MAX_CLAUSECOUNT = 1024;
		CBooleanQuery::CBooleanQuery(void):m_nMinShouldMatch(0)
		{
		}
		CBooleanQuery::CBooleanQuery(const CBooleanQuery& clone):m_nMinShouldMatch(0)
		{
			ClauseIterator iter = clone.m_clauses.begin();
			while (iter != clone.m_clauses.end())
			{
				add((*iter)->clone());
				iter++;
			}
		}

		CBooleanQuery::~CBooleanQuery(void)
		{
			clear();
		}
		CQuery* CBooleanQuery::clone()
		{
			return new CBooleanQuery(*this);
		}

		int32_t CBooleanQuery::getMaxClauseCount()
		{
			return MAX_CLAUSECOUNT;
		}
		void CBooleanQuery::setMaxClauseCount(int32_t maxCount)
		{
			MAX_CLAUSECOUNT = maxCount;
		}
		CWeight* CBooleanQuery::createWeight(CSearcher* pSearcher)
		{
			return new CBooleanWeight(pSearcher,this);

		}
		CScorer* CBooleanQuery::scorer(CSearcher* pSearcher)
		{
			CBooleanScorer* pScorer = new CBooleanScorer(pSearcher->getSimilarity(),m_nMinShouldMatch);
			ClauseIterator iter = m_clauses.begin();
			CBooleanClause* c;
			CScorer* pSubScorer;
			while (iter != m_clauses.end())
			{
				c = (*iter);
				pSubScorer = c->getQuery()->scorer(pSearcher);
				if(pSubScorer)
					pScorer->add(pSubScorer,c->isRequired(),c->isProhibited());
				else 
				{
					if(c->isRequired())
					{
						delete pScorer;
						pScorer = NULL;
						break;
					}
				}

				iter++;
			}
			return pScorer;
		}	
		CScorer* CBooleanQuery::scorer(CSearcher* pSearcher,int32_t* buffer,size_t bufSize,size_t* pBufUsed)
		{
			CBooleanScorer* pScorer = new CBooleanScorer(pSearcher->getSimilarity(),m_nMinShouldMatch);
			ClauseIterator iter = m_clauses.begin();
			CBooleanClause* c;
			CScorer* pSubScorer;
			int32_t* pSubBuffer = buffer;
			size_t nSubBufferSize = bufSize/m_clauses.size();
			size_t nCurBufferSize = nSubBufferSize;
			size_t nSubBufUsed = 0;
			while (iter != m_clauses.end())
			{
				c = (*iter);
				pSubScorer = c->getQuery()->scorer(pSearcher,pSubBuffer,nCurBufferSize,&nSubBufUsed);
				if(pSubScorer)
				{
					pScorer->add(pSubScorer,c->isRequired(),c->isProhibited());
					pSubBuffer += nSubBufUsed;
					nCurBufferSize = nSubBufferSize + (nCurBufferSize - nSubBufUsed);
				}
				else 
				{
					if(c->isRequired())
					{
						delete pScorer;
						pScorer = NULL;
						break;
					}
				}

				iter++;
			}
			if(pBufUsed)
			{
				*pBufUsed = pSubBuffer - buffer;
			}
			return pScorer;
		}

		void CBooleanQuery::add(CQuery* query, const bool required, const bool prohibited)
		{
			CBooleanClause* pClause = NULL; 
			try
			{
				CBooleanClause* pClause = new CBooleanClause(query,required,prohibited);
				add(pClause);
			}
			catch (...)
			{		
				if(pClause )
					delete pClause;
			}
			
		}
		void CBooleanQuery::add(CBooleanClause* pClause)
		{
			if((int32_t)m_clauses.size() >= MAX_CLAUSECOUNT)
				FIRTEX_THROW2(PARSE_ERROR,"Too Many Clause.");
			m_clauses.push_back(pClause);
		}
		void CBooleanQuery::clear()
		{
			ClauseIterator iter = m_clauses.begin();
			while (iter != m_clauses.end())
			{
				delete (*iter);
				iter++;
			}
			m_clauses.clear();
		}
		int32_t CBooleanQuery::getQueryLength() const
		{
			int32_t nQueryLen = 0;
			
			ClauseIterator iter = m_clauses.begin();
			CBooleanClause* c;			
			while (iter != m_clauses.end())
			{
				c = (*iter);
				nQueryLen += c->getQuery()->getQueryLength();
				iter++;
			}			
			return nQueryLen;
		}
		
		CTermArray* CBooleanQuery::extractTerms()
		{
			CTermArray* pArray = new CTermArray(false);

			CTermArray* pArray2;
			ClauseIterator iter = m_clauses.begin();
			while (iter != m_clauses.end())
			{
				pArray2 = (*iter)->getQuery()->extractTerms();
				if(pArray2)
				{
					FIRTEX_ASSERT2(pArray2->isDeleteTerms()==false);
					pArray->merge(pArray2);
					delete pArray2;
				}

				iter++;
			}
			return pArray;
		}
	}
}
