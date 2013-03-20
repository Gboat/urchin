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
// Created	: 2005/12/27
//

#ifndef _INDEXBARRELREADER_H
#define _INDEXBARRELREADER_H

#include "../utility/StdHeader.h"
#include "FieldsInfo.h"
#include "StoredFieldsReader.h"
#include "Term.h"
#include "TermDocFreqs.h"
#include "TermPositions.h"
#include "FieldIndexer.h"
#include "FieldsInfo.h"
#include "../store/Directory.h"
#include "../document/Document.h"
#include "../document/Field.h"
#include "ForwardIndexReader.h"

#include <map>

using namespace std;
using namespace firtex;
using namespace firtex::store;


namespace firtex
{
	namespace index
	{
		class CTermReader;
		class CNormsBytes;
		class CIndex;
		class CIndexBarrelReader
		{
		public:
			CIndexBarrelReader(CIndex* pIndex);
			CIndexBarrelReader();
			virtual ~CIndexBarrelReader(void);
		public:
			/**
			 * �����ĵ�docID��stored fields
			 * @param docID �ĵ����
			 * @return �ĵ��������ⲿά���ڴ�
			 */
			virtual document::CDocument*	document(docid_t docID) = 0;

			/**
			 * ��ȡָ���ĵ����ָ���ֶα�ŵ��ĵ�����
			 * @param docID �ĵ����
			 * @param fid �ֶα��
			 * @return �ֶζ���,�����ֶβ��Ǵ洢�ֶλ򲻴���ʱ����NULL
			 * @memory ���ڲ�����,���ⲿɾ��
			 */
			virtual document::CField*		field(docid_t docID,fieldid_t fid) = 0;

			/**
			 * delete a document
			 * @param docID document ID
			 * @return true if delete success, false if already deleted or some problem
			 */
			virtual bool	deleteDocument(docid_t docID) = 0;

			/** 
			 * �ָ���ǰɾ���������ĵ���ֻ���ڵ���{@link #optimize()}ǰ���ò���Ч
			 */
			virtual void	undeleteAll() = 0;

			/** 
			 * �ж��ĵ����ΪdocID���ĵ��Ƿ��Ѿ�ɾ��
			 * @param docID �ĵ����
			 */
			virtual bool	isDeleted(docid_t docID) = 0;

			/** 
			 * �ж��Ƿ�ɾ�����ĵ�
			 */
			virtual bool	hasDeletions() = 0;

			/**
			 * ɾ���ĵ�����
			 */
			virtual count_t	numDeletedDocs() = 0;

			/**
			 * ��ȡ����Ͱ��TermReader
			 * @param ���ʧ���򷵻�NULL���ڴ����ڲ�ά�����ⲿ����ɾ��
			 */
			virtual CTermReader*	termReader() = 0;

			/**
			 * ��ȡ����Ͱָ���ֶε�TermReader
			 * @param ���ʧ���򷵻�NULL
			 * @memory �ڴ�����ⲿ����ɾ��
			 */
			virtual CTermReader*	termReader(const tchar* field){return NULL;}

			/**
			 * ��ȡǰ�������Ķ���			 
			 * @return ǰ�������Ķ�������
			 * @memory �ڲ�����,�ⲿ����ɾ��
			 */
			virtual CForwardIndexReader*	getForwardIndexReader() = 0;

			/**
			 * ��ȡָ�����norms Bytes
			 * @param field ����
			 */
			virtual CNormsBytes*		norms(const tchar* field) = 0;

			/**
			 * ��ȡָ�����norms Bytes
			 * @param fid ����
			 */
			virtual CNormsBytes*		norms(fieldid_t fid) = 0;

			/**
			 * ��ȡ������
			 */
			virtual uint64_t		numTerms() = 0;

			/**
			 * ��ȡ����Ϣ
			 */
			virtual CFieldsInfo&	getFieldsInfo() = 0;

			/**
			 * �ر�����Ͱ���ͷ���Դ
			 */
			virtual void			close() = 0;		

			/**
			 * ��ȡ�򹤳�
			 */
			CFactoryFinder*		getFactoryFinder();
		protected:			
			CIndex*					m_pIndex;
		};
	}
}

#endif
