//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: ¹ùÈð½Ü(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/2/23
//
#ifndef _DICTHASHTERMREADERIMPL_H
#define _DICTHASHTERMREADERIMPL_H

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
		class CDictHashTermReaderImpl : public CTermReader
		{
		public:
			struct TERM_TABLE
			{			
				termid_t	tid;
				CTermInfo	ti;
			};
		public:
			CDictHashTermReaderImpl(void);
			CDictHashTermReaderImpl(CDirectory* pDirectory,const tchar* barrelname,CFieldInfo* pFieldInfo,CBitVector* pDocFilter);
			virtual ~CDictHashTermReaderImpl(void);
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
			 * get term's document postings,must be called after calling {@link seek()} success			 
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
			CTermReader*	clone(){return NULL;}

		protected:		
			/**
			 * get term information of a term
			 * @return term information
			 */
			CTermInfo*		termInfo(CTerm* term);			
		protected:						
			CInputDescriptor*	m_pInputDescriptor;			

			int64_t				m_nVocLength;
			int32_t				m_nTermCount;
			TERM_TABLE*			m_pTermTable;

			friend class CDictHashTermIterator;
			friend class CDictHashTermReader;
		};	
	}
}

#endif

