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
// Created	: 2006/4/27
// Modified	: 2006/10/11 changed to access index data directly,not by CIndex object.-by GuoRuijie
//

#ifndef _INDEXREADER_H
#define _INDEXREADER_H

#include "../utility/StdHeader.h"
#include "../utility/DateTime.h"
#include "../store/Directory.h"
#include "../document/Document.h"
#include "IndexBarrelReader.h"
#include "TermInfo.h"
#include "BarrelsInfo.h"

using namespace firtex::document;
using namespace firtex::store;
using namespace std;

namespace firtex
{
	namespace index
	{
		class CNormsBytes;
		class CIndex;
		class CIndexReader
		{
		protected:
			CIndexReader();
			CIndexReader(CIndex* pIndex);
		public:
			virtual ~CIndexReader(void);
		public:			
			/**
			 * ��ȡ�����İ汾��
			 * @return �汾���ַ�������ʽ������.�汾��
			 */
			tstring	getVersion();

			/** 
			 * �ж��ĵ����ΪdocID���ĵ��Ƿ�
			 * @param docID �ĵ����
			 */
			bool	isDeleted(docid_t docID);

			/** 
			 * �ж��Ƿ����ĵ�ɾ����
			 */
			bool	hasDeletions();

			/**
			 * ɾ���ĵ�����
			 */
			count_t numDeletedDocs();

			/**
			 * ɾ�������ĵ�
			 * @param docID �ĵ����
			 */
			bool	deleteDocument(docid_t docID);

			/** 
			 * �ָ���ǰɾ���������ĵ���ֻ���ڵ���{@link #optimize()}ǰ���ò���Ч
			 */
			void	undeleteAll();

			/**
			 * �ر������ͷ���Դ
			 */
			void	close();
			
			/**
			 * �����а������ĵ�����
			 */
			count_t	numDocs();

			/**
			 * ���ؿ��������ĵ���ţ�һ����������û��ɾ���ĵ���maxDoc()=numDocs()
			 * �����������������ĵ�����Ĵ�С
			 */
			count_t	maxDoc();			

			/**
			 * get total number of terms have been indexed
			 * @return number of terms
			 */
			uint64_t	getNumTerms();
			/**
			 * �����ĵ�docID��stored fields
			 * @param docID �ĵ����
			 * @memory ���ڲ�����,���ⲿɾ��
			 */
			document::CDocument* document(docid_t docID);

			/**
			 * ��ȡָ���ĵ����ָ���ֶα�ŵ��ĵ�����
			 * @param docID �ĵ����
			 * @param fid �ֶα��
			 * @return �ֶζ���,�����ֶβ��Ǵ洢�ֶλ򲻴���ʱ����NULL
			 * @memory ���ڲ�����,���ⲿɾ��
			 */
			document::CField*		field(docid_t docID,fieldid_t fid);

			/**
			 * ��ȡָ��Term���ĵ�Ƶ��
			 * @return �����ĵ�Ƶ��
			 */
			freq_t			docFreq(CTerm* term);

			/**
			 * ��ȡָ��Term��Term Info
			 * @return ����Term Info
			 */
			CTermInfo*		termInfo(CTerm* term);		

			/**
			 * ��ȡָ�����norm bytes
			 * @param fieldID ����
			 * @return norms bytes
			 * @memory �ڲ�����,�ⲿ����ɾ��
			 */
			CNormsBytes*		norms(fieldid_t fieldID);

			/**
			 * ��ȡָ�����norm bytes
			 * @param field ����
			 * @return norms bytes
			 * @memory �ڲ�����,�ⲿ����ɾ��
			 */
			CNormsBytes*		norms(const tchar* field);

			/**
			 * ��ȡ����Ͱ��Ϣ
			 */
			CBarrelsInfo*	getBarrelsInfo();			

			/**
			 * ��ȡ�ֶ���Ϣ
			 * @return �ֶ���Ϣ,�ڲ�����
			 */
			CFieldsInfo&	getFieldsInfo();
			
			/**
			 * ��ȡ�ڲ���������
			 * @return �ڲ�����������
			 * @memory �ڲ�����,�ⲿ����ɾ��
			 */
			CIndex*			getIndex(){return m_pIndex;}

			/**
			 * get term reader			 
			 * @return term reader, MUST be deleted by caller.
			 */
			CTermReader*	termReader();

			/**
			 * ��ȡTerm Vector �Ķ���
			 * @return �Ķ�������ָ��
			 * @memory �ڲ�����,�ⲿ����ɾ��
			 */
			CForwardIndexReader*	getForwardIndexReader();

			/**
			 * ��ȡ����������޸�ʱ��
			 * @return 64λ����,������localtime()��ȡ struct tm��ʽ��ʾ��ʱ��
			 */
			int64_t			lastModified();

			/**
			 * ��ȡ����������޸�ʱ��
			 * @param dt CDateTime����,���ڷ���ֵ
			 */
			void			lastModified(CDateTime& dt);

		public:
			/**
			 * ��ȡ����������޸�ʱ��
			 * @param pDirectory ����λ��
			 */
			static int64_t	lastModified(CDirectory* pDirectory);

			/**
			 * ��ȡ����������޸�ʱ��
			 * @param dt CDateTime����,���ڷ���ֵ
			 */
			static void		lastModified(CDirectory* pDirectory,CDateTime& dt);
		protected:
			/**
			 * create barrel reader
			 */			
			void	createBarrelReader();

			/**
			 * get index barrel reader 
			 * @return reference to index barrel reader
			 */
			CIndexBarrelReader* getBarrelReader();

			/**
			 * get internal term reader
			 * @return term reader, internal object
			 */
			CTermReader*		getTermReader();
		protected:
			CIndex*				m_pIndex;				///reference to index object
			CBarrelsInfo*		m_pBarrelsInfo;			///reference to CIndex's m_pBarrelsInfo
			CTermReader*		m_pTermReader;			///term reader
			CIndexBarrelReader*	m_pBarrelReader;		///barrel reader

			friend class CIndex;
		};	
		//////////////////////////////////////////////////////////////////////////
		//inline functions
		inline document::CDocument* CIndexReader::document(docid_t docID)
		{
			if(m_pBarrelReader == NULL)
			{
				createBarrelReader();
			}
			return m_pBarrelReader->document(docID);
		}
		inline document::CField* CIndexReader::field(docid_t docID,fieldid_t fid)
		{
			if(m_pBarrelReader == NULL)
			{
				createBarrelReader();
			}
			return m_pBarrelReader->field(docID,fid);
		}
		inline CBarrelsInfo* CIndexReader::getBarrelsInfo()
		{
			return m_pBarrelsInfo;
		}
	}
}

#endif
