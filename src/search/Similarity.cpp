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
// Created	: 2006/3/10
//
#include "search/Similarity.h"
#include "search/Searcher.h"
#include "search/DefaultSimilarity.h"

namespace firtex
{
	namespace search
	{
		bool CSimilarity::m_bNormTableInited = false;
		float CSimilarity::NORM_TABLE[256];
		CSimilarity* CSimilarity::m_pDefaultSimilarity = NULL;
		CSimilarity::CSimilarity(void)
		{			
			if(m_bNormTableInited == false)
			{
				initNormTable();
				m_bNormTableInited = true;
			}
		}
		CSimilarity::~CSimilarity(void)
		{			
		}
				
		float CSimilarity::idf(std::vector<CTerm*>* terms,CSearcher* searcher)
		{
			std::vector<CTerm*>::iterator iter = terms->begin();
			score_t _idf = 0;
			while (iter != terms->end())
			{
				_idf += idf((CTerm*)(*iter),searcher);
				iter++;
			}
			return _idf;
		}
		float CSimilarity::idf(CTerm* term,CSearcher* searcher)
		{
			return idf(searcher->docFreq(term), searcher->maxDocs());
		}	

		void CSimilarity::initNormTable()
		{
			for (int i = 0; i < 256; i++)
				NORM_TABLE[i] = CSmallFloat::byte315ToFloat((byte)i);		
		}
		
		CSimilarity* CSimilarity::getDefault()
		{
			if(m_pDefaultSimilarity == NULL)
				m_pDefaultSimilarity = new CDefaultSimilarity();
			return m_pDefaultSimilarity;
		}
		void CSimilarity::setDefault(CSimilarity* pDefault)
		{
			if(m_pDefaultSimilarity)
				delete m_pDefaultSimilarity;
			m_pDefaultSimilarity = pDefault;
		}
	}
}
