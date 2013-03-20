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
			 * 获取/设置查询对象的提升值(boost)
			 */
			float	getBoost(){return m_fBoost;}
			void	setBoost(float fBoost){m_fBoost = fBoost;}

			/**
			 * 获取查询对象的权重计算部件
			 * @param pSearcher 搜索对象指针
			 * @memory 非内部对象,需外部删除
			 */
			CWeight*		weight(CSearcher* pSearcher);
	
			/**
			 * 判断是否是某一查询的实例
			 * @param other 查询名称
			 */
			bool			instanceOf(const tchar* other);

			/** 
			 * 获取Similarity组件
			 * @param pSearcher 搜索组件指针
			 * @memory 内部对象,外部不能删除
			 */
			CSimilarity*	getSimilarity(CSearcher* pSearcher);	
		public:
			/**
			 * 对查询进行优化,生成新的查询对象,子类可重载,默认情况下返回自身
			 * @param pReader 用于访问索引
			 * @return 优化后的新的查询对象,默认情况下返回自身
			 * @memory 非内部对象,外部需要删除
			 */
			virtual CQuery*			optimize(firtex::index::CIndexReader* pReader);
			/**
			 * 创建Query对应的Weight
			 * @return Weight对象指针，该对象一般由外部调用create的对象销废
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
