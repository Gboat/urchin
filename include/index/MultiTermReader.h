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
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2006/1/4
//
#ifndef _MULTITERMSEARCHER_H
#define _MULTITERMSEARCHER_H

#include "TermReader.h"
#include "BarrelsInfo.h"

#include <map>
#include <vector>
using namespace std;

namespace firtex
{
	namespace index
	{		
		class CReaderCache
		{
		public:
			CReaderCache()
			{
				m_pTermReader = NULL;
				m_next = NULL;
			}
			CReaderCache(CBarrelInfo* barrelInfo,CTermReader* pSe)
			{
				m_barrelInfo = barrelInfo;
				m_pTermReader = pSe;
				m_next = NULL;
			}
			~CReaderCache()
			{
				m_barrelInfo = NULL;
				delete m_pTermReader;
				m_pTermReader = NULL;
				delete m_next;
				m_next = NULL;
			}
		protected:
			CBarrelInfo*	m_barrelInfo;
			CTermReader*	m_pTermReader;
			CReaderCache*	m_next;
			friend class CMultiTermReader;
		};

		class CMultiIndexBarrelReader;
		class CMultiTermReader :  public CTermReader
		{
		public:
			CMultiTermReader(void);
			CMultiTermReader(CMultiIndexBarrelReader* pBarrelReader);
			virtual ~CMultiTermReader(void);
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
			/**
			 * load reader
			 */
			CReaderCache* 	loadReader(const tchar* field);	
			
		protected:
			CMultiIndexBarrelReader*	m_pBarrelReader;
			CReaderCache*				m_pCurReader;

			map<tstring,CReaderCache*>	m_ReaderCache;	//search cache
		};
	}
}

#endif
