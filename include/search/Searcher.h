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
			 * �����ĵ�
			 * @param pQuery ��ѯ����
			 * @param nDocs ����nDocs�����
			 * @return ���ز�ѯ��������ڴ����ⲿά��ɾ��
			 */
			virtual CQueryHits*		search(CQuery* pQuery,int32_t nDocs) = 0;

			/**
			 * �Բ�ѯ�����Ż�,�����µĲ�ѯ����
			 * @param pOrgQuery ��Ҫ�Ż��Ĳ�ѯ����
			 * @return �Ż�����µĲ�ѯ����,Ĭ������·�������
			 * @memory ���ڲ�����,�ⲿ��Ҫɾ��
			 */
			virtual CQuery*			optimizeQuery(CQuery* pOrgQuery) = 0;

			/**
			 * ��ȡ������TermReader����
			 * @return ����TermReader�����ڴ����ڲ�ά��
			 */
			virtual	CTermReader*	termReader() = 0;

			/**
			 * ����ָ��Term���ĵ�Ƶ��
			 * @param pTerm Term����
			 * @return �ĵ�Ƶ��
			 */
			virtual freq_t			docFreq(CTerm* pTerm) = 0;

			/**
			 * ��ȡָ�����norms bytes
			 * @param field ����
			 * @return �������norms bytes���ڴ����ڲ�ά��
			 */
			virtual CNormsBytes*	norms(const tchar* field) = 0;

			/**
			 * ���ؿ��������ĵ���ţ�һ����������û��ɾ���ĵ���maxDoc()=numDocs()
			 * �����������������ĵ�����Ĵ�С
			 */
			virtual count_t			maxDocs()=0;

			/**
			 * �����ĵ�docID��stored fields
			 * @param docID �ĵ����
			 * @param �ĵ������ⲿά��
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
