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
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2006/6/10
//

#ifndef _DATETIMEINDEXER_H
#define _DATETIMEINDEXER_H

#include "../utility/StdHeader.h"
#include "FieldIndexer.h"
#include "Posting.h"
//#include <hash_map>
#include <map>

using namespace std;
//using namespace stdext;
using namespace firtex;

//#ifndef WIN32
//	namespace __gnu_cxx
//	{
//		template<> struct hash< int64_t  >
//		{
//			size_t operator()( int64_t __x ) const
//			{
//				return __x;
//			}
//		};
//	}
//#endif

namespace firtex
{
	namespace index
	{		
		//typedef hash_map<int64_t,CInMemoryPosting_DL0*> DateTimeMap;
		typedef map<int64_t,CInMemoryPosting_DL0*> DateTimeMap;
		class CDateTimeIndexer :	public CFieldIndexer
		{
			typedef struct _table_array
			{
				int64_t					dt;
				CInMemoryPosting_DL0*	posting;
			}table_array;
		public:
			CDateTimeIndexer(CMemCache* pMemCache);
			virtual ~CDateTimeIndexer(void);
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
			 * DOCUMENT_LEVEL_0 indexing
			 * @param did document id
			 * @param dt date time value			 
			 */
			inline void		docLevel0Indexing(docid_t did,int64_t dt);
		protected:			
			DateTimeMap			m_mapDateTime;
			
			CMemCache*			m_pMemCache;			///memory cache
			fileoffset_t		m_tdiFilePointer;       
		};

		//////////////////////////////////////////////////////////////////////////
		///inline functions
				
		inline void CDateTimeIndexer::docLevel0Indexing(docid_t did,int64_t dt)
		{			
			DateTimeMap::iterator iter = m_mapDateTime.find(dt);
			if(iter == m_mapDateTime.end())
			{
				CInMemoryPosting_DL0* p = new CInMemoryPosting_DL0(m_pMemCache);
				m_mapDateTime.insert(make_pair(dt,p));				
				p->addDocument(did);
			}
			else iter->second->addDocument(did);		
			
		}	
	}
}

#endif
