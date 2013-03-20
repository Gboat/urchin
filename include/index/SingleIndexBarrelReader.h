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
// Created	: 2005/11/27
//
#ifndef _SINGLEINDEXBARRELREADER_H
#define _SINGLEINDEXBARRELREADER_H

#include "IndexBarrelReader.h"
#include "NormsReader.h"
#include "../utility/BitVector.h"
using namespace firtex::utility;

namespace firtex
{
	namespace index
	{
		class CIndex;
		class CBarrelInfo;
		class CSingleIndexBarrelReader : public CIndexBarrelReader
		{
		public:
			CSingleIndexBarrelReader(CIndex* pIndex, CBarrelInfo* pBarrelInfo);
			virtual ~CSingleIndexBarrelReader(void);
		public:
			/**
			 * ��ָ��������Ͱ��
			 * @param pDirectory �洢λ��
			 * @param name ����Ͱ����
			 */
			void			open(const tchar* name);

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
			bool			deleteDocument(docid_t docID);

			/** 
			 * �ָ���ǰɾ���������ĵ���ֻ���ڵ���{@link #optimize()}ǰ���ò���Ч
			 */
			void			undeleteAll();

			/** 
			 * �ж��ĵ����ΪdocID���ĵ��Ƿ��Ѿ�ɾ��
			 * @param docID �ĵ����
			 */
			bool			isDeleted(docid_t docID);

			/** 
			 * �ж��Ƿ�ɾ�����ĵ�
			 */
			bool			hasDeletions();

			/**
			 * ɾ���ĵ�����
			 */
			count_t			numDeletedDocs();

			/**
			 * ��ȡ����Ͱ��TermReader
			 * @param ���ʧ���򷵻�NULL
			 * @memory �ڴ�����ⲿ����ɾ��
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
			 * ��ȡָ���ֶε�norms Bytes
			 * @param field �ֶ���
			 */
			CNormsBytes*		norms(const tchar* field);

			/**
			 * ��ȡָ���ֶε�norms Bytes
			 * @param fid �ֶα��
			 */
			CNormsBytes*		norms(fieldid_t fid);

			/**
			 * ��ȡ�ֶ���Ϣ
			 */
			CFieldsInfo&		getFieldsInfo(){return *m_pFieldsInfo;}

			/**
			 * ��ȡ������
			 */
			uint64_t			numTerms();

			/**
			 * �ر�����Ͱ���ͷ���Դ
			 */
			void				close();
		private:			
			tstring					m_name;
			CFieldsInfo*			m_pFieldsInfo;

			CBarrelInfo*			m_pBarrelInfo;

			CTermReader*			m_pTermReader;
			CStoredFieldsReader*	m_pStoredFieldsReader;
			CNormsReader*			m_pNormsReader;
			CForwardIndexReader*	m_pTermVectorReader;		

			CBitVector*				m_pDeletedDocs;
			bool					m_bDeletedDocsDirty;
		};
	}
}

#endif
