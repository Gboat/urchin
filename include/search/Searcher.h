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
// Created	: 2005/12/25
//
#ifndef __SEARCHER_H
#define __SEARCHER_H

#include "QueryHits.h"
#include "Similarity.h"

#include "../index/TermDocFreqs.h"
#include "../index/TermReader.h"
#include "../index/NormBytes.h"
using namespace firtex::index;
using namespace firtex;

namespace firtex
{
	namespace search
	{			
		class CSearcher
		{
		public:
			CSearcher(void);
			virtual ~CSearcher(void);
		public:
			/**
			 * 搜索文档
			 * @param pQuery 查询对象
			 * @param nDocs 返回nDocs个结果
			 * @return 返回查询结果集，内存由外部维护删除
			 */
			virtual CQueryHits*		search(CQuery* pQuery,int32_t nDocs) = 0;

			/**
			 * 对查询进行优化,生成新的查询对象
			 * @param pOrgQuery 需要优化的查询对象
			 * @return 优化后的新的查询对象,默认情况下返回自身
			 * @memory 非内部对象,外部需要删除
			 */
			virtual CQuery*			optimizeQuery(CQuery* pOrgQuery) = 0;

			/**
			 * 获取索引的TermReader对象
			 * @return 返回TermReader对象，内存由内部维护
			 */
			virtual	CTermReader*	termReader() = 0;

			/**
			 * 返回指定Term的文档频率
			 * @param pTerm Term对象
			 * @return 文档频率
			 */
			virtual freq_t			docFreq(CTerm* pTerm) = 0;

			/**
			 * 获取指定域的norms bytes
			 * @param field 域名
			 * @return 返回域的norms bytes，内存由内部维护
			 */
			virtual CNormsBytes*	norms(const tchar* field) = 0;

			/**
			 * 返回可能最大的文档编号，一般情况下如果没有删除文档则maxDoc()=numDocs()
			 * 可以用来决定分配文档数组的大小
			 */
			virtual count_t			maxDocs()=0;

			/**
			 * 返回文档docID的stored fields
			 * @param docID 文档编号
			 * @param 文档对象，外部维护
			 */
			virtual document::CDocument*		doc(docid_t docID)=0;

			CSimilarity*	getSimilarity()	{return m_pSimilarity;}
			void			setSimilarity(CSimilarity* pSimilarity)	{m_pSimilarity = pSimilarity;}
		public:
			/**
			 * set buffer for query
			 * @param buffer buffer for query
			 * @param length size of buffer			 
			 */
			void		setBuffer(int32_t* buffer,size_t length);
		
			/**
			 * get internal buffer for this query
			 */
			int32_t*	getBuffer();

			/**
			 * get internal buffer size
			 */
			size_t		getBufferSize();
		protected:

			/** create buffer by itself */
			void		createBuffer();
		protected:
			CSimilarity*	m_pSimilarity;

			int32_t*		m_pQueryBuffer;	///buffer for query
			size_t			m_nBufferSize;	///buffer size
			bool			m_bOwnBuffer;	///do we own the buffer?
		};
	}

}

#endif
