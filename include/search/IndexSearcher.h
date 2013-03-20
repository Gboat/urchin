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
#ifndef _INDEXSEARCHER_H
#define _INDEXSEARCHER_H

#include "Searcher.h"
#include "../index/IndexReader.h"
#include "Filter.h"
#include "Sort.h"


namespace firtex
{
	namespace search
	{
		class CQueryParser;
		class CIndexSearcher :	public CSearcher
		{			
		public:
			CIndexSearcher(void);
			CIndexSearcher(const tchar* indexpath);
			CIndexSearcher(firtex::index::CIndexReader*	pIndexReader);
			virtual ~CIndexSearcher(void);
		public:
			/**
			 * query processing 
			 * @param pQuery query object created by CQueryParser
			 * @param nDocs top-k documents we wanted
			 * @return query hits, MUST be deleted by caller
			 */
			CQueryHits*		search(CQuery* pQuery,int32_t nDocs);

			/**
			 * query processing
			 * @param pQuery query object created by CQueryParser
			 * @param pFilter filter used to remove some documents from hits
			 * @param nDocs top-k documents we wanted
			 * @return query hits, MUST be deleted by caller
			 */
			CQueryHits*		search(CQuery* pQuery,CFilter* pFilter,int32_t nDocs);

			/**
			 * query processing
			 * @param pQuery query object created by CQueryParser
			 * @param pFilter filter which can remove some documents from hits
			 * @param nDocs top-k documents we wanted
			 * @param pSort sorter used to re-sort hits
			 * @return query hits, MUST be deleted by caller
			 */
			CQueryHits*		search(CQuery* pQuery,CFilter* pFilter,CSort* pSort,int32_t nDocs);


			/**
			 * query processing 
			 * @param query query expression
			 * @param field field name, as a default field for searching
			 * @param nDocs top-k documents we wanted
			 * @return query hits, MUST be deleted by caller
			 */
			CQueryHits*		search(const tchar* query,const tchar* field,int32_t nDocs);

			/**
			 * query processing
			 * @param query query expression
			 * @param field field name, as a default field for searching
			 * @param pFilter filter which can remove some documents from hits
			 * @param nDocs top-k documents we wanted
			 * @param pSort sorter used to re-sort hits
			 * @return query hits, MUST be deleted by caller
			 */
			CQueryHits*		search(const tchar* query,const tchar* field,CFilter* pFilter,CSort* pSort,int32_t nDocs);

			/**
			 * get query parser
			 * @param query parser of searcher
			 */
			CQueryParser*	getQueryParser();

			/**
			 * 对查询进行优化,生成新的查询对象
			 * @param pOrgQuery 需要优化的查询对象
			 * @return 优化后的新的查询对象,默认情况下返回自身
			 * @memory 非内部对象,外部需要删除
			 */
			CQuery*			optimizeQuery(CQuery* pOrgQuery);
			
			/**
			 * 返回可能最大的文档编号，一般情况下如果没有删除文档则maxDoc()=numDocs()
			 * 可以用来决定分配文档数组的大小
			 */
			count_t			maxDocs(){return m_pIndexReader->maxDoc();}
			
			/**
			 * 捆绑索引器
			 * @param pIndex 索引器,由内部维护，外部不能删除
			 */
			void			attachReader(firtex::index::CIndexReader* pIndexReader);

			/**
			 * 释放索引器，释放后外部负责维护索引器，CIndexWriter不再持有该索引器
			 * @return 索引器，外部维护
			 */
			CIndexReader*	detachReader();

			/**
			 * get index reader of searcher
			 */
			CIndexReader*	getReader();

			/**
			 * 返回文档docID的stored fields
			 * @param docID 文档编号
			 * @param 文档对象，外部维护
			 */
			document::CDocument*		doc(docid_t docID);
		protected:
			/**
			 * 获取指定Term的文档频率
			 * @param t Term对象
			 * @return 文档频率
			 */
			freq_t			docFreq(firtex::index::CTerm* t);

			/**
			 * get term reader			 
			 * @return term reader, MUST be deleted by caller.
			 */
			firtex::index::CTermReader*	termReader();

			/**
			 * 获取指定域的norms bytes
			 * @param field 域名
			 * @return 返回域的norms bytes，内存由内部维护
			 */
			firtex::index::CNormsBytes*	norms(const tchar* field);
            
		private:
			firtex::index::CIndexReader*	m_pIndexReader;			
			bool			m_bDestroyIndex;	///destroy index object or not

			CQueryParser*	m_pQueryParser;		///query parser

			friend class CQuery;
			friend class CSimilarity;
		};
	}
}


#endif
