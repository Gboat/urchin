//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: 郭瑞杰(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/12/28
//
#ifndef _TERMQUERY_H
#define _TERMQUERY_H

#include "Weight.h"
#include "Query.h"
#include "TermScorer.h"
#include "IndexSearcher.h"
#include "../index/Term.h"

using namespace firtex::index;

namespace firtex
{
	namespace search
	{
		class CTermQuery :	public CQuery
		{
			class CTermWeight:public CWeight
			{
			public:
				CTermWeight(CSearcher* pSearcher,CTermQuery* pTermQuery,CTerm* pTerm);				
				virtual ~CTermWeight();
			public:				
				weight_t	getValue();				
			protected:
				CTerm*			m_pTerm;				
				CSearcher*		m_pSearcher;

				weight_t		m_fWeight;
				score_t			m_idf;
			};
		public:			
			CTermQuery(CTerm* t);
			CTermQuery(const CTermQuery& t);
			CTermQuery(void);
			virtual ~CTermQuery(void);
		public:
			const CTerm*	term(){return m_pTerm;}

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

			/**
			 * 克隆新的对象,需子类实现
			 * @memory 非内部对象,外部删除
			 */
			CQuery*		clone();

			static const tchar* getClassName(){return _T("TermQuery");}

			/**
			 * 获取查询名称标识
			 */
			const tchar* getQueryName() const{return getClassName();}

			/** get query length */
			int32_t			getQueryLength() const{return 1;}

			/**
			 * extract terms from query
			 * @return term array, MUST be deleted by the calller
			 */
			CTermArray*		extractTerms();
		protected:
			CWeight*	createWeight(CSearcher* pSearcher);
		protected:
			CTerm*			m_pTerm;
		};
	}
}

#endif
