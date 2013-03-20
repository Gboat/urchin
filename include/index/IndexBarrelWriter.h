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
// Created	: 2005/12/07
//

#ifndef _INDEXBARRELWRITER_H
#define _INDEXBARRELWRITER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../utility/StdHeader.h"
#include "FieldsInfo.h"
#include "Posting.h"
#include "StoredFieldsWriter.h"
#include "StoredFieldsReader.h"
#include "Term.h"
#include "TermDocFreqs.h"
#include "TermPositions.h"
#include "FieldIndexer.h"
#include "../store/Directory.h"
#include "../document/Document.h"
#include "../document/Field.h"
#include "../utility/MemCache.h"
#include "../index/NormsWriter.h"
#include "../index/ForwardIndexWriter.h"

#include <map>

using namespace std;
using namespace firtex;
using namespace firtex::store;

namespace firtex
{
	namespace index
	{		
		class CFactoryFinder;
		class CIndexBarrelReader;
		class CIndexBarrelWriter
		{
		public:
			CIndexBarrelWriter(CDirectory* pDirectory,CMemCache* pMemCache,const tchar* name);
			CIndexBarrelWriter(CDirectory* pDirectory,CMemCache* pMemCache);
			~CIndexBarrelWriter(void);
		public:
			/**
			 * open a index barrel
			 * @param barrelName index barrel name
			 */
			void			open(const tchar* barrelName);

			/** close barrel writer */
			void			close();

			/**
			 * set a new name of the barrel
			 * @param newName new name of the barrel
			 */
			void			rename(const tchar* newName);

			/** set index document schema */
			void			setSchema(CDocumentSchema* pSchema);

			/**
			 * add a analyzed Document
			 * @param pDoc analyzed document
			 */
			void			addDocument(document::CDocument* pDoc);	

			/**
			 * 批量添加Analyzed Document
			 * @param ppDocs 待添加的已经经过分析处理后的Documents
			 * @param numDocs ppDocs数组			 
			 * @return 成功添加的文档数
			 */
			int32_t			addDocuments(document::CDocument** ppDocs,int32_t numDocs);	
			
			/**
			 * determine if the memory cache for indexing is full 
			 * @return true if cache is full otherwise false.
			 */
			bool			cacheFull();
			/**
			 * determine if the memory cache for indexing is empty 
			 * @return true if empty,otherwise false
			 */
			bool			cacheEmpty(){return m_pMemCache->isEmpty();}			

			/** 
			 * set field factory's factory, it will be used to create field indexer
			 */
			void			setFactoryFinder(CFactoryFinder* pFactories);
			
			/**
			 * get current indexing barrel name
			 */
			const tstring&	getBarrelName(){return m_barrelName;}

			/**
			 * 获取指定域的索引器
			 * @param field 字段名
			 * @memory 内部对象,外部不能删除
			 */
			CFieldIndexer*	getFieldIndexer(const tchar* field);

			/**
			 * 获取内存索引的索引桶读取器，内存中的索引数据也看作是一个索引桶，
			 * 该对象用于读取内存中的索引数据
			 * @return 返回索引桶读取器，非内部对象，需外部删除
			 */
			CIndexBarrelReader*	inMemoryReader();

			/**
			 * get field indexer by field id
			 * @param fid field id
			 * @return internal indexer
			 */
			CFieldIndexer*	getFieldIndexer(fieldid_t fid);

			/**	get fields information of the in-memory index barrel */
			CFieldsInfo*	getFieldsInfo(){return m_pFieldsInfo;}

			/** get filter for deleted documents */
			CBitVector*		getDocFilter(){return m_pDocFilter;}

			/** set filter for deleted documents */
			void			setDocFilter(CBitVector* pDocFilter){m_pDocFilter = pDocFilter;}

			/** get stored fields writer */
			CStoredFieldsWriter*	getStoredFieldsWriter(){return m_pStoredFieldsWriter;}

			/** get term vector writer */
			CForwardIndexWriter*		getTermVectorWriter(){return m_pTermVectorWriter;}

			/** get norms writer */
			CNormsWriter*			getNormsWriter(){return m_pNormsWriter;}

		public:
			/** 
			 * get memory cache for indexing
			 * @return memory cache, internal object
			 */
			CMemCache*		getMemCache(){return m_pMemCache;}

			/**
			 * set memory cache for indexing
			 * @param pMemCache memory cache
			 */
			void			setMemCache(CMemCache* pMemCache){m_pMemCache = pMemCache;}
		public:
			/**
			 * write cache to database
			 */
			void			writeCache();

			/**
			 * reset cache content
			 * @param bResetPosting reset postings of indexer or not
			 */
			void			resetCache(bool bResetPosting = false);	

			/**
			 * close files of stored fields, term vector, norms and doc filter
			 */
			void			closeFiles();
		protected:			
			CDirectory*					m_pDirectory;
			tstring						m_barrelName;
			CStoredFieldsWriter*		m_pStoredFieldsWriter;	
			CForwardIndexWriter*			m_pTermVectorWriter;	
			CFieldsInfo*				m_pFieldsInfo;
			CMemCache*					m_pMemCache;			///cache for in-memory indexing
			CBitVector*					m_pDocFilter;			///filter for deleted documents

			CNormsWriter*				m_pNormsWriter;

			CFactoryFinder*			m_pFieldFactories;

			CFieldIndexer**				m_fieldIndexers;
			int32_t						m_numIndexers;

			friend class CInMemoryIndexBarrelReader;
			friend class CIndexWriter;
		};
	}
}

#endif
