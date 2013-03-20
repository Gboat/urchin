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
// Created	: 2005/12/27
//
#include "search/Query.h"
#include "search/Weight.h"
#include "utility/Exception.h"
#include "utility/Config.h"
using namespace firtex::utility;


namespace firtex
{
	namespace search
	{
		CQuery::CQuery(void)	
			:m_fBoost(1.0f)
		{			
		}
		CQuery::~CQuery(void)
		{			
		}
		CWeight* CQuery::weight(CSearcher* pSearcher)
		{			
			CWeight* weight = createWeight(pSearcher);			
			return weight;
		}

		bool CQuery::instanceOf(const tchar* other)
		{
			const tchar* t = getQueryName();
			if ( t==other || _tcscmp( t, other )==0 )
				return true;
			else
				return false;
		}
		CSimilarity* CQuery::getSimilarity(CSearcher* pSearcher)
		{
			return pSearcher->getSimilarity();
		}

		CQuery* CQuery::optimize(firtex::index::CIndexReader* pReader)
		{
			return this;
		}
		CWeight* CQuery::createWeight(CSearcher* pSearcher)
		{
			FIRTEX_THROW3(UNSUPPORTED_ERROR,_T("CQuery::createWeight()."));
			return NULL;
		}
		CScorer* CQuery::scorer(CSearcher* pSearcher)
		{
			FIRTEX_THROW3(UNSUPPORTED_ERROR,_T("CQuery::scorer()."));
			return NULL;
		}
		CScorer* CQuery::scorer(CSearcher* pSearcher,int32_t* buffer,size_t bufSize,size_t* pBufUsed)
		{
			FIRTEX_THROW3(UNSUPPORTED_ERROR,_T("CQuery::scorer()."));
			return NULL;
		}		
	}
}
