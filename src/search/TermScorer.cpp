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
// Created	: 2005/12/28
//
#include "search/TermScorer.h"
#include "search/Weight.h"
#include "search/Searcher.h"

namespace firtex
{
	namespace search
	{
		CTermScorer::CTermScorer(CWeight* pWeight,CTermDocFreqs* pTermDocs,CSimilarity* pSimilarity,CNormsBytes* norms):CScorer(pSimilarity),m_pNorms(norms)
		{
			m_pWeight = pWeight;
			m_pTermDocs = pTermDocs;

			m_docs = NULL;
			m_freqs = NULL;

			m_scores = NULL;
			m_pointer = 0;
			m_pointerMax = 0;
			m_numDocs = 0;
			m_fWeights = pWeight->getValue();
			for (int i = 0; i < TERMSCORE_CACHESIZE; i++)
				m_scoreCache[i] = pSimilarity->tf(i) * m_fWeights;
		}

		CTermScorer::~CTermScorer(void)
		{
			if(m_pTermDocs)
			{
				delete m_pTermDocs;
				m_pTermDocs = NULL;
			}
			if(m_scores)
			{
				delete[] m_scores;
				m_scores = NULL;
			}

			if(m_pWeight)
			{
				delete m_pWeight;
				m_pWeight = NULL;
			}			
			m_pNorms = NULL;
		}

		count_t	CTermScorer::nextDocs()
		{
			count_t num = m_pTermDocs->next(m_docs,m_freqs);
			if(num > m_pointerMax)
			{
				if(m_scores)
					delete[] m_scores;
				m_scores = new score_t[num];
				m_pointerMax = num;
			}
			m_numDocs = num;
			m_pointer = 0;
			return num;
		}
		count_t	CTermScorer::scores(docid_t*& docs,score_t*& scores)
		{
			freq_t f;
			score_t s;
			score_t* ps = m_scores;
			count_t pointer = m_pointer;
			for (int i = 0;i < m_numDocs;i++)
			{
				f = m_freqs[pointer];
				s = (f < TERMSCORE_CACHESIZE)?m_scoreCache[f]:(getSimilarity()->tf(f)*m_fWeights);
				s = (score_t)s * getSimilarity()->decodeNorm(m_pNorms->getNorm(m_docs[pointer]));
				ps[pointer] = s;
				pointer++;
			}	
			m_pointer = pointer;
			docs = m_docs;
			scores = ps;

			return m_numDocs;
		}

		bool CTermScorer::next()
		{
			bool ret = m_pTermDocs->next();
			if(ret)
			{
				m_pointerMax = 1;				
				m_freq = m_pTermDocs->freq();
				m_doc = m_pTermDocs->doc();
				m_pointer = 0;
				m_numDocs = 1;
			}
			return ret;
		}
		bool CTermScorer::skipTo(docid_t target,docid_t& nearTarget)
		{
			bool ret = m_pTermDocs->skipTo(target,nearTarget);
			if( (ret) || (nearTarget > target) )
			{
				m_pointerMax = 1;				
				m_freq = m_pTermDocs->freq();
				m_doc = m_pTermDocs->doc();
				m_pointer = 0;
				m_numDocs = 1;
			}
			return ret;
		}		
		score_t CTermScorer::score()
		{			
			freq_t f =m_freq;
			float raw = ( (f < TERMSCORE_CACHESIZE)?m_scoreCache[f]:(getSimilarity()->tf(f)*m_fWeights) );			
			return (score_t)raw * getSimilarity()->decodeNorm(m_pNorms->getNorm(m_doc));
		}
		docid_t CTermScorer::doc()
		{
			return m_doc;
		}
	}
}
