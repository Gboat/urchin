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
// Created	: 2005/12/27
//
#ifndef _QUERY_H
#define _QUERY_H

#include "Scorer.h"
#include "Searcher.h"

namespace firtex
{
	namespace index
	{
		class CTermArray;
	}
}

namespace firtex
{
	namespace search
	{
		class CSimilarity;		
		class CWeight;
		class CQuery
		{
		public:
			CQuery(void);
			virtual ~CQuery(void);
		public:
			/**
			 * ��ȡ/���ò�ѯ���������ֵ(boost)
			 */
			float	getBoost(){return m_fBoost;}
			void	setBoost(float fBoost){m_fBoost = fBoost;}

			/**
			 * ��ȡ��ѯ�����Ȩ�ؼ��㲿��
			 * @param pSearcher ��������ָ��
			 * @memory ���ڲ�����,���ⲿɾ��
			 */
			CWeight*		weight(CSearcher* pSearcher);
	
			/**
			 * �ж��Ƿ���ĳһ��ѯ��ʵ��
			 * @param other ��ѯ����
			 */
			bool			instanceOf(const tchar* other);

			/** 
			 * ��ȡSimilarity���
			 * @param pSearcher �������ָ��
			 * @memory �ڲ�����,�ⲿ����ɾ��
			 */
			CSimilarity*	getSimilarity(CSearcher* pSearcher);	
		public:
			/**
			 * �Բ�ѯ�����Ż�,�����µĲ�ѯ����,���������,Ĭ������·�������
			 * @param pReader ���ڷ�������
			 * @return �Ż�����µĲ�ѯ����,Ĭ������·�������
			 * @memory ���ڲ�����,�ⲿ��Ҫɾ��
			 */
			virtual CQuery*			optimize(firtex::index::CIndexReader* pReader);
			/**
			 * ����Query��Ӧ��Weight
			 * @return Weight����ָ�룬�ö���һ�����ⲿ����create�Ķ�������
			 */
			virtual CWeight*		createWeight(CSearcher* pSearcher);

			/**
			 * create scorer, must implement by sub classes
			 * @param pSearcher index searcher
			 * @return the scorer,not internal object,MUST delete it outside
			 */
			virtual CScorer*		scorer(CSearcher* pSearcher);

			/**
			 * create scorer, must implement by sub classes
			 * @param pSearcher index searcher
			 * @param buffer buffer for the scorer
			 * @param bufSize size of buffer
			 * @param bufUsed buffer used of the scorer
			 * @return the scorer,not internal object,MUST delete it outside
			 */
			virtual CScorer*		scorer(CSearcher* pSearcher,int32_t* buffer,size_t bufSize,size_t* pBufUsed);

			/**
			 * clone a new object,sub class must implement it
			 * @return query object,internal object
			 */
			virtual	CQuery*			clone() = 0;
						
			/**
			 * get identifier of query
			 */
			virtual const tchar*	getQueryName() const = 0;		

			/** 
			 * get query length 
			 * @return length of query
			 */
			virtual int32_t			getQueryLength() const = 0;

			/**
			 * extract terms from query
			 * @return term array, MUST be deleted by the caller
			 */
			virtual	firtex::index::CTermArray*		extractTerms() = 0;
		
		protected:
			float		m_fBoost;		
		};
	}
}

#endif
