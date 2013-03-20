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
