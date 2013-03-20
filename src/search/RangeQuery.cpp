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
// Created	: 2006/7/16
//
#include "search/RangeQuery.h"
#include "search/BooleanQuery.h"
#include "search/TermQuery.h"

#include <memory>

namespace firtex
{
	namespace search
	{
		CRangeQuery::CRangeQuery(CTerm* lowerTerm,CTerm* upperTerm,bool bInclusive)
			:m_bInclusive(bInclusive)
		{
			if(lowerTerm == NULL && upperTerm == NULL)
			{
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("At least one term must be non-null."));
			}
			if (lowerTerm != NULL && upperTerm != NULL && _tcscmp(lowerTerm->getField(),upperTerm->getField()))
			{
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("Both terms must be for the same field"));
			}
			if(lowerTerm)
				m_pLowerTerm = lowerTerm->clone();
			else m_pLowerTerm = NULL;
			if(upperTerm)
				m_pUpperTerm = upperTerm->clone();
			else m_pUpperTerm = NULL;
		}
		CRangeQuery::CRangeQuery(const CRangeQuery& clone)
		{
			if(clone.m_pLowerTerm)
				m_pLowerTerm = clone.m_pLowerTerm->clone();
			else m_pLowerTerm = NULL;
			if(clone.m_pUpperTerm)
				m_pUpperTerm = clone.m_pUpperTerm->clone();
			else m_pUpperTerm = NULL;
			m_bInclusive = clone.m_bInclusive;
		}
		CRangeQuery::~CRangeQuery(void)
		{
			if(m_pLowerTerm)
			{
				delete m_pLowerTerm;
				m_pLowerTerm = NULL;
			}
			if(m_pUpperTerm)
			{
				delete m_pUpperTerm;
				m_pUpperTerm = NULL;
			}
		}
		CQuery* CRangeQuery::clone()
		{
			return new CRangeQuery(*this);
		}
		CQuery* CRangeQuery::optimize(firtex::index::CIndexReader* pReader)
		{
			CTermReader* pTermReader = pReader->termReader();
			std::auto_ptr<firtex::index::CTermReader> termreader_ptr(pTermReader);

			CTermIterator* pTermIterator = pTermReader->termIterator(m_pLowerTerm,m_pUpperTerm);
			std::auto_ptr<firtex::index::CTermIterator> it_ptr(pTermIterator);

			CBooleanQuery* query = new CBooleanQuery();
			std::auto_ptr<CBooleanQuery> query_ptr(query);
			
			CTerm* lastTerm = NULL;
			try 
			{
				bool checkLower = false;
				if (!m_bInclusive)
					checkLower = true;			
				while(pTermIterator->next())
				{
					lastTerm = (CTerm*)pTermIterator->term();
					if (lastTerm != NULL) 
					{			
						CTermQuery* tq = new CTermQuery(lastTerm);
						tq->setBoost(getBoost());
						query->add(tq,false, false);
						
					}
					else 
					{
						break;
					}						
				}				
			}
			catch(...)
			{ 
				throw; 
			}
			if(query->getClauseCount() == 0)
			{
				return NULL;
			}
			return query_ptr.release();
		}
		int32_t CRangeQuery::getQueryLength() const
		{
			return 0;///rang query will translate to boolean query
		}
		CTermArray*	CRangeQuery::extractTerms()
		{
			return NULL;///rang query will translate to boolean query
		}
	}
}
