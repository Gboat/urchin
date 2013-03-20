//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: �����(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/5/9
//
#ifndef _BOOLEANQUERY_H
#define _BOOLEANQUERY_H

#include <vector>
#include "Query.h"
#include "Weight.h"

using namespace std;

namespace firtex
{
	namespace search
	{
		class CBooleanClause;
		typedef vector<CBooleanClause*> ClauseVector;
		typedef ClauseVector::const_iterator ClauseIterator;
		class CBooleanQuery : public CQuery
		{
			typedef vector<CWeight*> WeightVector;
			typedef WeightVector::iterator WeightIterator;
			class CBooleanWeight:public CWeight
			{
			public:
				CBooleanWeight(CSearcher* pSearcher,CBooleanQuery* pQuery);
				virtual ~CBooleanWeight();
			public:
				weight_t getValue();
			protected:
				CSearcher*		m_pSearcher;
				WeightVector	m_weights;
				weight_t		m_fWeight;
			};
		public:
			CBooleanQuery(void);
			CBooleanQuery(const CBooleanQuery& clone);
			virtual ~CBooleanQuery(void);
		public:			
			/**
			 * �Բ�ѯ�����Ż�,�����µĲ�ѯ����,���������,Ĭ������·�������
			 * @param pReader ���ڷ�������
			 * @return �Ż�����µĲ�ѯ����,Ĭ������·�������
			 * @memory ���ڲ�����,�ⲿ��Ҫɾ��
			 */
			CQuery*		optimize(firtex::index::CIndexReader* pReader);
			
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
			 * ��¡�µĶ���,������ʵ��
			 * @memory ���ڲ�����,�ⲿɾ��
			 */
			CQuery*		clone();		

			ClauseVector&	getClauses(){return m_clauses;}

			int32_t		getClauseCount(){return (int32_t)m_clauses.size();}

			/**
			 * ��Ӳ�ѯ
			 * @param required �ò�ѯ�Ƿ����ƥ��
			 * @param prohibited ƥ��ò�ѯ���ĵ������ų�
			 */
			void	add(CQuery* query, const bool required, const bool prohibited);
			void	add(CBooleanClause* pClause);

			void	clear();
		public:
			static int32_t	getMaxClauseCount();
			static void		setMaxClauseCount(int32_t maxCount);
		public:
			const tchar*	getQueryName() const{return getClassName();}

			/** get query length */
			int32_t			getQueryLength() const;

			/**
			 * extract terms from query
			 * @return term array, MUST be deleted by the caller
			 */
			firtex::index::CTermArray*		extractTerms();

		public:
			static const tchar* getClassName(){return _T("BooleanQuery");}
		protected:
			CWeight*	createWeight(CSearcher* pSearcher);
		protected:
			static int32_t	MAX_CLAUSECOUNT;
		protected:
			ClauseVector	m_clauses;
			
			int32_t			m_nMinShouldMatch;

			friend class CBooleanQuery::CBooleanWeight;
		};
	}
}


#endif
