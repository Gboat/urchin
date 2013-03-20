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
			 * �Բ�ѯ�����Ż�,�����µĲ�ѯ����
			 * @param pOrgQuery ��Ҫ�Ż��Ĳ�ѯ����
			 * @return �Ż�����µĲ�ѯ����,Ĭ������·�������
			 * @memory ���ڲ�����,�ⲿ��Ҫɾ��
			 */
			CQuery*			optimizeQuery(CQuery* pOrgQuery);
			
			/**
			 * ���ؿ��������ĵ���ţ�һ����������û��ɾ���ĵ���maxDoc()=numDocs()
			 * �����������������ĵ�����Ĵ�С
			 */
			count_t			maxDocs(){return m_pIndexReader->maxDoc();}
			
			/**
			 * ����������
			 * @param pIndex ������,���ڲ�ά�����ⲿ����ɾ��
			 */
			void			attachReader(firtex::index::CIndexReader* pIndexReader);

			/**
			 * �ͷ����������ͷź��ⲿ����ά����������CIndexWriter���ٳ��и�������
			 * @return ���������ⲿά��
			 */
			CIndexReader*	detachReader();

			/**
			 * get index reader of searcher
			 */
			CIndexReader*	getReader();

			/**
			 * �����ĵ�docID��stored fields
			 * @param docID �ĵ����
			 * @param �ĵ������ⲿά��
			 */
			document::CDocument*		doc(docid_t docID);
		protected:
			/**
			 * ��ȡָ��Term���ĵ�Ƶ��
			 * @param t Term����
			 * @return �ĵ�Ƶ��
			 */
			freq_t			docFreq(firtex::index::CTerm* t);

			/**
			 * get term reader			 
			 * @return term reader, MUST be deleted by caller.
			 */
			firtex::index::CTermReader*	termReader();

			/**
			 * ��ȡָ�����norms bytes
			 * @param field ����
			 * @return �������norms bytes���ڴ����ڲ�ά��
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
