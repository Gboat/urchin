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
			 * 添加索引字段
			 * @param did 文档内部编号
			 * @param pField 待索引字段对象指针
			 */
			void			addField(docid_t did,CField* pField);

			/**
			 * 批量添加索引字段，以减少函数调用开销，pdid和ppField必须一一对应，即ppField[i]的文档编号为pdid[i]
			 * @param pdid 文档内部编号数组，大小为numFields
			 * @param ppField 待索引字段数组，大小为numFields
			 * @param numFields pdid和ppFields数组大小
			 */
			void			addFields(docid_t* pdid,CField** ppField,int32_t numFields);
			
			/**
			 * write field index data
			 * @param pWriterDesc out descriptor
			 * @return offset of field vocabulary descriptor
			 */
			fileoffset_t	write(COutputDescriptor* pWriterDesc);
			
			/**
			 * 设置/获取索引数据在tdi文件中的偏移
			 */
			void			setFilePointer(fileoffset_t off){m_tdiFilePointer = off;};
			fileoffset_t	getFilePointer(){return m_tdiFilePointer;};

			/**
			 * 获取非重复的索引词个数
			 */
			uint64_t		distinctNumTerms();

			/** 
			 * 创建用于搜索内存中倒排索引的Term Reader
			 * @return Term Reader 对象，需外部删除
			 */
			CTermReader*	termReader();

			/**
			 * reset postings of the indexer
			 */
			void			reset();
		protected:
			/**
			 * WORD_LEVEL indexing
			 * @param did 文档ID
			 * @param tids term id 数组
			 * @param nNumTerms tids数组大小
			 */
			inline void		wordLevelIndexing(docid_t did,termid_t* tids,int32_t nNumTerms);

			/**
			 * DOCUMENT_LEVEL indexing
			 * @param did 文档ID
			 * @param tids term id 数组
			 * @param nNumTerms tids数组大小
			 */
			inline void		docLevelIndexing(docid_t did,termid_t* tids,int32_t nNumTerms);

			/**
			 * DOCUMENT_LEVEL_0 indexing
			 * @param did 文档ID
			 * @param tids term id 数组
			 * @param nNumTerms tids数组大小
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
					continue;//停用词，跳过
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
					continue;//停用词，跳过
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
					continue;//停用词，跳过
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
