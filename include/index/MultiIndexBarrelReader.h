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
#ifndef _MULTIINDEXBARRELREADER_H
#define _MULTIINDEXBARRELREADER_H

#include "IndexBarrelReader.h"
#include "BarrelsInfo.h"
#include "SingleIndexBarrelReader.h"
#include "../store/Directory.h"
#include "IndexBarrelWriter.h"
using namespace firtex::store;

namespace firtex
{
	namespace index
	{		
		class CBarrelReaderEntry
		{
		public:
			CBarrelReaderEntry(CIndex* pIndex,CBarrelInfo* pBarrelInfo)
			{
				m_pBarrelInfo = pBarrelInfo;
				if(pBarrelInfo->getWriter())
					m_pBarrel = pBarrelInfo->getWriter()->inMemoryReader();
				else 
					m_pBarrel = new CSingleIndexBarrelReader(pIndex,pBarrelInfo);
			}
			CBarrelReaderEntry(CBarrelInfo* pBarrelInfo,CIndexBarrelReader* pIndexBarrelReader)
			{
				m_pBarrelInfo = pBarrelInfo;
				m_pBarrel = pIndexBarrelReader;
			}
			~CBarrelReaderEntry()
			{
				delete m_pBarrel;								
				m_pBarrel = NULL;
				m_pBarrelInfo = NULL;
			}
		public:
			CBarrelInfo*			m_pBarrelInfo;
			CIndexBarrelReader*		m_pBarrel;

			friend class CMultiIndexBarrelReader;
		};
		

		class CMultiForwardIndexReader;
		class CMultiIndexBarrelReader : public CIndexBarrelReader
		{
		public:
			CMultiIndexBarrelReader(CIndex* pIndex,CBarrelsInfo* pBarrelsInfo);
			virtual ~CMultiIndexBarrelReader(void);
		public:
			/**
			 * ��ָ��������Ͱ��
			 * @param pDirectory �洢λ��
			 * @param name ����Ͱ����
			 */
			void				open(const tchar* name);

			/**
			 * �����ĵ�docID��stored fields
			 */
			document::CDocument*	document(docid_t docID);

			/**
			 * ��ȡָ���ĵ����ָ���ֶα�ŵ��ĵ�����
			 * @param docID �ĵ����
			 * @param fid �ֶα��
			 * @return �ֶζ���,�����ֶβ��Ǵ洢�ֶλ򲻴���ʱ����NULL
			 * @memory ���ڲ�����,���ⲿɾ��
			 */
			document::CField*	field(docid_t docID,fieldid_t fid);

			/** 
			 * �ָ���ǰɾ���������ĵ���ֻ���ڵ���{@link #optimize()}ǰ���ò���Ч
			 */
			void				undeleteAll();

			/** 
			 * �ж��ĵ����ΪdocID���ĵ��Ƿ��Ѿ�ɾ��
			 * @param docID �ĵ����
			 */
			bool				isDeleted(docid_t docID);

			/** 
			 * �ж��Ƿ�ɾ�����ĵ�
			 */
			bool				hasDeletions();

			/**
			 * ɾ���ĵ�����
			 */
			count_t				numDeletedDocs();

			/**
			 * delete a document
			 * @param docID document ID
			 * @return true if delete success, false if already deleted or some problem
			 */
			bool				deleteDocument(docid_t docID);

			/**
			 * get term reader			 
			 * @return term reader, MUST be deleted by caller.
			 */
			CTermReader*		termReader();

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
			uint64_t			numTerms();
            
			/**
			 * ��ȡ����Ϣ
			 */
			CFieldsInfo&		getFieldsInfo();

			/**
			 * �ر�����Ͱ���ͷ���Դ
			 */
			void				close();		

			/** �����򹤳� */
			/*void				setFactoryFinder(CFactoryFinder* pFactories);*/

			/** 
			 * �������Ͱ��ȡ��
			 * @param pBarrelInfo ����Ͱ��Ϣ
			 * @param pBarrelReader ��ȡ���������ڲ�ɾ��
			 */
			void				addBarrelReader(CBarrelInfo* pBarrelInfo,CIndexBarrelReader* pBarrelReader);
		public:
			/**
			 * ��ʼ���е���
			 */
			void				startIterator(){m_readersIterator = m_readers.begin();}
			/**
			 * �жϵ����Ƿ����
			 * @reture true ����δ���������Ե���{@link nextEntry(),nextReader()��nextBarrel()}
			 */
			bool				hasNext(){return (m_readersIterator != m_readers.end());}
			CBarrelReaderEntry* nextEntry();
			CIndexBarrelReader* nextReader();
			CBarrelInfo*		nextBarrel();		
		protected:
			void			addReader(CBarrelInfo* pBarrelInfo);
			bool			loadNorms(const tchar* field);
			bool			loadNorms(fieldid_t fid);
		private:			
			CBarrelsInfo*				m_pBarrelsInfo;
			CTermReader*				m_pTermReader;
			CMultiForwardIndexReader*	m_pTermVectorReader;
			vector<CBarrelReaderEntry*> m_readers;
			vector<CBarrelReaderEntry*>::iterator m_readersIterator;

			map<fieldid_t,CNormsBytes*>	m_normsByID;
			map<tstring,CNormsBytes*>	m_normsByName;
		};
		//////////////////////////////////////////////////////////////////////////
		//Inline Functions
		inline CBarrelReaderEntry* CMultiIndexBarrelReader::nextEntry()
		{
			return *m_readersIterator++;
		}
		inline CIndexBarrelReader* CMultiIndexBarrelReader::nextReader()
		{
			CIndexBarrelReader* pReader = (*m_readersIterator)->m_pBarrel;
			m_readersIterator++;
			return pReader;
		}		
		inline CBarrelInfo* CMultiIndexBarrelReader::nextBarrel()
		{
			CBarrelInfo* pBarrelInfo = (*m_readersIterator)->m_pBarrelInfo;
			m_readersIterator++;
			return pBarrelInfo;
		}
	}
}

#endif
