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
// Created	: 2006/2/23
//
#ifndef _DICTHASHTERMREADER_H
#define _DICTHASHTERMREADER_H

#include "../utility/StdHeader.h"
#include "../utility/BitVector.h"
#include "../store/IndexInput.h"
#include "../store/Directory.h"
#include "InMemoryTermReader.h"
#include "DictHashIndexer.h"
#include "TermInfo.h"
#include "TermReader.h"

using namespace firtex::store;

namespace firtex
{
	namespace index
	{
		class CDictHashIndexer;
		class CInputDescriptor;
		class CDictHashTermReaderImpl;
		class CDictHashTermReader :	public CTermReader
		{			
		public:
			CDictHashTermReader(void);
			CDictHashTermReader(CDictHashTermReaderImpl* pImpl);
			CDictHashTermReader(CDirectory* pDirectory,const tchar* barrelname,CFieldInfo* pFieldInfo,CBitVector* pDocFilter);
			virtual ~CDictHashTermReader(void);
		public:
			/**
			 * open a index barrel
			 * @param pDirectory index storage
			 * @param barrelname index barrel name 
			 * @param pFieldInfo field information
			 * @param pDocFilter filter for deleted documents
			 */
			void			open(CDirectory* pDirectory,const tchar* barrelname,CFieldInfo* pFieldInfo,CBitVector* pDocFilter);

			/**
			 * get the term iterator
			 * @param pLowerTerm lower bound 
			 * @param pUpperTerm upper bound
			 * @return term iterator, MUST be deleted by caller
			 */
			CTermIterator*	termIterator(CTerm* pLowerTerm,CTerm* pUpperTerm);

			/**
			 * get the term iterator
			 * @param field field name
			 * @return term iterator, MUST be deleted by caller			 
			 */
			CTermIterator*	termIterator(const tchar* field);

			/**
			 * seek a term
			 * @param pTerm term 
			 * @return true if success, otherwise false, if the return value is true then {@link termDocFreqs()} and 
			 * {@link termPositions()}can be called. 
			 */
			bool			seek(CTerm* term);

			/**
			 * get term's document postings,must be called after call seek() success			 
			 * @return return document postings,need to be deleted outside
			 */
			CTermDocFreqs*	termDocFreqs();

			/**
			 * get term's position postings,must be called after call seek() success			 
			 * @return return position postings,need to be deleted outside
			 */
			CTermPositions*	termPositions();

			/**
			 * get document frequency of a term
			 * @return document frequency
			 */
			freq_t			docFreq(CTerm* term);
			
			/**
			 * close term reader			 
			 */
			void			close();

			/**
			 * clone the term reader
			 * @return term reader, MUST be deleted by caller.
			 */
			CTermReader*	clone();
		protected:						
			CDictHashTermReaderImpl*	m_pTermReaderImpl;
			bool						m_bTermReaderImplOwner;
			CTermInfo*					m_pCurTermInfo;				

			friend class CDictHashTermIterator;
		};

		//////////////////////////////////////////////////////////////////////////
		///CDictHashInMemoryTermReader
		class CInMemoryPosting;
		class CDictHashIndexer;
		class CDictHashInMemoryTermReader :	public CInMemoryTermReader
		{		
		public:
			CDictHashInMemoryTermReader(void);
			CDictHashInMemoryTermReader(const tchar* field,CDictHashIndexer* pIndexer,CBitVector* pDocFilter);
			virtual ~CDictHashInMemoryTermReader(void);
		public:
			/**
			 * get the term iterator
			 * @param pLowerTerm lower bound 
			 * @param pUpperTerm upper bound
			 * @return term iterator, MUST be deleted by caller
			 */
			CTermIterator*	termIterator(CTerm* pLowerTerm,CTerm* pUpperTerm);

			/**
			 * get the term iterator
			 * @param field field name
			 * @return term iterator, MUST be deleted by caller			 
			 */
			CTermIterator*	termIterator(const tchar* field);

			/**
			 * seek a term
			 * @param pTerm term 
			 * @return true if success, otherwise false, if the return value is true then {@link termDocFreqs()} and 
			 * {@link termPositions()}can be called. 
			 */
			bool			seek(CTerm* term);

			/**
			 * get term's document postings,must be called after call seek() success			
			 * @return return document postings,need to be deleted outside
			 */
			CTermDocFreqs*	termDocFreqs();

			/**
			 * get term's position postings,must be called after call seek() success			 
			 * @return return position postings,need to be deleted outside
			 */
			CTermPositions*	termPositions();

			/**
			 * 获取指定term的df			 
			 */
			freq_t			docFreq(CTerm* term);
			
			/**
			 * 关闭搜索器，释放资源			 
			 */
			void			close();

			/**
			 * clone the term reader
			 * @return term reader, MUST be deleted by caller.
			 */
			CTermReader*	clone();
		protected:		
			/**
			 * 获取指定term的TermInfo
			 * @return 返回TermInfo，不需要外部删除
			 */
			CTermInfo*		termInfo(CTerm* term);			

			/**
			 * get in-memory posting
			 * @return reference to in-memory posting
			 */
			CInMemoryPosting*	inMemoryPosting();
		protected:			
			tstring				m_sField;
			CDictHashIndexer*	m_pIndexer;			
			CInMemoryPosting*	m_pCurPosting;
			CTermInfo*			m_pCurTermInfo;
			CTermInfo*			m_pTermInfo;

			friend class CDictHashInMemoryTermIterator;
		};
	}
}

#endif

