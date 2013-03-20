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
// Created	: 2006/7/3
//
#ifndef _WORDFIELDINDEXER_H
#define _WORDFIELDINDEXER_H

#include "../utility/StdHeader.h"
#include "FieldIndexer.h"
#include "Posting.h"
#include "OutputDescriptor.h"
#include "../utility/DynamicArray.h"

using namespace std;

using namespace firtex;

namespace firtex
{
	namespace index
	{	
		typedef CDynamicArray<CInMemoryPosting*,Const_NullValue<CInMemoryPosting*> > DynPostingArray;	
		class CDictHashIndexer :	public CFieldIndexer
		{
		public:
			CDictHashIndexer(CMemCache* pMemCache);
			virtual ~CDictHashIndexer(void);
		public:		
			/**
			 * ��������ֶ�
			 * @param did �ĵ��ڲ����
			 * @param pField �������ֶζ���ָ��
			 */
			void			addField(docid_t did,CField* pField);

			/**
			 * ������������ֶΣ��Լ��ٺ������ÿ�����pdid��ppField����һһ��Ӧ����ppField[i]���ĵ����Ϊpdid[i]
			 * @param pdid �ĵ��ڲ�������飬��СΪnumFields
			 * @param ppField �������ֶ����飬��СΪnumFields
			 * @param numFields pdid��ppFields�����С
			 */
			void			addFields(docid_t* pdid,CField** ppField,int32_t numFields);
			
			/**
			 * write field index data
			 * @param pWriterDesc out descriptor
			 * @return offset of field vocabulary descriptor
			 */
			fileoffset_t	write(COutputDescriptor* pWriterDesc);
			
			/**
			 * ����/��ȡ����������tdi�ļ��е�ƫ��
			 */
			void			setFilePointer(fileoffset_t off){m_tdiFilePointer = off;};
			fileoffset_t	getFilePointer(){return m_tdiFilePointer;};

			/**
			 * ��ȡ���ظ��������ʸ���
			 */
			uint64_t		distinctNumTerms();

			/** 
			 * �������������ڴ��е���������Term Reader
			 * @return Term Reader �������ⲿɾ��
			 */
			CTermReader*	termReader();

			/**
			 * reset postings of the indexer
			 */
			void			reset();
		protected:
			/**
			 * WORD_LEVEL indexing
			 * @param did �ĵ�ID
			 * @param tids term id ����
			 * @param nNumTerms tids�����С
			 */
			inline void		wordLevelIndexing(docid_t did,termid_t* tids,int32_t nNumTerms);

			/**
			 * DOCUMENT_LEVEL indexing
			 * @param did �ĵ�ID
			 * @param tids term id ����
			 * @param nNumTerms tids�����С
			 */
			inline void		docLevelIndexing(docid_t did,termid_t* tids,int32_t nNumTerms);

			/**
			 * DOCUMENT_LEVEL_0 indexing
			 * @param did �ĵ�ID
			 * @param tids term id ����
			 * @param nNumTerms tids�����С
			 */
			inline void		docLevel0Indexing(docid_t did,termid_t* tids,int32_t nNumTerms);
		protected:			
			DynPostingArray		m_array;				///dynamic array for postings
			
			CMemCache*			m_pMemCache;			///memory cache
			fileoffset_t		m_tdiFilePointer;       

			friend class CDictHashTermReader;
			friend class CDictHashTermIterator;
			friend class CDictHashInMemoryTermReader;
			friend class CDictHashInMemoryTermIterator;
		};

		//////////////////////////////////////////////////////////////////////////
		///inline functions

		inline void CDictHashIndexer::wordLevelIndexing(docid_t did,termid_t* tids,int32_t nNumTerms)
		{
			register count_t posInc = 0;			
			CInMemoryPosting_WL* curPosting;

			for (int32_t i = 0; i < nNumTerms; i++ )
			{
				if(tids[i] <= 0 )
				{
					posInc++;
					continue;//ͣ�ôʣ�����
				}					
				curPosting = (CInMemoryPosting_WL*)m_array[tids[i]];
				if(curPosting == NULL)
				{
					curPosting = new CInMemoryPosting_WL(m_pMemCache);
					m_array[tids[i]] = curPosting;									
				}								
				curPosting->addLocation( did, posInc++ );			
			}//end for			
		}
		inline void CDictHashIndexer::docLevelIndexing(docid_t did,termid_t* tids,int32_t nNumTerms)
		{			
			CInMemoryPosting_DL* curPosting;

			for (int32_t i = 0; i < nNumTerms; i++ )
			{
				if(tids[i] <= 0 )
				{				
					continue;//ͣ�ôʣ�����
				}					
				curPosting = (CInMemoryPosting_DL*)m_array[tids[i]];
				if(curPosting == NULL)
				{
					curPosting = new CInMemoryPosting_DL(m_pMemCache);
					m_array[tids[i]] = curPosting;									
				}				
				curPosting->addDocument(did);
			}//end for			
		}
		
		inline void CDictHashIndexer::docLevel0Indexing(docid_t did,termid_t* tids,int32_t nNumTerms)
		{
			CInMemoryPosting_DL0* curPosting;
			for (int32_t i = 0; i < nNumTerms; i++ )
			{
				if(tids[i] <= 0 )
				{				
					continue;//ͣ�ôʣ�����
				}					
				curPosting = (CInMemoryPosting_DL0*)m_array[tids[i]];
				if(curPosting == NULL)
				{
					curPosting = new CInMemoryPosting_DL0(m_pMemCache);
					m_array[tids[i]] = curPosting;									
				}				
				curPosting->addDocument(did);
			}//end for			
		}
	}
}


#endif
