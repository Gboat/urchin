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
// Created	: 2006/3/9
//

#include "search/PhraseQuery.h"
#include "search/TermQuery.h"
#include "search/Weight.h"
#include "search/Searcher.h"
#include "search/PhraseScorer.h"
#include <memory>

namespace firtex
{
	namespace search
	{
		//////////////////////////////////////////////////////////////////////////
		//CPhraseWeight	
		CPhraseQuery::CPhraseWeight::CPhraseWeight(CSearcher* pSearcher,CPhraseQuery* pQuery)
			:m_pSearcher(pSearcher)
			,CWeight(pQuery,false)
		{		
			CSimilarity* pSimilariy = m_pSearcher->getSimilarity();			
			m_fWeight = pSimilariy->idf(&(pQuery->m_terms),m_pSearcher) * pQuery->getBoost();
		}
		CPhraseQuery::CPhraseWeight::~CPhraseWeight()
		{			
		}
			
		weight_t  CPhraseQuery::CPhraseWeight::getValue()
		{
			return m_fWeight;
		}		

		//////////////////////////////////////////////////////////////////////////
		//CPhraseQuery
		CPhraseQuery::CPhraseQuery(void)
		{
			m_nSlop = 0;
			m_pTermQuery = NULL;
		}
		CPhraseQuery::CPhraseQuery(const CPhraseQuery& clone)
		{
			vector<CTerm*>::const_iterator iter = clone.m_terms.begin();
			while (iter != clone.m_terms.end())
			{
				m_terms.push_back((*iter)->clone());
				iter++;
			}			
			m_nSlop = clone.m_nSlop;
			if(clone.m_pTermQuery)
				m_pTermQuery = new CTermQuery(*clone.m_pTermQuery);
			else m_pTermQuery = NULL;

		}

		CPhraseQuery::~CPhraseQuery(void)
		{			
			if(m_pTermQuery)
			{
				delete m_pTermQuery;
				m_pTermQuery = NULL;
			}
			TermIterator iter = m_terms.begin();
			while (iter != m_terms.end())
			{
				delete (*iter);
				iter++;
			}
			m_terms.clear();
		}
		void CPhraseQuery::addTerm(CTerm* term)
		{
			m_terms.push_back(term->clone());
		}
		CWeight* CPhraseQuery::createWeight(CSearcher* pSearcher)
		{
			return new CPhraseWeight(pSearcher,this);
		}
		CScorer* CPhraseQuery::scorer(CSearcher* pSearcher)
		{
			if(m_terms.size() == 1)
			{
				if(!m_pTermQuery)
				{
					m_pTermQuery = new CTermQuery(m_terms[0]);
				}
				return m_pTermQuery->scorer(pSearcher);
			}
			else 
			{
				CTermReader* pTermReader = pSearcher->termReader();
				std::auto_ptr<CTermReader> termreader_ptr(pTermReader);
				size_t size = m_terms.size();				
				CTermPositions** pp = new CTermPositions*[size];
				for( size_t i = 0;i<size;i++)
				{
					if(pTermReader->seek(m_terms[i]) == false)
					{
						delete[] pp;
						return NULL;
					}
					pp[i] = pTermReader->termPositions();	
				}
				CScorer* pScorer = new CPhraseScorer(weight(pSearcher),getSimilarity(pSearcher),m_nSlop,pp,(int32_t)size,pSearcher->norms(m_terms[0]->getField()));
				delete[] pp;				
				return pScorer;
			}			
		}
		CScorer* CPhraseQuery::scorer(CSearcher* pSearcher,int32_t* buffer,size_t bufSize,size_t* pBufUsed)
		{
			if(m_terms.size() == 0)
				return NULL;
			else if(m_terms.size() == 1)
			{
				if(!m_pTermQuery)
				{
					m_pTermQuery = new CTermQuery(m_terms[0]);					
				}
				return m_pTermQuery->scorer(pSearcher,buffer,bufSize,pBufUsed);
			}
			else 
			{
				CTermReader* pTermReader = pSearcher->termReader();
				std::auto_ptr<CTermReader> termreader_ptr(pTermReader);				
				size_t nSize = m_terms.size();
				int32_t* pSubBuffer = buffer;
				size_t nSubBufferSize = bufSize/nSize;
				size_t nCurBufferSize = nSubBufferSize;
				size_t nSubBufUsed = 0;
				CTermPositions** pp = new CTermPositions*[nSize];
				for( size_t i = 0;i < nSize;i++)
				{
					if(pTermReader->seek(m_terms[i]) == false)
					{
						for (size_t nPP = 0;nPP < i;nPP++)
						{
							delete pp[nPP];
						}
						delete[] pp;

						return NULL;
					}
					pp[i] = pTermReader->termPositions();					
					nSubBufUsed = pp[i]->setBuffer(pSubBuffer,nCurBufferSize);
					pSubBuffer += nSubBufUsed;
					nCurBufferSize = nSubBufferSize + (nCurBufferSize - nSubBufUsed);					
				}
				CScorer* pScorer = new CPhraseScorer(weight(pSearcher),getSimilarity(pSearcher),m_nSlop,pp,(int32_t)nSize,pSearcher->norms(m_terms[0]->getField()));
				delete[] pp;				
				if(pBufUsed)
				{
					*pBufUsed = pSubBuffer - buffer;
				}
				return pScorer;
			}			
		}

		CQuery* CPhraseQuery::clone()
		{
			return new CPhraseQuery(*this);
		}

		CTermArray* CPhraseQuery::extractTerms()
		{
			CTermArray* pArray = new CTermArray(m_terms.size(),false);
			TermIterator iter = m_terms.begin();
			while (iter != m_terms.end())
			{
				pArray->add(*iter);
				iter++;
			}
			return pArray;
		}
	}
}

