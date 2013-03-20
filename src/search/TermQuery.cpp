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
// Created	: 2006/12/28
//
#include "search/TermQuery.h"
#include <memory>

namespace firtex
{
	namespace search
	{
		//////////////////////////////////////////////////////////////////////////
		//CTermWeight
		CTermQuery::CTermWeight::CTermWeight(CSearcher* pSearcher,CTermQuery* pTermQuery,CTerm* pTerm)
			:CWeight(pTermQuery,false)
		{
			m_pSearcher = pSearcher;			
			m_pTerm = pTerm;
			m_fWeight = pTermQuery->getSimilarity(m_pSearcher)->idf(m_pTerm,m_pSearcher)*pTermQuery->getBoost();
		}
		CTermQuery::CTermWeight::~CTermWeight()
		{
			m_pSearcher = NULL;			
			m_pTerm = NULL;
		}		
		weight_t CTermQuery::CTermWeight::getValue()
		{
			return m_fWeight;
		}		
		//////////////////////////////////////////////////////////////////////////
		//CTermQuery
		CTermQuery::CTermQuery(CTerm* t)
		{
			m_pTerm = t->clone();	
		}
		CTermQuery::CTermQuery(void)
		{
			m_pTerm = NULL;			
		}
		CTermQuery::CTermQuery(const CTermQuery& t)
		{
			m_pTerm = t.m_pTerm->clone();
		}

		CTermQuery::~CTermQuery(void)
		{
			if(m_pTerm)
			{
				delete m_pTerm;
				m_pTerm = NULL;
			}		
		}
		CWeight* CTermQuery::createWeight(CSearcher* pSearcher)
		{
			return new CTermWeight(pSearcher,this,m_pTerm);
		}

		CScorer* CTermQuery::scorer(CSearcher* pSearcher)
		{
			CTermReader* pTermReader = pSearcher->termReader();
			std::auto_ptr<CTermReader> termreader_ptr(pTermReader);
			if(pTermReader->seek(m_pTerm) == false)
				return NULL;			
			CTermDocFreqs* pTermDocs = pTermReader->termDocFreqs();
			CScorer* pScorer = new CTermScorer(weight(pSearcher),pTermDocs,getSimilarity(pSearcher),pSearcher->norms(m_pTerm->getField()));
			return pScorer;
		}
		CScorer* CTermQuery::scorer(CSearcher* pSearcher,int32_t* buffer,size_t bufSize,size_t* pBufUsed)
		{
			CTermReader* pTermReader = pSearcher->termReader();
			std::auto_ptr<CTermReader> termreader_ptr(pTermReader);

			if(pTermReader->seek(m_pTerm) == false)
				return NULL;			
			CTermDocFreqs* pTermDocs = pTermReader->termDocFreqs();
			size_t nBufUsed = pTermDocs->setBuffer(buffer,bufSize);
			if(pBufUsed)
				*pBufUsed = nBufUsed;
			CScorer* pScorer = new CTermScorer(weight(pSearcher),pTermDocs,getSimilarity(pSearcher),pSearcher->norms(m_pTerm->getField()));			
			return pScorer;
		}
		CQuery*	CTermQuery::clone()
		{
			return new CTermQuery(*this);
		}

		CTermArray* CTermQuery::extractTerms()
		{
			CTermArray* pArray = new CTermArray(1,false);
			pArray->add(m_pTerm);
			return pArray;
		}
	}
}

