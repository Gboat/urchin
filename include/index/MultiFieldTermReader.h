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
// Created	: 2005/11/20
//
#ifndef _MULTIFIELDTERMREADER_H
#define _MULTIFIELDTERMREADER_H

#include "../utility/StdHeader.h"
#include "../utility/BitVector.h"
#include "../store/Directory.h"
#include "FieldsInfo.h"
#include "TermReader.h"
#include "Term.h"
#include <map>
using namespace std;
using namespace firtex::store;

namespace firtex
{
	namespace index
	{
		class CFactoryFinder;
		class CMultiFieldTermReader :	public CTermReader
		{
		public:
			CMultiFieldTermReader(CDirectory* pDirectory,const tchar* barrelname,CFieldsInfo* pFieldsInfo,CBitVector* pDocFilter,CFactoryFinder* pFactories);
			CMultiFieldTermReader(CBitVector* pDocFilter);
			virtual ~CMultiFieldTermReader(void);
		public:
			/**
			 * open a index barrel
			 * @param pDirectory index storage
			 * @param barrelname index barrel name 
			 * @param pFieldInfo field information
			 * @param pDocFilter filter for deleted documents
			 */
			void			open(CDirectory* pDirectory,const tchar* barrelname,CFieldInfo* pFieldsInfo,CBitVector* pDocFilter);
			
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
			 * get term reader of a field
			 * @param field field name
			 * @return term reader, MUST be deleted by caller.
			 */
			CTermReader*	termReader(const tchar* field);

			/**
			 * close term reader			 
			 */
			void			close();

			/**
			 * clone the term reader
			 * @return term reader, MUST be deleted by caller.
			 */
			CTermReader*	clone();

			/** set factory finder */
			void			setFactoryFinder(CFactoryFinder* pFactoryFinder){m_pFieldFactoryFinder = pFactoryFinder;}

			/**
			 * add a term reader
			 * @param field field name
			 * @param pTermReader term reader
			 */
			void			addTermReader(const tchar* field,CTermReader* pTermReader);
		protected:
			/**
			 * get term information of a term
			 * @param term term
			 * @return term information
			 */
			CTermInfo*		termInfo(CTerm* term);
		protected:
			virtual void	open(CDirectory* pDirectory,const tchar* barrelname,CFieldsInfo* pFieldsInfo);
		protected:
			typedef map<tstring,CTermReader*> reader_map;
			
			reader_map			m_fieldsTermReaders;
			CTermReader*		m_pCurReader;

			CFactoryFinder*		m_pFieldFactoryFinder;
		};
	}
}

#endif
