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

#ifndef _PHRASEQUERY_H
#define _PHRASEQUERY_H

#include "Query.h"
#include "Weight.h"
#include <vector>
using namespace std;

namespace firtex
{
	namespace search
	{
		class CTermQuery;
		typedef vector<CTerm*>::iterator TermIterator;
		class CPhraseQuery :	public CQuery
		{
			class CPhraseWeight: public CWeight
			{
			public:
				CPhraseWeight(CSearcher* pSearcher,CPhraseQuery* pQuery);
				virtual ~CPhraseWeight();
			public:				
				weight_t	getValue();
			protected:				
				CSearcher*		m_pSearcher;
				score_t			m_fWeight;
			};
		public:
			CPhraseQuery(void);
			CPhraseQuery(const CPhraseQuery& clone);
			virtual ~CPhraseQuery(void);
		public:
			void		addTerm(CTerm* term);
			
			/**
			 * create scorer, must implement by sub classes
			 * @param pSearcher index searcher			 
			 * @return the scorer,not internal object,MUST delete it outside
			 */
			CScorer*	scorer(CSearcher* pSearcher);	

			/**
			 * create scorer, must implement by sub classes
			 * @param pSearcher index searcher
			 * @param buffer buffer for the scorer
			 * @param bufSize size of buffer
			 * @param pBufUsed buffer used of the scorer
			 * @return the scorer,not internal object,MUST delete it outside
			 */
			CScorer*	scorer(CSearcher* pSearcher,int32_t* buffer,size_t bufSize,size_t* pBufUsed);

			CQuery*		clone();

			void			setSlop(int32_t nSlop){m_nSlop = nSlop;}
			int32_t			getSlop(){return m_nSlop;}

			const tchar*	getQueryName() const{return getClassName();}

			/** get query length */
			int32_t			getQueryLength() const{return (int32_t)m_terms.size();}

			/**
			 * extract terms from query
			 * @return term array, MUST be deleted by the caller
			 */
			CTermArray*		extractTerms();
		public:
			static const tchar* getClassName(){return _T("PhraseQuery");}
		protected:
			CWeight* createWeight(CSearcher* pSearcher);
		protected:
			vector<CTerm*>	m_terms;
			CTermQuery*		m_pTermQuery;
			int32_t				m_nSlop;

			friend class CPhraseQuery::CPhraseWeight;
		};
	}
}

#endif
