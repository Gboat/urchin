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
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2006/11/29
//
#ifndef _INMEMORYINDEXBARRELREADER_H
#define _INMEMORYINDEXBARRELREADER_H

#include "IndexBarrelReader.h"
#include "../utility/BitVector.h"
#include "NormsReader.h"
using namespace firtex::utility;

namespace firtex
{
	namespace index
	{
		class CIndexBarrelWriter;
		class CInMemoryIndexBarrelReader :	public CIndexBarrelReader
		{
		public:
			CInMemoryIndexBarrelReader(CIndexBarrelWriter*	pIndexBarrelWriter);
			virtual ~CInMemoryIndexBarrelReader(void);
		public:
			/**
			 * �����ĵ�docID��stored fields
			 * @param docID �ĵ����
			 * @return �ĵ��������ⲿά���ڴ�
			 */
			document::CDocument*	document(docid_t docID);

			/**
			 * ��ȡָ���ĵ����ָ���ֶα�ŵ��ĵ�����
			 * @param docID �ĵ����
			 * @param fid �ֶα��
			 * @return �ֶζ���,�����ֶβ��Ǵ洢�ֶλ򲻴���ʱ����NULL
			 * @memory ���ڲ�����,���ⲿɾ��
			 */
			document::CField*		field(docid_t docID,fieldid_t fid);

			/**
			 * delete a document
			 * @param docID document ID
			 * @return true if delete success, false if already deleted or some problem
			 */
			bool	deleteDocument(docid_t docID);

			/** 
			 * �ָ���ǰɾ���������ĵ���ֻ���ڵ���{@link #optimize()}ǰ���ò���Ч
			 */
			void	undeleteAll();

			/** 
			 * �ж��ĵ����ΪdocID���ĵ��Ƿ��Ѿ�ɾ��
			 * @param docID �ĵ����
			 */
			bool	isDeleted(docid_t docID);

			/** 
			 * �ж��Ƿ�ɾ�����ĵ�
			 */
			bool	hasDeletions();

			/**
			 * ɾ���ĵ�����
			 */
			count_t	numDeletedDocs();

			/**
			 * ��ȡ����Ͱ��TermReader
			 * @param ���ʧ���򷵻�NULL���ڴ����ڲ�ά�����ⲿ����ɾ��
			 */
			CTermReader*	termReader();

			/**
			 * ��ȡ����Ͱָ���ֶε�TermReader
			 * @param ���ʧ���򷵻�NULL
			 * @memory �ڴ�����ⲿ����ɾ��
			 */
			CTermReader*	termReader(const tchar* field);

			/**
			 * ��ȡǰ�������Ķ���			 
			 * @return ǰ�������Ķ�������
			 * @memory �ڲ�����,�ⲿ����ɾ��
			 */
			CForwardIndexReader*	getForwardIndexReader();

			/**
			 * ��ȡָ�����norms Bytes
			 * @param field ����
			 */
			CNormsBytes*		norms(const tchar* field);

			/**
			 * ��ȡָ�����norms Bytes
			 * @param fid ����
			 */
			CNormsBytes*		norms(fieldid_t fid);

			/**
			 * ��ȡ������
			 */
			uint64_t		numTerms();

			/**
			 * ��ȡ����Ϣ
			 */
			CFieldsInfo&	getFieldsInfo();

			/**
			 * �ر�����Ͱ���ͷ���Դ
			 */
			void			close();

			/** �����򹤳� */
			void			setFactoryFinder(CFactoryFinder* pFactories){};
		protected:
			CIndexBarrelWriter*	m_pIndexBarrelWriter;
			CTermReader*		m_pTermReader;
			CNormsReader*		m_pNormsReader;
			CBitVector*			m_pDeletedDocs;
		};
	}
}

#endif
